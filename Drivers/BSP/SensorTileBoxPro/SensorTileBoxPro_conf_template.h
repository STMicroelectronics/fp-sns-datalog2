/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SensorTileBoxPro_conf_template.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.1.0
  * @date    20-July-2023
  * @brief   Template Configuration file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
*/
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SENSORTILEBOXPRO_CONF_H
#define SENSORTILEBOXPRO_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup SENSORTILEBOXPRO
  * @{
  */

/** @defgroup SENSORTILEBOXPRO_CONFIG Config
  * @{
  */

/** @defgroup SENSORTILEBOXPRO_CONFIG_Exported_Constants
  * @{
  */
/* COM Feature define */
#define USE_BSP_COM_FEATURE                 1U

/* COM define */
#define USE_COM_LOG                         1U

/* IRQ priorities */
#define BSP_BUTTON_USER_IT_PRIORITY         15U

/* UART4 Baud rate in bps  */
#define BUS_UART4_BAUDRATE                  115200U /* baud rate of UARTn = 115200 baud */

#define USE_MOTION_SENSOR_LIS2MDL_0        1U

#define USE_ENV_SENSOR_STTS22H_0           1U

#define USE_ENV_SENSOR_LPS22DF_0           1U

#define USE_MOTION_SENSOR_LIS2DU12_0       1U

#define USE_MOTION_SENSOR_LSM6DSV16X_0     1U
   
#define BSP_NFCTAG_INSTANCE         0U
 
//For using LSM6DSV16X and LIS2DU12 with I2C
#define ALL_SENSORS_I2C

#include "SensorTileBoxPro_bus.h"   

  
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif
#endif  /* SENSORTILEBOXPRO_CONF_H */

