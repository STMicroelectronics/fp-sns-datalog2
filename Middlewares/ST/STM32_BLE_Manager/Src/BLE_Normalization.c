/**
  ******************************************************************************
  * @file    BLE_Normalization.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add Event Counter service using vendor specific profiles.
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
#define COPY_EVENT_COUNTER_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x21,0x00,0x02,\
                                                                0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)


/* Exported variables --------------------------------------------------------*/
CustomNotifyEventNormalization_t CustomNotifyEventNormalization = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Event Counter service */
static BleCharTypeDef BleNormalization;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_Normalization(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                          uint8_t data_length, uint8_t *att_data);

/**
  * @brief  Init Event Counter service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for char service
  */
BleCharTypeDef *BLE_InitNormalizationService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for char info service */
  BleCharPointer = &BleNormalization;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_Normalization;
  COPY_EVENT_COUNTER_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = 2 + 4; /* 2 byte timestamp, 4 byte Event Counter Number */
  BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY);
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 0;

  BLE_MANAGER_PRINTF("BLE Normalization feature ok\r\n");

  return BleCharPointer;
}


/**
  * @brief  Update Event Counter characteristic
  * @param  float Normalization Counter Number
  * @retval tBleStatus   Status
  */
tBleStatus BLE_NormalizationUpdate(float Normalization)
{
  tBleStatus ret;
  uint8_t buff[2 + 4];
  int32_t intNormalization;

  intNormalization = (int32_t)(Normalization);   /* x1000 ??? mettersi d'accordo con luca */

  STORE_LE_16(buff, (HAL_GetTick() >> 3));
  STORE_LE_32(buff + 2, intNormalization);

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleNormalization, 0, 2 + 4, buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating Normalization Char\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Normalization Char\r\n");
    }
  }
  return ret;
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
static void AttrMod_Request_Normalization(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                          uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventNormalization != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventNormalization(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventNormalization(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyNormalization function Not Defined\r\n");
  }

  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t) sprintf((char *)BufferToWrite,
                                     "--->Normalization=%s\n",
                                     (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->Normalization=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

