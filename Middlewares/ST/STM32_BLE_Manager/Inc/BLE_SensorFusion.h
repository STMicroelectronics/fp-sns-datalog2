/**
  ******************************************************************************
  * @file    BLE_SensorFusion.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Sensor Fusion info service APIs.
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
#ifndef _BLE_SENSOR_FUSION_H_
#define _BLE_SENSOR_FUSION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/

/* Exported typedef --------------------------------------------------------- */
typedef struct
{
  int32_t Axis_x;
  int32_t Axis_y;
  int32_t Axis_z;
} BLE_MOTION_SENSOR_Axes_t;

typedef void (*CustomNotifyEventSensorFusion_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventSensorFusion_t CustomNotifyEventSensorFusion;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Sensor Fusion info service
  * @param  uint8_t NumberQuaternionsToSend  Number of quaternions send (1,2,3)
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Sensor Fusion info service
  */
extern BleCharTypeDef *BLE_InitSensorFusionService(uint8_t NumberQuaternionsToSend);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Sensor Fusion Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void BLE_SetSensorFusionAdvertiseData(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update quaternions characteristic value
  * @param  BLE_MOTION_SENSOR_Axes_t *data Structure containing the quaterions
  * @param  uint8_t NumberQuaternionsToSend  Number of quaternions send (1,2,3)
  * @retval tBleStatus      Status
  */
extern tBleStatus BLE_SensorFusionUpdate(BLE_MOTION_SENSOR_Axes_t *data, uint8_t NumberQuaternionsToSend);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_SENSOR_FUSION_H_ */

