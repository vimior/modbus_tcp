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
from  modbus_tcp import ModbusTcpServer, ModbusData


if __name__ == '__main__':
    modbus_data = ModbusData(100, 100, 100, 100)
    server = ModbusTcpServer()
    server.set_modbus_data(modbus_data)
    server.run()
