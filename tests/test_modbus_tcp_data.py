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
from modbus_tcp import ModbusData, ModbusDataSession, ModbusDataService

def print_bytes(datas, prefix=''):
    print('{}'.format(prefix), end=' ')
    for i in range(len(datas)):
        print('{:x}'.format(datas[i]).zfill(2), end=' ')
    print('')

def process_callback(req_data, res_data):
    print_bytes(req_data, prefix='[callback] req:')
    print_bytes(res_data, prefix='[callback] res:')

def main():
    # 创建Modbus寄存器
    modbus_data = ModbusData(20, 20, 20, 20)

    regs = [11, 12, 13, 14, 15, 16, 17, 18, 19, 20]
    code = modbus_data.write_input_registers(0x00, regs)
    print('write_input_registers, code={}'.format(code))

    service = ModbusDataService(modbus_data)
    session = ModbusDataSession()

    # 模拟请求数据(0x04): 从地址为0x00开始读取10个输入寄存器的数据
    req_data_1 = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x04, 0x00, 0x00, 0x00, 10]
    session.set_request_data(req_data_1)
    service.process_session(session)
    print_bytes(session.get_request_data(), prefix='req:')
    print_bytes(session.get_response_data(), prefix='res:')
    
    # 模拟请求数据(0x10): 往地址为0x00开始的10个保持寄存器写入数据
    req_data_2 = [0x00, 0x01, 0x00, 0x00, 0x00, 27, 0x01, 0x10, 0x00, 0x00, 0x00, 0x0A, 20, 0x00, 11, 0x00, 12, 0x00, 13, 0x00, 14, 0x00, 15, 0x00, 16, 0x00, 17, 0x00, 18, 0x00, 19, 0x00, 20]
    session.set_request_data(req_data_2)
    service.process_session(session)
    print_bytes(session.get_request_data(), prefix='req:')
    print_bytes(session.get_response_data(), prefix='res:')

    # 模拟请求数据(0x10): 从地址为0x00开始读取10个保持寄存器的数据
    req_data_3 = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x03, 0x00, 0x00, 0x00, 10]
    session.set_request_data(req_data_3)
    service.process_session(session)
    print_bytes(session.get_request_data(), prefix='req:')
    print_bytes(session.get_response_data(), prefix='res:')

    print('*******************处理粘包问题**********************')
    
    print('以下是一帧完整的Modbus TCP请求分成了三块')
    req_data_1_1 = [0x00, 0x01, 0x00, 0x00, 0x00]
    req_data_1_2 = [6, 0x01, 0x04, 0x00]
    req_data_1_3 = [0x00, 0x00, 10]

    service.process_data(req_data_1_1, callback=process_callback)
    service.process_data(req_data_1_2, callback=process_callback)
    service.process_data(req_data_1_3, callback=process_callback)

    print('以下是三帧完整的Modbus TCP数据，但是帧与帧之间并没有分开')
    req_data_2_1 = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x04, 0x00, 0x00, 0x00, 10, 0x00, 0x01, 0x00, 0x00, 0x00, 27, 0x01, 0x10, 0x00, 0x00, 0x00, 0x0A, 20, 0x00, 11, 0x00, 12, 0x00, 13, 0x00, 14, 0x00, 15, 0x00, 16, 0x00, 17, 0x00]
    req_data_2_2 = [18, 0x00, 19, 0x00, 20, 0x00, 0x01, 0x00, 0x00]
    req_data_2_3 = [0x00, 6, 0x01, 0x03, 0x00, 0x00, 0x00, 10]

    service.process_data(req_data_2_1, callback=process_callback)
    service.process_data(req_data_2_2, callback=process_callback)
    service.process_data(req_data_2_3, callback=process_callback)

if __name__ == '__main__':
    main()
