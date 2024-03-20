/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    BLE_Manager_Conf_Template.h
  * @author  System Research & Applications Team - Catania Lab.
  * @brief   BLE Manager configuration template file.
  *          This file should be copied to the application folder and renamed
  *          to BLE_Manager_Conf.h.
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
#ifndef __BLE_MANAGER_CONF_H__
#define __BLE_MANAGER_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Exported define ------------------------------------------------------------*/
/* Select the used bluetooth core:
 *
 * BLUENRG_1_2     0x00
 * BLUENRG_MS      0x01
 * BLUENRG_LP      0x02
 * BLUE_WB         0x03
*/

/* Comment this define if do not use parson */
#define BLE_MANAGER_USE_PARSON

#define BLUE_CORE BLUENRG_1_2

#ifndef BLE_MANAGER_USE_PARSON
#define BLE_MANAGER_NO_PARSON
#endif /* BLE_MANAGER_USE_PARSON */

/* Out-Of-Band data */
#define OUT_OF_BAND_ENABLEDATA      0x00
/* Defines the Max dimension of the Bluetooth config characteristic */
#define DEFAULT_MAX_CONFIG_CHAR_LEN      20
/* Bluetooth address types */
#define ADDRESS_TYPE      1
/* Enable High Power mode. High power mode should be enabled only to reach the maximum output power. */
#define ENABLE_HIGH_POWER_MODE      0x01
/* Power amplifier output level - The allowed PA levels depends on the device (see user manual for detailsl) */
#define POWER_AMPLIFIER_OUTPUT_LEVEL      0x04
/* Length for configuration values. */
#define CONFIG_VALUE_LENGTH      6
/* GAP Roles */
#define GAP_ROLES      0x01
/* Maximum number of allocable bluetooth characteristics */
#define BLE_MANAGER_MAX_ALLOCABLE_CHARS      32
/* Configuration values */
#define CONFIG_VALUE_OFFSETS      0x00
/* Defines the Max dimension of the Bluetooth std error characteristic */
#define DEFAULT_MAX_STDERR_CHAR_LEN      20
/* Defines the Max dimension of the Bluetooth characteristics for each packet */
#define DEFAULT_MAX_CHAR_LEN      255
/* MITM protection requirements */
#define MITM_PROTECTION_REQUIREMENTS      0x01
/* IO capabilities */
#define IO_CAPABILITIES      0x00
/* Authentication requirements */
#define AUTHENTICATION_REQUIREMENTS      0x01
/* Secure connection support option code */
#define SECURE_CONNECTION_SUPPORT_OPTION_CODE      0x01
/* Secure connection key press notification option code */
#define SECURE_CONNECTION_KEYPRESS_NOTIFICATION      0x00
/* Advertising policy for filtering (white list related) */
#define ADVERTISING_FILTER      0x00
/* USER CODE BEGIN 1 */

#define BLE_MANAGER_SDKV2

#define DEFAULT_MAX_STDOUT_CHAR_LEN     DEFAULT_MAX_CHAR_LEN
#define DEFAULT_MAX_EXTCONFIG_CHAR_LEN  DEFAULT_MAX_CHAR_LEN

/* For enabling the capability to handle BlueNRG Congestion */
#define ACC_BLUENRG_CONGESTION

/* USER CODE END 1 */

/* Define the Delay function to use inside the BLE Manager (HAL_Delay/osDelay) */
#define BLE_MANAGER_DELAY HAL_Delay
/* Function InitBleManager_BLE_Stack use this define as delay if defined */
/* #define BLE_INITIAL_DELAY HAL_Delay */

/****************** Memory management functions **************************/
#define BLE_MALLOC_FUNCTION      malloc
#define BLE_FREE_FUNCTION        free
#define BLE_MEM_CPY              memcpy

/*---------- Print messages from BLE Manager files at middleware level -----------*/

/* USER CODE BEGIN 2 */

/* Uncomment/Comment the following define for  disabling/enabling print messages from BLE Manager files */
#define BLE_MANAGER_DEBUG

#define BLE_DEBUG_LEVEL 1

#ifdef BLE_MANAGER_DEBUG
/**
  * User can change here printf with a custom implementation.
  * For example:
  * #include "STBOX1_config.h"
  * #include "main.h"
  * #define BLE_MANAGER_PRINTF  STBOX1_PRINTF
  */

#include <stdio.h>
#define BLE_MANAGER_PRINTF(...) printf(__VA_ARGS__)
#else /* BLE_MANAGER_DEBUG */
#define BLE_MANAGER_PRINTF(...)
#endif /* BLE_MANAGER_DEBUG */

/* USER CODE END 2 */

#ifdef __cplusplus
}
#endif

#endif /* __BLE_MANAGER_CONF_H__*/

