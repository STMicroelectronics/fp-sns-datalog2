/**
  ******************************************************************************
  * @file    BLE_FitnessActivities.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Fitness Activities info service APIs.
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
#ifndef _BLE_FITNESS_ACTIVITIES_H_
#define _BLE_FITNESS_ACTIVITIES_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomWriteRequestFitnessActivities_t)(uint8_t FitnessActivities);
typedef void (*CustomNotifyEventFitnessActivities_t)(BLE_NotifyEvent_t Event);

typedef enum
{
  BLE_MFA_NOACTIVITY          = 0x00,
  BLE_MFA_BICEPCURL           = 0x01,
  BLE_MFA_SQUAT               = 0x02,
  BLE_MFA_PUSHUP              = 0x03
} BLE_FitnessActivitiesType_t;

/* Exported Variables ------------------------------------------------------- */

extern CustomWriteRequestFitnessActivities_t CustomWriteRequestFitnessActivities;
extern CustomNotifyEventFitnessActivities_t CustomNotifyEventFitnessActivities;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Fitness Activities info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Fitness Activities info service
  */
extern BleCharTypeDef *BLE_InitFitnessActivitiesService(void);

/**
  * @brief  Update Fitness Activities characteristic
  * @param  uint8_t MotionCode Detected Motion
  * @retval tBleStatus   Status
  */
extern tBleStatus BLE_FitnessActivitiesUpdate(uint8_t Activity, uint32_t Counter);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_FITNESS_ACTIVITIES_H_ */

