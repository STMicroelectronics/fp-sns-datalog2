/**
  ******************************************************************************
  * @file    BLE_HighSpeedDataLog.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add High Speed Data Log info services using vendor specific
  *          profiles.
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
#define COPY_HIGH_SPEED_DATA_LOG_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x11,0x00,0x02,0x11,\
                                                                      0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Exported Variables ------------------------------------------------------- */
CustomNotifyEventHighSpeedDataLog_t CustomNotifyEventHighSpeedDataLog = NULL;
CustomWriteRequestHighSpeedDataLog_t CustomWriteRequestHighSpeedDataLog;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for High Speed Data Log info service */
static BleCharTypeDef BleCharHighSpeedDataLog;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_HighSpeedDataLog(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                             uint8_t data_length, uint8_t *att_data);
static void Write_Request_HighSpeedDataLog(void *BleCharPointer, uint16_t handle, uint16_t Offset, uint8_t data_length,
                                           uint8_t *att_data);

/**
  * @brief  Init High Speed Data Log info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for High Speed Data Log info service
  */
BleCharTypeDef *BLE_InitHighSpeedDataLogService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for High Speed Data Log info service */
  BleCharPointer = &BleCharHighSpeedDataLog;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_HighSpeedDataLog;
  BleCharPointer->Write_Request_CB = Write_Request_HighSpeedDataLog;
  COPY_HIGH_SPEED_DATA_LOG_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = 20;
  BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_WRITE_WITHOUT_RESP);
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_ATTRIBUTE_WRITE;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 1;

  if (CustomWriteRequestHighSpeedDataLog == NULL)
  {
    BLE_MANAGER_PRINTF("Error: Write request High Speed Data Log function not defined\r\n");
  }

  BLE_MANAGER_PRINTF("BLE High Speed Data Log features ok\r\n");

  return BleCharPointer;
}

/**
  * @brief  High Speed Data Log Send Buffer
  * @param  uint8_t* buffer
  * @param  uint32_t len
  * @retval tBleStatus   Status
  */
tBleStatus BLE_HighSpeedDataLogSendBuffer(uint8_t *buffer, uint32_t len)
{
  tBleStatus ret;
  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharHighSpeedDataLog, 0, (uint8_t)len, buffer);
  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if High Speed Data Log is subscribed
  *         or not to the one service
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
static void AttrMod_Request_HighSpeedDataLog(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                             uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventHighSpeedDataLog != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventHighSpeedDataLog(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventHighSpeedDataLog(BLE_NOTIFY_UNSUB);
    }
  }

#if (BLE_DEBUG_LEVEL>1)
  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "--->HSDataLog=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->HSDataLog=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *VoidCharPointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
static void Write_Request_HighSpeedDataLog(void *BleCharPointer, uint16_t handle, uint16_t Offset, uint8_t data_length,
                                           uint8_t *att_data)
{
  if (CustomWriteRequestHighSpeedDataLog != NULL)
  {
    CustomWriteRequestHighSpeedDataLog(att_data, data_length);
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nWrite request High Speed Data Log function not defined\r\n\n");
  }
}

