/**
  ******************************************************************************
  * @file    BLE_AudioSourceLocalization.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   E-BLE_AudioSourceLocalization info service APIs.
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
#ifndef _BLE_AUDIO_SOURCE_LOCALIZATION_H_
#define _BLE_AUDIO_SOURCE_LOCALIZATION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomNotifyEventAudioSourceLocalization_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventAudioSourceLocalization_t CustomNotifyEventAudioSourceLocalization;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Audio Source Localization info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Audio Source Localization info service
  */
extern BleCharTypeDef *BLE_InitAudioSourceLocalizationService(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Audio Source Localization Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void BLE_SetAudioSourceLocalizationAdvertiseData(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Audio Source Localization characteristic
  * @param  Buint16_t Angle  Audio Source Localization evaluated angle
  * @retval tBleStatus   Status
  */
extern tBleStatus BLE_AudioSourceLocalizationUpdate(uint16_t Angle);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_AUDIO_SOURCE_LOCALIZATION_H_ */

