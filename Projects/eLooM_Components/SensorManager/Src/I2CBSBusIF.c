/**
  ******************************************************************************
  * @file    I2CBSBusIF.c
  * @author  SRA - MCD
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */

#include "I2CBSBusIF.h"


// Private functions declaration
// *****************************
sys_error_code_t I2CBSBusCtrl(ABusIF *_this, EBusCtrlCmd eCtrlCmd, uint32_t nParams);

// Private variables
// *****************

// Public API implementation.
// **************************

ABusIF *I2CBSBusIFAlloc(uint16_t who_am_i, uint8_t address, GPIO_TypeDef *p_port, uint16_t pin, uint8_t auto_inc)
{
  I2CBSBusIF *_this = NULL;
  _this = (I2CBSBusIF *)SysAlloc(sizeof(I2CBSBusIF));
  if (_this != NULL)
  {
    ABusIFInit(&_this->super, who_am_i);

    _this->address = address;
    _this->auto_inc = auto_inc;
    _this->address_size = I2C_MEMADD_SIZE_8BIT;
    _this->transmit_receive = 0;
    _this->i2c_ack = 0;
    _this->p_bs_gpio_port = p_port;
    _this->bs_gpio_pin = pin;

    // initialize the software resources
    if (TX_SUCCESS != tx_semaphore_create(&_this->sync_obj, "I2C_IP_S", 0))
    {
      SysFree(_this);
      _this = NULL;
    }
    else
    {
      ABusIFSetHandle(&_this->super, _this);
      _this->super.m_pfBusCtrl = I2CBSBusCtrl;
    }
  }
  return (ABusIF *)_this;
}

ABusIF *I2CBSBusIFAlloc16(uint16_t who_am_i, uint16_t address, GPIO_TypeDef *p_port, uint16_t pin, uint8_t auto_inc)
{
  I2CBSBusIF *_this = NULL;
  _this = (I2CBSBusIF *)SysAlloc(sizeof(I2CBSBusIF));
  if (_this != NULL)
  {
    ABusIFInit(&_this->super, who_am_i);

    _this->address = address;
    _this->auto_inc = auto_inc;
    _this->address_size = I2C_MEMADD_SIZE_16BIT;
    _this->transmit_receive = 0;
    _this->i2c_ack = 0;
    _this->p_bs_gpio_port = p_port;
    _this->bs_gpio_pin = pin;

    // initialize the software resources
    if (TX_SUCCESS != tx_semaphore_create(&_this->sync_obj, "I2C_IP_S", 0))
    {
      SysFree(_this);
      _this = NULL;
    }
    else
    {
      ABusIFSetHandle(&_this->super, _this);
      _this->super.m_pfBusCtrl = I2CBSBusCtrl;
    }
  }
  return (ABusIF *)_this;
}

sys_error_code_t I2CBSBusCtrl(ABusIF *_this, EBusCtrlCmd eCtrlCmd, uint32_t nParams)
{
  return 0;
}


