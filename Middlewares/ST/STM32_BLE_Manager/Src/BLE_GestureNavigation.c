/**
  ******************************************************************************
  * @file    BLE_GestureNavigation.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add Gesture Navigation service using vendor specific profiles.
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
#define COPY_GESTURE_NAVIGATION_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x1F,0x00,0x02,0x11,\
                                                                     0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventGestureNavigation_t CustomNotifyEventGestureNavigation = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Gesture Navigation service */
static BleCharTypeDef BleGestureNavigation;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_GestureNavigation(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                              uint8_t data_length, uint8_t *att_data);

/**
  * @brief  Init Gesture Navigation service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Gesture Navigation service
  */
BleCharTypeDef *BLE_InitGestureNavigationService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for Gesture Navigation info service */
  BleCharPointer = &BleGestureNavigation;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_GestureNavigation;
  COPY_GESTURE_NAVIGATION_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = 2 + 1 + 1; /* 2 byte timestamp, 1 byte Gesture Code, 1 byte Button Code */
  BleCharPointer->Char_Properties = CHAR_PROP_NOTIFY;
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 0;

  BLE_MANAGER_PRINTF("BLE Gesture Navigation features ok\r\n");

  return BleCharPointer;
}

/**
  * @brief  Update Gesture Navigation characteristic
  * @param  BLE_GestureNavigation_t GestureNavigation Gesture Navigation Code
  * @param  BLE_ButtonNavigation_t ButtonNavigation Button Navigation Code
  * @retval tBleStatus   Status
  */
tBleStatus BLE_GestureNavigationUpdate(BLE_GestureNavigation_t GestureNavigation,
                                       BLE_ButtonNavigation_t ButtonNavigation)
{
  tBleStatus ret;
  uint8_t buff[2 + 1 + 1];

  STORE_LE_16(buff, (HAL_GetTick() >> 3));
  buff[2] = (uint8_t)GestureNavigation;
  buff[3] = (uint8_t)ButtonNavigation;

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleGestureNavigation, 0, 2 + 1 + 1, buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating Gesture Navigation Char\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Gesture Navigation Char\r\n");
    }
  }
  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Gesture Navigation is subscribed or not to the one service
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
static void AttrMod_Request_GestureNavigation(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                              uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventGestureNavigation != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventGestureNavigation(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventGestureNavigation(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventGestureNavigation function Not Defined\r\n");
  }

  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t) sprintf((char *)BufferToWrite, "--->Gest Nav=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->Gest Nav=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}
