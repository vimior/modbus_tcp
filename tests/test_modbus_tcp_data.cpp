#include <stdio.h>
#include <iostream>
#include "modbus_tcp.h"

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
  // using ModbusData = ModbusBaseData;
  using ModbusData = ModbusStructData;
  using DataService = ModbusTCP::DataService<ModbusData>;

  // init modbus data
  ModbusData modbus_data(10, 10, 10, 10);
  unsigned short regs[10] = { 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
  modbus_data.write_input_registers(0x00, regs, 10);

  ModbusTCP::DataSession session;

  // request data: read input register(0x00 ~ 0x0A)
  unsigned char req_data_1[12] = {0x00, 0x01, 0x00, 0x00, 0x00, 5, 0x01, 0x04, 0x00, 0x00, 0x00, 10};
  session.set_request_data(req_data_1, 12);
  DataService::process_session(&session, &modbus_data);
  print_datas<unsigned char>("read input register, resuest", req_data_1, 12);
  print_datas<unsigned char>("read input register, response", session.get_response_data(), session.get_response_length());

  // request data: write holding register(0x00 ~ 0x0A)
  unsigned char req_data_2[33] = {0x00, 0x01, 0x00, 0x00, 0x00, 26, 0x01, 0x10, 0x00, 0x00, 0x00, 0x0A, 20, 
    0x00, 11, 0x00, 12, 0x00, 13, 0x00, 14, 0x00, 15, 0x00, 16, 0x00, 17, 0x00, 18, 0x00, 19, 0x00, 20};
  session.set_request_data(req_data_2, 33);
  DataService::process_session(&session, &modbus_data);
  print_datas<unsigned char>("write holding register, resuest", req_data_2, 33);
  print_datas<unsigned char>("write holding register, response", session.get_response_data(), session.get_response_length());

  // request data: read holding register(0x00 ~ 0x0A)
  unsigned char req_data_3[12] = {0x00, 0x01, 0x00, 0x00, 0x00, 5, 0x01, 0x03, 0x00, 0x00, 0x00, 10};
  session.set_request_data(req_data_3, 12);
  DataService::process_session(&session, &modbus_data);
  print_datas<unsigned char>("read holding register, resuest", req_data_3, 12);
  print_datas<unsigned char>("read holding register, response", session.get_response_data(), session.get_response_length());

  return 0;
}