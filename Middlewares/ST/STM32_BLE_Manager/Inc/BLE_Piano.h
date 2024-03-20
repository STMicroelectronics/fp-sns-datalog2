/**
  ******************************************************************************
  * @file    BLE_Piano.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   BLE_Pianoinfo services APIs.
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
#ifndef _BLE_PIANO_H_
#define _BLE_PIANO_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomWriteRequestPiano_t)(uint8_t *att_data, uint8_t data_length);
typedef void (*CustomNotifyEventPiano_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventPiano_t CustomNotifyEventPiano;
extern CustomWriteRequestPiano_t CustomWriteRequestPiano;

/* Exported defines --------------------------------------------------------- */
#define BLE_PIANO_START 1
#define BLE_PIANO_STOP 0

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Piano info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for High Speed Data Log info service
  */
extern BleCharTypeDef *BLE_InitPianoService(void);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_PIANO_H_ */

