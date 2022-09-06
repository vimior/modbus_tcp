#include <stdio.h>
#include <iostream>
#include "modbus_data.h"

template <class T>
void print_datas(std::string str, const T *data, int length)
{
  printf("%s: ", str.c_str());
  for (int i = 0; i < length; i++) {
    printf("%d ", data[i]);
  }
  printf("\n");
}

int main(int argc, char *arg[])
{
  printf("modbus_base_ptr_data<unsigned char>, size=%ld\n", sizeof(modbus_base_ptr_data<unsigned char>));
  printf("modbus_base_ptr_data<unsigned short>, size=%ld\n", sizeof(modbus_base_ptr_data<unsigned short>));

  // 选择寄存器数据类型为 modbus_base_ptr_data 结构对应的操作类
  using ModbusData = ModbusBasePtrData;
  using StaticModbusData = StaticModbusBasePtrData;

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
  for (int i = 0; i < quantity; i++) {
    // 把地址为start_addr+i的线圈状态寄存器绑定到w_bits[i]
    StaticModbusData::get_coil_bit_struct(start_addr + i)->bind_data(&w_bits[i]);
  }

  // 读取线圈状态寄存器, 此时读取到的实际是w_bits的值
  StaticModbusData::read_coil_bits(start_addr, quantity, r_bits);
  print_datas<unsigned char>("[2] bits", r_bits, quantity);
  print_datas<unsigned char>("[2] w_bits", w_bits, quantity);

  unsigned char w_bits_2[quantity] = {1, 0, 1, 0, 1, 0, 1, 0};
  // 写数据到线圈状态寄存器, 由于绑定关系，会写入到w_bits
  StaticModbusData::write_coil_bits(start_addr, w_bits_2, quantity);
  // 读取线圈状态寄存器
  StaticModbusData::read_coil_bits(start_addr, quantity, r_bits);
  print_datas<unsigned char>("[3] bits", r_bits, quantity);
  print_datas<unsigned char>("[3] w_bits", w_bits, quantity);

  unsigned short r_regs[quantity] = {0};
  // 读取保持寄存器的值
  StaticModbusData::read_holding_registers(start_addr, quantity, r_regs);
  print_datas<unsigned short>("[1] regs", r_regs, quantity);

  // 把地址为start_addr的保持寄存器映射到w_regs
  // 在没有把相同地址的寄存器绑定到别的地方的情况下，那么w_regs+i就是指向地址为start_addr+i的保持寄存器
  unsigned short *w_regs = StaticModbusData::get_holding_register_struct(start_addr)->data_ptr;
  for (int i = 0; i < quantity; i++) {
    // 修改w_regs[i]的值，相当于修改了对应保持寄存器的值
    w_regs[i] = 10 + i;
  }
  // 读取保持寄存器的值
  StaticModbusData::read_holding_registers(start_addr, quantity, r_regs);
  print_datas<unsigned short>("[2] regs", r_regs, quantity);
  print_datas<unsigned short>("[2] w_regs", w_regs, quantity);

  return 0;
}