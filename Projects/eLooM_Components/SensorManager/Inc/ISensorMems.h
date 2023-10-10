/**
  ******************************************************************************
  * @file    ISensorMems.h
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

#ifndef INCLUDE_ISENSORMEMS_H_
#define INCLUDE_ISENSORMEMS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/systypes.h"
#include "services/syserror.h"
#include "services/systp.h"
#include "SensorDef.h"
#include "ISensor.h"

/**
  * Create  type name for ISensorMems.
  */
typedef struct _ISensorMems_t ISensorMems_t;

// Public API declaration
//***********************
/** Public interface **/
static inline sys_error_code_t ISensorGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal);
static inline float ISensorGetFS(ISensorMems_t *_this);
static inline float ISensorGetSensitivity(ISensorMems_t *_this);
static inline sys_error_code_t ISensorSetODR(ISensorMems_t *_this, float odr);
static inline sys_error_code_t ISensorSetFS(ISensorMems_t *_this, float fs);
static inline sys_error_code_t ISensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ISENSORMEMS_H_ */
