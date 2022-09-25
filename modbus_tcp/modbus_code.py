#!/usr/bin/env python3
# Software License Agreement (BSD License)
#
# Copyright (c) 2022, Vinman, Inc.
# All rights reserved.
#
# Author: Vinman <vinman.cub@gmail.com>

class ModbusFunCode:
    FC_READ_COILS           = 0x01
    FC_READ_DISCRETE_INPUTS = 0x02
    FC_READ_HOLDING_REGS    = 0x03
    FC_READ_INPUT_REGS      = 0x04
    FC_WRITE_SINGLE_COIL    = 0x05
    FC_WRITE_SINGLE_REG     = 0x06
    FC_WRITE_MULTIPLE_COILS = 0x0F
    FC_WRITE_MULTIPLE_REGS  = 0x10
    FC_MASK_WRITE_REG       = 0x16
    FC_WRITE_AND_READ_REGS  = 0x17


class ModbusCode:
    MODBUS_DATA_NOT_CREATE      = -1
    SUCCESS                     = 0
    ILLEGAL_FUNCTION            = 0x01
    ILLEGAL_DATA_ADDRESS        = 0x02
    ILLEGAL_DATA_VALUE          = 0x03
    SLAVE_DEVICE_FAILURE        = 0x04
    ACKNOWLEDGE                 = 0x05
    SLAVE_DEVICE_BUSY           = 0x06
    MEMORY_PRAITY_ERROR         = 0x08
    GATEWAY_PATH_UNAVAILABLE    = 0x0A
    GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND = 0x0B
