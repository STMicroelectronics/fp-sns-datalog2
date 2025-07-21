/**
  ******************************************************************************
  * @file    ble_sensor_fusion.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Sensor Fusion info service APIs.
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
#ifndef _BLE_SENSOR_FUSION_H_
#define _BLE_SENSOR_FUSION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Defines ----------------------------------------------------------*/
/* Feature mask for Sensor fusion short precision */
#define FEATURE_MASK_SENSORFUSION_SHORT 0x00000100

/* W2ST command for asking the calibration status */
#define W2ST_COMMAND_CAL_STATUS 0xFF
/* W2ST command for resetting the calibration */
#define W2ST_COMMAND_CAL_RESET  0x00
/* W2ST command for stopping the calibration process */
#define W2ST_COMMAND_CAL_STOP   0x01

/* Exported typedef --------------------------------------------------------- */
typedef struct
{
  int32_t axis_x;
  int32_t axis_y;
  int32_t axis_z;
} ble_motion_sensor_axes_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Sensor Fusion info service
  * @param  uint8_t number_quaternions_to_send  Number of quaternions send (1,2,3)
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Sensor Fusion info service
  */
extern ble_char_object_t *ble_init_sensor_fusion_service(uint8_t number_quaternions_to_send);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Sensor Fusion Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void ble_set_sensor_fusion_advertise_data(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update quaternions characteristic value
  * @param  ble_motion_sensor_axes_t *data Structure containing the quaterions
  * @param  uint8_t number_quaternions_to_send  Number of quaternions send (1,2,3)
  * @retval ble_status_t      Status
  */
extern ble_status_t ble_sensor_fusion_update(ble_motion_sensor_axes_t *data, uint8_t number_quaternions_to_send);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_sensor_fusion(ble_notify_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_SENSOR_FUSION_H_ */

