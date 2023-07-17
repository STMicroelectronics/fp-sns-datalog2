/**
  ******************************************************************************
  * @file    ILPS22QSTask.h
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
#ifndef ILPS22QSTASK_H_
#define ILPS22QSTASK_H_

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


#define ILPS22QS_MAX_DRDY_PERIOD           (1.0)
#ifndef ILPS22QS_MAX_WTM_LEVEL
#define ILPS22QS_MAX_WTM_LEVEL             (128)
#endif
#define ILPS22QS_MIN_WTM_LEVEL             (16)
#define ILPS22QS_MAX_SAMPLES_PER_IT        (ILPS22QS_MAX_WTM_LEVEL)

#define ILPS22QS_CFG_MAX_LISTENERS         2

/**
  * Create a type name for _ILPS22QSTask.
  */
typedef struct _ILPS22QSTask ILPS22QSTask;

// Public API declaration
//***********************

/**
  * Get the ISourceObservable interface for the gyroscope.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *ILPS22QSTaskGetPressSensorIF(ILPS22QSTask *_this);

/**
  * Allocate an instance of ILPS22QSTask.
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ILPS22QS sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *ILPS22QSTaskAlloc(const void *pIRQConfig, const void *pCSConfig);

/**
  * Get the Bus interface for the sensor task.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the Bus interface of the sensor.
  */
ABusIF *ILPS22QSTaskGetSensorIF(ILPS22QSTask *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *ILPS22QSTaskGetPressEventSrcIF(ILPS22QSTask *_this);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* ILPS22QSTASK_H_ */
