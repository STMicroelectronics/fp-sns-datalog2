/**
  ******************************************************************************
  * @file    ble_pn_p_like.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   ble_pn_p_like info services APIs.
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
#ifndef _BLE_PNPLIKE_H_
#define _BLE_PNPLIKE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Defines ----------------------------------------------------------*/
#ifndef DEFAULT_MAX_PNPL_NOTIFICATION_CHAR_LEN
#define DEFAULT_MAX_PNPL_NOTIFICATION_CHAR_LEN  20
#endif /* DEFAULT_MAX_PNPL_NOTIFICATION_CHAR_LEN */

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init PnPLike info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for PnPLike info service
  */
extern ble_char_object_t *ble_init_pn_p_like_service(void);

/**
  * @brief  PnPLike send buffer
  * @param  uint8_t* buffer
  * @param  uint32_t len
  * @retval ble_status_t Status
  */
extern ble_status_t ble_pn_p_like_update(uint8_t *buffer, uint8_t len);


/**
  * @brief  PnPLike Set Max Char Length
  * @param  uint16_t max_char_length
  * @retval none
  */
extern void ble_pn_p_like_set_max_char_length(uint16_t max_char_length);

/**
  * @brief  PnPLike Get Max Char Length
  * @param  None
  * @retval uint16_t max_char_length
  */
extern uint16_t ble_pn_p_like_get_max_char_length(void);

/**
  * @brief  PnPLike Reset Status
  * @param  None
  * @retval None
  */
extern void ble_pn_p_like_reset(void);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
extern void notify_event_pn_p_like(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
extern void write_request_pn_p_like_function(uint8_t *received_msg, uint32_t msg_length);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_PNPLIKE_H_ */

