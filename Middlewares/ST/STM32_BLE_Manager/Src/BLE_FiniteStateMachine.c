/**
  ******************************************************************************
  * @file    BLE_FiniteStateMachine.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Add Machine Learning info services using vendor specific
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
#define COPY_FINITE_STATE_MACHINE_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x10,0x00,0x02,0x11,\
                                                                       0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Exported variables --------------------------------------------------------*/
CustomNotifyEventFiniteStateMachine_t CustomNotifyEventFiniteStateMachine = NULL;
CustomReadFiniteStateMachine_t CustomReadFiniteStateMachine = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Finite State Machine info service */
static BleCharTypeDef BleCharFiniteStateMachine;

static BLE_FiniteStateMachineNumReg_t BleNumReg = BLE_FSM_16_REG;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_FiniteStateMachine(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                               uint8_t data_length, uint8_t *att_data);

#if (BLUE_CORE != BLUENRG_LP)
static void Read_Request_FiniteStateMachine(void *BleCharPointer, uint16_t handle);
#else /* (BLUE_CORE != BLUENRG_LP) */
static void Read_Request_FiniteStateMachine(void *BleCharPointer,
                                            uint16_t handle,
                                            uint16_t Connection_Handle,
                                            uint8_t Operation_Type,
                                            uint16_t Attr_Val_Offset,
                                            uint8_t Data_Length,
                                            uint8_t Data[]);
#endif /* (BLUE_CORE != BLUENRG_LP) */

/**
  * @brief  Init Finite State Machine info service
  * @param  BLE_FiniteStateMachineNumReg_t NumReg Number of Registers
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Machine Learning Core info service
  */
BleCharTypeDef *BLE_InitFiniteStateMachineService(BLE_FiniteStateMachineNumReg_t NumReg)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for Machine Learning Core info service */
  BleCharPointer = &BleCharFiniteStateMachine;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_FiniteStateMachine;
  BleCharPointer->Read_Request_CB = Read_Request_FiniteStateMachine;
  COPY_FINITE_STATE_MACHINE_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;

  if (NumReg == BLE_FSM_16_REG)
  {
    /* 2 byte timestamp, 16 FSM registers output, 2 FSM output state */
    BleCharPointer->Char_Value_Length = 2 + 16 + 2;
  }
  else
  {
    /* 2 byte timestamp, 8 FSM registers output, 1 FSM output state */
    BleCharPointer->Char_Value_Length = 2 + 8 + 1;
  }

  BleNumReg = NumReg;

  BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_READ);
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 0;

  BLE_MANAGER_PRINTF("BLE FSM features ok\r\n");

  return BleCharPointer;
}

/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *VoidCharPointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
#if (BLUE_CORE != BLUENRG_LP)
static void Read_Request_FiniteStateMachine(void *VoidCharPointer, uint16_t handle)
{
  if (CustomReadFiniteStateMachine != NULL)
  {
    uint8_t fsm_out[16];
    uint8_t fsm_status_a_mainpage;
    uint8_t fsm_status_b_mainpage;
    CustomReadFiniteStateMachine(fsm_out, &fsm_status_a_mainpage, &fsm_status_b_mainpage);
    BLE_FiniteStateMachineUpdate(fsm_out, &fsm_status_a_mainpage, &fsm_status_b_mainpage);
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nRead request FiniteStateMachine function not defined\r\n\n");
  }
}
#else /* (BLUE_CORE != BLUENRG_LP) */
static void Read_Request_FiniteStateMachine(void *BleCharPointer,
                                            uint16_t handle,
                                            uint16_t Connection_Handle,
                                            uint8_t Operation_Type,
                                            uint16_t Attr_Val_Offset,
                                            uint8_t Data_Length,
                                            uint8_t Data[])
{
  tBleStatus ret;
  if (CustomReadFiniteStateMachine != NULL)
  {
    uint8_t fsm_out[16];
    uint8_t fsm_status_a_mainpage;
    uint8_t fsm_status_b_mainpage;
    /* 2 byte timestamp, 16 FSM registers output, 2 FSM output state */
    uint8_t dimByte;
    uint8_t buff[2 + 16 + 2];
    CustomReadFiniteStateMachine(fsm_out, &fsm_status_a_mainpage, &fsm_status_b_mainpage);

    STORE_LE_16(buff, (HAL_GetTick() >> 3));

    if (BleNumReg == BLE_FSM_16_REG)
    {
      /* MFSM outputs registers */
      memcpy(buff + 2, fsm_out, 16U * sizeof(uint8_t));

      /* Status bit for FSM from 1 to 8   */
      buff[2 + 16] = fsm_status_a_mainpage;
      /* Status bit for FSM from 9 to 16   */
      buff[2 + 16 + 1] = fsm_status_b_mainpage;
      dimByte = 2 + 16 + 2;
    }
    else
    {
      /* FSM outputs registers */
      memcpy(buff + 2, fsm_out, 8U * sizeof(uint8_t));

      /* Status bit for FSM from 1 to 8   */
      buff[2 + 8] = fsm_status_a_mainpage;
      dimByte = 2 + 8 + 1;
    }
    ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, dimByte, buff);
    if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
    {
      if (BLE_StdErr_Service == BLE_SERV_ENABLE)
      {
        BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating FiniteStateMachine Char\n");
        Stderr_Update(BufferToWrite, BytesToWrite);
      }
      else
      {
        BLE_MANAGER_PRINTF("Error: Updating FiniteStateMachine Char\r\n");
      }
    }
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nRead request FiniteStateMachine function not defined\r\n\n");
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
  * @brief  Update Finite State Machine output registers characteristic
  * @param  uint8_t *fsm_out pointers to 16 FSM register
  * @param  uint8_t *fsm_status_a_mainpage pointer to the FSM status bits from 1 to 8
  * @param  uint8_t *fsm_status_a_mainpage pointer to the FSM status bits from 9 to 16
  * @retval tBleStatus Status
  */
tBleStatus BLE_FiniteStateMachineUpdate(uint8_t *fsm_out, uint8_t *fsm_status_a_mainpage,
                                        uint8_t *fsm_status_b_mainpage)
{
  tBleStatus ret;

  /* 2 byte timestamp, 16 FSM registers output, 2 MCL output state */
  uint8_t buff[2 + 16 + 2];

  /* TimeStamp */
  STORE_LE_16(buff, (HAL_GetTick() >> 3));

  if (BleNumReg == BLE_FSM_16_REG)
  {
    /* FSM outputs registers */
    memcpy(buff + 2, fsm_out, 16U * sizeof(uint8_t));

    /* Status bits for FSM from 1 to 8 */
    buff[18] = *fsm_status_a_mainpage;
    /* Status bit for FSM from 9 to 16 */
    buff[19] = *fsm_status_b_mainpage;

    ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharFiniteStateMachine, 0, 2 + 16 + 2, buff);
  }
  else
  {
    /* FSM outputs registers */
    memcpy(buff + 2, fsm_out, 8U * sizeof(uint8_t));

    /* Status bits for FSM from 1 to 8 */
    buff[10] = *fsm_status_a_mainpage;

    ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharFiniteStateMachine, 0, 2 + 8 + 1, buff);
  }

  if (ret != (tBleStatus)BLE_STATUS_SUCCESS)
  {
    if (BLE_StdErr_Service == BLE_SERV_ENABLE)
    {
      BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "Error Updating Finite State Machine Char\n");
      Stderr_Update(BufferToWrite, BytesToWrite);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Finite State Machine Char\r\n");
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
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event,this bit is set to 1 to notify that other
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.
  * @param  uint8_t data_length length of the data
  * @param  uint8_t *att_data attribute data
  * @retval None
  */
static void AttrMod_Request_FiniteStateMachine(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                               uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventFiniteStateMachine != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventFiniteStateMachine(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventFiniteStateMachine(BLE_NOTIFY_UNSUB);
    }
  }
#if (BLE_DEBUG_LEVEL>1)
  else
  {
    BLE_MANAGER_PRINTF("CustomNotifyEventFiniteStateMachine function Not Defined\r\n");
  }
  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite, "--->FSM=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->FSM=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}
