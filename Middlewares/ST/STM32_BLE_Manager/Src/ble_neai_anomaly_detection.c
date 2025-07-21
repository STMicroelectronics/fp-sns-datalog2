/**
  ******************************************************************************
  * @file    ble_neai_anomaly_detection.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add Anomaly Detection info services using vendor specific profiles.
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
#define COPY_NEAI_ANOMALYDETECTION_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x19,0x00,0x02,\
                                                                        0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,\
                                                                        0xc5,0x1b)

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for anomaly detection info service */
static ble_char_object_t anomaly_detection_ble_char;
/* Size for AD characteristic */
static uint8_t  anomaly_detection_char_size;
/* TODO: How many AD libraries you want manage? */
/*static uint8_t activeLibraries; */

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_neai_anomaly_detection(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
__weak void write_request_neai_anomaly_detection_function(uint8_t *att_data, uint8_t data_length);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_ad(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset, uint8_t data_length,
                                uint8_t *att_data);
static void write_request_ad(void *ble_char_pointer, uint16_t handle, uint16_t offset, uint8_t data_length,
                             uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
ble_char_object_t *ble_init_ad_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer = NULL;
  anomaly_detection_char_size = 9;

  /* Init data structure pointer for anomaly detection info service */
  ble_char_pointer = &anomaly_detection_ble_char;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_ad;
  ble_char_pointer->write_request_cb = write_request_ad;
  COPY_NEAI_ANOMALYDETECTION_CHAR_UUID((ble_char_pointer->uuid));

  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = anomaly_detection_char_size;
  ble_char_pointer->char_properties = ((uint8_t)(CHAR_PROP_NOTIFY)) | ((uint8_t)(CHAR_PROP_WRITE));
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_ATTRIBUTE_WRITE;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 0;

  BLE_MANAGER_PRINTF("BLE NEAI Anomaly Detection char is ok\r\n");

  return ble_char_pointer;
}

/**
  * @brief  Update NEAI AD characteristic value
  * @param  ble_ad_output_t output contains info about:
  * - phase = (idle=0x00) | (learning=0x01) | (detecting=0x02)
  * - state = (NEAI_OK=0x00) | .....
  * - progress = from 0x00 to 0x64 (completion percentage)
  * - status = (normal=0x00) | (anomaly=0x01)
  * - similarity = from 0x00 to 0x64 (inference probability)
  *
  * ONLY THE PHASE FIELD IS MANDATORY
  * if you don't want use one of the others info, you can put NEAI_AD_ESCAPE
  *
  * @retval ble_status_t:          Status
  */
ble_status_t ble_neai_anomaly_detection_update(ble_ad_output_t output)
{

  ble_status_t ret;

  uint8_t buff[/*Timestamp*/ 2U + /*For future use*/ 2U + /*Phase*/ 1U + /* State*/ 1U
                             + /* Phase Progress*/ 1U + /* Status*/ 1U + /* Similarity*/ 1U ];

  /* Time Stamp */
  STORE_LE_16(buff, (HAL_GetTick() >> 3));

  buff[2] = (uint8_t)0xFF;
  buff[3] = (uint8_t)0xFF;
  buff[4] = (uint8_t)output.phase;
  buff[5] = (uint8_t)output.state;
  buff[6] = output.progress;
  buff[7] = output.status;
  buff[8] = output.similarity;

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&anomaly_detection_ble_char, 0, anomaly_detection_char_size, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating NEAI Anomaly Detection Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating NEAI Anomaly Detection Char\r\n");
    }
  }
  return ret;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Piano is subscribed or not to the one service
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

static void attr_mod_request_ad(void *void_char_pointer, uint16_t attr_handle, uint16_t offset, uint8_t data_length,
                                uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_neai_anomaly_detection(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_neai_anomaly_detection(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write,
                                      "--->NEAI_AD=%s\n",
                                      (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->NEAI_AD=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
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
static void write_request_ad(void *ble_char_pointer, uint16_t handle, uint16_t offset, uint8_t data_length,
                             uint8_t *att_data)
{
  write_request_neai_anomaly_detection_function(att_data, data_length);
}

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t Event Sub/Unsub
  * @retval None
  */
__weak void notify_event_neai_anomaly_detection(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify neai anomaly detection function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_neai_anomaly_detection could be implemented in the user file
   */
}

/********************************************************
  * Callback function to manage the write request events *
  *******************************************************/
/**
  * @brief  Callback Function for Neai Anomaly Detection write request.
  * @param  uint8_t *att_data
  * @param  uint8_t data_length
  * @retval None
  */
__weak void write_request_neai_anomaly_detection_function(uint8_t *att_data, uint8_t data_length)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(att_data);
  BLE_UNUSED(data_length);

  BLE_MANAGER_PRINTF("\r\nWrite request neai anomaly detection function not defined (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the write_request_neai_anomaly_detection_function could be implemented in the user file
   */
}