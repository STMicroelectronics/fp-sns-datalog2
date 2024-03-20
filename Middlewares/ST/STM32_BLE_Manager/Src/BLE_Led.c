/**
  ******************************************************************************
  * @file    BLE_Led.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add led info services using vendor specific profiles.
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
#define COPY_LED_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x20,0x00,0x00,0x00,\
                                                      0x00,0x01,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define LED_ADVERTISE_DATA_POSITION  15

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventLed_t CustomNotifyEventLed = NULL;
CustomReadRequestLed_t CustomReadRequestLed = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for led info service */
static BleCharTypeDef BleCharLed;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_Led(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                uint8_t *att_data);
#if (BLUE_CORE != BLUENRG_LP)
static void Read_Request_Led(void *BleCharPointer, uint16_t handle);
#else /* (BLUE_CORE != BLUENRG_LP) */
static void Read_Request_Led(void *BleCharPointer,
                             uint16_t handle,
                             uint16_t Connection_Handle,
                             uint8_t Operation_Type,
                             uint16_t Attr_Val_Offset,
                             uint8_t Data_Length,
                             uint8_t Data[]);
#endif /* (BLUE_CORE != BLUENRG_LP) */

/**
  * @brief  Init led info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for led info service
  */
BleCharTypeDef *BLE_InitLedService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for led info service */
  BleCharPointer = &BleCharLed;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_Led;
  BleCharPointer->Read_Request_CB = Read_Request_Led;
  COPY_LED_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = 2 + 1;
  BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_READ);
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 0;

  if (CustomReadRequestLed == NULL)
  {
    BLE_MANAGER_PRINTF("Warning: Read request led function not defined\r\n");
  }

  BLE_MANAGER_PRINTF("BLE Led features ok\r\n");

  return BleCharPointer;
}

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Led Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
void BLE_SetLedAdvertiseData(uint8_t *manuf_data)
{
  /* Setting Led Advertise Data */
  manuf_data[LED_ADVERTISE_DATA_POSITION] |= 0x20U;
}
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update LEDs characteristic value
  * @param  uint8_t LedStatus LEDs status 0/1 (off/on)
  * @retval tBleStatus   Status
  */
tBleStatus BLE_LedStatusUpdate(uint8_t LedStatus)
{
  tBleStatus ret;
  uint8_t buff[2 + 1];

  STORE_LE_16(buff, (HAL_GetTick() >> 3));
  buff[2] = LedStatus;

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharLed, 0, 2 + 1, buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating Led Char\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Led Char\r\n");
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
static void AttrMod_Request_Led(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                uint8_t *att_data)
{
  if (CustomNotifyEventLed != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventLed(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventLed(BLE_NOTIFY_UNSUB);
    }
  }

#if (BLE_DEBUG_LEVEL>1)
  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t) sprintf((char *)BufferToWrite, "--->Led=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->Led=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *VoidCharPointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
#if (BLUE_CORE != BLUENRG_LP)
static void Read_Request_Led(void *VoidCharPointer, uint16_t handle)
{
  if (CustomReadRequestLed != NULL)
  {
    uint8_t LedStatus;
    CustomReadRequestLed(&LedStatus);
    BLE_LedStatusUpdate(LedStatus);
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nRead request Led function not defined\r\n\n");
  }
}
#else /* (BLUE_CORE != BLUENRG_LP) */
static void Read_Request_Led(void *BleCharPointer,
                             uint16_t handle,
                             uint16_t Connection_Handle,
                             uint8_t Operation_Type,
                             uint16_t Attr_Val_Offset,
                             uint8_t Data_Length,
                             uint8_t Data[])
{
  tBleStatus ret;
  if (CustomReadRequestLed != NULL)
  {
    uint8_t LedStatus;
    uint8_t buff[2 + 1];

    CustomReadRequestLed(&LedStatus);

    STORE_LE_16(buff, (HAL_GetTick() >> 3));
    buff[2] = LedStatus;

    ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, 2 + 1, buff);
    if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
    {
      if (BLE_StdErr_Service == BLE_SERV_ENABLE)
      {
        BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating Led Char\n");
        Stderr_Update(BufferToWrite, BytesToWrite);
      }
      else
      {
        BLE_MANAGER_PRINTF("Error: Updating Led Char\r\n");
      }
    }
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nRead request Led function not defined\r\n\n");
  }

  ret = aci_gatt_srv_authorize_resp_nwk(Connection_Handle, handle,
                                        Operation_Type, 0, Attr_Val_Offset,
                                        Data_Length, Data);
  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("aci_gatt_srv_authorize_resp_nwk() failed: 0x%02x\r\n", ret);
  }
}
#endif /* (BLUE_CORE != BLUENRG_LP) */

