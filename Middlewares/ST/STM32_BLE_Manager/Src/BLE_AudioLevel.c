/**
  ******************************************************************************
  * @file    BLE_AudioLevel.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add audio level info services using vendor specific profiles.
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
#define COPY_AUDIO_LEVEL_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x04,0x00,0x00,0x00,0x00,\
                                                              0x01,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define AUDIO_LEVEL_ADVERTISE_DATA_POSITION  15

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventAudioLevel_t CustomNotifyEventAudioLevel = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for audio level info service */
static BleCharTypeDef BleCharAudioLevel;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_AudioLevel(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                       uint8_t *att_data);

/**
  * @brief  Init audio level info service
  * @param  uint8_t AudioLevelNumber: Number of audio level features (up to 4 audio level are supported)
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for audio level info service
  */
BleCharTypeDef *BLE_InitAudioLevelService(uint8_t AudioLevelNumber)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  uint8_t TempResult = 2U + AudioLevelNumber;

  /* Init data structure pointer for audio level info service */
  BleCharPointer = &BleCharAudioLevel;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_AudioLevel;
  COPY_AUDIO_LEVEL_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = (uint16_t)TempResult;
  BleCharPointer->Char_Properties = CHAR_PROP_NOTIFY;
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 0;

  BLE_MANAGER_PRINTF("BLE Audio Level features ok\r\n");

  return BleCharPointer;
}

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Environmental Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
void BLE_SetAudioLevelAdvertiseData(uint8_t *manuf_data)
{
  /* Setting Audio Level Advertise Data */
  manuf_data[AUDIO_LEVEL_ADVERTISE_DATA_POSITION] |= 0x04U;
}
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update audio level characteristic values
  * @param  uint16_t *AudioLevelData:    SNR dB audio level array
  * @param  uint8_t AudioLevelNumber:    Number of audio level features (up to 4 audio level are supported)
  * @retval tBleStatus   Status
  */
tBleStatus BLE_AudioLevelUpdate(uint16_t *AudioLevelData, uint8_t AudioLevelNumber)
{
  tBleStatus ret;
  uint8_t Counter;

  uint8_t buff[2 /*Time Stamp*/ + 4 /*Max number audio level*/];

  /*Time Stamp*/
  STORE_LE_16(buff, (HAL_GetTick() >> 3));

  /* Audio Levels */
  for (Counter = 0; Counter < AudioLevelNumber; Counter++)
  {
    buff[2U + Counter] = (uint8_t)(AudioLevelData[Counter] & 0xFFU);
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharAudioLevel, 0, (2U + AudioLevelNumber), buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating Audio Level Data Char\r\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Audio Level Data Char\r\n");
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
static void AttrMod_Request_AudioLevel(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                       uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventAudioLevel != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventAudioLevel(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventAudioLevel(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventAudioLevel function Not Defined\r\n");
  }

  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t) sprintf((char *)BufferToWrite,
                                     "--->Audio Level=%s\n",
                                     (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->Audio Level=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

