/**
  ******************************************************************************
  * @file    ble_finite_state_machine.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Finite State Machine info services APIs.
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
#ifndef _BLE_FINITE_STATE_MACHINE_H_
#define _BLE_FINITE_STATE_MACHINE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef void (*custom_notify_event_finite_state_machine_t)(ble_notify_event_t event);
typedef void (*custom_read_finite_state_machine_t)(uint8_t *fsm_out,
                                                   uint8_t *fsm_status_a_mainpage,
                                                   uint8_t *fsm_status_b_mainpage);

/* Enum type Number Finite State Machine Registers */
typedef enum
{
  BLE_FSM_8_REG  = 0, /* 8 FSM Registers */
  BLE_FSM_16_REG = 1  /* 16 FSM Registers */
} ble_finite_state_machine_num_reg_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Machine Learning Core info service
  * @param  ble_finite_state_machine_num_reg_t num_reg Number of Registers
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Machine Learning Core info service
  */
extern ble_char_object_t *ble_init_finite_state_machine_service(ble_finite_state_machine_num_reg_t num_reg);

/**
  * @brief  Update Machine Learning Core output registers characteristic
  * @param  uint8_t *fsm_out pointers to 16 FSM register
  * @param  uint8_t *fsm_status_a_mainpage pointer to the FSM status bits from 1 to 8
  * @param  uint8_t *fsm_status_a_mainpage pointer to the FSM status bits from 9 to 16
  * @retval ble_status_t Status
  */
extern ble_status_t ble_finite_state_machine_update(uint8_t *fsm_out, uint8_t *fsm_status_a_mainpage,
                                                    uint8_t *fsm_status_b_mainpage);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_finite_state_machine(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
extern void read_request_finite_state_machine_function(uint8_t *fsm_out,
                                                       uint8_t *fsm_status_a_mainpage,
                                                       uint8_t *fsm_status_b_mainpage);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_FINITE_STATE_MACHINE_H_ */

