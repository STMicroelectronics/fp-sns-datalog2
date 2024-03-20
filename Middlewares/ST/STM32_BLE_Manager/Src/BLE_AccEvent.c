/**
  ******************************************************************************
  * @file    BLE_AccEvent.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add Acceleromenter HW Event info service using vendor specific profiles.
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
#define COPY_ACC_EVENT_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x04,0x00,\
                                                            0x00,0x01,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define ACC_EVENT_ADVERTISE_DATA_POSITION  17

/* Exported variables --------------------------------------------------------*/

CustomReadRequestAccEvent_t CustomReadRequestAccEvent = NULL;
CustomNotifyEventAccEvent_t CustomNotifyEventAccEvent = NULL;


/* Private variables ---------------------------------------------------------*/

/* Data structure pointer for HW Acceleromenter Event service */
static BleCharTypeDef BleCharAccEvent;


/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_AccEvent(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                     uint8_t *att_data);
#if (BLUE_CORE != BLUENRG_LP)
static void Read_Request_AccEvent(void *BleCharPointer, uint16_t handle);
#else /* (BLUE_CORE != BLUENRG_LP) */
static void Read_Request_AccEvent(void *BleCharPointer,
                                  uint16_t handle,
                                  uint16_t Connection_Handle,
                                  uint8_t Operation_Type,
                                  uint16_t Attr_Val_Offset,
                                  uint8_t Data_Length,
                                  uint8_t Data[]);
#endif /* (BLUE_CORE != BLUENRG_LP) */

/**
  * @brief  Init HW Acceleromenter Event info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for HW Acceleromenter Event info service
  */
BleCharTypeDef *BLE_InitAccEnvService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;


  BleCharPointer = &BleCharAccEvent;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_AccEvent;
  BleCharPointer->Read_Request_CB = Read_Request_AccEvent;
  COPY_ACC_EVENT_CHAR_UUID((BleCharPointer->uuid));

  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = 2 + 3;
  BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_READ);
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 1;

  if (CustomReadRequestAccEvent == NULL)
  {
    BLE_MANAGER_PRINTF("Warning: Read request HW Acceleromenter Event function not defined\r\n");
  }

  BLE_MANAGER_PRINTF("BLE HW Acceleromenter Event features ok\r\n");

  return BleCharPointer;
}

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting HW Acceleromenter Event Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
void BLE_SetAccEnvAdvertiseData(uint8_t *manuf_data)
{
  manuf_data[ACC_EVENT_ADVERTISE_DATA_POSITION] |= 0x04U;
}
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update  HW Acceleromenter Event characteristic
  * @param  uint16_t Command to Send
  * @param  uint8_t Command Length
  * @retval tBleStatus: Status
  */
tBleStatus BLE_AccEnvUpdate(uint16_t Command, uint8_t dimByte)
{
  tBleStatus ret;
  uint8_t buff[2 + 3];

  STORE_LE_16(buff, (HAL_GetTick() >> 3));

  if (dimByte == 3U)
  {
    buff[2] = 0;
    STORE_LE_16(buff + 3, Command);
  }
  else
  {
    STORE_LE_16(buff + 2, Command);
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharAccEvent, 0, 2U + dimByte, buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating HW Acc Event Char\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating HW Acc Event Char\r\n");
    }
  }
  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if environmental is subscribed or not to the one service
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
static void AttrMod_Request_AccEvent(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                     uint8_t *att_data)
{

  if (CustomNotifyEventAccEvent != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventAccEvent(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventAccEvent(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventAccEvent function Not Defined\r\n");
  }

  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t) sprintf((char *)BufferToWrite, "--->AccEvent=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->AccEvent=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
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
static void Read_Request_AccEvent(void *VoidCharPointer, uint16_t handle)
{
  if (CustomReadRequestAccEvent != NULL)
  {
    uint16_t Command;
    uint8_t dimByte;
    CustomReadRequestAccEvent(&Command, &dimByte);
    BLE_AccEnvUpdate(Command, dimByte);
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nRead request AccEvent function not defined\r\n\n");
  }
}
#else /* (BLUE_CORE != BLUENRG_LP) */
static void Read_Request_AccEvent(void *BleCharPointer,
                                  uint16_t handle,
                                  uint16_t Connection_Handle,
                                  uint8_t Operation_Type,
                                  uint16_t Attr_Val_Offset,
                                  uint8_t Data_Length,
                                  uint8_t Data[])
{
  tBleStatus ret;
  if (CustomReadRequestAccEvent != NULL)
  {
    uint16_t Command;
    uint8_t dimByte;
    uint8_t buff[2 + 3];

    CustomReadRequestAccEvent(&Command, &dimByte);
    STORE_LE_16(buff, (HAL_GetTick() >> 3));

    if (dimByte == 3U)
    {
      buff[2] = 0;
      STORE_LE_16(buff + 3, Command);
    }
    else
    {
      STORE_LE_16(buff + 2, Command);
    }
    ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, 2U + dimByte, buff);
    if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
    {
      if (BLE_StdErr_Service == BLE_SERV_ENABLE)
      {
        BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating AccEvent Char\n");
        Stderr_Update(BufferToWrite, BytesToWrite);
      }
      else
      {
        BLE_MANAGER_PRINTF("Error: Updating AccEvent Char\r\n");
      }
    }
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nRead request AccEvent function not defined\r\n\n");
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

