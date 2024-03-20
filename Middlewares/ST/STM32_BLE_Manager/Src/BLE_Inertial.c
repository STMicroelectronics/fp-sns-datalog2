/**
  ******************************************************************************
  * @file    BLE_Inertial.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add inertial info services using vendor specific profiles.
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
#define COPY_INERTIAL_CHAR_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x00,\
                                                            0x00,0x01,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define INERTIAL_ADVERTISE_DATA_POSITION  16

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventInertial_t CustomNotifyEventInertial = NULL;

/* Private Types ----------------------------------------------------------- */
typedef struct
{
  uint8_t AccIsEnable;
  uint8_t GyroIsEnable;
  uint8_t MagIsEnabled;
} BLE_Manager_InertialFeaturesEnabled_t;

/* Private variables ---------------------------------------------------------*/
/* Data structure for identify inertial info services enabled */
BLE_Manager_InertialFeaturesEnabled_t InertialFeaturesEnabled;
/* Data structure pointer for inertial info service */
static BleCharTypeDef BleCharInertial;
/* Size for inertial BLE characteristic */
static uint8_t  InertialCharSize;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_Inertial(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                     uint8_t *att_data);

/**
  * @brief  Init inertial info service
  * @param  uint8_t AccEnable:   1 for enabling the BLE accelerometer feature, 0 otherwise.
  * @param  uint8_t GyroEnable:  1 for enabling the BLE gyroscope feature, 0 otherwise.
  * @param  uint8_t MagEnabled:  1 for esabling the BLE magnetometer feature, 0 otherwise.
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for environmental info service
  */
BleCharTypeDef *BLE_InitInertialService(uint8_t AccEnable, uint8_t GyroEnable, uint8_t MagEnabled)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer = NULL;
  InertialCharSize = 2;

  /* Init data structure for identify environmental info services enabled */
  InertialFeaturesEnabled.AccIsEnable = AccEnable;
  InertialFeaturesEnabled.GyroIsEnable = GyroEnable;
  InertialFeaturesEnabled.MagIsEnabled = MagEnabled;

  if ((AccEnable == 1U) ||
      (GyroEnable == 1U)   ||
      (MagEnabled == 1U))
  {
    /* Init data structure pointer for inertial info service */
    BleCharPointer = &BleCharInertial;
    memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
    BleCharPointer->AttrMod_Request_CB = AttrMod_Request_Inertial;
    COPY_INERTIAL_CHAR_UUID((BleCharPointer->uuid));

    /* Enables BLE accelerometer feature */
    if (AccEnable == 1U)
    {
      BleCharPointer->uuid[14] |= 0x80U;
      InertialCharSize += 3U * 2U;
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("\t--> Accelerometer feature enabled\r\n");
#endif /* (BLE_DEBUG_LEVEL>1) */
    }

    /* Enables BLE gyroscope feature */
    if (GyroEnable == 1U)
    {
      BleCharPointer->uuid[14] |= 0x40U;
      InertialCharSize += 3U * 2U;
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("\t--> Gyroscope feature enabled\r\n");
#endif /* (BLE_DEBUG_LEVEL>1) */
    }

    /* Enables BLE magnetometer feature */
    if (MagEnabled == 1U)
    {
      BleCharPointer->uuid[14] |= 0x20U;
      InertialCharSize += 3U * 2U;
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("\t--> Magnetometer feature enabled\r\n");
#endif /* (BLE_DEBUG_LEVEL>1) */
    }

    BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
    BleCharPointer->Char_Value_Length = InertialCharSize;
    BleCharPointer->Char_Properties = CHAR_PROP_NOTIFY;
    BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
    BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
    BleCharPointer->Enc_Key_Size = 16;
    BleCharPointer->Is_Variable = 0;

    BLE_MANAGER_PRINTF("BLE Inertial features ok\r\n");
  }
  else
  {
    BLE_MANAGER_PRINTF(" ERROR: None inertial features is enabled\r\n");
  }

  return BleCharPointer;
}

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Inertial Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
void BLE_SetInertialAdvertiseData(uint8_t *manuf_data)
{
  /* Setting Accelerometer Advertise Data */
  if (InertialFeaturesEnabled.AccIsEnable == 1U)
  {
    manuf_data[INERTIAL_ADVERTISE_DATA_POSITION] |= 0x80U;
  }

  /* Setting Gyroscope Advertise Data */
  if (InertialFeaturesEnabled.GyroIsEnable == 1U)
  {
    manuf_data[INERTIAL_ADVERTISE_DATA_POSITION] |= 0x40U;
  }

  /* Setting Magnetometer Advertise Data */
  if (InertialFeaturesEnabled.MagIsEnabled == 1U)
  {
    manuf_data[INERTIAL_ADVERTISE_DATA_POSITION] |= 0x20U;
  }
}
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update acceleration/Gryoscope and Magneto characteristics value
  * @param  BLE_MANAGER_INERTIAL_Axes_t Acc:     Structure containing acceleration value in mg
  * @param  BLE_MANAGER_INERTIAL_Axes_t Gyro:    Structure containing Gyroscope value
  * @param  BLE_MANAGER_INERTIAL_Axes_t Mag:     Structure containing magneto value
  * @retval tBleStatus      Status
  */
tBleStatus BLE_AccGyroMagUpdate(BLE_MANAGER_INERTIAL_Axes_t *Acc,
                                BLE_MANAGER_INERTIAL_Axes_t *Gyro,
                                BLE_MANAGER_INERTIAL_Axes_t *Mag)
{
  tBleStatus ret;
  uint8_t BuffPos;

  uint8_t buff[2 + (3 * 2)/*Acc*/ + (3 * 2)/*Gyro*/ + (3 * 2)/*Mag*/];

  /* Time Stamp */
  STORE_LE_16(buff, (HAL_GetTick() >> 3));
  BuffPos = 2;

  if (InertialFeaturesEnabled.AccIsEnable == 1U)
  {
    STORE_LE_16(buff + BuffPos, ((uint16_t)Acc->Axis_x));
    BuffPos += 2U;
    STORE_LE_16(buff + BuffPos, ((uint16_t)Acc->Axis_y));
    BuffPos += 2U;
    STORE_LE_16(buff + BuffPos, ((uint16_t)Acc->Axis_z));
    BuffPos += 2U;
  }

  if (InertialFeaturesEnabled.GyroIsEnable == 1U)
  {
    Gyro->Axis_x /= 100;
    Gyro->Axis_y /= 100;
    Gyro->Axis_z /= 100;

    STORE_LE_16(buff + BuffPos, ((uint16_t)Gyro->Axis_x));
    BuffPos += 2U;
    STORE_LE_16(buff + BuffPos, ((uint16_t)Gyro->Axis_y));
    BuffPos += 2U;
    STORE_LE_16(buff + BuffPos, ((uint16_t)Gyro->Axis_z));
    BuffPos += 2U;
  }

  if (InertialFeaturesEnabled.MagIsEnabled == 1U)
  {
    STORE_LE_16(buff + BuffPos, ((uint16_t)Mag->Axis_x));
    BuffPos += 2U;
    STORE_LE_16(buff + BuffPos, ((uint16_t)Mag->Axis_y));
    BuffPos += 2U;
    STORE_LE_16(buff + BuffPos, ((uint16_t)Mag->Axis_z));
    BuffPos += 2U;
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharInertial, 0, InertialCharSize, buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (ret != (tBleStatus)BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      if (BLE_StdErr_Service == BLE_SERV_ENABLE)
      {
        BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating Acc/Gyro/Mag Char\n");
        Stderr_Update(BufferToWrite, BytesToWrite);
      }
      else
      {
        BLE_MANAGER_PRINTF("Error: Updating Acc/Gyro/Mag Char ret=%x\r\n", ret);
      }
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating Acc/Gyro/Mag Char ret=%x\r\n", ret);
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
static void AttrMod_Request_Inertial(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                                     uint8_t *att_data)
{
  if (CustomNotifyEventInertial != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventInertial(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventInertial(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventInertial function Not Defined\r\n");
  }

  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite,
                                    "--->Acc/Gyro/Mag=%s\n",
                                    (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->Acc/Gyro/Mag=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

