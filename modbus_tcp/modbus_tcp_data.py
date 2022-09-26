#!/usr/bin/env python3
# Software License Agreement (BSD License)
#
# Copyright (c) 2022, Vinman, Inc.
# All rights reserved.
#
# Author: Vinman <vinman.cub@gmail.com>

import struct
from .modbus_code import ModbusCode, ModbusFunCode
from .modbus_data import ModbusData
from .modbus_exception import ModbusException


class __CData(object):
    __fmt__ = '>B'
    __length__  = 1
    def __init__(self, data=0):
        self.__val = 0
        self.__raw_data = b''
        self.set_data(data)
    
    def set_value(self, val):
        self.__val = val
        self.__raw_data = struct.pack(self.__fmt__, val)
    
    def set_raw(self, raw):
        self.__raw_data = raw[:self.__length__]
        self.__val = struct.unpack(self.__fmt__, self.__raw_data)[0]
    
    def set_data(self, data):
        if isinstance(data, bytes):
            self.set_raw(data)
        else:
            self.set_value(data)

    @property
    def value(self):
        return self.__val
    
    @property
    def raw_data(self):
        return self.__raw_data


class UnsignedChar(__CData):
    __fmt__ = '>B'
    __length__  = 1


class UnsignedShort(__CData):
    __fmt__ = '>H'
    __length__  = 2


class _DataFrame(object):
    class MBAP(object):
        def __init__(self):
            self.__transaction_id = UnsignedShort(0x01)
            self.__protocol_id = UnsignedShort(0x00)
            self.__length = UnsignedShort(2)
            self.__unit_id = UnsignedChar(0x00)
        
        @property
        def data_length(self):
            return 7
        
        @property
        def transaction_id(self):
            return self.__transaction_id.value
        
        @property
        def protocol_id(self):
            return self.__protocol_id.value
            
        @property
        def length(self):
            return self.__length.value

        @property
        def unit_id(self):
            return self.__unit_id.value
        
        @property
        def raw_data(self):
            return self.__transaction_id.raw_data + self.__protocol_id.raw_data + self.__length.raw_data + self.__unit_id.raw_data
        
        def set_transaction_id(self, transaction_id):
            self.__transaction_id.set_data(transaction_id)
        
        def set_protocol_id(self, protocol_id):
            self.__protocol_id.set_data(protocol_id)

        def set_length(self, length):
            self.__length.set_data(length)
        
        def set_unit_id(self, unit_id):
            self.__unit_id.set_data(unit_id)
        
        def set_data(self, data):
            self.set_transaction_id(data[0:2])
            self.set_protocol_id(data[2:4])
            self.set_length(data[4:6])
            self.set_unit_id(data[6])

    class PDU(object):
        def __init__(self):
            self.__func_code = UnsignedChar(0x00)
            self.__raw_data = self.__func_code.raw_data
        
        @property
        def data_length(self):
            return len(self.raw_data)

        @property
        def func_code(self):
            return self.__func_code.value

        @property
        def raw_data(self):
            return self.__raw_data
        
        def reset(self, func_code):
            self.__func_code.set_data(func_code)
            self.__raw_data = self.__func_code.raw_data
        
        def set_code(self, code):
            if code == ModbusCode.SUCCESS:
                return
            self.__func_code.set_value(self.func_code + 0x80)
            self.__raw_data = self.__func_code.raw_data
            self.__raw_data += UnsignedChar(code).raw_data
        
        def set_raw_data(self, raw_data):
            self.reset(raw_data[0])
            self.__raw_data = raw_data
        
        def add_datas(self, datas, size=1, signed=False):
            if isinstance(datas, bytes):
                self.__raw_data += datas
            else:
                fmt = '>{}{}'.format(len(datas), ('h' if signed else 'H') if size == 2 else  ('b' if signed else 'B'))
                self.__raw_data += struct.pack(fmt, *datas)
        
        def add_raw_data(self, raw_data):
            self.__raw_data += raw_data
        
        def get_datas(self, start, end, fmt):
            return struct.unpack(fmt, self.__raw_data[start:end])
        
        def get_uint8(self, offset):
            return self.get_datas(offset, offset + 1, fmt='>B')[0]
        
        def get_uint16(self, offset):
            return self.get_datas(offset, offset + 2, fmt='>H')[0]
        
        def get_uint32(self, offset):
            return self.get_datas(offset, offset + 4, fmt='>I')[0]

        def get_int8(self, offset):
            return self.get_datas(offset, offset + 1, fmt='>b')[0]

        def get_int16(self, offset):
            return self.get_datas(offset, offset + 2, fmt='>h')[0]

        def get_int32(self, offset):
            return self.get_datas(offset, offset + 4, fmt='>i')[0]
        
        def get_fp32(self, offset):
            return self.get_datas(offset, offset + 4, fmt='>f')[0]

    def __init__(self):
        self.mbap = self.MBAP()
        self.pdu = self.PDU()
    
    @property
    def data_length(self):
        return self.mbap.data_length + self.pdu.data_length
    
    def reset(self, func_code):
        self.pdu.reset(func_code)
        self.mbap.set_length(2)

    def set_data(self, data):
        if len(data) < 8:
            raise ModbusException('the length of data must gt 8')
        self.mbap.set_data(data[:7])
        self.pdu.set_raw_data(data[7:])
        self.mbap.set_length(self.pdu.data_length + 1)
    
    def add_datas(self, datas, size=1, signed=False):
        self.pdu.add_datas(datas, size=size, signed=signed)
        self.mbap.set_length(self.pdu.data_length + 1)

    def set_code(self, code):
        if code == ModbusCode.SUCCESS:
            return
        self.pdu.set_code(code)
        self.mbap.set_length(3)
    
    @property
    def raw_data(self):
        return self.mbap.raw_data + self.pdu.raw_data

class ModbusDataSession(object):
    def __init__(self):
        self.request = _DataFrame()
        self.response = _DataFrame()

    def set_request_data(self, data):
        if not isinstance(data, bytes):
            data = bytes(data)
        self.request.set_data(data)

    def get_request_data(self):
        return self.request.raw_data

    def get_response_data(self):
        return self.response.raw_data


class ModbusDataService(object):
    def __init__(self, modbus_data : ModbusData):
        self.__modbus_data = modbus_data
        self.__data_buf = b''
        self.__session = None
    
    def process_data(self, data, callback=None, is_checked=False):
        if self.__session is None:
            self.__session = ModbusDataSession()
        if is_checked:
            self.__session.set_request_data(data)
            self.process_session(self.__session)
            if callback and callable(callback):
                callback(self.__session.get_request_data(), self.__session.get_response_data())
            return
        
        buf_len = len(self.__data_buf)
        data_len = 0
        remain = len(data)
        cpy_inx = 0
        while True:
            if buf_len + remain < 7:
                # 长度不够
                if isinstance(data, bytes):
                    self.__data_buf += data[cpy_inx:]
                else:
                    self.__data_buf += bytes(data[cpy_inx:])
                return
            if buf_len < 7:
                if isinstance(data, bytes):
                    self.__data_buf += data[cpy_inx:cpy_inx + 7 - buf_len]
                else:
                    self.__data_buf += bytes(data[cpy_inx:cpy_inx + 7 - buf_len])
                cpy_inx += 7 - buf_len
                remain = len(data) - cpy_inx
                buf_len = 7
            data_len = UnsignedShort(self.__data_buf[4:6]).value
            if data_len > 254:
                # Modbus TCP一帧数据最多260字节
                print('Modbus tcp data length is wrong, discard this part of data and clear the buffer, len={}'.format(data_len))
                self.__data_buf = b''
                return
            if buf_len + remain < data_len + 6:
                # 数据长度不够
                if isinstance(data, bytes):
                    self.__data_buf += data[cpy_inx:]
                else:
                    self.__data_buf += bytes(data[cpy_inx:])
                return
            if isinstance(data, bytes):
                self.__data_buf += data[cpy_inx:cpy_inx + data_len + 6 - buf_len]
            else:
                self.__data_buf += bytes(data[cpy_inx:cpy_inx + data_len + 6 - buf_len])
            self.__session.set_request_data(self.__data_buf)
            self.process_session(self.__session)
            if callback and callable(callback):
                callback(self.__session.get_request_data(), self.__session.get_response_data())
            cpy_inx += data_len + 6 - buf_len
            remain = len(data) - cpy_inx
            buf_len = 0
            self.__data_buf = b''
            if remain == 0:
                return 
    
    def process_session(self, session : ModbusDataSession):
        session.response.set_data(session.request.raw_data[:8])
        length = session.request.mbap.length + 6
        if session.request.data_length < 8 or length > 260 or session.request.data_length < length:
            session.response.set_code(ModbusCode.ILLEGAL_DATA_VALUE)
            return
        code = ModbusCode.ILLEGAL_FUNCTION
        if session.request.pdu.func_code == ModbusFunCode.FC_READ_COILS \
            or session.request.pdu.func_code == ModbusFunCode.FC_READ_DISCRETE_INPUTS:
            code = self.__read_bits(session)
        elif session.request.pdu.func_code == ModbusFunCode.FC_READ_HOLDING_REGS \
            or session.request.pdu.func_code == ModbusFunCode.FC_READ_INPUT_REGS:
            code = self.__read_registers(session)
        elif session.request.pdu.func_code == ModbusFunCode.FC_WRITE_SINGLE_COIL:
            code = self.__write_single_coil_bit(session)
        elif session.request.pdu.func_code == ModbusFunCode.FC_WRITE_SINGLE_REG:
            code = self.__write_single_holding_register(session)
        elif session.request.pdu.func_code == ModbusFunCode.FC_WRITE_MULTIPLE_COILS:
            code = self.__write_multiple_coil_bits(session)
        elif session.request.pdu.func_code == ModbusFunCode.FC_WRITE_MULTIPLE_REGS:
            code = self.__write_multiple_holding_registers(session)
        elif session.request.pdu.func_code == ModbusFunCode.FC_MASK_WRITE_REG:
            code = self.__mask_write_holding_register(session)
        elif session.request.pdu.func_code == ModbusFunCode.FC_WRITE_AND_READ_REGS:
            code = self.__write_and_read_multiple_holding_registers(session)
        session.response.set_code(code)

    def __read_bits(self, session : ModbusDataSession):
        if session.request.data_length < 12:
            return ModbusCode.ILLEGAL_DATA_VALUE
        start_addr = session.request.pdu.get_uint16(1)
        quantity = session.request.pdu.get_uint16(3)
        code = ModbusCode.ILLEGAL_DATA_VALUE
        if 0x0001 <= quantity <= 0x07D0:
            if session.request.pdu.func_code == ModbusFunCode.FC_READ_COILS:
                code, bits = self.__modbus_data.read_coil_bits(start_addr, quantity)
            else:
                code, bits = self.__modbus_data.read_input_bits(start_addr, quantity)
            if code == ModbusCode.SUCCESS:
                byte_size = (quantity + 7) // 8
                data = [0] * byte_size
                for i in range(quantity):
                    if bits[i]:
                        data[i // 8] = data[i // 8] | (1 << (i % 8))
                session.response.add_datas([byte_size], size=1)
                session.response.add_datas(data, size=1)
        return code

    def __read_registers(self, session : ModbusDataSession):
        if session.request.data_length < 12:
            return ModbusCode.ILLEGAL_DATA_VALUE
        start_addr = session.request.pdu.get_uint16(1)
        quantity = session.request.pdu.get_uint16(3)
        code = ModbusCode.ILLEGAL_DATA_VALUE
        if 0x0001 <= quantity <= 0x007D:
            if session.request.pdu.func_code == ModbusFunCode.FC_READ_HOLDING_REGS:
                code, regs = self.__modbus_data.read_holding_registers(start_addr, quantity)
            else:
                code, regs = self.__modbus_data.read_input_registers(start_addr, quantity)
            if code == ModbusCode.SUCCESS:
                byte_size = quantity * 2
                session.response.add_datas([byte_size], size=1)
                session.response.add_datas(regs, size=2)
        return code

    def __write_single_coil_bit(self, session : ModbusDataSession):
        if session.request.data_length < 12:
            return ModbusCode.ILLEGAL_DATA_VALUE
        bit_addr = session.request.pdu.get_uint16(1)
        bit_val = session.request.pdu.get_uint16(3)
        code = ModbusCode.ILLEGAL_DATA_VALUE
        if bit_val == 0x0000 or bit_val == 0xFF00:
            code = self.__modbus_data.write_coil_bits(bit_addr, [1 if bit_val == 0xFF00 else 0])
            if code == ModbusCode.SUCCESS:
                session.response.add_datas(session.request.pdu.raw_data[1:5])
        return code

    def __write_single_holding_register(self, session : ModbusDataSession):
        if session.request.data_length < 12:
            return ModbusCode.ILLEGAL_DATA_VALUE
        reg_addr = session.request.pdu.get_uint16(1)
        reg_val = session.request.pdu.get_uint16(3)
        code = self.__modbus_data.write_holding_registers(reg_addr, [reg_val])
        if code == ModbusCode.SUCCESS:
            session.response.add_datas(session.request.pdu.raw_data[1:5])
        return code

    def __write_multiple_coil_bits(self, session : ModbusDataSession):
        if session.request.data_length < 13:
            return ModbusCode.ILLEGAL_DATA_VALUE
        start_addr = session.request.pdu.get_uint16(1)
        quantity = session.request.pdu.get_uint16(3)
        byte_count = session.request.pdu.get_uint8(5)
        quantity_ok = 0x0001 <= quantity <= 0x07B0
        byte_count_ok = byte_count >= (quantity + 7) // 8
        pdu_len_ok = (session.request.data_length - 7 - 6) >= byte_count
        code = ModbusCode.ILLEGAL_DATA_VALUE
        if quantity_ok and byte_count_ok and pdu_len_ok:
            bits = [0] * quantity
            for i in range(quantity):
                bit_val = session.request.pdu.get_uint8(i // 8 + 6)
                bits[i] = (bool)(bit_val & (1 << (i % 8)))
            code = self.__modbus_data.write_coil_bits(start_addr, bits)
            if code == ModbusCode.SUCCESS:
                session.response.add_datas(session.request.pdu.raw_data[1:5])
        return code
        
    def __write_multiple_holding_registers(self, session : ModbusDataSession):
        if session.request.data_length < 13:
            return ModbusCode.ILLEGAL_DATA_VALUE
        start_addr = session.request.pdu.get_uint16(1)
        quantity = session.request.pdu.get_uint16(3)
        byte_count = session.request.pdu.get_uint8(5)
        quantity_ok = 0x0001 <= quantity <= 0x007B
        byte_count_ok = byte_count == quantity * 2
        pdu_len_ok = (session.request.data_length - 7 - 6) >= byte_count
        code = ModbusCode.ILLEGAL_DATA_VALUE
        if quantity_ok and byte_count_ok and pdu_len_ok:
            regs = [0] * quantity
            for i in range(quantity):
                regs[i] = session.request.pdu.get_uint16(i * 2 + 6)
            code = self.__modbus_data.write_holding_registers(start_addr, regs)
            if code == ModbusCode.SUCCESS:
                session.response.add_datas(session.request.pdu.raw_data[1:5])
        return code

    def __mask_write_holding_register(self, session : ModbusDataSession):
        if session.request.data_length < 14:
            return ModbusCode.ILLEGAL_DATA_VALUE
        ref_addr = session.request.pdu.get_uint16(1)
        and_mask = session.request.pdu.get_uint16(3)
        or_mask = session.request.pdu.get_uint16(5)
        code = self.__modbus_data.mask_write_holding_register(ref_addr, and_mask, or_mask)
        if code == ModbusCode.SUCCESS:
            session.response.add_datas(session.request.pdu.raw_data[1:7])
        return code

    def __write_and_read_multiple_holding_registers(self, session : ModbusDataSession):
        if session.request.data_length < 17:
            return ModbusCode.ILLEGAL_DATA_VALUE
        r_start_addr = session.request.pdu.get_uint16(1)
        r_quantity = session.request.pdu.get_uint16(3)
        w_start_addr = session.request.pdu.get_uint16(5)
        w_quantity = session.request.pdu.get_uint16(7)
        byte_count = session.request.pdu.get_uint8(9)
        r_quantity_ok = 0x0001 <= r_quantity <= 0x007D
        w_quantity_ok = 0x0001 <= w_quantity <= 0x0079
        byte_count_ok = byte_count == w_quantity * 2
        pdu_len_ok = (session.request.data_length - 7 - 10) >= byte_count
        code = ModbusCode.ILLEGAL_DATA_VALUE
        if r_quantity_ok and w_quantity_ok and byte_count_ok and pdu_len_ok:
            w_regs = [0] * w_quantity
            for i in range(w_quantity):
                w_regs[i] = session.request.pdu.get_uint16(i * 2 + 10)
            code, r_regs = self.__modbus_data.write_and_read_holding_registers(w_start_addr, w_regs, r_start_addr, r_quantity)
            if code == ModbusCode.SUCCESS:
                byte_size = r_quantity * 2
                session.response.add_datas([byte_size], size=1)
                session.response.add_datas(r_regs, size=2)
        return code
