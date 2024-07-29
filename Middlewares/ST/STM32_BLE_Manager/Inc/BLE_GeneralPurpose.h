/**
  ******************************************************************************
  * @file    BLE_GeneralPurpose.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.11.0
  * @date    15-February-2024
  * @brief   General Purpose info services APIs.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BLE_GENERAL_PURPOSE_H_
#define _BLE_GENERAL_PURPOSE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines --------------------------------------------------------- */
#ifndef BLE_GENERAL_PURPOSE_MAX_CHARS_NUM
#define BLE_GENERAL_PURPOSE_MAX_CHARS_NUM 3U
#endif /* BLE_GENERAL_PURPOSE_MAX_CHARS_NUM */

#ifndef BLE_GENERAL_PURPOSE_MAX_CHARS_DIM
#define BLE_GENERAL_PURPOSE_MAX_CHARS_DIM 20
#endif /* BLE_GENERAL_PURPOSE_MAX_CHARS_DIM */

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomNotifyEventGeneralPurpose_t)(uint8_t BLE_CharUuid, BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventGeneralPurpose_t CustomNotifyEventGeneralPurpose;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init General Purpose Service
  * @brief  uint8_t BLE_CharUuid Number of General Purpose Char (UUID[14] byte)
  * @param  uint8_t Size Dimensions of the BLE chars without couting the 2 bytes used for TimeStamp
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for General Purpose info service
  */
extern BleCharTypeDef *BLE_InitGeneralPurposeService(uint8_t BLE_CharUuid,uint8_t Size);

/**
  * @brief  Update General Purpose characteristic value
  * @param  uint8_t BLE_CharUuid Number of General Purpose Char (UUID[14] byte)
  * @param  uint8_t *Data data to Update
  * @retval tBleStatus   Status
  */
tBleStatus BLE_GeneralPurposeStatusUpdate(uint8_t BLE_CharUuid, uint8_t *Data);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_GENERAL_PURPOSE_H_ */

