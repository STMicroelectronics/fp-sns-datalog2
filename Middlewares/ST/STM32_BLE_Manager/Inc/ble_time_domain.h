/**
  ******************************************************************************
  * @file    ble_time_domain.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Time Domain info services APIs.
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
#ifndef _BLE_TIME_DOMAIN_H_
#define _BLE_TIME_DOMAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Types ----------------------------------------------------------- */
/**
  * @brief  X-Y-Z Generic Value in float
  */
typedef struct
{
  float axes_x;         /* !< Generic X Value in float */
  float axes_y;         /* !< Generic Y Value in float */
  float axes_z;         /* !< Generic Z Value in float */
} ble_manager_time_domain_generic_value_t;

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Time Domain info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Time Domain info service
  */
extern ble_char_object_t *ble_init_time_domain_service(void);

/*
 * @brief  Update Time Domain characteristic value
 * @param  ble_manager_time_domain_generic_value_t peak_value
 * @param  ble_manager_time_domain_generic_value_t speed_rms_value
 * @retval ble_status_t   Status
 */
extern ble_status_t ble_time_domain_update(ble_manager_time_domain_generic_value_t peak_value,
                                           ble_manager_time_domain_generic_value_t speed_rms_value);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_time_domain(ble_notify_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_TIME_DOMAIN_H_ */

