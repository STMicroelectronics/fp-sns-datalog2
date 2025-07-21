/**
  ******************************************************************************
  * @file    ble_fft_alarm_subrange_status.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   FFT Alarm Subrange Status info services APIs.
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
#ifndef _BLE_FFT_ALARM_SUBRANGE_STATUS_H_
#define _BLE_FFT_ALARM_SUBRANGE_STATUS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
/**
  * @brief  X-Y-Z Generic Value in float
  */
typedef struct
{
  float axis_x;         /* !< Generic X Value in float */
  float axis_y;         /* !< Generic Y Value in float */
  float axis_z;         /* !< Generic Z Value in float */
} ble_fft_alarm_subrange_value_t;

/**
  * @brief  Warning Alarm Datatype
  */
typedef enum
{
  BLE_GOOD_SUBRANGE          = (uint8_t)0x00,  /* !< GOOD STATUS */
  BLE_WARNING_SUBRANGE       = (uint8_t)0x01,  /* !< WARNING STATUS */
  BLE_ALARM_SUBRANGE         = (uint8_t)0x02,  /* !< ALARM STATUS */
  BLE_NONE_SUBRANGE          = (uint8_t)0x03,  /* !< RFU STATUS */
} ble_fft_alarm_type_t;

/**
  * @brief  STATUS for FFT Subrange Warning-Alarm
  */
typedef struct
{
  ble_fft_alarm_type_t status_axis_x;   /* !< X STATUS ALARM */
  ble_fft_alarm_type_t status_axis_y;   /* !< Y STATUS ALARM */
  ble_fft_alarm_type_t status_axis_z;   /* !< Z STATUS ALARM */
} ble_fft_alarm_subrange_status_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init FFT Alarm Subrange Status info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for FFT Alarm Subrange Status info service
  */
extern ble_char_object_t *ble_init_fft_alarm_subrange_status_service(void);

/*
 * @brief  Update FFT Alarm Subrange RMS status value
 * @param  ble_fft_alarm_subrange_status_t alarm_status
 * @param  ble_fft_alarm_subrange_value_t subrange_max_value
 * @param  ble_fft_alarm_subrange_value_t subrange_freq_max_value
 * @retval ble_status_t   Status
 */
extern ble_status_t ble_fft_alarm_subrange_status_update(ble_fft_alarm_subrange_status_t alarm_status,
                                                         ble_fft_alarm_subrange_value_t subrange_max_value,
                                                         ble_fft_alarm_subrange_value_t subrange_freq_max_value);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_fft_alarm_subrange_status(ble_notify_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_FFT_ALARM_SUBRANGE_STATUS_H_ */

