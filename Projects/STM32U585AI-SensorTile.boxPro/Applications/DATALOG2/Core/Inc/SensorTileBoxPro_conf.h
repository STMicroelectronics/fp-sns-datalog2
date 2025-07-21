/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : SensorTileBoxPro_conf_template.h
  * @brief          : Template Configuration file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "SensorTileBoxPro_bus.h"
#include "SensorTileBoxPro_errno.h"

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

/**  Definition for SD DETECT INTERRUPT PIN  **/
extern EXTI_HandleTypeDef hexti5;
#define H_EXTI_5                                hexti5
#define H_EXTI_SD_DETECT                        hexti5
#define BSP_SD_DETECT_PIN                       GPIO_PIN_5
#define BSP_SD_DETECT_GPIO_PORT                 GPIOC
#define BSP_SD_DETECT_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define BSP_SD_DETECT_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOC_CLK_DISABLE()
#define BSP_SD_DETECT_EXTI_LINE                 EXTI_LINE_5
#define BSP_SD_DETECT_EXTI_IRQN                 EXTI5_IRQn
#define BSP_SD_DETECT_IRQHANDLER                EXTI5_IRQHandler


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

