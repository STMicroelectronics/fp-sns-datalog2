/**
 ******************************************************************************
 * @file    LSM6DSV16XTask.h
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
#ifndef LSM6DSV16XTASK_H_
#define LSM6DSV16XTASK_H_

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
#include "ISensorMlc.h"
#include "ISensorMlc_vtbl.h"

//TODO: STF.Begin - where should these be defined ?
#define LSM6DSV16X_MAX_DRDY_PERIOD           (1.0)    /* seconds */

#ifndef LSM6DSV16X_MAX_WTM_LEVEL
#define LSM6DSV16X_MAX_WTM_LEVEL             (256)    /* samples */
#endif

#define LSM6DSV16X_MIN_WTM_LEVEL             (16)     /* samples */
#define LSM6DSV16X_MAX_SAMPLES_PER_IT        (LSM6DSV16X_MAX_WTM_LEVEL)
//STF.End

#define LSM6DSV16X_CFG_MAX_LISTENERS         2

/**
 * Create a type name for _LSM6DSV16XTask.
 */
typedef struct _LSM6DSV16XTask LSM6DSV16XTask;

// Public API declaration
//***********************

/**
 * Get the ISourceObservable interface for the accelerometer.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the generic object ::ISourceObservable if success,
 * or NULL if out of memory error occurs.
 */
ISourceObservable* LSM6DSV16XTaskGetAccSensorIF(LSM6DSV16XTask *_this);

/**
 * Get the ISourceObservable interface for the gyroscope.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the generic object ::ISourceObservable if success,
 * or NULL if out of memory error occurs.
 */
ISourceObservable* LSM6DSV16XTaskGetGyroSensorIF(LSM6DSV16XTask *_this);

/**
  * Get the ISourceObservable interface for mlc.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *LSM6DSV16XTaskGetMlcSensorIF(LSM6DSV16XTask *_this);

/**
  * Get the ISensorMlc interface.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISensorMlc if success,
  * or NULL if out of memory error occurs.
  */
ISensorMlc_t *LSM6DSV16XTaskGetSensorMlcIF(LSM6DSV16XTask *_this);

/**
 * Allocate an instance of LSM6DSV16XTask.
 *
 * @param pIRQ1Config [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
 *        It must be a GPIO connected to the ISM33DHCX sensor and configured in EXTI mode.
 *        If it is NULL then the sensor is configured in polling mode.
 * @param pMLCConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
 *        It must be a GPIO connected to the ISM33DHCX MLC and configured in EXTI mode.
 * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
 *        It must be a GPIO identifying the SPI CS Pin.
 * @return a pointer to the generic object ::AManagedTaskEx if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx* LSM6DSV16XTaskAlloc(const void *pIRQConfig, const void *pMLCConfig, const void *pCSConfig);

/**
 * Get the Bus interface for the sensor task.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the Bus interface of the sensor.
 */
ABusIF* LSM6DSV16XTaskGetSensorIF(LSM6DSV16XTask *_this);

/**
 * Get the ::IEventSrc interface for the sensor task.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the ::IEventSrc interface of the sensor.
 */
IEventSrc* LSM6DSV16XTaskGetAccEventSrcIF(LSM6DSV16XTask *_this);

/**
 * Get the ::IEventSrc interface for the sensor task.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the ::IEventSrc interface of the sensor.
 */
IEventSrc* LSM6DSV16XTaskGetGyroEventSrcIF(LSM6DSV16XTask *_this);

/**
 * Get the ::IEventSrc interface for the sensor task.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the ::IEventSrc interface of the sensor.
 */
IEventSrc* LSM6DSV16XTaskGetMlcEventSrcIF(LSM6DSV16XTask *_this);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* LSM6DSV16XTASK_H_ */
