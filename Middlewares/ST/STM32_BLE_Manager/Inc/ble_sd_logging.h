/**
  ******************************************************************************
  * @file    ble_sd_logging.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   SD Logging info services APIs.
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
#ifndef _BLE_SDLOGGING_H_
#define _BLE_SDLOGGING_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  SDLOG_STOP       = 0x00,
  SDLOG_RUNNING    = 0x01,
  SDLOG_NO_SD_CARD = 0x02
} ble_sd_log_status_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init SDLogging info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for environmental info service
  */
extern ble_char_object_t *ble_init_sd_log_service(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting SDLog Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void ble_set_sd_log_advertise_data(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update SDLog characteristic value
  * @param  ble_sd_log_status_t status: SD Log Status
  * @param  uint32_t feature_mask: Feature Mask
  * @param  uint32_t time_step:    Time Step
  * @retval ble_status_t:          Status
  */
extern ble_status_t ble_sd_logging_update(ble_sd_log_status_t status, uint32_t feature_mask, uint32_t time_step);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_sd_logging(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
extern void read_request_sd_logging_function(ble_sd_log_status_t *status, uint32_t *feature_mask, uint32_t *time_step);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
extern void write_request_sd_logging_function(uint8_t *att_data, uint8_t data_length);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_SDLOGGING_H_ */
