/**
 ******************************************************************************
 * @file    LIS2DU12Task.h
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
#ifndef LIS2DU12TASK_H_
#define LIS2DU12TASK_H_

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

#define LIS2DU12_MAX_DRDY_PERIOD           (1.0)    /* seconds */

#ifndef LIS2DU12_MAX_WTM_LEVEL
#define LIS2DU12_MAX_WTM_LEVEL             (92)    /* samples */
#endif
#define LIS2DU12_MIN_WTM_LEVEL             (16)
#define LIS2DU12_MAX_SAMPLES_PER_IT        (LIS2DU12_MAX_WTM_LEVEL)

#define LIS2DU12_CFG_MAX_LISTENERS         2

/**
 * Create  type name for _LIS2DU12Task.
 */
typedef struct _LIS2DU12Task LIS2DU12Task;

// Public API declaration
//***********************

/**
 * Get the ISourceObserver interface for the accelerometer.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the generic object ::ISourceObserver if success,
 * or NULL if out of memory error occurs.
 */
ISourceObservable* LIS2DU12TaskGetAccSensorIF(LIS2DU12Task *_this);

/**
 * Allocate an instance of IIS3DWBTask.
 *
 * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
 *        It must be a GPIO connected to the LIS2DU12 sensor and configured in EXTI mode.
 *        If it is NULL then the sensor is configured in polling mode.
 * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
 *        It must be a GPIO identifying the SPI CS Pin.
 * @return a pointer to the generic object ::AManagedTaskEx if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx* LIS2DU12TaskAlloc(const void *pIRQConfig, const void *pCSConfig);

/**
 * Get the Bus interface for the sensor task.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the Bus interface of the sensor.
 */
ABusIF* LIS2DU12TaskGetSensorIF(LIS2DU12Task *_this);

/**
 * Get the ::IEventSrc interface for the sensor task.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the ::IEventSrc interface of the sensor.
 */
IEventSrc* LIS2DU12TaskGetEventSrcIF(LIS2DU12Task *_this);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* LIS2DU12TASK_H_ */
