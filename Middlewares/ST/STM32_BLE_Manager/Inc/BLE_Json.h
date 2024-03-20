/**
  ******************************************************************************
  * @file    BLE_Json.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   BLE_Json info services APIs.
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
#ifndef _BLE_JSON_H_
#define _BLE_JSON_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  BLE_JSON_READ_MODES     = 0,/* This should be the First One */
  BLE_JSON_NFC_WIFI       = 1,
  BLE_JSON_NFC_VCARD      = 2,
  BLE_JSON_GENERIC_TEXT   = 3,
  BLE_JSON_NFC_URL        = 4,
  BLE_JSON_TOTAL_NUMBER   = 5 /* This should be the last One */
} BLE_JSON_MODE_t;

typedef void (*CustomWriteRequestJson_t)(uint8_t *received_msg, uint8_t msg_length);
typedef void (*CustomNotifyEventJson_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomWriteRequestJson_t CustomWriteRequestJson;
extern CustomNotifyEventJson_t CustomNotifyEventJson;

extern char *JsonCommandName[BLE_JSON_TOTAL_NUMBER];
extern char *JsonAnswer;


/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Json info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Json info service
  */
extern BleCharTypeDef *BLE_InitJsonService(void);

/**
  * @brief  Json send buffer
  * @param  uint8_t* buffer
  * @param  uint32_t len
  * @retval tBleStatus Status
  */
extern tBleStatus BLE_JsonUpdate(uint8_t *buffer, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_JSON_H_ */

