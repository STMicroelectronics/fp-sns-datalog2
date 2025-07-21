/**
  ******************************************************************************
  * @file    ble_gesture_recognition.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add Gesture Recognition service using vendor specific profiles.
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
#define COPY_GESTURE_REC_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x02,0x00,0x01,0x11,0xe1,\
                                                              0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define GESTURE_RECOGNITION_ADVERTISE_DATA_POSITION  18

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Gesture Recognition service */
static ble_char_object_t ble_char_gesture_recognition;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_gesture_recognition(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
__weak void read_request_gesture_recognition_function(ble_gr_output_t *gesture_recognition_code);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_gesture_recognition(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                                 uint8_t data_length, uint8_t *att_data);
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
static void read_request_gesture_recognition(void *ble_char_pointer, uint16_t handle);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
static void read_request_gesture_recognition(void *ble_char_pointer,
                                             uint16_t handle,
                                             uint16_t connection_handle,
                                             uint8_t operation_type,
                                             uint16_t attr_val_offset,
                                             uint8_t data_length,
                                             uint8_t data[]);
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Gesture Recognition service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Gesture Recognition service
  */
ble_char_object_t *ble_init_gesture_recognition_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Init data structure pointer for Gesture Recognition info service */
  ble_char_pointer = &ble_char_gesture_recognition;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_gesture_recognition;
  ble_char_pointer->read_request_cb = read_request_gesture_recognition;
  COPY_GESTURE_REC_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = 2 + 1; /* 2 byte timestamp, 1 byte action */
  ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_READ);
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 0;

  BLE_MANAGER_PRINTF("BLE Gesture Recognition features ok\r\n");

  return ble_char_pointer;
}

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Gesture Recognition Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
void ble_set_gesture_recognition_advertise_data(uint8_t *manuf_data)
{
  manuf_data[GESTURE_RECOGNITION_ADVERTISE_DATA_POSITION] |= 0x02U;
}
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Gesture Recognition characteristic
  * @param  ble_gr_output_t gesture_recognition_code Gesture Recognition Recognized
  * @retval ble_status_t   Status
  */
ble_status_t ble_gesture_recognition_update(ble_gr_output_t gesture_recognition_code)
{
  ble_status_t ret;
  uint8_t buff[2 + 1];

  STORE_LE_16(buff, (HAL_GetTick() / 10U));
  buff[2] = (uint8_t)gesture_recognition_code;

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_gesture_recognition, 0, 2 + 1, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating Gesture Recognition Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Gesture Recognition Char\r\n");
    }
  }
  return ret;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Gesture Recognition is subscribed
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
static void attr_mod_request_gesture_recognition(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                                 uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_gesture_recognition(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_gesture_recognition(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t) sprintf((char *)buffer_to_write,
                                       "--->Gest Rec=%s\n",
                                       (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->Gest Rec=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *void_char_pointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
static void read_request_gesture_recognition(void *void_char_pointer, uint16_t handle)
{
  ble_gr_output_t gesture_recognition_code;
  read_request_gesture_recognition_function(&gesture_recognition_code);
  ble_gesture_recognition_update(gesture_recognition_code);
}
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
static void read_request_gesture_recognition(void *ble_char_pointer,
                                             uint16_t handle,
                                             uint16_t connection_handle,
                                             uint8_t operation_type,
                                             uint16_t attr_val_offset,
                                             uint8_t data_length,
                                             uint8_t data[])
{
  ble_status_t ret;

  ble_gr_output_t gesture_recognition_code;
  uint8_t buff[2 + 1];

  read_request_gesture_recognition_function(&gesture_recognition_code);

  STORE_LE_16(buff, (HAL_GetTick() / 10U));
  buff[2] = (uint8_t)gesture_recognition_code;

  ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, 2 + 1, buff);
  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating Gesture Recognition Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating Gesture Recognition Char\r\n");
    }
  }

#if (BLUE_CORE == STM32WB05N)
  ret = aci_gatt_srv_authorize_resp_nwk(connection_handle, 0x0004, handle,
                                        operation_type, 0, attr_val_offset,
                                        data_length, data);
#else /* (BLUE_CORE == STM32WB05N) */
  ret = aci_gatt_srv_authorize_resp_nwk(connection_handle, handle,
                                        operation_type, 0, attr_val_offset,
                                        data_length, data);
#endif /* (BLUE_CORE == STM32WB05N) */

  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("aci_gatt_srv_authorize_resp_nwk() failed: 0x%02x\r\n", ret);
  }
}
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t Event Sub/Unsub
  * @retval None
  */
__weak void notify_event_gesture_recognition(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify gesture recognition function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_gesture_recognition could be implemented in the user file
   */
}

/********************************************************
  * Callback function to manage the read request events *
  *******************************************************/
/**
  * @brief  Callback Function for Gesture Recognition read request.
  * @param  ble_gr_output_t *gesture_recognition_code
  * @retval None
  */
__weak void read_request_gesture_recognition_function(ble_gr_output_t *gesture_recognition_code)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(gesture_recognition_code);

  BLE_MANAGER_PRINTF("\r\nRead request gesture recognition function not defined (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the read_request_env_function could be implemented in the user file
   */
}
