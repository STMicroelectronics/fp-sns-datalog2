/**
  ******************************************************************************
  * @file    ISensorRanging_vtbl.h
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

#ifndef INCLUDE_ISENSORRANGING_VTBL_H_
#define INCLUDE_ISENSORRANGING_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ISensor.h"
#include "ISensor_vtbl.h"

/**
  * Create a type name for ISensorRanging_vtbl.
  */
typedef struct _ISensorRanging_vtbl ISensorRanging_vtbl;

struct _ISensorRanging_vtbl
{
  ISensor_vtbl vtbl;
  sys_error_code_t (*SensorGetProfile)(ISensorRanging_t *_this, ProfileConfig_t *p_config);
  sys_error_code_t (*SensorGetIT)(ISensorRanging_t *_this, ITConfig_t *p_it_config);
  uint32_t (*SensorGetAddress)(ISensorRanging_t *_this);
  uint32_t (*SensorGetPowerMode)(ISensorRanging_t *_this);
  sys_error_code_t (*SensorSetRangingFrequency)(ISensorRanging_t *_this, uint32_t frequency);
  sys_error_code_t (*SensorSetRangingResolution)(ISensorRanging_t *_this, uint8_t resolution);
  sys_error_code_t (*SensorSetRangingMode)(ISensorRanging_t *_this, uint8_t mode);
  sys_error_code_t (*SensorSetIntegrationTime)(ISensorRanging_t *_this, uint32_t timing_budget);
  sys_error_code_t (*SensorConfigIT)(ISensorRanging_t *_this, ITConfig_t *p_it_config);
  sys_error_code_t (*SensorSetAddress)(ISensorRanging_t *_this, uint32_t address);
  sys_error_code_t (*SensorSetPowerMode)(ISensorRanging_t *_this, uint32_t power_mode);
};

struct _ISensorRanging_t
{
  /**
    * Pointer to the virtual table for the class.
    */
  const ISensorRanging_vtbl *vptr;
};

// Inline functions definition
// ***************************

static inline sys_error_code_t ISensorGetProfile(ISensorRanging_t *_this, ProfileConfig_t *p_config)
{
  return _this->vptr->SensorGetProfile(_this, p_config);
}

static inline sys_error_code_t ISensorGetIT(ISensorRanging_t *_this, ITConfig_t *p_it_config)
{
  return _this->vptr->SensorGetIT(_this, p_it_config);
}

static inline uint32_t ISensorGetAddress(ISensorRanging_t *_this)
{
  return _this->vptr->SensorGetAddress(_this);
}

static inline uint32_t ISensorGetPowerMode(ISensorRanging_t *_this)
{
  return _this->vptr->SensorGetPowerMode(_this);
}

static inline sys_error_code_t ISensorSetRangingFrequency(ISensorRanging_t *_this, uint32_t frequency)
{
  return _this->vptr->SensorSetRangingFrequency(_this, frequency);
}

static inline sys_error_code_t ISensorSetRangingResolution(ISensorRanging_t *_this, uint8_t resolution)
{
  return _this->vptr->SensorSetRangingResolution(_this, resolution);
}

static inline sys_error_code_t ISensorSetRangingMode(ISensorRanging_t *_this, uint8_t mode)
{
  return _this->vptr->SensorSetRangingMode(_this, mode);
}

static inline sys_error_code_t ISensorSetIntegrationTime(ISensorRanging_t *_this, uint32_t timing_budget)
{
  return _this->vptr->SensorSetIntegrationTime(_this, timing_budget);
}

static inline sys_error_code_t ISensorConfigIT(ISensorRanging_t *_this, ITConfig_t *p_it_config)
{
  return _this->vptr->SensorConfigIT(_this, p_it_config);
}

static inline sys_error_code_t ISensorSetAddress(ISensorRanging_t *_this, uint32_t address)
{
  return _this->vptr->SensorSetAddress(_this, address);
}

static inline sys_error_code_t ISensorSetPowerMode(ISensorRanging_t *_this, uint32_t power_mode)
{
  return _this->vptr->SensorSetPowerMode(_this, power_mode);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ISENSORRANGING_VTBL_H_ */
