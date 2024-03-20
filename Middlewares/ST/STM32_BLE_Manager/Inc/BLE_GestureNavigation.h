/**
  ******************************************************************************
  * @file    BLE_GestureNavigation.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Gesture Recognition info service APIs.
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
#ifndef _BLE_GESTURE_NAVIGATION_H_
#define _BLE_GESTURE_NAVIGATION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/

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
} BLE_GestureNavigation_t;

typedef enum
{
  BN_UNDEFINED = 0x00,
  BN_LEFT      = 0x01,
  BN_RIGHT     = 0x02,
  BN_UP        = 0x03,
  BN_DOWN      = 0x04
} BLE_ButtonNavigation_t;

typedef void (*CustomNotifyEventGestureNavigation_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventGestureNavigation_t CustomNotifyEventGestureNavigation;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Gesture Navigation info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Gesture Navigation info service
  */
extern BleCharTypeDef *BLE_InitGestureNavigationService(void);


/**
  * @brief  Update Gesture Navigation characteristic
  * @param  BLE_GestureNavigation_t GestureNavigation Gesture Navigation Code
  * @param  BLE_ButtonNavigation_t ButtonNavigation Button Navigation Code
  * @retval tBleStatus   Status
  */
extern tBleStatus BLE_GestureNavigationUpdate(BLE_GestureNavigation_t GestureNavigation,
                                              BLE_ButtonNavigation_t ButtonNavigation);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_GESTURE_NAVIGATION_H_ */

