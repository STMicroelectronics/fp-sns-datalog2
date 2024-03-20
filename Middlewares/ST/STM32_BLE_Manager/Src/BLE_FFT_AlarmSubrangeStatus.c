/**
  ******************************************************************************
  * @file    BLE_FFT_AlarmSubrangeStatus.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add BLE FFT Alarm Subrange Status info services using vendor
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
#define COPY_FFT_ALARM_SUBRANGE_STATUS_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x09,0x00,0x02,\
                                                                            0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,\
                                                                            0xc5,0x1b)

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventFFT_AlarmSubrangeStatus_t CustomNotifyEventFFT_AlarmSubrangeStatus = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for FFT Alarm Subrange Status info service */
static BleCharTypeDef BleCharFFT_AlarmSubrangeStatus;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_FFT_AlarmSubrangeStatus(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                                    uint8_t data_length, uint8_t *att_data);

/**
  * @brief  Init FFT Alarm Subrange Status info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for FFT Alarm Subrange Status info service
  */
BleCharTypeDef *BLE_InitFFT_AlarmSubrangeStatusService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for FFT Alarm Subrange Status info service */
  BleCharPointer = &BleCharFFT_AlarmSubrangeStatus;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_FFT_AlarmSubrangeStatus;
  COPY_FFT_ALARM_SUBRANGE_STATUS_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = 2 + 13;
  BleCharPointer->Char_Properties = CHAR_PROP_NOTIFY;
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 0;

  BLE_MANAGER_PRINTF("BLE FFT Alarm Subrange Status features ok\r\n");

  return BleCharPointer;
}

/*
 * @brief  Update FFT Alarm Subrange RMS status value
 * @param  sBLE_FFT_AlarmSubrangeStatus_t              AlarmStatus
 * @param  BLE_FFT_AlarmSubrangeStatusGenericValue_t        SubrangeMaxValue
 * @param  BLE_FFT_AlarmSubrangeStatusGenericValue_t        SubrangeFreqMaxValue
 * @retval tBleStatus   Status
 */
tBleStatus BLE_FFT_AlarmSubrangeStatusUpdate(sBLE_FFT_AlarmSubrangeStatus_t          AlarmStatus,
                                             BLE_FFT_AlarmSubrangeStatusGenericValue_t    SubrangeMaxValue,
                                             BLE_FFT_AlarmSubrangeStatusGenericValue_t    SubrangeFreqMaxValue)
{
  tBleStatus ret;

  float TempResult;

  uint8_t Buff[2 + 13];

  uint8_t Alarm_X = (uint8_t)AlarmStatus.STATUS_AXIS_X;
  uint8_t Alarm_Y = (uint8_t)AlarmStatus.STATUS_AXIS_Y;
  uint8_t Alarm_Z = (uint8_t)AlarmStatus.STATUS_AXIS_Z;

  /* Time Stamp */
  STORE_LE_16(Buff, (HAL_GetTick() >> 3));

  /* Acceleration peak global status */
  Buff[2] = (Alarm_X << 4) | (Alarm_Y  << 2) | (Alarm_Z);

  /*  Peak info for X-axis */
  TempResult = SubrangeFreqMaxValue.x * ((float)10);
  STORE_LE_16(Buff + 3, ((uint16_t)TempResult));
  TempResult = SubrangeMaxValue.x * ((float)100);
  STORE_LE_16(Buff + 5, ((uint16_t)TempResult));

  /*  Peak info for Y-axis */
  TempResult = SubrangeFreqMaxValue.y * ((float)10);
  STORE_LE_16(Buff + 7, ((uint16_t)TempResult));
  TempResult = SubrangeMaxValue.y * ((float)100);
  STORE_LE_16(Buff + 9, ((uint16_t)TempResult));

  /*  Peak info for Z-axis */
  TempResult = SubrangeFreqMaxValue.z * ((float)10);
  STORE_LE_16(Buff + 11, ((uint16_t)TempResult));
  TempResult = SubrangeMaxValue.z * ((float)100);
  STORE_LE_16(Buff + 13, ((uint16_t)TempResult));

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharFFT_AlarmSubrangeStatus, 0, 2 + 13, Buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (ret != (tBleStatus)BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      if (BLE_StdErr_Service == BLE_SERV_ENABLE)
      {
        BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating FFT Alarm Subrange Status Char\n");
        Stderr_Update(BufferToWrite, BytesToWrite);
      }
      else
      {
        BLE_MANAGER_PRINTF("Error Updating FFT Alarm Subrange Status Char ret=%x\r\n", ret);
      }
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating FFT Alarm Subrange Status Char ret=%x\r\n", ret);
    }
  }

  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if FFT Alarm Subrange Status is subscribed
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
static void AttrMod_Request_FFT_AlarmSubrangeStatus(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                                    uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventFFT_AlarmSubrangeStatus != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventFFT_AlarmSubrangeStatus(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventFFT_AlarmSubrangeStatus(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventFFT_AlarmSubrangeStatus function Not Defined\r\n");
  }

  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t) sprintf((char *)BufferToWrite,
                                     "--->FFT Alarm SubRangeStatus=%s\n",
                                     (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->FFT Alarm SubRangeStatus=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* BLE_DEBUG_LEVEL>1 */
}

