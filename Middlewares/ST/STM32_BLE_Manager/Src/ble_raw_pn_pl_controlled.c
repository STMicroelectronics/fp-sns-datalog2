/**
  ******************************************************************************
  * @file    ble_raw_pn_pl_controlled.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add Raw PnPL ControlledL info services using vendor specific profile.
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
#define COPY_RAW_PNPL_CONTROLLED_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x23,0x00,0x02,0x11,\
                                                                      0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for RawPnPLControlled info service */
static ble_char_object_t ble_char_raw_pn_pl_controlled;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_raw_pn_p_like_controlled(ble_notify_event_t event);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_raw_pn_pl_controlled(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                                  uint8_t data_length,
                                                  uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief Init Raw PnPL ControlledL Service
  * @param uint8_t max_char_size Max Dimension of the BLE Char
  * @param uint8_t is_variable variable/Fixed length flag
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Raw PnPL Controlled service
  */
ble_char_object_t *ble_init_raw_pn_pl_controlled_service(uint8_t max_char_size, uint8_t is_variable)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Init data structure pointer for RawPnPLControlled info service */
  ble_char_pointer = &ble_char_raw_pn_pl_controlled;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_raw_pn_pl_controlled;
  COPY_RAW_PNPL_CONTROLLED_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = max_char_size;
  ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY);
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = is_variable;

  BLE_MANAGER_PRINTF("BLE RawPnPLControlled features ok\r\n");

  return ble_char_pointer;
}

/**
  * @brief  Return the max Char length for Raw PnPL Controlled characteristic
  * @param  None
  * @retval uint16_t   Max Char length
  */
uint16_t ble_raw_pn_pl_controlled_max_char_size(void)
{
  return ble_char_raw_pn_pl_controlled.char_value_length;
}

/**
  * @brief  Update  Raw PnPL Controlled characteristic value
  * @param  uint8_t *data data to Update
  * @param  uint8_t data_size Size of data to Update
  * @retval ble_status_t   Status
  */
ble_status_t ble_raw_pn_pl_controlled_status_update(uint8_t *data, uint8_t data_size)
{
  ble_status_t ret;
  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_raw_pn_pl_controlled, 0, data_size, data);

  return ret;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the GATT attribute
  *         With this function it's possible to understand if PnPLike is subscribed or not to the one service
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
static void attr_mod_request_raw_pn_pl_controlled(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                                  uint8_t data_length,
                                                  uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_raw_pn_p_like_controlled(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_raw_pn_p_like_controlled(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write,
                                      "--->RawPnPLControlled=%s\n",
                                      (att_data[0] == BLE_NOTIFY_SUB) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->RawPnPLControlled=%s", (att_data[0] == BLE_NOTIFY_SUB) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t Event Sub/Unsub
  * @retval None
  */
__weak void notify_event_raw_pn_p_like_controlled(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify raw PnP like controlled function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_raw_pn_p_like_controlled could be implemented in the user file
   */
}
