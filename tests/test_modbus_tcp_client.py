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
from  modbus_tcp import ModbusTcpClient


if __name__ == '__main__':
    client = ModbusTcpClient('127.0.0.1')
    print(client.read_coil_bits(0x00, 16))
    print(client.read_input_bits(0x00, 16))
    print(client.read_holding_registers(0x20, 2))
    print(client.read_input_registers(0x40, 9, signed=True))
    print(client.read_coil_bits(0x40, 200))
