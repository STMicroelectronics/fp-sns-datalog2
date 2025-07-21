/**
  ******************************************************************************
  * @file    ble_objects_detection.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add Objects Detection info services using vendor specific profiles.
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
#define COPY_TOF_MOBJ_CHAR_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x13,\
                                                            0x00,0x02,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Objects Detection info service */
static ble_char_object_t ble_char_objects_detection;
static uint8_t tof_enable_human_presence = 0;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_objects_detection(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
__weak void write_request_objects_detection_function(void);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_objects_detection(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                               uint8_t data_length, uint8_t *att_data);
static void write_request_objects_detection(void *void_char_pointer, uint16_t handle, uint16_t offset,
                                            uint8_t data_length,
                                            uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Objects Detection info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Objects Detection info service
  */
ble_char_object_t *ble_init_objects_detection_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Init data structure pointer for Objects Detection info service */
  ble_char_pointer = &ble_char_objects_detection;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_objects_detection;
  ble_char_pointer->write_request_cb = write_request_objects_detection;
  COPY_TOF_MOBJ_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = 2 + (2 * 4) + 1;
  ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY) |
                                      ((uint8_t)CHAR_PROP_WRITE_WITHOUT_RESP) |
                                      ((uint8_t)CHAR_PROP_WRITE);
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = ((uint8_t)GATT_NOTIFY_ATTRIBUTE_WRITE) |
                                    ((uint8_t)GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP);
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 1;

  BLE_MANAGER_PRINTF("BLE Objects Detection features ok\r\n");

  return ble_char_pointer;
}

/**
  * @brief  Update number of the objects and related distance value detection (by ToF sensor)
  * @param  uint16_t distances Distance of the detected objects
  * @param  uint8_t  human_presence Detected object inside fixed distance range
  * @retval ble_status_t   Status
  */
ble_status_t ble_objects_detection_status_update(uint16_t *distances, uint8_t human_presence)
{
  ble_status_t ret;

  uint8_t buff[2 + (2 * 4) + 1];

  /* for TimeStamp */
  uint8_t byte_counter = 2;
  int32_t number;

  STORE_LE_16(buff, (HAL_GetTick() / 10U));

  if (tof_enable_human_presence != 0U)
  {
    buff[byte_counter] = human_presence;
    byte_counter++;
  }
  else
  {
    for (number = 0; number < 4; number++)
    {
      if (distances[number] != (uint16_t)0)
      {
        STORE_LE_16(buff + byte_counter, distances[number]);
        byte_counter += 2U;
      }
    }
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_objects_detection, 0U, byte_counter, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating Objects Detection Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Objects Detection Char\r\n");
    }
  }
  return ret;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Objects Detection is subscribed or not to the one service
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
static void attr_mod_request_objects_detection(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                               uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_objects_detection(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_objects_detection(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write,
                                      "--->ObjectsDetection=%s\n",
                                      (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->ObjectsDetection=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
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
static void write_request_objects_detection(void *void_char_pointer, uint16_t handle, uint16_t offset,
                                            uint8_t data_length,
                                            uint8_t *att_data)
{
  if (att_data[0] != 0U)
  {
    tof_enable_human_presence = 1U;
  }
  else
  {
    tof_enable_human_presence = 0U;
  }

  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t) sprintf((char *)buffer_to_write, "Writing on T-o-F MObj Presence= %x\r\n", att_data[0]);
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("Writing on T-o-F MObj Presence= %x\r\n", att_data[0]);
  }

  write_request_objects_detection_function();
}

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t Event Sub/Unsub
  * @retval None
  */
__weak void notify_event_objects_detection(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify objects detection function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_objects_detection could be implemented in the user file
   */
}

/********************************************************
  * Callback function to manage the write request events *
  *******************************************************/
/**
  * @brief  Callback Function for objects detection write request.
  * @param  None
  * @retval None
  */
__weak void write_request_objects_detection_function(void)
{

  BLE_MANAGER_PRINTF("\r\nWrite request objects detection function not defined (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the write_request_objects_detection_function could be implemented in the user file
   */
}
