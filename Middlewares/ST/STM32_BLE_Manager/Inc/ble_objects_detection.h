/**
  ******************************************************************************
  * @file    ble_objects_detection.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Object detection info services APIs.
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
#ifndef _BLE_OBJECTS_DETECTION_H_
#define _BLE_OBJECTS_DETECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Object Detection info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for object detection info service
  */
extern ble_char_object_t *ble_init_objects_detection_service(void);

/**
  * @brief  Update number of the objects and related distance value detection (by ToF sensor)
  * @param  uint16_t distances Distance of the detected objects
  * @param  uint8_t  human_presence Detected object inside fixed distance range
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_objects_detection_status_update(uint16_t *distances, uint8_t human_presence);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_objects_detection(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
extern void write_request_objects_detection_function(void);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_OBJECTS_DETECTION_H_ */

