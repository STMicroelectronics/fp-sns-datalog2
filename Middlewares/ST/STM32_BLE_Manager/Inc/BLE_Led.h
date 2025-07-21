/**
  ******************************************************************************
  * @file    ble_led.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Led info services APIs.
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
#ifndef _BLE_LED_H_
#define _BLE_LED_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Defines ----------------------------------------------------------*/
/* Feature mask for LED */
#define FEATURE_MASK_LED 0x20000000

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init led info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for led info service
  */
extern ble_char_object_t *ble_init_led_service(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Led Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void ble_set_led_advertise_data(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update LEDs characteristic value
  * @param  uint8_t led_status LEDs status 0/1 (off/on)
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_led_status_update(uint8_t led_status);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_led(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
extern void read_request_led_function(uint8_t *led_status);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_LED_H_ */

