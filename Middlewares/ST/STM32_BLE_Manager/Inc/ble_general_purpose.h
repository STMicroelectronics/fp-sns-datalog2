/**
  ******************************************************************************
  * @file    ble_general_purpose.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   General Purpose info services APIs.
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

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init General Purpose Service
  * @brief  uint8_t ble_char_uuid Number of General Purpose Char (UUID[14] byte)
  * @param  uint8_t size Dimensions of the BLE chars without counting the 2 bytes used for TimeStamp
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for General Purpose info service
  */
extern ble_char_object_t *ble_init_general_purpose_service(uint8_t ble_char_uuid, uint8_t size);

/**
  * @brief  Update General Purpose characteristic value
  * @param  uint8_t ble_char_uuid Number of General Purpose Char (UUID[14] byte)
  * @param  uint8_t *data data to Update
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_general_purpose_status_update(uint8_t ble_char_uuid, uint8_t *data);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_general_purpose(uint8_t ble_char_uuid, ble_notify_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_GENERAL_PURPOSE_H_ */

