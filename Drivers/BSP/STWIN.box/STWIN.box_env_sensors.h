/**
 ******************************************************************************
 * @file    STWIN.box_env_sensors.h
 * @author  SRA
 * @brief   This file contains definitions for STWIN.box_env_sensors.c
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STWIN_BOX_ENV_SENSORS_H__
#define STWIN_BOX_ENV_SENSORS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STWIN.box.h"
#include "env_sensor.h"

#ifndef USE_ENV_SENSOR_ILPS22QS_0
#define USE_ENV_SENSOR_ILPS22QS_0	1
#endif

#ifndef USE_ENV_SENSOR_STTS22H_0
#define USE_ENV_SENSOR_STTS22H_0	1
#endif

#if (USE_ENV_SENSOR_ILPS22QS_0 == 1)
#include "ilps22qs.h"
#endif

#if (USE_ENV_SENSOR_STTS22H_0 == 1)
#include "stts22h.h"
#endif

#if (USE_ENV_SENSOR_ILPS22QS_0 == 1)
#define ILPS22QS_0 (0)
#endif

#if (USE_ENV_SENSOR_STTS22H_0 == 1)
#define STTS22H_0 (USE_ENV_SENSOR_ILPS22QS_0)
#endif

/* Environmental Sensor instance Info */
typedef struct
{
  uint8_t Temperature;
  uint8_t Pressure;
  uint8_t Humidity;
  uint8_t LowPower;
  float_t HumMaxOdr;
  float_t TempMaxOdr;
  float_t PressMaxOdr;
} ENV_SENSOR_Capabilities_t;

typedef struct
{
  uint32_t Functions;
} ENV_SENSOR_Ctx_t;

#define ENV_TEMPERATURE      1U
#define ENV_PRESSURE         2U

#define ENV_FUNCTIONS_NBR    2U
#define ENV_INSTANCES_NBR    (USE_ENV_SENSOR_ILPS22QS_0 + USE_ENV_SENSOR_STTS22H_0)

#if (ENV_INSTANCES_NBR == 0)
#error "No environmental sensor instance has been selected"
#endif

int32_t BSP_ENV_SENSOR_Init(uint32_t Instance, uint32_t Functions);
int32_t BSP_ENV_SENSOR_DeInit(uint32_t Instance);
int32_t BSP_ENV_SENSOR_GetCapabilities(uint32_t Instance, ENV_SENSOR_Capabilities_t *Capabilities);
int32_t BSP_ENV_SENSOR_ReadID(uint32_t Instance, uint8_t *Id);
int32_t BSP_ENV_SENSOR_Enable(uint32_t Instance, uint32_t Function);
int32_t BSP_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function);
int32_t BSP_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float_t *Odr);
int32_t BSP_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float_t Odr);
int32_t BSP_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float_t *Value);

#ifdef __cplusplus
}
#endif

#endif /* STWIN_BOX_ENV_SENSORS_H__ */
