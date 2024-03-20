/**
  ******************************************************************************
  * @file    BLE_BinaryContent.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   BLE_BinaryContent info services APIs.
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
#ifndef _BLE_BINARY_CONTENT_H_
#define _BLE_BINARY_CONTENT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Defines ----------------------------------------------------------*/
#ifndef DEFAULT_MAX_BINARY_CONTENT_CHAR_LEN
#define DEFAULT_MAX_BINARY_CONTENT_CHAR_LEN  20
#endif /* DEFAULT_MAX_BINARY_CONTENT_CHAR_LEN */

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomWriteRequestBinaryContent_t)(uint8_t *received_msg, uint32_t msg_length);
typedef void (*CustomNotifyEventBinaryContent_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomWriteRequestBinaryContent_t CustomWriteRequestBinaryContent;
extern CustomNotifyEventBinaryContent_t CustomNotifyEventBinaryContent;

/* Exported functions ------------------------------------------------------- */

__weak void Write_Request_BinaryContent(void *BleCharPointer, uint16_t handle, uint16_t Offset, uint8_t data_length,
                                        uint8_t *att_data);

/**
  * @brief  Init BinaryContent info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for BinaryContent info service
  */
extern BleCharTypeDef *BLE_InitBinaryContentService(void);

/**
  * @brief  BinaryContent send buffer
  * @param  uint8_t* buffer
  * @param  uint32_t len
  * @retval tBleStatus Status
  */
extern tBleStatus BLE_BinaryContentUpdate(uint8_t *buffer, uint8_t len);

/**
  * @brief  BinaryContent Set Max Char Length
  * @param  uint16_t MaxCharLength
  * @retval none
  */
extern void BLE_BinaryContentSetMaxCharLength(uint16_t MaxCharLength);

/**
  * @brief  BinaryContent Get Max Char Length
  * @param  None
  * @retval uint16_t MaxCharLength
  */
extern uint16_t BLE_BinaryContentGetMaxCharLength(void);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_BINARY_CONTENT_H_ */

