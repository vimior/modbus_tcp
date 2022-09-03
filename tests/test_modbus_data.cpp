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
  // init modbus data
  ModbusData modbus_data(10, 10, 10, 10);
  // set the modbus data instance to static
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

  unsigned short r_regs[8] = {0};
  unsigned short w_regs[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  // read holding registers
  StaticModbusData::read_holding_registers(0x00, 8, r_regs);
  print_datas<unsigned short>("[1] regs", r_regs, 8);

  // write holding registers
  StaticModbusData::write_holding_registers(0x00, w_regs, 8);

  // read holding registers
  StaticModbusData::read_holding_registers(0x00, 8, r_regs);
  print_datas<unsigned short>("[2] regs", r_regs, 8);

  return 0;
}