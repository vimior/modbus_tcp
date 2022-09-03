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

#ifndef ON
#define ON 1
#endif

#ifndef OFF
#define OFF 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

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

template <class T>
struct modbus_base_data {
  modbus_base_data() { data = 0; }

  /* get: 数据的读操作 */
  T get() { return get_data(); }

  /* set: 数据的写操作 */
  int set(T val) { return set_data(val); }

  /* get_data: 直接获取寄存器的值 */
  T get_data() { return data; }

  /* set_data: 直接设置寄存器的值 */
  int set_data(T val) { data = val; return 0; }

private:
  T data;
};

enum ModbusDataCode {
  MODBUS_DATA_NOT_CREATE = -1,     // ModbusData未创建
  MODBUS_NONE = 0x0,          // 正常
  MODBUS_DATA_ILLEGAL_ADDR = 0x03  // 访问的寄存器地址非法
};

/* Modbus数据寄存器的操作类 */
class ModbusData
{
public:
  /* ModbusData: 构造数据寄存器
    @param coil_bit_count: 线圈状态寄存器数量
    @param input_bit_count: 离散输入状态寄存器数量
    @param holding_reg_count: 保持寄存器数量
    @param input_reg_count: 输入寄存器数量
    @param coil_bit_start_addr: 线圈状态寄存器起始地址, 默认0x00
    @param input_bit_start_addr: 离散输入状态寄存器起始地址, 默认0x00
    @param holding_reg_start_addr: 保持寄存器起始地址, 默认0x00
    @param input_reg_start_addr: 输入寄存器起始地址, 默认0x00
  */
  ModbusData(unsigned int coil_bit_count, unsigned int input_bit_count,
    unsigned int holding_reg_count, unsigned int input_reg_count, 
    unsigned int coil_bit_start_addr = 0x00, unsigned int input_bit_start_addr = 0x00,
    unsigned int holding_reg_start_addr = 0x00, unsigned int input_reg_start_addr = 0x00);
  
  /* ~ModbusData: 销毁数据寄存器
  */
  ~ModbusData();

  /********************** READ *********************/

  /* read_coil_bits: 从线圈状态寄存器读取数据
    @param addr: 要读取的寄存器起始地址
    @param quantity: 要读取的寄存器数量
    @param bits: 存储读取到的寄存器的值的数组, 数组大小不能小于quantity
    :return: 成功返回0
  */
  int read_coil_bits(int addr, int quantity, unsigned char *bits);

  /* read_input_bits: 从离散输入寄存器读取数据
    @param addr: 要读取的寄存器起始地址
    @param quantity: 要读取的寄存器数量
    @param bits: 存储读取到的寄存器的值的数组, 数组大小不能小于quantity
    :return: 成功返回0
  */
  int read_input_bits(int addr, int quantity, unsigned char *bits);

  /* read_holding_registers: 从保持寄存器读取数据
    @param addr: 要读取的寄存器起始地址
    @param quantity: 要读取的寄存器数量
    @param regs: 存储读取到的寄存器的值的数组, 数组大小不能小于quantity
    :return: 成功返回0
  */
  int read_holding_registers(int addr, int quantity, unsigned short *regs);
  
  /* read_input_registers: 从输入寄存器读取数据
    @param addr: 要读取的寄存器起始地址
    @param quantity: 要读取的寄存器数量
    @param regs: 存储读取到的寄存器的值的数组, 数组大小不能小于quantity
    :return: 成功返回0
  */
  int read_input_registers(int addr, int quantity, unsigned short *regs);

  /********************** WRITE *********************/

  /* write_coil_bits: 写数据到线圈状态寄存器
    @param addr: 要写入的寄存器的起始地址
    @param bits: 要写入到寄存器的数据数组, 数组大小不能小于quantity
    @param quantity: 要写入的寄存器数量
    :return: 成功返回0
  */
  int write_coil_bits(int addr, unsigned char *bits, int quantity);

  /* write_input_bits: 写数据到线圈状态寄存器
    @param addr: 要写入的寄存器的起始地址
    @param bits: 要写入到寄存器的数据数组, 数组大小不能小于quantity
    @param quantity: 要写入的寄存器数量
    :return: 成功返回0
  */
  int write_input_bits(int addr, unsigned char *bits, int quantity);
  
  /* write_holding_registers: 写数据到线圈状态寄存器
    @param addr: 要写入的寄存器的起始地址
    @param regs: 要写入到寄存器的数据数组, 数组大小不能小于quantity
    @param quantity: 要写入的寄存器数量
    :return: 成功返回0
  */
  int write_holding_registers(int addr, unsigned short *regs, int quantity);
  
  /* write_input_registers: 写数据到线圈状态寄存器
    @param addr: 要写入的寄存器的起始地址
    @param regs: 要写入到寄存器的数据数组, 数组大小不能小于quantity
    @param quantity: 要写入的寄存器数量
    :return: 成功返回0
  */
  int write_input_registers(int addr, unsigned short *regs, int quantity);

  /* mask_write_holding_register: 对保持寄存器进行掩码处理
    先把寄存器的值读取出来curr_val, 然后进行掩码操作(curr_val & and_mask) | (or_mask & ~and_mask), 把掩码操作后的值写回寄存器
    @param and_mask: 进行"与"操作的掩码
    @param or_mask: 进行"或"操作的掩码
    :return: 成功返回0
  */
  int mask_write_holding_register(int addr, unsigned short and_mask, unsigned short or_mask);

  /* write_and_read_holding_registers: 同时对保持寄存器进行读写(先写后读)
    @param w_addr: 要写入的寄存器的起始地址
    @param w_regs: 要写入到寄存器的数据数组, 数组大小不能小于w_quantity
    @param w_quantity: 要写入的寄存器数量
    @param r_addr: 要读取的寄存器起始地址
    @param r_quantity: 要读取的寄存器数量
    @param r_regs: 存储读取到的寄存器的值的数组, 数组大小不能小于r_quantity
    :return: 成功返回0
  */
  int write_and_read_holding_registers(int w_addr, unsigned short *w_regs, int w_quantity, int r_addr, int r_quantity, unsigned short *r_regs);

  /********************** GET *********************/

  /* get_coil_bit_struct: 获取指定地址的线圈状态寄存器
    @param addr: 寄存器地址
    :return: 成功返回寄存器指针，失败返回NULL
  */
  modbus_base_data<unsigned char>* get_coil_bit_struct(int addr);
  
  /* get_input_bit_struct: 获取指定地址的离散输入状态寄存器
    @param addr: 寄存器地址
    :return: 成功返回寄存器指针，失败返回NULL
  */
  modbus_base_data<unsigned char>* get_input_bit_struct(int addr);
  
  /* get_holding_register_struct: 获取指定地址的保持寄存器
    @param addr: 寄存器地址
    :return: 成功返回寄存器指针，失败返回NULL
  */
  modbus_base_data<unsigned short>* get_holding_register_struct(int addr);
  
  /* get_input_register_struct: 获取指定地址的输入寄存器
    @param addr: 寄存器地址
    :return: 成功返回寄存器指针，失败返回NULL
  */
  modbus_base_data<unsigned short>* get_input_register_struct(int addr);

private:
  unsigned int coil_bit_start_addr_;    // 线圈状态寄存器起始地址
  unsigned int input_bit_start_addr_;   // 离散输入状态寄存器起始地址
  unsigned int holding_reg_start_addr_; // 保持寄存器起始地址
  unsigned int input_reg_start_addr_;   // 输入寄存器起始地址
  unsigned int coil_bit_count_;     // 线圈状态寄存器数量
  unsigned int input_bit_count_;    // 离散输入状态寄存器数量
  unsigned int holding_reg_count_;  // 保持寄存器数量
  unsigned int input_reg_count_;    // 输入寄存器数量
  modbus_base_data<unsigned char> *coil_bits_;      // 线圈状态寄存器数组
  modbus_base_data<unsigned char> *input_bits_;     // 离散输入状态寄存器数组
  modbus_base_data<unsigned short> *holding_regs_;  // 保持寄存器数组
  modbus_base_data<unsigned short> *input_regs_;    // 输入寄存器数组
};

/* Modbus数据寄存器的静态操作类 */
class StaticModbusData 
{
public:
  static void set_modbus_data(ModbusData* modbus_data);
  static ModbusData* get_modbus_data(void);

  static int read_coil_bits(int addr, int quantity, unsigned char *bits);
  static int read_input_bits(int addr, int quantity, unsigned char *bits);
  static int read_holding_registers(int addr, int quantity, unsigned short *regs);
  static int read_input_registers(int addr, int quantity, unsigned short *regs);

  static int write_coil_bits(int addr, unsigned char *bits, int quantity);
  static int write_input_bits(int addr, unsigned char *bits, int quantity);
  static int write_holding_registers(int addr, unsigned short *regs, int quantity);
  static int write_input_registers(int addr, unsigned short *regs, int quantity);

  static int mask_write_holding_register(int addr, unsigned short and_mask, unsigned short or_mask);
  static int write_and_read_holding_registers(int w_addr, unsigned short *w_regs, int w_quantity, int r_addr, int r_quantity, unsigned short *r_regs);


  static modbus_base_data<unsigned char>* get_coil_bit_struct(int addr);
  static modbus_base_data<unsigned char>* get_input_bit_struct(int addr);
  static modbus_base_data<unsigned short>* get_holding_register_struct(int addr);
  static modbus_base_data<unsigned short>* get_input_register_struct(int addr);

private:
  static ModbusData *modbus_data_;
};

#endif // _MODBUS_DATA_H_