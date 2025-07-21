/**
  ******************************************************************************
  * @file    ble_gas_concentration.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Gas concentration info services APIs.
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
#ifndef _BLE_GAS_CONCENTRATION_H_
#define _BLE_GAS_CONCENTRATION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init gas concentration info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for gas concentration info service
  */
extern ble_char_object_t *ble_init_gas_concentration_service(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Gas Concentration Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void ble_set_gas_concentration_advertise_data(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Gas Concentration Value
  * @param  uint16_t Measured Gas Concentration value
  * @retval ble_status_t   Status
  */
extern ble_status_t BLE_GasConcentrationStatusUpdate(uint32_t gas);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_gas_concentration(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
extern void read_request_gas_concentration_function(uint32_t *gas);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_GAS_CONCENTRATION_H_ */

