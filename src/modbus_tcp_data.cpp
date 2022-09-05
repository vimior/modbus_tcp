/*
 * Software License Agreement (MIT License)
 *
 * Copyright (c) 2022, Vinman, Inc.
 * All rights reserved.
 *
 * Author: Vinman <vinman.cub@gmail.com>
 */

#include <string.h>
#include "modbus_tcp_data.h"

namespace ModbusTCP
{
  /************************* DataFrame ***************************/

  DataFrame::DataFrame(int buf_size) : buf_size_(buf_size)
  {
    if (buf_size_ < 12) buf_size_ = 12;
    raw_data = new unsigned char[buf_size_];
    pdu_data = raw_data + 7;
    data_length = 0;
  }
  
  DataFrame::~DataFrame()
  {
    if (raw_data != NULL) {
      delete[] raw_data;
      raw_data = NULL;
      pdu_data = NULL;
    }
  }

  void DataFrame::set_raw_data(unsigned char *data, int length)
  {
    if (buf_size_ < length) {
      resize_pdu_buf(length - 7);
    }
    memcpy(raw_data, data, length);
    data_length = length;
  }

  void DataFrame::add_pdu_data(void *data, int length)
  {
    if (buf_size_ < data_length + length) {
      resize_pdu_buf(data_length + length - 7);
    }
    memcpy(pdu_data + data_length - 7, data, length);
    data_length += length;
  }

  void DataFrame::resize_pdu_buf(int pdu_size)
  {
    if (pdu_size <= buf_size_ - 7) return;
    unsigned char *old = raw_data;
    buf_size_ = pdu_size + 7;
    raw_data = new unsigned char[buf_size_];
    pdu_data = raw_data + 7;
    memcpy(raw_data, old, data_length);
    if (old != NULL) {
      delete[] old;
    }
  }
  
  void DataFrame::update_mbap_length(void)
  {
    HexData::bin16_to_8(data_length - 6, raw_data + 4);
  }

  void DataFrame::set_code(unsigned char code)
  {
    if (code == EXP_NONE) return;
    printf("func_code=%d, code=%d\n", pdu_data[0], code);
    memset(pdu_data, 0, buf_size_ - 7);
    pdu_data[0] = pdu_data[0] + 0x80;
    pdu_data[1] = code;
    data_length = 7 + 2;
  }

  /************************* DataSession ***************************/

  DataSession::DataSession(int req_buf_size, int res_buf_size)
  {
    request = new DataFrame(req_buf_size);
    response = new DataFrame(res_buf_size);
  }

  DataSession::~DataSession()
  {
    if (request != NULL) {
      delete request;
      request = NULL;
    }
    if (response != NULL) {
      delete response;
      response = NULL;
    }
  }
  
  void DataSession::set_request_data(unsigned char *data, int length)
  {
    request->set_raw_data(data, length);
  }

  unsigned char* DataSession::get_response_data(void)
  {
    return response->raw_data;
  }
  int DataSession::get_response_length(void)
  {
    return response->data_length;
  }

  /************************* DataService ***************************/

  template <class ModbusData>
  void DataService<ModbusData>::process_session(DataSession *session, ModbusData *modbus_data)
  {
    session->response->set_raw_data(session->request->raw_data, 8);
    int code = EXP_NONE;
    switch (session->request->pdu_data[0]) {
      case MODBUS_FC_READ_COILS:  // 0x01
      case MODBUS_FC_READ_DISCRETE_INPUTS: // 0x02
        code = _read_bits(session, modbus_data);
        break;
      case MODBUS_FC_READ_HOLDING_REGS:  // 0x03
      case MODBUS_FC_READ_INPUT_REGS:    // 0x04
        code = _read_registers(session, modbus_data);
        break;
      case MODBUS_FC_WRITE_SINGLE_COIL: // 0x05
        code = _write_single_coil_bit(session, modbus_data);
        break;
      case MODBUS_FC_WRITE_SINGLE_REG: // 0x06
        code = _write_single_holding_register(session, modbus_data);
        break;
      case MODBUS_FC_WRITE_MULTIPLE_COILS:  // 0x0F
        code = _write_multiple_coil_bits(session, modbus_data);
        break;
      case MODBUS_FC_WRITE_MULTIPLE_REGS:  // 0x10
        code = _write_multiple_holding_registers(session, modbus_data);
        break;
      case MODBUS_FC_MASK_WRITE_REG: // 0x16
        code = _mask_write_holding_register(session, modbus_data);
        break;
      case MODBUS_FC_WRITE_AND_READ_REGS: // 0x17
        code = _write_and_read_multiple_holding_registers(session, modbus_data);
        break;
      
      default:
        code = EXP_ILLEGAL_FUNCTION;
        break;
    }
    session->response->set_code(code);
    session->response->update_mbap_length();
  }

  /* 0x01/0x02 */
  template <class ModbusData>
  int DataService<ModbusData>::_read_bits(DataSession *session, ModbusData *modbus_data)
  {
    int start_addr = HexData::bin8_to_u16(session->request->pdu_data + 1);
    int quantity = HexData::bin8_to_u16(session->request->pdu_data + 3);
    int code = EXP_ILLEGAL_DATA_VALUE;
    if (quantity >= 0x0001 && quantity <= 0x07D0) {
      unsigned char *bits = new unsigned char[quantity];
      if (session->request->pdu_data[0] == MODBUS_FC_READ_COILS) {
        code = modbus_data->read_coil_bits(start_addr, quantity, bits);
      }
      else {
        code = modbus_data->read_input_bits(start_addr, quantity, bits);
      }
      if (code == EXP_NONE) {
        int byte_size = (quantity + 7) / 8;
        session->response->resize_pdu_buf(byte_size + 2);
        unsigned char *data = new unsigned char[byte_size]{0};
        for (int i = 0; i < quantity; i++) {
          if (bits[i]) {
            data[i / 8] = data[i / 8] | (1 << (i % 8));
          }
        }
        session->response->add_pdu_data(&byte_size, 1);
        session->response->add_pdu_data(data, byte_size);
        delete[] data;
      }
      delete[] bits;
    }
    return code;
  }

  /* 0x03/0x04 */
  template <class ModbusData>
  int DataService<ModbusData>::_read_registers(DataSession *session, ModbusData *modbus_data)
  {
    int start_addr = HexData::bin8_to_u16(session->request->pdu_data + 1);
    int quantity = HexData::bin8_to_u16(session->request->pdu_data + 3);
    int code = EXP_ILLEGAL_DATA_VALUE;
    if (quantity >= 0x0001 && quantity <= 0x007D) {
      unsigned short *regs = new unsigned short[quantity];
      if (session->request->pdu_data[0] == MODBUS_FC_READ_HOLDING_REGS) {
        code = modbus_data->read_holding_registers(start_addr, quantity, regs);
      }
      else {
        code = modbus_data->read_input_registers(start_addr, quantity, regs);
      }
      if (code == EXP_NONE) {
        unsigned char byte_size = quantity * 2;
        session->response->resize_pdu_buf(byte_size + 2);
        session->response->add_pdu_data(&byte_size, 1);
        unsigned char tmp[2];
        for (int i = 0; i < quantity; i++) {
          HexData::bin16_to_8(regs[i], tmp);
          session->response->add_pdu_data(tmp, 2);
        }
      }
      delete[] regs;
    }
    return code;
  }

  /* 0x05 */
  template <class ModbusData>
  int DataService<ModbusData>::_write_single_coil_bit(DataSession *session, ModbusData *modbus_data)
  {
    int bit_addr = HexData::bin8_to_u16(session->request->pdu_data + 1);
    int bit_val = HexData::bin8_to_u16(session->request->pdu_data + 3);
    int code = EXP_ILLEGAL_DATA_VALUE;
    if (bit_val == 0x0000 || bit_val == 0xFF00) {
      unsigned char bits[1] = {bit_val == 0xFF00};
      code = modbus_data->write_coil_bits(bit_addr, bits, 1);
      if (code == EXP_NONE) {
        session->response->add_pdu_data(&session->request->pdu_data[1], 4);
      }
    }
    return code;
  }

  /* 0x06 */
  template <class ModbusData>
  int DataService<ModbusData>::_write_single_holding_register(DataSession *session, ModbusData *modbus_data)
  {
    int reg_addr = HexData::bin8_to_u16(session->request->pdu_data + 1);
    unsigned short reg_val = HexData::bin8_to_u16(session->request->pdu_data + 3);
    unsigned short regs[1] = {reg_val};
    int code = modbus_data->write_holding_registers(reg_addr, regs, 1);
    if (code == EXP_NONE) {
      session->response->add_pdu_data(&session->request->pdu_data[1], 4);
    }
    return code;
  }

  /* 0x0F */
  template <class ModbusData>
  int DataService<ModbusData>::_write_multiple_coil_bits(DataSession *session, ModbusData *modbus_data)
  {
    int start_addr = HexData::bin8_to_u16(session->request->pdu_data + 1);
    int quantity = HexData::bin8_to_u16(session->request->pdu_data + 3);
    int byte_count = session->request->pdu_data[5];
    bool quantity_ok = quantity >= 0x0001 && quantity <= 0x07B0;
    bool byte_count_ok = byte_count >= (quantity + 7) / 8;
    bool pdu_len_ok = (session->request->data_length - 7 - 6) >= byte_count;
    int code = EXP_ILLEGAL_DATA_VALUE;
    if (quantity_ok && byte_count_ok && pdu_len_ok) {
      unsigned char *bits = new unsigned char[quantity] { 0 };
      for (int i = 0; i < quantity; i++) {
        unsigned char bit_val = session->request->pdu_data[i / 8 + 6];
        bits[i] = (bool)(bit_val & (1 << (i % 8)));
      }
      code = modbus_data->write_coil_bits(start_addr, bits, quantity);
      delete[] bits;
      if (code == EXP_NONE) {
        session->response->add_pdu_data(&session->request->pdu_data[1], 4);
      }
    }
    return code;
  }

  /* 0x10 */
  template <class ModbusData>
  int DataService<ModbusData>::_write_multiple_holding_registers(DataSession *session, ModbusData *modbus_data)
  {
    int start_addr = HexData::bin8_to_u16(session->request->pdu_data + 1);
    int quantity = HexData::bin8_to_u16(session->request->pdu_data + 3);
    int byte_count = session->request->pdu_data[5];
    bool quantity_ok = quantity >= 0x0001 && quantity <= 0x007B;
    bool byte_count_ok = byte_count == quantity * 2;
    bool pdu_len_ok = (session->request->data_length - 7 - 6) >= byte_count;
    int code = EXP_ILLEGAL_DATA_VALUE;
    if (quantity_ok && byte_count_ok && pdu_len_ok) {
      unsigned short *regs = new unsigned short[quantity] { 0 };
      for (int i = 0; i < quantity; i++) {
        regs[i] = HexData::bin8_to_u16(session->request->pdu_data + i * 2 + 6);
      }
      code = modbus_data->write_holding_registers(start_addr, regs, quantity);
      delete[] regs;
      if (code == EXP_NONE) {
        session->response->add_pdu_data(&session->request->pdu_data[1], 4);
      }
    }
    return code;
  }

  /* 0x16 */
  template <class ModbusData>
  int DataService<ModbusData>::_mask_write_holding_register(DataSession *session, ModbusData *modbus_data)
  {
    int ref_addr = HexData::bin8_to_u16(session->request->pdu_data + 1);
    unsigned short and_mask = HexData::bin8_to_u16(session->request->pdu_data + 3);
    unsigned short or_mask = HexData::bin8_to_u16(session->request->pdu_data + 5);
    int code = modbus_data->mask_write_holding_register(ref_addr, and_mask, or_mask);
    if (code == EXP_NONE) {
      session->response->add_pdu_data(&session->request->pdu_data[1], 6);
    }
    return code;
  }

  /* 0x17 */
  template <class ModbusData>
  int DataService<ModbusData>::_write_and_read_multiple_holding_registers(DataSession *session, ModbusData *modbus_data)
  {
    int r_start_addr = HexData::bin8_to_u16(session->request->pdu_data + 1);
    int r_quantity = HexData::bin8_to_u16(session->request->pdu_data + 3);
    int w_start_addr = HexData::bin8_to_u16(session->request->pdu_data + 5);
    int w_quantity = HexData::bin8_to_u16(session->request->pdu_data + 7);
    int byte_count = session->request->pdu_data[9];
    bool r_quantity_ok = r_quantity >= 0x0001 && r_quantity <= 0x007D;
    bool w_quantity_ok = w_quantity >= 0x0001 && w_quantity <= 0x0079;
    bool byte_count_ok = byte_count == w_quantity * 2;
    bool pdu_len_ok = (session->request->data_length - 7 - 10) >= byte_count;
    int code = EXP_ILLEGAL_DATA_VALUE;
    if (r_quantity_ok && w_quantity_ok && byte_count_ok && pdu_len_ok) {
      unsigned short *r_regs = new unsigned short[r_quantity];
      unsigned short *w_regs = new unsigned short[w_quantity];
      for (int i = 0; i < w_quantity; i++) {
        w_regs[i] = HexData::bin8_to_u16(session->request->pdu_data + i * 2 + 10);
      }
      code = modbus_data->write_and_read_holding_registers(w_start_addr, w_regs, w_quantity, r_start_addr, r_quantity, r_regs);
      if (code == EXP_NONE) {
        unsigned char byte_size = r_quantity * 2;
        session->response->resize_pdu_buf(byte_size + 2);
        session->response->add_pdu_data(&byte_size, 1);
        unsigned char tmp[2];
        for (int i = 0; i < r_quantity; i++) {
          HexData::bin16_to_8(r_regs[i], tmp);
          session->response->add_pdu_data(tmp, 2);
        }
      }
      delete[] r_regs;
      delete[] w_regs;
    }
    return code;
  }
  /* 模板类需要特化 */
  template class DataService<ModbusBaseData>;
  template class DataService<ModbusStructData>;
} // namespace ModbusTCP

