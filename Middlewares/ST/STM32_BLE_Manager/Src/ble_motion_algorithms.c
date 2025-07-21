/**
  ******************************************************************************
  * @file    ble_motion_algorithms.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add Motion Algorithms service using vendor specific profiles.
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
#define COPY_MOTION_ALGORITHM_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x0A,0x00,0x02,0x11,\
                                                                   0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Motion Algorithms service */
static ble_char_object_t ble_char_motion_algorithms;

static ble_motion_algorithms_type_t MotionAlgorithmSelected = BLE_MOTION_ALGORITHMS_NO_ALGO;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_motion_algorithms(ble_notify_event_t event,
                                           ble_motion_algorithms_type_t algorithm);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
__weak void write_request_motion_algorithms_function(ble_motion_algorithms_type_t algorithm);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_motion_algorithms(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                               uint8_t data_length, uint8_t *att_data);
static void write_request_motion_algorithms(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                            uint8_t data_length, uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Motion Algorithms service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer forMotion Algorithms service
  */
ble_char_object_t *ble_init_motion_algorithms_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Init data structure pointer for Motion Algorithms info service */
  ble_char_pointer = &ble_char_motion_algorithms;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_motion_algorithms;
  ble_char_pointer->write_request_cb = write_request_motion_algorithms;
  COPY_MOTION_ALGORITHM_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = 2 + 1 + 1; /* 2 byte timestamp, 1 byte action, 1 byte algorithm */
  ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_WRITE);
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = ((uint8_t)GATT_NOTIFY_ATTRIBUTE_WRITE) |
                                    ((uint8_t)GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP);
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 1;

  BLE_MANAGER_PRINTF("BLE Motion Algorithms features ok\r\n");

  return ble_char_pointer;
}

/**
  * @brief  Update Motion Algorithms characteristic
  * @param  uint8_t motion_code Detected Motion
  * @retval ble_status_t   Status
  */
ble_status_t ble_motion_algorithms_update(uint8_t motion_code)
{
  ble_status_t ret;
  uint8_t buff[2 + 1 + 1];

  STORE_LE_16(buff, (HAL_GetTick() / 10U));
  buff[2] = (uint8_t)MotionAlgorithmSelected;
  buff[3] = (uint8_t)motion_code;

  BLE_MANAGER_PRINTF("MotionAlgorithmSelected= %d       motion code= %d\r\n", MotionAlgorithmSelected, motion_code);

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_motion_algorithms, 0, 2 + 1 + 1, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating MotionAlgorithms Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating MotionAlgorithms Char\r\n");
    }
  }
  return ret;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Motion Algorithms is subscribed or not to the one service
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
static void attr_mod_request_motion_algorithms(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                               uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_motion_algorithms(BLE_NOTIFY_SUB, MotionAlgorithmSelected);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_motion_algorithms(BLE_NOTIFY_UNSUB, MotionAlgorithmSelected);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t) sprintf((char *)buffer_to_write,
                                       "--->MotionAlgorithms[%d]=%s\n",
                                       MotionAlgorithmSelected,
                                       (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->MotionAlgorithms[%d]=%s", MotionAlgorithmSelected,
                       (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**
  * @brief  This event is given when a write request is received by the server from the client.
  * @param  void *ble_char_pointer
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
static void write_request_motion_algorithms(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                            uint8_t data_length, uint8_t *att_data)
{
  MotionAlgorithmSelected = (ble_motion_algorithms_type_t)att_data[0];
  write_request_motion_algorithms_function(MotionAlgorithmSelected);
}

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t Event Sub/Unsub
  * @param  ble_motion_algorithms_type_t algorithm Motion Algorithm Type
  * @retval None
  */
__weak void notify_event_motion_algorithms(ble_notify_event_t event,
                                           ble_motion_algorithms_type_t algorithm)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);
  BLE_UNUSED(algorithm);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify motion algorithms function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_motion_algorithms could be implemented in the user file
   */
}

/********************************************************
  * Callback function to manage the write request events *
  *******************************************************/
/**
  * @brief  Callback Function for objects detection write request.
  * @param  ble_motion_algorithms_type_t algorithm Motion Algorithm Type
  * @retval None
  */
__weak void write_request_motion_algorithms_function(ble_motion_algorithms_type_t algorithm)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(algorithm);

  BLE_MANAGER_PRINTF("\r\nWrite request motion algorithms function not defined (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the write_request_motion_algorithms_function could be implemented in the user file
   */
}

