/**
  ******************************************************************************
  * @file    BLE_Manager_Conf.h
  * @author  SRA
  * @brief   BLE Manager configuration template file.
  *          This file should be copied to the application folder and renamed
  *          to BLE_Manager_Conf.h.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BLE_MANAGER_CONF_H__
#define __BLE_MANAGER_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "services/sysmem.h"

/* Exported define ------------------------------------------------------------*/
#define BLUE_CORE BLUENRG_1_2

/* Uncomment the following define for BlueST-SDK V2 */
#define BLE_MANAGER_SDKV2


/* Define the Max dimension of the Bluetooth characteristics for Debug Console services  */
#define DEFAULT_MAX_STDOUT_CHAR_LEN     217
#define DEFAULT_MAX_STDERR_CHAR_LEN     20

#define BLE_MANAGER_MAX_ALLOCABLE_CHARS 7U

/* For enabling the capability to handle BlueNRG Congestion */
#define ACC_BLUENRG_CONGESTION

/* Define the Max Dimension of the Bluetooth characteristics for PnPL (only for Notification) */
#define DEFAULT_MAX_PNPL_NOTIFICATION_CHAR_LEN 217

/* Define the Max Dimension of the Bluetooth characteristics for PnPL Raw controlled (only for Notification) */
#define DEFAULT_MAX_RAW_NOTIFICATION_CHAR_LEN 217

/* Define the Delay function to use inside the BLE Manager (HAL_Delay/osDelay) */
#define BLE_MANAGER_DELAY tx_thread_sleep

/****************** Malloc/Free **************************/
#define BLE_MALLOC_FUNCTION SysAlloc
#define BLE_FREE_FUNCTION SysFree
#define BLE_MEM_CPY memcpy


/*---------- Print messages from BLE Manager files at middleware level -----------*/
/* Uncomment/Comment the following define for  disabling/enabling print messages from BLE Manager files */
//#define BLE_MANAGER_DEBUG

#ifdef BLE_MANAGER_DEBUG
/* Define the Verbosity level (1/2/3) */
#define BLE_DEBUG_LEVEL 3

/**
  * User can change here printf with a custom implementation.
  */
#define BLE_MANAGER_PRINTF(...) printf(__VA_ARGS__)

#else
#define BLE_MANAGER_PRINTF(...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BLE_MANAGER_CONF_H__*/

