/**
  ******************************************************************************
  * @file    BLE_SensorFusion.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add Sensor Fusion service using vendor specific profiles.
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
#define COPY_SENSOR_FUSION_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x01,0x00,0x00,0x01,\
                                                                0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define SENSOR_FUSION_ADVERTISE_DATA_POSITION  17

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventSensorFusion_t CustomNotifyEventSensorFusion = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Sensor Fusion service */
static BleCharTypeDef BleCharSensorFusion;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_SensorFusion(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                         uint8_t data_length, uint8_t *att_data);

/**
  * @brief  Init Sensor Fusion info service
  * @param  uint8_t NumberQuaternionsToSend  Number of quaternions send (1,2,3)
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Sensor Fusion info service
  */
extern BleCharTypeDef *BLE_InitSensorFusionService(uint8_t NumberQuaternionsToSend)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  uint8_t DataLenght = 2U + (6U * NumberQuaternionsToSend);

  if ((NumberQuaternionsToSend > 0U) && (NumberQuaternionsToSend <= 3U))
  {
    /* Init data structure pointer for Sensor Fusion info service */
    BleCharPointer = &BleCharSensorFusion;
    memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
    BleCharPointer->AttrMod_Request_CB = AttrMod_Request_SensorFusion;
    COPY_SENSOR_FUSION_CHAR_UUID((BleCharPointer->uuid));
    BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
    BleCharPointer->Char_Value_Length = DataLenght;
    BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY);
    BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
    BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
    BleCharPointer->Enc_Key_Size = 16;
    BleCharPointer->Is_Variable = 1;

    BLE_MANAGER_PRINTF("BLE Sensor Fusion features ok\r\n");
  }
  else
  {
    BLE_MANAGER_PRINTF("BLE Sensor Fusion features init error: Number Quaternions To Send not correct\r\n");
    BleCharPointer = NULL;
  }

  return BleCharPointer;
}

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Sensor Fusion Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
void BLE_SetSensorFusionAdvertiseData(uint8_t *manuf_data)
{
  manuf_data[SENSOR_FUSION_ADVERTISE_DATA_POSITION] |= 0x01U;
}
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update quaternions characteristic value
  * @param  BLE_MOTION_SENSOR_Axes_t *data Structure containing the quaterions
  * @param  uint8_t NumberQuaternionsToSend  Number of quaternions send (1,2,3)
  * @retval tBleStatus      Status
  */
tBleStatus BLE_SensorFusionUpdate(BLE_MOTION_SENSOR_Axes_t *data, uint8_t NumberQuaternionsToSend)
{
  tBleStatus ret;
  uint8_t dimByte;

  uint8_t buff[2U + (6U * 3U)];

  STORE_LE_16(buff, (HAL_GetTick() >> 3));

  switch (NumberQuaternionsToSend)
  {
    case 1:
      STORE_LE_16(buff + 2, data[0].Axis_x);
      STORE_LE_16(buff + 4, data[0].Axis_y);
      STORE_LE_16(buff + 6, data[0].Axis_z);
      break;
    case 2:
      STORE_LE_16(buff + 2, data[0].Axis_x);
      STORE_LE_16(buff + 4, data[0].Axis_y);
      STORE_LE_16(buff + 6, data[0].Axis_z);
      STORE_LE_16(buff + 8, data[1].Axis_x);
      STORE_LE_16(buff + 10, data[1].Axis_y);
      STORE_LE_16(buff + 12, data[1].Axis_z);
      break;
    case 3:
      STORE_LE_16(buff + 2, data[0].Axis_x);
      STORE_LE_16(buff + 4, data[0].Axis_y);
      STORE_LE_16(buff + 6, data[0].Axis_z);
      STORE_LE_16(buff + 8, data[1].Axis_x);
      STORE_LE_16(buff + 10, data[1].Axis_y);
      STORE_LE_16(buff + 12, data[1].Axis_z);
      STORE_LE_16(buff + 14, data[2].Axis_x);
      STORE_LE_16(buff + 16, data[2].Axis_y);
      STORE_LE_16(buff + 18, data[2].Axis_z);
      break;
  }

  dimByte = 2U + (6U * NumberQuaternionsToSend);
  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharSensorFusion, 0, dimByte, buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating Sensor Fusion Char\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Sensor Fusion Char\r\n");
    }
  }
  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Sensor Fusion is subscribed or not to the one service
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
static void AttrMod_Request_SensorFusion(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                         uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventSensorFusion != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventSensorFusion(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventSensorFusion(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventSensorFusion function Not Defined\r\n");
  }
  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite,
                                    "--->Sensor Fusion=%s\n",
                                    (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->Sensor Fusion=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

