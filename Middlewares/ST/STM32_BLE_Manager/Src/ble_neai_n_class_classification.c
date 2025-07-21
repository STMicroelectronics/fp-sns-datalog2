/**
  ******************************************************************************
  * @file    ble_neai_n_class_classification.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   NEAI Classification info services APIs.
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
#define COPY_NEAI_CLASSIFICATION_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x1a,0x00,0x02,0x11,\
                                                                      0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Private variables ---------------------------------------------------------*/

/* Data structure pointer for NEAI Classification info service */
static ble_char_object_t ble_char_ncc;
/* Size for Classification Characteristic */
static uint8_t  ncc_char_max_size;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_neai_n_class_classification(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
__weak void write_request_neai_n_class_classification_function(uint8_t *att_data, uint8_t data_length);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_neai_classification(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                                 uint8_t data_length, uint8_t *att_data);
static void write_request_neai_classification(void *ble_char_pointer, uint16_t handle, uint16_t offset,
                                              uint8_t data_length, uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
ble_char_object_t *ble_init_neai_classification_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer = NULL;
  ncc_char_max_size = 8U + CLASS_NUMBER_NCC;

  /* Init data structure pointer for N-Class Classification info service */
  ble_char_pointer = &ble_char_ncc;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_neai_classification;
  ble_char_pointer->write_request_cb = write_request_neai_classification;
  COPY_NEAI_CLASSIFICATION_CHAR_UUID((ble_char_pointer->uuid));

  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = ncc_char_max_size;
  ble_char_pointer->char_properties = ((uint8_t)(CHAR_PROP_NOTIFY)) | ((uint8_t)(CHAR_PROP_WRITE));
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_ATTRIBUTE_WRITE;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 1;

  BLE_MANAGER_PRINTF("BLE NEAI Classification char is ok\r\n");

  return ble_char_pointer;
}

/**
  * @brief  Update NEAI Classification Characteristic Value
  * @param  ble_1cc_output_t output contains info about:
  * - phase = (idle=0x00) | (classification=0x01)
  * - state = (NEAI_OK=0x00) | .....
  * - is_outlier = 0x01 if it is an outlier, 0x00 otherwise
  *
  * ONLY THE PHASE FIELD IS MANDATORY
  *
  * @retval ble_status_t:          Status
  */
ble_status_t ble_neai_1_class_classification_update(ble_1cc_output_t output)
{

  ble_status_t ret;
  uint8_t char_length = 6;

  uint8_t buff[/*Timestamp*/ 2 + /*For future use*/ 2 + /* Type Selector */ 1 + /*Phase*/ 1 + /* State*/ 1
                             + /* Is Outlier */ 1];

  /* Time Stamp */
  STORE_LE_16(buff, (HAL_GetTick() >> 3));

  buff[2] = (uint8_t)0xFF;
  buff[3] = (uint8_t)0xFF;

  buff[4] = (uint8_t)output.sel;
  buff[5] = (uint8_t)output.phase;

  if (output.phase == NEAI_NCC_PHASE_CLASSIFICATION)
  {
    buff[6] = (uint8_t)output.state;
    buff[7] = (uint8_t)output.is_outlier;
    char_length += 2U;
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_ncc, 0, char_length, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating NEAI Classification Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating NEAI Classification Char\r\n");
    }
  }
  return ret;
}

/**
  * @brief  Update NEAI Classification Characteristic Value
  * @param  ble_ncc_output_t output contains info about:
  * - phase = (idle=0x00) | (classification=0x01)
  * - state = (NEAI_OK=0x00) | .....
  * - major class = from 0x00 to CLASS_NUMBER_NCC
  * - probabilities = array with size equals to CLASS_NUMBER_NCC that contains class probabilities
  *
  * ONLY THE PHASE FIELD IS MANDATORY
  *
  * @retval ble_status_t:          Status
  */
ble_status_t ble_neai_n_class_classification_update(ble_ncc_output_t output)
{

  ble_status_t ret;
  uint8_t char_length = 6;
  uint8_t prob;
  uint8_t index;
  float temp;

  uint8_t buff[/*Timestamp*/ 2 + /*For future use*/ 2 + /* Type Selector */ 1U + /*Phase*/ 1U + /* State*/ 1U
                             + /*Most Probable Class*/ 1U + /*Classes Probability*/ + CLASS_NUMBER_NCC ];

  /* Time Stamp */
  STORE_LE_16(buff, (HAL_GetTick() >> 3));

  buff[2] = (uint8_t)0xFF;
  buff[3] = (uint8_t)0xFF;

  buff[4] = (uint8_t)output.sel;
  buff[5] = (uint8_t)output.phase;

  if (output.phase == NEAI_NCC_PHASE_CLASSIFICATION)
  {
    buff[6] = (uint8_t)output.state;
    buff[7] = output.most_probable_class;

    for (prob = 8, index = 0; prob < ((uint8_t)(8U + CLASS_NUMBER_NCC)); prob++, index++)
    {
      temp = (float)100.0 * output.probabilities[index];
      buff[prob] = (uint8_t)(((int32_t)temp));
    }
    char_length = char_length + (2U + CLASS_NUMBER_NCC);
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_ncc, 0, char_length, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating NEAI Classification Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating NEAI Classification Char\r\n");
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

static void attr_mod_request_neai_classification(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                                 uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_neai_n_class_classification(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_neai_n_class_classification(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write,
                                      "--->NEAI_NCC=%s\n",
                                      (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->NEAI_NCC=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
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
static void write_request_neai_classification(void *ble_char_pointer, uint16_t handle, uint16_t offset,
                                              uint8_t data_length, uint8_t *att_data)
{
  write_request_neai_n_class_classification_function(att_data, data_length);
}

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t Event Sub/Unsub
  * @retval None
  */
__weak void notify_event_neai_n_class_classification(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify neai n class classification function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_neai_n_class_classification could be implemented in the user file
   */
}

/********************************************************
  * Callback function to manage the write request events *
  *******************************************************/
/**
  * @brief  Callback Function for Neai N Class Classification write request.
  * @param  uint8_t *att_data
  * @param  uint8_t data_length
  * @retval None
  */
__weak void write_request_neai_n_class_classification_function(uint8_t *att_data, uint8_t data_length)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(att_data);
  BLE_UNUSED(data_length);

  BLE_MANAGER_PRINTF("\r\nWrite request neai n class classification function not defined (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the write_request_neai_n_class_classification_function could be implemented in the user file
   */
}
