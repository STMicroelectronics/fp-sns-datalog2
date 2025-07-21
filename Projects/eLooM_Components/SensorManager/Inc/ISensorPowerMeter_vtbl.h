/**
  ******************************************************************************
  * @file    ISensorPowerMeter_vtbl.h
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

#ifndef INCLUDE_ISENSORPOWERMETER_VTBL_H_
#define INCLUDE_ISENSORPOWERMETER_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ISensor.h"
#include "ISensor_vtbl.h"

/**
  * Create a type name for ISensorPowerMeter_vtbl.
  */
typedef struct _ISensorPowerMeter_vtbl ISensorPowerMeter_vtbl;

struct _ISensorPowerMeter_vtbl
{
  ISensor_vtbl vtbl;
  uint32_t (*SensorGetADCConversionTime)(ISensorPowerMeter_t *_this);
  uint32_t (*SensorGetRShunt)(ISensorPowerMeter_t *_this);
  sys_error_code_t (*SensorSetADCConversionTime)(ISensorPowerMeter_t *_this, uint32_t adc_conversion_time);
  sys_error_code_t (*SensorSetRShunt)(ISensorPowerMeter_t *_this, uint32_t r_shunt);
};

struct _ISensorPowerMeter_t
{
  /**
    * Pointer to the virtual table for the class.
    */
  const ISensorPowerMeter_vtbl *vptr;
};

// Inline functions definition
// ***************************

static inline uint32_t ISensorGetADCConversionTime(ISensorPowerMeter_t *_this)
{
  return _this->vptr->SensorGetADCConversionTime(_this);
}

static inline uint32_t ISensorGetRShunt(ISensorPowerMeter_t *_this)
{
  return _this->vptr->SensorGetRShunt(_this);
}

static inline sys_error_code_t ISensorSetADCConversionTime(ISensorPowerMeter_t *_this, uint32_t adc_conversion_time)
{
  return _this->vptr->SensorSetADCConversionTime(_this, adc_conversion_time);
}

static inline sys_error_code_t ISensorSetRShunt(ISensorPowerMeter_t *_this, uint32_t r_shunt)
{
  return _this->vptr->SensorSetRShunt(_this, r_shunt);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ISENSORPOWERMETER_VTBL_H_ */
