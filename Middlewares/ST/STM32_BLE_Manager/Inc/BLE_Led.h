/**
  ******************************************************************************
  * @file    BLE_Led.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Led info services APIs.
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
#ifndef _BLE_LED_H_
#define _BLE_LED_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomNotifyEventLed_t)(BLE_NotifyEvent_t Event);
typedef void (*CustomReadRequestLed_t)(uint8_t *LedStatus);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventLed_t CustomNotifyEventLed;
extern CustomReadRequestLed_t CustomReadRequestLed;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init led info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for led info service
  */
extern BleCharTypeDef *BLE_InitLedService(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Led Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void BLE_SetLedAdvertiseData(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update LEDs characteristic value
  * @param  uint8_t LedStatus LEDs status 0/1 (off/on)
  * @retval tBleStatus   Status
  */
tBleStatus BLE_LedStatusUpdate(uint8_t LedStatus);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_LED_H_ */

