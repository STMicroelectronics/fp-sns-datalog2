/**
  ******************************************************************************
  * @file    ble_gesture_recognition.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Gesture Recognition info service APIs.
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
#ifndef _BLE_GESTURE_RECOGNITION_H_
#define _BLE_GESTURE_RECOGNITION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  BLE_GR_NOGESTURE       = 0x00,
  BLE_GR_PICKUP          = 0x01,
  BLE_GR_GLANCE          = 0x02,
  BLE_GR_WAKEUP          = 0x03
} ble_gr_output_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Gesture Recognition info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Gesture Recognition info service
  */
extern ble_char_object_t *ble_init_gesture_recognition_service(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Gesture Recognition Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void ble_set_gesture_recognition_advertise_data(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Gesture Recognition characteristic
  * @param  ble_gr_output_t gesture_recognition_code Gesture Recognition Recognized
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_gesture_recognition_update(ble_gr_output_t gesture_recognition_code);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_gesture_recognition(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
extern void read_request_gesture_recognition_function(ble_gr_output_t *gesture_recognition_code);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_GESTURE_RECOGNITION_H_ */

