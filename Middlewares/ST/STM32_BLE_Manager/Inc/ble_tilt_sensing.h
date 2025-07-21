/**
  ******************************************************************************
  * @file    ble_tilt_sensing.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Tilt Sensing info service APIs.
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
#ifndef _BLE_TILT_SENSING_H_
#define _BLE_TILT_SENSING_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/

/* Exported typedef --------------------------------------------------------- */
/* Either pitch, roll and gravity inclination or theta, psi and phi */
typedef struct
{
  float angles_array[3];
} ble_angles_output_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Tilt Sensing info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Tilt Sensing info service
  */
extern ble_char_object_t *ble_init_tilt_sensing_service(void);

/**
  * @brief  Update Tilt Sensing characteristic
  * @param  ble_angles_output_t tilt_sensing_measure Tilt Sensing Recognized
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_tilt_sensing_update(ble_angles_output_t tilt_sensing_measure);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_tilt_sensing(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
extern void read_request_tilt_sensing_function(ble_angles_output_t *tilt_sensing_measure);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_TILT_SENSING_H_ */

