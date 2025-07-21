/**
  ******************************************************************************
  * @file    ble_gesture_navigation.h
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
#ifndef _BLE_GESTURE_NAVIGATION_H_
#define _BLE_GESTURE_NAVIGATION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  GN_UNDEFINED    = 0x00,
  GN_SWYPE_L_TO_R = 0x01,
  GN_SWYPE_R_TO_L = 0x02,
  GN_SWYPE_U_TO_D = 0x03,
  GN_SWYPE_D_TO_U = 0x04,
  GN_SINGLE_PRESS = 0x05,
  GN_DOUBLE_PRESS = 0x06,
  GN_TRIPLE_PRESS = 0x07,
  GN_LONG_PRESS   = 0x08
} ble_gesture_navigation_t;

typedef enum
{
  BN_UNDEFINED = 0x00,
  BN_LEFT      = 0x01,
  BN_RIGHT     = 0x02,
  BN_UP        = 0x03,
  BN_DOWN      = 0x04
} ble_button_navigation_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Gesture Navigation info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Gesture Navigation info service
  */
extern ble_char_object_t *ble_init_gesture_navigation_service(void);


/**
  * @brief  Update Gesture Navigation characteristic
  * @param  ble_gesture_navigation_t gesture_navigation Gesture Navigation Code
  * @param  ble_button_navigation_t button_navigation Button Navigation Code
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_gesture_navigation_update(ble_gesture_navigation_t gesture_navigation,
                                                  ble_button_navigation_t button_navigation);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_gesture_navigation(ble_notify_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_GESTURE_NAVIGATION_H_ */

