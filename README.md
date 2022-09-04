# modbus_tcp
Modbus TCP标准协议的C++实现，支持两种不同的Modbus数据结构的使用
- 基本型数据结构
  - 数据结构包含原始数据，大小和原始数据一致
  - 数据的更新需要主动调用数据结构的`set_data`方法更新
  - Modbus TCP指令读/写只能读/写结构的原始数据
  
- 扩展型数据结构
  - 数据结构包含原始数据和数据操作指针，大小比原始数据要大
  - 数据的更新可以主动调用数据结构的`set_data`方法更新
  - 数据结构支持额外绑定数据的读写方法，使用数据结构的`bind_get`和`bind_set`方法
  - Modbus TCP指令读/写优先调用数据额外绑定的方法，如果没有绑定就读/写结构的原始数据

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

  # 测试扩展型数据结构的Modbus数据寄存器读写
  ./build/bin/test_modbus_struct_data

  # 测试Modbus TCP数据处理
  ./build/bin/test_modbus_tcp_data
  ```

## 功能支持说明
- Modbus数据寄存器读写
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
- Modbus TCP服务器（未实现）
- Modbus TCP客户端（未实现）

## Modbus数据寄存器读写
- 基本型数据结构的寄存器读写
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

- 扩展型数据结构的寄存器读写
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

  // 从地址0x02开始读8个线圈状态寄存器的值
  unsigned char r_bits[8] = {0};
  StaticModbusData::read_coil_bits(0x02, 8, r_bits);

  // 从地址0x05开始写3输入寄存器的值
  unsigned short w_regs[3] = {11, 22, 33};
  StaticModbusData::write_input_registers(0x05, w_regs, 3);

  // 更新地址为0x03保持寄存器的值为55, (注意操作的地址要存在)
  StaticModbusData::get_holding_register_struct(0x03)->set_data(55);

  // 给地址为0x05的离散输入寄存器额外绑定读方法
  // 绑定之后Modbus TCP指令读取该寄存器时，读取到的数据就是该方法的返回值
  unsigned char get_bit(unsigned char val) {
    return 11; 
  }
  StaticModbusData::get_input_bit_struct(0x05)->bind_get(get_bit);
  
  // 给地址为0x06的保持寄存器额外绑定写方法
  // 绑定之后Modbus TCP指令每次写该寄存器时，都会调用到该方法
  int set_reg(unsigned short val) {
    printf("set_reg: %d\n", val);
    return 0;
  }
  StaticModbusData::get_holding_register_struct(0x06)->bind_set(set_reg);
  ```

## Modbus TCP数据处理
- 这里假定已经在程序别的地方创建好Modbus寄存器，并绑定到Modbus数据的静态操作类上，参照 __Modbus数据寄存器读写__
  
  ```c++
  // 头文件导入
  #include "modbus_tcp.h"

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