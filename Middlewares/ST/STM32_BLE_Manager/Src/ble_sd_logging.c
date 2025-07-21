/**
  ******************************************************************************
  * @file    ble_sd_logging.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add SD Logging info services using vendor specific profiles.
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
#define COPY_SDLOGGING_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x10,0x00,\
                                                            0x00,0x01,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define SDLOGGING_ADVERTISE_DATA_POSITION  17

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for SDLog info service */
static ble_char_object_t ble_char_sd_log;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_sd_logging(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
__weak void read_request_sd_logging_function(ble_sd_log_status_t *status, uint32_t *feature_mask, uint32_t *time_step);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
__weak void write_request_sd_logging_function(uint8_t *att_data, uint8_t data_length);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_sd_log(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset, uint8_t data_length,
                                    uint8_t *att_data);
static void write_request_sd_log(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset, uint8_t data_length,
                                 uint8_t *att_data);
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
static void read_request_sd_log(void *ble_char_pointer, uint16_t handle);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
static void read_request_sd_log(void *ble_char_pointer,
                                uint16_t handle,
                                uint16_t connection_handle,
                                uint8_t operation_type,
                                uint16_t attr_val_offset,
                                uint8_t data_length,
                                uint8_t data[]);
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init SDLogging info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for SDLogging info service
  */
ble_char_object_t *ble_init_sd_log_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer = NULL;

  /* Init data structure pointer for SDLogging info service */
  ble_char_pointer = &ble_char_sd_log;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_sd_log;
  ble_char_pointer->write_request_cb = write_request_sd_log;
  ble_char_pointer->read_request_cb = read_request_sd_log;
  COPY_SDLOGGING_CHAR_UUID((ble_char_pointer->uuid));

  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = 2 + 9;
  ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY) |
                                      ((uint8_t)CHAR_PROP_WRITE) |
                                      ((uint8_t)(CHAR_PROP_READ));
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = ((uint8_t)GATT_NOTIFY_ATTRIBUTE_WRITE) |
                                    ((uint8_t)GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP);
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 0;

  BLE_MANAGER_PRINTF("BLE SDLogging features ok\r\n");

  return ble_char_pointer;
}

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting SDLog Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
void ble_set_sd_log_advertise_data(uint8_t *manuf_data)
{
  manuf_data[SDLOGGING_ADVERTISE_DATA_POSITION] |= 0x10U;
}
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update SDLog characteristic value
  * @param  ble_sd_log_status_t status:       SD Log Status
  * @param  uint32_t feature_mask: Feature Mask
  * @param  uint32_t time_step:    time_step
  * @retval ble_status_t:          Status
  */
ble_status_t ble_sd_logging_update(ble_sd_log_status_t status, uint32_t feature_mask, uint32_t time_step)
{
  ble_status_t ret;

  uint8_t buff[2 + 9];

  /* Time Stamp */
  STORE_LE_16(buff, (HAL_GetTick() / 10U));
  buff[2] = (uint8_t)status;
  STORE_LE_32(buff + 3, feature_mask);
  STORE_LE_32(buff + 7, time_step);

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_sd_log, 0, 2 + 9, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating SDLog Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating SDLog Char\r\n");
    }
  }
  return ret;
}


/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if environmental is subscribed or not to the one service
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
static void attr_mod_request_sd_log(void *void_char_pointer, uint16_t attr_handle, uint16_t offset, uint8_t data_length,
                                    uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_sd_logging(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_sd_logging(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "--->SDLog=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->SDLog=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
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
static void write_request_sd_log(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset, uint8_t data_length,
                                 uint8_t *att_data)
{
  write_request_sd_logging_function(att_data, data_length);
}

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *void_char_pointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
static void read_request_sd_log(void *void_char_pointer, uint16_t handle)
{
  ble_sd_log_status_t status;
  uint32_t feature_mask;
  uint32_t time_step;

  read_request_sd_logging_function(&status, &feature_mask, &time_step);
  ble_sd_logging_update(status, feature_mask, time_step);
}
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *void_char_pointer
  * @param  uint16_t handle Handle of the attribute
  * @param  uint16_t connection_handle Connection handle
  * @param  uint8_t operation_type see aci_gatt_srv_authorize_nwk_event
  * @param  uint16_t attr_val_offset ffset from which the value needs to be read or write
  * @param  uint8_t data_length Length of Data field
  * @param  uint8_t data[] The data that the client has requested to write
  * @retval None
  */
static void read_request_sd_log(void *ble_char_pointer,
                                uint16_t handle,
                                uint16_t connection_handle,
                                uint8_t operation_type,
                                uint16_t attr_val_offset,
                                uint8_t data_length,
                                uint8_t data[])
{
  ble_status_t ret;

  ble_sd_log_status_t status;
  uint32_t feature_mask;
  uint32_t time_step;
  uint8_t buff[2 + 9];

  read_request_sd_logging_function(&status, &feature_mask, &time_step);

  STORE_LE_16(buff, (HAL_GetTick() / 10U));
  buff[2] = status;
  STORE_LE_32(buff + 3, feature_mask);
  STORE_LE_32(buff + 7, time_step);

  ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, 9, buff);
  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating SDLogging Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating SDLogging Char\r\n");
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
__weak void notify_event_sd_logging(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify sd logging function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_sd_logging could be implemented in the user file
   */
}

/********************************************************
  * Callback function to manage the read request events *
  *******************************************************/
/**
  * @brief  Callback Function for SD Logging read request.
  * @param  ble_sd_log_status_t *status
  * @param  uint32_t *feature_mask
  * @param  uint32_t *time_step
  * @retval None
  */
__weak void read_request_sd_logging_function(ble_sd_log_status_t *status, uint32_t *feature_mask, uint32_t *time_step)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(status);
  BLE_UNUSED(feature_mask);
  BLE_UNUSED(time_step);

  BLE_MANAGER_PRINTF("\r\nRead request sd logging function not defined (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the read_request_sd_logging_function could be implemented in the user file
   */
}

/********************************************************
  * Callback function to manage the write request events *
  *******************************************************/
/**
  * @brief  Callback Function for SD Logging write request.
  * @param  uint8_t *att_data
  * @param  uint8_t data_length
  * @retval None
  */
__weak void write_request_sd_logging_function(uint8_t *att_data, uint8_t data_length)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(att_data);
  BLE_UNUSED(data_length);

  BLE_MANAGER_PRINTF("\r\nWrite request sd logging function not defined (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the write_request_high_speed_data_log_function could be implemented in the user file
   */
}
