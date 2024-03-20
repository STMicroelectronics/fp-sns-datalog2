/**
  ******************************************************************************
  * @file    BLE_AudioSceneClassification.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Audio Scene Classification info service APIs.
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
#ifndef _BLE_AUDIO_SCENE_CALSSIFICATION_H_
#define _BLE_AUDIO_SCENE_CALSSIFICATION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  BLE_ASC_HOME      = 0x00,
  BLE_ASC_OUTDOOR   = 0x01,
  BLE_ASC_CAR       = 0x02,
  BLE_ASC_OFF       = 0xF0, /* Off */
  BLE_ASC_ON        = 0xF1, /* On */
  BLE_ASC_UNDEFINED = 0xFF
} BLE_ASC_output_t;

typedef enum
{
  BLE_ASC_ALG_SCENE_CLASS = 0x00,
  BLE_ASC_ALG_BABY_CRYING = 0x01,
  BLE_ASC_ALG_UNDEFINED = 0xFF
} BLE_ASC_Algorithm_t;

typedef void (*CustomReadRequestAudioSceneClass_t)(BLE_ASC_output_t *ASC_Code, BLE_ASC_Algorithm_t *ASC_AlgId);
typedef void (*CustomNotifyEventAudioSceneClass_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomReadRequestAudioSceneClass_t CustomReadRequestAudioSceneClass;
extern CustomNotifyEventAudioSceneClass_t CustomNotifyEventAudioSceneClass;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Audio Scene Classification info service
  * @param  uint8_t SendAlgorithmCode 0/1 -> Send also the Audio Scene Classificatio algorithm code
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Activity Classification info service
  */
extern BleCharTypeDef *BLE_InitAudioSceneClassService(uint8_t SendAlgorithmCode);

/**
  * @brief  Update Audio Scene Classification characteristic
  * @param  BLE_ASC_output_t ASC_Code Audio Scene Classification Code
  * @param BLE_ASC_Algorithm_t ASC_AlgId Algorithm Id
  * @retval tBleStatus   Status
  */
extern tBleStatus BLE_AudioSceneClassUpdate(BLE_ASC_output_t ASC_Code, BLE_ASC_Algorithm_t ASC_AlgId);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_AUDIO_SCENE_CALSSIFICATION_H_ */

