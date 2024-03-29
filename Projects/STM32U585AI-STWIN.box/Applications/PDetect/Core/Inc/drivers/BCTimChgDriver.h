/**
  ******************************************************************************
  * @file    BCTimChgDriver.h
  * @author  SRA
  * @brief   Driver to support the battery charger STBC02
  *
  * This file implements the Timer Driver to detect the CHG pin frequency of STBC02.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#ifndef INC_DRIVERS_BCTIMCHGDRIVER_H_
#define INC_DRIVERS_BCTIMCHGDRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "drivers/IDriver.h"
#include "drivers/IDriver_vtbl.h"
#include "mx.h"


/**
  * Create  type name for _BCDriver_t.
  */
typedef struct _BCTimChgDriverParams_t BCTimChgDriverParams_t;

/**
  * Create  type name for _BCDriver_t.
  */
typedef struct _BCTimChgDriver_t BCTimChgDriver_t;

/**
  * Initialization parameters for the driver.
  */
struct _BCTimChgDriverParams_t
{
  /**
    * Specifies the peripheral HAL initialization parameters generated by CubeMX.
    * This is the hardware timer used to implement the 1-wire communication.
    */
  MX_TIMParams_t *p_mx_tim_cfg;

  /**
    * Specifies the peripheral HAL initialization parameters generated by CubeMX.
    * This is the GPIO used to implement the STBC02 state reading.
    */
  MX_GPIOParams_t *p_mx_gpio_chg_cfg;

  /**
    * Specifies the peripheral HAL initialization parameters generated by CubeMX.
    * This is the GPIO used to implement the STBC02 enabling.
    */
  MX_GPIOParams_t *p_mx_gpio_cen_cfg;

};

/**
  *  BCDriver_t internal structure.
  */
struct _BCTimChgDriver_t
{
  /**
    * Base class object.
    */
  IDriver super;

  /* Driver variables should be added here. */

  /**
    * Specifies all the configuration parameters for the TIM peripheral linked to
    * an instance of this class. It based on the HAL driver generated by CubeMX.
    */
  BCTimChgDriverParams_t mx_handle;

};


/** Public API declaration */
/***************************/

/**
  * Allocate an instance of BCDriver_t. The driver is allocated
  * in the FreeRTOS heap.
  *
  * @return a pointer to the generic interface ::IDriver if success,
  * or SYS_OUT_OF_MEMORY_ERROR_CODE otherwise.
  */
IDriver *BCTimChgDriverAlloc(void);

/**
  * Register the capture callback with the driver.
  *
  * @param _this [IN] specifies a pointer to a driver object.
  * @param callback_f specifies a callback function.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
  */
sys_error_code_t BCTimChgDriverRegisterCaptureCallback(BCTimChgDriver_t *_this, pTIM_CallbackTypeDef callback_f);


/** Inline functions definition */
/********************************/

#ifdef __cplusplus
}
#endif

#endif /* INC_DRIVERS_BCTIMCHGDRIVER_H_ */
