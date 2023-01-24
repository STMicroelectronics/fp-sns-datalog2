/**
  ******************************************************************************
  * @file    IIS3DWBTask.h
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
#ifndef IIS3DWBTASK_H_
#define IIS3DWBTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "services/AManagedTaskEx.h"
#include "services/AManagedTaskEx_vtbl.h"
#include "ABusIF.h"
#include "events/DataEventSrc.h"
#include "events/DataEventSrc_vtbl.h"
#include "ISensor.h"
#include "ISensor_vtbl.h"

//TODO: STF.Begin - where should these be defined ?
#define IIS3DWB_MAX_DRDY_PERIOD           (1.0)
#ifndef IIS3DWB_MAX_WTM_LEVEL
#define IIS3DWB_MAX_WTM_LEVEL             (256)
#endif
#define IIS3DWB_MIN_WTM_LEVEL             (16)
#define IIS3DWB_MAX_SAMPLES_PER_IT        (IIS3DWB_MAX_WTM_LEVEL)
//STF.End

#define IIS3DWB_CFG_MAX_LISTENERS           2

/**
  * Create  type name for _IIS3DWBTask.
  */
typedef struct _IIS3DWBTask IIS3DWBTask;


// Public API declaration
//***********************

/**
  * Get the ISourceObservable interface for the accelerometer.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *IIS3DWBTaskGetAccSensorIF(IIS3DWBTask *_this);

/**
  * Allocate an instance of IIS3DWBTask.
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the IIS3DWB sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IIS3DWBTaskAlloc(const void *pIRQConfig, const void *pCSConfig);

/**
  * Get the SPI interface for the sensor task.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the SPI interface of the sensor.
  */
ABusIF *IIS3DWBTaskGetSensorIF(IIS3DWBTask *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *IIS3DWBTaskGetEventSrcIF(IIS3DWBTask *_this);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* IIS3DWBTASK_H_ */
