/**
  ******************************************************************************
  * @file    ble_ota.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Ota characteristic APIs.
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
#ifndef _BLE_OTA_H_
#define _BLE_OTA_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BLUE_WB

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init OTA characteristic
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for ota characteristic
  */
extern ble_char_object_t *ble_init_ota_service(void);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
extern void write_request_ota_function(uint8_t *att_data, uint8_t data_length);

#else /* BLUE_WB */
#error "This Feature is only for WB BLE Chip"
#endif /* BLUE_WB */

#ifdef __cplusplus
}
#endif

#endif /* _BLE_GNSS_H_ */

