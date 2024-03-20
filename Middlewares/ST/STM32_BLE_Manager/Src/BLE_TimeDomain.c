/**
  ******************************************************************************
  * @file    BLE_TimeDomain.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add BLE Time Domain info services using vendor specific profiles.
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
#define COPY_TIME_DOMAIN_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x06,0x00,0x02,\
                                                              0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventTimeDomain_t CustomNotifyEventTimeDomain = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Time Domain info service */
static BleCharTypeDef BleCharTimeDomain;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_TimeDomain(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                       uint8_t *att_data);

/**
  * @brief  Init Time Domain info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Time Domain info service
  */
BleCharTypeDef *BLE_InitTimeDomainService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for Time Domain info service */
  BleCharPointer = &BleCharTimeDomain;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_TimeDomain;
  COPY_TIME_DOMAIN_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = 2 + 18;
  BleCharPointer->Char_Properties = CHAR_PROP_NOTIFY;
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 0;

  BLE_MANAGER_PRINTF("BLE Time Domain features ok\r\n");

  return BleCharPointer;
}


/*
 * @brief  Update Time Domain characteristic value
 * @param  BLE_MANAGER_TimeDomainGenericValue_t PeakValue
 * @param  BLE_MANAGER_TimeDomainGenericValue_t SpeedRmsValue
 * @retval tBleStatus   Status
 */
tBleStatus BLE_TimeDomainUpdate(BLE_MANAGER_TimeDomainGenericValue_t PeakValue,
                                BLE_MANAGER_TimeDomainGenericValue_t SpeedRmsValue)
{
  tBleStatus ret;
  uint16_t Temp;

  uint8_t Buff[2 + 18];
  uint8_t BuffPos;

  float TempFloat;
  uint8_t *TempBuff = (uint8_t *) & TempFloat;

  float TempResult;

  STORE_LE_16(Buff, (HAL_GetTick() >> 3));

  /* PeakValue.x * 100 --> for sending 2 bytes */
  TempResult = PeakValue.x * ((float)100);
  Temp = (uint16_t)TempResult;
  STORE_LE_16(Buff + 2, Temp);

  /* PeakValue.y * 100 --> for sending 2 bytes */
  TempResult = PeakValue.y * ((float)100);
  Temp = (uint16_t)TempResult;
  STORE_LE_16(Buff + 4, Temp);

  /* PeakValue.z * 100 --> for sending 2 bytes */
  TempResult = PeakValue.z * ((float)100);
  Temp = (uint16_t)TempResult;
  STORE_LE_16(Buff + 6, Temp);

  BuffPos = 8;

  /* SpeedRmsValue.x * 1000 --> Converts from m/s to mm/s */
  TempFloat = SpeedRmsValue.x * ((float)1000);
  Buff[BuffPos] = TempBuff[0];
  BuffPos++;
  Buff[BuffPos] = TempBuff[1];
  BuffPos++;
  Buff[BuffPos] = TempBuff[2];
  BuffPos++;
  Buff[BuffPos] = TempBuff[3];
  BuffPos++;

  /* SpeedRmsValue.y * 1000 --> Converts from m/s to mm/s */
  TempFloat = SpeedRmsValue.y * ((float)1000);
  Buff[BuffPos] = TempBuff[0];
  BuffPos++;
  Buff[BuffPos] = TempBuff[1];
  BuffPos++;
  Buff[BuffPos] = TempBuff[2];
  BuffPos++;
  Buff[BuffPos] = TempBuff[3];
  BuffPos++;

  /* SpeedRmsValue.z * 1000 --> Converts from m/s to mm/s */
  TempFloat = SpeedRmsValue.z * ((float)1000);
  Buff[BuffPos] = TempBuff[0];
  BuffPos++;
  Buff[BuffPos] = TempBuff[1];
  BuffPos++;
  Buff[BuffPos] = TempBuff[2];
  BuffPos++;
  Buff[BuffPos] = TempBuff[3];
  BuffPos++;

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharTimeDomain, 0, 20, Buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (ret != (tBleStatus)BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      if (BLE_StdErr_Service == BLE_SERV_ENABLE)
      {
        BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating Time Domain Char\n");
        Stderr_Update(BufferToWrite, BytesToWrite);
      }
      else
      {
        BLE_MANAGER_PRINTF("Error Updating Time Domain Char ret=%x\r\n", ret);
      }
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Time Domain Char ret=%x\r\n", ret);
    }
  }

  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Time Domain is subscribed or not to the one service
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
static void AttrMod_Request_TimeDomain(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                       uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventTimeDomain != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventTimeDomain(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventTimeDomain(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventTimeDomain function Not Defined\r\n");
  }

  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite,
                                    "--->TimeDomain=%s\n",
                                    (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->TimeDomain=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

