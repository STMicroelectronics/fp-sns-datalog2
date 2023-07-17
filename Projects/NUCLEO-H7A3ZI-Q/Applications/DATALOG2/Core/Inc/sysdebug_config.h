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
  * Copyright (c) 2022 STMicroelectronics.
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


#define SYS_DBG_INIT                       SYS_DBG_ON                 ///< Init task debug control byte
#define SYS_DBG_DRIVERS                    SYS_DBG_OFF                ///< Drivers debug control byte
#define SYS_DBG_APP                        SYS_DBG_ON                 ///< Generic Application debug control byte
#define SYS_DBG_APMH                       SYS_DBG_ON                 ///< Application Power Mode Helper debug control byte
#define SYS_DBG_SYSTS                      SYS_DBG_ON                 ///< system Timestamp service debug control byte
#define SYS_DBG_HW                         SYS_DBG_ON                 ///< Hello World task debug control byte
#define SYS_DBG_SPIBUS                     SYS_DBG_ON                 ///< SPIBus task debug control byte
#define SYS_DBG_I2CBUS                     SYS_DBG_ON                 ///< I2CBus task debug control byte
#define SYS_DBG_ISM330DHCX                 SYS_DBG_ON                 ///< ISM330DHCX sensor task debug control byte
#define SYS_DBG_IIS2DLPC                   SYS_DBG_ON                 ///< IIS2DLP sensor task debug control byte
#define SYS_DBG_IIS2MDC                    SYS_DBG_ON                 ///< IIS2MDC sensor task debug control byte
#define SYS_DBG_DPU                        SYS_DBG_ON                 ///< Digital Processing Unit debug control byte
#define SYS_DBG_DPT1                       SYS_DBG_ON                 ///< Digital Process Task with 1 DPU debug control byte
#define SYS_DBG_DT                         SYS_DBG_ON                 ///< Datalog Task with 1 DPU debug control byte


/* eLooM - hardware configuration for the debug services provided by the framework */
/***********************************************************************************/

#include "mx.h"


/* eLooM DBG UART used for the system log */
extern UART_HandleTypeDef huart3;
void MX_USART3_UART_Init(void);

#define SYS_DBG_UART                       huart3
#define SYS_DBG_UART_INIT                  MX_USART3_UART_Init
#define SYS_DBG_UART_TIMEOUT_MS            5000

#endif /* SYS_DEBUG */


#ifdef __cplusplus
}
#endif

