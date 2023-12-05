/**
  ******************************************************************************
  * @file    ISensorLight_vtbl.h
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

#ifndef INCLUDE_ISENSORLIGHT_VTBL_H_
#define INCLUDE_ISENSORLIGHT_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ISensor.h"
#include "ISensor_vtbl.h"

/**
  * Create a type name for ISensorLight_vtbl.
  */
typedef struct _ISensorLight_vtbl ISensorLight_vtbl;

struct _ISensorLight_vtbl
{
  ISensor_vtbl vtbl;
  sys_error_code_t (*SensorGetIntermeasurementTime)(ISensorLight_t *_this, uint32_t *p_measured, uint32_t *p_nominal);
  float (*SensorGetExposureTime)(ISensorLight_t *_this);
  sys_error_code_t (*SensorGetLightGain)(ISensorLight_t *_this, float *LightGain);
  sys_error_code_t (*SensorSetIntermeasurementTime)(ISensorLight_t *_this, uint32_t intermeasurement_time);
  sys_error_code_t (*SensorSetExposureTime)(ISensorLight_t *_this, float exposure_time);
  sys_error_code_t (*SensorSetLightGain)(ISensorLight_t *_this, float LightGain, uint8_t channel);
};

struct _ISensorLight_t
{
  /**
    * Pointer to the virtual table for the class.
    */
  const ISensorLight_vtbl *vptr;
};

// Inline functions definition
// ***************************

static inline sys_error_code_t ISensorGetIntermeasurementTime(ISensorLight_t *_this, uint32_t *p_measured,
                                                              uint32_t *p_nominal)
{
  return _this->vptr->SensorGetIntermeasurementTime(_this, p_measured, p_nominal);
}

static inline float ISensorGetExposureTime(ISensorLight_t *_this)
{
  return _this->vptr->SensorGetExposureTime(_this);
}

static inline sys_error_code_t ISensorGetLightGain(ISensorLight_t *_this, float *LightGain)
{
  return _this->vptr->SensorGetLightGain(_this, LightGain);
}

static inline sys_error_code_t ISensorSetIntermeasurementTime(ISensorLight_t *_this, uint32_t intermeasurement_time)
{
  return _this->vptr->SensorSetIntermeasurementTime(_this, intermeasurement_time);
}

static inline sys_error_code_t ISensorSetExposureTime(ISensorLight_t *_this, float exposure_time)
{
  return _this->vptr->SensorSetExposureTime(_this, exposure_time);
}

static inline sys_error_code_t ISensorSetLightGain(ISensorLight_t *_this, float LightGain, uint8_t channel)
{
  return _this->vptr->SensorSetLightGain(_this, LightGain, channel);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ISENSORLIGHT_VTBL_H_ */
