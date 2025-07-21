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


#define SYS_DBG_INIT                       SYS_DBG_ON                 ///< Init task debug control byte
#define SYS_DBG_DRIVERS                    SYS_DBG_OFF                ///< Drivers debug control byte
#define SYS_DBG_APP                        SYS_DBG_ON                 ///< Generic Application debug control byte
#define SYS_DBG_APMH                       SYS_DBG_ON                 ///< Application Power Mode Helper debug control byte
#define SYS_DBG_SPIBUS                     SYS_DBG_ON                 ///< SPIBus task debug control byte
#define SYS_DBG_I2CBUS                     SYS_DBG_ON                 ///< I2CBus task debug control byte
#define SYS_DBG_ISM330DHCX                 SYS_DBG_ON                 ///< ISM330DHCX sensor task debug control byte
#define SYS_DBG_ISM330IS                   SYS_DBG_ON                 ///< ISM330IS sensor task debug control byte
#define SYS_DBG_IIS3DWB                    SYS_DBG_ON                 ///< IIS3DWB sensor task debug control byte
#define SYS_DBG_IMP23ABSU                  SYS_DBG_ON                 ///< IMP23ABSU task debug control byte
#define SYS_DBG_IMP34DT05                  SYS_DBG_ON                 ///< IMP34DT05 task debug control byte
#define SYS_DBG_IIS2MDC                    SYS_DBG_ON                 ///< IIS2MDC task debug control byte
#define SYS_DBG_IIS2DLPC                   SYS_DBG_ON                 ///< IIS2DLPC task debug control byte
#define SYS_DBG_ILPS22QS                   SYS_DBG_ON                 ///< ILPS22QS task debug control byte
#define SYS_DBG_STTS22H                    SYS_DBG_ON                 ///< STTS22H task debug control byte
#define SYS_DBG_IIS2ICLX                   SYS_DBG_ON                 ///< IIS2ICLX sensor task debug control byte
#define SYS_DBG_VL53L8CX                   SYS_DBG_ON                 ///< VL53L8CX task debug control byte
#define SYS_DBG_STHS34PF80                 SYS_DBG_ON                 ///< STHS34PF80 task debug control byte
#define SYS_DBG_VD6283TX                   SYS_DBG_ON                 ///< VD6283TX task debug control byte
#define SYS_DBG_SHT40                      SYS_DBG_ON                 ///< SHT40 task debug control byte
#define SYS_DBG_SGP40                      SYS_DBG_ON                 ///< SGP40 task debug control byte
#define SYS_DBG_LPS22DF                    SYS_DBG_ON                 ///< LPS22DF task debug control byte
#define SYS_DBG_SMUTIL                     SYS_DBG_ON                 ///< Utility task debug control byte
#define SYS_DBG_SDC                        SYS_DBG_ON                 ///< SDCARD task debug control byte
#define SYS_DBG_UTIL                       SYS_DBG_ON                 ///< UTIL task debug control byte
#define SYS_DBG_DT                         SYS_DBG_ON                 ///< Datalog task debug control byte
#define SYS_DBG_BLE                        SYS_DBG_ON                 ///< Datalog task debug control byte
#define SYS_DBG_SYSTS                      SYS_DBG_ON                 ///< SysTimestamp debug control byte
#define SYS_DBG_BCP                        SYS_DBG_ON                 ///< Battery Charger Protocol debug control byte

/* ODeV - hardware configuration for the debug services provided by the framework */
/**********************************************************************************/

#include "mx.h"

/* eLooM DBG UART used for the system log */
extern UART_HandleTypeDef huart2;
void MX_USART2_UART_Init(void);

#define SYS_DBG_UART                       huart2
#define SYS_DBG_UART_INIT                  MX_USART2_UART_Init
#define SYS_DBG_UART_TIMEOUT_MS            5000


#ifdef __cplusplus
}
#endif


#endif /* SYS_DEBUG */
