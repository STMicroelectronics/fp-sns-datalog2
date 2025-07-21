/**
  ******************************************************************************
  * @file    ISensor.h
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

#ifndef INCLUDE_ISENSOR_H_
#define INCLUDE_ISENSOR_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "SensorDef.h"
#include "services/ISourceObservable.h"


/**
  * Create  type name for ISensor.
  */
typedef struct _ISensor_t ISensor_t;


// Public API declaration
//***********************
/** Public interface **/
static inline sys_error_code_t ISensorEnable(ISensor_t *_this);
static inline sys_error_code_t ISensorDisable(ISensor_t *_this);
static inline boolean_t ISensorIsEnabled(ISensor_t *_this);
static inline SensorDescriptor_t ISensorGetDescription(ISensor_t *_this);
static inline SensorStatus_t ISensorGetStatus(ISensor_t *_this);
static inline SensorStatus_t *ISensorGetStatusPointer(ISensor_t *_this);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ISENSOR_H_ */
