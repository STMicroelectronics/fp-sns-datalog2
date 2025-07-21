/**
  ******************************************************************************
  * @file    ble_acc_event.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Acceleromenter HW Event service APIs.
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
#ifndef _BLE_ACC_EVENT_H_
#define _BLE_ACC_EVENT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Defines ----------------------------------------------------------*/
/* Feature mask for Accelerometer events */
#define FEATURE_MASK_ACC_EVENTS 0x00000400

typedef enum
{
  ACC_NOT_USED     = 0x00,
  ACC_6D_OR_TOP    = 0x01,
  ACC_6D_OR_LEFT   = 0x02,
  ACC_6D_OR_BOTTOM = 0x03,
  ACC_6D_OR_RIGTH  = 0x04,
  ACC_6D_OR_UP     = 0x05,
  ACC_6D_OR_DOWN   = 0x06,
  ACC_TILT         = 0x08,
  ACC_FREE_FALL    = 0x10,
  ACC_SINGLE_TAP   = 0x20,
  ACC_DOUBLE_TAP   = 0x40,
  ACC_WAKE_UP      = 0x80
} acc_event_type_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init HW Acceleromenter Event info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for HW Acceleromenter Event info service
  */
extern ble_char_object_t *ble_init_acc_env_service(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting HW Acceleromenter Event Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void ble_set_acc_env_advertise_data(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update HW Acceleromenter Event characteristic value
  * @param  uint8_t acc_events
  * @param  uint16_t num_steps
  * @retval ble_status_t: Status
  */
extern ble_status_t ble_acc_env_update(uint8_t acc_events, uint16_t num_steps);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_acc_event(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
extern void read_request_acc_event_function(uint8_t *acc_events, uint16_t *num_steps);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_ACC_EVENT_H_ */

