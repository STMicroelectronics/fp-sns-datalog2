/**
  ******************************************************************************
  * @file    ble_high_speed_data_log.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add High Speed Data Log info services using vendor specific
  *          profiles.
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
#define COPY_HIGH_SPEED_DATA_LOG_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x11,0x00,0x02,0x11,\
                                                                      0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for High Speed Data Log info service */
static ble_char_object_t ble_char_high_speed_data_log;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_high_speed_data_log(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
__weak void write_request_high_speed_data_log_function(uint8_t *att_data, uint8_t data_length);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_high_speed_data_log(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                                 uint8_t data_length, uint8_t *att_data);
static void write_request_high_speed_data_log(void *ble_char_pointer, uint16_t handle, uint16_t offset,
                                              uint8_t data_length,
                                              uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init High Speed Data Log info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for High Speed Data Log info service
  */
ble_char_object_t *ble_init_high_speed_data_log_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Init data structure pointer for High Speed Data Log info service */
  ble_char_pointer = &ble_char_high_speed_data_log;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_high_speed_data_log;
  ble_char_pointer->write_request_cb = write_request_high_speed_data_log;
  COPY_HIGH_SPEED_DATA_LOG_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = 20;
  ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_WRITE_WITHOUT_RESP);
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_ATTRIBUTE_WRITE;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 1;

  BLE_MANAGER_PRINTF("BLE High Speed Data Log features ok\r\n");

  return ble_char_pointer;
}

/**
  * @brief  High Speed Data Log Send Buffer
  * @param  uint8_t* buffer
  * @param  uint32_t len
  * @retval ble_status_t   Status
  */
ble_status_t ble_high_speed_data_log_send_buffer(uint8_t *buffer, uint32_t len)
{
  ble_status_t ret;
  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_high_speed_data_log, 0, (uint8_t)len, buffer);
  return ret;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if High Speed Data Log is subscribed
  *         or not to the one service
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
static void attr_mod_request_high_speed_data_log(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                                 uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_high_speed_data_log(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_high_speed_data_log(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write,
                                      "--->HSDataLog=%s\n",
                                      (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->HSDataLog=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
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
static void write_request_high_speed_data_log(void *ble_char_pointer, uint16_t handle, uint16_t offset,
                                              uint8_t data_length,
                                              uint8_t *att_data)
{
  write_request_high_speed_data_log_function(att_data, data_length);
}

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t Event Sub/Unsub
  * @retval None
  */
__weak void notify_event_high_speed_data_log(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify high speed data log function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_high_speed_data_log could be implemented in the user file
   */
}

/********************************************************
  * Callback function to manage the write request events *
  *******************************************************/
/**
  * @brief  Callback Function for High Speed Data Log write request.
  * @param  uint8_t *att_data
  * @param  uint8_t data_length
  * @retval None
  */
__weak void write_request_high_speed_data_log_function(uint8_t *att_data, uint8_t data_length)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(att_data);
  BLE_UNUSED(data_length);

  BLE_MANAGER_PRINTF("\r\nWrite request high speed data log function not defined (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the write_request_high_speed_data_log_function could be implemented in the user file
   */
}
