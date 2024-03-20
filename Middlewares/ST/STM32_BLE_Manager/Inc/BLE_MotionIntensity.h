/**
  ******************************************************************************
  * @file    BLE_MotionIntensity.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Motion Intensity info service APIs.
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
#ifndef _BLE_MOTION_INTENSITY_H_
#define _BLE_MOTION_INTENSITY_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  BLE_ID_ON_DESK                 = 0x00,
  BLE_ID_BED_COUCH_PILLOW        = 0x01,
  BLE_ID_LIGHT_MOVEMENTS         = 0x02,
  BLE_ID_BIKING                  = 0x03,
  BLE_ID_TYPING_WRITING          = 0x04,
  BLE_ID_HI_TYPING__SLOW_WALKING = 0x05,
  BLE_ID_WASHING_HANDS_WALKING   = 0x06,
  BLE_ID_FWALKING                = 0x07,
  BLE_ID_FWALKING_JOGGING        = 0x08,
  BLE_ID_FJOGGING_BRUSHING       = 0x09,
  BLE_ID_SPRINTING               = 0x0A
} BLE_ID_output_t;

typedef void (*CustomReadRequestMotionIntensity_t)(BLE_ID_output_t *MotionIntensityCode);
typedef void (*CustomNotifyEventMotionIntensity_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomReadRequestMotionIntensity_t CustomReadRequestMotionIntensity;
extern CustomNotifyEventMotionIntensity_t CustomNotifyEventMotionIntensity;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Motion Intensity info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Motion Intensity info service
  */
extern BleCharTypeDef *BLE_InitMotionIntensityService(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Motion Intensity Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void BLE_SetMotionIntensityAdvertiseData(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Motion Intensity characteristic
  * @param  BLE_ID_output_t MotionIntensityCode Motion Intensity Recognized
  * @retval tBleStatus   Status
  */
extern tBleStatus BLE_MotionIntensityUpdate(BLE_ID_output_t MotionIntensityCode);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MOTION_INTENSITY_H_ */

