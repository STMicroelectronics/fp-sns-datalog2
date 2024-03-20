/**
  ******************************************************************************
  * @file    BLE_SDLogging.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add SD Logging info services using vendor specific profiles.
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
#define COPY_SDLOGGING_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x10,0x00,\
                                                            0x00,0x01,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define SDLOGGING_ADVERTISE_DATA_POSITION  17

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventSDLog_t CustomNotifyEventSDLog = NULL;
CustomWriteRequestSDLog_t CustomWriteSDLog = NULL;
CustomReadRequestSDLog_t CustomReadRequestSDLog = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for SDLog info service */
static BleCharTypeDef BleCharSDLog;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_SDLog(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                  uint8_t *att_data);
static void Write_Request_SDLog(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                uint8_t *att_data);
#if (BLUE_CORE != BLUENRG_LP)
static void Read_Request_SDLog(void *BleCharPointer, uint16_t handle);
#else /* (BLUE_CORE != BLUENRG_LP) */
static void Read_Request_SDLog(void *BleCharPointer,
                               uint16_t handle,
                               uint16_t Connection_Handle,
                               uint8_t Operation_Type,
                               uint16_t Attr_Val_Offset,
                               uint8_t Data_Length,
                               uint8_t Data[]);
#endif /* (BLUE_CORE != BLUENRG_LP) */

/**
  * @brief  Init SDLogging info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for SDLogging info service
  */
BleCharTypeDef *BLE_InitSDLogService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer = NULL;

  /* Init data structure pointer for SDLogging info service */
  BleCharPointer = &BleCharSDLog;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_SDLog;
  BleCharPointer->Write_Request_CB = Write_Request_SDLog;
  BleCharPointer->Read_Request_CB = Read_Request_SDLog;
  COPY_SDLOGGING_CHAR_UUID((BleCharPointer->uuid));

  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = 2 + 9;
  BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY) |
                                    ((uint8_t)CHAR_PROP_WRITE) |
                                    ((uint8_t)(CHAR_PROP_READ));
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = ((uint8_t)GATT_NOTIFY_ATTRIBUTE_WRITE) |
                                  ((uint8_t)GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP);
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 0;

  BLE_MANAGER_PRINTF("BLE SDLogging features ok\r\n");

  return BleCharPointer;
}

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting SDLog Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
void BLE_SetSDLogAdvertiseData(uint8_t *manuf_data)
{
  manuf_data[SDLOGGING_ADVERTISE_DATA_POSITION] |= 0x10U;
}
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update SDLog characteristic value
  * @param  BLE_SDLog_Status_t Status:       SD Log Status
  * @param  uint32_t FeatureMask: Feature Mask
  * @param  uint32_t TimeStep:    TimeStep
  * @retval tBleStatus:          Status
  */
tBleStatus BLE_SDLoggingUpdate(BLE_SDLog_Status_t Status, uint32_t FeatureMask, uint32_t TimeStep)
{
  tBleStatus ret;

  uint8_t buff[2 + 9];

  /* Time Stamp */
  STORE_LE_16(buff, (HAL_GetTick() >> 3));
  buff[2] = (uint8_t)Status;
  STORE_LE_32(buff + 3, FeatureMask);
  STORE_LE_32(buff + 7, TimeStep);

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharSDLog, 0, 2 + 9, buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating SDLog Char\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating SDLog Char\r\n");
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
static void AttrMod_Request_SDLog(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                  uint8_t *att_data)
{
  if (CustomNotifyEventSDLog != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventSDLog(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventSDLog(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventSDLog function Not Defined\r\n");
  }

  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "--->SDLog=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->SDLog=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *VoidCharPointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
static void Write_Request_SDLog(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                uint8_t *att_data)
{
  if (CustomWriteSDLog != NULL)
  {
    CustomWriteSDLog(att_data, data_length);
  }
}

#if (BLUE_CORE != BLUENRG_LP)
/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *VoidCharPointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
static void Read_Request_SDLog(void *VoidCharPointer, uint16_t handle)
{
  if (CustomReadRequestSDLog != NULL)
  {
    BLE_SDLog_Status_t Status;
    uint32_t FeatureMask;
    uint32_t TimeStep;

    CustomReadRequestSDLog(&Status, &FeatureMask, &TimeStep);
    BLE_SDLoggingUpdate(Status, FeatureMask, TimeStep);
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nRead request SDLogging function not defined\r\n\n");
  }
}
#else /* (BLUE_CORE != BLUENRG_LP) */
/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *VoidCharPointer
  * @param  uint16_t handle Handle of the attribute
  * @param  uint16_t Connection_Handle Connection handle
  * @param  uint8_t Operation_Type see aci_gatt_srv_authorize_nwk_event
  * @param  uint16_t Attr_Val_Offset ffset from which the value needs to be read or write
  * @param  uint8_t Data_Length Length of Data field
  * @param  uint8_t Data[] The data that the client has requested to write
  * @retval None
  */
static void Read_Request_SDLog(void *BleCharPointer,
                               uint16_t handle,
                               uint16_t Connection_Handle,
                               uint8_t Operation_Type,
                               uint16_t Attr_Val_Offset,
                               uint8_t Data_Length,
                               uint8_t Data[])
{
  tBleStatus ret;
  if (CustomReadRequestSDLog != NULL)
  {
    BLE_SDLog_Status_t Status;
    uint32_t FeatureMask;
    uint32_t TimeStep;
    uint8_t buff[2 + 9];

    CustomReadRequestSDLog(&Status, &FeatureMask, &TimeStep);

    STORE_LE_16(buff, (HAL_GetTick() >> 3));
    buff[2] = Status;
    STORE_LE_32(buff + 3, FeatureMask);
    STORE_LE_32(buff + 7, TimeStep);

    ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, 9, buff);
    if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
    {
      if (BLE_StdErr_Service == BLE_SERV_ENABLE)
      {
        BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating SDLogging Char\n");
        Stderr_Update(BufferToWrite, BytesToWrite);
      }
      else
      {
        BLE_MANAGER_PRINTF("Error: Updating SDLogging Char\r\n");
      }
    }
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nRead request SDLogging function not defined\r\n\n");
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
