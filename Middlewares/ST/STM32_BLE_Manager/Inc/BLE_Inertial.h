/**
  ******************************************************************************
  * @file    ble_inertial.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Inertial info services APIs.
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
#ifndef _BLE_INERTIAL_H_
#define _BLE_INERTIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Defines ----------------------------------------------------------*/
/* Feature mask for Accelerometer */
#define FEATURE_MASK_ACC   0x00800000
/* Feature mask for Gyroscope */
#define FEATURE_MASK_GRYO  0x00400000
/* Feature mask for Magnetometer */
#define FEATURE_MASK_MAG   0x00200000

/* Exported Types ------------------------------------------------------- */
typedef struct
{
  int32_t axis_x;
  int32_t axis_y;
  int32_t axis_z;
} ble_manager_inertial_axes_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init inertial info service
  * @param  uint8_t acc_enable:   1 for enabling the BLE accelerometer feature, 0 otherwise.
  * @param  uint8_t gyro_enable:  1 for enabling the BLE gyroscope feature, 0 otherwise.
  * @param  uint8_t mag_enable:  1 for esabling the BLE magnetometer feature, 0 otherwise.
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for environmental info service
  */
extern ble_char_object_t *ble_init_inertial_service(uint8_t acc_enable, uint8_t gyro_enable, uint8_t mag_enable);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Inertial Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void ble_set_inertial_advertise_data(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update acceleration/Gryoscope and Magneto characteristics value
  * @param  ble_manager_inertial_axes_t acc:     Structure containing acceleration value in mg
  * @param  ble_manager_inertial_axes_t gyro:    Structure containing Gyroscope value
  * @param  ble_manager_inertial_axes_t mag:     Structure containing magneto value
  * @retval ble_status_t      Status
  */
extern ble_status_t ble_acc_gyro_mag_update(ble_manager_inertial_axes_t *acc,
                                            ble_manager_inertial_axes_t *gyro,
                                            ble_manager_inertial_axes_t *mag);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_inertial(ble_notify_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_INERTIAL_H_ */

