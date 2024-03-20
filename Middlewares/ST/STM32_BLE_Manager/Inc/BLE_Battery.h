/**
  ******************************************************************************
  * @file    BLE_Battery.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Battery info services APIs.
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
#ifndef _BLE_BATTERY_H_
#define _BLE_BATTERY_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomNotifyEventBattery_t)(BLE_NotifyEvent_t Battery);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventBattery_t CustomNotifyEventBattery;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init battery info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for battery info service
  */
extern BleCharTypeDef *BLE_InitBatteryService(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Battery Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void BLE_SetBatteryAdvertiseData(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Battery characteristic
  * @param  int32_t BatteryLevel %Charge level
  * @param  uint32_t Voltage Battery Voltage
  * @param  uint32_t Current Battery Current (0x8000 if not available)
  * @param  uint32_t Status Charging/Discharging
  * @retval tBleStatus   Status
  */
tBleStatus BLE_BatteryUpdate(uint32_t BatteryLevel, uint32_t Voltage, uint32_t Current, uint32_t Status);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_BATTERY_H_ */

