/**
 ******************************************************************************
 * @file    sysdebug_config.h
 * @author  SRA
 * @brief   Configure the debug log functionality
 *
 * Each logic module of the application should define a DEBUG control byte
 * used to turn on/off the log for the module.
 *
 *********************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *********************************************************************************
 */

#ifndef SYSDEBUG_CONFIG_H_
#define SYSDEBUG_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SYS_DBG_LEVEL                      SYS_DBG_LEVEL_VERBOSE /*!< set the level of the system log: all log messages with minor level are discharged. */

/* Example */
/*#define SYS_DBG_MODULE1     SYS_DBG_ON|GTS_DBG_HALT  ///< Module 1 debug control byte */
/*#define SYS_DBG_MODULE2     SYS_DBG_ON               ///< Module 2 debug control byte */


#define SYS_DBG_INIT                       SYS_DBG_ON                 ///< Init task debug control byte
#define SYS_DBG_DRIVERS                    SYS_DBG_OFF                ///< Drivers debug control byte
#define SYS_DBG_SYSTS                      SYS_DBG_OFF                ///< Drivers debug control byte
#define SYS_DBG_APP                        SYS_DBG_ON                ///< Generic Application debug control byte
#define SYS_DBG_APMH                       SYS_DBG_ON                 ///< Application Power Mode Helper debug control byte
#define SYS_DBG_HW                         SYS_DBG_ON                 ///< Hello World task debug control byte
#define SYS_DBG_SPIBUS                     SYS_DBG_ON                 ///< SPIBus task debug control byte
#define SYS_DBG_I2CBUS                     SYS_DBG_ON                 ///< I2CBus task debug control byte
#define SYS_DBG_ISM330DHCX                 SYS_DBG_ON                 ///< ISM330DHCX sensor task debug control byte
#define SYS_DBG_ISM330IS                   SYS_DBG_ON                 ///< ISM330IS sensor task debug control byte
#define SYS_DBG_IIS3DWB                    SYS_DBG_ON                ///< IIS3DWB sensor task debug control byte
#define SYS_DBG_HTS221                     SYS_DBG_ON                 ///< HTS221 sensor task debug control byte
#define SYS_DBG_ILPS22QS                   SYS_DBG_ON                 ///< ILPS22QS sensor task debug control byte
#define SYS_DBG_LPS22DF                    SYS_DBG_ON                 ///< HTS221 sensor task debug control byte
#define SYS_DBG_LPS22HH                    SYS_DBG_ON                 ///< LPS22DF sensor task debug control byte
#define SYS_DBG_ENV                        SYS_DBG_OFF                 ///< ENV sensor task debug control byte
#define SYS_DBG_SMUTIL                     SYS_DBG_ON                 ///< Utility task debug control byte
#define SYS_DBG_SDC                        SYS_DBG_OFF                 ///< SDCARD task debug control byte
#define SYS_DBG_IMP23ABSU                  SYS_DBG_OFF                 ///< IMP23ABSU task debug control byte
#define SYS_DBG_IMP34DT05                  SYS_DBG_OFF                 ///< IMP34DT05 task debug control byte
#define SYS_DBG_IIS2MDC                    SYS_DBG_ON                 ///< IIS2MDC task debug control byte
#define SYS_DBG_IIS2ICLX                   SYS_DBG_ON                 ///< IIS2ICLX task debug control byte
#define SYS_DBG_STTS22H                    SYS_DBG_ON                 ///< STTS22H task debug control byte
#define SYS_DBG_IIS2DLPC                   SYS_DBG_ON                 ///< IIS2DLPC task debug control byte
#define SYS_DBG_MP23DB01HP                 SYS_DBG_ON                 ///< MP23DB01HP task debug control byte
#define SYS_DBG_SL                         SYS_DBG_ON                 ///< Sensor Listener debug control byte
#define SYS_DBG_UTIL                       SYS_DBG_ON                 ///< UTIL task debug control byte
#define SYS_DBG_DT                         SYS_DBG_ON                 ///< Datalog Task with 1 DPU debug control byte
#define SYS_DBG_DPU                        SYS_DBG_ON                 ///< Digital Processing Unit debug control byte
//#define SYS_DBG_DPT1                       SYS_DBG_ON                 ///< Digital Process Task with 1 DPU debug control byte
//#define SYS_DBG_USBCDC                     SYS_DBG_ON                 ///< USB CDC Task with 1 DPU debug control byte


/* ODeV - hardware configuration for the debug services provided by the framework */
/**********************************************************************************/

#include "mx.h"

/* ODeV test point PINs */
//#define SYS_DBG_TP1_PORT                   SYS_DBG_TP1_GPIO_Port
//#define SYS_DBG_TP1_PIN                    SYS_DBG_TP1_Pin
//#define SYS_DBG_TP1_CLK_ENABLE             __HAL_RCC_GPIOG_CLK_ENABLE
//#define SYS_DBG_TP2_PORT                   SYS_DBG_TP2_GPIO_Port
//#define SYS_DBG_TP2_PIN                    SYS_DBG_TP2_Pin
//#define SYS_DBG_TP2_CLK_ENABLE             __HAL_RCC_GPIOG_CLK_ENABLE

/* ODeV DBG UART used for the system log */
extern UART_HandleTypeDef huart1;
void MX_USART1_UART_Init(void);

#define SYS_DBG_UART                       huart1
#define SYS_DBG_UART_INIT                  MX_USART1_UART_Init
#define SYS_DBG_UART_TIMEOUT_MS            5000

/* ODeV runtime statistic timer configuration for FreeRTOS */
extern TIM_HandleTypeDef htim6;
void MX_TIM6_Init(void);

#define SYS_DBG_TIM                        htim6
#define SYS_DBG_TIM_INIT                   MX_TIM6_Init
#define SYS_DBG_TIM_IRQ_N                  TIM6_DAC_IRQn
#define SYS_DBG_TIM_IRQ_HANDLER            TIM6_DACUNDER_IRQHandler
#endif /* SYS_DEBUG */


#ifdef __cplusplus
}
#endif

