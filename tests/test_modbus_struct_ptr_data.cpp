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

unsigned short get_reg(unsigned short val) {
  return 99;
}

int main(int argc, char *arg[])
{
  printf("modbus_struct_ptr_data<unsigned char>, size=%ld\n", sizeof(modbus_struct_ptr_data<unsigned char>));
  printf("modbus_struct_ptr_data<unsigned short>, size=%ld\n", sizeof(modbus_struct_ptr_data<unsigned short>));

  // 选择寄存器数据类型为 modbus_struct_ptr_data 结构对应的操作类
  using ModbusData = ModbusStructPtrData;
  using StaticModbusData = StaticModbusStructPtrData;

  // 创建Modbus寄存器
  ModbusData modbus_data(20, 20, 20, 20);
  // 绑定寄存器操作实例到对应的静态操作类，这样就可以通过静态类操作到实例了
  // 也可以不绑定，直接通过实例操作，绑定是为了在整个程序的任何地方都能通过静态类访问
  StaticModbusData::set_modbus_data(&modbus_data);

  int start_addr = 0x00; // 该示例操作的寄存器起始地址
  int quantity = 8; // 该示例操作的寄存器个数

  unsigned short r_regs[quantity] = {0};
  // 读取保持寄存器的值
  StaticModbusData::read_holding_registers(start_addr, quantity, r_regs);
  print_datas<unsigned short>("[1] regs", r_regs, quantity);

  unsigned short w_regs[quantity] = {1, 2, 3, 4, 5, 6, 7, 8};
  for (int i = 0; i < quantity; i++) {
    // 把地址为start_addr+i的保持寄存器绑定到w_regs[i]
    StaticModbusData::get_holding_register_struct(start_addr + i)->bind_data(&w_regs[i]);
  }

  // 读取保持寄存器的值
  StaticModbusData::read_holding_registers(start_addr, quantity, r_regs);
  print_datas<unsigned short>("[2] regs", r_regs, quantity);
  print_datas<unsigned short>("[2] w_regs", w_regs, quantity);

  unsigned short w_regs_2[quantity] = {11, 12, 13, 14, 15, 16, 17, 18};
  // 写数据到保持寄存器, 由于绑定关系，会写入到w_regs
  StaticModbusData::write_holding_registers(start_addr, w_regs_2, quantity);
  // 读取保持寄存器的值
  StaticModbusData::read_holding_registers(start_addr, quantity, r_regs);
  print_datas<unsigned short>("[3] regs", r_regs, quantity);
  print_datas<unsigned short>("[3] w_regs", w_regs, quantity);

  for (int i = 0; i < quantity; i++) {
    // 为地址为start_addr+i的保持寄存器额外绑定get方法
    StaticModbusData::get_holding_register_struct(start_addr + i)->bind_get(get_reg);
  }
  print_datas<unsigned short>("[4-1] w_regs", w_regs, quantity);
  // 读取保持寄存器的值, 此时读取到的值是由绑定的get方法返回, 同时会把获取到的值更新到原始数据指向(也就是会修改w_regs的值)
  StaticModbusData::read_holding_registers(start_addr, quantity, r_regs);
  print_datas<unsigned short>("[4] regs", r_regs, quantity);
  print_datas<unsigned short>("[4-2] w_regs", w_regs, quantity);

  printf("========更改操作地址=========\n");
  // 由于上面的操作更改了数据的指向，所以寄存器的原始数据的地址不一定连续
  // 以下操作更改操作地址为0x0A, 该地址在上面的操作之外
  start_addr = 0x0A;

  StaticModbusData::read_holding_registers(start_addr, quantity, r_regs);
  print_datas<unsigned short>("[1] regs", r_regs, quantity);
  // 把地址为start_addr的保持寄存器映射到datas
  // 在没有把相同地址的寄存器绑定到别的地方的情况下，那么datas+i就是指向地址为start_addr+i的保持寄存器
  unsigned short *datas = StaticModbusData::get_holding_register_struct(start_addr)->data_ptr;
  for (int i = 0; i < quantity; i++) {
    // 修改datas[i]的值，相当于修改了对应保持寄存器的值
    datas[i] = 20 + i;
  }
  // 读取保持寄存器的值
  StaticModbusData::read_holding_registers(start_addr, quantity, r_regs);
  print_datas<unsigned short>("[2] regs", r_regs, quantity);
  print_datas<unsigned short>("[2] datas", datas, quantity);

  unsigned short datas2[quantity] = {31, 32, 33, 34, 35, 36, 37, 38};
  for (int i = 0; i < quantity; i++) {
    // 把地址为start_addr+i的保持寄存器绑定到datas2[i]
    StaticModbusData::get_holding_register_struct(start_addr + i)->bind_data(&datas2[i]);
  }
  // 读取保持寄存器的值
  StaticModbusData::read_holding_registers(start_addr, quantity, r_regs);
  print_datas<unsigned short>("[3] regs", r_regs, quantity);
  print_datas<unsigned short>("[3] datas2", datas2, quantity);
  // 由于变更了数据指向，此时datas的数据不再跟随寄存器
  print_datas<unsigned short>("[3] datas", datas, quantity);

  for (int i = 0; i < quantity; i++) {
    // 为地址为start_addr+i的保持寄存器额外绑定get方法
    StaticModbusData::get_holding_register_struct(start_addr + i)->bind_get(get_reg);
  }
  
  print_datas<unsigned short>("[4-1] datas2", datas2, quantity);
  // 读取保持寄存器的值
  StaticModbusData::read_holding_registers(start_addr, quantity, r_regs);
  print_datas<unsigned short>("[4] regs", r_regs, quantity);
  // datas2的数据会在读取绑定的寄存器时同步更新
  print_datas<unsigned short>("[4-2] datas2", datas2, quantity);
  // 由于变更了数据指向，此时datas的数据不再跟随寄存器
  print_datas<unsigned short>("[4] datas", datas, quantity);

  return 0;
}