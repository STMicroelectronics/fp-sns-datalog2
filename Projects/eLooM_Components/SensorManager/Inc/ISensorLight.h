/**
  ******************************************************************************
  * @file    ISensorLight.h
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

#ifndef INCLUDE_ISENSORLIGHT_H_
#define INCLUDE_ISENSORLIGHT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/systypes.h"
#include "services/syserror.h"
#include "services/systp.h"
#include "SensorDef.h"
#include "ISensor.h"

/**
  * Create  type name for ISensorLight.
  */
typedef struct _ISensorLight_t ISensorLight_t;

// Public API declaration
//***********************
/** Public interface **/
static inline sys_error_code_t ISensorGetIntermeasurementTime(ISensorLight_t *_this, uint32_t *p_measured,
                                                              uint32_t *p_nominal);
static inline uint32_t ISensorGetExposureTime(ISensorLight_t *_this);
static inline sys_error_code_t ISensorGetLightGain(ISensorLight_t *_this, float_t *LightGain);
static inline sys_error_code_t ISensorSetIntermeasurementTime(ISensorLight_t *_this, uint32_t intermeasurement_time);
static inline sys_error_code_t ISensorSetExposureTime(ISensorLight_t *_this, uint32_t exposure_time);
static inline sys_error_code_t ISensorSetLightGain(ISensorLight_t *_this, float_t LightGain, uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ISENSORLIGHT_H_ */
