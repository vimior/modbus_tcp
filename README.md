# modbus_tcp
Modbus TCP标准协议的C++实现，支持两种不同的Modbus数据结构的使用
- 基本型数据结构: `modbus_base_data`
  - 数据结构包含原始数据，大小和原始数据一致
  - 数据的更新需要主动调用数据结构的`set_data`方法更新
  - Modbus TCP指令读/写只能读/写结构的原始数据

- 基本型指针数据结构: `modbus_base_ptr_data`
  - 在`基本型数据结构`的基础上去除了原始数据，增加了指向原始数据的指针
  - 用的时候需要通过`bind_data`绑定原始数据的指向
  
- 扩展型数据结构: `modbus_struct_data`
  - 数据结构包含原始数据和数据操作指针，大小比原始数据要大
  - 数据的更新可以主动调用数据结构的`set_data`方法更新
  - 数据结构支持额外绑定数据的读写方法，使用数据结构的`bind_get`和`bind_set`方法
    - `bind_get`: 参数是一个函数(返回值和参数都为为原始数据类型，调用时会以寄存器的原始数据值作为实参传递，所以该函数在实现时可以根据情况返回一个新的值或者把原始数据返回，函数的返回值将会覆盖原始数据值)。可以通过`unbind_get`方法来解绑
    - `bind_set`: 参数是一个函数(返回值为整型、参数为原始数据类型，调用时会把要设置的值作为参数传递，该函数返回0时会把设置的值更新到原始数据，否则不更新原始数据)。可以通过`unbind_set`方法来解绑
  - Modbus TCP指令读/写优先调用数据额外绑定的方法(如果有绑定的get方法，会把该方法的结果更新到原始数据，如果有绑定的设置方法，会把设置的值更新到原始数据并把该值当作操作传递给绑定的设置方法)，如果没有绑定就读/写结构的原始数据

- 扩展型指针数据结构: `modbus_struct_ptr_data`
  - 在`扩展型数据结构`的基础上去除了原始数据，增加了指向原始数据的指针
  - 用的时候需要通过`bind_data`绑定原始数据的指向
  - 如果额外绑定了读写方法，会优先使用额外绑定的方法，同时会根据额外绑定的方法的返回值更新原始数据所指向的数据区域

## 编译测试
- 编译
  ```bash
  # 清楚编译
  make clean
  # 编译
  make

  # 注: 编译成静态库，生成的库文件在build/lib里面
  # 注: 默认会把测试例子也编译，生成的可执行文件在build/bin里面
  ```
- 测试
  ```bash
  # 测试基本型数据结构的Modbus数据寄存器读写
  ./build/bin/test_modbus_base_data

  # 测试基本型指针数据结构的Modbus数据寄存器读写
  ./build/bin/test_modbus_base_ptr_data

  # 测试扩展型数据结构的Modbus数据寄存器读写
  ./build/bin/test_modbus_struct_data

  # 测试扩展型指针数据结构的Modbus数据寄存器读写
  ./build/bin/test_modbus_struct_ptr_data

  # 测试Modbus TCP数据处理
  ./build/bin/test_modbus_tcp_data
  ```

## 功能支持说明
- Modbus数据寄存器读写
  - 基本型数据操作类: `ModbusBaseData` 和 `StaticModbusBaseData`
    - 同类寄存器的原始数据的真实地址不连续
    - 不可以访问原始数据的指向
    - 不能修改原始数据的指向
  - 基本型指针数据操作类: `ModbusBasePtrData` 和 `StaticModbusBasePtrData`
    - 同类寄存器的原始数据的真实地址默认是连续的，另外绑定了数据指向的寄存器除外
    - 可以访问到原始数据的指向: `get_XXX_struct(addr)->data_ptr`
    - 可以修改原始数据的指向: `get_XXX_struct(addr)->bind_data(...)`
  - 扩展型数据操作类: `ModbusStructData` 和 `StaticModbusStructData`
    - 同类寄存器的原始数据的真实地址不连续
    - 不可以访问原始数据的指向
    - 不能修改原始数据的指向
    - 可以额外绑定寄存器的读写方法: `get_XXX_struct(addr)->bind_get(...)`、`get_XXX_struct(addr)->bind_set(...)`
  - 扩展型指针数据操作类: `ModbusStructPtrData` 和 `StaticModbusStructPtrData`
    - 同类寄存器的原始数据的真实地址默认是连续的，另外绑定了数据指向的寄存器除外
    - 可以访问到原始数据的指向: `get_XXX_struct(addr)->data_ptr`
    - 可以修改原始数据的指向, `get_XXX_struct(addr)->bind_data(...)`
    - 可以额外绑定寄存器的读写方法: `get_XXX_struct(addr)->bind_get(...)`、`get_XXX_struct(addr)->bind_set(...)`
    - 注: 如果既修改了原始数据的指向，也重新绑定了额外的读写方法，那么优先会用额外绑定的方法(也就是原始数据的指向就没用)
  - 其它混合型数据操作类: `ModbusDataTemplate<A, B>` 和 `StaticModbusDataTemplate<A, B>`
    - 因A和B的不同而不同

- Modbus TCP数据处理(支持的指令如下)
  - __0x01__: 读取线圈状态寄存器(1位寄存器)
  - __0x02__: 读取离散输入寄存器(1位寄存器)
  - __0x03__: 读取保持寄存器(16位寄存器)
  - __0x04__: 读取输入寄存器(16位寄存器)
  - __0x05__: 写单个线圈状态寄存器
  - __0x06__: 写单个保持寄存器
  - __0x0F__: 写多个线圈状态寄存器
  - __0x10__: 写多个保持寄存器
  - __0x16__: 以掩码的形式写保持寄存器
  - __0x17__: 先写后读多个保持寄存器(写和读的地址和个数是独立的)
  - Modbus TCP数据操作类: `ModbusTCP::DataService<T>`, T指Modbus数据操作类(非静态)

- Modbus TCP服务器（未实现）

- Modbus TCP客户端（未实现）

## Modbus数据寄存器读写
- 基本型数据结构的寄存器读写(参考[test_modbus_base_data](./tests/test_modbus_base_data.cpp))
  ```c++
  // 头文件导入
  #include "modbus_data.h"

  // 指定使用基本型数据机构
  using ModbusData = ModbusBaseData;
  using StaticModbusData = StaticModbusBaseData;

  // 创建Modbus数据寄存器（一般整个程序只需要创建一次）
  ModbusData modbus_data(10, 10, 10, 10);

  // 把创建好的Modbus数据寄存器绑定到静态类，方便通过静态类操作该Modbus数据寄存器
  StaticModbusData::set_modbus_data(&modbus_data);

  // 从地址0x02开始读8个线圈状态寄存器的值
  unsigned char r_bits[8] = {0};
  StaticModbusData::read_coil_bits(0x02, 8, r_bits);

  // 从地址0x05开始写3输入寄存器的值
  unsigned short w_regs[3] = {11, 22, 33};
  StaticModbusData::write_input_registers(0x05, w_regs, 3);

  // 更新地址为0x03保持寄存器的值为55, (注意操作的地址要存在)
  StaticModbusData::get_holding_register_struct(0x03)->set_data(55);
  ```

- 基本型指针数据结构的寄存器读写(参考[test_modbus_base_ptr_data](./tests/test_modbus_base_ptr_data.cpp))
  ```c++
  // 头文件导入
  #include "modbus_data.h"

  // 指定使用基本型指针数据机构
  using ModbusData = ModbusBasePtrData;
  using StaticModbusData = StaticModbusBasePtrData;

  // 创建Modbus数据寄存器（一般整个程序只需要创建一次）
  ModbusData modbus_data(10, 10, 10, 10);

  // 把创建好的Modbus数据寄存器绑定到静态类，方便通过静态类操作该Modbus数据寄存器
  StaticModbusData::set_modbus_data(&modbus_data);

  // 把地址为0-3的保持寄存器的数据分别绑定到w_regs[0]-w_regs[3]
  // 绑定后地址为0-3的保持寄存器的的数据和w_regs是双向绑定的，指向的是同一个内存地址
  unsigned short w_regs[4] = {1, 2, 3, 4};
  for (int i = 0; i < 4; i++) {
    StaticModbusData::get_holding_register_struct(i)->bind_data(&w_regs[i]);
  }

  // 映射寄存器地址到一个指针，通过该指针可以操作寄存器
  // 注意寄存器不能被绑定到别的地方
  unsigned short *data_ptr = StaticModbusData::get_holding_register_struct(4)->data_ptr;
  for (int i = 0; i < 4; i++) {
    // 实际操作的就是寄存器的原始数据的指向
    *(data_ptr + i) = 5 + i;
  }
  ```

- 扩展型数据结构的寄存器读写(参考[test_modbus_struct_data](./tests/test_modbus_struct_data.cpp))
  ```c++
  // 头文件导入
  #include "modbus_data.h"

  // 指定使用扩展型数据结构
  using ModbusData = ModbusStructData;
  using StaticModbusData = StaticModbusStructData;

  // 创建Modbus寄存器（一般整个程序只需要创建一次）
  ModbusData modbus_data(10, 10, 10, 10);
  
  // 把创建好的Modbus数据寄存器绑定到静态类，方便通过静态类操作该Modbus数据寄存器
  StaticModbusData::set_modbus_data(&modbus_data);

  unsigned char get_bit(unsigned char val) {
    return 11; 
  }
  // 给地址为0x05的离散输入寄存器额外绑定读方法
  // 绑定之后Modbus TCP指令读取该寄存器时，读取到的数据就是该方法的返回值
  StaticModbusData::get_input_bit_struct(0x05)->bind_get(get_bit);
  
  // 给地址为0x06的保持寄存器额外绑定写方法
  // 绑定之后Modbus TCP指令每次写该寄存器时，都会调用到该方法
  int set_reg(unsigned short val) {
    printf("set_reg: %d\n", val);
    return 0;
  }
  StaticModbusData::get_holding_register_struct(0x06)->bind_set(set_reg);
  ```

- 扩展型指针数据结构的寄存器读写(参考[test_modbus_struct_ptr_data](./tests/test_modbus_struct_ptr_data.cpp))
  ```c++
  // 头文件导入
  #include "modbus_data.h"

  // 指定使用扩展型指针数据结构
  using ModbusData = ModbusStructPtrData;
  using StaticModbusData = StaticModbusStructPtrData;

  // 创建Modbus数据寄存器（一般整个程序只需要创建一次）
  ModbusData modbus_data(10, 10, 10, 10);

  // 把创建好的Modbus数据寄存器绑定到静态类，方便通过静态类操作该Modbus数据寄存器
  StaticModbusData::set_modbus_data(&modbus_data);

  // 把地址为0-3的保持寄存器的数据分别绑定到w_regs[0]-w_regs[3]
  // 绑定后地址为0-3的保持寄存器的的数据和w_regs是双向绑定的，指向的是同一个内存地址
  unsigned short w_regs[4] = {1, 2, 3, 4};
  for (int i = 0; i < 4; i++) {
    StaticModbusData::get_holding_register_struct(i)->bind_data(&w_regs[i]);
  }

  // 映射寄存器地址到一个指针，通过该指针可以操作寄存器
  // 注意寄存器不能被绑定到别的地方
  unsigned short *data_ptr = StaticModbusData::get_holding_register_struct(4)->data_ptr;
  for (int i = 0; i < 4; i++) {
    // 实际操作的就是寄存器的原始数据的指向
    *(data_ptr + i) = 5 + i;
  }

  unsigned short get_reg(unsigned short val) {
    return 99; 
  }
  // 给地址为0x05的保持寄存器额外绑定读方法
  // 绑定之后Modbus TCP指令读取该寄存器时，读取到的数据就是该方法的返回值
  StaticModbusData::get_holding_register_struct(0x01)->bind_get(get_reg);
  // 此时读取地址0x01的寄存器的值，将由get_reg返回，并会把返回的结果更新到该寄存器绑定的数据指向(也就是w_regs[1])
  ```

## Modbus TCP数据处理
- 这里假定已经在程序别的地方创建好Modbus寄存器，并绑定到Modbus数据的静态操作类上，参照 __Modbus数据寄存器读写__
  
  ```c++
  // 头文件导入
  #include "modbus_tcp_data.h"

  // 这里假定已经在程序的别的地方创建好Modbus寄存器，并绑定到静态操作类
  // 这里只演示Modbus TCP数据的处理

  // 指定要使用的数据结构（注：要和创建Modbus寄存器时的数据类型一致）
  using ModbusData = ModbusStructData;
  using StaticModbusData = StaticModbusStructData;
  // 指定Modbus TCP数据处理的类型
  using DataService = ModbusTCP::DataService<ModbusData>;

  // 获取Modbus寄存器数据指针
  ModbusData *modbus_data= StaticModbusData::get_modbus_data();

  // 创建一个DataSession
  ModbusTCP::DataSession session;

  // 这里模拟请求的数据: 相当于从地址为0x00开始读取10个输入寄存器
  unsigned char req_data_1[12] = {0x00, 0x01, 0x00, 0x00, 0x00, 5, 0x01, 0x04, 0x00, 0x00, 0x00, 10};
  // 指定请求数据和长度
  session.set_request_data(req_data_1, 12);
  // 处理请求数据
  DataService::process_session(&session, modbus_data);
  print_datas<unsigned char>("read input register, resuest", req_data_1, 12);
  print_datas<unsigned char>("read input register, response", session.get_response_data(), session.get_response_length());

  // 这里模拟请求的数据: 相当于从地址为0x00开始写10个保持寄存器
  unsigned char req_data_2[33] = {0x00, 0x01, 0x00, 0x00, 0x00, 26, 0x01, 0x10, 0x00, 0x00, 0x00, 0x0A, 20, 0x00, 11, 0x00, 12, 0x00, 13, 0x00, 14, 0x00, 15, 0x00, 16, 0x00, 17, 0x00, 18, 0x00, 19, 0x00, 20};
  // 指定请求数据和长度
  session.set_request_data(req_data_2, 33);
  // 处理请求数据
  DataService::process_session(&session, modbus_data);
  print_datas<unsigned char>("write holding register, resuest", req_data_2, 33);
  print_datas<unsigned char>("write holding register, response", session.get_response_data(), session.get_response_length());

  // 这里模拟请求的数据: 相当于从地址为0x00开始读取10个保持寄存器
  unsigned char req_data_3[12] = {0x00, 0x01, 0x00, 0x00, 0x00, 5, 0x01, 0x03, 0x00, 0x00, 0x00, 10};
  // 指定请求数据和长度
  session.set_request_data(req_data_3, 12);
  // 处理请求数据
  DataService::process_session(&session, modbus_data);
  print_datas<unsigned char>("read holding register, resuest", req_data_3, 12);
  print_datas<unsigned char>("read holding register, response", session.get_response_data(), session.get_response_length());
  ```


## Modbus TCP服务器
```c++
待实现
```
## Modbus TCP客户端
```c++
待实现
```