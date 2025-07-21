/**
  ******************************************************************************
  * @file    ble_motion_intensity.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Motion Intensity info service APIs.
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
#ifndef _BLE_MOTION_INTENSITY_H_
#define _BLE_MOTION_INTENSITY_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  BLE_ID_ON_DESK                 = 0x00,
  BLE_ID_BED_COUCH_PILLOW        = 0x01,
  BLE_ID_LIGHT_MOVEMENTS         = 0x02,
  BLE_ID_BIKING                  = 0x03,
  BLE_ID_TYPING_WRITING          = 0x04,
  BLE_ID_HI_TYPING__SLOW_WALKING = 0x05,
  BLE_ID_WASHING_HANDS_WALKING   = 0x06,
  BLE_ID_FWALKING                = 0x07,
  BLE_ID_FWALKING_JOGGING        = 0x08,
  BLE_ID_FJOGGING_BRUSHING       = 0x09,
  BLE_ID_SPRINTING               = 0x0A
} ble_id_output_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Motion Intensity info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Motion Intensity info service
  */
extern ble_char_object_t *ble_init_motion_intensity_service(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Motion Intensity Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void ble_set_motion_intensity_advertise_data(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Motion Intensity characteristic
  * @param  ble_id_output_t motion_intensity_code Motion Intensity Recognized
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_motion_intensity_update(ble_id_output_t motion_intensity_code);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_motion_intensity(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
extern void read_request_motion_intensity_function(ble_id_output_t *motion_intensity_code);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MOTION_INTENSITY_H_ */

