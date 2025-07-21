/**
  ******************************************************************************
  * @file    ISensorRanging.h
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

#ifndef INCLUDE_ISENSORRANGING_H_
#define INCLUDE_ISENSORRANGING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/systypes.h"
#include "services/syserror.h"
#include "services/systp.h"
#include "SensorDef.h"
#include "ISensor.h"

/**
  * Create  type name for ISensorRanging.
  */
typedef struct _ISensorRanging_t ISensorRanging_t;

// Public API declaration
//***********************
/** Public interface **/
static inline sys_error_code_t ISensorGetProfile(ISensorRanging_t *_this, ProfileConfig_t *p_config);
static inline sys_error_code_t ISensorGetIT(ISensorRanging_t *_this, ITConfig_t *p_it_config);
static inline uint32_t ISensorGetAddress(ISensorRanging_t *_this);
static inline uint32_t ISensorGetPowerMode(ISensorRanging_t *_this);
static inline sys_error_code_t ISensorSetRangingFrequency(ISensorRanging_t *_this, uint32_t frequency);
static inline sys_error_code_t ISensorSetRangingResolution(ISensorRanging_t *_this, uint8_t resolution);
static inline sys_error_code_t ISensorSetRangingMode(ISensorRanging_t *_this, uint8_t mode);
static inline sys_error_code_t ISensorSetIntegrationTime(ISensorRanging_t *_this, uint32_t timing_budget);
static inline sys_error_code_t ISensorConfigIT(ISensorRanging_t *_this, ITConfig_t *p_it_config);
static inline sys_error_code_t ISensorSetAddress(ISensorRanging_t *_this, uint32_t address);
static inline sys_error_code_t ISensorSetPowerMode(ISensorRanging_t *_this, uint32_t power_mode);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ISENSORRANGING_H_ */
