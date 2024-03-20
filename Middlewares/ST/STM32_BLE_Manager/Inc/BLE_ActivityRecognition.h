/**
  ******************************************************************************
  * @file    BLE_ActivityRecognition.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Activity Recognition info service APIs.
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
#ifndef _BLE_ACTIVITY_RECOGNITION_H_
#define _BLE_ACTIVITY_RECOGNITION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  BLE_AR_NOACTIVITY          = 0x00,
  BLE_AR_STATIONARY          = 0x01,
  BLE_AR_WALKING             = 0x02,
  BLE_AR_FASTWALKING         = 0x03,
  BLE_AR_JOGGING             = 0x04,
  BLE_AR_BIKING              = 0x05,
  BLE_AR_DRIVING             = 0x06,
  BLE_AR_STAIRS              = 0x07,
  BLE_AR_ERROR               = 0x08
} BLE_AR_output_t;

typedef enum
{
  HAR_ALGO_IDX_NONE           = 0xFF,
  HAR_GMP_IDX                 = 0x00,
  HAR_IGN_IDX                 = 0x01,
  HAR_IGN_WSDM_IDX            = 0x02,
  HAR_MLC_LSM6DSOX_ID         = 0x03,
  HAR_ALGO_IDX_NUMBER         = 0x04
} BLE_AR_algoIdx_t;

typedef void(*CustomReadRequestActRec_t)(BLE_AR_output_t *ActivityCode, BLE_AR_algoIdx_t *Algorithm);
typedef void (*CustomNotifyEventActRec_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomReadRequestActRec_t CustomReadRequestActRec;
extern CustomNotifyEventActRec_t CustomNotifyEventActRec;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Activity Recognition info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Activity Recognition info service
  */
extern BleCharTypeDef *BLE_InitActRecService(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Activity Recognition Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void BLE_SetActRecAdvertiseData(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Activity Recognition characteristic
  * @param  BLE_AR_output_t ActivityCode Activity Recognized
  * @param  BLE_AR_algoIdx_t Algorithm Code
  * @retval tBleStatus   Status
  */
extern tBleStatus BLE_ActRecUpdate(BLE_AR_output_t ActivityCode, BLE_AR_algoIdx_t Algorithm);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_ACTIVITY_RECOGNITION_H_ */

