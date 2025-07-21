/**
  ******************************************************************************
  * @file    ble_implementation.h
  * @author  System Research & Applications Team - Catania Lab.
  * @brief   BLE Implementation header template file.
  *          This file should be copied to the application folder and renamed
  *          to ble_implementation.h.
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

/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BLE_IMPLEMENTATION_H_
#define _BLE_IMPLEMENTATION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/**
  * User can added here the header file for the selected BLE features.
  * For example:
  * #include "ble_environmental.h"
  * #include "ble_inertial.h"
  */

#include "ble_high_speed_data_log.h"
#include "ble_machine_learning_core.h"
#include "ble_pn_p_like.h"
#include "ble_raw_pn_pl_controlled.h"

/* necessary for using floor on BLE_Manager.h */
#include <math.h>

/* Exported Defines --------------------------------------------------------*/
#define STM32L4xx

/* For Custom Command */
#define ENABLE_READ_CUSTOM_COMMAND_EXT_CONFIG      0
/* Enable/Disable BlueNRG config extend services */
#define ENABLE_EXT_CONFIG      1
/* Enable/Disable BlueNRG config services */
#define ENABLE_CONFIG      0
/* For Set Date Command */
#define ENABLE_SET_DATE_EXT_CONFIG      0
/* For Change Secure PIN Command */
#define ENABLE_CHANGE_SECURE_PIN_EXT_CONFIG      0
/* Enable/Disable Secure Connection */
#define ENABLE_SECURE_CONNECTION      0
/* For Version Fw Command */
#define ENABLE_VERSION_FW_EXT_CONFIG      1
/* For Read Certificate Command */
#define ENABLE_READ_CERTIFICATE_EXT_CONFIG      0
/* For Clear Secure Data Base Command */
#define ENABLE_CLEAR_SECURE_DATA_BASE_EXT_CONFIG      1
/* For Set board Name Command */
#define ENABLE_SET_NAME_EXT_CONFIG      0
/* Number of audio channels (Max audio channels 4) */
#define AUDIO_CHANNELS_NUMBER      1
/* For Reboot on DFU Command */
#define ENABLE_REBOOT_ON_DFU_MODE_EXT_CONFIG      0
/* For Power off Command */
#define ENABLE_POWER_OFF_EXT_CONFIG      0
/* Number of the general purpose features to use */
#define NUM_GENERAL_PURPOSE      3
/* Enable/Disable magnetometer data (Disable= 0- Enable=1) */
#define ENABLE_MAG_DATA      1
/* For Help Command */
#define ENABLE_HELP_EXT_CONFIG      1
/* Secure PIN */
#define SECURE_PIN      123456
/* Audio Scene Classificatio algorithm code */
#define ALGORITHM_CODE      0
/* variable/Fixed length flag */
#define CHAR_SIZE_VARIABLE      0
/* Enable/Disable pressure data (Disable= 0- Enable=1) */
#define ENABLE_ENV_PRESSURE_DATA      1
/* For Swapping the Flash Banks */
#define ENABLE_BANKS_SWAP_EXT_CONFIG      1
/* Max Dimension of the BLE Char */
#define MAX_CHAR_SIZE      3
/* Enable/Disable Random Secure PIN */
#define ENABLE_RANDOM_SECURE_PIN      0
/* Enable/Disable BlueNRG console services */
#define ENABLE_CONSOLE      1
/* Enable/Disable giroscope data (Disable= 0- Enable=1) */
#define ENABLE_GYRO_DATA      1
/* Size of the general purpose feature */
#define GENERAL_PURPOSE_SIZE_3      1
/* For Set Certificate Command */
#define ENABLE_SET_CERTIFICATE_EXT_CONFIG      0
/* Size of the general purpose feature */
#define GENERAL_PURPOSE_SIZE_2      1
/* For UID Command */
#define ENABLE_STM32_UID_EXT_CONFIG      1
/* For Set Wi-Fi Command */
#define ENABLE_SET_WIFI_EXT_CONFIG      0
/* Enable/Disable humidity data (Disable= 0- Enable=1) */
#define ENABLE_ENV_HUMIDITY_DATA      1
/* For Info Command */
#define ENABLE_INFO_EXT_CONFIG      1
/* Size of the general purpose feature */
#define GENERAL_PURPOSE_SIZE_1      1
/* Number of quaternion to send (max value 3) */
#define NUMBER_OF_QUATERNION      1
/* For Set Time Command */
#define ENABLE_SET_TIME_EXT_CONFIG      0
/* Enable/Disable number of temperature (0, 1, 2) */
#define ENABLE_ENV_TEMPERATURE_DATA      1
/* For Reading the Flash Banks Fw Ids */
#define ENABLE_READ_BANKS_FW_ID_EXT_CONFIG      1
/* Supported hardware platform */
#define USED_PLATFORM      0x0DU
/* Enable/Disable accelerometer data (Disable= 0- Enable=1) */
#define ENABLE_ACC_DATA      1
/* For Power Status Command */
#define ENABLE_POWER_STATUS_EXT_CONFIG      0
/* Select the used hardware platform
 *
 * STEVAL-WESU1                         --> BLE_MANAGER_STEVAL_WESU1_PLATFORM
 * STEVAL-STLKT01V1 (SensorTile)        --> BLE_MANAGER_SENSOR_TILE_PLATFORM
 * STEVAL-BCNKT01V1 (BlueCoin)          --> BLE_MANAGER_BLUE_COIN_PLATFORM
 * STEVAL-IDB008Vx                      --> BLE_MANAGER_STEVAL_IDB008VX_PLATFORM
 * STEVAL-BCN002V1B (BlueTile)          --> BLE_MANAGER_STEVAL_BCN002V1_PLATFORM
 * STEVAL-MKSBOX1V1 (SensorTile.box)    --> BLE_MANAGER_SENSOR_TILE_BOX_PLATFORM
 * DISCOVERY-IOT01A                     --> BLE_MANAGER_DISCOVERY_IOT01A_PLATFORM
 * STEVAL-STWINKT1                      --> BLE_MANAGER_STEVAL_STWINKT1_PLATFORM
 * STEVAL-STWINKT1B                     --> BLE_MANAGER_STEVAL_STWINKT1B_PLATFORM
 * STEVAL_STWINBX1                      --> BLE_MANAGER_STEVAL_STWINBX1_PLATFORM
 * SENSOR_TILE_BOX_PRO                  --> BLE_MANAGER_SENSOR_TILE_BOX_PRO_PLATFORM
 * STEVAL_ASTRA1                        --> BLE_MANAGER_STEVAL_ASTRA1_PLATFORM
 * STM32NUCLEO Board                    --> BLE_MANAGER_NUCLEO_PLATFORM
 * STM32U5A5ZJ_NUCLEO Board             --> BLE_MANAGER_STM32U5A5ZJ_NUCLEO_PLATFORM
 * STM32U575ZI_NUCLEO Board             --> BLE_MANAGER_STM32U575ZI_NUCLEO_PLATFORM
 * STM32F446RE_NUCLEO Board             --> BLE_MANAGER_STM32F446RE_NUCLEO_PLATFORM
 * STM32L053R8_NUCLEO Board             --> BLE_MANAGER_STM32L053R8_NUCLEO_PLATFORM
 * STM32L476RG_NUCLEO Board             --> BLE_MANAGER_STM32L476RG_NUCLEO_PLATFORM
 * STM32F401RE_NUCLEO Board             --> BLE_MANAGER_STM32F401RE_NUCLEO_PLATFORM
 * Not defined platform                 --> BLE_MANAGER_UNDEF_PLATFORM
 *
 * For example:
 * #define BLE_MANAGER_USED_PLATFORM  BLE_MANAGER_NUCLEO_PLATFORM
 *
*/

/* Used platform */
#define BLE_MANAGER_USED_PLATFORM       BLE_MANAGER_STEVAL_STWINKT1B_PLATFORM

/* STM32 Unique ID */
#define BLE_STM32_UUID          UID_BASE

/* STM32 MCU_ID */
#ifdef DBGMCU_BASE
#define BLE_STM32_MCU_ID        ((uint32_t *)DBGMCU_BASE)
#else /* DBGMCU_BASE */
#define BLE_STM32_MCU_ID        ((uint32_t *)0x00000000UL)
#endif /* DBGMCU_BASE */

/* STM32  Microcontrolles type */
#define BLE_STM32_MICRO "STM32L4xx"

/* USER CODE BEGIN 1 */

/* Package Version firmware */
#define BLE_VERSION_FW_MAJOR  '4'
#define BLE_VERSION_FW_MINOR  '0'
#define BLE_VERSION_FW_PATCH  '0'

/* Firmware Package Name */
#define BLE_FW_PACKAGENAME          "HSD2v31"

/* Max Stream ID dedicated to custom data, utility stream (like advertise option byte) could be added after this */
#define MAX_CUSTOM_DATA_STREAM_ID          (SM_MAX_SENSORS)
/* USER CODE END 1 */

/* Exported Variables ------------------------------------------------------- */

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/* Exported functions ------------------------------------------------------- */
extern uint8_t BLE_GetFWID(void);
extern void ble_init_custom_service(void);
extern void ble_set_custom_advertise_data(uint8_t *manuf_data);
extern void bluetooth_init(void);
extern void set_board_name(void);
extern void enable_extended_configuration_command(void);

#if (BLUE_CORE != BLUE_WB)
extern void reset_ble_manager(void);
#endif /* (BLUE_CORE != BLUE_WB) */

/* USER CODE BEGIN 3 */

/* USER CODE END 3 */

#ifdef __cplusplus
}
#endif

#endif /* _BLE_IMPLEMENTATION_H_ */

