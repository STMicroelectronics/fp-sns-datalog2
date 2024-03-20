/**
  ******************************************************************************
  * @file    BLE_HighSpeedDataLog.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   BLE_HighSpeedDataLog info services APIs.
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
#ifndef _BLE_HIGH_SPEED_DATA_LOG_H_
#define _BLE_HIGH_SPEED_DATA_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomNotifyEventHighSpeedDataLog_t)(BLE_NotifyEvent_t Event);
typedef void (*CustomWriteRequestHighSpeedDataLog_t)(uint8_t *att_data, uint8_t data_length);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventHighSpeedDataLog_t CustomNotifyEventHighSpeedDataLog;
extern CustomWriteRequestHighSpeedDataLog_t CustomWriteRequestHighSpeedDataLog;


/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init High Speed Data Log info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for High Speed Data Log info service
  */
extern BleCharTypeDef *BLE_InitHighSpeedDataLogService(void);

/**
  * @brief  High Speed Data Log Send Buffer
  * @param  uint8_t* buffer
  * @param  uint32_t len
  * @retval tBleStatus   Status
  */
tBleStatus BLE_HighSpeedDataLogSendBuffer(uint8_t *buffer, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_HIGH_SPEED_DATA_LOG_H_ */

