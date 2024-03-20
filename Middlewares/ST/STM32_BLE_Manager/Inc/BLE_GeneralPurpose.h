/**
  ******************************************************************************
  * @file    BLE_GeneralPurpose.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   General Purpose info services APIs.
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
typedef void (*CustomNotifyEventGeneralPurpose_t)(uint8_t GP_CharNum, BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventGeneralPurpose_t CustomNotifyEventGeneralPurpose;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init General Purpose Service
  * @param  uint8_t GP_CharNum General Purpose char number to create
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for General Purpose info service
  */
extern BleCharTypeDef *BLE_InitGeneralPurposeService(uint8_t Size);


/**
  * @brief  Update GeneralPurpose characteristic value
  * @param  uint8_t GP_CharNum General Purpose char number to update
  * @param  uint8_t *Data data to Update
  * @retval tBleStatus   Status
  */
tBleStatus BLE_GeneralPurposeStatusUpdate(uint8_t GP_CharNum, uint8_t *Data);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_GENERAL_PURPOSE_H_ */

