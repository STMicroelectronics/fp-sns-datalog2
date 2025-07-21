/**
  ******************************************************************************
  * @file    ble_medical_signal_16_bit.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add Medical Signal 16 Bits service using vendor specific profiles.
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
#define COPY_MEDICAL_SIGNAL_16BIT_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x26,0x00,\
                                                                       0x02,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,\
                                                                       0xd5,0xc5,0x1b)

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Medical Signal 16 Bits service */
static ble_char_object_t ble_medical_signal_16_bit;

static uint16_t medical_signal_16_bit_max_char_length = DEFAULT_MAX_MEDICAL_SIGNAL_16BIT_CHAR_LEN;

/* Private functions ---------------------------------------------------------*/
static void attr_mod_request_medical_signal_16_bit(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                                   uint8_t data_length, uint8_t *att_data);

/**
  * @brief  Init Medical Signal 16 Bits service
  * @param  uint16_t max_lenght Max Char length
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for char service
  */
ble_char_object_t *ble_init_medical_signal_16_bit_service(uint16_t max_lenght)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Init data structure pointer for char info service */
  ble_char_pointer = &ble_medical_signal_16_bit;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_medical_signal_16_bit;
  COPY_MEDICAL_SIGNAL_16BIT_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = max_lenght;
  ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY);
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 1;

  medical_signal_16_bit_max_char_length = max_lenght;

  BLE_MANAGER_PRINTF("BLE MedicalSig16Bis feature ok\r\n");

  return ble_char_pointer;
}


/**
  * @brief  Update Medical Signal 16 Bits characteristic
  * @param  ble_medical_signal_16_bit_t data_type_id Signal Data Type ID
  * @param  uint32_t time_stamp time_stamp
  * @param  int32_t sample_data_size Total Number of Values
  * @param  int16_t *sample_data  Values array
  * @retval ble_status_t   Status
  */
ble_status_t ble_medical_signal_16_bit_update(ble_medical_signal_16_bit_t data_type_id, uint32_t time_stamp,
                                              int32_t sample_data_size,
                                              int16_t *sample_data)
{
  ble_status_t ret;
  uint8_t buff[DEFAULT_MAX_MEDICAL_SIGNAL_16BIT_CHAR_LEN];
  int32_t counter;
  int32_t total_size = (sample_data_size << 1) + 4 /* TimeStamp */ + 1 /* data_type_id */;

  if (total_size > medical_signal_16_bit_max_char_length)
  {
    BLE_MANAGER_PRINTF("BLE MedicalSig16Bits Error ... Too many Bytes %d>%d\r\n", total_size,
                       medical_signal_16_bit_max_char_length);
    return (ble_status_t) BLE_ERROR_LIMIT_REACHED;
  }

  STORE_LE_32(buff, time_stamp);
  buff[4] = (uint8_t) data_type_id;
  for (counter = 0; counter < sample_data_size; counter++)
  {
    STORE_LE_16(buff + 5 + (counter << 1), sample_data[counter]);
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_medical_signal_16_bit, 0, total_size, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating MedicalSig16Bis Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating MedicalSig16Bis Char\r\n");
    }
  }
  return ret;
}


/**
  * @brief  Medical Signal 16 Bits Get Max Char Length
  * @param  None
  * @retval uint16_t max_char_length
  */
uint16_t ble_medical_signal_16_bit_get_max_char_length(void)
{
  return medical_signal_16_bit_max_char_length;
}

/**
  * @brief  Medical Signal 16 Bits Set Max Char Length
  * @param  uint16_t max_char_length
  * @retval none
  */
void ble_medical_signal_16_bit_set_max_char_length(uint16_t max_char_length)
{
  medical_signal_16_bit_max_char_length = max_char_length;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if E-Compass is subscribed or not to the one service
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
static void attr_mod_request_medical_signal_16_bit(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                                   uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_medical_signal_16_bit(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_medical_signal_16_bit(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t) sprintf((char *)buffer_to_write,
                                       "--->MedicalSignal16Bit=%s\n",
                                       (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->MedicalSignal16Bit=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t Event Sub/Unsub
  * @retval None
  */
void notify_event_medical_signal_16_bit(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  BLE_MANAGER_PRINTF("\r\nNotify medical signal 16 bit function not defined (It is a weak function)\r\n");
  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_medical_signal_16_bit could be implemented in the user file
   */
}

