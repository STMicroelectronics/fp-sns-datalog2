/**
  ******************************************************************************
  * @file    ble_motion_intensity.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add Motion Intensity service using vendor specific profiles.
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
#define COPY_INTENSITY_DETECTION_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x20,0x00,0x01,0x11,\
                                                                      0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define INTENSITY_DETECTION_ADVERTISE_DATA_POSITION  18

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Motion Intensity service */
static ble_char_object_t ble_char_motion_intensity;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_motion_intensity(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
__weak void read_request_motion_intensity_function(ble_id_output_t *motion_intensity_code);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_motion_intensity(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                              uint8_t data_length, uint8_t *att_data);
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
static void read_request_motion_intensity(void *ble_char_pointer, uint16_t handle);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
static void read_request_motion_intensity(void *ble_char_pointer,
                                          uint16_t handle,
                                          uint16_t connection_handle,
                                          uint8_t operation_type,
                                          uint16_t attr_val_offset,
                                          uint8_t data_length,
                                          uint8_t data[]);
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Motion Intensity service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Motion Intensity service
  */
ble_char_object_t *ble_init_motion_intensity_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Init data structure pointer for Motion Intensity info service */
  ble_char_pointer = &ble_char_motion_intensity;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_motion_intensity;
  ble_char_pointer->read_request_cb = read_request_motion_intensity;
  COPY_INTENSITY_DETECTION_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = 2 + 1; /* 2 byte timestamp, 1 byte action */
  ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_READ);
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 0;

  BLE_MANAGER_PRINTF("BLE Motion Intensity features ok\r\n");

  return ble_char_pointer;
}

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Motion Intensity Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
void ble_set_motion_intensity_advertise_data(uint8_t *manuf_data)
{
  manuf_data[INTENSITY_DETECTION_ADVERTISE_DATA_POSITION] |= 0x20U;
}
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Motion Intensity characteristic
  * @param  ble_id_output_t motion_intensity_code Motion Intensity Recognized
  * @retval ble_status_t   Status
  */
ble_status_t ble_motion_intensity_update(ble_id_output_t motion_intensity_code)
{
  ble_status_t ret;
  uint8_t buff[2 + 1];

  STORE_LE_16(buff, (HAL_GetTick() / 10U));
  buff[2] = (uint8_t)motion_intensity_code;

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_motion_intensity, 0, 2 + 1, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating Motion Intensity Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Motion Intensity Char\r\n");
    }
  }
  return ret;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Motion Intensity is subscribed or not to the one service
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
static void attr_mod_request_motion_intensity(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                              uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_motion_intensity(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_motion_intensity(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write,
                                      "--->MotionIntensity=%s\n",
                                      (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->MotionIntensity=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
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
static void read_request_motion_intensity(void *void_char_pointer, uint16_t handle)
{
  ble_id_output_t motion_intensity_code;
  read_request_motion_intensity_function(&motion_intensity_code);
  ble_motion_intensity_update(motion_intensity_code);
}
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
static void read_request_motion_intensity(void *ble_char_pointer,
                                          uint16_t handle,
                                          uint16_t connection_handle,
                                          uint8_t operation_type,
                                          uint16_t attr_val_offset,
                                          uint8_t data_length,
                                          uint8_t data[])
{
  ble_status_t ret;

  ble_id_output_t motion_intensity_code;
  uint8_t buff[2 + 1];

  read_request_motion_intensity_function(&motion_intensity_code);

  STORE_LE_16(buff, (HAL_GetTick() / 10U));
  buff[2] = (uint8_t)motion_intensity_code;

  ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, 2 + 1, buff);
  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating Motion Intensity Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating Motion Intensity Char\r\n");
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
__weak void notify_event_motion_intensity(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify motion intensity function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_motion_intensity could be implemented in the user file
   */
}

/********************************************************
  * Callback function to manage the read request events *
  *******************************************************/
/**
  * @brief  Callback Function for Motion Intensity read request.
  * @param  ble_id_output_t *motion_intensity_code Motion Intensity Type Code
  * @retval None
  */
__weak void read_request_motion_intensity_function(ble_id_output_t *motion_intensity_code)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(motion_intensity_code);

  BLE_MANAGER_PRINTF("\r\nRead request motion intensity function not defined (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the read_request_motion_intensity_function could be implemented in the user file
   */
}
