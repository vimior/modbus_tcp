
#!/usr/bin/env python3
# Software License Agreement (BSD License)
#
# Copyright (c) 2022, Vinman, Inc.
# All rights reserved.
#
# Author: Vinman <vinman.cub@gmail.com>

import unittest
import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from modbus_tcp import ModbusData, StaticModbusData, ModbusCode


class TestModbusData(unittest.TestCase):
    def test_coil_bits(self):
        # test read success
        code, bits = StaticModbusData.read_coil_bits(0x00, 20)
        self.assertTrue(code == 0 and len(bits) == 20)
        # test read failed
        code, bits = StaticModbusData.read_coil_bits(0x0A, 20)
        self.assertTrue(code == ModbusCode.ILLEGAL_DATA_ADDRESS)
        # test get struct success
        bit_struct = StaticModbusData.get_coil_bit_struct(0x0A)
        self.assertTrue(bit_struct != None)
        # test get struct failed
        bit_struct = StaticModbusData.get_coil_bit_struct(0x20)
        self.assertTrue(bit_struct == None)

        # test bind get
        def get_bit(val):
            return 1
        StaticModbusData.get_coil_bit_struct(0x06).bind_get(get_bit)
        code, bits = StaticModbusData.read_coil_bits(0x06, 1)
        self.assertTrue(code == 0 and bits[0] == 1)

        # test bind set
        def set_bit1(val):
            return 0
        def set_bit2(val):
            return -1
        StaticModbusData.get_coil_bit_struct(0x07).bind_set(set_bit1)
        StaticModbusData.get_coil_bit_struct(0x08).bind_set(set_bit2)
        code, bits = StaticModbusData.read_coil_bits(0x07, 2)
        self.assertTrue(code == 0 and bits[0] == 0 and bits[1] == 0)
        code = StaticModbusData.write_coil_bits(0x07, [1, 1])
        self.assertTrue(code == 0)
        code, bits = StaticModbusData.read_coil_bits(0x07, 2)
        # 由于绑定的set_bit2方法返回值非0，所以写入时是不会更新寄存器的值的，所以此时读取到的还是0
        self.assertTrue(code == 0 and bits[0] == 1 and bits[1] == 0)

        # test set_data
        code, bits = StaticModbusData.read_coil_bits(0x09, 1)
        self.assertTrue(code == 0 and bits[0] == 0)
        StaticModbusData.get_coil_bit_struct(0x09).set_data(1)
        code, bits = StaticModbusData.read_coil_bits(0x09, 1)
        self.assertTrue(code == 0 and bits[0] == 1)

    def test_input_bits(self):
        # test read success
        code, bits = StaticModbusData.read_input_bits(0x00, 30)
        self.assertTrue(code == 0 and len(bits) == 30)
        # test read failed
        code, bits = StaticModbusData.read_input_bits(0x0A, 30)
        self.assertTrue(code == ModbusCode.ILLEGAL_DATA_ADDRESS)
        # test get struct success
        bit_struct = StaticModbusData.get_input_bit_struct(0x0A)
        self.assertTrue(bit_struct != None)
        # test get struct failed
        bit_struct = StaticModbusData.get_input_bit_struct(0x20)
        self.assertTrue(bit_struct == None)

        # test bind get
        def get_bit(val):
            return 1
        StaticModbusData.get_input_bit_struct(0x06).bind_get(get_bit)
        code, bits = StaticModbusData.read_input_bits(0x06, 1)
        self.assertTrue(code == 0 and bits[0] == 1)

        # test bind set
        def set_bit1(val):
            return 0
        def set_bit2(val):
            return -1
        StaticModbusData.get_input_bit_struct(0x07).bind_set(set_bit1)
        StaticModbusData.get_input_bit_struct(0x08).bind_set(set_bit2)
        code, bits = StaticModbusData.read_input_bits(0x07, 2)
        self.assertTrue(code == 0 and bits[0] == 0 and bits[1] == 0)
        code = StaticModbusData.write_input_bits(0x07, [1, 1])
        self.assertTrue(code == 0)
        code, bits = StaticModbusData.read_input_bits(0x07, 2)
        # 由于绑定的set_bit2方法返回值非0，所以写入时是不会更新寄存器的值的，所以此时读取到的还是0
        self.assertTrue(code == 0 and bits[0] == 1 and bits[1] == 0)

        # test set_data
        code, bits = StaticModbusData.read_input_bits(0x09, 1)
        self.assertTrue(code == 0 and bits[0] == 0)
        StaticModbusData.get_input_bit_struct(0x09).set_data(1)
        code, bits = StaticModbusData.read_input_bits(0x09, 1)
        self.assertTrue(code == 0 and bits[0] == 1)

    def test_holding_regs(self):
        # test read success
        code, bits = StaticModbusData.read_holding_registers(0x00, 40)
        self.assertTrue(code == 0 and len(bits) == 40)
        # test read failed
        code, bits = StaticModbusData.read_holding_registers(0x0A, 40)
        self.assertTrue(code == ModbusCode.ILLEGAL_DATA_ADDRESS)
        # test get struct success
        reg_struct = StaticModbusData.get_holding_register_struct(0x0A)
        self.assertTrue(reg_struct != None)
        # test get struct failed
        reg_struct = StaticModbusData.get_holding_register_struct(0x30)
        self.assertTrue(reg_struct == None)

        # test bind get
        def get_reg(val):
            return 22
        StaticModbusData.get_holding_register_struct(0x06).bind_get(get_reg)
        code, regs = StaticModbusData.read_holding_registers(0x06, 1)
        self.assertTrue(code == 0 and regs[0] == 22)

        # test bind set
        def set_reg1(val):
            return 0
        def set_reg2(val):
            return -1
        StaticModbusData.get_holding_register_struct(0x07).bind_set(set_reg1)
        StaticModbusData.get_holding_register_struct(0x08).bind_set(set_reg2)
        code, regs = StaticModbusData.read_holding_registers(0x07, 2)
        self.assertTrue(code == 0 and regs[0] == 0 and regs[1] == 0)
        code = StaticModbusData.write_holding_registers(0x07, [33, 66])
        self.assertTrue(code == 0)
        code, regs = StaticModbusData.read_holding_registers(0x07, 2)
        # 由于绑定的set_reg2方法返回值非0，所以写入时是不会更新寄存器的值的，所以此时读取到的还是0
        self.assertTrue(code == 0 and regs[0] == 33 and regs[1] == 0)

        # test set_data
        code, regs = StaticModbusData.read_holding_registers(0x09, 1)
        self.assertTrue(code == 0 and regs[0] == 0)
        StaticModbusData.get_holding_register_struct(0x09).set_data(99)
        code, regs = StaticModbusData.read_holding_registers(0x09, 1)
        self.assertTrue(code == 0 and regs[0] == 99)

    def test_input_regs(self):
        # test read success
        code, bits = StaticModbusData.read_input_registers(0x00, 50)
        self.assertTrue(code == 0 and len(bits) == 50)
        # test read failed
        code, bits = StaticModbusData.read_input_registers(0x0A, 50)
        self.assertTrue(code == ModbusCode.ILLEGAL_DATA_ADDRESS)
        # test get struct success
        reg_struct = StaticModbusData.get_input_register_struct(0x0A)
        self.assertTrue(reg_struct != None)
        # test get struct failed
        reg_struct = StaticModbusData.get_input_register_struct(0x40)
        self.assertTrue(reg_struct == None)

        # test bind get
        def get_reg(val):
            return 22
        StaticModbusData.get_input_register_struct(0x06).bind_get(get_reg)
        code, regs = StaticModbusData.read_input_registers(0x06, 1)
        self.assertTrue(code == 0 and regs[0] == 22)

        # test bind set
        def set_reg1(val):
            return 0
        def set_reg2(val):
            return -1
        StaticModbusData.get_input_register_struct(0x07).bind_set(set_reg1)
        StaticModbusData.get_input_register_struct(0x08).bind_set(set_reg2)
        code, regs = StaticModbusData.read_input_registers(0x07, 2)
        self.assertTrue(code == 0 and regs[0] == 0 and regs[1] == 0)
        code = StaticModbusData.write_input_registers(0x07, [33, 66])
        self.assertTrue(code == 0)
        code, regs = StaticModbusData.read_input_registers(0x07, 2)
        # 由于绑定的set_reg2方法返回值非0，所以写入时是不会更新寄存器的值的，所以此时读取到的还是0
        self.assertTrue(code == 0 and regs[0] == 33 and regs[1] == 0)

        # test set_data
        code, regs = StaticModbusData.read_input_registers(0x09, 1)
        self.assertTrue(code == 0 and regs[0] == 0)
        StaticModbusData.get_input_register_struct(0x09).set_data(99)
        code, regs = StaticModbusData.read_input_registers(0x09, 1)
        self.assertTrue(code == 0 and regs[0] == 99)


if __name__ == '__main__':
    modbus_data = ModbusData(20, 30, 40, 50)
    StaticModbusData.set_modbus_data(modbus_data)

    unittest.main()
