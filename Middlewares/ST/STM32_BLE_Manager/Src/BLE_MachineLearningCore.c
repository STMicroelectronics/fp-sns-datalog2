/**
  ******************************************************************************
  * @file    BLE_MachineLearningCore.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add Machine Learning Core info services using vendor specific
  *          profiles.
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
#define COPY_MACHINE_LEARNING_CORE_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x0F,0x00,0x02,\
                                                                        0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,\
                                                                        0xc5,0x1b)

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventMachineLearningCore_t CustomNotifyEventMachineLearningCore = NULL;
CustomReadMachineLearningCore_t CustomReadMachineLearningCore = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Machine Learning Core info service */
static BleCharTypeDef BleCharMachineLearningCore;

static BLE_MachineLearningCoreNumReg_t BleNumReg = BLE_MLC_8_REG;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_MachineLearningCore(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                                uint8_t data_length, uint8_t *att_data);

#if (BLUE_CORE != BLUENRG_LP)
static void Read_Request_MachineLearningCore(void *BleCharPointer, uint16_t handle);
#else /* (BLUE_CORE != BLUENRG_LP) */
static void Read_Request_MachineLearningCore(void *BleCharPointer,
                                             uint16_t handle,
                                             uint16_t Connection_Handle,
                                             uint8_t Operation_Type,
                                             uint16_t Attr_Val_Offset,
                                             uint8_t Data_Length,
                                             uint8_t Data[]);
#endif /* (BLUE_CORE != BLUENRG_LP) */

/**
  * @brief  Init Machine Learning Core info service
  * @param  BLE_MachineLearningCoreNumReg_t NumReg Number MLC registers
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Machine Learning Core info service
  */
BleCharTypeDef *BLE_InitMachineLearningCoreService(BLE_MachineLearningCoreNumReg_t NumReg)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for Machine Learning Core info service */
  BleCharPointer = &BleCharMachineLearningCore;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_MachineLearningCore;
  BleCharPointer->Read_Request_CB = Read_Request_MachineLearningCore;
  COPY_MACHINE_LEARNING_CORE_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;

  if (NumReg == BLE_MLC_8_REG)
  {
    /* 2 byte timestamp, 8 MLC registers output, 1 MCL output state */
    BleCharPointer->Char_Value_Length = 2 + 8 + 1;
  }
  else
  {
    /* 2 byte timestamp, 4 FSM registers output, 1 FSM output state */
    BleCharPointer->Char_Value_Length = 2 + 4 + 1;
  }

  BleNumReg = NumReg;

  BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_READ);
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 0;

  if (CustomReadMachineLearningCore == NULL)
  {
    BLE_MANAGER_PRINTF("Warning: Read request Machine Learning Core function not defined\r\n");
  }

  BLE_MANAGER_PRINTF("BLE MLC features ok\r\n");

  return BleCharPointer;
}

/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *VoidCharPointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
#if (BLUE_CORE != BLUENRG_LP)
static void Read_Request_MachineLearningCore(void *VoidCharPointer, uint16_t handle)
{
  if (CustomReadMachineLearningCore != NULL)
  {
    uint8_t mlc_out[8];
    uint8_t mlc_status_mainpage;
    CustomReadMachineLearningCore(mlc_out, &mlc_status_mainpage);
    BLE_MachineLearningCoreUpdate(mlc_out, &mlc_status_mainpage);
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nRead request MachineLearningCore function not defined\r\n\n");
  }
}
#else /* (BLUE_CORE != BLUENRG_LP) */
static void Read_Request_MachineLearningCore(void *BleCharPointer,
                                             uint16_t handle,
                                             uint16_t Connection_Handle,
                                             uint8_t Operation_Type,
                                             uint16_t Attr_Val_Offset,
                                             uint8_t Data_Length,
                                             uint8_t Data[])
{
  tBleStatus ret;
  if (CustomReadMachineLearningCore != NULL)
  {
    uint8_t mlc_out[8];
    uint8_t mlc_status_mainpage;
    /* 2 byte timestamp, 8 MLC registers output, 1 MCL output state */
    uint8_t dimByte;
    uint8_t buff[2 + 8 + 1];
    CustomReadMachineLearningCore(mlc_out, &mlc_status_mainpage);

    STORE_LE_16(buff, (HAL_GetTick() >> 3));

    if (BleNumReg == BLE_MLC_8_REG)
    {
      /* MLC outputs registers */
      memcpy(buff + 2, mlc_out, 8U * sizeof(uint8_t));

      /* Status bit for MLC from 1 to 8   */
      buff[10] = mlc_status_mainpage;
      dimByte = 2 + 8 + 1;
    }
    else
    {
      /* MLC outputs registers */
      memcpy(buff + 2, mlc_out, 4U * sizeof(uint8_t));

      /* Status bit for MLC from 1 to 4   */
      buff[6] = mlc_status_mainpage;
      dimByte = 2 + 4 + 1;
    }
    ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, dimByte, buff);
    if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
    {
      if (BLE_StdErr_Service == BLE_SERV_ENABLE)
      {
        BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating MachineLearningCore Char\n");
        Stderr_Update(BufferToWrite, BytesToWrite);
      }
      else
      {
        BLE_MANAGER_PRINTF("Error: Updating MachineLearningCore Char\r\n");
      }
    }
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nRead request MachineLearningCore function not defined\r\n\n");
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

/**
  * @brief  Update Machine Learning Core output registers characteristic
  * @param  uint8_t *mlc_out pointers to 8 MLC register
  * @param  uint8_t *mlc_status_mainpage pointer to the MLC status bits from 1 to 8
  * @retval tBleStatus Status
  */
tBleStatus BLE_MachineLearningCoreUpdate(uint8_t *mlc_out, uint8_t *mlc_status_mainpage)
{
  tBleStatus ret;

  /* 2 byte timestamp, 8 MLC registers output, 1 MCL output state */
  uint8_t buff[2 + 8 + 1];

  /* TimeStamp */
  STORE_LE_16(buff, (HAL_GetTick() >> 3));

  if (BleNumReg == BLE_MLC_8_REG)
  {
    /* MLC outputs registers */
    memcpy(buff + 2, mlc_out, 8U * sizeof(uint8_t));

    /* Status bit for MLC from 1 to 8   */
    buff[10] = *mlc_status_mainpage;

    ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharMachineLearningCore, 0, 2 + 8 + 1, buff);
  }
  else
  {
    /* MLC outputs registers */
    memcpy(buff + 2, mlc_out, 4U * sizeof(uint8_t));

    /* Status bit for MLC from 1 to 4   */
    buff[6] = *mlc_status_mainpage;

    ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharMachineLearningCore, 0, 2 + 4 + 1, buff);
  }

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating Machine Learning Core Char\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Machine Learning Core Char\r\n");
    }
  }
  return ret;
}

/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Machine Learning Core is subscribed
  *         or not to the one service
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
static void AttrMod_Request_MachineLearningCore(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                                uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventMachineLearningCore != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventMachineLearningCore(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventMachineLearningCore(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventMachineLearningCore function Not Defined\r\n");
  }
  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "--->MLC=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->MLC=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}


