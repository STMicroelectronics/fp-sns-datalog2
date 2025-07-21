/**
  ******************************************************************************
  * @file    ble_ota.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add OTA characteristic using vendor specific profiles.
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

#ifdef BLUE_WB

/* Private define ------------------------------------------------------------*/
#define COPY_OTA_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0xFE,0x11,\
                                                      0x8e,0x22,0x45,0x41,0x9d,0x4c,0x21,0xed,0xae,0x82,0xed,0x19)

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for OTA characteristic  */
static ble_char_object_t ble_char_ota;

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
__weak void write_request_ota_function(uint8_t *att_data, uint8_t data_length);

/* Private functions prototype -----------------------------------------------*/
static void write_request_ota(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset, uint8_t data_length,
                              uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init OTA characteristic
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for OTA characteristic
  */
ble_char_object_t *ble_init_ota_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Init data structure pointer for OTA characteristic  */
  ble_char_pointer = &ble_char_ota;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->write_request_cb = write_request_ota;
  COPY_OTA_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = 3;
  ble_char_pointer->char_properties = CHAR_PROP_WRITE_WITHOUT_RESP;
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_ATTRIBUTE_WRITE;
  ble_char_pointer->enc_key_size = 10;
  ble_char_pointer->is_variable = 0;

  BLE_MANAGER_PRINTF("BLE Ota features ok\r\n");

  return ble_char_pointer;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if OTA is subscribed or not to the one service
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
static void write_request_ota(void *void_char_pointer, uint16_t attr_handle, uint16_t offset, uint8_t data_length,
                              uint8_t *att_data)
{
  write_request_ota_function(att_data, data_length);
}

#endif /* BLUE_WB */

/********************************************************
  * Callback function to manage the write request events *
  *******************************************************/
/**
  * @brief  Callback Function for OTA write request.
  * @param  uint8_t *att_data
  * @param  uint8_t data_length
  * @retval None
  */
__weak void write_request_ota_function(uint8_t *att_data, uint8_t data_length)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(att_data);
  BLE_UNUSED(data_length);

  BLE_MANAGER_PRINTF("\r\nWrite request OTA function not defined (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the write_request_ota_function could be implemented in the user file
   */
}

