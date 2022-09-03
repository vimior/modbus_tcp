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

unsigned char get_bit(unsigned char val) {
  return 11; 
}

unsigned short get_reg(unsigned short val) {
  return 99;
}

int set_reg(unsigned short val) {
  printf("set_reg: %d\n", val);
  return 0;
}

int main(int argc, char *arg[])
{
  printf("modbus_base_data<unsigned char>, size=%ld\n", sizeof(modbus_base_data<unsigned char>));
  printf("modbus_base_data<unsigned short>, size=%ld\n", sizeof(modbus_base_data<unsigned short>));

  // select ModbusData type
  using ModbusData = ModbusBaseData;
  using StaticModbusData = StaticModbusBaseData;

  // init modbus data
  ModbusData modbus_data(10, 10, 10, 10);
  // set the instance associated with the `StaticModbusData`
  // at this time, the static method of the `StaticModbusData` is equivalent to calling the method of the `modbus_data`
  StaticModbusData::set_modbus_data(&modbus_data);

  unsigned char r_bits[8] = {0};
  unsigned char w_bits[8] = {0, 1, 0, 1, 0, 1, 0, 1};
  // read coil bits
  StaticModbusData::read_coil_bits(0x00, 8, r_bits);
  print_datas<unsigned char>("[1] bits", r_bits, 8);

  // write coil bits
  StaticModbusData::write_coil_bits(0x00, w_bits, 8);

  // read coil bits
  StaticModbusData::read_coil_bits(0x00, 8, r_bits);
  print_datas<unsigned char>("[2] bits", r_bits, 8);

  // bind the get function: not available
  for (int i = 0; i < 8; i++) {
    StaticModbusData::get_coil_bit_struct(i)->bind_get(get_bit);
  }
  // read coil bits
  StaticModbusData::read_coil_bits(0x00, 8, r_bits);
  print_datas<unsigned char>("[3] bits", r_bits, 8);


  unsigned short r_regs[8] = {0};
  unsigned short w_regs[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  // read holding registers
  StaticModbusData::read_holding_registers(0x00, 8, r_regs);
  print_datas<unsigned short>("[1] regs", r_regs, 8);

  // bind the set function: not available
  for (int i = 0; i < 8; i++) {
    StaticModbusData::get_holding_register_struct(i)->bind_set(set_reg);
  }
  // write holding registers
  StaticModbusData::write_holding_registers(0x00, w_regs, 8);

  // read holding registers
  StaticModbusData::read_holding_registers(0x00, 8, r_regs);
  print_datas<unsigned short>("[2] regs", r_regs, 8);

  // bind the get function: not available
  for (int i = 0; i < 8; i++) {
    StaticModbusData::get_holding_register_struct(i)->bind_get(get_reg);
  }
  // read holding registers
  StaticModbusData::read_holding_registers(0x00, 8, r_regs);
  print_datas<unsigned short>("[3] regs", r_regs, 8);

  return 0;
}