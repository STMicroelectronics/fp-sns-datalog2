/**
  ******************************************************************************
  * @file    I2CBSBusIF.h
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
#ifndef I2CBSBUSIF_H_
#define I2CBSBUSIF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ABusIF.h"
#include "tx_api.h"
#include "I2CBusIF.h"


/**
  * Create a type name for _I2CBSBusIF.
  */
typedef struct _I2CBSBusIF I2CBSBusIF;

/**
  * Specifies the I2C interface for a generic sensor.
  */
struct _I2CBSBusIF
{
  /**
    * I2CBSBusIF extends I2CBusIF
    */

  /**
    * The bus connector encapsulates the function pointer to read and write in the bus,
    * and it is compatible with the the ST universal sensor driver.
    */
  ABusIF super;

  /**
    * Slave address.
    */
  uint16_t address;

  /**
    * Address auto-increment (Multi-byte read/write).
    */
  uint16_t auto_inc;

  /**
    * Synchronization object used to synchronize the sensor with the bus.
    */
  TX_SEMAPHORE sync_obj;

  uint8_t address_size;

  uint8_t transmit_receive;

  uint8_t i2c_ack;

  /**
    * Board Select GPIO Port
    */
  GPIO_TypeDef *p_bs_gpio_port;

  /**
    * Board Select GPIO Pin.
    */
  uint16_t bs_gpio_pin;

};


// Public API declaration
// **********************

ABusIF *I2CBSBusIFAlloc(uint16_t who_am_i, uint8_t address, GPIO_TypeDef *p_port, uint16_t pin, uint8_t auto_inc);
ABusIF *I2CBSBusIFAlloc16(uint16_t who_am_i, uint16_t address, GPIO_TypeDef *p_port, uint16_t pin, uint8_t auto_inc);

// Inline function definition
// **************************

#ifdef __cplusplus
}
#endif

#endif /* I2CBSBUSIF_H_ */
