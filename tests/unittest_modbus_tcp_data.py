
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
        self.assertTrue(res.mbap.transaction_id == req.mbap.transaction_id)
        self.assertTrue(res.mbap.protocol_id == req.mbap.protocol_id)
        self.assertTrue(res.mbap.unit_id == req.mbap.unit_id)
        self.assertTrue(res.mbap.length == 5 and res.data_length == 11)
        self.assertTrue(res.pdu.func_code == req.pdu.func_code)

        # test read failed
        req_data = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x01, 0x00, 0x00, 0x00, 20]
        session.set_request_data(req_data)
        service.process_session(session)
        req = session.request
        res = session.response
        self.assertTrue(res.mbap.transaction_id == req.mbap.transaction_id)
        self.assertTrue(res.mbap.protocol_id == req.mbap.protocol_id)
        self.assertTrue(res.mbap.unit_id == req.mbap.unit_id)
        self.assertTrue(res.mbap.length == 3 and res.data_length == 9)
        self.assertTrue(res.pdu.func_code == req.pdu.func_code + 0x80)
        self.assertTrue(res.raw_data[8] == ModbusCode.ILLEGAL_DATA_ADDRESS)

    def test_input_bits(self):
        session = ModbusDataSession()
        # test read success
        req_data = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x02, 0x00, 0x00, 0x00, 10]
        session.set_request_data(req_data)
        service.process_session(session)
        req = session.request
        res = session.response
        self.assertTrue(res.mbap.transaction_id == req.mbap.transaction_id)
        self.assertTrue(res.mbap.protocol_id == req.mbap.protocol_id)
        self.assertTrue(res.mbap.unit_id == req.mbap.unit_id)
        self.assertTrue(res.mbap.length == 5 and res.data_length == 11)
        self.assertTrue(res.pdu.func_code == req.pdu.func_code)

        # test read failed
        req_data = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x02, 0x00, 0x00, 0x00, 20]
        session.set_request_data(req_data)
        service.process_session(session)
        req = session.request
        res = session.response
        self.assertTrue(res.mbap.transaction_id == req.mbap.transaction_id)
        self.assertTrue(res.mbap.protocol_id == req.mbap.protocol_id)
        self.assertTrue(res.mbap.unit_id == req.mbap.unit_id)
        self.assertTrue(res.mbap.length == 3 and res.data_length == 9)
        self.assertTrue(res.pdu.func_code == req.pdu.func_code + 0x80)
        self.assertTrue(res.raw_data[8] == ModbusCode.ILLEGAL_DATA_ADDRESS)

    def test_holding_regs(self):
        session = ModbusDataSession()
        # test read success
        req_data = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x03, 0x00, 0x00, 0x00, 10]
        session.set_request_data(req_data)
        service.process_session(session)
        req = session.request
        res = session.response
        self.assertTrue(res.mbap.transaction_id == req.mbap.transaction_id)
        self.assertTrue(res.mbap.protocol_id == req.mbap.protocol_id)
        self.assertTrue(res.mbap.unit_id == req.mbap.unit_id)
        self.assertTrue(res.mbap.length == 23 and res.data_length == 29)
        self.assertTrue(res.pdu.func_code == req.pdu.func_code)

        # test read failed
        req_data = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x03, 0x00, 0x00, 0x00, 20]
        session.set_request_data(req_data)
        service.process_session(session)
        req = session.request
        res = session.response
        self.assertTrue(res.mbap.transaction_id == req.mbap.transaction_id)
        self.assertTrue(res.mbap.protocol_id == req.mbap.protocol_id)
        self.assertTrue(res.mbap.unit_id == req.mbap.unit_id)
        self.assertTrue(res.mbap.length == 3 and res.data_length == 9)
        self.assertTrue(res.pdu.func_code == req.pdu.func_code + 0x80)
        self.assertTrue(res.raw_data[8] == ModbusCode.ILLEGAL_DATA_ADDRESS)

    def test_input_regs(self):
        session = ModbusDataSession()
        # test read success
        req_data = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x04, 0x00, 0x00, 0x00, 10]
        session.set_request_data(req_data)
        service.process_session(session)
        req = session.request
        res = session.response
        self.assertTrue(res.mbap.transaction_id == req.mbap.transaction_id)
        self.assertTrue(res.mbap.protocol_id == req.mbap.protocol_id)
        self.assertTrue(res.mbap.unit_id == req.mbap.unit_id)
        self.assertTrue(res.mbap.length == 23 and res.data_length == 29)
        self.assertTrue(res.pdu.func_code == req.pdu.func_code)

        # test read failed
        req_data = [0x00, 0x01, 0x00, 0x00, 0x00, 6, 0x01, 0x04, 0x00, 0x00, 0x00, 20]
        session.set_request_data(req_data)
        service.process_session(session)
        req = session.request
        res = session.response
        self.assertTrue(res.mbap.transaction_id == req.mbap.transaction_id)
        self.assertTrue(res.mbap.protocol_id == req.mbap.protocol_id)
        self.assertTrue(res.mbap.unit_id == req.mbap.unit_id)
        self.assertTrue(res.mbap.length == 3 and res.data_length == 9)
        self.assertTrue(res.pdu.func_code == req.pdu.func_code + 0x80)
        self.assertTrue(res.raw_data[8] == ModbusCode.ILLEGAL_DATA_ADDRESS)

if __name__ == '__main__':
    modbus_data = ModbusData(10, 10, 10, 10)
    
    service = ModbusDataService(modbus_data)

    unittest.main()
