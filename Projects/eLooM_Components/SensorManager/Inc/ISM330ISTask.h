/**
 ******************************************************************************
 * @file    ISM330ISTask.h
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
 
#ifndef ISM330ISTASK_H_
#define ISM330ISTASK_H_

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
#include "ISensorLL.h"
#include "ISensorLL_vtbl.h"

//TODO: STF.Begin - where should these be defined ?
#define ISM330IS_MAX_DRDY_PERIOD           (1.0)    /* seconds */

#ifndef ISM330IS_MAX_WTM_LEVEL
#define ISM330IS_MAX_WTM_LEVEL             (256)    /* samples */
#endif

#define ISM330IS_MIN_WTM_LEVEL             (16)     /* samples */
#define ISM330IS_MAX_SAMPLES_PER_IT        (ISM330IS_MAX_WTM_LEVEL)
//STF.End

#define ISM330IS_CFG_MAX_LISTENERS         2

/**
 * Create a type name for _ISM330ISTask.
 */
typedef struct _ISM330ISTask ISM330ISTask;

// Public API declaration
//***********************

/**
 * Get the ISourceObservable interface for the accelerometer.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the generic object ::ISourceObservable if success,
 * or NULL if out of memory error occurs.
 */
ISourceObservable* ISM330ISTaskGetAccSensorIF(ISM330ISTask *_this);

/**
 * Get the ISourceObservable interface for the gyroscope.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the generic object ::ISourceObservable if success,
 * or NULL if out of memory error occurs.
 */
ISourceObservable* ISM330ISTaskGetGyroSensorIF(ISM330ISTask *_this);

/**
 * Get the ISourceObservable fake interface for ispu.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the generic object ::ISourceObservable if success,
 * or NULL if out of memory error occurs.
 */
ISourceObservable* ISM330ISTaskGetIspuSensorIF(ISM330ISTask *_this);

/**
 * Get the interface for the ispu.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the generic object ::ISourceObservable if success,
 * or NULL if out of memory error occurs.
 */
ISensorLL_t* ISM330ISTaskGetSensorLLIF(ISM330ISTask *_this);

/**
 * Allocate an instance of ISM330ISTask.
 *
 * @param pIRQ1Config [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
 *        It must be a GPIO connected to the ISM33DHCX sensor and configured in EXTI mode.
 *        If it is NULL then the sensor is configured in polling mode.
 * @param pISPUConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
 *        It must be a GPIO connected to the ISM33DHCX ISPU and configured in EXTI mode.
 * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
 *        It must be a GPIO identifying the SPI CS Pin.
 * @return a pointer to the generic object ::AManagedTaskEx if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx* ISM330ISTaskAlloc(const void *pIRQConfig, const void *pISPUConfig, const void *pCSConfig);

/**
 * Get the Bus interface for the sensor task.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the Bus interface of the sensor.
 */
ABusIF* ISM330ISTaskGetSensorIF(ISM330ISTask *_this);

/**
 * Get the ::IEventSrc interface for the sensor task.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the ::IEventSrc interface of the sensor.
 */
IEventSrc* ISM330ISTaskGetAccEventSrcIF(ISM330ISTask *_this);

/**
 * Get the ::IEventSrc interface for the sensor task.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the ::IEventSrc interface of the sensor.
 */
IEventSrc* ISM330ISTaskGetGyroEventSrcIF(ISM330ISTask *_this);

/**
 * Get the ::IEventSrc interface for the sensor task.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the ::IEventSrc interface of the sensor.
 */
IEventSrc* ISM330ISTaskGetMlcEventSrcIF(ISM330ISTask *_this);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* ISM330ISTASK_H_ */
