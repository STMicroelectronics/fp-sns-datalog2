/**
  ******************************************************************************
  * @file    ble_medical_signal_24_bit.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Medical Signal 24 Bits info service APIs.
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
#ifndef _BLE_MEDICAL_SIGNAL_24BIT_H_
#define _BLE_MEDICAL_SIGNAL_24BIT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/
#ifndef DEFAULT_MAX_MEDICAL_SIGNAL_24BIT_CHAR_LEN
#define DEFAULT_MAX_MEDICAL_SIGNAL_24BIT_CHAR_LEN  20
#endif /* DEFAULT_MAX_MEDICAL_SIGNAL_24BIT_CHAR_LEN */

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  BLE_RNB_RED_PPG1  = 0x00, /* 1 Signal */
  BLE_RNB_BLUE_PPG2 = 0x01, /* 1 Signal */
  BLE_PPG3          = 0x02, /* 1 Signal */
  BLE_PPG4          = 0x03, /* 1 Signal */
  BLE_PPG5          = 0x04, /* 1 Signal */
  BLE_PPG6          = 0x05, /* 1 Signal */
  BLE_PRESSURE      = 0x0C  /* 1 Signal */
} ble_medical_signal_24_bit_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Medical Signal 24 Bits info service
  * @param  uint16_t max_lenght Max Char Length
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Normalization info service
  */
extern ble_char_object_t *ble_init_medical_signal_24_bit_service(uint16_t max_lenght);

/**
  * @brief  Update Medical Signal 24 Bits characteristic
  * @param  ble_medical_signal_24_bit_t data_type_id Signal Data Type ID
  * @param  uint32_t time_stamp time_stamp
  * @param  int32_t sample_data_size Total Number of Values
  * @param  int16_t *sample_data  Values array
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_medical_signal_24_bit_update(ble_medical_signal_24_bit_t data_type_id, uint32_t time_stamp,
                                                     int32_t sample_data_size, int32_t *sample_data);

/**
  * @brief  Medical Signal 24 Bits Get Max Char Length
  * @param  None
  * @retval uint24_t max_char_length
  */
extern uint16_t ble_medical_signal_24_bit_get_max_char_length(void);

/**
  * @brief  Medical Signal 24 Bits Set Max Char Length
  * @param  uint16_t max_char_length
  * @retval none
  */
extern void ble_medical_signal_24_bit_set_max_char_length(uint16_t max_char_length);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_medical_signal_24_bit(ble_notify_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MEDICAL_SIGNAL_24BIT_H_ */

