/**
  ******************************************************************************
  * @file    BLE_PnPLike.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add PnPLike info services using vendor specific profile.
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
#define COPY_PNPLIKE_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x1b,\
                                                          0x00,0x02,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Exported Variables ------------------------------------------------------- */
/* Identifies the notification Events */
CustomNotifyEventPnPLike_t CustomNotifyEventPnPLike = NULL;
CustomWriteRequestPnPLike_t CustomWriteRequestPnPLike = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for PnPLike info service */
static BleCharTypeDef BleCharPnPLike;
/* Buffer used to save the complete command received via BLE*/
static uint8_t *ble_command_buffer = NULL;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_PnPLike(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                    uint8_t *att_data);
static void Write_Request_PnPLike(void *BleCharPointer, uint16_t handle, uint16_t Offset, uint8_t data_length,
                                  uint8_t *att_data);


static uint16_t PnPLikeContentMaxCharLength = DEFAULT_MAX_PNPL_NOTIFICATION_CHAR_LEN;
/**
  * @brief  Init PnPLike info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for PnPLike info service
  */
BleCharTypeDef *BLE_InitPnPLikeService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for PnPLike info service */
  BleCharPointer = &BleCharPnPLike;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_PnPLike;
  BleCharPointer->Write_Request_CB = Write_Request_PnPLike;
  COPY_PNPLIKE_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = PnPLikeContentMaxCharLength;
  BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_WRITE_WITHOUT_RESP);
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_ATTRIBUTE_WRITE;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 1;

  if (CustomWriteRequestPnPLike == NULL)
  {
    BLE_MANAGER_PRINTF("Error: Write request PnPLike function not defined\r\n");
  }

  BLE_MANAGER_PRINTF("BLE PnPLike features ok\r\n");

  return BleCharPointer;
}

/**
  * @brief  PnPLike Send Buffer
  * @param  uint8_t* buffer
  * @param  uint32_t len
  * @retval tBleStatus Status
  */
tBleStatus BLE_PnPLikeUpdate(uint8_t *buffer, uint8_t len)
{
  tBleStatus ret;
  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharPnPLike, 0, len, buffer);

  return ret;

}

/**
  * @brief  This function is called when there is a change on the GATT attribute
  *         With this function it's possible to understand if PnPLike is subscribed or not to the one service
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
static void AttrMod_Request_PnPLike(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                    uint8_t *att_data)
{
  if (CustomNotifyEventPnPLike != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventPnPLike(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventPnPLike(BLE_NOTIFY_UNSUB);
    }
  }

#if (BLE_DEBUG_LEVEL>1)
  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite,
                                    "--->PnPLike=%s\n",
                                    (att_data[0] == BLE_NOTIFY_SUB) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->PnPLike=%s", (att_data[0] == BLE_NOTIFY_SUB) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}


/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *VoidCharPointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
static void Write_Request_PnPLike(void *BleCharPointer, uint16_t handle, uint16_t Offset, uint8_t data_length,
                                  uint8_t *att_data)
{
  uint32_t CommandBufLen = 0;

  if (CustomWriteRequestPnPLike != NULL)
  {
    CommandBufLen = BLE_Command_TP_Parse(&ble_command_buffer, att_data, data_length);

    if (CommandBufLen > 0U)
    {
      CustomWriteRequestPnPLike(ble_command_buffer, CommandBufLen);

#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("\r\n%s\r\n", ble_command_buffer);
#endif /* (BLE_DEBUG_LEVEL>1) */

      BLE_FREE_FUNCTION(ble_command_buffer);
      ble_command_buffer = NULL;
    }
  }
  else
  {
#if (BLE_DEBUG_LEVEL>1)
    BLE_MANAGER_PRINTF("\r\nWrite request PnPLike function not defined\r\n");
#endif /* (BLE_DEBUG_LEVEL>1) */
  }
}

/**
  * @brief  PnPLike Set Max Char Length
  * @param  uint16_t MaxCharLength
  * @retval none
  */
void BLE_PnPLikeSetMaxCharLength(uint16_t MaxCharLength)
{
  PnPLikeContentMaxCharLength = MaxCharLength;
}

/**
  * @brief  PnPLike Get Max Char Length
  * @param  None
  * @retval uint16_t MaxCharLength
  */
uint16_t BLE_PnPLikeGetMaxCharLength(void)
{
  return PnPLikeContentMaxCharLength;
}



