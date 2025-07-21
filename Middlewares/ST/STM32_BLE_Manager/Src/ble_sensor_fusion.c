/**
  ******************************************************************************
  * @file    ble_sensor_fusion.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add Sensor Fusion service using vendor specific profiles.
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
#define COPY_SENSOR_FUSION_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x01,0x00,0x00,0x01,\
                                                                0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define SENSOR_FUSION_ADVERTISE_DATA_POSITION  17

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Sensor Fusion service */
static ble_char_object_t ble_char_sensor_fusion;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_sensor_fusion(ble_notify_event_t event);

/* Private functions prototypes ----------------------------------------------*/
static void attr_mod_request_sensor_fusion(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                           uint8_t data_length, uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Sensor Fusion info service
  * @param  uint8_t number_quaternions_to_send  Number of quaternions send (1,2,3)
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Sensor Fusion info service
  */
ble_char_object_t *ble_init_sensor_fusion_service(uint8_t number_quaternions_to_send)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  uint8_t DataLenght = 2U + (6U * number_quaternions_to_send);

  if ((number_quaternions_to_send > 0U) && (number_quaternions_to_send <= 3U))
  {
    /* Init data structure pointer for Sensor Fusion info service */
    ble_char_pointer = &ble_char_sensor_fusion;
    memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
    ble_char_pointer->attr_mod_request_cb = attr_mod_request_sensor_fusion;
    COPY_SENSOR_FUSION_CHAR_UUID((ble_char_pointer->uuid));
    ble_char_pointer->char_uuid_type = UUID_TYPE_128;
    ble_char_pointer->char_value_length = DataLenght;
    ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY);
    ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
    ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
    ble_char_pointer->enc_key_size = 16;
    ble_char_pointer->is_variable = 1;

    BLE_MANAGER_PRINTF("BLE Sensor Fusion features ok\r\n");
  }
  else
  {
    BLE_MANAGER_PRINTF("BLE Sensor Fusion features init error: Number Quaternions To Send not correct\r\n");
    ble_char_pointer = NULL;
  }

  return ble_char_pointer;
}

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Sensor Fusion Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
void ble_set_sensor_fusion_advertise_data(uint8_t *manuf_data)
{
  manuf_data[SENSOR_FUSION_ADVERTISE_DATA_POSITION] |= 0x01U;
}
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update quaternions characteristic value
  * @param  ble_motion_sensor_axes_t *data Structure containing the quaterions
  * @param  uint8_t number_quaternions_to_send  Number of quaternions send (1,2,3)
  * @retval ble_status_t      Status
  */
ble_status_t ble_sensor_fusion_update(ble_motion_sensor_axes_t *data, uint8_t number_quaternions_to_send)
{
  ble_status_t ret;
  uint8_t dim_byte;

  uint8_t buff[2U + (6U * 3U)];

  STORE_LE_16(buff, (HAL_GetTick() / 10U));

  switch (number_quaternions_to_send)
  {
    case 1:
      STORE_LE_16(buff + 2, data[0].axis_x);
      STORE_LE_16(buff + 4, data[0].axis_y);
      STORE_LE_16(buff + 6, data[0].axis_z);
      break;
    case 2:
      STORE_LE_16(buff + 2, data[0].axis_x);
      STORE_LE_16(buff + 4, data[0].axis_y);
      STORE_LE_16(buff + 6, data[0].axis_z);
      STORE_LE_16(buff + 8, data[1].axis_x);
      STORE_LE_16(buff + 10, data[1].axis_y);
      STORE_LE_16(buff + 12, data[1].axis_z);
      break;
    case 3:
      STORE_LE_16(buff + 2, data[0].axis_x);
      STORE_LE_16(buff + 4, data[0].axis_y);
      STORE_LE_16(buff + 6, data[0].axis_z);
      STORE_LE_16(buff + 8, data[1].axis_x);
      STORE_LE_16(buff + 10, data[1].axis_y);
      STORE_LE_16(buff + 12, data[1].axis_z);
      STORE_LE_16(buff + 14, data[2].axis_x);
      STORE_LE_16(buff + 16, data[2].axis_y);
      STORE_LE_16(buff + 18, data[2].axis_z);
      break;
  }

  dim_byte = 2U + (6U * number_quaternions_to_send);
  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_sensor_fusion, 0, dim_byte, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating Sensor Fusion Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Sensor Fusion Char\r\n");
    }
  }
  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Sensor Fusion is subscribed or not to the one service
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
static void attr_mod_request_sensor_fusion(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                           uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_sensor_fusion(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_sensor_fusion(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write,
                                      "--->Sensor Fusion=%s\n",
                                      (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->Sensor Fusion=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  BLE_NotifyEvent_t Event Sub/Unsub
  * @retval None
  */
__weak void notify_event_sensor_fusion(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  BLE_MANAGER_PRINTF("\r\nNotify sensor fusion function not defined (It is a weak function)\r\n");
  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_sensor_fusion could be implemented in the user file
   */
}
