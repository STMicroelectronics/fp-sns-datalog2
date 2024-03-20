/**
  ******************************************************************************
  * @file    BLE_NeaiAnomalyDetection.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add Anomaly Detection info services using vendor specific profiles.
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
#define COPY_NEAI_ANOMALYDETECTION_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x19,0x00,0x02,\
                                                                        0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,\
                                                                        0xc5,0x1b)

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventNeaiAnomalyDetection_t  CustomNotifyEventAD = NULL;
CustomWriteRequestAnomalyDetection_t CustomWriteRequestAD = NULL;
/* Private Types ----------------------------------------------------------- */

/* Private variables ---------------------------------------------------------*/

/* Data structure pointer for anomaly detection info service */
static BleCharTypeDef ADBleChar;
/* Size for AD characteristic */
static uint8_t  ADCharSize;
/* TODO: How many AD libraries you want manage? */
/*static uint8_t activeLibraries; */

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_AD(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                               uint8_t *att_data);
static void Write_Request_AD(void *BleCharPointer, uint16_t handle, uint16_t Offset, uint8_t data_length,
                             uint8_t *att_data);

BleCharTypeDef *BLE_InitADService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer = NULL;
  ADCharSize = 9;

  /* Init data structure pointer for anomaly detection info service */
  BleCharPointer = &ADBleChar;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_AD;
  BleCharPointer->Write_Request_CB = Write_Request_AD;
  COPY_NEAI_ANOMALYDETECTION_CHAR_UUID((BleCharPointer->uuid));

  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = ADCharSize;
  BleCharPointer->Char_Properties = ((uint8_t)(CHAR_PROP_NOTIFY)) | ((uint8_t)(CHAR_PROP_WRITE_WITHOUT_RESP));
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_ATTRIBUTE_WRITE;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 0;

  if (CustomWriteRequestAD == NULL)
  {
    BLE_MANAGER_PRINTF("CustomWriteRequestAD function Not Defined\r\n");
  }

  BLE_MANAGER_PRINTF("BLE NEAI Anomaly Detection char is ok\r\n");

  return BleCharPointer;
}

/**
  * @brief  Update NEAI AD characteristic value
  * @param  BLE_AD_output_t output contains info about:
  * - phase = (idle=0x00) | (learning=0x01) | (detecting=0x02)
  * - state = (NEAI_OK=0x00) | .....
  * - progress = from 0x00 to 0x64 (completion percentage)
  * - status = (normal=0x00) | (anomaly=0x01)
  * - similarity = from 0x00 to 0x64 (inference probability)
  *
  * ONLY THE PHASE FIELD IS MANDATORY
  * if you don't want use one of the others info, you can put NEAI_AD_ESCAPE
  *
  * @retval tBleStatus:          Status
  */
tBleStatus BLE_NeaiAnomalyDetectionUpdate(BLE_AD_output_t output)
{

  tBleStatus ret;

  /* Execution Time + Phase + State + Phase Progress + Status + Similarity */
  uint8_t buff[4 + 1 + 1 + 1 + 1 + 1 ];

  buff[0] = NEAI_AD_ESCAPE;
  buff[1] = NEAI_AD_ESCAPE;
  buff[2] = NEAI_AD_ESCAPE;
  buff[3] = NEAI_AD_ESCAPE;
  buff[4] = (uint8_t)output.phase;
  buff[5] = (uint8_t)output.state;
  buff[6] = output.progress;
  buff[7] = output.status;
  buff[8] = output.similarity;

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ADBleChar, 0, ADCharSize, buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating NEAI Anomaly Detection Char\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating NEAI Anomaly Detection Char\r\n");
    }
  }
  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Piano is subscribed or not to the one service
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

static void AttrMod_Request_AD(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset, uint8_t data_length,
                               uint8_t *att_data)
{
  if (CustomNotifyEventAD != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventAD(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventAD(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventAD function Not Defined\r\n");
  }

  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "--->NEAI_AD=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->NEAI_AD=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**
  * @brief  This event is given when a write request is received by the server from the client.
  * @param  void *VoidCharPointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
static void Write_Request_AD(void *BleCharPointer, uint16_t handle, uint16_t Offset, uint8_t data_length,
                             uint8_t *att_data)
{
  if (CustomWriteRequestAD != NULL)
  {
    CustomWriteRequestAD(att_data, data_length);
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nWrite request AD not defined\r\n\n");
  }
}
