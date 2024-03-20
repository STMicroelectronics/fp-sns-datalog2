/**
  ******************************************************************************
  * @file    BLE_FFT_Amplitude.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add BLE FFT amplitude info services using vendor specific profiles.
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
#define COPY_FFT_AMPLITUDE_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x05,0x00,0x02,\
                                                                0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventFFT_Amplitude_t CustomNotifyEventFFT_Amplitude = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for FFT Amplitude info service */
static BleCharTypeDef BleCharFFTAmplitude;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_FFTAmplitude(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                         uint8_t data_length, uint8_t *att_data);

/**
  * @brief  Init FFT Amplitude info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for FFT Amplitude info service
  */
BleCharTypeDef *BLE_InitFFTAmplitudeService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for FFT Amplitude info service */
  BleCharPointer = &BleCharFFTAmplitude;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_FFTAmplitude;
  COPY_FFT_AMPLITUDE_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = 20;
  BleCharPointer->Char_Properties = CHAR_PROP_NOTIFY;
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 1;

  BLE_MANAGER_PRINTF("BLE FFT Amplitude features ok\r\n");

  return BleCharPointer;
}

/*
 * @brief  Update FFT Amplitude characteristic value
 * @param  uint8_t *DataToSend:
 *                  - 2 bytes for number of samples
 *                  - 1 byte for number of components (up to 3 components)
 *                  - 4 bytes for frequency steps
 *                  - 4 bytes for each component
 *                     (if number of components is more 1, for example 3, send the data in this format:
 *                      X1,X2,X3,...Xn,Y1,Y2,Y3,...Yn,X1,Z2,Z3,...Zn)
 * @param  uint16_t DataNumber Number of samples
 * @param  uint8_t *SendingFFT
 * @param  uint16_t *CountSendData
 * @retval tBleStatus   Status
 */
tBleStatus BLE_FFTAmplitudeUpdate(uint8_t *DataToSend, uint16_t DataNumber, uint8_t *SendingFFT,
                                  uint16_t *CountSendData)
{
  tBleStatus ret;

  uint16_t TotalSize;

  uint16_t index;
  uint16_t indexStart;
  uint16_t indexStop;

  uint8_t Buff[20];

  uint8_t  NumByteSent;

  /* nSample + nComponents + Frequency Steps + Samples */
  TotalSize = 2U + 1U + 4U + ((DataToSend[2] * DataNumber) * 4U) ;

  indexStart = 20U * (*CountSendData);
  indexStop =  20U * ((*CountSendData) + 1U);

  NumByteSent = 20;

  if (indexStop > TotalSize)
  {
    indexStop = TotalSize;
    NumByteSent = (uint8_t)(TotalSize % NumByteSent);
  }

  for (index = indexStart; index < indexStop; index++)
  {
    Buff[index - indexStart] = DataToSend[index];
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharFFTAmplitude, 0, NumByteSent, Buff);

  if (ret == (tBleStatus)BLE_STATUS_SUCCESS)
  {
    (*CountSendData)++;

    if (indexStop == TotalSize)
    {
      *SendingFFT = 0;
      *CountSendData = 0;
    }
  }

  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if FFT Amplitude is subscribed or not to the one service
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
static void AttrMod_Request_FFTAmplitude(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                         uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventFFT_Amplitude != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventFFT_Amplitude(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventFFT_Amplitude(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventFFT_Amplitude function Not Defined\r\n");
  }

  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t) sprintf((char *)BufferToWrite,
                                     "--->FFT Amplitude=%s\n",
                                     (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->FFT Amplitude=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* BLE_DEBUG_LEVEL>1 */
}

