/**
  ******************************************************************************
  * @file    SensorTileBoxPro_conf_template.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V2.0.0
  * @date    20-Oct-2024
  * @brief  Configuration file
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

 /* FINISHA --> BOARD_ID = 0x0DU
    FINISHB --> BOARD_ID = 0x11U */
 typedef enum
 {
   FINISHA = 0,
   FINISHB = 1,
   FINISH_ERROR =2
 }FinishGood_TypeDef;

/* COM Feature define */
#define USE_BSP_COM_FEATURE                1U

/* COM define */
#define USE_COM_LOG                        1U

/* IRQ priorities */
#define BSP_BUTTON_USER_IT_PRIORITY        15U

/* UART4 Baud rate in bps  */
#define BUS_UART4_BAUDRATE                 115200U /* baud rate of UARTn = 115200 baud */

#define USE_MOTION_SENSOR_LIS2MDL_0        1U

#define USE_ENV_SENSOR_STTS22H_0           1U

#define USE_ENV_SENSOR_LPS22DF_0           1U

#define USE_MOTION_SENSOR_LIS2DU12_0       1U

#define USE_MOTION_SENSOR_LSM6DSV16X_0     1U

#define BSP_NFCTAG_INSTANCE         0U

#if (USE_MOTION_SENSOR_LIS2MDL_0 + USE_MOTION_SENSOR_LSM6DSV16X_0 + USE_MOTION_SENSOR_LIS2DU12_0 == 0 )
#undef USE_MOTION_SENSOR_LSM6DSV16X_0
#define USE_MOTION_SENSOR_LSM6DSV16X_0     1U
#endif

#if (USE_ENV_SENSOR_STTS22H_0 + USE_ENV_SENSOR_LPS22DF_0 == 0 )
#undef USE_ENV_SENSOR_STTS22H_0
#define USE_ENV_SENSOR_STTS22H_0     1U
#endif

/* For using LSM6DSV16X and LIS2DU12 with I2C */
#define ALL_SENSORS_I2C

 /* LIS2MDL magneto Sensor */
#define BSP_LIS2MDL_0_I2C_INIT                  BSP_I2C1_Init
#define BSP_LIS2MDL_0_I2C_DEINIT                BSP_I2C1_DeInit
#define BSP_LIS2MDL_0_I2C_READ_REG              BSP_I2C1_ReadReg
#define BSP_LIS2MDL_0_I2C_WRITE_REG             BSP_I2C1_WriteReg

/* LPS22DF Pressure Sensor */
#define BSP_LPS22DF_0_I2C_INIT                  BSP_I2C1_Init
#define BSP_LPS22DF_0_I2C_DEINIT                BSP_I2C1_DeInit
#define BSP_LPS22DF_0_I2C_READ_REG              BSP_I2C1_ReadReg
#define BSP_LPS22DF_0_I2C_WRITE_REG             BSP_I2C1_WriteReg

/* STTS22H Temperature Sensor */
#define BSP_STTS22H_0_I2C_INIT                  BSP_I2C1_Init
#define BSP_STTS22H_0_I2C_DEINIT                BSP_I2C1_DeInit
#define BSP_STTS22H_0_I2C_READ_REG              BSP_I2C1_ReadReg
#define BSP_STTS22H_0_I2C_WRITE_REG             BSP_I2C1_WriteReg

/* LIS2DU12 acc Sensor */
#ifdef ALL_SENSORS_I2C
#define BSP_LIS2DU12_0_I2C_INIT                 BSP_I2C1_Init
#define BSP_LIS2DU12_0_I2C_DEINIT               BSP_I2C1_DeInit
#define BSP_LIS2DU12_0_I2C_READ_REG             BSP_I2C1_ReadReg
#define BSP_LIS2DU12_0_I2C_WRITE_REG            BSP_I2C1_WriteReg
#else
#define BSP_LIS2DU12_0_SPI_INIT                 BSP_SPI2_Init
#define BSP_LIS2DU12_0_SPI_DEINIT               BSP_SPI2_DeInit
#define BSP_LIS2DU12_0_SPI_SEND                 BSP_SPI2_Send
#define BSP_LIS2DU12_0_SPI_RECV                 BSP_SPI2_Recv
#define BSP_LIS2DU12_CS_PORT                    GPIOI
#define BSP_LIS2DU12_CS_PIN                     GPIO_PIN_7
#endif

/* LSM6DSV16X acc - gyro - qvar Sensor */
#ifdef ALL_SENSORS_I2C
#define BSP_LSM6DSV16X_0_I2C_INIT               BSP_I2C1_Init
#define BSP_LSM6DSV16X_0_I2C_DEINIT             BSP_I2C1_DeInit
#define BSP_LSM6DSV16X_0_I2C_READ_REG           BSP_I2C1_ReadReg
#define BSP_LSM6DSV16X_0_I2C_WRITE_REG          BSP_I2C1_WriteReg
#else
#define BSP_LSM6DSV16X_0_SPI_INIT		        BSP_SPI2_Init
#define BSP_LSM6DSV16X_0_SPI_DEINIT		        BSP_SPI2_DeInit
#define BSP_LSM6DSV16X_0_SPI_SEND		        BSP_SPI2_Send
#define BSP_LSM6DSV16X_0_SPI_RECV		        BSP_SPI2_Recv
#define BSP_LSM6DSV16X_CS_PORT			        GPIOI
#define BSP_LSM6DSV16X_CS_PIN			        GPIO_PIN_5
#endif

/* ST25DV nfc Device */
#define BSP_ST25DV_I2C_INIT                     BSP_I2C2_Init
#define BSP_ST25DV_I2C_DEINIT                   BSP_I2C2_DeInit
#define BSP_ST25DV_I2C_READ_REG_16              BSP_I2C2_ReadReg16
#define BSP_ST25DV_I2C_WRITE_REG_16             BSP_I2C2_WriteReg16
#define BSP_ST25DV_I2C_RECV                     BSP_I2C2_Recv
#define BSP_ST25DV_I2C_IS_READY                 BSP_I2C2_IsReady

/* ST25DVXXKC nfc Device */
#define BSP_ST25DVXXKC_I2C_INIT                 BSP_I2C2_Init
#define BSP_ST25DVXXKC_I2C_DEINIT               BSP_I2C2_DeInit
#define BSP_ST25DVXXKC_I2C_READ_REG_16          BSP_I2C2_ReadReg16
#define BSP_ST25DVXXKC_I2C_WRITE_REG_16         BSP_I2C2_WriteReg16
#define BSP_ST25DVXXKC_I2C_RECV                 BSP_I2C2_Recv
#define BSP_ST25DVXXKC_I2C_IS_READY             BSP_I2C2_IsReady

/* nfctag GPO pin */
extern EXTI_HandleTypeDef                       hexti12;
#define H_EXTI_12                               hexti12
#define GPO_EXTI                                hexti12
#define BSP_GPO_PIN                             GPIO_PIN_12
#define BSP_GPO_GPIO_PORT                       GPIOE
#define BSP_GPO_EXTI_LINE                       EXTI_LINE_12
#define BSP_GPO_EXTI_IRQN                       EXTI12_IRQn
#define BSP_GPO_CLK_ENABLE()                    __HAL_RCC_GPIOE_CLK_ENABLE()
#define BSP_GPO_EXTI_IRQHANDLER                 EXTI12_IRQHandler

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

/* STC3115 battery Sensor */
#define BSP_STC3115_0_I2C_INIT    		        BSP_I2C4_Init
#define BSP_STC3115_0_I2C_DEINIT		        BSP_I2C4_DeInit
#define BSP_STC3115_0_I2C_READ_REG			    BSP_I2C4_ReadReg
#define BSP_STC3115_0_I2C_WRITE_REG			    BSP_I2C4_WriteReg

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

