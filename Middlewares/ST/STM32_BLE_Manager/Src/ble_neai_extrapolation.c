/**
  ******************************************************************************
  * @file    ble_neai_extrapolation.c
  * @author  System Research & Applications Team - System LAB DU.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   NEAI Extrapolation info services APIs.
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
#define COPY_NEAI_EXTRAPOLATION_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x24,0x00,0x02,0x11,\
                                                                     0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define VALUE_LENGTH 248U

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for NEAI Extrapolation info service */
static ble_char_object_t ext_ble_char;

/* Private functions ---------------------------------------------------------*/
static void attr_mod_request_neai_extrapolation(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                                uint8_t data_length, uint8_t *att_data);

static void write_request_neai_extrapolation(void *ble_char_pointer, uint16_t handle, uint16_t offset,
                                             uint8_t data_length, uint8_t *att_data);

ble_char_object_t *ble_init_neai_extrapolation_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer = NULL;

  /* Init data structure pointer for N-Class Extrapolation info service */
  ble_char_pointer = &ext_ble_char;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_neai_extrapolation;
  ble_char_pointer->write_request_cb = write_request_neai_extrapolation;
  COPY_NEAI_EXTRAPOLATION_CHAR_UUID((ble_char_pointer->uuid));

  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = VALUE_LENGTH;
  ble_char_pointer->char_properties = ((uint8_t)(CHAR_PROP_NOTIFY)) | ((uint8_t)(CHAR_PROP_WRITE));
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_ATTRIBUTE_WRITE;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 1;

  BLE_MANAGER_PRINTF("BLE NEAI Extrapolation char is ok\r\n");

  return ble_char_pointer;
}

/**
  * @brief  Update NEAI Extrapolation Characteristic Value
  * @param  ble_e_output_t output contains info about:
  * - phase = (IDLE=0x00) | (EXTRAPOLATION=0x01) | (BUSY=0x02)
  * - state = (NEAI_OK=0x00) | (NEAI_NOINIT=0x01) | ...
  * - extrapolated_value = ...
  * - measurement_unit = ...
  * - timestamp = ...
  *
  * ONLY THE PHASE FIELD IS MANDATORY
  *
  * @retval ble_status_t:          Status
  */
ble_status_t BLE_NeaiExtrapolationUpdate(ble_e_output_t output)
{
  ble_status_t ret;
  JSON_Value *root_json;
  JSON_Object *json_status;
  char *serialized_json;
  uint32_t size;

  root_json = json_value_init_object();
  json_status = json_value_get_object(root_json);

  if (output.phase == NEAI_EXT_PHASE_IDLE)
  {
    json_object_dotset_number(json_status, "phase", NEAI_EXT_PHASE_IDLE);
  }
  else if (output.phase == NEAI_EXT_PHASE_BUSY)
  {
    json_object_dotset_number(json_status, "phase", NEAI_EXT_PHASE_BUSY);
  }
  else if (output.phase == NEAI_EXT_PHASE_EXTRAPOLATION)
  {
    ret = json_object_dotset_number(json_status, "phase", output.phase);
    ret = json_object_dotset_number(json_status, "state", output.state);
    if (output.state == NEAI_EXT_STATE_OK)
    {
      ret = json_object_dotset_number(json_status, "target", output.extrapolated_value);
      if (strlen(output.measurement_unit) < 6)
      {
        ret = json_object_dotset_string(json_status, "unit", output.measurement_unit);
      }
      else
        ret = json_object_dotset_string(json_status, "unit", "unit");
    }
  }
  if (strcmp(NEAI_STUB_LIB_ID, NEAI_ID_E) != 0)
  {
    ret = json_object_dotset_boolean(json_status, "stub", false);
  }

  ret = json_object_dotset_number(json_status, "timestamp", HAL_GetTick());

  serialized_json = json_serialize_to_string(root_json);
  size = json_serialization_size(root_json);

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ext_ble_char, 0, size, (uint8_t *)serialized_json);

  json_free_serialized_string(serialized_json);
  json_value_free(root_json);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating NEAI Extrapolation Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating NEAI Extrapolation Char\r\n");
    }
  }
  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Piano is subscribed or not to the one service
  * @param  void *VoidCharPointer
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

static void attr_mod_request_neai_extrapolation(void *VoidCharPointer, uint16_t attr_handle, uint16_t offset,
                                                uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_neai_extrapolation(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_neai_extrapolation(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "--->NEAI_E=%s\n",
                                      (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->NEAI_E=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
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
static void write_request_neai_extrapolation(void *ble_char_pointer, uint16_t handle, uint16_t offset,
                                             uint8_t data_length, uint8_t *att_data)
{
  write_request_neai_extrapolation_function(att_data, data_length);
}

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t Event Sub/Unsub
  * @retval None
  */
void notify_event_neai_extrapolation(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  BLE_MANAGER_PRINTF("\r\nNotify neai extrapolation function not defined (It is a weak function)\r\n");
  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_neai_extrapolation could be implemented in the user file
   */
}

/*******************************************************************
  * Callback function prototype to manage the write request events *
  ******************************************************************/
/**
  * @brief  Callback Function for Json write request.
  * @param  uint8_t *att_data
  * @param  uint8_t data_length
  * @retval None
  */
void write_request_neai_extrapolation_function(uint8_t *att_data, uint8_t data_length)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(att_data);
  BLE_UNUSED(data_length);

  BLE_MANAGER_PRINTF("\r\nRead request neai extrapolation function not defined (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the write_request_neai_extrapolation_function could be implemented in the user file
   */
}