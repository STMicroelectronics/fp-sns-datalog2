/**
  ******************************************************************************
  * @file    BLE_ECompass.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   E-Compass info service APIs.
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
#ifndef _BLE_ECOMPASS_H_
#define _BLE_ECOMPASS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomNotifyECompass_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyECompass_t CustomNotifyECompass;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init E-Compass info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for E-Compass info service
  */
extern BleCharTypeDef *BLE_InitECompassService(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting E-Compass Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void BLE_SetECompassAdvertiseData(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update E-Compass characteristic
  * @param  uint16_t Angle measured
  * @retval tBleStatus   Status
  */
extern tBleStatus BLE_ECompassUpdate(uint16_t Angle);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_ECOMPASS_H_ */

