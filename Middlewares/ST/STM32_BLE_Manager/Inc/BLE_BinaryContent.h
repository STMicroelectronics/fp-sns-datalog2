/**
  ******************************************************************************
  * @file    BLE_BinaryContent.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.11.0
  * @date    15-February-2024
  * @brief   BLE_BinaryContent info services APIs.
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

/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *BleCharPointer
  * @param  uint16_t handle Handle of the attribute
  * @param  uint16_t Offset
  * @param  uint8_t data_length
  * @param  uint8_t *att_data
  * @retval None
  */
extern __weak void Write_Request_BinaryContent(void *BleCharPointer, uint16_t handle, uint16_t Offset, uint8_t data_length,
                                        uint8_t *att_data);

/**
  * @brief  This function is called to parse a Binary Content packet.
  * @param  buffer_out: pointer to the output buffer.
  * @param  buffer_in: pointer to the input data.
  * @param  len: buffer in length
  * @retval Buffer out length.
  */
extern __weak uint32_t BLE_BinaryContent_Parse(uint8_t **buffer_out, uint8_t *buffer_in, uint32_t len);

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


/**
  * @brief  BinaryContent Reset Status
  * @param  None
  * @retval None
  */
extern void BLE_BinaryContentReset(void);


#ifdef __cplusplus
}
#endif

#endif /* _BLE_BINARY_CONTENT_H_ */

