/**
  ******************************************************************************
  * @file    SensorTileBoxPro_motion_sensors.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.2.0
  * @date    03-Jun-2024
  * @brief   This file contains definitions for the BSP Motion Sensors
  *          interface for SensorTileBoxPro
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
#ifndef __SENSORTILEBOXPRO_MOTION_SENSORS_H__
#define __SENSORTILEBOXPRO_MOTION_SENSORS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "SensorTileBoxPro_conf.h"
#include "motion_sensor.h"

#ifndef USE_MOTION_SENSOR_LIS2MDL_0
#define USE_MOTION_SENSOR_LIS2MDL_0          1
#endif /* USE_MOTION_SENSOR_LIS2MDL_0 */

#ifndef USE_MOTION_SENSOR_LIS2DU12_0
#define USE_MOTION_SENSOR_LIS2DU12_0          0
#endif /* USE_MOTION_SENSOR_LIS2DU12_0 */

#ifndef USE_MOTION_SENSOR_LSM6DSV16X_0
#define USE_MOTION_SENSOR_LSM6DSV16X_0        0
#endif /* USE_MOTION_SENSOR_LSM6DSV16X_0 */

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
#include "lis2mdl.h"
#endif /* USE_MOTION_SENSOR_LIS2MDL_0 == 1 */

#if (USE_MOTION_SENSOR_LIS2DU12_0 == 1)
#include "lis2du12.h"
#endif /* USE_MOTION_SENSOR_LIS2DU12_0 == 1 */

#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
#include "lsm6dsv16x.h"
#endif /* USE_MOTION_SENSOR_LSM6DSV16X_0 == 1 */

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
#define LIS2MDL_0 (0)
#endif /* USE_MOTION_SENSOR_LSM6DSV16X_0 == 1 */

#if (USE_MOTION_SENSOR_LIS2DU12_0 == 1)
#define LIS2DU12_0 (USE_MOTION_SENSOR_LIS2MDL_0)
#endif /* USE_MOTION_SENSOR_LIS2DU12_0 == 1 */

#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
#define LSM6DSV16X_0 (USE_MOTION_SENSOR_LIS2MDL_0 + USE_MOTION_SENSOR_LIS2DU12_0)
#endif /* USE_MOTION_SENSOR_LSM6DSV16X_0 == 1 */

typedef struct
{
  int32_t x;
  int32_t y;
  int32_t z;
} BSP_MOTION_SENSOR_Axes_t;

typedef struct
{
  int16_t x;
  int16_t y;
  int16_t z;
} BSP_MOTION_SENSOR_AxesRaw_t;

/* Motion Sensor instance Info */
typedef struct
{
  uint8_t  Acc;
  uint8_t  Gyro;
  uint8_t  Magneto;
  uint8_t  LowPower;
  uint32_t GyroMaxFS;
  uint32_t AccMaxFS;
  uint32_t MagMaxFS;
  float    GyroMaxOdr;
  float    AccMaxOdr;
  float    MagMaxOdr;
} BSP_MOTION_SENSOR_Capabilities_t;

typedef struct
{
  uint32_t Functions;
} BSP_MOTION_SENSOR_Ctx_t;

#define MOTION_GYRO             1U
#define MOTION_ACCELERO         2U
#define MOTION_MAGNETO          4U

#define BSP_MOTION_FUNCTIONS_NBR    3U
#define BSP_MOTION_INSTANCES_NBR    (USE_MOTION_SENSOR_LIS2MDL_0\
                                     + USE_MOTION_SENSOR_LIS2DU12_0 + USE_MOTION_SENSOR_LSM6DSV16X_0)

#if (BSP_MOTION_INSTANCES_NBR == 0)
#error "No motion sensor instance has been selected"
#endif /* BSP_MOTION_INSTANCES_NBR == 0 */

int32_t BSP_MOTION_SENSOR_Init(uint32_t Instance, uint32_t Functions);
int32_t BSP_MOTION_SENSOR_DeInit(uint32_t Instance);
int32_t BSP_MOTION_SENSOR_GetCapabilities(uint32_t Instance, BSP_MOTION_SENSOR_Capabilities_t *Capabilities);
int32_t BSP_MOTION_SENSOR_ReadID(uint32_t Instance, uint8_t *Id);
int32_t BSP_MOTION_SENSOR_Enable(uint32_t Instance, uint32_t Function);
int32_t BSP_MOTION_SENSOR_Disable(uint32_t Instance, uint32_t Function);
int32_t BSP_MOTION_SENSOR_GetAxes(uint32_t Instance, uint32_t Function, BSP_MOTION_SENSOR_Axes_t *Axes);
int32_t BSP_MOTION_SENSOR_GetAxesRaw(uint32_t Instance, uint32_t Function, BSP_MOTION_SENSOR_AxesRaw_t *Axes);
int32_t BSP_MOTION_SENSOR_GetSensitivity(uint32_t Instance, uint32_t Function, float *Sensitivity);
int32_t BSP_MOTION_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float *Odr);
int32_t BSP_MOTION_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float Odr);
int32_t BSP_MOTION_SENSOR_GetFullScale(uint32_t Instance, uint32_t Function, int32_t *Fullscale);
int32_t BSP_MOTION_SENSOR_SetFullScale(uint32_t Instance, uint32_t Function, int32_t Fullscale);

#ifdef __cplusplus
}
#endif

#endif /* __SENSORTILEBOXPRO_MOTION_SENSORS_H__ */

