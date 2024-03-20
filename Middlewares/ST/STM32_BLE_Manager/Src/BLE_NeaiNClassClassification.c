/**
  ******************************************************************************
  * @file    BLE_NeaiNClassClassification.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   NEAI Classification info services APIs.
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
#define COPY_NEAI_CLASSIFICATION_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x1a,0x00,0x02,0x11,\
                                                                      0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventNeaiClassification_t  CustomNotifyEventNCC = NULL;
CustomWriteRequestClassification_t    CustomWriteRequestNCC = NULL;
/* Private Types ----------------------------------------------------------- */

/* Private variables ---------------------------------------------------------*/

/* Data structure pointer for NEAI Classification info service */
static BleCharTypeDef NccBleChar;
/* Size for Classification Characteristic */
static uint8_t  NccCharMaxSize;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_Neai_Classification(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                                uint8_t data_length, uint8_t *att_data);
static void Write_Request_Neai_Classification(void *BleCharPointer, uint16_t handle, uint16_t Offset,
                                              uint8_t data_length, uint8_t *att_data);

BleCharTypeDef *BLE_InitNeaiClassificationService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer = NULL;
  NccCharMaxSize = 8U + CLASS_NUMBER_NCC;

  /* Init data structure pointer for N-Class Classification info service */
  BleCharPointer = &NccBleChar;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_Neai_Classification;
  BleCharPointer->Write_Request_CB = Write_Request_Neai_Classification;
  COPY_NEAI_CLASSIFICATION_CHAR_UUID((BleCharPointer->uuid));

  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = NccCharMaxSize;
  BleCharPointer->Char_Properties = ((uint8_t)(CHAR_PROP_NOTIFY)) | ((uint8_t)(CHAR_PROP_WRITE_WITHOUT_RESP));
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_ATTRIBUTE_WRITE;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 1;

  if (CustomWriteRequestNCC == NULL)
  {
    BLE_MANAGER_PRINTF("CustomWriteRequestNeaiClassification function Not Defined\r\n");
  }

  BLE_MANAGER_PRINTF("BLE NEAI Classification char is ok\r\n");

  return BleCharPointer;
}

/**
  * @brief  Update NEAI Classification Characteristic Value
  * @param  BLE_1CC_output_t output contains info about:
  * - phase = (idle=0x00) | (classification=0x01)
  * - state = (NEAI_OK=0x00) | .....
  * - is_outlier = 0x01 if it is an outlier, 0x00 otherwise
  *
  * ONLY THE PHASE FIELD IS MANDATORY
  *
  * @retval tBleStatus:          Status
  */
tBleStatus BLE_Neai1ClassClassificationUpdate(BLE_1CC_output_t output)
{

  tBleStatus ret;
  uint8_t char_length = 6;
  uint8_t buff[/*Execution Time*/ 4 + /* Type Selector */ 1 + /*Phase*/ 1 + /* State*/ 1 + /* Is Outlier */ 1];

  buff[0] = NEAI_NCC_ESCAPE;
  buff[1] = NEAI_NCC_ESCAPE;
  buff[2] = NEAI_NCC_ESCAPE;
  buff[3] = NEAI_NCC_ESCAPE;
  buff[4] = (uint8_t)output.sel;
  buff[5] = (uint8_t)output.phase;

  if (output.phase == NEAI_NCC_PHASE_CLASSIFICATION)
  {
    buff[6] = (uint8_t)output.state;
    buff[7] = (uint8_t)output.is_outlier;
    char_length += 2U;
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&NccBleChar, 0, char_length, buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating NEAI Classification Char\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating NEAI Classification Char\r\n");
    }
  }
  return ret;
}


/**
  * @brief  Update NEAI Classification Characteristic Value
  * @param  BLE_NCC_output_t output contains info about:
  * - phase = (idle=0x00) | (classification=0x01)
  * - state = (NEAI_OK=0x00) | .....
  * - major class = from 0x00 to CLASS_NUMBER_NCC
  * - probabilities = array with size equals to CLASS_NUMBER_NCC that contains class probabilities
  *
  * ONLY THE PHASE FIELD IS MANDATORY
  *
  * @retval tBleStatus:          Status
  */
tBleStatus BLE_NeaiNClassClassificationUpdate(BLE_NCC_output_t output)
{

  tBleStatus ret;
  uint8_t char_length = 6;
  uint8_t prob;
  uint8_t i;
  float temp;

  /* Execution Time + Type Selector + Phase + State + Most Probable Class + Classes Probability */
  uint8_t buff[ 4U + 1U + 1U + 1U + 1U + CLASS_NUMBER_NCC ];

  buff[0] = NEAI_NCC_ESCAPE;
  buff[1] = NEAI_NCC_ESCAPE;
  buff[2] = NEAI_NCC_ESCAPE;
  buff[3] = NEAI_NCC_ESCAPE;
  buff[4] = (uint8_t)output.sel;
  buff[5] = (uint8_t)output.phase;

  if (output.phase == NEAI_NCC_PHASE_CLASSIFICATION)
  {
    buff[6] = (uint8_t)output.state;
    buff[7] = output.most_probable_class;

    for (prob = 8, i = 0; prob < ((uint8_t)(8U + CLASS_NUMBER_NCC)); prob++, i++)
    {
      temp = (float)100.0 * output.probabilities[i];
      buff[prob] = (uint8_t)(((int32_t)temp));
    }
    char_length = char_length + (2U + CLASS_NUMBER_NCC);
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&NccBleChar, 0, char_length, buff);

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating NEAI Classification Char\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating NEAI Classification Char\r\n");
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

static void AttrMod_Request_Neai_Classification(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                                uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventNCC != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventNCC(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventNCC(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventNCC function Not Defined\r\n");
  }

  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "--->NEAI_NCC=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->NEAI_NCC=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**
  * @brief  This event is given when a write request is received by the server from the client.
  * @param  void *VoidCharPointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
static void Write_Request_Neai_Classification(void *BleCharPointer, uint16_t handle, uint16_t Offset,
                                              uint8_t data_length, uint8_t *att_data)
{
  if (CustomWriteRequestNCC != NULL)
  {
    CustomWriteRequestNCC(att_data, data_length);
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nWrite request NCC not defined\r\n\n");
  }
}
