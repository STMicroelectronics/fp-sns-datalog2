/**
  ******************************************************************************
  * @file    ble_fft_amplitude.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   FFT Amplitude info services APIs.
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
#ifndef _BLE_FFT_AMPLITUDE_H_
#define _BLE_FFT_AMPLITUDE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init FFT Amplitude info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for FFT Amplitude info service
  */
extern ble_char_object_t *ble_init_fft_amplitude_service(void);

/*
 * @brief  Update FFT Amplitude characteristic value
 * @param  uint8_t *data_to_send:
 *                  - 2 bytes for number of samples
 *                  - 1 byte for number of components (up to 3 components)
 *                  - 4 bytes for frequency steps
 *                  - 4 bytes for each component
 *                     (if number of components is more 1, for example 3, send the data in this format:
 *                      X1,X2,X3,...Xn,Y1,Y2,Y3,...Yn,X1,Z2,Z3,...Zn)
 * @param  uint16_t data_number Number of samples
 * @param  uint8_t *sending_fft
 * @param  uint16_t *count_send_data
 * @retval ble_status_t   Status
 */
extern ble_status_t ble_fft_amplitude_update(uint8_t *data_to_send, uint16_t data_number, uint8_t *sending_fft,
                                             uint16_t *count_send_data);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_fft_amplitude(ble_notify_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_FFT_AMPLITUDE_H_ */

