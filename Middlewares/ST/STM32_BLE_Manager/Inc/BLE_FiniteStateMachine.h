/**
  ******************************************************************************
  * @file    BLE_FiniteStateMachine.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Finite State Machine info services APIs.
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
#ifndef _BLE_FINITE_STATE_MACHINE_H_
#define _BLE_FINITE_STATE_MACHINE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomNotifyEventFiniteStateMachine_t)(BLE_NotifyEvent_t Event);
typedef void (*CustomReadFiniteStateMachine_t)(uint8_t *fsm_out,
                                               uint8_t *fsm_status_a_mainpage,
                                               uint8_t *fsm_status_b_mainpage);

/* Enum type Number Finite State Machine Registers */
typedef enum
{
  BLE_FSM_8_REG  = 0, /* 8 FSM Registers */
  BLE_FSM_16_REG = 1  /* 16 FSM Registers */
} BLE_FiniteStateMachineNumReg_t;

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventFiniteStateMachine_t CustomNotifyEventFiniteStateMachine;
extern CustomReadFiniteStateMachine_t CustomReadFiniteStateMachine;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Machine Learning Core info service
  * @param  BLE_FiniteStateMachineNumReg_t NumReg Number of Registers
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Machine Learning Core info service
  */
BleCharTypeDef *BLE_InitFiniteStateMachineService(BLE_FiniteStateMachineNumReg_t NumReg);

/**
  * @brief  Update Machine Learning Core output registers characteristic
  * @param  uint8_t *fsm_out pointers to 16 FSM register
  * @param  uint8_t *fsm_status_a_mainpage pointer to the FSM status bits from 1 to 8
  * @param  uint8_t *fsm_status_a_mainpage pointer to the FSM status bits from 9 to 16
  * @retval tBleStatus Status
  */
tBleStatus BLE_FiniteStateMachineUpdate(uint8_t *fsm_out, uint8_t *fsm_status_a_mainpage,
                                        uint8_t *fsm_status_b_mainpage);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_FINITE_STATE_MACHINE_H_ */

