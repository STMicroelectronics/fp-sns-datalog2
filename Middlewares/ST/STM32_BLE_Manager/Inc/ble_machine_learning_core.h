/**
  ******************************************************************************
  * @file    ble_machine_learning_core.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Machine Learning Core info services APIs.
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
#ifndef _BLE_MACHINE_LEARNING_CORE_H_
#define _BLE_MACHINE_LEARNING_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Enum type Number Machine Learning Core Registers */
typedef enum
{
  BLE_MLC_4_REG = 0, /* 4 MLC Registers */
  BLE_MLC_8_REG = 1  /* 8 MLC Registers */
} ble_machine_learning_core_num_reg_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Machine Learning Core info service
  * @param  ble_machine_learning_core_num_reg_t num_reg Number MLC registers
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Machine Learning Core info service
  */
extern ble_char_object_t *ble_init_machine_learning_core_service(ble_machine_learning_core_num_reg_t num_reg);

/**
  * @brief  Update Machine Learning Core output registers characteristic
  * @param  uint8_t *mlc_out pointers to 8 MLC register
  * @param  uint8_t *mlc_status_mainpage pointer to the MLC status bits from 1 to 8
  * @retval ble_status_t Status
  */
extern ble_status_t ble_machine_learning_core_update(uint8_t *mlc_out, uint8_t *mlc_status_mainpage);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_machine_learning_core(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
extern void read_request_machine_learning_core_function(uint8_t *mlc_out, uint8_t *mlc_status_mainpage);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MACHINE_LEARNING_CORE_H_ */

