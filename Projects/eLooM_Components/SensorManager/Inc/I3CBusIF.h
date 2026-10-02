/**
  ******************************************************************************
  * @file    I3CBusIF.h
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
#ifndef I3CBUSIF_H_
#define I3CBUSIF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ABusIF.h"
#include "tx_api.h"

#define I3C_BUS_REG_ADDR_SIZE_8BIT   (1U)
#define I3C_BUS_REG_ADDR_SIZE_16BIT  (2U)

/**
  * Create a type name for _I3CBusIF.
  */
typedef struct _I3CBusIF I3CBusIF;

/**
  * Specifies the I3C interface for a generic sensor.
  */
struct _I3CBusIF
{
  /**
    * The bus connector encapsulates the function pointer to read and write in the bus,
    * and it is compatible with the the ST universal sensor driver.
    */
  ABusIF super;

  /**
    * Slave address.
    */
  uint16_t static_address;

  /**
    * Dynamic address.
    */
  uint16_t dynamic_address;

  /**
    * Address auto-increment (Multi-byte read/write).
    */
  uint16_t auto_inc;

  /**
    * Register address size in bytes (1 or 2).
    */
  uint8_t reg_addr_size;

  /**
    * Synchronization object used to synchronize the sensor with the bus.
    */
  TX_SEMAPHORE sync_obj;

  /**
    * One-time flag to indicate that CCC startup sequence has been done for this sensor.
    */
  uint8_t ccc_config_done;

  /**
    * Last I/O result produced by the bus task for this sensor.
    */
  sys_error_code_t last_io_error;
};


// Public API declaration
// **********************
/**
  * Initialize a sensor object. It must be called once before using the sensor.
  *
  * @param _this [IN] specifies a sensor object.
  * @param nWhoAmI [IN] specifies the sensor ID. It can be zero.
  * @param static_address [IN] specifies the static I3C address of the sensor. It can be zero if the sensor doesn't have a static address or if the static address is not used.
  * @param dynamic_address [IN] specifies the dynamic I3C address of the sensor. It can be zero if the sensor doesn't have a dynamic address or if the dynamic address is not used.
  * @param auto_inc [IN] specifies the auto-increment value for multi-byte read/write. It can be zero if the sensor doesn't support auto-increment or if auto-increment is not used.
  * @return SYS_NO_EROR_CODE if success, an error code otherwise.
  */
ABusIF *I3CBusIFAlloc(uint16_t who_am_i, uint8_t static_address, uint8_t dynamic_address, uint8_t auto_inc);

sys_error_code_t I3CBusIFSetRegAddrSize(I3CBusIF *_this, uint8_t reg_addr_size);

sys_error_code_t I3CBusIFWaitIOComplete(I3CBusIF *_this);
sys_error_code_t I3CBusIFNotifyIOComplete(I3CBusIF *_this);


// Inline function definition
// **************************


#ifdef __cplusplus
}
#endif

#endif /* I3CBUSIF_H_ */
