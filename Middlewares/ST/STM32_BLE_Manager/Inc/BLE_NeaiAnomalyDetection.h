/**
  ******************************************************************************
  * @file    BLE_NeaiAnomalyDetection.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   NEAI(Cartesiam) Anomaly Detection info services APIs.
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
#ifndef _BLE_NEAI_ANOMALY_DETECTION_H_
#define _BLE_NEAI_ANOMALY_DETECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Defines ---------------------------------------------------------*/
#define NEAI_AD_ESCAPE 0xFF

typedef enum
{
  NEAI_AD_STATUS_NORMAL  = 0x00,
  NEAI_AD_STATUS_ANOMALY = 0x01,
  NEAI_AD_STATUS_NULL    = 0xFF
} BLE_AD_status_t;

typedef enum
{
  NEAI_AD_PHASE_IDLE          = 0x00,
  NEAI_AD_PHASE_LEARNING      = 0x01,
  NEAI_AD_PHASE_DETECTING     = 0x02,
  NEAI_AD_PHASE_IDLE_TRAINED  = 0x03,
  NEAI_AD_PHASE_BUSY          = 0x04,
  NEAI_AD_PHASE_NULL          = 0xFF
} BLE_AD_phase_t;

typedef enum
{
  NEAI_AD_STATE_OK              = 0x00,
  NEAI_AD_STATE_NOINIT          = 0x7B,
  NEAI_AD_STATE_BOARD_ERROR     = 0x7C,
  NEAI_AD_STATE_BUFFER_ERROR    = 0x7D,
  NEAI_NOT_STATE_ENOUGH_CALL    = 0x7E,
  NEAI_AD_STATE_LEARNING_DONE   = 0x7F,
  NEAI_AD_STATE_UNKNOWN         = 0x80,
  NEAI_AD_STATE_NULL            = 0xFF
} BLE_AD_state_t;

typedef struct
{
  BLE_AD_phase_t phase;
  BLE_AD_state_t state;
  uint8_t progress;
  uint8_t status;
  uint8_t similarity;
} BLE_AD_output_t;

typedef enum
{
  NEAI_AD_STOP    = 0x00,
  NEAI_AD_LEARN   = 0x01,
  NEAI_AD_DETECT  = 0x02,
  NEAI_AD_RESET   = 0xFF
} BLE_AD_command_t;


typedef void (*CustomNotifyEventNeaiAnomalyDetection_t)(BLE_NotifyEvent_t Event);
typedef void (*CustomWriteRequestAnomalyDetection_t)(uint8_t *att_data, uint8_t data_length);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventNeaiAnomalyDetection_t CustomNotifyEventAD;
extern CustomWriteRequestAnomalyDetection_t CustomWriteRequestAD;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init NeaiAnomalyDetection info service
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for environmental info service
  */
extern BleCharTypeDef *BLE_InitADService(void);

/**
  * @brief  Update NeaiAnomalyDetection characteristic value
  * @param  uint8_t phase:        0 when AI lib is stopped, 1 when learning occurred, 2 when detection occurred
  * @param  uint8_t state:        library result of current phase
  * @param  uint8_t completion_percentage:
  * @param  BLE_AD_output_t output:
  * @param  uint8_t similarity:
  * @retval tBleStatus:           Status
  */
extern tBleStatus BLE_NeaiAnomalyDetectionUpdate(BLE_AD_output_t output);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_NEAI_ANOMALY_DETECTION_H_ */
