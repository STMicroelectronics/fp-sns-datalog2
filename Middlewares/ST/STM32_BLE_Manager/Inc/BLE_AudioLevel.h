/**
  ******************************************************************************
  * @file    BLE_AudioLevel.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Audio level info services APIs.
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
#ifndef _BLE_AUDIO_LEVEL_H_
#define _BLE_AUDIO_LEVEL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Typedef --------------------------------------------------------- */
typedef void (*CustomNotifyEventAudioLevel_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventAudioLevel_t CustomNotifyEventAudioLevel;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init audio level info service
  * @param  uint8_t AudioLevelNumber: Number of audio level features (up to 4 audio level are supported)
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for audio level info service
  */
extern BleCharTypeDef *BLE_InitAudioLevelService(uint8_t AudioLevelNumber);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Environmental Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void BLE_SetAudioLevelAdvertiseData(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update audio level characteristic values
  * @param  uint16_t *AudioLevelData:    SNR dB audio level array
  * @param  uint8_t AudioLevelNumber:    Number of audio level features (up to 4 audio level are supported)
  * @retval tBleStatus   Status
  */
tBleStatus BLE_AudioLevelUpdate(uint16_t *AudioLevelData, uint8_t AudioLevelNumber);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_AUDIO_LEVEL_H_ */

