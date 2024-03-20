/**
  ******************************************************************************
  * @file    BLE_Ota.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add OTA characteristic using vendor specific profiles.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "BLE_Manager.h"
#include "BLE_ManagerCommon.h"

#ifdef BLUE_WB

/* Private define ------------------------------------------------------------*/
#define COPY_OTA_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0xFE,0x11,\
                                                      0x8e,0x22,0x45,0x41,0x9d,0x4c,0x21,0xed,0xae,0x82,0xed,0x19)

/* Exported variables --------------------------------------------------------*/
CustomWriteRequestOta_t CustomWriteRequestOta = NULL;


/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for OTA characteristic  */
static BleCharTypeDef BleCharOta;

/* Private functions ---------------------------------------------------------*/
static void Write_Request_Ota(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                              uint8_t *att_data);

/**
  * @brief  Init OTA characteristic
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for OTA characteristic
  */
BleCharTypeDef *BLE_InitOtaService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for OTA characteristic  */
  BleCharPointer = &BleCharOta;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->Write_Request_CB = Write_Request_Ota;
  COPY_OTA_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = 3;
  BleCharPointer->Char_Properties = CHAR_PROP_WRITE_WITHOUT_RESP;
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_ATTRIBUTE_WRITE;
  BleCharPointer->Enc_Key_Size = 10;
  BleCharPointer->Is_Variable = 0;

  if (CustomWriteRequestOta == NULL)
  {
    BLE_MANAGER_PRINTF("Warning: Write request Ota function not defined\r\n");
  }

  BLE_MANAGER_PRINTF("BLE Ota features ok\r\n");

  return BleCharPointer;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if OTA is subscribed or not to the one service
  * @param  void *VoidCharPointer
  * @param  uint16_t attr_handle Handle of the attribute
  * @param  uint16_t Offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode:
  *                          - Bits 0-14: offset of the reported value inside the attribute.
  *                          - Bit 15: if the entire value of the attribute does not fit inside a single
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event, this bit is set to 1 to notify that other
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.
  * @param  uint8_t data_length length of the data
  * @param  uint8_t *att_data attribute data
  * @retval None
  */
static void Write_Request_Ota(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                              uint8_t *att_data)
{
  if (CustomWriteRequestOta != NULL)
  {
    CustomWriteRequestOta(att_data, data_length);
  }
}

#endif /* BLUE_WB */

/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
