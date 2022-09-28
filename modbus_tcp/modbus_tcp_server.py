#!/usr/bin/env python3
# Software License Agreement (BSD License)
#
# Copyright (c) 2022, Vinman, Inc.
# All rights reserved.
#
# Author: Vinman <vinman.cub@gmail.com>

import sys
import socket
import logging
import selectors
import threading
import functools
from .modbus_tcp_data import ModbusDataService
from .log import logger as default_logger


class ModbusTcpServer(object):
    def __init__(self, host='127.0.0.1', port=502, logger=None):
        sock_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock_server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock_server.bind((host, port))
        sock_server.listen(10)
        sock_server.setblocking(False)
        self.__sel = selectors.DefaultSelector()
        self.__sel.register(sock_server, selectors.EVENT_READ, self.__accept_callback)
        self.__sock_map = {}  # {sock: {'addr': addr, 'service': ModbusDataService(...)}}
        self.__thread = threading.Thread(target=self.__loop_thread, daemon=True)
        self.__modbus_data = None
        self.__alive = False
        self.logger = logger if logger and isinstance(logger, logging.Logger) else default_logger
    
    def start(self):
        if self.__alive:
            return
        if self.__modbus_data is None:
            self.logger.error('please set_modbus_data at first')
            return
        self.__alive = True
        self.__thread.start()
    
    def run(self):
        self.start()
        self.__thread.join()
    
    def set_modbus_data(self, modbus_data):
        self.__modbus_data = modbus_data
    
    def __accept_callback(self, sock, mask):
        conn, addr = sock.accept()
        self.logger.info('A new connection, addr={}'.format(addr))
        conn.setblocking(False)
        self.__sel.register(conn, selectors.EVENT_READ, self.__read_callback)
        self.__sock_map[conn] = {'addr': addr, 'service': ModbusDataService(self.__modbus_data, logger=self.logger)}
    
    def __read_callback(self, sock, mask):
        info = self.__sock_map[sock]
        try:
            data = sock.recv(256)
            if data:
                info['service'].process_data(data, callback=functools.partial(self.__process_callback, sock=sock))
            else:
                self.__sock_close(sock)
        except Exception as e:
            self.logger.error('[{}] read exception, {}'.format(info['addr'], e))
            self.__sock_close(sock)
    
    def __process_callback(self, req_data, res_data, sock=None):
        self.logger.debug('req: {}'.format(req_data))
        self.logger.debug('res: {}'.format(res_data))
        if self.__sock_send(sock, res_data) != 0:
            self.__sock_close(sock)
    
    def __sock_close(self, sock):
        if sock is None:
            return
        info = self.__sock_map[sock]
        self.logger.info('[{}] sock is closed'.format(info['addr']))
        self.__sel.unregister(sock)
        try:
            sock.close()
        except:
            pass
        del self.__sock_map[sock]
    
    def __sock_send(self, sock, data):
        if sock is not None:
            try:
                sock.send(data)
            except Exception as e:
                info = self.__sock_map[sock]
                self.logger.error('[{}] sock send exception, {}'.format(info['addr'], e))
                return -1
        return 0

    def __loop_thread(self):
        self.logger.info('modbus tcp server running ....')
        while True:
            try:
                events = self.__sel.select()
                if not events:  # InterruptedError
                    break
                for key, mask in events:
                    try:
                        callback = key.data
                        callback(key.fileobj, mask)
                    except Exception as e:
                        self.logger.error('callback exception, id={}, {}'.format(id(key.fileobj), e))
            except Exception as e:
                self.logger.error('select exception: {}'.format(e))
                break

        self.logger.info('modbug tcp server is over')
