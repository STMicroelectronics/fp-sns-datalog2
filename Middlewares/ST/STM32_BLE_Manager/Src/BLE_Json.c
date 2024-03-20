/**
  ******************************************************************************
  * @file    BLE_Json.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add Json info services using vendor specific profile.
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

/* Private define ------------------------------------------------------------*/
#define COPY_JSON_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x20,\
                                                       0x00,0x02,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Exported Variables ------------------------------------------------------- */
/* Identifies the notification Events */
CustomNotifyEventJson_t CustomNotifyEventJson = NULL;
CustomWriteRequestJson_t CustomWriteRequestJson = NULL;

/* Well know Commands */
char *JsonCommandName[BLE_JSON_TOTAL_NUMBER] =
{
  "ReadModes",
  "NFCWiFi",
  "NFCVCard",
  "GenericText",
  "NFCURL"
};

char *JsonAnswer = "Answer";


/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Json info service */
static BleCharTypeDef BleCharJson;
/* Buffer used to save the complete command received via BLE*/
static uint8_t *ble_command_buffer = NULL;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_Json(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                 uint8_t *att_data);
static void Write_Request_Json(void *BleCharPointer, uint16_t handle, uint16_t Offset, uint8_t data_length,
                               uint8_t *att_data);

/**
  * @brief  Init Json info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Json info service
  */
BleCharTypeDef *BLE_InitJsonService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for Json info service */
  BleCharPointer = &BleCharJson;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_Json;
  BleCharPointer->Write_Request_CB = Write_Request_Json;
  COPY_JSON_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = 20;
  BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_WRITE_WITHOUT_RESP);
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_ATTRIBUTE_WRITE;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 1;

  if (CustomWriteRequestJson == NULL)
  {
    BLE_MANAGER_PRINTF("Error: Write request Json function not defined\r\n");
  }

  BLE_MANAGER_PRINTF("BLE Json features ok\r\n");

  return BleCharPointer;
}

/**
  * @brief  Json Send Buffer
  * @param  uint8_t* buffer
  * @param  uint32_t len
  * @retval tBleStatus Status
  */
tBleStatus BLE_JsonUpdate(uint8_t *buffer, uint32_t len)
{
  tBleStatus ret;
  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharJson, 0, (uint8_t)len, buffer);

  return ret;

}

/**
  * @brief  This function is called when there is a change on the GATT attribute
  *         With this function it's possible to understand if Json is subscribed or not to the one service
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
static void AttrMod_Request_Json(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                 uint8_t *att_data)
{
  if (CustomNotifyEventJson != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventJson(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventJson(BLE_NOTIFY_UNSUB);
    }
  }

#if (BLE_DEBUG_LEVEL>1)
  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "--->Json=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->Json=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}


/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *VoidCharPointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
static void Write_Request_Json(void *BleCharPointer, uint16_t handle, uint16_t Offset, uint8_t data_length,
                               uint8_t *att_data)
{
  uint32_t CommandBufLen = 0;

  if (CustomWriteRequestJson != NULL)
  {
    CommandBufLen = BLE_Command_TP_Parse(&ble_command_buffer, att_data, data_length);

    if (CommandBufLen > 0U)
    {
      CustomWriteRequestJson(ble_command_buffer, CommandBufLen);

      BLE_FREE_FUNCTION(ble_command_buffer);
      ble_command_buffer = NULL;
    }
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nWrite request Json function not defined\r\n\n");
  }
}


