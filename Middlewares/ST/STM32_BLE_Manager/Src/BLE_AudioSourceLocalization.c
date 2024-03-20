/**
  ******************************************************************************
  * @file    BLE_AudioSourceLocalization.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add BLE_AudioSourceLocalization service using vendor specific profiles.
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
#define COPY_AUDIO_SOURCE_LOCALIZATION_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x10,0x00,0x00,0x00,\
                                                                            0x00,0x01,0x11,0xe1,0xac,0x36,0x00,\
                                                                            0x02,0xa5,0xd5,0xc5,0x1b)

#define AUDIO_SOURCE_LOCALIZATION_ADVERTISE_DATA_POSITION  18

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventAudioSourceLocalization_t CustomNotifyEventAudioSourceLocalization = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Audio Source Localization service */
static BleCharTypeDef BleAudioSourceLocalization;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_AudioSourceLocalization(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                                    uint8_t data_length, uint8_t *att_data);

/**
  * @brief  Init Audio Source Localization service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Audio Source Localization service
  */
BleCharTypeDef *BLE_InitAudioSourceLocalizationService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for Audio Source Localization info service */
  BleCharPointer = &BleAudioSourceLocalization;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_AudioSourceLocalization;
  COPY_AUDIO_SOURCE_LOCALIZATION_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = 2 + 2; /* 2 byte timestamp, 2 byte angle */
  BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY);
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 0;

  BLE_MANAGER_PRINTF("BLE Audio Source Localization features ok\r\n");

  return BleCharPointer;
}

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Audio Source Localization Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
void BLE_SetAudioSourceLocalizationAdvertiseData(uint8_t *manuf_data)
{
  manuf_data[AUDIO_SOURCE_LOCALIZATION_ADVERTISE_DATA_POSITION] |= 0x10U;
}
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Audio Source Localization characteristic
  * @param  uint16_t Angle Audio Source Localization evaluated angle
  * @retval tBleStatus   Status
  */
tBleStatus BLE_AudioSourceLocalizationUpdate(uint16_t Angle)
{
  tBleStatus ret;
  uint8_t buff[2 + 2];

  STORE_LE_16(buff, (HAL_GetTick() >> 3));
  STORE_LE_16(buff + 2, Angle);

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleAudioSourceLocalization, 0, 2 + 2, buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating Audio Source Localization Char\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Audio Source Localization Char\r\n");
    }
  }
  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Audio Source Localization
  *         is subscribed or not to the one service
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
static void AttrMod_Request_AudioSourceLocalization(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                                    uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventAudioSourceLocalization != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventAudioSourceLocalization(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventAudioSourceLocalization(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventAudioSourceLocalization function Not Defined\r\n");
  }

  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t) sprintf((char *)BufferToWrite,
                                     "\tAudioSrcLoc=%s\n",
                                     (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("\tAudioSrcLoc=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* BLE_DEBUG_LEVEL>1 */
}

