/**
  ******************************************************************************
  * @file    ble_battery.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Battery info services APIs.
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
#ifndef _BLE_BATTERY_H_
#define _BLE_BATTERY_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init battery info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for battery info service
  */
extern ble_char_object_t *ble_init_battery_service(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Battery Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void ble_set_battery_advertise_data(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Battery characteristic
  * @param  int32_t battery_level %Charge level
  * @param  uint32_t voltage Battery voltage
  * @param  uint32_t current Battery current (0x8000 if not available)
  * @param  uint32_t status Charging/Discharging
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_battery_update(uint32_t battery_level, uint32_t voltage, uint32_t current, uint32_t status);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_battery(ble_notify_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_BATTERY_H_ */

