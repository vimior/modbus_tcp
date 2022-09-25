#!/usr/bin/env python3
# Software License Agreement (BSD License)
#
# Copyright (c) 2022, Vinman, Inc.
# All rights reserved.
#
# Author: Vinman <vinman.cub@gmail.com>

from .modbus_code import ModbusCode

class ModbusStructData(object):
    def __init__(self, data = 0):
        self.__data = data
        self.__get_func = None
        self.__set_func = None
    
    def bind_get(self, func):
        self.__get_func = func
    
    def bind_set(self, func):
        self.__set_func = func
    
    def unbind_get(self):
        self.__get_func = None
    
    def unbind_set(self):
        self.__set_func = None
    
    def get_data(self):
        return self.__data

    def set_data(self, val):
        self.__data = val
    
    def get(self):
        if self.__get_func is not None:
            self.__data = self.__get_func(self.__data)
        return self.__data
    
    def set(self, val):
        if self.__set_func is not None:
            code = self.__set_func(val)
            self.__data = val if code == 0 else self.__data
            return code
        else:
            self.__data = val
            return 0


class ModbusData(object):
    def __init__(
        self,
        coil_bit_count,
        input_bit_count,
        holding_reg_count,
        input_reg_count,
        coil_bit_start_addr=0x00,
        input_bit_start_addr=0x00,
        holding_reg_start_addr=0x00,
        input_reg_start_addr=0x00
    ):
        self.__coil_bit_count = coil_bit_count
        self.__input_bit_count = input_bit_count
        self.__holding_reg_count = holding_reg_count
        self.__input_reg_count = input_reg_count
        self.__coil_bit_start_addr = coil_bit_start_addr
        self.__input_bit_start_addr = input_bit_start_addr
        self.__holding_reg_start_addr = holding_reg_start_addr
        self.__input_reg_start_addr = input_reg_start_addr
        self.__coil_bits = [ModbusStructData() for _ in range(coil_bit_count)]
        self.__input_bits = [ModbusStructData() for _ in range(input_bit_count)]
        self.__holding_regs = [ModbusStructData() for _ in range(holding_reg_count)]
        self.__input_regs = [ModbusStructData() for _ in range(input_reg_count)]
    
    def __get_coil_bits_inx(self, addr, quantity):
        """
        根据地址和数量获取起始寄存器索引

        @return: 成功返回索引, 失败(地址非法)返回-1 
        """
        inx = addr - self.__coil_bit_start_addr
        return inx if inx >= 0 and inx <= self.__coil_bit_count - quantity else -1
    
    def __get_input_bits_inx(self, addr, quantity):
        """
        根据地址和数量获取起始寄存器索引

        @return: 成功返回索引, 失败(地址非法)返回-1 
        """
        inx = addr - self.__input_bit_start_addr
        return inx if inx >= 0 and inx <= self.__input_bit_count - quantity else -1

    def __get_holding_regs_inx(self, addr, quantity):
        """
        根据地址和数量获取起始寄存器索引

        @return: 成功返回索引, 失败(地址非法)返回-1 
        """
        inx = addr - self.__holding_reg_start_addr
        return inx if inx >= 0 and inx <= self.__holding_reg_count - quantity else -1

    def __get_input_regs_inx(self, addr, quantity):
        """
        根据地址和数量获取起始寄存器索引

        @return: 成功返回索引, 失败(地址非法)返回-1 
        """
        inx = addr - self.__input_reg_start_addr
        return inx if inx >= 0 and inx <= self.__input_reg_count - quantity else -1

    def read_coil_bits(self, addr, quantity):
        inx = self.__get_coil_bits_inx(addr, quantity)
        if inx < 0:
            return ModbusCode.ILLEGAL_DATA_ADDRESS, None
        bits = []
        for i in range(quantity):
            bits.append(self.__coil_bits[inx + i].get())
        return ModbusCode.SUCCESS, bits

    def read_input_bits(self, addr, quantity):
        inx = self.__get_input_bits_inx(addr, quantity)
        if inx < 0:
            return ModbusCode.ILLEGAL_DATA_ADDRESS, None
        bits = []
        for i in range(quantity):
            bits.append(self.__input_bits[inx + i].get())
        return ModbusCode.SUCCESS, bits

    def read_holding_registers(self, addr, quantity):
        inx = self.__get_holding_regs_inx(addr, quantity)
        if inx < 0:
            return ModbusCode.ILLEGAL_DATA_ADDRESS, None
        regs = []
        for i in range(quantity):
            regs.append(self.__holding_regs[inx + i].get())
        return ModbusCode.SUCCESS, regs

    def read_input_registers(self, addr, quantity):
        inx = self.__get_input_regs_inx(addr, quantity)
        if inx < 0:
            return ModbusCode.ILLEGAL_DATA_ADDRESS, None
        regs = []
        for i in range(quantity):
            regs.append(self.__input_regs[inx + i].get())
        return ModbusCode.SUCCESS, regs

    def write_coil_bits(self, addr, bits):
        inx = self.__get_coil_bits_inx(addr, len(bits))
        if inx < 0:
            return ModbusCode.ILLEGAL_DATA_ADDRESS
        for i in range(len(bits)):
            bit = 1 if bits[i] else 0
            # self.__coil_bits[inx + i].set(bit)
            if self.__coil_bits[inx + i].get() != bit:
                self.__coil_bits[inx + i].set(bit)
        return ModbusCode.SUCCESS

    def write_input_bits(self, addr, bits):
        inx = self.__get_input_bits_inx(addr, len(bits))
        if inx < 0:
            return ModbusCode.ILLEGAL_DATA_ADDRESS
        for i in range(len(bits)):
            bit = 1 if bits[i] else 0
            # self.__input_bits[inx + i].set(bit)
            if self.__input_bits[inx + i].get() != bit:
                self.__input_bits[inx + i].set(bit)
        return ModbusCode.SUCCESS

    def write_holding_registers(self, addr, regs):
        inx = self.__get_holding_regs_inx(addr, len(regs))
        if inx < 0:
            return ModbusCode.ILLEGAL_DATA_ADDRESS
        for i in range(len(regs)):
            # self.__holding_regs[inx + i].set(regs[i])
            if self.__holding_regs[inx + i].get() != regs[i]:
                self.__holding_regs[inx + i].set(regs[i])
        return ModbusCode.SUCCESS

    def write_input_registers(self, addr, regs):
        inx = self.__get_input_regs_inx(addr, len(regs))
        if inx < 0:
            return ModbusCode.ILLEGAL_DATA_ADDRESS
        for i in range(len(regs)):
            # self.__input_regs[inx + i].set(regs[i])
            if self.__input_regs[inx + i].get() != regs[i]:
                self.__input_regs[inx + i].set(regs[i])
        return ModbusCode.SUCCESS

    def mask_write_holding_register(self, addr, and_mask, or_mask):
        inx = self.__get_holding_regs_inx(addr, 1)
        if inx < 0:
            return ModbusCode.ILLEGAL_DATA_ADDRESS
        old_val = self.__holding_regs[inx].get()
        new_val = (old_val & and_mask) | (or_mask & ~and_mask)
        # self.__holding_regs[inx].set(new_val)
        if old_val != new_val:
            self.__holding_regs[inx].set(new_val)
        return ModbusCode.SUCCESS

    def write_and_read_holding_registers(self, w_addr, w_regs, r_addr, r_quantity):
        w_inx = self.__get_holding_regs_inx(w_addr, len(w_regs))
        if w_inx < 0:
            return ModbusCode.ILLEGAL_DATA_ADDRESS, None
        
        r_inx = self.__get_holding_regs_inx(r_addr, r_quantity)
        if r_inx < 0:
            return ModbusCode.ILLEGAL_DATA_ADDRESS, None

        # write
        for i in range(len(w_regs)):
            # self.__holding_regs[w_inx + i].set(w_regs[i])
            if self.__holding_regs[w_inx + i].get() != w_regs[i]:
                self.__holding_regs[w_inx + i].set(w_regs[i])
        
        # read
        regs = []
        for i in range(r_quantity):
            regs.append(self.__holding_regs[r_inx + i].get())
        return ModbusCode.SUCCESS, regs

    def get_coil_bit_struct(self, addr):
        inx = self.__get_coil_bits_inx(addr, 1)
        return None if inx < 0 else self.__coil_bits[inx]

    def get_input_bit_struct(self, addr):
        inx = self.__get_input_bits_inx(addr, 1)
        return None if inx < 0 else self.__input_bits[inx]

    def get_holding_register_struct(self, addr):
        inx = self.__get_holding_regs_inx(addr, 1)
        return None if inx < 0 else self.__holding_regs[inx]

    def get_input_register_struct(self, addr):
        inx = self.__get_input_regs_inx(addr, 1)
        return None if inx < 0 else self.__input_regs[inx]


class StaticModbusData(object):
    __modbus_data = None

    @classmethod
    def set_modbus_data(cls, modbus_data):
        cls.__modbus_data = modbus_data

    @classmethod
    def get_modbus_data(cls):
        return cls.__modbus_data

    @classmethod
    def read_coil_bits(cls, addr, quantity):
        if cls.__modbus_data is None:
            return ModbusCode.MODBUS_DATA_NOT_CREATE, None
        return cls.__modbus_data.read_coil_bits(addr, quantity)
    
    @classmethod
    def read_input_bits(cls, addr, quantity):
        if cls.__modbus_data is None:
            return ModbusCode.MODBUS_DATA_NOT_CREATE, None
        return cls.__modbus_data.read_input_bits(addr, quantity)

    @classmethod
    def read_holding_registers(cls, addr, quantity):
        if cls.__modbus_data is None:
            return ModbusCode.MODBUS_DATA_NOT_CREATE, None
        return cls.__modbus_data.read_holding_registers(addr, quantity)

    @classmethod
    def read_input_registers(cls, addr, quantity):
        if cls.__modbus_data is None:
            return ModbusCode.MODBUS_DATA_NOT_CREATE, None
        return cls.__modbus_data.read_input_registers(addr, quantity)

    @classmethod
    def write_coil_bits(cls, addr, bits):
        if cls.__modbus_data is None:
            return ModbusCode.MODBUS_DATA_NOT_CREATE
        return cls.__modbus_data.write_coil_bits(addr, bits)

    @classmethod
    def write_input_bits(cls, addr, bits):
        if cls.__modbus_data is None:
            return ModbusCode.MODBUS_DATA_NOT_CREATE
        return cls.__modbus_data.write_input_bits(addr, bits)

    @classmethod
    def write_holding_registers(cls, addr, regs):
        if cls.__modbus_data is None:
            return ModbusCode.MODBUS_DATA_NOT_CREATE
        return cls.__modbus_data.write_holding_registers(addr, regs)

    @classmethod
    def write_input_registers(cls, addr, regs):
        if cls.__modbus_data is None:
            return ModbusCode.MODBUS_DATA_NOT_CREATE
        return cls.__modbus_data.write_input_registers(addr, regs)

    @classmethod
    def mask_write_holding_register(cls, addr, and_mask, or_mask):
        if cls.__modbus_data is None:
            return ModbusCode.MODBUS_DATA_NOT_CREATE
        return cls.__modbus_data.mask_write_holding_register(addr, and_mask, or_mask)

    @classmethod
    def write_and_read_holding_registers(cls, w_addr, w_regs, r_addr, r_quantity):
        if cls.__modbus_data is None:
            return ModbusCode.MODBUS_DATA_NOT_CREATE, None
        return cls.__modbus_data.write_and_read_holding_registers(w_addr, w_regs, r_addr, r_quantity)

    @classmethod
    def get_coil_bit_struct(cls, addr):
        return None if cls.__modbus_data is None else cls.__modbus_data.get_coil_bit_struct(addr)

    @classmethod
    def get_input_bit_struct(cls, addr):
        return None if cls.__modbus_data is None else cls.__modbus_data.get_input_bit_struct(addr)

    @classmethod
    def get_holding_register_struct(cls, addr):
        return None if cls.__modbus_data is None else cls.__modbus_data.get_holding_register_struct(addr)

    @classmethod
    def get_input_register_struct(cls, addr):
        return None if cls.__modbus_data is None else cls.__modbus_data.get_input_register_struct(addr)