/**
  ******************************************************************************
  * @file    BLE_MedicalSignal16Bit.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.11.0
  * @date    15-February-2024
  * @brief   Add Medical Signal 16 Bits service using vendor specific profiles.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#define COPY_MEDICAL_SIGNAL_16BIT_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x26,0x00,0x02,\
                                                                0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)


/* Exported variables --------------------------------------------------------*/
CustomNotifyEventMedicalSignal16Bit_t CustomNotifyEventMedicalSignal16Bit = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Medical Signal 16 Bits service */
static BleCharTypeDef BleMedicalSignal16Bit;

static uint16_t MedicalSignal16BitMaxCharLength = DEFAULT_MAX_MEDICAL_SIGNAL_16BIT_CHAR_LEN;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_MedicalSignal16Bit(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                          uint8_t data_length, uint8_t *att_data);

/**
  * @brief  Init Medical Signal 16 Bits service
  * @param  uint16_t MaxLenght Max Char length
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for char service
  */
BleCharTypeDef *BLE_InitMedicalSignal16BitService(uint16_t MaxLenght)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for char info service */
  BleCharPointer = &BleMedicalSignal16Bit;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_MedicalSignal16Bit;
  COPY_MEDICAL_SIGNAL_16BIT_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = MaxLenght;
  BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY);
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 1;
  
  MedicalSignal16BitMaxCharLength = MaxLenght;

  BLE_MANAGER_PRINTF("BLE MedicalSig16Bis feature ok\r\n");

  return BleCharPointer;
}


/**
  * @brief  Update Medical Signal 16 Bits characteristic
  * @param  BLE_MedicalSignal16Bit_t DataTypeID Signal Data Type ID
  * @param  uint32_t timeStamp timeStamp
  * @param  int32_t SampleDataSize Total Number of Values
  * @param  int16_t *SampleData  Values array
  * @retval tBleStatus   Status
  */
tBleStatus BLE_MedicalSignal16BitUpdate(BLE_MedicalSignal16Bit_t DataTypeID, uint32_t timeStamp, int32_t SampleDataSize, int16_t *SampleData)
{
  tBleStatus ret;
  uint8_t buff[DEFAULT_MAX_MEDICAL_SIGNAL_16BIT_CHAR_LEN];
  int32_t Counter;
  int32_t TotalSize = (SampleDataSize<<1) + 4 /* TimeStamp */ + 1 /* DataTypeID */;

  if( TotalSize> MedicalSignal16BitMaxCharLength) {
    BLE_MANAGER_PRINTF("BLE MedicalSig16Bits Error ... Too many Bytes %d>%d\r\n",TotalSize,MedicalSignal16BitMaxCharLength);
    return (tBleStatus) BLE_ERROR_LIMIT_REACHED;
  }
  
  STORE_LE_32(buff, timeStamp);
  buff[4] = (uint8_t) DataTypeID;
  for(Counter=0; Counter<SampleDataSize; Counter++) {   
    STORE_LE_16(buff+5+(Counter<<1), SampleData[Counter]);
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleMedicalSignal16Bit, 0,TotalSize, buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating MedicalSig16Bis Char\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating MedicalSig16Bis Char\r\n");
    }
  }
  return ret;
}


/**
  * @brief  Medical Signal 16 Bits Get Max Char Length
  * @param  None
  * @retval uint16_t MaxCharLength
  */
uint16_t BLE_MedicalSignal16BitGetMaxCharLength(void)
{
  return MedicalSignal16BitMaxCharLength;
}

/**
  * @brief  Medical Signal 16 Bits Set Max Char Length
  * @param  uint16_t MaxCharLength
  * @retval none
  */
void BLE_MedicalSignal16BitSetMaxCharLength(uint16_t MaxCharLength)
{
  MedicalSignal16BitMaxCharLength = MaxCharLength;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if E-Compass is subscribed or not to the one service
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
static void AttrMod_Request_MedicalSignal16Bit(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                          uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventMedicalSignal16Bit != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventMedicalSignal16Bit(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventMedicalSignal16Bit(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyMedicalSignal16Bit function Not Defined\r\n");
  }

  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t) sprintf((char *)BufferToWrite,
                                     "--->MedicalSignal16Bit=%s\n",
                                     (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->MedicalSignal16Bit=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

