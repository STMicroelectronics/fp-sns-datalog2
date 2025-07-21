/**
  ******************************************************************************
  * @file    ble_pn_p_like.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add PnPLike info services using vendor specific profile.
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

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "ble_manager.h"
#include "ble_manager_common.h"

/* Private define ------------------------------------------------------------*/
#define COPY_PNPLIKE_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x1b,\
                                                          0x00,0x02,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for PnPLike info service */
static ble_char_object_t ble_char_pn_p_like;
/* Buffer used to save the complete command received via BLE*/
static uint8_t *ble_command_buffer = NULL;

static uint16_t pn_p_like_content_max_char_length = DEFAULT_MAX_PNPL_NOTIFICATION_CHAR_LEN;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_pn_p_like(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
__weak void write_request_pn_p_like_function(uint8_t *received_msg, uint32_t msg_length);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_pn_p_like(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                       uint8_t data_length,
                                       uint8_t *att_data);

static void write_request_pn_p_like(void *ble_char_pointer, uint16_t handle, uint16_t offset, uint8_t data_length,
                                    uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init PnPLike info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for PnPLike info service
  */
ble_char_object_t *ble_init_pn_p_like_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Init data structure pointer for PnPLike info service */
  ble_char_pointer = &ble_char_pn_p_like;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_pn_p_like;
  ble_char_pointer->write_request_cb = write_request_pn_p_like;
  COPY_PNPLIKE_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = pn_p_like_content_max_char_length;
  ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_WRITE_WITHOUT_RESP);
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_ATTRIBUTE_WRITE;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 1;

  BLE_MANAGER_PRINTF("BLE PnPLike features ok\r\n");

  return ble_char_pointer;
}

/**
  * @brief  PnPLike Send Buffer
  * @param  uint8_t* buffer
  * @param  uint32_t len
  * @retval ble_status_t Status
  */
ble_status_t ble_pn_p_like_update(uint8_t *buffer, uint8_t len)
{
  ble_status_t ret;
  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_pn_p_like, 0, len, buffer);

  return ret;

}

/**
  * @brief  PnPLike Set Max Char Length
  * @param  uint16_t max_char_length
  * @retval none
  */
void ble_pn_p_like_set_max_char_length(uint16_t max_char_length)
{
  pn_p_like_content_max_char_length = max_char_length;
}

/**
  * @brief  PnPLike Get Max Char Length
  * @param  None
  * @retval uint16_t max_char_length
  */
uint16_t ble_pn_p_like_get_max_char_length(void)
{
  return pn_p_like_content_max_char_length;
}

/**
  * @brief  PnPLike Reset Status
  * @param  None
  * @retval None
  */
void ble_pn_p_like_reset(void)
{
  if (ble_command_buffer != NULL)
  {
    BLE_FREE_FUNCTION(ble_command_buffer);
    ble_command_buffer = NULL;
  }
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the GATT attribute
  *         With this function it's possible to understand if PnPLike is subscribed or not to the one service
  * @param  void *void_char_pointer
  * @param  uint16_t attr_handle Handle of the attribute
  * @param  uint16_t offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode:
  *                          - Bits 0-14: offset of the reported value inside the attribute.
  *                          - Bit 15: if the entire value of the attribute does not fit inside a single
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event, this bit is set to 1 to notify that other
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.
  * @param  uint8_t data_length length of the data
  * @param  uint8_t *att_data attribute data
  * @retval None
  */
static void attr_mod_request_pn_p_like(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                       uint8_t data_length,
                                       uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_pn_p_like(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_pn_p_like(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write,
                                      "--->PnPLike=%s\n",
                                      (att_data[0] == BLE_NOTIFY_SUB) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->PnPLike=%s", (att_data[0] == BLE_NOTIFY_SUB) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**
  * @brief  This event is given when a write request is received by the server from the client.
  * @param  void *ble_char_pointer
  * @param  uint16_t handle Handle of the attribute
  * @param  uint16_t offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode:
  *                          - Bits 0-14: offset of the reported value inside the attribute.
  *                          - Bit 15: if the entire value of the attribute does not fit inside a single
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event, this bit is set to 1 to notify that other
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.
  * @param  uint8_t data_length length of the data
  * @param  uint8_t *att_data attribute data
  * @retval None
  */
static void write_request_pn_p_like(void *ble_char_pointer, uint16_t handle, uint16_t offset, uint8_t data_length,
                                    uint8_t *att_data)
{
  uint32_t command_buf_len = 0;

  command_buf_len = ble_command_tp_parse(&ble_command_buffer, att_data, data_length);

  if (command_buf_len > 0U)
  {
    write_request_pn_p_like_function(ble_command_buffer, command_buf_len);

#if (BLE_DEBUG_LEVEL>1)
    BLE_MANAGER_PRINTF("\r\n%.*s\r\n", command_buf_len, ble_command_buffer);
#endif /* (BLE_DEBUG_LEVEL>1) */

    BLE_FREE_FUNCTION(ble_command_buffer);
    ble_command_buffer = NULL;
  }
}

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t Event Sub/Unsub
  * @retval None
  */
__weak void notify_event_pn_p_like(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify PnP like function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_pn_p_like could be implemented in the user file
   */
}

/********************************************************
  * Callback function to manage the write request events *
  *******************************************************/
/**
  * @brief  Callback Function for PnP Like write request.
  * @param  uint8_t *received_msg
  * @param  uint8_t msg_length
  * @retval None
  */
__weak void write_request_pn_p_like_function(uint8_t *received_msg, uint32_t msg_length)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(received_msg);
  BLE_UNUSED(msg_length);

  BLE_MANAGER_PRINTF("\r\nWrite PnP like detection function not defined (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the write_request_pn_p_like_function could be implemented in the user file
   */
}



