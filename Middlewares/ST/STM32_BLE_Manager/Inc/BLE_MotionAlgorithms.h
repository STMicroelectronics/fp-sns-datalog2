/**
  ******************************************************************************
  * @file    BLE_MotionAlgorithms.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Motion Algorithms info service APIs.
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
#ifndef _BLE_MOTION_ALGORITHMS_H_
#define _BLE_MOTION_ALGORITHMS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  BLE_MOTION_ALGORITHMS_NO_ALGO = 0x00,
  BLE_MOTION_ALGORITHMS_PE      = 0x01,
  BLE_MOTION_ALGORITHMS_SD      = 0x02,
  BLE_MOTION_ALGORITHMS_VC      = 0x03
} BLE_MotionAlgorithmsType_t;

typedef void (*CustomNotifyEventMotionAlgorithms_t)(BLE_NotifyEvent_t Event, BLE_MotionAlgorithmsType_t Algorithm);
typedef void (*CustomWriteRequestMotionAlgorithms_t)(BLE_MotionAlgorithmsType_t Algorithm);

/* Exported Variables ------------------------------------------------------- */

extern CustomWriteRequestMotionAlgorithms_t CustomWriteRequestMotionAlgorithms;
extern CustomNotifyEventMotionAlgorithms_t CustomNotifyEventMotionAlgorithms;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Motion Algorithms info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Motion Algorithms info service
  */
extern BleCharTypeDef *BLE_InitMotionAlgorithmsService(void);

/**
  * @brief  Update Motion Algorithms characteristic
  * @param  uint8_t MotionCode Detected Motion
  * @retval tBleStatus   Status
  */
extern tBleStatus BLE_MotionAlgorithmsUpdate(uint8_t MotionCode);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MOTION_ALGORITHMS_H_ */

