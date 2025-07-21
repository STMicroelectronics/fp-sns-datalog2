/**
  ******************************************************************************
  * @file    ble_normalization.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Event Counter info service APIs.
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
#ifndef _BLE_NORMALIZATION_H_
#define _BLE_NORMALIZATION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Event Counter info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for normalization info service
  */
extern ble_char_object_t *ble_init_normalization_service(void);

/**
  * @brief  Update Event Counter characteristic
  * @param float Event Counter Number
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_normalization_update(float normalization);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_normalization(ble_notify_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_NORMALIZATION_H_ */

