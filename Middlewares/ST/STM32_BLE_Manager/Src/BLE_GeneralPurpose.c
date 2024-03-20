/**
  ******************************************************************************
  * @file    BLE_GeneralPurpose.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add General Purpose info services using vendor specific profiles.
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
#define COPY_GP_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x00,\
                                                     0x00,0x03,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)


/* Exported variables --------------------------------------------------------*/
CustomNotifyEventGeneralPurpose_t CustomNotifyEventGeneralPurpose = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for General Purpose info service */
static BleCharTypeDef BleCharGeneralPurpose[BLE_GENERAL_PURPOSE_MAX_CHARS_NUM];

static int32_t NumberAllocatedGP = 0;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_GeneralPurpose(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                           uint8_t data_length, uint8_t *att_data);

/**
  * @brief  Init General Purpose info service
  * @param  uint8_t Size size of General Purpose char to allocate
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for General Purpose info service
  */
BleCharTypeDef *BLE_InitGeneralPurposeService(uint8_t Size)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;
  uint8_t GP_CharNum = (uint8_t)NumberAllocatedGP;

  /* Some Controls */
  if ((uint8_t)NumberAllocatedGP >= BLE_GENERAL_PURPOSE_MAX_CHARS_NUM)
  {
    BLE_MANAGER_PRINTF("Error GP_CharNum must be < %d\r\n", BLE_GENERAL_PURPOSE_MAX_CHARS_NUM);
    return NULL;
  }
  /* Increment the Number of Allocated General Purpose Features */
  NumberAllocatedGP++;

  /* Init data structure pointer for General Purpose info service */
  BleCharPointer = &BleCharGeneralPurpose[GP_CharNum];
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_GeneralPurpose;
  COPY_GP_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->uuid[14] = GP_CharNum;
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = Size;
  BleCharPointer->Char_Properties = (uint8_t)CHAR_PROP_NOTIFY;
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 0;

  BLE_MANAGER_PRINTF("BLE General Purpose feature [%d] ok\r\n", GP_CharNum);

  return BleCharPointer;
}


/**
  * @brief  Update General Purpose characteristic value
  * @param  uint8_t GP_CharNum General Purpose char number to update
  * @param  uint8_t *Data data to Update
  * @retval tBleStatus   Status
  */
tBleStatus BLE_GeneralPurposeStatusUpdate(uint8_t GP_CharNum, uint8_t *Data)
{
  tBleStatus ret;
  uint8_t buff[BLE_GENERAL_PURPOSE_MAX_CHARS_DIM];

  STORE_LE_16(buff, (HAL_GetTick() >> 3));
  memcpy(buff + 2, Data, (uint32_t)BleCharGeneralPurpose[GP_CharNum].Char_Value_Length - 2U);

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharGeneralPurpose[GP_CharNum],
                                   0,
                                   (uint8_t)BleCharGeneralPurpose[GP_CharNum].Char_Value_Length,
                                   buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating GP[%d] Char\n", GP_CharNum);
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating GP[%d] Char\r\n", GP_CharNum);
    }
  }
  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if led is subscribed or not to the one service
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
static void AttrMod_Request_GeneralPurpose(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                           uint8_t data_length, uint8_t *att_data)
{
  uint8_t GP_CharNum = BLE_GENERAL_PURPOSE_MAX_CHARS_NUM;

  if (CustomNotifyEventGeneralPurpose != NULL)
  {
    /* find the right GP feature */
    BleCharTypeDef *LocalBleChar = (BleCharTypeDef *)VoidCharPointer;
    uint32_t search;
    for (search = 0;
         ((search < (uint32_t)NumberAllocatedGP) && (GP_CharNum == BLE_GENERAL_PURPOSE_MAX_CHARS_NUM));
         search++)
    {
      if (LocalBleChar->uuid[14] == BleCharGeneralPurpose[search].uuid[14])
      {
        GP_CharNum = (uint8_t)search;
      }
    }

    if (GP_CharNum != BLE_GENERAL_PURPOSE_MAX_CHARS_NUM)
    {
      /* if we had found the corresponding General Purpose Feature */
      if (att_data[0] == 01U)
      {
        CustomNotifyEventGeneralPurpose(GP_CharNum, BLE_NOTIFY_SUB);
      }
      else if (att_data[0] == 0U)
      {
        CustomNotifyEventGeneralPurpose(GP_CharNum, BLE_NOTIFY_UNSUB);
      }
    }
  }

#if (BLE_DEBUG_LEVEL>1)
  if (GP_CharNum != BLE_GENERAL_PURPOSE_MAX_CHARS_NUM)
  {
    if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t) sprintf((char *)BufferToWrite,
                                       "--->GP[%d]=%s\n",
                                       GP_CharNum,
                                       (att_data[0] == 01U) ? " ON" : " OFF");
      Term_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("--->GP[%d]=%s", GP_CharNum, (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
    }
  }
  else
  {
    /* if we didn't find a suitable General Purpose char */
    if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t) sprintf((char *)BufferToWrite,
                                       "--->GP[Not Found]=%s\n",
                                       (att_data[0] == 01U) ? " ON" : " OFF");
      Term_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("--->GP[Not Found]=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
    }
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}


