#!/usr/bin/env python3
# Software License Agreement (BSD License)
#
# Copyright (c) 2022, Vinman, Inc.
# All rights reserved.
#
# Author: Vinman <vinman.cub@gmail.com>

import os
import sys
import time
import threading
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from modbus_tcp import ModbusData, StaticModbusData, ModbusDataService

def print_bytes(datas, prefix=''):
    print('{}'.format(prefix), end=' ')
    for i in range(len(datas)):
        print('{:x}'.format(datas[i]).zfill(2), end=' ')
    print('')

def process_callback(req_data, res_data):
    print_bytes(req_data, prefix='[callback] req:')
    print_bytes(res_data, prefix='[callback] res:')

def get_reg(val):
  return 99

def set_reg(val):
  return -1

def thread1_handle():
    print('======线程 1 启动=====')
    # 10秒后每隔5s主动修改地址从0x00开始的10个输入寄存器的数据
    val = 1
    time.sleep(10)
    while True:
        for i in range(10):
            StaticModbusData.get_input_register_struct(i).set_data(val + i)
        print('======刚刚修改了地址为0x00开始的10个输入寄存器的值======')
        time.sleep(5)
        val = (val + 10) % 65535

def thread2_handle():
    print('======线程 2 启动=====')
    # 60秒后给地址为0x00开始的10个保持寄存器的绑定读方法
    time.sleep(60)
    for i in range(10):
        StaticModbusData.get_holding_register_struct(i).bind_get(get_reg)
    print('=====给地址为0x00开始的10个保持寄存器绑定了get方法, 之后获取这些地址的保持寄存器得到的都是99=====')


def thread3_handle():
    print('======线程 3 启动=====')
    # 30秒后给地址为0x00开始的10个保持寄存器的绑定写方法
    time.sleep(30)
    for i in range(10):
        StaticModbusData.get_holding_register_struct(i).bind_set(set_reg)
    print('=====给地址为0x00开始的10个保持寄存器绑定了set方法, 之后写这些地址的保持寄存器都会调用这个set方法=====')
    # 由于绑定的set_reg返回值不是0，所以设置是失败的
    # 为了区分，这里把这10个寄存器的值设置为1
    for i in range(10):
        StaticModbusData.get_holding_register_struct(i).set_data(22)

    print('=====为了区分, 这里把这10个保持寄存器的值都修改为22=====')
    
    print('=====但由于绑定的方法返回值不为0, 所以所有写入都是无效的, 往后无论怎么写入, 读取到的都是22=====')

def main():
    # 创建Modbus寄存器, 四种寄存器各100个
    modbus_data = ModbusData(100, 100, 100, 100)
    # 绑定寄存器操作实例到对应的静态操作类，这样就可以通过静态类操作到实例了
    # 也可以不绑定，直接通过实例操作，绑定是为了在整个程序的任何地方都能通过静态类访问
    StaticModbusData.set_modbus_data(modbus_data)

    val = 1
    # 这里创建几个线程模拟实际程序的使用
    threading.Thread(target=thread1_handle, daemon=True).start()
    threading.Thread(target=thread2_handle, daemon=True).start()
    threading.Thread(target=thread3_handle, daemon=True).start()

    service = ModbusDataService(modbus_data)
    
    # 这3个都是完整的数据
    # 模拟请求数据(0x04): 从地址为0x00开始读取10个输入寄存器的数据
    req_data_1 = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x04, 0x00, 0x00, 0x00, 10]
    # 模拟请求数据(0x10): 往地址为0x00开始的10个保持寄存器写入数据
    req_data_2 = [0x00, 0x01, 0x00, 0x00, 0x00, 27, 0x01, 0x10, 0x00, 0x00, 0x00, 0x0A, 20, 0x00, 11, 0x00, 12, 0x00, 13, 0x00, 14, 0x00, 15, 0x00, 16, 0x00, 17, 0x00, 18, 0x00, 19, 0x00, 20]
    # 模拟请求数据(0x10): 从地址为0x00开始读取10个保持寄存器的数据
    req_data_3 = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x03, 0x00, 0x00, 0x00, 10]
    
    # 这3个合起来才是一帧完整的数据，相当于req_data_1
    req_data_1_1 = [0x00, 0x01, 0x00, 0x00, 0x00]
    req_data_1_2 = [6, 0x01, 0x04, 0x00]
    req_data_1_3 = [0x00, 0x00, 10]

    # 这3个合起来是3帧数据，对应着req_data_1，req_data_2，req_data_3，但都不是独立的1帧数据（有的比1帧少，有的比1帧多）
    req_data_2_1 = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x04, 0x00, 0x00, 0x00, 10, 0x00, 0x01, 0x00, 0x00, 0x00, 27, 0x01, 0x10, 0x00, 0x00, 0x00, 0x0A, 20, 0x00, 11, 0x00, 12, 0x00, 13, 0x00, 14, 0x00, 15, 0x00, 16, 0x00, 17, 0x00]
    req_data_2_2 = [18, 0x00, 19, 0x00, 20, 0x00, 0x01, 0x00, 0x00]
    req_data_2_3 = [0x00, 6, 0x01, 0x03, 0x00, 0x00, 0x00, 10]

    inx = 0;  # 用来选择数据
    # 这里的主循环模拟一直在接收Socket的Modbus TCP数据
    while True:
        # 每2秒处理接收一个数据
        time.sleep(2)
        print('收到1个数据, inx={}'.format(inx))
        if inx == 0:
            service.process_data(req_data_1, callback=process_callback)
        elif inx == 1:
            service.process_data(req_data_2, callback=process_callback)
        elif inx == 2:
            service.process_data(req_data_3, callback=process_callback)
        elif inx == 3:
            service.process_data(req_data_1_1, callback=process_callback)
        elif inx == 4:
            service.process_data(req_data_1_2, callback=process_callback)
        elif inx == 5:
            service.process_data(req_data_1_3, callback=process_callback)
        elif inx == 6:
            service.process_data(req_data_2_1, callback=process_callback)
        elif inx == 7:
            service.process_data(req_data_2_2, callback=process_callback)
        elif inx == 8:
            service.process_data(req_data_2_3, callback=process_callback)
        inx = (inx + 1) % 9


if __name__ == '__main__':
    main()
