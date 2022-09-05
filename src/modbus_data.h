/*
 * Software License Agreement (MIT License)
 *
 * Copyright (c) 2022, Vinman, Inc.
 * All rights reserved.
 *
 * Author: Vinman <vinman.cub@gmail.com>
 */

#ifndef _MODBUS_DATA_H_
#define _MODBUS_DATA_H_

#include <functional>
#include "modbus_data_type.h"

#define MODBUS_FC_READ_COILS            0x01
#define MODBUS_FC_READ_DISCRETE_INPUTS  0x02
#define MODBUS_FC_READ_HOLDING_REGS     0x03
#define MODBUS_FC_READ_INPUT_REGS       0x04
#define MODBUS_FC_WRITE_SINGLE_COIL     0x05
#define MODBUS_FC_WRITE_SINGLE_REG      0x06
#define MODBUS_FC_WRITE_MULTIPLE_COILS  0x0F
#define MODBUS_FC_WRITE_MULTIPLE_REGS   0x10
#define MODBUS_FC_MASK_WRITE_REG        0x16
#define MODBUS_FC_WRITE_AND_READ_REGS   0x17

enum ModbusDataCode {
  MODBUS_DATA_NOT_CREATE = -1,     // ModbusData未创建
  MODBUS_NONE = 0x0,          // 正常
  MODBUS_DATA_ILLEGAL_ADDR = 0x03  // 访问的寄存器地址非法
};

typedef unsigned char uchar;
typedef unsigned short ushort;

/* Modbus数据结构 */
typedef modbus_base_data<uchar> modbus_bit_base_data;
typedef modbus_base_data<ushort> modbus_reg_base_data;
typedef modbus_struct_data<uchar> modbus_bit_struct_data;
typedef modbus_struct_data<ushort> modbus_reg_struct_data;

/* Modbus指针数据结构 */
typedef modbus_base_ptr_data<uchar> modbus_bit_base_ptr_data;
typedef modbus_base_ptr_data<ushort> modbus_reg_base_ptr_data;
typedef modbus_struct_ptr_data<uchar> modbus_bit_struct_ptr_data;
typedef modbus_struct_ptr_data<ushort> modbus_reg_struct_ptr_data;

enum BIND_FLAGS {
  GET_FUNC_FLAG = 1,
  SET_FUNC_FLAG = 2,
  DATA_FLAG = 4
};

/* Modbus数据寄存器的操作模板类 */
template <typename BIT_T = modbus_bit_base_data, typename REG_T = modbus_reg_base_data>
class ModbusDataTemplate
{
public:
  /* ModbusDataTemplate: 构造数据寄存器
   * @param coil_bit_count: 线圈状态寄存器数量
   * @param input_bit_count: 离散输入状态寄存器数量
   * @param holding_reg_count: 保持寄存器数量
   * @param input_reg_count: 输入寄存器数量
   * @param coil_bit_start_addr: 线圈状态寄存器起始地址, 默认0x00
   * @param input_bit_start_addr: 离散输入状态寄存器起始地址, 默认0x00
   * @param holding_reg_start_addr: 保持寄存器起始地址, 默认0x00
   * @param input_reg_start_addr: 输入寄存器起始地址, 默认0x00
   */
  ModbusDataTemplate(unsigned int coil_bit_count, unsigned int input_bit_count,
    unsigned int holding_reg_count, unsigned int input_reg_count, 
    unsigned int coil_bit_start_addr = 0x00, unsigned int input_bit_start_addr = 0x00,
    unsigned int holding_reg_start_addr = 0x00, unsigned int input_reg_start_addr = 0x00);
  
  /* ~ModbusDataTemplate: 销毁数据寄存器
  */
  ~ModbusDataTemplate();

  /********************** READ *********************/

  /* read_coil_bits: 从线圈状态寄存器读取数据
   * @param addr: 要读取的寄存器起始地址
   * @param quantity: 要读取的寄存器数量
   * @param bits: 存储读取到的寄存器的值的数组, 数组大小不能小于quantity
   * :return: 成功返回0
   */
  int read_coil_bits(int addr, int quantity, uchar *bits);

  /* read_input_bits: 从离散输入寄存器读取数据
    @param addr: 要读取的寄存器起始地址
    @param quantity: 要读取的寄存器数量
    @param bits: 存储读取到的寄存器的值的数组, 数组大小不能小于quantity
    :return: 成功返回0
  */
  int read_input_bits(int addr, int quantity, uchar *bits);

  /* read_holding_registers: 从保持寄存器读取数据
   * @param addr: 要读取的寄存器起始地址
   * @param quantity: 要读取的寄存器数量
   * @param regs: 存储读取到的寄存器的值的数组, 数组大小不能小于quantity
   * :return: 成功返回0
   */
  int read_holding_registers(int addr, int quantity, ushort *regs);
  
  /* read_input_registers: 从输入寄存器读取数据
   * @param addr: 要读取的寄存器起始地址
   * @param quantity: 要读取的寄存器数量
   * @param regs: 存储读取到的寄存器的值的数组, 数组大小不能小于quantity
   * :return: 成功返回0
   */
  int read_input_registers(int addr, int quantity, ushort *regs);

  /********************** WRITE *********************/

  /* write_coil_bits: 写数据到线圈状态寄存器
   * @param addr: 要写入的寄存器的起始地址
   * @param bits: 要写入到寄存器的数据数组, 数组大小不能小于quantity
   * @param quantity: 要写入的寄存器数量
   * :return: 成功返回0
   */
  int write_coil_bits(int addr, uchar *bits, int quantity);

  /* write_input_bits: 写数据到线圈状态寄存器
   * @param addr: 要写入的寄存器的起始地址
   * @param bits: 要写入到寄存器的数据数组, 数组大小不能小于quantity
   * @param quantity: 要写入的寄存器数量
   * :return: 成功返回0
   */
  int write_input_bits(int addr, uchar *bits, int quantity);
  
  /* write_holding_registers: 写数据到线圈状态寄存器
   * @param addr: 要写入的寄存器的起始地址
   * @param regs: 要写入到寄存器的数据数组, 数组大小不能小于quantity
   * @param quantity: 要写入的寄存器数量
   * :return: 成功返回0
   */
  int write_holding_registers(int addr, ushort *regs, int quantity);
  
  /* write_input_registers: 写数据到线圈状态寄存器
   * @param addr: 要写入的寄存器的起始地址
   * @param regs: 要写入到寄存器的数据数组, 数组大小不能小于quantity
   * @param quantity: 要写入的寄存器数量
   * :return: 成功返回0
   */
  int write_input_registers(int addr, ushort *regs, int quantity);

  /* mask_write_holding_register: 对保持寄存器进行掩码处理
   * 先把寄存器的值读取出来curr_val, 然后进行掩码操作(curr_val & and_mask) | (or_mask & ~and_mask), 把掩码操作后的值写回寄存器
   * @param and_mask: 进行"与"操作的掩码
   * @param or_mask: 进行"或"操作的掩码
   * :return: 成功返回0
   */
  int mask_write_holding_register(int addr, ushort and_mask, ushort or_mask);

  /* write_and_read_holding_registers: 同时对保持寄存器进行读写(先写后读)
   * @param w_addr: 要写入的寄存器的起始地址
   * @param w_regs: 要写入到寄存器的数据数组, 数组大小不能小于w_quantity
   * @param w_quantity: 要写入的寄存器数量
   * @param r_addr: 要读取的寄存器起始地址
   * @param r_quantity: 要读取的寄存器数量
   * @param r_regs: 存储读取到的寄存器的值的数组, 数组大小不能小于r_quantity
   * :return: 成功返回0
   */
  int write_and_read_holding_registers(int w_addr, ushort *w_regs, int w_quantity, int r_addr, int r_quantity, ushort *r_regs);

  /********************** GET *********************/

  /* get_coil_bit_struct: 获取指定地址的线圈状态寄存器
   * @param addr: 寄存器地址
   * :return: 成功返回寄存器指针，失败返回NULL
   */
  BIT_T* get_coil_bit_struct(int addr);
  
  /* get_input_bit_struct: 获取指定地址的离散输入状态寄存器
   * @param addr: 寄存器地址
   * :return: 成功返回寄存器指针，失败返回NULL
   */
  BIT_T* get_input_bit_struct(int addr);
  
  /* get_holding_register_struct: 获取指定地址的保持寄存器
   * @param addr: 寄存器地址
   * :return: 成功返回寄存器指针，失败返回NULL
   */
  REG_T* get_holding_register_struct(int addr);
  
  /* get_input_register_struct: 获取指定地址的输入寄存器
   * @param addr: 寄存器地址
   * :return: 成功返回寄存器指针，失败返回NULL
   */
  REG_T* get_input_register_struct(int addr);

  // /* bind_get_coil_bit: 给指定地址的线圈状态寄存器绑定额外的读方法 bind_get
  //  * @param addr: 寄存器地址
  //  * @param func: 要绑定的函数(函数指针或std::function)
  //  * :return: 成功返回0
  //  */
  // int bind_get_coil_bit(int addr, uchar(*func)(uchar))
  // {
  //   return _bind_get<BIT_T, uchar(*)(uchar)>(addr - coil_bit_start_addr_, coil_bit_count_, coil_bits_, func);
  // }
  // int bind_get_coil_bit(int addr, std::function<uchar (uchar)> func) 
  // {
  //   return _bind_get<BIT_T, std::function<uchar (uchar)>>(addr - coil_bit_start_addr_, coil_bit_count_, coil_bits_, func);
  // }

  // /* bind_set_coil_bit: 给指定地址的线圈状态寄存器绑定额外的写方法 bind_set 
  //  * @param addr: 寄存器地址
  //  * @param func: 要绑定的函数(函数指针或std::function)
  //  * :return: 成功返回0
  //  */
  // int bind_set_coil_bit(int addr, int (*func)(uchar))
  // {
  //   return _bind_set<BIT_T, int(*)(uchar)>(addr - coil_bit_start_addr_, coil_bit_count_, coil_bits_, func);
  // }
  // int bind_set_coil_bit(int addr, std::function<int (uchar)> func)
  // {
  //   return _bind_set<BIT_T, std::function<int (uchar)>>(addr - coil_bit_start_addr_, coil_bit_count_, coil_bits_, func);
  // }

  // /* bind_coil_bit_data: 修改指定地址的线圈状态寄存器原始数据的数据指向 bind_data 
  //  * @param addr: 寄存器地址
  //  * @param data: 寄存器原始数据的目标指向
  //  * :return: 成功返回0 
  //  */
  // int bind_coil_bit_data(int addr, uchar *data)
  // {
  //   return _bind_data<BIT_T, uchar*>(addr - coil_bit_start_addr_, coil_bit_count_, coil_bits_, data);
  // }

  // /* bind_get_input_bit: 给指定地址的离散输入状态寄存器绑定额外的读方法 bind_get 
  //  * @param addr: 寄存器地址
  //  * @param func: 要绑定的函数(函数指针或std::function)
  //  * :return: 成功返回0 
  //  */
  // int bind_get_input_bit(int addr, uchar(*func)(uchar))
  // {
  //   return _bind_get<BIT_T, uchar(*)(uchar)>(addr - input_bit_start_addr_, input_bit_count_, input_bits_, func);
  // }
  // int bind_get_input_bit(int addr, std::function<uchar (uchar)> func)
  // {
  //   return _bind_get<BIT_T, std::function<uchar (uchar)>>(addr - input_bit_start_addr_, input_bit_count_, input_bits_, func);
  // }

  // /* bind_set_input_bit: 给指定地址的离散输入状态寄存器绑定额外的写方法 bind_set 
  //  * @param addr: 寄存器地址
  //  * @param func: 要绑定的函数(函数指针或std::function)
  //  * :return: 成功返回0 
  //  */
  // int bind_set_input_bit(int addr, int (*func)(uchar))
  // {
  //   return _bind_set<BIT_T, int(*)(uchar)>(addr - input_bit_start_addr_, input_bit_count_, input_bits_, func);
  // }
  // int bind_set_input_bit(int addr, std::function<int (uchar)> func)
  // {
  //   return _bind_set<BIT_T, std::function<int (uchar)>>(addr - input_bit_start_addr_, input_bit_count_, input_bits_, func);
  // }

  // /* bind_input_bit_data: 修改指定地址的离散输入状态寄存器原始数据的数据指向 bind_data 
  //  * @param addr: 寄存器地址
  //  * @param data: 寄存器原始数据的目标指向
  //  * :return: 成功返回0 
  //  */
  // int bind_input_bit_data(int addr, uchar *data)
  // {
  //   return _bind_data<BIT_T, uchar*>(addr - input_bit_start_addr_, input_bit_count_, input_bits_, data);
  // }

  // /* bind_get_holding_register: 给指定地址的保持寄存器绑定额外的读方法 bind_get 
  //  * @param addr: 寄存器地址
  //  * @param func: 要绑定的函数(函数指针或std::function)
  //  * :return: 成功返回0 
  //  */
  // int bind_get_holding_register(int addr, ushort(*func)(ushort))
  // {
  //   return _bind_get<REG_T, ushort(*)(ushort)>(addr - holding_reg_start_addr_, holding_reg_count_, holding_regs_, func);
  // }
  // int bind_get_holding_register(int addr, std::function<ushort (ushort)> func)
  // {
  //   return _bind_get<REG_T, std::function<ushort (ushort)>>(addr - holding_reg_start_addr_, holding_reg_count_, holding_regs_, func);
  // }

  // /* bind_set_holding_register: 给指定地址的保持寄存器绑定额外的写方法 bind_set 
  //  * @param addr: 寄存器地址
  //  * @param func: 要绑定的函数(函数指针或std::function)
  //  * :return: 成功返回0 
  //  */
  // int bind_set_holding_register(int addr, int (*func)(ushort))
  // {
  //   return _bind_set<REG_T, int(*)(ushort)>(addr - holding_reg_start_addr_, holding_reg_count_, holding_regs_, func);
  // }
  // int bind_set_holding_register(int addr, std::function<int (ushort)> func)
  // {
  //   return _bind_set<REG_T, std::function<int (ushort)>>(addr - holding_reg_start_addr_, holding_reg_count_, holding_regs_, func);
  // }

  // /* bind_holding_register_data: 修改指定地址的保持寄存器原始数据的数据指向 bind_data 
  //  * @param addr: 寄存器地址
  //  * @param data: 寄存器原始数据的目标指向
  //  * :return: 成功返回0 
  //  */
  // int bind_holding_register_data(int addr, ushort *data)
  // {
  //   return _bind_data<REG_T, ushort*>(addr - holding_reg_start_addr_, holding_reg_count_, holding_regs_, data);
  // }

  // /* bind_get_input_register: 给指定地址的输入寄存器绑定额外的读方法 bind_get 
  //  * @param addr: 寄存器地址
  //  * @param func: 要绑定的函数(函数指针或std::function)
  //  * :return: 成功返回0 
  //  */
  // int bind_get_input_register(int addr, ushort(*func)(ushort))
  // {
  //   return _bind_get<REG_T, ushort(*)(ushort)>(addr - input_reg_start_addr_, input_reg_count_, input_regs_, func);
  // }
  // int bind_get_input_register(int addr, std::function<ushort (ushort)> func)
  // {
  //   return _bind_get<REG_T, std::function<ushort (ushort)>>(addr - input_reg_start_addr_, input_reg_count_, input_regs_, func);
  // }

  // /* bind_set_input_register: 给指定地址的输入寄存器绑定额外的写方法 bind_set 
  //  * @param addr: 寄存器地址
  //  * @param func: 要绑定的函数(函数指针或std::function)
  //  * :return: 成功返回0 
  //  */
  // int bind_set_input_register(int addr, int (*func)(ushort))
  // {
  //   return _bind_set<REG_T, int(*)(ushort)>(addr - input_reg_start_addr_, input_reg_count_, input_regs_, func);
  // }
  // int bind_set_input_register(int addr, std::function<int (ushort)> func)
  // {
  //   return _bind_set<REG_T, std::function<int (ushort)>>(addr - input_reg_start_addr_, input_reg_count_, input_regs_, func);
  // }

  // /* bind_input_register_data: 修改指定地址的输入寄存器原始数据的数据指向 bind_data 
  //  * @param addr: 寄存器地址
  //  * @param data: 寄存器原始数据的目标指向
  //  * :return: 成功返回0 
  //  */
  // int bind_input_register_data(int addr, ushort *data)
  // {
  //   return _bind_data<REG_T, ushort*>(addr - input_reg_start_addr_, input_reg_count_, input_regs_, data);
  // }

private:
  template <typename SOURCES_T, typename PARAM_T>
  int _bind_get(int inx, int count, SOURCES_T *sources, PARAM_T param);
  template <typename SOURCES_T, typename PARAM_T>
  int _bind_set(int inx, int count, SOURCES_T *sources, PARAM_T param);
  template <typename SOURCES_T, typename PARAM_T>
  int _bind_data(int inx, int count, SOURCES_T *sources, PARAM_T param);

private:
  unsigned int coil_bit_start_addr_;    // 线圈状态寄存器起始地址
  unsigned int input_bit_start_addr_;   // 离散输入状态寄存器起始地址
  unsigned int holding_reg_start_addr_; // 保持寄存器起始地址
  unsigned int input_reg_start_addr_;   // 输入寄存器起始地址
  unsigned int coil_bit_count_;     // 线圈状态寄存器数量
  unsigned int input_bit_count_;    // 离散输入状态寄存器数量
  unsigned int holding_reg_count_;  // 保持寄存器数量
  unsigned int input_reg_count_;    // 输入寄存器数量
  BIT_T *coil_bits_;      // 线圈状态寄存器数组
  BIT_T *input_bits_;     // 离散输入状态寄存器数组
  REG_T *holding_regs_;  // 保持寄存器数组
  REG_T *input_regs_;    // 输入寄存器数组
  uchar *coil_bits_data_;
  uchar *input_bits_data_;
  ushort *holding_regs_data_;
  ushort *input_regs_data_;
};

/* Modbus数据寄存器的静态操作模板类 */
template <typename BIT_T = modbus_bit_base_data, typename REG_T = modbus_reg_base_data>
class StaticModbusDataTemplate 
{
public:
  static void set_modbus_data(ModbusDataTemplate<BIT_T, REG_T>* modbus_data);
  static ModbusDataTemplate<BIT_T, REG_T>* get_modbus_data(void);

  static int read_coil_bits(int addr, int quantity, uchar *bits);
  static int read_input_bits(int addr, int quantity, uchar *bits);
  static int read_holding_registers(int addr, int quantity, ushort *regs);
  static int read_input_registers(int addr, int quantity, ushort *regs);

  static int write_coil_bits(int addr, uchar *bits, int quantity);
  static int write_input_bits(int addr, uchar *bits, int quantity);
  static int write_holding_registers(int addr, ushort *regs, int quantity);
  static int write_input_registers(int addr, ushort *regs, int quantity);

  static int mask_write_holding_register(int addr, ushort and_mask, ushort or_mask);
  static int write_and_read_holding_registers(int w_addr, ushort *w_regs, int w_quantity, int r_addr, int r_quantity, ushort *r_regs);

  static BIT_T* get_coil_bit_struct(int addr);
  static BIT_T* get_input_bit_struct(int addr);
  static REG_T* get_holding_register_struct(int addr);
  static REG_T* get_input_register_struct(int addr);

  // static int bind_get_coil_bit(int addr, uchar(*func)(uchar)) // bind_get
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_get_coil_bit(addr, func) : MODBUS_DATA_NOT_CREATE;
  // }
  // static int bind_get_coil_bit(int addr, std::function<uchar (uchar)> func) // bind_get
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_get_coil_bit(addr, func) : MODBUS_DATA_NOT_CREATE;
  // }
  // static int bind_set_coil_bit(int addr, int (*func)(uchar)) // bind_set
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_set_coil_bit(addr, func) : MODBUS_DATA_NOT_CREATE;
  // }
  // static int bind_set_coil_bit(int addr, std::function<int (uchar)> func) // bind_set
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_set_coil_bit(addr, func) : MODBUS_DATA_NOT_CREATE;
  // }
  // static int bind_coil_bit_data(int addr, uchar *data) // bind_data
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_coil_bit_data(addr, data) : MODBUS_DATA_NOT_CREATE;
  // }

  // static int bind_get_input_bit(int addr, uchar(*func)(uchar)) // bind_get
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_get_input_bit(addr, func) : MODBUS_DATA_NOT_CREATE;
  // }
  // static int bind_get_input_bit(int addr, std::function<uchar (uchar)> func) // bind_get
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_get_input_bit(addr, func) : MODBUS_DATA_NOT_CREATE;
  // }
  // static int bind_set_input_bit(int addr, int (*func)(uchar)) // bind_set
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_set_input_bit(addr, func) : MODBUS_DATA_NOT_CREATE;
  // }
  // static int bind_set_input_bit(int addr, std::function<int (uchar)> func) // bind_set
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_set_input_bit(addr, func) : MODBUS_DATA_NOT_CREATE;
  // }
  // static int bind_input_bit_data(int addr, uchar *data) // bind_data
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_input_bit_data(addr, data) : MODBUS_DATA_NOT_CREATE;
  // }

  // static int bind_get_holding_register(int addr, ushort(*func)(ushort)) // bind_get
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_get_holding_register(addr, func) : MODBUS_DATA_NOT_CREATE;
  // }
  // static int bind_get_holding_register(int addr, std::function<ushort (ushort)> func) // bind_get
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_get_holding_register(addr, func) : MODBUS_DATA_NOT_CREATE;
  // }
  // static int bind_set_holding_register(int addr, int (*func)(ushort)) // bind_set
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_set_holding_register(addr, func) : MODBUS_DATA_NOT_CREATE;
  // }
  // static int bind_set_holding_register(int addr, std::function<int (ushort)> func) // bind_set
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_set_holding_register(addr, func) : MODBUS_DATA_NOT_CREATE;
  // }
  // static int bind_holding_register_data(int addr, ushort *data) // bind_data
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_holding_register_data(addr, data) : MODBUS_DATA_NOT_CREATE;
  // }

  // static int bind_get_input_register(int addr, ushort(*func)(ushort)) // bind_get
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_get_input_register(addr, func) : MODBUS_DATA_NOT_CREATE;
  // }
  // static int bind_get_input_register(int addr, std::function<ushort (ushort)> func) // bind_get
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_get_input_register(addr, func) : MODBUS_DATA_NOT_CREATE;
  // }
  // static int bind_set_input_register(int addr, int (*func)(ushort)) // bind_set
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_set_input_register(addr, func) : MODBUS_DATA_NOT_CREATE;
  // }
  // static int bind_set_input_register(int addr, std::function<int (ushort)> func) // bind_set
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_set_input_register(addr, func) : MODBUS_DATA_NOT_CREATE;
  // }
  // static int bind_input_register_data(int addr, ushort *data) // bind_data
  // {
  //   return modbus_data_ != NULL ? modbus_data_->bind_input_register_data(addr, data) : MODBUS_DATA_NOT_CREATE;
  // }

private:
  static ModbusDataTemplate<BIT_T, REG_T> *modbus_data_;
};

// Modbus数据寄存器的操作类(基本型数据结构), 不额外占用空间开销, 但不支持绑定额外的读写方法
typedef ModbusDataTemplate<modbus_bit_base_data, modbus_reg_base_data> ModbusBaseData;
// Modbus数据寄存器的静态操作类(基本型数据结构), 不额外占用空间, 但不支持绑定额外的读写方法
typedef StaticModbusDataTemplate<modbus_bit_base_data, modbus_reg_base_data> StaticModbusBaseData;

// Modbus数据寄存器的操作类(扩展型数据结构), 支持绑定额外的读写方法, 但会占用额外的空间开销
typedef ModbusDataTemplate<modbus_bit_struct_data, modbus_reg_struct_data> ModbusStructData;
// Modbus数据寄存器的静态操作类(扩展型数据结构), 支持绑定额外的读写方法, 但会占用额外的空间开销
typedef StaticModbusDataTemplate<modbus_bit_struct_data, modbus_reg_struct_data> StaticModbusStructData;

// Modbus数据寄存器的操作类(基本型指针数据结构), 不额外占用空间开销, 但不支持绑定额外的读写方法, 支持修改原始数据的地址指向
typedef ModbusDataTemplate<modbus_bit_base_ptr_data, modbus_reg_base_ptr_data> ModbusBasePtrData;
// Modbus数据寄存器的静态操作类(基本型指针数据结构), 不额外占用空间, 但不支持绑定额外的读写方法, 支持修改原始数据的地址指向
typedef StaticModbusDataTemplate<modbus_bit_base_ptr_data, modbus_reg_base_ptr_data> StaticModbusBasePtrData;

// Modbus数据寄存器的操作类(扩展型指针数据结构), 支持绑定额外的读写方法, 但会占用额外的空间开销, 支持修改原始数据的地址指向
typedef ModbusDataTemplate<modbus_bit_struct_ptr_data, modbus_reg_struct_ptr_data> ModbusStructPtrData;
// Modbus数据寄存器的静态操作类(扩展型指针数据结构), 支持绑定额外的读写方法, 但会占用额外的空间开销, 支持修改原始数据的地址指向
typedef StaticModbusDataTemplate<modbus_bit_struct_ptr_data, modbus_reg_struct_ptr_data> StaticModbusStructPtrData;


// 混搭
// ModbusDataTemplate<modbus_bit_base_data, modbus_reg_struct_data>
// StaticModbusDataTemplate<modbus_bit_base_data, modbus_reg_struct_data>
// ModbusDataTemplate<modbus_bit_base_data, modbus_reg_base_ptr_data>
// StaticModbusDataTemplate<modbus_bit_base_data, modbus_reg_base_ptr_data>
// ModbusDataTemplate<modbus_bit_base_data, modbus_reg_struct_ptr_data>
// StaticModbusDataTemplate<modbus_bit_base_data, modbus_reg_struct_ptr_data>

// ModbusDataTemplate<modbus_bit_base_ptr_data, modbus_reg_base_data>
// StaticModbusDataTemplate<modbus_bit_base_ptr_data, modbus_reg_base_data>
// ModbusDataTemplate<modbus_bit_base_ptr_data, modbus_reg_struct_data>
// StaticModbusDataTemplate<modbus_bit_base_ptr_data, modbus_reg_struct_data>
// ModbusDataTemplate<modbus_bit_base_ptr_data, modbus_reg_struct_ptr_data>
// StaticModbusDataTemplate<modbus_bit_base_ptr_data, modbus_reg_struct_ptr_data>

// ModbusDataTemplate<modbus_bit_struct_data, modbus_reg_base_data>
// StaticModbusDataTemplate<modbus_bit_struct_data, modbus_reg_base_data>
// ModbusDataTemplate<modbus_bit_struct_data, modbus_reg_base_ptr_data>
// StaticModbusDataTemplate<modbus_bit_struct_data, modbus_reg_base_ptr_data>
// ModbusDataTemplate<modbus_bit_struct_data, modbus_reg_struct_ptr_data>
// StaticModbusDataTemplate<modbus_bit_struct_data, modbus_reg_struct_ptr_data>

// ModbusDataTemplate<modbus_bit_struct_ptr_data, modbus_reg_base_data>
// StaticModbusDataTemplate<modbus_bit_struct_ptr_data, modbus_reg_base_data>
// ModbusDataTemplate<modbus_bit_struct_ptr_data, modbus_reg_base_ptr_data>
// StaticModbusDataTemplate<modbus_bit_struct_ptr_data, modbus_reg_base_ptr_data>
// ModbusDataTemplate<modbus_bit_struct_ptr_data, modbus_reg_struct_data>
// StaticModbusDataTemplate<modbus_bit_struct_ptr_data, modbus_reg_struct_data>


#endif // _MODBUS_DATA_H_