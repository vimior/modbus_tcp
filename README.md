# modbus_tcp (Python实现)

__C++实现请切换到[C++分支](https://github.com/vimior/modbus_tcp)__

## 功能概括
- __Modbus数据的处理__（寄存器数据的定义和操作）
- __Modbus TCP数据的处理__（输入为接收到的Modbus TCP数据，输出为回复数据，支持粘包处理）
- 实际综合使用示例: [test_main](tests/test_main.py)

Modbus寄存器数据(`ModbusStructData`)说明
  - 数据结构不仅包含原始数据， 还支持给数据绑定额外的读写方法
  - 数据的更新可以主动调用数据结构的`set_data`方法更新
  - 数据结构支持额外绑定数据的读写方法，使用数据结构的`bind_get`和`bind_set`方法
    - `bind_get`: 参数是一个函数(返回值和参数都为为原始数据类型，调用时会以寄存器的原始数据值作为实参传递，所以该函数在实现时可以根据情况返回一个新的值或者把原始数据返回，函数的返回值将会覆盖原始数据值)。可以通过`unbind_get`方法来解绑
    - `bind_set`: 参数是一个函数(返回值为整型、参数为原始数据类型，调用时会把要设置的值作为参数传递，该函数返回0时会把设置的值更新到原始数据，否则不更新原始数据)。可以通过`unbind_set`方法来解绑
  - Modbus TCP指令读/写优先调用数据额外绑定的方法(如果有绑定的get方法，会把该方法的结果更新到原始数据，如果有绑定的设置方法，会把设置的值更新到原始数据并把该值当作操作传递给绑定的设置方法)，如果没有绑定就读/写结构的原始数据

## 安装测试
- 安装
  ```bash
  python setup.py install
  ```
- 测试
  ```bash
  # 测试Modbus数据寄存器读写
  python tests/test_modbus_data.py

  # 测试Modbus TCP数据处理
  python tests/test_modbus_tcp_data.py
  ```

## 功能支持说明
- Modbus数据寄存器读写
  - 数据操作类: `ModbusData` 和 `StaticModbusData`
    - 可以额外绑定寄存器的读写方法: `get_XXX_struct(addr).bind_get(...)`、`get_XXX_struct(addr).bind_set(...)`

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
  - Modbus TCP数据操作类: `ModbusDataService`

## Modbus数据操作
- Modbus寄存器数据读写(参考[test_modbus_data](./tests/test_modbus_data.py))
  ```python
  from modbus_tcp import ModbusData, StaticModbusData

  # 创建Modbus寄存器
  modbus_data = ModbusData(20, 20, 20, 20)

  # 绑定寄存器操作实例到对应的静态操作类，这样就可以通过静态类操作到实例了
  # 也可以不绑定，直接通过实例操作，绑定是为了在整个程序的任何地方都能通过静态类访问
  StaticModbusData.set_modbus_data(modbus_data)

  # 从地址0x02开始读8个线圈状态寄存器的值
  code, r_bits = StaticModbusData.read_coil_bits(0x02, 8)

  # 从地址0x05开始写3输入寄存器的值
  w_regs = [11, 22, 33]
  StaticModbusData.write_input_registers(0x05, w_regs)

  # 更新地址为0x03保持寄存器的值为55, (注意操作的地址要存在)
  StaticModbusData.get_holding_register_struct(0x03).set_data(55)
  ```

- Modbus TCP数据处理(参考[test_modbus_tcp_data](./tests/test_modbus_tcp_data.py))
  ```python
  from modbus_tcp import ModbusData, StaticModbusData, ModbusDataSession, ModbusDataService
  
  # 获取Modbus寄存器数据操作实例
  modbus_data= StaticModbusData.get_modbus_data()

  # 创建一个 ModbusDataService
  service = ModbusDataService(modbus_data)

  # 创建一个 ModbusDataSession
  session = ModbusDataSession()

  # 这里模拟请求的数据: 相当于从地址为0x00开始读取10个输入寄存器
  req_data_1 = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x04, 0x00, 0x00, 0x00, 10]
  # 指定请求数据
  session.set_request_data(req_data_1)
  # 处理请求数据
  service.process_session(&session, modbus_data);
  print('read input register, req={}'.format(session.get_request_data()))
  print('read input register, res={}'.format(session.get_response_data()))

  # 这里模拟请求的数据: 相当于从地址为0x00开始写10个保持寄存器
  req_data_2 = [0x00, 0x01, 0x00, 0x00, 0x00, 27, 0x01, 0x10, 0x00, 0x00, 0x00, 0x0A, 20, 0x00, 11, 0x00, 12, 0x00, 13, 0x00, 14, 0x00, 15, 0x00, 16, 0x00, 17, 0x00, 18, 0x00, 19, 0x00, 20]
  # 指定请求数据
  session.set_request_data(req_data_2)
  # 处理请求数据
  service.process_session(&session, modbus_data);
  print('write holding register, req={}'.format(session.get_request_data()))
  print('write holding register, res={}'.format(session.get_response_data()))

  # 这里模拟请求的数据: 相当于从地址为0x00开始读取10个保持寄存器
  req_data_3 = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x03, 0x00, 0x00, 0x00, 10]
  # 指定请求数据
  session.set_request_data(req_data_3)
  # 处理请求数据
  service.process_session(&session, modbus_data);
  print('read holding register, req={}'.format(session.get_request_data()))
  print('read holding register, res={}'.format(session.get_response_data()))
  ```