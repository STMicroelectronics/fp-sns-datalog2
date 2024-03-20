/**
  ******************************************************************************
  * @file    BLE_FFT_Amplitude.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   FFT Amplitude info services APIs.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomNotifyEventFFT_Amplitude_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventFFT_Amplitude_t CustomNotifyEventFFT_Amplitude;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init FFT Amplitude info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for FFT Amplitude info service
  */
extern BleCharTypeDef *BLE_InitFFTAmplitudeService(void);

/*
 * @brief  Update FFT Amplitude characteristic value
 * @param  uint8_t *DataToSend:
 *                  - 2 bytes for number of samples
 *                  - 1 byte for number of components (up to 3 components)
 *                  - 4 bytes for frequency steps
 *                  - 4 bytes for each component
 *                     (if number of components is more 1, for example 3, send the data in this format:
 *                      X1,X2,X3,...Xn,Y1,Y2,Y3,...Yn,X1,Z2,Z3,...Zn)
 * @param  uint16_t DataNumber Number of samples
 * @param  uint8_t *SendingFFT
 * @param  uint16_t *CountSendData
 * @retval tBleStatus   Status
 */
tBleStatus BLE_FFTAmplitudeUpdate(uint8_t *DataToSend, uint16_t DataNumber, uint8_t *SendingFFT,
                                  uint16_t *CountSendData);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_FFT_AMPLITUDE_H_ */

