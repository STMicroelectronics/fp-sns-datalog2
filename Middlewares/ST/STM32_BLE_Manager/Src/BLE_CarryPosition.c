/**
  ******************************************************************************
  * @file    BLE_CarryPosition.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add Carry Position service using vendor specific profiles.
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
#define COPY_CARRY_POSITION_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x08,0x00,0x01,0x11,\
                                                                 0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define CARRY_POSITION_ADVERTISE_DATA_POSITION  18

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventCarryPosition_t CustomNotifyEventCarryPosition = NULL;
CustomReadRequestCarryPosition_t CustomReadRequestCarryPosition = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Carry Position service */
static BleCharTypeDef BleCarryPosition;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_CarryPosition(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                          uint8_t data_length, uint8_t *att_data);
#if (BLUE_CORE != BLUENRG_LP)
static void Read_Request_CarryPosition(void *BleCharPointer, uint16_t handle);
#else /* (BLUE_CORE != BLUENRG_LP) */
static void Read_Request_CarryPosition(void *BleCharPointer,
                                       uint16_t handle,
                                       uint16_t Connection_Handle,
                                       uint8_t Operation_Type,
                                       uint16_t Attr_Val_Offset,
                                       uint8_t Data_Length,
                                       uint8_t Data[]);
#endif /* (BLUE_CORE != BLUENRG_LP) */

/**
  * @brief  Init Carry Position service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Carry Position service
  */
BleCharTypeDef *BLE_InitCarryPositionService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for Carry Position info service */
  BleCharPointer = &BleCarryPosition;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_CarryPosition;
  BleCharPointer->Read_Request_CB = Read_Request_CarryPosition;
  COPY_CARRY_POSITION_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = 2 + 1; /* 2 byte timestamp, 1 byte action */
  BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_READ);
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 0;

  if (CustomReadRequestCarryPosition == NULL)
  {
    BLE_MANAGER_PRINTF("Warning: Read request Carry Position function not defined\r\n");
  }

  BLE_MANAGER_PRINTF("BLE Carry Position features ok\r\n");

  return BleCharPointer;
}

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Carry Position Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
void BLE_SetCarryPositionAdvertiseData(uint8_t *manuf_data)
{
  manuf_data[CARRY_POSITION_ADVERTISE_DATA_POSITION] |= 0x08U;
}
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Carry Position characteristic
  * @param  BLE_CP_output_t CarryPositionCode Carry Position Recognized
  * @retval tBleStatus   Status
  */
tBleStatus BLE_CarryPositionUpdate(BLE_CP_output_t CarryPositionCode)
{
  tBleStatus ret;
  uint8_t buff[2 + 1];

  STORE_LE_16(buff, (HAL_GetTick() >> 3));
  buff[2] = (uint8_t)CarryPositionCode;

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCarryPosition, 0, 2 + 1, buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating Carry Position Char\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Carry Position Char\r\n");
    }
  }
  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Carry Position is subscribed or not to the one service
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
static void AttrMod_Request_CarryPosition(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                          uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventCarryPosition != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventCarryPosition(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventCarryPosition(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventCarryPosition function Not Defined\r\n");
  }

  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t) sprintf((char *)BufferToWrite,
                                     "--->Carry Position=%s\n",
                                     (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->Carry Position=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* BLE_DEBUG_LEVEL>1 */
}

/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *VoidCharPointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
#if (BLUE_CORE != BLUENRG_LP)
static void Read_Request_CarryPosition(void *VoidCharPointer, uint16_t handle)
{
  if (CustomReadRequestCarryPosition != NULL)
  {
    BLE_CP_output_t CarryPositionCode;
    CustomReadRequestCarryPosition(&CarryPositionCode);
    BLE_CarryPositionUpdate(CarryPositionCode);
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nRead request Carry Position function not defined\r\n\n");
  }
}
#else /* (BLUE_CORE != BLUENRG_LP) */
static void Read_Request_CarryPosition(void *BleCharPointer,
                                       uint16_t handle,
                                       uint16_t Connection_Handle,
                                       uint8_t Operation_Type,
                                       uint16_t Attr_Val_Offset,
                                       uint8_t Data_Length,
                                       uint8_t Data[])
{
  tBleStatus ret;
  if (CustomReadRequestCarryPosition != NULL)
  {
    BLE_CP_output_t CarryPositionCode;
    uint8_t buff[2 + 1];

    CustomReadRequestCarryPosition(&CarryPositionCode);

    STORE_LE_16(buff, (HAL_GetTick() >> 3));
    buff[2] = (uint8_t)CarryPositionCode;

    ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, 2 + 1, buff);
    if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
    {
      if (BLE_StdErr_Service == BLE_SERV_ENABLE)
      {
        BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating  Carry Position Char\n");
        Stderr_Update(BufferToWrite, BytesToWrite);
      }
      else
      {
        BLE_MANAGER_PRINTF("Error: Updating  Carry Position Char\r\n");
      }
    }
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nRead request Carry Position function not defined\r\n\n");
  }

  ret = aci_gatt_srv_authorize_resp_nwk(Connection_Handle, handle,
                                        Operation_Type, 0, Attr_Val_Offset,
                                        Data_Length, Data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("aci_gatt_srv_authorize_resp_nwk() failed: 0x%02x\r\n", ret);
  }
}
#endif /* (BLUE_CORE != BLUENRG_LP) */

