/**
  ******************************************************************************
  * @file    BLE_QVAR.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add QVAR info services using vendor specific profiles.
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
#define COPY_QVAR_CHAR_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x16,\
                                                        0x00,0x02,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)


/* Exported variables --------------------------------------------------------*/
CustomNotifyEventQVAR_t CustomNotifyEventQVAR = NULL;


/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for inertial info service */
static BleCharTypeDef BleCharQVAR;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_QVAR(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                 uint8_t *att_data);

/**
  * @brief  Init QVAR info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for QVAR info service
  */
BleCharTypeDef *BLE_InitQVARService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer = NULL;


  /* Init data structure pointer for inertial info service */
  BleCharPointer = &BleCharQVAR;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_QVAR;
  COPY_QVAR_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length =  2 + 4 + 1 + 4 + 4;
  BleCharPointer->Char_Properties = CHAR_PROP_NOTIFY;
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 1;

  BLE_MANAGER_PRINTF("BLE QVAR features ok\r\n");

  return BleCharPointer;
}


/**
  * @brief  Update The QVAR Char
  * @param  int32_t QVAR Value [LSB]
  * @param  uint8_t Flag (to be defined)
  * @param  int32_t DQVAR Value [LSB]
  * @param  uint32_t Parameter (to be defined)
  * @param  int32_t NumberElement Number of elements to send
  * @retval tBleStatus      Status
  */
tBleStatus BLE_QVARUpdate(int32_t QVAR, uint8_t Flag, int32_t DQVAR, uint32_t Param, int32_t NumberElement)
{
  tBleStatus ret;
  uint8_t BuffPos;

  uint8_t buff[2 + 4 + 1 + 4 + 4];

  /* Time Stamp */
  STORE_LE_16(buff, (HAL_GetTick() >> 3));
  BuffPos = 2U;

  STORE_LE_32(buff + BuffPos, QVAR);
  BuffPos += 4U;

  if (NumberElement > 1)
  {
    buff[2 + BuffPos] = Flag;
    BuffPos += 1U;
    if (NumberElement > 2)
    {
      STORE_LE_32(buff + BuffPos, DQVAR);
      BuffPos += 4U;
      if (NumberElement == 4)
      {
        STORE_LE_32(buff + BuffPos, Param);
        BuffPos += 4U;
      }
    }
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharQVAR, 0, BuffPos, buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (ret != (tBleStatus)BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      if (BLE_StdErr_Service == BLE_SERV_ENABLE)
      {
        BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating QVAR Char\n");
        Stderr_Update(BufferToWrite, BytesToWrite);
      }
      else
      {
        BLE_MANAGER_PRINTF("Error: Updating QVAR Char ret=%x\r\n", ret);
      }
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: UpdatingQVARg Char ret=%x\r\n", ret);
    }
  }

  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if inertial is subscribed or not to the one service
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
static void AttrMod_Request_QVAR(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                 uint8_t *att_data)
{
  if (CustomNotifyEventQVAR != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventQVAR(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventQVAR(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventQVAR function Not Defined\r\n");
  }

  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "--->QVAR=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->QVAR=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

