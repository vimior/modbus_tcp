/*
 * Software License Agreement (MIT License)
 *
 * Copyright (c) 2022, Vinman, Inc.
 * All rights reserved.
 *
 * Author: Vinman <vinman.cub@gmail.com>
 */

#ifndef _MODBUS_TCP_H_
#define _MODBUS_TCP_H_

#include "modbus_data.h"

namespace ModbusTCP
{
  enum MODBUS_TCP_EXP_CODE {
    EXP_NONE = MODBUS_NONE,
    EXP_ILLEGAL_FUNCTION = 0x01,
    EXP_ILLEGAL_DATA_ADDRESS = MODBUS_DATA_ILLEGAL_ADDR, // 0x02
    EXP_ILLEGAL_DATA_VALUE = 0x03,
    EXP_SLAVE_DEVICE_FAILURE = 0x04,
    EXP_ACKNOWLEDGE = 0x05,
    EXP_SLAVE_DEVICE_BUSY = 0x06,
    EXP_MEMORY_PRAITY_ERROR = 0x08,
    EXP_GATEWAY_PATH_UNAVAILABLE = 0x0A,
    EXP_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND = 0x0B
  };

  class HexData
  {
  public:
    static void bin16_to_8(int a, unsigned char *b) {
      unsigned short temp = a;
      b[0] = (unsigned char)(temp >> 8);
      b[1] = (unsigned char)temp;
    }

    static unsigned short bin8_to_u16(unsigned char *a) {
      unsigned short tmp = (a[0] << 8) + a[1];
      return tmp;
    }
  };

  class DataFrame
  {
  public:
    DataFrame(int buf_size = 12);
    ~DataFrame();

    void set_raw_data(unsigned char *data, int length);
    void add_pdu_data(void *data, int length);
    void resize_pdu_buf(int pdu_size);
    void update_mbap_length(void);
    void set_code(unsigned char code);

  public:
    int data_length; // 已经存放的数据长度
    unsigned char *raw_data; // 完整数据, MBAP + PDU
    unsigned char *pdu_data; // PDU数据
  private:
    int buf_size_; // 预分配的缓冲区大小
  };

  class DataSession
  {
    template <class ModbusData>
    friend class DataService; // 把DataService设为友元类，可以访问到该类的私有变量
  public:
    DataSession(int req_buf_size = 12, int res_buf_size = 12);
    ~DataSession();

    void set_request_data(unsigned char *data, int length);
    unsigned char* get_response_data(void);
    int get_response_length(void);
  
  private:
    DataFrame *request;
    DataFrame *response;
  };

  template <class ModbusData>
  class DataService
  {
  public:
    DataService(ModbusData *modbus_data);
    ~DataService();
    
    /* process_data: 处理接收到的数据
     * @param data: 接收到的数据
     * @param length: 数据长度
     * @param callback: 每处理一帧完整的Modbus TCP数据的回调，参数是指向session的指针
     * @param is_checked: 是否是完整的一帧Modbus TCP请求数据，如果为false，函数内部会做处理(检查、拆包等)
     * :return: 返回
     *    -1: 表示缓存的数据长度不够一帧完整的Modbus TCP数据
     *     0: 
     */
    int process_data(unsigned char *data, int length, void(*callback)(DataSession *), bool is_checked = false);
    static void process_session(DataSession *session, ModbusData *modbus_data);
  private:
    // 0x01/0x02
    static int _read_bits(DataSession *session, ModbusData *modbus_data);
    // 0x03/0x04
    static int _read_registers(DataSession *session, ModbusData *modbus_data);
    // 0x05
    static int _write_single_coil_bit(DataSession *session, ModbusData *modbus_data);
    // 0x06
    static int _write_single_holding_register(DataSession *session, ModbusData *modbus_data);
    // 0x0F
    static int _write_multiple_coil_bits(DataSession *session, ModbusData *modbus_data);
    // 0x10
    static int _write_multiple_holding_registers(DataSession *session, ModbusData *modbus_data);
    // 0x16
    static int _mask_write_holding_register(DataSession *session, ModbusData *modbus_data);
    // 0x17
    static int _write_and_read_multiple_holding_registers(DataSession *session, ModbusData *modbus_data);
  
  private:
    int data_length_;     // 缓冲区内的数据长度
    unsigned char *buf_;  // 缓冲区
    ModbusData *modbus_data_; // 寄存器操作实例
    DataSession *session_;
  };
}

#endif // _MODBUS_TCP_H_