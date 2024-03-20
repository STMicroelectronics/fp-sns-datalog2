/**
  ******************************************************************************
  * @file    BLE_FitnessActivities.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add Fitness Activities service using vendor specific profiles.
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
#define COPY_FITNESS_ACTIVITIES_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x0E,0x00,0x02,0x11,\
                                                                     0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventFitnessActivities_t CustomNotifyEventFitnessActivities = NULL;
CustomWriteRequestFitnessActivities_t CustomWriteRequestFitnessActivities = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Fitness Activities service */
static BleCharTypeDef BleCharFitnessActivities;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_FitnessActivities(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                              uint8_t data_length, uint8_t *att_data);
static void Write_Request_FitnessActivities(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                            uint8_t data_length, uint8_t *att_data);

/**
  * @brief  Init Fitness Activities service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer forFitness Activities service
  */
BleCharTypeDef *BLE_InitFitnessActivitiesService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for Fitness Activities info service */
  BleCharPointer = &BleCharFitnessActivities;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_FitnessActivities;
  BleCharPointer->Write_Request_CB = Write_Request_FitnessActivities;
  COPY_FITNESS_ACTIVITIES_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = 2 + 1 + 2; /* 2 byte timestamp, 1 byte action, 2 byte counter */
  BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_WRITE);
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = ((uint8_t)GATT_NOTIFY_ATTRIBUTE_WRITE) |
                                  ((uint8_t)GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP);
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 1;

  if (CustomWriteRequestFitnessActivities == NULL)
  {
    BLE_MANAGER_PRINTF("Warning: Write request Fitness Activities function not defined\r\n");
  }

  BLE_MANAGER_PRINTF("BLE Fitness Activities features ok\r\n");

  return BleCharPointer;
}

/**
  * @brief  Update Fitness Activities characteristic
  * @param  uint8_t MotionCode Detected Motion
  * @retval tBleStatus   Status
  */
tBleStatus BLE_FitnessActivitiesUpdate(uint8_t Activity, uint32_t Counter)
{
  tBleStatus ret;
  uint8_t buff[2 + 1 + 2];

  STORE_LE_16(buff, (HAL_GetTick() >> 3));
  buff[2] = (uint8_t)Activity;
  STORE_LE_16(buff + 3, Counter);

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharFitnessActivities, 0, 2 + 1 + 2, buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating FitnessActivities Char\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating FitnessActivities Char\r\n");
    }
  }
  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Fitness Activities is subscribed or not to the one service
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
static void AttrMod_Request_FitnessActivities(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                              uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventFitnessActivities != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventFitnessActivities(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventFitnessActivities(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventFitnessActivities function Not Defined\r\n");
  }

  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t) sprintf((char *)BufferToWrite,
                                     "--->Fitness Act=%s\n",
                                     (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->Fitness Act=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *VoidCharPointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
static void Write_Request_FitnessActivities(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                            uint8_t data_length, uint8_t *att_data)
{
#if (BLE_DEBUG_LEVEL>1)
  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = sprintf((char *)BufferToWrite, "--->FitnessActivitieSelected= %d\r\n", att_data[0]);
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
    BLE_MANAGER_PRINTF("--->FitnessActivitieSelected= %d\r\n", att_data[0]);
#endif /* (BLE_DEBUG_LEVEL>1) */

  if (CustomWriteRequestFitnessActivities != NULL)
  {
    CustomWriteRequestFitnessActivities(att_data[0]);
  }
}


