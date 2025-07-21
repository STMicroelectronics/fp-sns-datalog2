/**
  ******************************************************************************
  * @file    ble_time_domain_alarm_speed_rms_status.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add BLE Time Domain Alarm Speed RMS Status info services using vendor
  *          specific profiles.
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
#define COPY_TD_ALARM_SPEED_RMS_STATUS_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x07,0x00,0x02,\
                                                                            0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,\
                                                                            0xc5,0x1b)

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Time Domain Alarm Speed RMS Status info service */
static ble_char_object_t ble_char_td_alarm_speed_rms_status;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_time_domain_alarm_speed_rms_status(ble_notify_event_t event);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_td_alarm_speed_rms_status(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                                       uint8_t data_length, uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Time Domain Alarm Speed RMS Status info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Time Domain Alarm Speed RMS
  *                                              Status info service
  */
ble_char_object_t *ble_init_td_alarm_speed_rms_status_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Init data structure pointer for Time Domain Alarm Speed RMS Status info service */
  ble_char_pointer = &ble_char_td_alarm_speed_rms_status;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_td_alarm_speed_rms_status;
  COPY_TD_ALARM_SPEED_RMS_STATUS_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = 2 + 13;
  ble_char_pointer->char_properties = CHAR_PROP_NOTIFY;
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 0;

  BLE_MANAGER_PRINTF("BLE Time Domain Alarm Speed RMS Status features ok\r\n");

  return ble_char_pointer;
}

/*
 * @brief  Update Time Domain Alarm Speed RMS status value
 * @param  ble_td_alarm_speed_rms_status_t              Alarm
 * @param  ble_td_alarm_speed_rms_status_generic_value_t        speed_rms_value
 * @retval ble_status_t   Status
 */
ble_status_t ble_td_alarm_speed_rms_status_update(ble_td_alarm_speed_rms_status_t Alarm,
                                                  ble_td_alarm_speed_rms_status_generic_value_t speed_rms_value)
{
  ble_status_t ret;

  float temp_float;
  uint32_t *temp_buff = (uint32_t *) & temp_float;

  uint8_t buff[2 + 13];
  uint8_t buff_pos;

  uint8_t alarm_x = (uint8_t)Alarm.status_axis_x;
  uint8_t alarm_y = (uint8_t)Alarm.status_axis_y;
  uint8_t alarm_z = (uint8_t)Alarm.status_axis_z;

  /* Timestamp */
  STORE_LE_16(buff, (HAL_GetTick() / 10U));

  /* Acceleration rms global status */
  buff[2] = (alarm_x << 4) | (alarm_y  << 2) | (alarm_z);
  buff_pos = 3;

  /* RMS speed as mm/s */
  temp_float = speed_rms_value.axis_x;
  STORE_LE_32(&buff[buff_pos], *temp_buff);
  buff_pos += ((uint8_t)4);
  temp_float = speed_rms_value.axis_y;
  STORE_LE_32(&buff[buff_pos], *temp_buff);
  buff_pos += ((uint8_t)4);
  temp_float = speed_rms_value.axis_z;
  STORE_LE_32(&buff[buff_pos], *temp_buff);

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_td_alarm_speed_rms_status, 0, 2 + 13, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ret != (ble_status_t)BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      if (ble_std_err_service == BLE_SERV_ENABLE)
      {
        bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write,
                                          "Error Updating Time Domain Alarm Speed RMS Status Char\n");
        std_err_update(buffer_to_write, bytes_to_write);
      }
      else
      {
        BLE_MANAGER_PRINTF("Error Updating Time Domain Alarm Speed RMS Status Char ret=%x\r\n", ret);
      }
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Time Domain Alarm Speed RMS Status Char ret=%x\r\n", ret);
    }
  }

  return ret;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Time Domain Alarm Speed RMS
  *         Status is subscribed or not to the one service
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
static void attr_mod_request_td_alarm_speed_rms_status(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                                       uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_time_domain_alarm_speed_rms_status(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_time_domain_alarm_speed_rms_status(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write,
                                      "--->TD Alarm Speed RMS Status=%s\n",
                                      (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->TD Alarm Speed RMS Status=%s",
                       (att_data[0] == 01U == BLE_NOTIFY_SUB) ? " ON\r\n" : " OFF\r\n");
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
__weak void notify_event_time_domain_alarm_speed_rms_status(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify time domain alarm speed rms status function not defined (Weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_time_domain_alarm_speed_rms_status could be implemented in the user file
   */
}
