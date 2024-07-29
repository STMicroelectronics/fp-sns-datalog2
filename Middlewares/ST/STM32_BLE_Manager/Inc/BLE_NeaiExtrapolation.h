/**
  ******************************************************************************
  * @file    BLE_NeaiExtrapolation.h
  * @author  System Research & Applications Team - System LAB DU.
  * @version 1.0.0
  * @date    2-May-2024
  * @brief   NEAI Extrapolation info services APIs.
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

/**
  *  How NEAI Extrapolation characteristic works?
  *
  *  This characteristic is based on JSON messages exchange.
  *  All exchanged messages are made up of an object which contains the following field:
  *
  *  - Phase (ENUM), it can be equals to IDLE(0), EXTRAPOLATION(1), BUSY(2), it is THE ONE WHICH IS MANDATORY
  *  - State (ENUM), it can be equals to STATE_OK(0), NOINIT(1), BOARD_ERROR(2), ...
  *  - Target (FLOAT), it expresses the estimated target value
  *  - Measurement Unit (STRING), it specifies the measurement unit and it accepts up to 5 character
  *  - TimeStamp (UINT32), SysTick is used as timestamp
  *
  *  At the end of the day, only the phase field is mandatory, the others fields are optional.
  *
  */

  /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BLE_NEAI_EXTRAPOLATION_H_
#define _BLE_NEAI_EXTRAPOLATION_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

typedef enum
{
  NEAI_EXT_PHASE_IDLE            = 0x00,
  NEAI_EXT_PHASE_EXTRAPOLATION   = 0x01,
  NEAI_EXT_PHASE_BUSY            = 0x02
} BLE_EXT_phase_t;

typedef enum
{
  NEAI_EXT_STATE_OK                         = 0x00,
  NEAI_EXT_STATE_NOINIT                     = 0x7B,
  NEAI_EXT_STATE_BOARD_ERROR                = 0x7C,
  NEAI_EXT_STATE_BUFFER_ERROR               = 0x7D,
  NEAI_EXT_STATE_ENOUGH_CALL                = 0x7E,
  NEAI_EXT_STATE_RECOMMENDED_LEARNING_DONE  = 0x7F,
  NEAI_EXT_STATE_UNKNOWN                    = 0x80
} BLE_EXT_state_t;

typedef struct
{
  BLE_EXT_phase_t phase;
  BLE_EXT_state_t state;
  float extrapolated_value;
  char *measurement_unit;
} BLE_E_output_t;

typedef enum
{
  NEAI_EXT_COMMAND_STOP         = 0x00,
  NEAI_EXT_COMMAND_EXTRAPOLATE  = 0x01
} BLE_EXT_command_t;


typedef void (*CustomNotifyEventNeaiExtrapolation_t)(BLE_NotifyEvent_t Event);
typedef void (*CustomWriteRequestExtrapolation_t)(uint8_t * att_data, uint8_t data_length);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventNeaiExtrapolation_t CustomNotifyEventE;
extern CustomWriteRequestExtrapolation_t CustomWriteRequestE;

/* Exported functions ------------------------------------------------------- */

/**
 * @brief  Init extrapolation info service
 * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for extrapolation info service
 */
extern BleCharTypeDef* BLE_InitNeaiExtrapolationService(void);

/**
 * @brief  Update extrapolation characteristic value according to extrapolation library output
 * @param  BLE_E_output_t    output
 * @retval tBleStatus        status
 */
extern tBleStatus BLE_NeaiExtrapolationUpdate(BLE_E_output_t output);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_NEAI_EXTRAPOLATION_H_ */
