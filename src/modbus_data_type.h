/*
 * Software License Agreement (MIT License)
 *
 * Copyright (c) 2022, Vinman, Inc.
 * All rights reserved.
 *
 * Author: Vinman <vinman.cub@gmail.com>
 */

#ifndef _MODBUS_DATA_TYPE_H_
#define _MODBUS_DATA_TYPE_H_

#include <stdio.h>
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

#define NOT_SUPPORT -2

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

  /* 是否是数据指针结构, 用来和非数据指针结构区分 */
  bool is_ptr_struct() { return false; }

  // /* 不支持, 定义仅仅为了兼容modbus_struct_data的代码 */ 
  // int bind_get(T(*func)(T)) { printf("`modbus_base_data` is not support bind_get, please use `modbus_struct_data`\n"); return NOT_SUPPORT; }
  // int bind_get(std::function<T (T)> func) { printf("`modbus_base_data` is not support bind_get, please use `modbus_struct_data`\n"); return NOT_SUPPORT; }
  // int bind_set(int (*func)(T)) { printf("`modbus_base_data` is not support bind_set, please use `modbus_struct_data`\n"); return NOT_SUPPORT; }
  // int bind_set(std::function<int (T)> func) { printf("`modbus_base_data` is not support bind_set, please use `modbus_struct_data`\n"); return NOT_SUPPORT; }

  /* 没用，仅仅为了兼容代码 */
  int bind_data(T *val) { printf("`modbus_base_data` is not support bind_data, please use `modbus_base_ptr_data`\n"); return NOT_SUPPORT; }

private:
  T data;
};

/* modbus_base_ptr_data: Modbus寄存器基本指针数据结构 
 * 支持修改原始数据的地址指向
 * 与 modbus_base_data 相比, 本身不存储数据，只有一个数据指针
 */
#pragma pack(1)
template <class T>
struct modbus_base_ptr_data {
  modbus_base_ptr_data() { data_ptr = NULL; }

  /* get: 数据的读操作 */
  T get() { return get_data(); }

  /* set: 数据的写操作 */
  int set(T val) { return set_data(val); }

  /* get_data: 直接获取寄存器的值 */
  T get_data() { return data_ptr != NULL ? *data_ptr : 0; }

  /* set_data: 直接设置寄存器的值 */
  int set_data(T val) {
    if (data_ptr != NULL) *data_ptr = val;
    return 0;
  }

  /* 是否是数据指针结构, 用来和非数据指针结构区分 */
  bool is_ptr_struct() { return true; }

  // /* 不支持, 定义仅仅为了兼容modbus_struct_data的代码 */ 
  // int bind_get(T(*func)(T)) { printf("`modbus_base_ptr_data` is not support bind_get, please use `modbus_struct_ptr_data`\n"); return NOT_SUPPORT; }
  // int bind_get(std::function<T (T)> func) { printf("`modbus_base_ptr_data` is not support bind_get, please use `modbus_struct_ptr_data`\n"); return NOT_SUPPORT; }
  // int bind_set(int (*func)(T)) { printf("`modbus_base_ptr_data` is not support bind_set, please use `modbus_struct_ptr_data`\n"); return NOT_SUPPORT; }
  // int bind_set(std::function<int (T)> func) { printf("`modbus_base_ptr_data` is not support bind_set, please use `modbus_struct_ptr_data`\n"); return NOT_SUPPORT; }

  /* bind_data: 原始数据绑定 */
  int bind_data(T *val) { data_ptr = val; return 0; }

public:
  T *data_ptr;
};
#pragma pack()

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

  /* unbind_get: 解绑额外绑定的读方法，即通过bind_get绑定的方法 */
  void unbind_get() { flags.has_get = FALSE; flags.is_std_get = FALSE; }

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

  /* unbind_set: 解绑额外绑定的写方法，即通过bind_set绑定的方法 */
  void unbind_set() { flags.has_set = FALSE; flags.is_std_set = FALSE; }

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
    int code = (op != NULL) ? op->set(val) : 0;
    data = code == 0 ? val : data;
    return code;
    // data = val;
    // return (op != NULL) ? op->set(data) : 0;
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

  /* unbind_get: 解绑额外绑定的读方法，即通过bind_get绑定的方法 */
  void unbind_get() { if (op != NULL) op->unbind_get(); }

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

  /* unbind_set: 解绑额外绑定的写方法，即通过bind_set绑定的方法 */
  void unbind_set() { if (op != NULL) op->unbind_set(); }

  /* 是否是数据指针结构, 用来和非数据指针结构区分 */
  bool is_ptr_struct() { return false; }

  /* 没用，仅仅为了兼容代码 */
  int bind_data(T *val) { printf("`modbus_struct_data` is not support bind_data, please use `modbus_struct_ptr_data`\n"); return NOT_SUPPORT; }

private:
  T data;
  modbus_struct_data_op<T> *op;
};
#pragma pack()

/* modbus_struct_ptr_data: Modbus寄存器可扩展指针数据结构
 * 支持绑定额外的数据读写方法，但是占用空间大
 * 支持修改原始数据的地址指向
 * 与 modbus_struct_data 相比, 本身不存储数据，只有一个数据指针
 */
#pragma pack(1)
template <class T>
struct modbus_struct_ptr_data {
  modbus_struct_ptr_data() {
    data_ptr = NULL;
    op = NULL;
  }
  ~modbus_struct_ptr_data() {
    if (op != NULL) { delete op; op = NULL; }
  }

  /* get: 数据的读操作 */
  T get() {
    if (op != NULL) {
      if (data_ptr == NULL) {
        return op->get(0);
      }
      else {
        return *data_ptr = op->get(*data_ptr);
      }
    }
    else {
      return data_ptr != NULL ? *data_ptr : 0;
    }
  }

  /* get_data: 直接获取寄存器的值（不调用额外的读方法） */
  T get_data() { return data_ptr != NULL ? *data_ptr : 0; }

  /* set: 数据的写操作 */
  int set(T val) {
    int code = (op != NULL) ? op->set(val) : 0;
    if (code == 0) set_data(val);
    return code;
  }

  /* set_data: 直接设置寄存器的值 (不调用额外的写方法) */
  int set_data(T val) {
    if (data_ptr != NULL) *data_ptr = val;
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

  /* unbind_get: 解绑额外绑定的读方法，即通过bind_get绑定的方法 */
  void unbind_get() { if (op != NULL) op->unbind_get(); }

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

  /* unbind_set: 解绑额外绑定的写方法，即通过bind_set绑定的方法 */
  void unbind_set() { if (op != NULL) op->unbind_set(); }

  /* 是否是数据指针结构, 用来和非数据指针结构区分 */
  bool is_ptr_struct() { return true; }

  /* bind_data: 原始数据绑定 */
  int bind_data(T *val) { data_ptr = val; return 0; }

public:
  T *data_ptr;
private:
  modbus_struct_data_op<T> *op;
};
#pragma pack()

#endif // _MODBUS_DATA_TYPE_H_