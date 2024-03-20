/**
  ******************************************************************************
  * @file    BLE_TimeDomainAlarmSpeed_RMS_Status.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add BLE Time Domain Alarm Speed RMS Status info services using vendor
  *          specific profiles.
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
#define COPY_TD_ALARM_SPEED_RMS_STATUS_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x07,0x00,0x02,\
                                                                            0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,\
                                                                            0xc5,0x1b)

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventTD_AlarmSpeed_RMS_Status_t CustomNotifyEventTD_AlarmSpeed_RMS_Status = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Time Domain Alarm Speed RMS Status info service */
static BleCharTypeDef BleCharTD_AlarmSpeed_RMS_Status;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_TD_AlarmSpeed_RMS_Status(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                                     uint8_t data_length, uint8_t *att_data);

/**
  * @brief  Init Time Domain Alarm Speed RMS Status info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Time Domain Alarm Speed RMS Status info service
  */
BleCharTypeDef *BLE_InitTD_AlarmSpeed_RMS_StatusService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for Time Domain Alarm Speed RMS Status info service */
  BleCharPointer = &BleCharTD_AlarmSpeed_RMS_Status;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_TD_AlarmSpeed_RMS_Status;
  COPY_TD_ALARM_SPEED_RMS_STATUS_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = 2 + 13;
  BleCharPointer->Char_Properties = CHAR_PROP_NOTIFY;
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 0;

  BLE_MANAGER_PRINTF("BLE Time Domain Alarm Speed RMS Status features ok\r\n");

  return BleCharPointer;
}

/*
 * @brief  Update Time Domain Alarm Speed RMS status value
 * @param  sBLE_TD_AlarmSpeed_RMS_Status_t              Alarm
 * @param  BLE_TD_AlarmSpeed_RMS_StatusGenericValue_t        SpeedRmsValue
 * @retval tBleStatus   Status
 */
tBleStatus BLE_TD_AlarmSpeed_RMS_StatusUpdate(sBLE_TD_AlarmSpeed_RMS_Status_t Alarm,
                                              BLE_TD_AlarmSpeed_RMS_StatusGenericValue_t SpeedRmsValue)
{
  tBleStatus ret;

  float TempFloat;
  uint32_t *TempBuff = (uint32_t *) & TempFloat;

  uint8_t Buff[2 + 13];
  uint8_t BuffPos;

  uint8_t Alarm_X = (uint8_t)Alarm.STATUS_AXIS_X;
  uint8_t Alarm_Y = (uint8_t)Alarm.STATUS_AXIS_Y;
  uint8_t Alarm_Z = (uint8_t)Alarm.STATUS_AXIS_Z;

  /* Timestamp */
  STORE_LE_16(Buff, (HAL_GetTick() >> 3));

  /* Acceleration rms global status */
  Buff[2] = (Alarm_X << 4) | (Alarm_Y  << 2) | (Alarm_Z);
  BuffPos = 3;

  /* RMS speed as mm/s */
  TempFloat = SpeedRmsValue.x;
  STORE_LE_32(&Buff[BuffPos], *TempBuff);
  BuffPos += ((uint8_t)4);
  TempFloat = SpeedRmsValue.y;
  STORE_LE_32(&Buff[BuffPos], *TempBuff);
  BuffPos += ((uint8_t)4);
  TempFloat = SpeedRmsValue.z;
  STORE_LE_32(&Buff[BuffPos], *TempBuff);

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharTD_AlarmSpeed_RMS_Status, 0, 2 + 13, Buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (ret != (tBleStatus)BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      if (BLE_StdErr_Service == BLE_SERV_ENABLE)
      {
        BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite,
                                        "Error Updating Time Domain Alarm Speed RMS Status Char\n");
        Stderr_Update(BufferToWrite, BytesToWrite);
      }
      else
      {
        BLE_MANAGER_PRINTF("Error Updating Time Domain Alarm Speed RMS Status Char ret=%x\r\n", ret);
      }
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Time Domain Alarm Speed RMS Status Char ret=%x\r\n", ret);
    }
  }

  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Time Domain Alarm Speed RMS
  *         Status is subscribed or not to the one service
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
static void AttrMod_Request_TD_AlarmSpeed_RMS_Status(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                                     uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventTD_AlarmSpeed_RMS_Status != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventTD_AlarmSpeed_RMS_Status(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventTD_AlarmSpeed_RMS_Status(BLE_NOTIFY_UNSUB);
    }
  }

#if (BLE_DEBUG_LEVEL>1)
  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite,
                                    "--->TD Alarm Speed RMS Status=%s\n",
                                    (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->TD Alarm Speed RMS Status=%s",
                       (att_data[0] == 01U == BLE_NOTIFY_SUB) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

