#!/usr/bin/env python3
# Software License Agreement (BSD License)
#
# Copyright (c) 2022, Vinman, Inc.
# All rights reserved.
#
# Author: Vinman <vinman.cub@gmail.com>

import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from modbus_tcp import ModbusData, StaticModbusData

def get_bit(val):
    return 1

def get_reg(val):
    return 99

def set_reg(val):
    print('set_reg: {}'.format(val))
    return 0

def main():
    # 创建Modbus寄存器
    modbus_data = ModbusData(20, 20, 20, 20)
    # 绑定寄存器操作实例到对应的静态操作类，这样就可以通过静态类操作到实例了
    # 也可以不绑定，直接通过实例操作，绑定是为了在整个程序的任何地方都能通过静态类访问
    StaticModbusData.set_modbus_data(modbus_data)
    
    start_addr = 0x00  # 该示例操作的寄存器起始地址
    quantity = 8  # 该示例操作的寄存器个数

    # 读取线圈状态寄存器
    code, r_bits = StaticModbusData.read_coil_bits(start_addr, quantity)
    print('[1] read_coil_bits, code={}, bits={}'.format(code, r_bits))

    w_bits = [0, 1, 0, 1, 0, 1, 0, 1]
    # 写数据到线圈状态寄存器
    code = StaticModbusData.write_coil_bits(start_addr, w_bits)
    print('write_coil_bits, code={}'.format(code))

    # 读取线圈状态寄存器
    code, r_bits = StaticModbusData.read_coil_bits(start_addr, quantity)
    print('[2] read_coil_bits, code={}, bits={}'.format(code, r_bits))

    # 给线圈状态寄存器额外绑定get方法，之后读取对应寄存器的值将由绑定的方法返回
    for i in range(quantity):
        StaticModbusData.get_coil_bit_struct(start_addr+ i).bind_get(get_bit)

    # 读取线圈状态寄存器, 此时读取的值将由绑定的方法返回
    code, r_bits = StaticModbusData.read_coil_bits(start_addr, quantity)
    print('[3] read_coil_bits, code={}, bits={}'.format(code, r_bits))

    # 读取保持寄存器
    code, r_regs = StaticModbusData.read_holding_registers(start_addr, quantity)
    print('[1] read_holding_registers, code={}, regs={}'.format(code, r_regs))

    # 给保持寄存器额外绑定set方法，之后写对应寄存器的值将会调用绑定的方法
    for i in range(quantity):
        StaticModbusData.get_holding_register_struct(start_addr+ i).bind_set(set_reg)

    w_regs = [1, 2, 3, 4, 5, 6, 7, 8]
    # 写数据到保持寄存器, 此时每个要写入的寄存器都会调用到绑定的set方法
    code = StaticModbusData.write_holding_registers(start_addr, w_regs)
    print('write_holding_registers, code={}'.format(code))

    # 读取保持寄存器
    code, r_regs = StaticModbusData.read_holding_registers(start_addr, quantity)
    print('[2] read_holding_registers, code={}, regs={}'.format(code, r_regs))

    # 给保持寄存器额外绑定get方法，之后读取对应寄存器的值将由绑定的方法返回
    for i in range(quantity):
        StaticModbusData.get_holding_register_struct(start_addr+ i).bind_get(get_reg)

    # 读取保持寄存器, 此时读取的值将由绑定的方法返回
    code, r_regs = StaticModbusData.read_holding_registers(start_addr, quantity)
    print('[3] read_holding_registers, code={}, regs={}'.format(code, r_regs))


if __name__ == '__main__':
    main()
