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

/* modbus_base_data: Modbus寄存器基本数据结构 
 * 没有额外的空间开销，但不支持绑定额外的读写方法
 */
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

  /* 不支持, 定义仅仅为了兼容modbus_struct_data的代码 */ 
  int bind_get(T(*func)(T)) { return -1; }
  int bind_get(std::function<T (T)> func) { return -1; }
  int bind_set(int (*func)(T)) { return -1; }
  int bind_set(std::function<int (T)> func) { return -1; }

private:
  T data;
};

/* modbus_struct_data_std_get: 封装了std::function类型的结构体，结构体存储Modbus数据的额外读方法 */
template <class T>
struct modbus_struct_data_std_get {
  // 重载运算符：可以通过把实例当作func方法调用
  T operator()(T val) { return (func_.operator bool()) ? func_(val) : val; }
  void set_func(std::function<T (T)> func) { func_ = func; }
private:
  std::function<T (T)> func_;
};

/* modbus_struct_data_std_set: 封装了std::function类型的结构体，结构体存储Modbus数据的额外写方法 */
template <class T>
struct modbus_struct_data_std_set {
  // 重载运算符：可以通过把实例当作func方法调用
  int operator()(T val) { return (func_.operator bool()) ? func_(val) : 0; }
  void set_func(std::function<int (T)> func) { func_ = func; }
private:
  std::function<int (T)> func_;
};

/* modbus_struct_data_op: modbus_struct_data的额外读写操作结构 */
#pragma pack(1)
template <class T>
struct modbus_struct_data_op {
  modbus_struct_data_op() {
    flags.has_get = FALSE;
    flags.has_set = FALSE;
    ptr_get = NULL;
    ptr_set = NULL;
    std_get = NULL;
    std_set = NULL;
  }
  ~modbus_struct_data_op() {
    if (std_get != NULL) { delete std_get; std_get = NULL; }
    if (std_set != NULL) { delete std_set; std_set = NULL; }
  }

  /* get: 数据的额外读操作 */
  T get(T val) { return flags.has_get ? flags.is_std_get ? (*std_get)(val) : ptr_get(val) : val; }
  
  /* set: 数据的额外写操作 */
  int set(T val) { return flags.has_set ? flags.is_std_set ? (*std_set)(val) : ptr_set(val) : 0; }

  /* bind_get: 数据的额外读方法的绑定
  * 1. 函数指针作为绑定参数
  * 2. std::function作为绑定参数
  */ 
  int bind_get(T(*func)(T))
  {
    ptr_get = func;
    flags.is_std_get = FALSE;
    flags.has_get = TRUE;
    return 0;
  }
  int bind_get(std::function<T (T)> func)
  {
    if (std_get == NULL) std_get = new modbus_struct_data_std_get<T>();
    std_get->set_func(func);
    flags.is_std_get = TRUE;
    flags.has_get = TRUE;
    return 0;
  }

  /* bind_set: 数据的额外写方法的绑定
  * 1. 函数指针作为绑定参数
  * 2. std::function作为绑定参数
  */ 
  int bind_set(int (*func)(T)) {
    ptr_set = func;
    flags.is_std_set = FALSE;
    flags.has_set = TRUE;
    return 0;
  }
  int bind_set(std::function<int (T)> func) {
    if (std_set == NULL) std_set = new modbus_struct_data_std_set<T>();
    std_set->set_func(func);
    flags.is_std_set = TRUE;
    flags.has_set = TRUE;
    return 0;
  }

private:
  struct {
    unsigned char has_get : 1;    // 是否绑定了额外的数据的读方法
    unsigned char has_set : 1;    // 是否绑定了额外的数据的写方法
    unsigned char is_std_get : 1; // 绑定的额外读方法是否是std::function类型
    unsigned char is_std_set : 1; // 绑定的额外写方法是否是std::function类型
  } flags;

  // 数据的额外读方法(函数指针的形式)
  T (*ptr_get)(T);
  // 数据的额外写方法(函数指针的形式)
  int (*ptr_set)(T);

  // 数据的额外读方法(std::function的形式)
  // 这里没有直接定义std::function(32个字节), 而是用了个指针(8个字节)占位，真正绑定再创建
  modbus_struct_data_std_get<T> *std_get;
  // 数据的额外写方法(std::function的形式)
  // 这里没有直接定义std::function(32个字节), 而是用了个指针(8个字节)占位，真正绑定再创建
  modbus_struct_data_std_set<T> *std_set;
};
#pragma pack()

/* modbus_struct_data: Modbus寄存器可扩展数据结构
 * 支持绑定额外的数据读写方法，但是占用空间大
 */
#pragma pack(1)
template <class T>
struct modbus_struct_data {
  modbus_struct_data() {
    data = 0;
    op = NULL;
  }
  ~modbus_struct_data() {
    if (op != NULL) { delete op; op = NULL; }
  }

  /* get: 数据的读操作 */
  T get() {
    return data = (op != NULL) ? op->get(data) : data;
  }

  /* get_data: 直接获取寄存器的值（不调用额外的读方法） */
  T get_data() { return data; }

  /* set: 数据的写操作 */
  int set(T val) {
    data = val;
    return (op != NULL) ? op->set(data) : 0;
  }

  /* set_data: 直接设置寄存器的值 (不调用额外的写方法) */
  int set_data(T val) {
    data = val;
    return 0;
  }
  
  /* bind_get: 数据的额外读方法的绑定
  * 1. 函数指针作为绑定参数
  * 2. std::function作为绑定参数
  */ 
  int bind_get(T(*func)(T))
  {
    if (op == NULL) op = new modbus_struct_data_op<T>();
    return op->bind_get(func);
  }
  int bind_get(std::function<T (T)> func)
  {
    if (op == NULL) op = new modbus_struct_data_op<T>();
    return op->bind_get(func);
  }

  /* bind_set: 数据的额外写方法的绑定
  * 1. 函数指针作为绑定参数
  * 2. std::function作为绑定参数
  */ 
  int bind_set(int (*func)(T)) {
    if (op == NULL) op = new modbus_struct_data_op<T>();
    return op->bind_set(func);
  }
  int bind_set(std::function<int (T)> func) {
    if (op == NULL) op = new modbus_struct_data_op<T>();
    return op->bind_set(func);
  }

private:
  T data;
  modbus_struct_data_op<T> *op;
};
#pragma pack()

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
  modbus_struct_data<unsigned char>* get_coil_bit_struct(int addr);
  
  /* get_input_bit_struct: 获取指定地址的离散输入状态寄存器
    @param addr: 寄存器地址
    :return: 成功返回寄存器指针，失败返回NULL
  */
  modbus_struct_data<unsigned char>* get_input_bit_struct(int addr);
  
  /* get_holding_register_struct: 获取指定地址的保持寄存器
    @param addr: 寄存器地址
    :return: 成功返回寄存器指针，失败返回NULL
  */
  modbus_struct_data<unsigned short>* get_holding_register_struct(int addr);
  
  /* get_input_register_struct: 获取指定地址的输入寄存器
    @param addr: 寄存器地址
    :return: 成功返回寄存器指针，失败返回NULL
  */
  modbus_struct_data<unsigned short>* get_input_register_struct(int addr);

private:
  unsigned int coil_bit_start_addr_;    // 线圈状态寄存器起始地址
  unsigned int input_bit_start_addr_;   // 离散输入状态寄存器起始地址
  unsigned int holding_reg_start_addr_; // 保持寄存器起始地址
  unsigned int input_reg_start_addr_;   // 输入寄存器起始地址
  unsigned int coil_bit_count_;     // 线圈状态寄存器数量
  unsigned int input_bit_count_;    // 离散输入状态寄存器数量
  unsigned int holding_reg_count_;  // 保持寄存器数量
  unsigned int input_reg_count_;    // 输入寄存器数量
  modbus_struct_data<unsigned char> *coil_bits_;      // 线圈状态寄存器数组
  modbus_struct_data<unsigned char> *input_bits_;     // 离散输入状态寄存器数组
  modbus_struct_data<unsigned short> *holding_regs_;  // 保持寄存器数组
  modbus_struct_data<unsigned short> *input_regs_;    // 输入寄存器数组
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


  static modbus_struct_data<unsigned char>* get_coil_bit_struct(int addr);
  static modbus_struct_data<unsigned char>* get_input_bit_struct(int addr);
  static modbus_struct_data<unsigned short>* get_holding_register_struct(int addr);
  static modbus_struct_data<unsigned short>* get_input_register_struct(int addr);

private:
  static ModbusData *modbus_data_;
};

#endif // _MODBUS_DATA_H_