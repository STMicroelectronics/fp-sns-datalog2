/**
  ******************************************************************************
  * @file    ble_fitness_activities.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Fitness Activities info service APIs.
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
#ifndef _BLE_FITNESS_ACTIVITIES_H_
#define _BLE_FITNESS_ACTIVITIES_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef enum
{
  BLE_MFA_NOACTIVITY          = 0x00,
  BLE_MFA_BICEPCURL           = 0x01,
  BLE_MFA_SQUAT               = 0x02,
  BLE_MFA_PUSHUP              = 0x03
} ble_fitness_activities_type_t;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Fitness Activities info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Fitness Activities info service
  */
extern ble_char_object_t *ble_init_fitness_activities_service(void);

/**
  * @brief  Update Fitness Activities characteristic
  * @param  uint8_t activity
  * @param  uint8_t counter
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_fitness_activities_update(uint8_t activity, uint32_t counter);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_fitness_activities(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
extern void write_request_fitness_activities_function(uint8_t fitness_activities);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_FITNESS_ACTIVITIES_H_ */

