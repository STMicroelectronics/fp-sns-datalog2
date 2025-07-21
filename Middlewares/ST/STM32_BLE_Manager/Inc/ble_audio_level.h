/**
  ******************************************************************************
  * @file    ble_audio_level.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Audio level info services APIs.
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
#ifndef _BLE_AUDIO_LEVEL_H_
#define _BLE_AUDIO_LEVEL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init audio level info service
  * @param  uint8_t audio_level_number: Number of audio level features (up to 4 audio level are supported)
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for audio level info service
  */
extern ble_char_object_t *ble_init_audio_level_service(uint8_t audio_level_number);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Environmental Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void ble_set_audio_level_advertise_data(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update audio level characteristic values
  * @param  uint16_t *audio_level_data:    SNR dB audio level array
  * @param  uint8_t audio_level_number:    Number of audio level features (up to 4 audio level are supported)
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_audio_level_update(uint16_t *audio_level_data, uint8_t audio_level_number);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_audio_level(ble_notify_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_AUDIO_LEVEL_H_ */

