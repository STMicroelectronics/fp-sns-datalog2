/**
  ******************************************************************************
  * @file    BLE_MedicalSignal24Bit.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.11.0
  * @date    15-February-2024
  * @brief   Medical Signal 24 Bits info service APIs.
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
typedef void (*CustomNotifyEventMedicalSignal24Bit_t)(BLE_NotifyEvent_t Event);

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  BLE_RNB_RED_PPG1  = 0x00, // 1 Signal
  BLE_RNB_BLUE_PPG2 = 0x01, // 1 Signal
  BLE_PPG3          = 0x02, // 1 Signal
  BLE_PPG4          = 0x03, // 1 Signal
  BLE_PPG5          = 0x04, // 1 Signal
  BLE_PPG6          = 0x05, // 1 Signal
  BLE_PRESSURE      = 0x0C  // 1 Signal
} BLE_MedicalSignal24Bit_t;


/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventMedicalSignal24Bit_t CustomNotifyEventMedicalSignal24Bit;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Medical Signal 24 Bits info service
  * @param  uint16_t MaxLenght Max Char Lenght
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Normalization info service
  */
extern BleCharTypeDef *BLE_InitMedicalSignal24BitService(uint16_t MaxLenght);

/**
  * @brief  Update Medical Signal 24 Bits characteristic
  * @param  BLE_MedicalSignal24Bit_t DataTypeID Signal Data Type ID
  * @param  uint32_t timeStamp timeStamp
  * @param  int32_t SampleDataSize Total Number of Values
  * @param  int16_t *SampleData  Values array
  * @retval tBleStatus   Status
  */
extern tBleStatus BLE_MedicalSignal24BitUpdate(BLE_MedicalSignal24Bit_t DataTypeID, uint32_t timeStamp, int32_t SampleDataSize, int32_t *SampleData);

/**
  * @brief  Medical Signal 24 Bits Get Max Char Length
  * @param  None
  * @retval uint24_t MaxCharLength
  */
extern uint16_t BLE_MedicalSignal24BitGetMaxCharLength(void);

/**
  * @brief  Medical Signal 24 Bits Set Max Char Length
  * @param  uint16_t MaxCharLength
  * @retval none
  */
extern void BLE_MedicalSignal24BitSetMaxCharLength(uint16_t MaxCharLength);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MEDICAL_SIGNAL_24BIT_H_ */

