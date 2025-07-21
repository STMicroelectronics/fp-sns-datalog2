/**
  ******************************************************************************
  * @file    ble_audio_scene_classification.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Audio Scene Classification info service APIs.
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
#ifndef _BLE_AUDIO_SCENE_CALSSIFICATION_H_
#define _BLE_AUDIO_SCENE_CALSSIFICATION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  BLE_ASC_HOME      = 0x00,
  BLE_ASC_OUTDOOR   = 0x01,
  BLE_ASC_CAR       = 0x02,
  BLE_ASC_OFF       = 0xF0, /* Off */
  BLE_ASC_ON        = 0xF1, /* On */
  BLE_ASC_UNDEFINED = 0xFF
} ble_asc_output_t;

typedef enum
{
  BLE_ASC_ALG_SCENE_CLASS = 0x00,
  BLE_ASC_ALG_BABY_CRYING = 0x01,
  BLE_ASC_ALG_UNDEFINED = 0xFF
} ble_asc_algorithm_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Audio Scene Classification info service
  * @param  uint8_t send_algorithm_code 0/1 -> Send also the Audio Scene Classificatio algorithm code
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Activity Classification info service
  */
extern ble_char_object_t *ble_init_audio_scene_class_service(uint8_t send_algorithm_code);

/**
  * @brief  Update Audio Scene Classification characteristic
  * @param  ble_asc_output_t asc_code Audio Scene Classification Code
  * @param ble_asc_algorithm_t asc_alg_id Algorithm Id
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_audio_scene_class_update(ble_asc_output_t asc_code, ble_asc_algorithm_t asc_alg_id);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_audio_scene_classification(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
extern void read_request_audio_scene_classification_function(ble_asc_output_t *asc_code,
                                                             ble_asc_algorithm_t *asc_alg_id);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_AUDIO_SCENE_CALSSIFICATION_H_ */

