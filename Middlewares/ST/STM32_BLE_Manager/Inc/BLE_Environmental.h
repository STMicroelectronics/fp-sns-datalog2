/**
  ******************************************************************************
  * @file    BLE_Environmental.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Environmental info services APIs.
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
#ifndef _BLE_ENVIRONMENTAL_H_
#define _BLE_ENVIRONMENTAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomReadRequestEnv_t)(int32_t *Press, uint16_t *Hum, int16_t *Temp1, int16_t *Temp2);
typedef void (*CustomNotifyEventEnv_t)(BLE_NotifyEvent_t Env);

/* Exported Variables ------------------------------------------------------- */
extern CustomReadRequestEnv_t CustomReadRequestEnv;
extern CustomNotifyEventEnv_t CustomNotifyEventEnv;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init environmental info service
  * @param  uint8_t PressEnable:    1 for enabling the BLE pressure feature, 0 otherwise.
  * @param  uint8_t HumEnable:      1 for enabling the BLE humidity feature, 0 otherwise.
  * @param  uint8_t NumTempEnabled: 0 for disabling BLE temperature feature
  *                                 1 for enabling only one BLE temperature feature
  *                                 2 for enabling two BLE temperatures feature
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for environmental info service
  */
extern BleCharTypeDef *BLE_InitEnvService(uint8_t PressEnable, uint8_t HumEnable, uint8_t NumTempEnabled);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Environmental Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void BLE_SetEnvAdvertiseData(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Environmental characteristic value
  * @param  int32_t Press:       Pressure in mbar (Set 0 if not used)
  * @param  uint16_t Hum:        humidity RH (Relative Humidity) in thenths of % (Set 0 if not used)
  * @param  int16_t Temp1:       Temperature in tenths of degree (Set 0 if not used)
  * @param  int16_t Temp2:       Temperature in tenths of degree (Set 0 if not used)
  * @retval tBleStatus:          Status
  */
extern tBleStatus BLE_EnvironmentalUpdate(int32_t Press, uint16_t Hum, int16_t Temp1, int16_t Temp2);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_ENVIRONMENTAL_H_ */

