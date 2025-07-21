/**
  ******************************************************************************
  * @file    ble_e_compass.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   E-Compass info service APIs.
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
#ifndef _BLE_ECOMPASS_H_
#define _BLE_ECOMPASS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Defines --------------------------------------------------------*/
/* Feature mask for e-compass */
#define FEATURE_MASK_ECOMPASS 0x00000040

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init E-Compass info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for E-Compass info service
  */
extern ble_char_object_t *ble_init_e_compass_service(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting E-Compass Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void ble_set_e_compass_advertise_data(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update E-Compass characteristic
  * @param  uint16_t angle measured
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_e_compass_update(uint16_t angle);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_e_compass(ble_notify_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_ECOMPASS_H_ */

