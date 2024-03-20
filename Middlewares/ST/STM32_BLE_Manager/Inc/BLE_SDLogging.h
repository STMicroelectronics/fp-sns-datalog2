/**
  ******************************************************************************
  * @file    BLE_SDLogging.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   SD Logging info services APIs.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BLE_SDLOGGING_H_
#define _BLE_SDLOGGING_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  SDLOG_STOP       = 0x00,
  SDLOG_RUNNING    = 0x01,
  SDLOG_NO_SD_CARD = 0x02
} BLE_SDLog_Status_t;

typedef void (*CustomNotifyEventSDLog_t)(BLE_NotifyEvent_t Event);
typedef void (*CustomWriteRequestSDLog_t)(uint8_t *att_data, uint8_t data_length);
typedef void (*CustomReadRequestSDLog_t)(BLE_SDLog_Status_t *Status, uint32_t *FeatureMask, uint32_t *TimeStep);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventSDLog_t CustomNotifyEventSDLog;
extern CustomWriteRequestSDLog_t CustomWriteSDLog;
extern CustomReadRequestSDLog_t CustomReadRequestSDLog;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init SDLogging info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for environmental info service
  */
extern BleCharTypeDef *BLE_InitSDLogService(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting SDLog Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void BLE_SetSDLogAdvertiseData(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update SDLog characteristic value
  * @param  BLE_SDLog_Status_t Status: SD Log Status
  * @param  uint32_t FeatureMask: Feature Mask
  * @param  uint32_t TimeStep:    Time Step
  * @retval tBleStatus:          Status
  */
extern tBleStatus BLE_SDLoggingUpdate(BLE_SDLog_Status_t Status, uint32_t FeatureMask, uint32_t TimeStep);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_SDLOGGING_H_ */
