/**
  ******************************************************************************
  * @file    BLE_ObjectsDetection.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.11.0
  * @date    15-February-2024
  * @brief   Object detection info services APIs.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BLE_OBJECTS_DETECTION_H_
#define _BLE_OBJECTS_DETECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomWriteRequestObjectsDetection_t)(void);
typedef void (*CustomNotifyEventObjectsDetection_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomWriteRequestObjectsDetection_t CustomWriteRequestObjectsDetection;
extern CustomNotifyEventObjectsDetection_t CustomNotifyEventObjectsDetection;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Object Detection info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for object detection info service
  */
extern BleCharTypeDef *BLE_InitObjectsDetectionService(void);

/**
  * @brief  Update number of the objects and related distance value detection (by ToF sensor)
  * @param  uint16_t Distance Distance of the detected objects
  * @param  uint8_t  HumanPresence Detected object inside fixed distance range
  * @retval tBleStatus   Status
  */
tBleStatus BLE_ObjectsDetectionStatusUpdate(uint16_t *Distances, uint8_t HumanPresence);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_OBJECTS_DETECTION_H_ */

