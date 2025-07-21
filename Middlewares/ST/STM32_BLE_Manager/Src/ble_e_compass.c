/**
  ******************************************************************************
  * @file    ble_e_compass.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add E-Compass service using vendor specific profiles.
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
#define COPY_ECOMPASS_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x40,\
                                                           0x00,0x01,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define ECOMPASS_ADVERTISE_DATA_POSITION  18

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for E-Compass service */
static ble_char_object_t ble_char_e_compass;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_e_compass(ble_notify_event_t event);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_e_compass(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                       uint8_t data_length,
                                       uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init E-Compass service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for E-Compass service
  */
ble_char_object_t *ble_init_e_compass_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Init data structure pointer for E-Compass info service */
  ble_char_pointer = &ble_char_e_compass;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_e_compass;
  COPY_ECOMPASS_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = 2 + 2; /* 2 byte timestamp, 2 byte angle */
  ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY);
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 0;

  BLE_MANAGER_PRINTF("BLE E-Compass features ok\r\n");

  return ble_char_pointer;
}

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting E-Compass Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
void ble_set_e_compass_advertise_data(uint8_t *manuf_data)
{
  manuf_data[ECOMPASS_ADVERTISE_DATA_POSITION] |= 0x40U;
}
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update E-Compass characteristic
  * @param  uint16_t angle measured
  * @retval ble_status_t   Status
  */
ble_status_t ble_e_compass_update(uint16_t angle)
{
  ble_status_t ret;
  uint8_t buff[2 + 2];

  STORE_LE_16(buff, (HAL_GetTick() / 10U));
  STORE_LE_16(buff + 2, angle);

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_e_compass, 0, 2 + 2, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating E-Compass Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating E-Compass Char\r\n");
    }
  }
  return ret;
}

/* Private functions ---------------------------------------------------------*/
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
static void attr_mod_request_e_compass(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                       uint8_t data_length,
                                       uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_e_compass(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_e_compass(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t) sprintf((char *)buffer_to_write,
                                       "--->E-Compass=%s\n",
                                       (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->E-Compass=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* BLE_DEBUG_LEVEL>1 */
}

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t event Sub/Unsub
  * @retval None
  */
__weak void notify_event_e_compass(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify e-compass function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_e_compass could be implemented in the user file
   */
}

