/**
  ******************************************************************************
  * @file    ble_pedometer_algorithm.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Pedometer Algorithm info service APIs.
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
#ifndef _BLE_PEDOMETER_ALGORITHM_H_
#define _BLE_PEDOMETER_ALGORITHM_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef struct
{
  uint8_t cadence;      /* [steps/min] */
  uint32_t n_steps;
} ble_pm_output_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Pedometer Algorithm info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Pedometer Algorithm info service
  */
extern ble_char_object_t *ble_init_pedometer_algorithm_service(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Pedometer Algorithm Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void ble_set_pedometer_algorithm_advertise_data(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Pedometer Algorithm characteristic
  * @param  ble_pm_output_t pedometer_algorithm_data Pedometer Algorithm data output
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_pedometer_algorithm_update(ble_pm_output_t *pedometer_algorithm_data);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_pedometer_algorithm(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
extern void read_request_pedometer_algorithm_function(ble_pm_output_t *pedometer_algorithm_data);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_PEDOMETER_ALGORITHM_H_ */

