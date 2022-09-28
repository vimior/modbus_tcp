#!/usr/bin/env python3
# Software License Agreement (BSD License)
#
# Copyright (c) 2022, Vinman, Inc.
# All rights reserved.
#
# Author: Vinman <vinman.cub@gmail.com>

import logging
import functools
import sys

# 日志格式
# LOGGET_FMT = '[%(levelname)s][%(asctime)s][%(filename)s:%(lineno)d] - - %(message)s'
LOGGET_FMT = '[%(levelname)s][%(asctime)s] - - %(message)s'
LOGGET_DATE_FMT = '%Y-%m-%d %H:%M:%S'


class Logger(logging.Logger):
    """
    自定义日志类，单例模式
    """
    def __new__(cls, *args, **kwargs):
        if not hasattr(cls, '_logger'):
            cls._logger = logging.Logger('modbus_tcp')
            stream_handler = logging.StreamHandler(sys.stdout)
            stream_handler.setLevel(logging.DEBUG)
            stream_handler.setFormatter(logging.Formatter(LOGGET_FMT, LOGGET_DATE_FMT))
            cls._logger.addHandler(stream_handler)
        return cls._logger

logger = Logger()
logger.setLevel(logging.INFO)
