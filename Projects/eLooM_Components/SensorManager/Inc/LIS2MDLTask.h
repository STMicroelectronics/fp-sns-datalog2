/**
 ******************************************************************************
 * @file    LIS2MDLTask.h
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
#ifndef LIS2MDLTASK_H_
#define LIS2MDLTASK_H_

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

/**
 * Create  type name for _LIS2MDLTask.
 */
typedef struct _LIS2MDLTask LIS2MDLTask;


// Public API declaration
//***********************

/**
 * Get the ISourceObservable interface for the accelerometer.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the generic object ::ISourceObservable if success,
 * or NULL if out of memory error occurs.
 */
ISourceObservable *LIS2MDLTaskGetMagSensorIF(LIS2MDLTask *_this);


/**
 * Allocate an instance of LIS2MDLTask.
 *
 * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
 *        It must be a GPIO connected to the LIS2MDL sensor and configured in EXTI mode.
 *        If it is NULL then the sensor is configured in polling mode.
 * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
 *        It must be a GPIO identifying the SPI CS Pin.
 * @return a pointer to the generic object ::AManagedTaskEx if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx *LIS2MDLTaskAlloc(const void *pIRQConfig, const void *pCSConfig);

/**
 * Get the Bus interface for the sensor task.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the Bus interface of the sensor.
 */
ABusIF *LIS2MDLTaskGetSensorIF(LIS2MDLTask *_this);

/**
 * Get the ::IEventSrc interface for the sensor task.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the ::IEventSrc interface of the sensor.
 */
IEventSrc *LIS2MDLTaskGetMagEventSrcIF(LIS2MDLTask *_this);

// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* LIS2MDLTASK_H_ */
