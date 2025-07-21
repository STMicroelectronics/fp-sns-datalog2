/**
  ******************************************************************************
  * @file    sysdebug_config.h
  * @author  SRA
  * @brief   Configure the debug log functionality
  *
  * Each logic module of the application should define a DEBUG control byte
  * used to turn on/off the log for the module.
  *
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

#ifndef SYSDEBUG_CONFIG_H_
#define SYSDEBUG_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SYS_DBG_LEVEL                      SYS_DBG_LEVEL_VERBOSE /*!< set the level of the system log: all log messages with minor level are discharged. */

/* Example */
/*#define SYS_DBG_MODULE1     SYS_DBG_ON|GTS_DBG_HALT  ///< Module 1 debug control byte */
/*#define SYS_DBG_MODULE2     SYS_DBG_ON               ///< Module 2 debug control byte */

#define SYS_DBG_INIT                       SYS_DBG_ON                 /* Init task debug control byte */
#define SYS_DBG_DRIVERS                    SYS_DBG_ON                 /* Drivers debug control byte */
#define SYS_DBG_APP                        SYS_DBG_ON                 /* Generic Application debug control byte */
#define SYS_DBG_APMH                       SYS_DBG_ON                 /* Application Power Mode Helper debug control byte */
#define SYS_DBG_SPIBUS                     SYS_DBG_OFF                /* SPIBus task debug control byte */
#define SYS_DBG_I2CBUS                     SYS_DBG_OFF                /* I2CBus task debug control byte */
#define SYS_DBG_ISM330DHCX                 SYS_DBG_OFF                /* ISM330DHCX sensor task debug control byte */
#define SYS_DBG_IIS3DWB                    SYS_DBG_OFF                /* IIS3DWB sensor task debug control byte */
#define SYS_DBG_HTS221                     SYS_DBG_OFF                /* HTS221 sensor task debug control byte */
#define SYS_DBG_IMP23ABSU                  SYS_DBG_OFF                /* IMP23ABSU task debug control byte */
#define SYS_DBG_IMP34DT05                  SYS_DBG_OFF                /* IMP34DT05 task debug control byte */
#define SYS_DBG_IIS2MDC                    SYS_DBG_OFF                /* IIS2MDC task debug control byte */
#define SYS_DBG_IIS2DLPC                   SYS_DBG_OFF                /* IIS2DLPC task debug control byte */
#define SYS_DBG_ILPS22QS                   SYS_DBG_OFF                /* ILPS22QS task debug control byte */
#define SYS_DBG_STTS22H                    SYS_DBG_OFF                /* STTS22H task debug control byte */
#define SYS_DBG_IIS2ICLX                   SYS_DBG_OFF                /* IIS2ICLX sensor task debug control byte */
#define SYS_DBG_SMUTIL                     SYS_DBG_ON                 /* Utility task debug control byte */
#define SYS_DBG_SDC                        SYS_DBG_ON                 /* SDCARD task debug control byte */
#define SYS_DBG_UTIL                       SYS_DBG_ON                 /* UTIL task debug control byte */
#define SYS_DBG_DT                         SYS_DBG_ON                 /* Datalog task debug control byte */
#define SYS_DBG_BLE                        SYS_DBG_ON                 /* BLE task debug control byte */
#define SYS_DBG_ACO                        SYS_DBG_ON                 /* Datalog task debug control byte */
#define SYS_DBG_SYSTS                      SYS_DBG_OFF                /* TimeStamp service debug control byte */
#define SYS_DBG_DPU                        SYS_DBG_OFF                /* DPU debug control byte */

/* ODeV - hardware configuration for the debug services provided by the framework */
/**********************************************************************************/

#include "mx.h"

/* ODeV DBG UART used for the system log */
extern UART_HandleTypeDef huart2;
void MX_USART2_UART_Init(void);

#define SYS_DBG_UART                       huart2
#define SYS_DBG_UART_INIT                  MX_USART2_UART_Init
#define SYS_DBG_UART_TIMEOUT_MS            5000
#endif /* SYS_DEBUG */

#ifdef __cplusplus
}
#endif

