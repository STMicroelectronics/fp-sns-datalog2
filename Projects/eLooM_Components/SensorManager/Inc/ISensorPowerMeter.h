/**
  ******************************************************************************
  * @file    ISensorPowerMeter.h
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

#ifndef INCLUDE_ISENSORPOWERMETER_H_
#define INCLUDE_ISENSORPOWERMETER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/systypes.h"
#include "services/syserror.h"
#include "services/systp.h"
#include "SensorDef.h"
#include "ISensor.h"

/**
  * Create  type name for ISensorPowerMeter.
  */
typedef struct _ISensorPowerMeter_t ISensorPowerMeter_t;

// Public API declaration
//***********************
/** Public interface **/
static inline uint32_t ISensorGetADCConversionTime(ISensorPowerMeter_t *_this);
static inline uint32_t ISensorGetRShunt(ISensorPowerMeter_t *_this);
static inline sys_error_code_t ISensorSetADCConversionTime(ISensorPowerMeter_t *_this, uint32_t adc_conversion_time);
static inline sys_error_code_t ISensorSetRShunt(ISensorPowerMeter_t *_this, uint32_t r_shunt);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ISENSORPOWERMETER_H_ */
