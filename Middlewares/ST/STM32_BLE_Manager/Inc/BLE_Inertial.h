/**
  ******************************************************************************
  * @file    BLE_Inertial.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Inertial info services APIs.
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
#ifndef _BLE_INERTIAL_H_
#define _BLE_INERTIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomNotifyEventInertial_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventInertial_t CustomNotifyEventInertial;

/* Exported Types ------------------------------------------------------- */
typedef struct
{
  int32_t Axis_x;
  int32_t Axis_y;
  int32_t Axis_z;
} BLE_MANAGER_INERTIAL_Axes_t;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init inertial info service
  * @param  uint8_t AccEnable:   1 for enabling the BLE accelerometer feature, 0 otherwise.
  * @param  uint8_t GyroEnable:  1 for enabling the BLE gyroscope feature, 0 otherwise.
  * @param  uint8_t MagEnabled:  1 for esabling the BLE magnetometer feature, 0 otherwise.
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for environmental info service
  */
extern BleCharTypeDef *BLE_InitInertialService(uint8_t AccEnable, uint8_t GyroEnable, uint8_t MagEnabled);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Inertial Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void BLE_SetInertialAdvertiseData(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update acceleration/Gryoscope and Magneto characteristics value
  * @param  BLE_MANAGER_INERTIAL_Axes_t Acc:     Structure containing acceleration value in mg
  * @param  BLE_MANAGER_INERTIAL_Axes_t Gyro:    Structure containing Gyroscope value
  * @param  BLE_MANAGER_INERTIAL_Axes_t Mag:     Structure containing magneto value
  * @retval tBleStatus      Status
  */
extern tBleStatus BLE_AccGyroMagUpdate(BLE_MANAGER_INERTIAL_Axes_t *Acc,
                                       BLE_MANAGER_INERTIAL_Axes_t *Gyro,
                                       BLE_MANAGER_INERTIAL_Axes_t *Mag);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_INERTIAL_H_ */

