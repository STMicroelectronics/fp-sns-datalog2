/**
  ******************************************************************************
  * @file    ble_qvar.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   QVAR info services APIs.
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
#ifndef _BLE_QVAR_H_
#define _BLE_QVAR_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init QVAR info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for QVAR info service
  */
extern ble_char_object_t *ble_init_qvar_service(void);

/**
  * @brief  Update The QVAR Char
  * @param  int32_t qvar Value [LSB]
  * @param  uint8_t flag (to be defined)
  * @param  int32_t dqvar Value [LSB]
  * @param  uint32_t Parameter (to be defined)
  * @param  int32_t number_element Number of elements to send
  * @retval ble_status_t      Status
  */
extern ble_status_t ble_qvar_update(int32_t qvar, uint8_t flag, int32_t dqvar, uint32_t param, int32_t number_element);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_qvar(ble_notify_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_QVAR_H_ */

