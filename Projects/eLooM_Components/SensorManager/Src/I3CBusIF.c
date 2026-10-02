/**
  ******************************************************************************
  * @file    I3CBusIF.c
  * @author  SRA - MCD
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */

#include "I3CBusIF.h"


// Private functions declaration
// *****************************

// Private variables
// *****************

// Public API implementation.
// **************************

ABusIF *I3CBusIFAlloc(uint16_t who_am_i, uint8_t static_address, uint8_t dynamic_address, uint8_t auto_inc)
{
  I3CBusIF *_this = NULL;

  _this = SysAlloc(sizeof(I3CBusIF));
  if (_this != NULL)
  {
    ABusIFInit(&_this->super, who_am_i);
    _this->static_address = static_address;
    _this->dynamic_address = dynamic_address;
    _this->auto_inc = auto_inc;
    _this->reg_addr_size = I3C_BUS_REG_ADDR_SIZE_8BIT;
    _this->ccc_config_done = 0U;
    _this->last_io_error = SYS_NO_ERROR_CODE;

    // initialize the software resources
    if (TX_SUCCESS != tx_semaphore_create(&_this->sync_obj, "I3C_IP_S", 0))
    {
      SysFree(_this);
      _this = NULL;
    }
    else
    {
      ABusIFSetHandle(&_this->super, _this);
    }
  }

  return (ABusIF *)_this;
}

sys_error_code_t I3CBusIFSetRegAddrSize(I3CBusIF *_this, uint8_t reg_addr_size)
{
  assert_param(_this != NULL);

  if ((reg_addr_size != I3C_BUS_REG_ADDR_SIZE_8BIT) &&
      (reg_addr_size != I3C_BUS_REG_ADDR_SIZE_16BIT))
  {
    return SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  _this->reg_addr_size = reg_addr_size;

  return SYS_NO_ERROR_CODE;
}

sys_error_code_t I3CBusIFWaitIOComplete(I3CBusIF *_this)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (TX_SUCCESS != tx_semaphore_get(&_this->sync_obj, TX_WAIT_FOREVER))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }
  else
  {
    res = _this->last_io_error;
    _this->last_io_error = SYS_NO_ERROR_CODE;
  }

  return res;
}

sys_error_code_t I3CBusIFNotifyIOComplete(I3CBusIF *_this)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (TX_SUCCESS != tx_semaphore_put(&_this->sync_obj))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }

  return res;
}

// Private functions definition
// ****************************

