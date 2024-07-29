/**
  ******************************************************************************
  * @file    BLE_MedicalSignal16Bit.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.11.0
  * @date    15-February-2024
  * @brief   Medical Signal 16 Bits info service APIs.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BLE_MEDICAL_SIGNAL_16BIT_H_
#define _BLE_MEDICAL_SIGNAL_16BIT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/
#ifndef DEFAULT_MAX_MEDICAL_SIGNAL_16BIT_CHAR_LEN
#define DEFAULT_MAX_MEDICAL_SIGNAL_16BIT_CHAR_LEN  20
#endif /* DEFAULT_MAX_MEDICAL_SIGNAL_16BIT_CHAR_LEN */

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomNotifyEventMedicalSignal16Bit_t)(BLE_NotifyEvent_t Event);

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  BLE_ELECTROMYOGRAPHY    = 0x06, // 1 Signal
  BLE_BIOIMPEDANCE        = 0x07, // 4 Signal
  BLE_GALVANIC_SKIN_RESP  = 0x08, // 1 Signal
  BLE_ACCELEROMETER       = 0x09, // 3 Signal
  BLE_GYROSCOPE           = 0x0A, // 3 Signal
  BLE_MAGNETOMETER        = 0x0B, // 3 Signal
  BLE_TEMPERATURE         = 0x0D, // 1 Signal
  BLE_ECG_CHANNEL1        = 0x10, // 1 Signal
  BLE_ECG_CHANNEL2        = 0x11, // 1 Signal
  BLE_ECG_CHANNEL3        = 0x12, // 1 Signal
  BLE_ECG_CHANNEL4        = 0x13, // 1 Signal
  BLE_ECG_CHANNEL5        = 0x14, // 1 Signal
  BLE_ECG_CHANNEL6        = 0x15, // 1 Signal
  BLE_ECG_CHANNEL7        = 0x16, // 1 Signal
  BLE_ECG_CHANNEL8        = 0x17, // 1 Signal
  BLE_ECG_CHANNEL9        = 0x18, // 1 Signal
  BLE_ECG_CHANNEL10       = 0x19, // 1 Signal
  BLE_ECG_CHANNEL11       = 0x1A, // 1 Signal
  BLE_ECG_CHANNEL12       = 0x1B, // 1 Signal
  BLE_BIOIMPEDANCE_DZ     = 0x20, // 1 Signal
  BLE_BIOIMPEDANCE_Z0     = 0x21, // 1 Signal
  BLE_BIOIMPEDANCE_ZE     = 0x22, // 1 Signal
  BLE_BIOIMPEDANCE_ZC     = 0x23 // 1 Signal
} BLE_MedicalSignal16Bit_t;


/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventMedicalSignal16Bit_t CustomNotifyEventMedicalSignal16Bit;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Medical Signal 16 Bits info service
  * @param  uint16_t MaxLenght Max Char Lenght
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Normalization info service
  */
extern BleCharTypeDef *BLE_InitMedicalSignal16BitService(uint16_t MaxLenght);

/**
  * @brief  Update Medical Signal 16 Bits characteristic
  * @param  BLE_MedicalSignal16Bit_t DataTypeID Signal Data Type ID
  * @param  uint32_t timeStamp timeStamp
  * @param  int32_t SampleDataSize Total Number of Values
  * @param  int16_t *SampleData  Values array
  * @retval tBleStatus   Status
  */
extern tBleStatus BLE_MedicalSignal16BitUpdate(BLE_MedicalSignal16Bit_t DataTypeID, uint32_t timeStamp, int32_t SampleDataSize, int16_t *SampleData);

/**
  * @brief  Medical Signal 16 Bits Get Max Char Length
  * @param  None
  * @retval uint16_t MaxCharLength
  */
extern uint16_t BLE_MedicalSignal16BitGetMaxCharLength(void);

/**
  * @brief  Medical Signal 16 Bits Set Max Char Length
  * @param  uint16_t MaxCharLength
  * @retval none
  */
extern void BLE_MedicalSignal16BitSetMaxCharLength(uint16_t MaxCharLength);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MEDICAL_SIGNAL_16BIT_H_ */

