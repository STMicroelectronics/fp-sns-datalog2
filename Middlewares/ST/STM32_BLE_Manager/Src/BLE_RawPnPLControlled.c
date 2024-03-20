/**
  ******************************************************************************
  * @file    BLE_RawPnPLControlled.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add Raw PnPL ControlledL info services using vendor specific profile.
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
#define COPY_RAW_PNPL_CONTROLLED_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x23,0x00,0x02,0x11,\
                                                                      0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Exported Variables ------------------------------------------------------- */
/* Identifies the notification Events */
CustomNotifyEventRawPnPLControlled_t CustomNotifyEventRawPnPLControlled = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for RawPnPLControlled info service */
static BleCharTypeDef BleCharRawPnPLControlled;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_RawPnPLControlled(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                              uint8_t data_length,
                                              uint8_t *att_data);

/**
  * @brief Init Raw PnPL ControlledL Service
  * @param uint8_t MaxCharSize Max Dimension of the BLE Char
  * @param uint8_t isVariable variable/Fixed length flag
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Raw PnPL Controlled service
  */
BleCharTypeDef *BLE_InitRawPnPLControlledService(uint8_t MaxCharSize, uint8_t isVariable)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for RawPnPLControlled info service */
  BleCharPointer = &BleCharRawPnPLControlled;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_RawPnPLControlled;
  COPY_RAW_PNPL_CONTROLLED_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = MaxCharSize;
  BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY);
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = isVariable;

  BLE_MANAGER_PRINTF("BLE RawPnPLControlled features ok\r\n");

  return BleCharPointer;
}

/**
  * @brief  Return the max Char length for Raw PnPL Controlled characteristic
  * @param  None
  * @retval uint16_t   Max Char length
  */
uint16_t BLE_RawPnPLControlledMaxCharSize(void)
{
  return BleCharRawPnPLControlled.Char_Value_Length;
}

/**
  * @brief  Update  Raw PnPL Controlled characteristic value
  * @param  uint8_t *Data data to Update
  * @param  uint8_t DataSize Size of data to Update
  * @retval tBleStatus   Status
  */
tBleStatus BLE_RawPnPLControlledStatusUpdate(uint8_t *Data, uint8_t DataSize)
{
  tBleStatus ret;
  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharRawPnPLControlled, 0, DataSize, Data);

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
static void AttrMod_Request_RawPnPLControlled(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                              uint8_t data_length,
                                              uint8_t *att_data)
{
  if (CustomNotifyEventRawPnPLControlled != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventRawPnPLControlled(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventRawPnPLControlled(BLE_NOTIFY_UNSUB);
    }
  }

#if (BLE_DEBUG_LEVEL>1)
  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite,
                                    "--->RawPnPLControlled=%s\n",
                                    (att_data[0] == BLE_NOTIFY_SUB) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->RawPnPLControlled=%s", (att_data[0] == BLE_NOTIFY_SUB) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}
