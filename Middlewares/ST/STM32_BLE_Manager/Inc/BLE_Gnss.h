/**
  ******************************************************************************
  * @file    ble_gnss.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Gnss info services APIs.
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
#ifndef _BLE_GNSS_H_
#define _BLE_GNSS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init gnss info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for gnss info service
  */
extern ble_char_object_t *ble_init_gnss_service(void);

/**
  * @brief  Update Gnss characteristic
  * @param  int32_t  latitude    Gnss latitude
  * @param  int32_t  longitude   Gnss longitude
  * @param  uint32_t altitude    Gnss altitude
  * @param  uint8_t  number_sat  Number of satellites in view
  * @param  uint8_t  sig_quality Signal quality indicator
  * @retval ble_status_t   Status
  */
extern ble_status_t ble_gnss_update(int32_t latitude, int32_t longitude, uint32_t altitude, uint8_t number_sat,
                                    uint8_t sig_quality);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_gnss(ble_notify_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_GNSS_H_ */

