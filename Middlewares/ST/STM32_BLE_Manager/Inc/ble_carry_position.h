/**
  ******************************************************************************
  * @file    ble_carry_position.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Carry Position info service APIs.
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
#ifndef _BLE_CARRY_POSITION_H_
#define _BLE_CARRY_POSITION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  BLE_CP_UNKNOWN             = 0x00,
  BLE_CP_ONDESK              = 0x01,
  BLE_CP_INHAND              = 0x02,
  BLE_CP_NEARHEAD            = 0x03,
  BLE_CP_SHIRTPOCKET         = 0x04,
  BLE_CP_TROUSERPOCKET       = 0x05,
  BLE_CP_ARMSWING            = 0x06,
  BLE_CP_JACKETPOCKET        = 0x07
} ble_cp_output_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Carry Position info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Carry Position info service
  */
extern ble_char_object_t *ble_init_carry_position_service(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Carry Position Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void ble_set_carry_position_advertise_data(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Carry Position characteristic
  * @param  ble_cp_output_t carry_position_code Carry Position Recognized
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_carry_position_update(ble_cp_output_t carry_position_code);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_carry_position(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
extern void read_request_carry_position_function(ble_cp_output_t *carry_position_code);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_CARRY_POSITION_H_ */

