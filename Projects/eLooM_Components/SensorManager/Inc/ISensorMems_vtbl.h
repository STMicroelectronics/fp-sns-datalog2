/**
  ******************************************************************************
  * @file    ISensorMems_vtbl.h
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

#ifndef INCLUDE_ISENSORMEMS_VTBL_H_
#define INCLUDE_ISENSORMEMS_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ISensor.h"
#include "ISensor_vtbl.h"

/**
  * Create a type name for ISensorMems_vtbl.
  */
typedef struct _ISensorMems_vtbl ISensorMems_vtbl;

struct _ISensorMems_vtbl
{
  ISensor_vtbl vtbl;
  sys_error_code_t (*SensorGetODR)(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal);
  float_t (*SensorGetFS)(ISensorMems_t *_this);
  float_t (*SensorGetSensitivity)(ISensorMems_t *_this);
  sys_error_code_t (*SensorSetODR)(ISensorMems_t *_this, float_t odr);
  sys_error_code_t (*SensorSetFS)(ISensorMems_t *_this, float_t fs);
  sys_error_code_t (*SensorSetFifoWM)(ISensorMems_t *_this, uint16_t fifoWM);
};

struct _ISensorMems_t
{
  /**
    * Pointer to the virtual table for the class.
    */
  const ISensorMems_vtbl *vptr;
};

// Inline functions definition
// ***************************

static inline sys_error_code_t ISensorGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal)
{
  return _this->vptr->SensorGetODR(_this, p_measured, p_nominal);
}

static inline float_t ISensorGetFS(ISensorMems_t *_this)
{
  return _this->vptr->SensorGetFS(_this);
}

static inline float_t ISensorGetSensitivity(ISensorMems_t *_this)
{
  return _this->vptr->SensorGetSensitivity(_this);
}

static inline sys_error_code_t ISensorSetODR(ISensorMems_t *_this, float_t odr)
{
  return _this->vptr->SensorSetODR(_this, odr);
}

static inline sys_error_code_t ISensorSetFS(ISensorMems_t *_this, float_t fs)
{
  return _this->vptr->SensorSetFS(_this, fs);
}

static inline sys_error_code_t ISensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  if (_this->vptr->SensorSetFifoWM != NULL)
  {
    return _this->vptr->SensorSetFifoWM(_this, fifoWM);
  }
  return SYS_INVALID_FUNC_CALL_ERROR_CODE;
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ISENSORMEMS_VTBL_H_ */
