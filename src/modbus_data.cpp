/*
 * Software License Agreement (MIT License)
 *
 * Copyright (c) 2022, Vinman, Inc.
 * All rights reserved.
 *
 * Author: Vinman <vinman.cub@gmail.com>
 */

#include <cstdlib>
#include "modbus_data.h"

ModbusData* StaticModbusData::modbus_data_ = NULL;

/**************** ModbusData *****************/

ModbusData::ModbusData(unsigned int coil_bit_count, unsigned int input_bit_count,
    unsigned int holding_reg_count, unsigned int input_reg_count, 
    unsigned int coil_bit_start_addr, unsigned int input_bit_start_addr,
    unsigned int holding_reg_start_addr, unsigned int input_reg_start_addr)
: coil_bit_start_addr_(coil_bit_start_addr), input_bit_start_addr_(input_bit_start_addr)
, holding_reg_start_addr_(holding_reg_start_addr), input_reg_start_addr_(input_reg_start_addr)
, coil_bit_count_(coil_bit_count), input_bit_count_(input_bit_count)
, holding_reg_count_(holding_reg_count), input_reg_count_(input_reg_count)
{
  coil_bits_ = new modbus_base_data<unsigned char>[coil_bit_count_];
  input_bits_ = new modbus_base_data<unsigned char>[input_bit_count_];
  holding_regs_ = new modbus_base_data<unsigned short>[holding_reg_count_];
  input_regs_ = new modbus_base_data<unsigned short>[input_reg_count_];
}

ModbusData::~ModbusData()
{
  if (coil_bits_ != NULL) {
    delete[] coil_bits_;
    coil_bits_ = NULL;
  }
  if (input_bits_ != NULL) {
    delete[] input_bits_;
    input_bits_ = NULL;
  }
  if (holding_regs_ != NULL) {
    delete[] holding_regs_;
    holding_regs_ = NULL;
  }
  if (input_regs_ != NULL) {
    delete[] input_regs_;
    input_regs_ = NULL;
  }
}

int ModbusData::read_coil_bits(int addr, int quantity, unsigned char *bits)
{
  int inx = addr - coil_bit_start_addr_;
  if (inx < 0 || inx + quantity > coil_bit_count_)
    return MODBUS_DATA_ILLEGAL_ADDR;
  for (int i = 0; i < quantity; i++) {
    bits[i] = coil_bits_[inx + i].get();
  }
  return MODBUS_NONE;
}

int ModbusData::read_input_bits(int addr, int quantity, unsigned char *bits)
{
  int inx = addr - input_bit_start_addr_;
  if (inx < 0 || inx + quantity > input_bit_count_)
    return MODBUS_DATA_ILLEGAL_ADDR;
  for (int i = 0; i < quantity; i++) {
    bits[i] = input_bits_[inx + i].get();
  }
  return MODBUS_NONE;
}

int ModbusData::read_holding_registers(int addr, int quantity, unsigned short *regs)
{
  int inx = addr - holding_reg_start_addr_;
  if (inx < 0 || inx + quantity > holding_reg_count_)
    return MODBUS_DATA_ILLEGAL_ADDR;
  for (int i = 0; i < quantity; i++) {
    regs[i] = holding_regs_[inx + i].get();
  }
  return MODBUS_NONE;
}

int ModbusData::read_input_registers(int addr, int quantity, unsigned short *regs)
{
  int inx = addr - input_reg_start_addr_;
  if (inx < 0 || inx + quantity > input_reg_count_)
    return MODBUS_DATA_ILLEGAL_ADDR;
  for (int i = 0; i < quantity; i++) {
    regs[i] = input_regs_[inx + i].get();
  }
  return MODBUS_NONE;
}

int ModbusData::write_coil_bits(int addr, unsigned char *bits, int quantity)
{
  int inx = addr - coil_bit_start_addr_;
  if (inx < 0 || inx + quantity > coil_bit_count_)
    return MODBUS_DATA_ILLEGAL_ADDR;
  for (int i = 0; i < quantity; i++) {
    unsigned char bit = bits[i] ? ON : OFF;
    // coil_bits_[inx + i].set(bit);
    if (coil_bits_[inx + i].get() != bit) {
      coil_bits_[inx + i].set(bit);
    }
  }
  return MODBUS_NONE;
}

int ModbusData::write_input_bits(int addr, unsigned char *bits, int quantity)
{
  int inx = addr - input_bit_start_addr_;
  if (inx < 0 || inx + quantity > input_bit_count_)
    return MODBUS_DATA_ILLEGAL_ADDR;
  for (int i = 0; i < quantity; i++) {
    unsigned char bit = bits[i] ? ON : OFF;
    // input_bits_[inx + i].set(bit);
    if (input_bits_[inx + i].get() != bit) {
      input_bits_[inx + i].set(bit);
    }
  }
  return MODBUS_NONE;
}

int ModbusData::write_holding_registers(int addr, unsigned short *regs, int quantity)
{
  int inx = addr - holding_reg_start_addr_;
  if (inx < 0 || inx + quantity > holding_reg_count_)
    return MODBUS_DATA_ILLEGAL_ADDR;
  for (int i = 0; i < quantity; i++) {
    // holding_regs_[inx + i].set(regs[i]);
    if (holding_regs_[inx + i].get() != regs[i]) {
      holding_regs_[inx + i].set(regs[i]);
    }
  }
  return MODBUS_NONE;
}

int ModbusData::write_input_registers(int addr, unsigned short *regs, int quantity)
{
  int inx = addr - input_reg_start_addr_;
  if (inx < 0 || inx + quantity > input_reg_count_)
    return MODBUS_DATA_ILLEGAL_ADDR;
  for (int i = 0; i < quantity; i++) {
    // input_regs_[inx + i].set(regs[i]);
    if (input_regs_[inx + i].get() != regs[i]) {
      input_regs_[inx + i].set(regs[i]);
    }
  }
  return MODBUS_NONE;
}

int ModbusData::mask_write_holding_register(int addr, unsigned short and_mask, unsigned short or_mask)
{
  int inx = addr - holding_reg_start_addr_;
  if (inx < 0 || inx + 1 > holding_reg_count_)
    return MODBUS_DATA_ILLEGAL_ADDR;
  unsigned short old_val = holding_regs_[inx].get();
  unsigned short new_val = (old_val & and_mask) | (or_mask & ~and_mask);
  // holding_regs_[inx].set(new_val);
  if (old_val != new_val) {
    holding_regs_[inx].set(new_val);
  }
  return MODBUS_NONE;
}

int ModbusData::write_and_read_holding_registers(int w_addr, unsigned short *w_regs, int w_quantity, int r_addr, int r_quantity, unsigned short *r_regs)
{
  int w_inx = w_addr - holding_reg_start_addr_;
  int r_inx = r_addr - holding_reg_start_addr_;
  if (w_inx < 0 || w_inx + w_quantity > holding_reg_count_
    || r_inx < 0 || r_inx + r_quantity > holding_reg_count_)
    return MODBUS_DATA_ILLEGAL_ADDR;
  for (int i = 0; i < w_quantity; i++) {
    // holding_regs_[w_inx + i].set(w_regs[i]);
    if (holding_regs_[w_inx + i].get() != w_regs[i]) {
      holding_regs_[w_inx + i].set(w_regs[i]);
    }
  }
  for (int i = 0; i < r_quantity; i++) {
    r_regs[i] = holding_regs_[r_inx + i].get();
  }
  return MODBUS_NONE;
}

modbus_base_data<unsigned char>* ModbusData::get_coil_bit_struct(int addr)
{
  int inx = addr - coil_bit_start_addr_;
  if (inx < 0 || inx >= coil_bit_count_)
    return NULL;
  return &coil_bits_[inx];
}

modbus_base_data<unsigned char>* ModbusData::get_input_bit_struct(int addr)
{
  int inx = addr - input_bit_start_addr_;
  if (inx < 0 || inx >= input_bit_count_)
    return NULL;
  return &input_bits_[inx];
}

modbus_base_data<unsigned short>* ModbusData::get_holding_register_struct(int addr)
{
  int inx = addr - holding_reg_start_addr_;
  if (inx < 0 || inx >= holding_reg_count_)
    return NULL;
  return &holding_regs_[inx];
}

modbus_base_data<unsigned short>* ModbusData::get_input_register_struct(int addr)
{
  int inx = addr - input_reg_start_addr_;
  if (inx < 0 || inx >= input_reg_count_)
    return NULL;
  return &input_regs_[inx];
}


/**************** StaticModbusData *****************/

void StaticModbusData::set_modbus_data(ModbusData* modbus_data)
{
  modbus_data_ = modbus_data;
}

ModbusData* StaticModbusData::get_modbus_data(void)
{
  return modbus_data_;
}

int StaticModbusData::read_coil_bits(int addr, int quantity, unsigned char *bits)
{
  if (modbus_data_ == NULL) return MODBUS_DATA_NOT_CREATE;
  return modbus_data_->read_coil_bits(addr, quantity, bits);
}

int StaticModbusData::read_input_bits(int addr, int quantity, unsigned char *bits)
{
  if (modbus_data_ == NULL) return MODBUS_DATA_NOT_CREATE;
  return modbus_data_->read_input_bits(addr, quantity, bits);
}

int StaticModbusData::read_holding_registers(int addr, int quantity, unsigned short *regs)
{
  if (modbus_data_ == NULL) return MODBUS_DATA_NOT_CREATE;
  return modbus_data_->read_holding_registers(addr, quantity, regs);
}

int StaticModbusData::read_input_registers(int addr, int quantity, unsigned short *regs)
{
  if (modbus_data_ == NULL) return MODBUS_DATA_NOT_CREATE;
  return modbus_data_->read_input_registers(addr, quantity, regs);
}

int StaticModbusData::write_coil_bits(int addr, unsigned char *bits, int quantity)
{
  if (modbus_data_ == NULL) return MODBUS_DATA_NOT_CREATE;
  return modbus_data_->write_coil_bits(addr, bits, quantity);
}

int StaticModbusData::write_input_bits(int addr, unsigned char *bits, int quantity)
{
  if (modbus_data_ == NULL) return MODBUS_DATA_NOT_CREATE;
  return modbus_data_->write_input_bits(addr, bits, quantity);
}

int StaticModbusData::write_holding_registers(int addr, unsigned short *regs, int quantity)
{
  if (modbus_data_ == NULL) return MODBUS_DATA_NOT_CREATE;
  return modbus_data_->write_holding_registers(addr, regs, quantity);
}

int StaticModbusData::write_input_registers(int addr, unsigned short *regs, int quantity)
{
  if (modbus_data_ == NULL) return MODBUS_DATA_NOT_CREATE;
  return modbus_data_->write_input_registers(addr, regs, quantity);
}

int StaticModbusData::mask_write_holding_register(int addr, unsigned short and_mask, unsigned short or_mask)
{
  if (modbus_data_ == NULL) return MODBUS_DATA_NOT_CREATE;
  return modbus_data_->mask_write_holding_register(addr, and_mask, or_mask);
}

int StaticModbusData::write_and_read_holding_registers(int w_addr, unsigned short *w_regs, int w_quantity, int r_addr, int r_quantity, unsigned short *r_regs)
{
  if (modbus_data_ == NULL) return MODBUS_DATA_NOT_CREATE;
  return modbus_data_->write_and_read_holding_registers(w_addr, w_regs, w_quantity, r_addr, r_quantity, r_regs);
}

modbus_base_data<unsigned char>* StaticModbusData::get_coil_bit_struct(int addr)
{
  if (modbus_data_ == NULL) return NULL;
  return modbus_data_->get_coil_bit_struct(addr);
}

modbus_base_data<unsigned char>* StaticModbusData::get_input_bit_struct(int addr)
{
  if (modbus_data_ == NULL) return NULL;
  return modbus_data_->get_input_bit_struct(addr);
}

modbus_base_data<unsigned short>* StaticModbusData::get_holding_register_struct(int addr)
{
  if (modbus_data_ == NULL) return NULL;
  return modbus_data_->get_holding_register_struct(addr);
}

modbus_base_data<unsigned short>* StaticModbusData::get_input_register_struct(int addr)
{
  if (modbus_data_ == NULL) return NULL;
  return modbus_data_->get_input_register_struct(addr);
}