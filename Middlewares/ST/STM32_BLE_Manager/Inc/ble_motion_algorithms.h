/**
  ******************************************************************************
  * @file    ble_motion_algorithms.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Motion Algorithms info service APIs.
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
#ifndef _BLE_MOTION_ALGORITHMS_H_
#define _BLE_MOTION_ALGORITHMS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  BLE_MOTION_ALGORITHMS_NO_ALGO = 0x00,
  BLE_MOTION_ALGORITHMS_PE      = 0x01,
  BLE_MOTION_ALGORITHMS_SD      = 0x02,
  BLE_MOTION_ALGORITHMS_VC      = 0x03
} ble_motion_algorithms_type_t;

typedef void (*custom_notify_event_motion_algorithms_t)(ble_notify_event_t event,
                                                        ble_motion_algorithms_type_t algorithm);

typedef void (*custom_write_request_motion_algorithms_t)(ble_motion_algorithms_type_t algorithm);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Motion Algorithms info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Motion Algorithms info service
  */
extern ble_char_object_t *ble_init_motion_algorithms_service(void);

/**
  * @brief  Update Motion Algorithms characteristic
  * @param  uint8_t motion_code Detected Motion
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_motion_algorithms_update(uint8_t motion_code);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_motion_algorithms(ble_notify_event_t event,
                                           ble_motion_algorithms_type_t algorithm);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
extern void write_request_motion_algorithms_function(ble_motion_algorithms_type_t algorithm);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MOTION_ALGORITHMS_H_ */

