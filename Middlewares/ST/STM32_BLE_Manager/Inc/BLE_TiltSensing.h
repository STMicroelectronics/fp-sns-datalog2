/**
  ******************************************************************************
  * @file    BLE_TiltSensing.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Tilt Sensing info service APIs.
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
  float AnglesArray[3];
} BLE_ANGLES_output_t;

typedef void (*CustomReadRequestTiltSensing_t)(BLE_ANGLES_output_t *TiltSensingMeasure);
typedef void (*CustomNotifyEventTiltSensing_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomReadRequestTiltSensing_t CustomReadRequestTiltSensing;
extern CustomNotifyEventTiltSensing_t CustomNotifyEventTiltSensing;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Tilt Sensing info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Tilt Sensing info service
  */
extern BleCharTypeDef *BLE_InitTiltSensingService(void);

/**
  * @brief  Update Tilt Sensing characteristic
  * @param  BLE_ANGLES_output_t TiltSensingMeasure Tilt Sensing Recognized
  * @retval tBleStatus   Status
  */
tBleStatus BLE_TiltSensingUpdate(BLE_ANGLES_output_t TiltSensingMeasure);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_TILT_SENSING_H_ */

