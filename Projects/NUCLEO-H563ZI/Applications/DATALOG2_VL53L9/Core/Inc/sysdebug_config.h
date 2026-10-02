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

#define SYS_DBG_LEVEL                      SYS_DBG_LEVEL_LLA /*!< set the level of the system log: all log messages with minor level are discharged. */

/* Example */
/*#define SYS_DBG_MODULE1     SYS_DBG_ON|GTS_DBG_HALT  ///< Module 1 debug control byte */
/*#define SYS_DBG_MODULE2     SYS_DBG_ON               ///< Module 2 debug control byte */


#define SYS_DBG_INIT                       SYS_DBG_OFF                 /* Init task debug control byte*/
#define SYS_DBG_DRIVERS                    SYS_DBG_ON                /* Drivers debug control byte*/
#define SYS_DBG_APP                        SYS_DBG_OFF                 /* Generic Application debug control byte*/
#define SYS_DBG_APMH                       SYS_DBG_OFF                 /* Application Power Mode Helper debug control byte*/
#define SYS_DBG_SPIBUS                     SYS_DBG_OFF                 /* SPIBus task debug control byte*/
#define SYS_DBG_I3CBUS                     SYS_DBG_ON                 /* I3CBus task debug control byte*/
#define SYS_DBG_IIS2DULPX                  SYS_DBG_OFF                 /* IIS2DULPX sensor task debug control byte*/
#define SYS_DBG_IIS2MDC                    SYS_DBG_OFF                 /* IIS2MDC sensor task debug control byte*/
#define SYS_DBG_ILPS22QS                   SYS_DBG_OFF                 /* ILPS22QS sensor task debug control byte*/
#define SYS_DBG_ISM330IS                   SYS_DBG_OFF                 /* ISM330IS sensor task debug control byte*/
#define SYS_DBG_ISM6HG256X                 SYS_DBG_OFF                 /* ISM6HG256X sensor task debug control byte*/
#define SYS_DBG_IIS3DWB10IS                SYS_DBG_OFF                 /* IIS3DWB10IS sensor task debug control byte*/
#define SYS_DBG_UTIL                       SYS_DBG_OFF                 /* Util task debug control byte*/
#define SYS_DBG_DT                         SYS_DBG_OFF                 /* Datalog Task with 1 DPU debug control byte*/
#define SYS_DBG_SYSTS                      SYS_DBG_OFF                 /* SysTimestamp debug control byte*/
#define SYS_DBG_VL53L9                     SYS_DBG_ON                 /* VL53L9 sensor task debug control byte*/

/* ODeV - hardware configuration for the debug services provided by the framework */
/**********************************************************************************/

#include "mx.h"

/* eLooM DBG UART used for the system log */
extern UART_HandleTypeDef huart3;
void MX_USART3_UART_Init(void);

#define SYS_DBG_UART                       huart3
#define SYS_DBG_UART_INIT                  MX_USART3_UART_Init
#define SYS_DBG_UART_TIMEOUT_MS            5000


#ifdef __cplusplus
}
#endif


#endif /* SYS_DEBUG */
