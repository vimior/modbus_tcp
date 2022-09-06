#include <stdio.h>
#include <iostream>
#include "modbus_tcp_data.h"

template <class T>
void print_datas(std::string str, const T *data, const int length)
{
  printf("%s: ", str.c_str());
  for (int i = 0; i < length; i++) {
    printf("%d ", data[i]);
  }
  printf("\n");
}

// void callback(ModbusTCP::DataSession *s) {
//   print_datas<unsigned char>("request", s->get_request_data(), s->get_request_length());
//   print_datas<unsigned char>("response", s->get_response_data(), s->get_response_length());
// }

void callback(const unsigned char *req, const int req_len, const unsigned char *res, const int res_len) {
  print_datas<unsigned char>("request", req, req_len);
  print_datas<unsigned char>("response", res, res_len);
}

int main(int argc, char *arg[])
{
  // 选择寄存器数据类型为 modbus_struct_data 结构对应的操作类
  using ModbusData = ModbusStructData;
  using DataService = ModbusTCP::DataService<ModbusData>;

  // 创建Modbus寄存器
  ModbusData modbus_data(10, 10, 10, 10);
  unsigned short regs[10] = { 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
  modbus_data.write_input_registers(0x00, regs, 10);

  ModbusTCP::DataSession session;

  // 模拟请求数据(0x04): 从地址为0x00开始读取10个输入寄存器的数据
  unsigned char req_data_1[12] = {0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x04, 0x00, 0x00, 0x00, 10};
  session.set_request_data(req_data_1, 12);
  DataService::process_session(&session, &modbus_data);
  print_datas<unsigned char>("read input register, resuest", req_data_1, 12);
  print_datas<unsigned char>("read input register, response", session.get_response_data(), session.get_response_length());

  // 模拟请求数据(0x10): 往地址为0x00开始的10个保持寄存器写入数据
  unsigned char req_data_2[33] = {0x00, 0x01, 0x00, 0x00, 0x00, 27, 0x01, 0x10, 0x00, 0x00, 0x00, 0x0A, 20, 
    0x00, 11, 0x00, 12, 0x00, 13, 0x00, 14, 0x00, 15, 0x00, 16, 0x00, 17, 0x00, 18, 0x00, 19, 0x00, 20};
  session.set_request_data(req_data_2, 33);
  DataService::process_session(&session, &modbus_data);
  print_datas<unsigned char>("write holding register, resuest", req_data_2, 33);
  print_datas<unsigned char>("write holding register, response", session.get_response_data(), session.get_response_length());

  // 模拟请求数据(0x10): 从地址为0x00开始读取10个保持寄存器的数据
  unsigned char req_data_3[12] = {0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x03, 0x00, 0x00, 0x00, 10};
  session.set_request_data(req_data_3, 12);
  DataService::process_session(&session, &modbus_data);
  print_datas<unsigned char>("read holding register, resuest", req_data_3, 12);
  print_datas<unsigned char>("read holding register, response", session.get_response_data(), session.get_response_length());

  printf("*******************实例化处理粘包问题**********************\n");
  DataService service(&modbus_data);

  printf("以下是一帧完整的Modbus TCP请求分成了三块\n");
  unsigned char req_data_1_1[5] =  {0x00, 0x01, 0x00, 0x00, 0x00 };
  unsigned char req_data_1_2[4] =  {6, 0x01, 0x04, 0x00};
  unsigned char req_data_1_3[3] =  {0x00, 0x00, 10};

  service.process_data(req_data_1_1, 5, callback);
  service.process_data(req_data_1_2, 4, callback);
  service.process_data(req_data_1_3, 3, callback);

  printf("以下是三帧完整的Modbus TCP数据，但是帧与帧之间并没有分开\n");
  unsigned char req_data_2_1[40] =  { 0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x04, 0x00, 0x00, 0x00, 10, 0x00, 0x01, 0x00, 0x00, 0x00, 27, 0x01, 0x10, 0x00, 0x00, 0x00, 0x0A, 20, 
    0x00, 11, 0x00, 12, 0x00, 13, 0x00, 14, 0x00, 15, 0x00, 16, 0x00, 17, 0x00 };
  unsigned char req_data_2_2[9] =  {18, 0x00, 19, 0x00, 20, 0x00, 0x01, 0x00, 0x00};
  unsigned char req_data_2_3[8] =  {0x00, 6, 0x01, 0x03, 0x00, 0x00, 0x00, 10};

  service.process_data(req_data_2_1, 40, callback);
  service.process_data(req_data_2_2, 9, callback);
  service.process_data(req_data_2_3, 8, callback);

  return 0;
}