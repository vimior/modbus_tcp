
#!/usr/bin/env python3
# Software License Agreement (BSD License)
#
# Copyright (c) 2022, Vinman, Inc.
# All rights reserved.
#
# Author: Vinman <vinman.cub@gmail.com>

import unittest
import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from modbus_tcp import ModbusData, ModbusCode, ModbusDataSession, ModbusDataService


class TestModbusTcpData(unittest.TestCase):
    def test_coil_bits(self):
        session = ModbusDataSession()
        # test read success
        req_data = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x01, 0x00, 0x00, 0x00, 10]
        session.set_request_data(req_data)
        service.process_session(session)
        req = session.request
        res = session.response
        self.assertTrue(res.transaction_id.value == req.transaction_id.value)
        self.assertTrue(res.protocol_id.value == req.protocol_id.value)
        self.assertTrue(res.unit_id.value == req.unit_id.value)
        self.assertTrue(res.func_code.value == req.func_code.value)
        self.assertTrue(res.length.value == 5 and res.data_length == 11)

        # test read failed
        req_data = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x01, 0x00, 0x00, 0x00, 20]
        session.set_request_data(req_data)
        service.process_session(session)
        req = session.request
        res = session.response
        self.assertTrue(res.transaction_id.value == req.transaction_id.value)
        self.assertTrue(res.protocol_id.value == req.protocol_id.value)
        self.assertTrue(res.unit_id.value == req.unit_id.value)
        self.assertTrue(res.func_code.value == req.func_code.value + 0x80)
        self.assertTrue(res.length.value == 3 and res.data_length == 9)
        self.assertTrue(res.raw_data[8] == ModbusCode.ILLEGAL_DATA_ADDRESS)

    def test_input_bits(self):
        session = ModbusDataSession()
        # test read success
        req_data = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x02, 0x00, 0x00, 0x00, 10]
        session.set_request_data(req_data)
        service.process_session(session)
        req = session.request
        res = session.response
        self.assertTrue(res.transaction_id.value == req.transaction_id.value)
        self.assertTrue(res.protocol_id.value == req.protocol_id.value)
        self.assertTrue(res.unit_id.value == req.unit_id.value)
        self.assertTrue(res.func_code.value == req.func_code.value)
        self.assertTrue(res.length.value == 5 and res.data_length == 11)

        # test read failed
        req_data = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x02, 0x00, 0x00, 0x00, 20]
        session.set_request_data(req_data)
        service.process_session(session)
        req = session.request
        res = session.response
        self.assertTrue(res.transaction_id.value == req.transaction_id.value)
        self.assertTrue(res.protocol_id.value == req.protocol_id.value)
        self.assertTrue(res.unit_id.value == req.unit_id.value)
        self.assertTrue(res.func_code.value == req.func_code.value + 0x80)
        self.assertTrue(res.length.value == 3 and res.data_length == 9)
        self.assertTrue(res.raw_data[8] == ModbusCode.ILLEGAL_DATA_ADDRESS)

    def test_holding_regs(self):
        session = ModbusDataSession()
        # test read success
        req_data = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x03, 0x00, 0x00, 0x00, 10]
        session.set_request_data(req_data)
        service.process_session(session)
        req = session.request
        res = session.response
        self.assertTrue(res.transaction_id.value == req.transaction_id.value)
        self.assertTrue(res.protocol_id.value == req.protocol_id.value)
        self.assertTrue(res.unit_id.value == req.unit_id.value)
        self.assertTrue(res.func_code.value == req.func_code.value)
        self.assertTrue(res.length.value == 23 and res.data_length == 29)

        # test read failed
        req_data = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x03, 0x00, 0x00, 0x00, 20]
        session.set_request_data(req_data)
        service.process_session(session)
        req = session.request
        res = session.response
        self.assertTrue(res.transaction_id.value == req.transaction_id.value)
        self.assertTrue(res.protocol_id.value == req.protocol_id.value)
        self.assertTrue(res.unit_id.value == req.unit_id.value)
        self.assertTrue(res.func_code.value == req.func_code.value + 0x80)
        self.assertTrue(res.length.value == 3 and res.data_length == 9)
        self.assertTrue(res.raw_data[8] == ModbusCode.ILLEGAL_DATA_ADDRESS)

    def test_input_regs(self):
        session = ModbusDataSession()
        # test read success
        req_data = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x04, 0x00, 0x00, 0x00, 10]
        session.set_request_data(req_data)
        service.process_session(session)
        req = session.request
        res = session.response
        self.assertTrue(res.transaction_id.value == req.transaction_id.value)
        self.assertTrue(res.protocol_id.value == req.protocol_id.value)
        self.assertTrue(res.unit_id.value == req.unit_id.value)
        self.assertTrue(res.func_code.value == req.func_code.value)
        self.assertTrue(res.length.value == 23 and res.data_length == 29)

        # test read failed
        req_data = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x04, 0x00, 0x00, 0x00, 20]
        session.set_request_data(req_data)
        service.process_session(session)
        req = session.request
        res = session.response
        self.assertTrue(res.transaction_id.value == req.transaction_id.value)
        self.assertTrue(res.protocol_id.value == req.protocol_id.value)
        self.assertTrue(res.unit_id.value == req.unit_id.value)
        self.assertTrue(res.func_code.value == req.func_code.value + 0x80)
        self.assertTrue(res.length.value == 3 and res.data_length == 9)
        self.assertTrue(res.raw_data[8] == ModbusCode.ILLEGAL_DATA_ADDRESS)

if __name__ == '__main__':
    modbus_data = ModbusData(10, 10, 10, 10)
    
    service = ModbusDataService(modbus_data)

    unittest.main()
