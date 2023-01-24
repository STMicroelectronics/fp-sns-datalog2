/**
  ******************************************************************************
  * @file    IIS2DLPCTask.h
  * @author  SRA - MCD
  *
  * @date    30-Jul-2021
  *
  * @brief
  *
  *
  *
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
#ifndef IIS2DLPCTASK_H_
#define IIS2DLPCTASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/systp.h"
#include "services/syserror.h"
#include "services/AManagedTaskEx.h"
#include "services/AManagedTaskEx_vtbl.h"
#include "ABusIF.h"
#include "events/DataEventSrc.h"
#include "events/DataEventSrc_vtbl.h"
#include "ISensor.h"
#include "ISensor_vtbl.h"

#define IIS2DLPC_MAX_DRDY_PERIOD           (1.0)    /* seconds */

#ifndef IIS2DLPC_MAX_WTM_LEVEL
#define IIS2DLPC_MAX_WTM_LEVEL             (16)    /* samples */
#endif

#define IIS2DLPC_CFG_MAX_LISTENERS         2

/**
  * Create  type name for _IIS2DLPCTask.
  */
typedef struct _IIS2DLPCTask IIS2DLPCTask;

// Public API declaration
//***********************

/**
  * Get the ISourceObserver interface for the accelerometer.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObserver if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *IIS2DLPCTaskGetAccSensorIF(IIS2DLPCTask *_this);

/**
  * Allocate an instance of IIS3DWBTask.
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the IIS2DLPC sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IIS2DLPCTaskAlloc(const void *pIRQConfig, const void *pCSConfig);

/**
  * Get the Bus interface for the sensor task.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the Bus interface of the sensor.
  */
ABusIF *IIS2DLPCTaskGetSensorIF(IIS2DLPCTask *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *IIS2DLPCTaskGetEventSrcIF(IIS2DLPCTask *_this);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* IIS2DLPCTASK_H_ */
