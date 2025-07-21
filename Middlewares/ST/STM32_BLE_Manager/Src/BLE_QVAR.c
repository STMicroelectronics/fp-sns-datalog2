/**
  ******************************************************************************
  * @file    ble_qvar.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add QVAR info services using vendor specific profiles.
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
#define COPY_QVAR_CHAR_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x16,\
                                                        0x00,0x02,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for inertial info service */
static ble_char_object_t ble_char_qvar;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_qvar(ble_notify_event_t event);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_qvar(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset, uint8_t data_length,
                                  uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init QVAR info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for QVAR info service
  */
ble_char_object_t *ble_init_qvar_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer = NULL;


  /* Init data structure pointer for inertial info service */
  ble_char_pointer = &ble_char_qvar;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_qvar;
  COPY_QVAR_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length =  2 + 4 + 1 + 4 + 4;
  ble_char_pointer->char_properties = CHAR_PROP_NOTIFY;
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 1;

  BLE_MANAGER_PRINTF("BLE QVAR features ok\r\n");

  return ble_char_pointer;
}

/**
  * @brief  Update The QVAR Char
  * @param  int32_t qvar Value [LSB]
  * @param  uint8_t flag (to be defined)
  * @param  int32_t dqvar Value [LSB]
  * @param  uint32_t Parameter (to be defined)
  * @param  int32_t number_element Number of elements to send
  * @retval ble_status_t      Status
  */
ble_status_t ble_qvar_update(int32_t qvar, uint8_t flag, int32_t dqvar, uint32_t param, int32_t number_element)
{
  ble_status_t ret;
  uint8_t buff_pos;

  uint8_t buff[2 + 4 + 1 + 4 + 4];

  /* Time Stamp */
  STORE_LE_16(buff, (HAL_GetTick() / 10U));
  buff_pos = 2U;

  STORE_LE_32(buff + buff_pos, qvar);
  buff_pos += 4U;

  if (number_element > 1)
  {
    buff[2U + buff_pos] = flag;
    buff_pos += 1U;
    if (number_element > 2)
    {
      STORE_LE_32(buff + buff_pos, dqvar);
      buff_pos += 4U;
      if (number_element == 4)
      {
        STORE_LE_32(buff + buff_pos, param);
        buff_pos += 4U;
      }
    }
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_qvar, 0, buff_pos, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ret != (ble_status_t)BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      if (ble_std_err_service == BLE_SERV_ENABLE)
      {
        bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating QVAR Char\n");
        std_err_update(buffer_to_write, bytes_to_write);
      }
      else
      {
        BLE_MANAGER_PRINTF("Error: Updating QVAR Char ret=%x\r\n", ret);
      }
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: UpdatingQVARg Char ret=%x\r\n", ret);
    }
  }

  return ret;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if inertial is subscribed or not to the one service
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
static void attr_mod_request_qvar(void *void_char_pointer, uint16_t attr_handle, uint16_t offset, uint8_t data_length,
                                  uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_qvar(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_qvar(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "--->QVAR=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->QVAR=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t event Sub/Unsub
  * @retval None
  */
__weak void notify_event_qvar(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify Qvar function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_normalization could be implemented in the user file
   */
}
