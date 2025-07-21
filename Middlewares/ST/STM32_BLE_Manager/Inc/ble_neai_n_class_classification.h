/**
  ******************************************************************************
  * @file    ble_neai_n_class_classification.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   NEAI Classification info services APIs.
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

/**
  *  How NEAI NCC characteristic works? It depends on CLASSIFICATION TYPE SELECTOR byte
  *
  *  If selector == 1, so you're using 1-Class Classification library
  *
  *  Byte    |    0    |    1    |    2    |    3    |                 4              |    5    |    6    |       7
  *  Field   |      ********  NOT USED  ********     |  CLASSIFICATION TYPE SELECTOR  |  PHASE  |  STATE  |   IS OUTLIER
  *
  *  If selector == 2, so you're using N-Class Classification library
  *
  *  Byte    | 0 | 1 | 2 | 3 |                4             |   5   |   6   |       7             |    8 - 12
  *  Field   |  * NOT USED * | CLASSIFICATION TYPE SELECTOR | PHASE | STATE | MOST PROBABLE CLASS | PROBABILITIES
  *
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BLE_NEAI_CLASSIFICATION_H_
#define _BLE_NEAI_CLASSIFICATION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Defines ---------------------------------------------------------*/
#ifndef CLASS_NUMBER_NCC
#define CLASS_NUMBER_NCC 2U
#endif /* CLASS_NUMBER_NCC */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#define NEAI_NCC_ESCAPE  0xFF

typedef enum
{
  NEAI_NCC_SEL_1CLASS            = 0x01,
  NEAI_NCC_SEL_NCLASS            = 0x02
} ble_ncc_type_sel_t;

typedef enum
{
  NEAI_NCC_PHASE_IDLE            = 0x00,
  NEAI_NCC_PHASE_CLASSIFICATION  = 0x01,
  NEAI_NCC_PHASE_BUSY            = 0x02
} ble_ncc_phase_t;

typedef enum
{
  NEAI_NCC_STATE_OK                         = 0x00,
  NEAI_NCC_STATE_NOINIT                     = 0x7B,
  NEAI_NCC_STATE_BOARD_ERROR                = 0x7C,
  NEAI_NCC_STATE_BUFFER_ERROR               = 0x7D,
  NEAI_NCC_STATE_ENOUGH_CALL                = 0x7E,
  NEAI_NCC_STATE_RECOMMENDED_LEARNING_DONE  = 0x7F,
  NEAI_NCC_STATE_UNKNOWN                    = 0x80
} ble_ncc_state_t;

typedef struct
{
  ble_ncc_type_sel_t sel;
  ble_ncc_phase_t phase;
  ble_ncc_state_t state;
  bool is_outlier;
} ble_1cc_output_t;

typedef struct
{
  ble_ncc_type_sel_t sel;
  ble_ncc_phase_t phase;
  ble_ncc_state_t state;
  uint8_t most_probable_class;
  float *probabilities;
} ble_ncc_output_t;

typedef enum
{
  NEAI_NCC_COMMAND_STOP      = 0x00,
  NEAI_NCC_COMMAND_CLASSIFY  = 0x01
} ble_ncc_command_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init classification info service
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for N-Class Classification info service
  */
extern ble_char_object_t *ble_init_neai_classification_service(void);

/**
  * @brief  Update classification characteristic value with 1-Class library output
  * @param  ble_ncc_output_t    output:
  * @retval ble_status_t:         Status
  */
extern ble_status_t ble_neai_1_class_classification_update(ble_1cc_output_t output);

/**
  * @brief  Update classification characteristic value with N-Class library output
  * @param  ble_ncc_output_t    output:
  * @retval ble_status_t:         Status
  */
extern ble_status_t ble_neai_n_class_classification_update(ble_ncc_output_t output);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_neai_n_class_classification(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
extern void write_request_neai_n_class_classification_function(uint8_t *att_data, uint8_t data_length);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_NEAI_CLASSIFICATION_H_ */
