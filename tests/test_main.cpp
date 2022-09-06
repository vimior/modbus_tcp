#include <stdio.h>
#include <iostream>
#include <thread>
#include "modbus_tcp_data.h"

// 选择寄存器数据类型为 modbus_struct_data 结构对应的操作类
using ModbusData = ModbusStructData;
using StaticModbusData = StaticModbusStructData;

template <class T>
void print_datas(std::string str, const T *data, int length)
{
  printf("%s: ", str.c_str());
  for (int i = 0; i < length; i++) {
    printf("%d ", data[i]);
  }
  printf("\n");
}

void callback(const unsigned char *req, const int req_len, const unsigned char *res, const int res_len) {
  print_datas<unsigned char>("request", req, req_len);
  print_datas<unsigned char>("response", res, res_len);
}

unsigned short get_reg(unsigned short val) {
  return 99;
}

int set_reg(unsigned short val) {
  return -1;
}

static void thread1_handle_(void *arg) {
	printf("======线程 1 启动=====\n");
  // 10秒后每隔5s主动修改地址从0x00开始的10个输入寄存器的数据
  unsigned short val = 1;
  std::this_thread::sleep_for(std::chrono::seconds(10));
  while (1) {
    for (int i = 0; i < 10; i++) {
      StaticModbusData::get_input_register_struct(i)->set_data(val + i);
    }
    printf("======刚刚修改了地址为0x00开始的10个输入寄存器的值======");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    val = (val + 10) % 65535;
  }
}

static void thread2_handle_(void *arg) {
	printf("======线程 2 启动=====\n");
  // 60秒后给地址为0x00开始的10个保持寄存器的绑定读方法
  std::this_thread::sleep_for(std::chrono::seconds(60));
  for (int i = 0; i < 10; i++) {
    StaticModbusData::get_holding_register_struct(i)->bind_get(get_reg);
  }
  printf("=====给地址为0x00开始的10个保持寄存器绑定了get方法, 之后获取这些地址的保持寄存器得到的都是99=====\n");
}

static void thread3_handle_(void *arg) {
	printf("======线程 3 启动=====\n");
  // 30秒后给地址为0x00开始的10个保持寄存器的绑定写方法
  std::this_thread::sleep_for(std::chrono::seconds(30));
  for (int i = 0; i < 10; i++) {
    StaticModbusData::get_holding_register_struct(i)->bind_set(set_reg);
  }
  printf("=====给地址为0x00开始的10个保持寄存器绑定了set方法, 之后写这些地址的保持寄存器都会调用这个set方法=====\n");
  // 由于绑定的set_reg返回值不是0，所以设置是失败的
  // 为了区分，这里把这10个寄存器的值设置为1
  for (int i = 0; i < 10; i++) {
    StaticModbusData::get_holding_register_struct(i)->set_data(22);
  }
  printf("=====为了区分, 这里把这10个保持寄存器的值都修改为22=====\n");
  
  printf("=====但由于绑定的方法返回值不为0, 所以所有写入都是无效的, 往后无论怎么写入, 读取到的都是22=====\n");
}

int main(int argc, char *arg[])
{
  // 创建Modbus寄存器, 四种寄存器各100个
  ModbusData modbus_data(100, 100, 100, 100);
  // 绑定寄存器操作实例到对应的静态操作类，这样就可以通过静态类操作到实例了
  // 也可以不绑定，直接通过实例操作，绑定是为了在整个程序的任何地方都能通过静态类访问
  StaticModbusData::set_modbus_data(&modbus_data);

  int val = 1;
  // 这里创建几个线程模拟实际程序的使用
  std::thread th1(thread1_handle_, &val);
  std::thread th2(thread2_handle_, &val);
  std::thread th3(thread3_handle_, &val);

  ModbusTCP::DataService<ModbusData> service(&modbus_data);
  
  // 这3个都是完整的数据
  // 模拟请求数据(0x04): 从地址为0x00开始读取10个输入寄存器的数据
  unsigned char req_data_1[12] = {0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x04, 0x00, 0x00, 0x00, 10};
  // 模拟请求数据(0x10): 往地址为0x00开始的10个保持寄存器写入数据
  unsigned char req_data_2[33] = {0x00, 0x01, 0x00, 0x00, 0x00, 27, 0x01, 0x10, 0x00, 0x00, 0x00, 0x0A, 20, 
    0x00, 11, 0x00, 12, 0x00, 13, 0x00, 14, 0x00, 15, 0x00, 16, 0x00, 17, 0x00, 18, 0x00, 19, 0x00, 20};
  // 模拟请求数据(0x10): 从地址为0x00开始读取10个保持寄存器的数据
  unsigned char req_data_3[12] = {0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x03, 0x00, 0x00, 0x00, 10};
  
  // 这3个合起来才是一帧完整的数据，相当于req_data_1
  unsigned char req_data_1_1[5] =  {0x00, 0x01, 0x00, 0x00, 0x00 };
  unsigned char req_data_1_2[4] =  {6, 0x01, 0x04, 0x00};
  unsigned char req_data_1_3[3] =  {0x00, 0x00, 10};

  // 这3个合起来是3帧数据，对应着req_data_1，req_data_2，req_data_3，但都不是独立的1帧数据（有的比1帧少，有的比1帧多）
  unsigned char req_data_2_1[40] =  { 0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x04, 0x00, 0x00, 0x00, 10, 0x00, 0x01, 0x00, 0x00, 0x00, 27, 0x01, 0x10, 0x00, 0x00, 0x00, 0x0A, 20, 
    0x00, 11, 0x00, 12, 0x00, 13, 0x00, 14, 0x00, 15, 0x00, 16, 0x00, 17, 0x00 };
  unsigned char req_data_2_2[9] =  {18, 0x00, 19, 0x00, 20, 0x00, 0x01, 0x00, 0x00};
  unsigned char req_data_2_3[8] =  {0x00, 6, 0x01, 0x03, 0x00, 0x00, 0x00, 10};

  int inx = 0; // 用来选择数据
  // 这里的主循环模拟一直在接收Socket的Modbus TCP数据
  while (1)
  {
    // 每2秒处理接收一个数据
    std::this_thread::sleep_for(std::chrono::seconds(2));
    printf("收到1个数据, inx=%d\n", inx);
    switch (inx)
    {
      case 0:
        service.process_data(req_data_1, 12, callback);
        break;
      case 1:
        service.process_data(req_data_2, 33, callback);
        break;
      case 2:
        service.process_data(req_data_3, 12, callback);
        break;
      case 3:
        service.process_data(req_data_1_1, 5, callback);
        break;
      case 4:
        service.process_data(req_data_1_2, 4, callback);
        break;
      case 5:
        service.process_data(req_data_1_3, 3, callback);
        break;
      case 6:
        service.process_data(req_data_2_1, 40, callback);
        break;
      case 7:
        service.process_data(req_data_2_2, 9, callback);
        break;
      case 8:
        service.process_data(req_data_2_3, 8, callback);
        break;
      default:
        break;
    }
    inx = (inx + 1) % 9;
  }

  return 0;
}