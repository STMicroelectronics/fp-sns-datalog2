/**
  ******************************************************************************
  * @file    ble_activity_recognition.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Activity Recognition info service APIs.
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
#ifndef _BLE_ACTIVITY_RECOGNITION_H_
#define _BLE_ACTIVITY_RECOGNITION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  BLE_AR_NOACTIVITY          = 0x00,
  BLE_AR_STATIONARY          = 0x01,
  BLE_AR_WALKING             = 0x02,
  BLE_AR_FASTWALKING         = 0x03,
  BLE_AR_JOGGING             = 0x04,
  BLE_AR_BIKING              = 0x05,
  BLE_AR_DRIVING             = 0x06,
  BLE_AR_STAIRS              = 0x07,
  BLE_AR_ADULTINCAR          = 0x08,
  BLE_AR_ERROR               = 0x09
} ble_ar_output_t;

typedef enum
{
  HAR_ALGO_IDX_NONE           = 0xFF,
  HAR_GMP_IDX                 = 0x00,
  HAR_IGN_IDX                 = 0x01,
  HAR_IGN_WSDM_IDX            = 0x02,
  HAR_MLC_LSM6DSOX_ID         = 0x03,
  HAR_ALGO_IDX_NUMBER         = 0x04
} ble_ar_algo_idx_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Activity Recognition info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Activity Recognition info service
  */
extern ble_char_object_t *ble_init_act_rec_service(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Activity Recognition Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void ble_set_act_rec_advertise_data(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Activity Recognition characteristic
  * @param  ble_ar_output_t activity_code Activity Recognized
  * @param  ble_ar_algo_idx_t Algorithm Code
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_act_rec_update(ble_ar_output_t activity_code, ble_ar_algo_idx_t algorithm);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_activity_recognition(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
extern void read_request_activity_recognition_function(ble_ar_output_t *activity_code, ble_ar_algo_idx_t *algorithm);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_ACTIVITY_RECOGNITION_H_ */

