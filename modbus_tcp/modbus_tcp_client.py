#!/usr/bin/env python3
# Software License Agreement (BSD License)
#
# Copyright (c) 2022, Vinman, Inc.
# All rights reserved.
#
# Author: Vinman <vinman.cub@gmail.com>

import time
import socket
import struct
import threading
from .modbus_tcp_data import ModbusDataSession


class ModbusTcpClient(object):
    def __init__(self, ip, port=502, unit_id=0x01):
        self.__sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.__sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.__sock.setblocking(True)
        self.__sock.connect((ip, port))

        self.__unit_id = unit_id
        self.__lock = threading.Lock()
        self.session = ModbusDataSession()

    def __pack_to_send(self, unit_id=None):
        transaction_id = self.session.request.mbap.transaction_id
        transaction_id = transaction_id % 65535 + 1
        self.session.request.mbap.set_transaction_id(transaction_id)
        self.session.request.mbap.set_unit_id(unit_id if unit_id is not None else self.__unit_id)
        self.__sock.send(self.session.request.raw_data)
    
    def __wait_to_response(self, timeout=5):
        expired = time.monotonic() + timeout
        length = 0
        recv_data = b''
        while time.monotonic() < expired:
            if len(recv_data) < 7:
                recv_data += self.__sock.recv(7 - len(recv_data))
            if len(recv_data) < 7:
                continue
            if length == 0:
                length = struct.unpack('>H', recv_data[4:6])[0]
            if len(recv_data) < length + 6:
                recv_data += self.__sock.recv(length + 6 - len(recv_data))
            if len(recv_data) < length + 6:
                continue
            self.session.set_response_data(recv_data)
            if self.session.response.mbap.transaction_id != self.session.request.mbap.transaction_id:
                print('transaction_id warning, send={}, recv={}'.format(
                    self.session.request.mbap.transaction_id,
                    self.session.response.mbap.transaction_id
                ))
                continue
            if self.session.response.mbap.protocol_id != self.session.request.mbap.protocol_id:
                print('protocol_id warning, send={}, recv={}'.format(
                    self.session.request.mbap.protocol_id,
                    self.session.response.mbap.protocol_id
                ))
                continue
            if self.session.response.mbap.unit_id != self.session.request.mbap.unit_id:
                print('unit_id warning, send={}, recv={}'.format(
                    self.session.request.mbap.unit_id,
                    self.session.response.mbap.unit_id
                ))
                continue
            if self.session.response.pdu.func_code != self.session.request.pdu.func_code \
                and self.session.response.pdu.func_code != self.session.request.pdu.func_code + 0x80:
                print('func_code warning, send={}, recv={}'.format(
                    self.session.request.pdu.func_code,
                    self.session.response.pdu.func_code
                ))
                continue
            return 0 if self.session.response.pdu.func_code == self.session.request.pdu.func_code else self.session.response.pdu.get_int8(1)
        return -3  # TIMEOUT
    
    def __request(self, unit_id=None):
        with self.__lock:
            self.__pack_to_send(unit_id=unit_id)
            return self.__wait_to_response()
    
    def read_coil_bits(self, addr, quantity):
        self.session.request.reset(0x01)
        self.session.request.add_datas([addr, quantity], size=2)
        code = self.__request()
        if code != 0:
            return code, list(self.session.response.raw_data)
        else:
            return code, [(self.session.response.raw_data[9 + i // 8] >> (i % 8) & 0x01) for i in range(quantity)]

    def read_input_bits(self, addr, quantity):
        self.session.request.reset(0x02)
        self.session.request.add_datas([addr, quantity], size=2)
        code = self.__request()
        if code != 0:
            return code, list(self.session.response.raw_data)
        else:
            return code, [(self.session.response.raw_data[9 + i // 8] >> (i % 8) & 0x01) for i in range(quantity)]

    def read_holding_registers(self, addr, quantity, signed=False):
        self.session.request.reset(0x03)
        self.session.request.add_datas([addr, quantity], size=2)
        code = self.__request()
        if code != 0:
            return code, list(self.session.response.raw_data)
        else:
            return code, self.session.response.pdu.get_int16(2, count=quantity, signed=signed)

    def read_input_registers(self, addr, quantity, signed=False):
        self.session.request.reset(0x04)
        self.session.request.add_datas([addr, quantity], size=2)
        code = self.__request()
        if code != 0:
            return code, list(self.session.response.raw_data)
        else:
            return code, self.session.response.pdu.get_int16(2, count=quantity, signed=signed)

    def write_single_coil_bit(self, addr, on):
        self.session.request.reset(0x05)
        self.session.request.add_datas([addr, 0xFF00 if on else 0x0000], size=2)
        return self.__request()

    def write_single_holding_register(self, addr, val, signed=False):
        self.session.request.reset(0x06)
        self.session.request.add_datas([addr], size=2)
        self.session.request.add_datas([val], size=2, signed=signed)
        return self.__request()

    def write_multiple_coil_bits(self, addr, bits):
        self.session.request.reset(0x0F)
        datas = [0] * ((len(bits) + 7) // 8)
        for i in range(len(bits)):
            if bits[i]:
                datas[i // 8] |= (1 << (i % 8))
        self.session.request.add_datas([addr, len(bits)], size=2)
        self.session.request.add_datas([len(datas)], size=1)
        self.session.request.add_datas(datas, size=1)
        return self.__request()

    def write_multiple_holding_registers(self, addr, regs, signed=False):
        self.session.request.reset(0x10)
        self.session.request.add_datas([addr, len(regs)], size=2)
        self.session.request.add_datas([len(regs) * 2], size=1)
        self.session.request.add_datas(regs, size=2, signed=signed)
        return self.__request()

    def mask_write_holding_register(self, addr, and_mask, or_mask):
        self.session.request.reset(0x16)
        self.session.request.add_datas([addr, and_mask, or_mask], size=2)
        return self.__request()

    def write_and_read_holding_registers(self, r_addr, r_quantity, w_addr, w_regs, r_signed=False, w_signed=False):
        self.session.request.reset(0x17)
        self.session.request.add_datas([r_addr, r_quantity, w_addr, len(w_regs)], size=2)
        self.session.request.add_datas([len(w_regs) * 2], size=1)
        self.session.request.add_datas(w_regs, size=2, signed=w_signed)
        code = self.__request()
        if code != 0:
            return code, list(self.session.response.raw_data)
        else:
            return code, self.session.response.pdu.get_int16(2, count=r_quantity, signed=r_signed)
