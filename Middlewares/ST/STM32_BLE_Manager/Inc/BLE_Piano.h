/**
  ******************************************************************************
  * @file    ble_piano.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   BLE_Pianoinfo services APIs.
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
#ifndef _BLE_PIANO_H_
#define _BLE_PIANO_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines --------------------------------------------------------- */
#define BLE_PIANO_START 1
#define BLE_PIANO_STOP 0

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Piano info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for High Speed Data Log info service
  */
extern ble_char_object_t *ble_init_piano_service(void);

/**
  * @brief  Piano Send Buffer
  * @param  uint8_t* buffer
  * @param  uint32_t len
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_piano_send_buffer(uint8_t *buffer, uint32_t len);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_piano(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
extern void write_request_piano_function(uint8_t *att_data, uint8_t data_length);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_PIANO_H_ */

