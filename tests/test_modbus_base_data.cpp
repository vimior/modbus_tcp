#include <stdio.h>
#include <iostream>
#include "modbus_data.h"

template <class T>
void print_datas(std::string str, T *data, int length)
{
  printf("%s: ", str.c_str());
  for (int i = 0; i < length; i++) {
    printf("%d ", data[i]);
  }
  printf("\n");
}

int main(int argc, char *arg[])
{
  printf("modbus_base_data<unsigned char>, size=%ld\n", sizeof(modbus_base_data<unsigned char>));
  printf("modbus_base_data<unsigned short>, size=%ld\n", sizeof(modbus_base_data<unsigned short>));

  // 选择寄存器数据类型为 modbus_base_data 结构对应的操作类
  using ModbusData = ModbusBaseData;
  using StaticModbusData = StaticModbusBaseData;

  // 创建Modbus寄存器
  ModbusData modbus_data(10, 10, 10, 10);
  // 绑定寄存器操作实例到对应的静态操作类，这样就可以通过静态类操作到实例了
  // 也可以不绑定，直接通过实例操作，绑定是为了在整个程序的任何地方都能通过静态类访问
  StaticModbusData::set_modbus_data(&modbus_data);

  int start_addr = 0x00; // 该示例操作的寄存器起始地址
  int quantity = 8; // 该示例操作的寄存器个数

  unsigned char r_bits[quantity] = {0};
  // 读取线圈状态寄存器
  StaticModbusData::read_coil_bits(start_addr, quantity, r_bits);
  print_datas<unsigned char>("[1] bits", r_bits, quantity);

  unsigned char w_bits[quantity] = {0, 1, 0, 1, 0, 1, 0, 1};
  // 写数据到线圈状态寄存器
  StaticModbusData::write_coil_bits(start_addr, w_bits, quantity);

  // 读取线圈状态寄存器
  StaticModbusData::read_coil_bits(start_addr, quantity, r_bits);
  print_datas<unsigned char>("[2] bits", r_bits, quantity);

  unsigned short r_regs[quantity] = {0};
  // 读取保持寄存器
  StaticModbusData::read_holding_registers(start_addr, quantity, r_regs);
  print_datas<unsigned short>("[1] regs", r_regs, quantity);

  unsigned short w_regs[quantity] = {1, 2, 3, 4, 5, 6, 7, 8};
  // 写数据到保持寄存器
  StaticModbusData::write_holding_registers(start_addr, w_regs, quantity);

  // 读取保持寄存器
  StaticModbusData::read_holding_registers(start_addr, quantity, r_regs);
  print_datas<unsigned short>("[2] regs", r_regs, quantity);

  return 0;
}