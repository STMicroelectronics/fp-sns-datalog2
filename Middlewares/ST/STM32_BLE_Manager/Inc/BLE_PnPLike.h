/**
  ******************************************************************************
  * @file    BLE_PnPLike.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.11.0
  * @date    15-February-2024
  * @brief   BLE_PnPLike info services APIs.
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
#ifndef _BLE_PNPLIKE_H_
#define _BLE_PNPLIKE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Defines ----------------------------------------------------------*/
#ifndef DEFAULT_MAX_PNPL_NOTIFICATION_CHAR_LEN
#define DEFAULT_MAX_PNPL_NOTIFICATION_CHAR_LEN  20
#endif /* DEFAULT_MAX_PNPL_NOTIFICATION_CHAR_LEN */

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomWriteRequestPnPLike_t)(uint8_t *received_msg, uint8_t msg_length);
typedef void (*CustomNotifyEventPnPLike_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomWriteRequestPnPLike_t CustomWriteRequestPnPLike;
extern CustomNotifyEventPnPLike_t CustomNotifyEventPnPLike;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init PnPLike info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for PnPLike info service
  */
extern BleCharTypeDef *BLE_InitPnPLikeService(void);

/**
  * @brief  PnPLike send buffer
  * @param  uint8_t* buffer
  * @param  uint32_t len
  * @retval tBleStatus Status
  */
extern tBleStatus BLE_PnPLikeUpdate(uint8_t *buffer, uint8_t len);


/**
  * @brief  PnPLike Set Max Char Length
  * @param  uint16_t MaxCharLength
  * @retval none
  */
extern void BLE_PnPLikeSetMaxCharLength(uint16_t MaxCharLength);

/**
  * @brief  PnPLike Get Max Char Length
  * @param  None
  * @retval uint16_t MaxCharLength
  */
extern uint16_t BLE_PnPLikeGetMaxCharLength(void);

/**
  * @brief  PnPLike Reset Status
  * @param  None
  * @retval None
  */
extern void BLE_PnPLikeReset(void);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_PNPLIKE_H_ */

