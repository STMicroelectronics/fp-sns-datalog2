/**
  ******************************************************************************
  * @file    SensorTileBoxPro_env_sensors.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.2.0
  * @date    03-Jun-2024
  * @brief   This file contains definitions for the BSP Environmental
  *          Sensors interface for SensorTileBoxPro
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SENSORTILEBOXPRO_ENV_SENSORS_H__
#define __SENSORTILEBOXPRO_ENV_SENSORS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "SensorTileBoxPro_conf.h"
#include "env_sensor.h"

#ifndef USE_ENV_SENSOR_STTS22H_0
#define USE_ENV_SENSOR_STTS22H_0         1
#endif /* USE_ENV_SENSOR_STTS22H_0 */

#ifndef USE_ENV_SENSOR_LPS22DF_0
#define USE_ENV_SENSOR_LPS22DF_0         0
#endif /* USE_ENV_SENSOR_LPS22DF_0 */

#if (USE_ENV_SENSOR_STTS22H_0 == 1)
#include "stts22h.h"
#endif /* USE_ENV_SENSOR_STTS22H_0 == 1 */

#if (USE_ENV_SENSOR_LPS22DF_0 == 1)
#include "lps22df.h"
#endif /* USE_ENV_SENSOR_LPS22DF_0 == 1 */

#if (USE_ENV_SENSOR_STTS22H_0 == 1)
#define STTS22H_0 (0)
#endif /* USE_ENV_SENSOR_STTS22H_0 == 1 */

#if (USE_ENV_SENSOR_LPS22DF_0 == 1)
#define LPS22DF_0 (USE_ENV_SENSOR_STTS22H_0)
#endif /* USE_ENV_SENSOR_LPS22DF_0 == 1 */

/* Environmental Sensor instance Info */
typedef struct
{
  uint8_t Temperature;
  uint8_t Pressure;
  uint8_t Humidity;
  uint8_t LowPower;
  float   HumMaxOdr;
  float   TempMaxOdr;
  float   PressMaxOdr;
} BSP_ENV_SENSOR_Capabilities_t;

typedef struct
{
  uint32_t Functions;
} BSP_ENV_SENSOR_Ctx_t;

#ifndef ENV_TEMPERATURE
#define ENV_TEMPERATURE      1U
#endif /* ENV_TEMPERATURE */
#ifndef ENV_PRESSURE
#define ENV_PRESSURE         2U
#endif /* ENV_TEMPERATURE */
#ifndef ENV_HUMIDITY
#define ENV_HUMIDITY        4U
#endif /* ENV_TEMPERATURE */

#define BSP_ENV_FUNCTIONS_NBR    2U
#define BSP_ENV_INSTANCES_NBR    (USE_ENV_SENSOR_STTS22H_0 + USE_ENV_SENSOR_LPS22DF_0)

#if (BSP_ENV_INSTANCES_NBR == 0)
#error "No environmental sensor instance has been selected"
#endif /* BSP_ENV_INSTANCES_NBR == 0 */

int32_t BSP_ENV_SENSOR_Init(uint32_t Instance, uint32_t Functions);
int32_t BSP_ENV_SENSOR_DeInit(uint32_t Instance);
int32_t BSP_ENV_SENSOR_GetCapabilities(uint32_t Instance, BSP_ENV_SENSOR_Capabilities_t *Capabilities);
int32_t BSP_ENV_SENSOR_ReadID(uint32_t Instance, uint8_t *Id);
int32_t BSP_ENV_SENSOR_Enable(uint32_t Instance, uint32_t Function);
int32_t BSP_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function);
int32_t BSP_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float *Odr);
int32_t BSP_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float Odr);
int32_t BSP_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float *Value);

#ifdef __cplusplus
}
#endif

#endif /* __SENSORTILEBOXPRO_ENV_SENSORS_H__ */

