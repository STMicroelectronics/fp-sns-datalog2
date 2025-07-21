/**
  ******************************************************************************
  * @file    ble_time_domain.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add BLE Time Domain info services using vendor specific profiles.
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
#define COPY_TIME_DOMAIN_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x06,0x00,0x02,\
                                                              0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Time Domain info service */
static ble_char_object_t ble_char_time_domain;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_time_domain(ble_notify_event_t event);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_time_domain(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                         uint8_t data_length,
                                         uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Time Domain info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Time Domain info service
  */
ble_char_object_t *ble_init_time_domain_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Init data structure pointer for Time Domain info service */
  ble_char_pointer = &ble_char_time_domain;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_time_domain;
  COPY_TIME_DOMAIN_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = 2 + 18;
  ble_char_pointer->char_properties = CHAR_PROP_NOTIFY;
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 0;

  BLE_MANAGER_PRINTF("BLE Time Domain features ok\r\n");

  return ble_char_pointer;
}

/*
 * @brief  Update Time Domain characteristic value
 * @param  ble_manager_time_domain_generic_value_t peak_value
 * @param  ble_manager_time_domain_generic_value_t speed_rms_value
 * @retval ble_status_t   Status
 */
ble_status_t ble_time_domain_update(ble_manager_time_domain_generic_value_t peak_value,
                                    ble_manager_time_domain_generic_value_t speed_rms_value)
{
  ble_status_t ret;
  uint16_t temp;

  uint8_t buff[2 + 18];
  uint8_t buff_pos;

  float temp_float;
  uint8_t *temp_buff = (uint8_t *) & temp_float;

  float temp_result;

  STORE_LE_16(buff, (HAL_GetTick() / 10U));

  /* peak_value.axes_x * 100 --> for sending 2 bytes */
  temp_result = peak_value.axes_x * ((float)100);
  temp = (uint16_t)temp_result;
  STORE_LE_16(buff + 2, temp);

  /* peak_value.axes_y * 100 --> for sending 2 bytes */
  temp_result = peak_value.axes_y * ((float)100);
  temp = (uint16_t)temp_result;
  STORE_LE_16(buff + 4, temp);

  /* peak_value.axes_z * 100 --> for sending 2 bytes */
  temp_result = peak_value.axes_z * ((float)100);
  temp = (uint16_t)temp_result;
  STORE_LE_16(buff + 6, temp);

  buff_pos = 8;

  /* speed_rms_value.axes_x * 1000 --> Converts from m/s to mm/s */
  temp_float = speed_rms_value.axes_x * ((float)1000);
  buff[buff_pos] = temp_buff[0];
  buff_pos++;
  buff[buff_pos] = temp_buff[1];
  buff_pos++;
  buff[buff_pos] = temp_buff[2];
  buff_pos++;
  buff[buff_pos] = temp_buff[3];
  buff_pos++;

  /* speed_rms_value.axes_y * 1000 --> Converts from m/s to mm/s */
  temp_float = speed_rms_value.axes_y * ((float)1000);
  buff[buff_pos] = temp_buff[0];
  buff_pos++;
  buff[buff_pos] = temp_buff[1];
  buff_pos++;
  buff[buff_pos] = temp_buff[2];
  buff_pos++;
  buff[buff_pos] = temp_buff[3];
  buff_pos++;

  /* speed_rms_value.axes_z * 1000 --> Converts from m/s to mm/s */
  temp_float = speed_rms_value.axes_z * ((float)1000);
  buff[buff_pos] = temp_buff[0];
  buff_pos++;
  buff[buff_pos] = temp_buff[1];
  buff_pos++;
  buff[buff_pos] = temp_buff[2];
  buff_pos++;
  buff[buff_pos] = temp_buff[3];
  buff_pos++;

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_time_domain, 0, 20, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ret != (ble_status_t)BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      if (ble_std_err_service == BLE_SERV_ENABLE)
      {
        bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating Time Domain Char\n");
        std_err_update(buffer_to_write, bytes_to_write);
      }
      else
      {
        BLE_MANAGER_PRINTF("Error Updating Time Domain Char ret=%x\r\n", ret);
      }
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Time Domain Char ret=%x\r\n", ret);
    }
  }

  return ret;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Time Domain is subscribed or not to the one service
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
static void attr_mod_request_time_domain(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                         uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_time_domain(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_time_domain(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write,
                                      "--->TimeDomain=%s\n",
                                      (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->TimeDomain=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t event Sub/Unsub
  * @retval None
  */
__weak void notify_event_time_domain(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify time domain function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_time_domain could be implemented in the user file
   */
}
