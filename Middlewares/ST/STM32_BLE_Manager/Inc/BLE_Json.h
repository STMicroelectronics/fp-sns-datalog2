/**
  ******************************************************************************
  * @file    ble_json.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   BLE_Json info services APIs.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
} ble_json_mode_t;

/* Exported Variables ------------------------------------------------------- */
extern char *json_command_name[BLE_JSON_TOTAL_NUMBER];
extern char *json_answer;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Json info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Json info service
  */
extern ble_char_object_t *ble_init_json_service(void);

/**
  * @brief  Json send buffer
  * @param  uint8_t* buffer
  * @param  uint32_t len
  * @retval ble_status_t Status
  */
extern ble_status_t ble_json_update(uint8_t *buffer, uint32_t len);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_json(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
extern void write_request_json_function(uint8_t *received_msg, uint32_t msg_length);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_JSON_H_ */

