/**
  ******************************************************************************
  * @file    ble_machine_learning_core.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add Machine Learning Core info services using vendor specific
  *          profiles.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "ble_manager.h"
#include "ble_manager_common.h"

/* Private define ------------------------------------------------------------*/
#define COPY_MACHINE_LEARNING_CORE_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x0F,0x00,0x02,\
                                                                        0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,\
                                                                        0xc5,0x1b)

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for Machine Learning Core info service */
static ble_char_object_t ble_char_machine_learning_core;

static ble_machine_learning_core_num_reg_t ble_num_reg = BLE_MLC_8_REG;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_machine_learning_core(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
__weak void read_request_machine_learning_core_function(uint8_t *mlc_out, uint8_t *mlc_status_mainpage);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_machine_learning_core(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                                   uint8_t data_length, uint8_t *att_data);

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
static void read_request_machine_learning_core(void *ble_char_pointer, uint16_t handle);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
static void read_request_machine_learning_core(void *ble_char_pointer,
                                               uint16_t handle,
                                               uint16_t connection_handle,
                                               uint8_t operation_type,
                                               uint16_t attr_val_offset,
                                               uint8_t data_length,
                                               uint8_t data[]);
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init Machine Learning Core info service
  * @param  ble_machine_learning_core_num_reg_t num_reg Number MLC registers
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for Machine Learning Core info service
  */
ble_char_object_t *ble_init_machine_learning_core_service(ble_machine_learning_core_num_reg_t num_reg)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Init data structure pointer for Machine Learning Core info service */
  ble_char_pointer = &ble_char_machine_learning_core;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_machine_learning_core;
  ble_char_pointer->read_request_cb = read_request_machine_learning_core;
  COPY_MACHINE_LEARNING_CORE_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;

  if (num_reg == BLE_MLC_8_REG)
  {
    /* 2 byte timestamp, 8 MLC registers output, 1 MCL output state */
    ble_char_pointer->char_value_length = 2 + 8 + 1;
  }
  else
  {
    /* 2 byte timestamp, 4 FSM registers output, 1 FSM output state */
    ble_char_pointer->char_value_length = 2 + 4 + 1;
  }

  ble_num_reg = num_reg;

  ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_READ);
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 0;

  BLE_MANAGER_PRINTF("BLE MLC features ok\r\n");

  return ble_char_pointer;
}

/**
  * @brief  Update Machine Learning Core output registers characteristic
  * @param  uint8_t *mlc_out pointers to 8 MLC register
  * @param  uint8_t *mlc_status_mainpage pointer to the MLC status bits from 1 to 8
  * @retval ble_status_t Status
  */
ble_status_t ble_machine_learning_core_update(uint8_t *mlc_out, uint8_t *mlc_status_mainpage)
{
  ble_status_t ret;

  /* 2 byte timestamp, 8 MLC registers output, 1 MCL output state */
  uint8_t buff[2 + 8 + 1];

  /* TimeStamp */
  STORE_LE_16(buff, (HAL_GetTick() / 10U));

  if (ble_num_reg == BLE_MLC_8_REG)
  {
    /* MLC outputs registers */
    memcpy(buff + 2, mlc_out, 8U * sizeof(uint8_t));

    /* Status bit for MLC from 1 to 8   */
    buff[10] = *mlc_status_mainpage;

    ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_machine_learning_core, 0, 2 + 8 + 1, buff);
  }
  else
  {
    /* MLC outputs registers */
    memcpy(buff + 2, mlc_out, 4U * sizeof(uint8_t));

    /* Status bit for MLC from 1 to 4   */
    buff[6] = *mlc_status_mainpage;

    ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_machine_learning_core, 0, 2 + 4 + 1, buff);
  }

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating Machine Learning Core Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error Updating Machine Learning Core Char\r\n");
    }
  }
  return ret;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if Machine Learning Core is subscribed
  *         or not to the one service
  * @param  void *void_char_pointer
  * @param  uint16_t attr_handle Handle of the attribute
  * @param  uint16_t offset: (SoC mode) the offset is never used and it is always 0. Network coprocessor mode:
  *                          - Bits 0-14: offset of the reported value inside the attribute.
  *                          - Bit 15: if the entire value of the attribute does not fit inside a single
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT event, this bit is set to 1 to notify that other
  *                            ACI_GATT_ATTRIBUTE_MODIFIED_EVENT events will follow to report the remaining value.
  * @param  uint8_t data_length length of the data
  * @param  uint8_t *att_data attribute data
  * @retval None
  */
static void attr_mod_request_machine_learning_core(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                                   uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_machine_learning_core(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_machine_learning_core(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "--->MLC=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->MLC=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *void_char_pointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
static void read_request_machine_learning_core(void *void_char_pointer, uint16_t handle)
{
  uint8_t mlc_out[8];
  uint8_t mlc_status_mainpage;
  read_request_machine_learning_core_function(mlc_out, &mlc_status_mainpage);
  ble_machine_learning_core_update(mlc_out, &mlc_status_mainpage);
}
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
static void read_request_machine_learning_core(void *ble_char_pointer,
                                               uint16_t handle,
                                               uint16_t connection_handle,
                                               uint8_t operation_type,
                                               uint16_t attr_val_offset,
                                               uint8_t data_length,
                                               uint8_t data[])
{
  ble_status_t ret;

  uint8_t mlc_out[8];
  uint8_t mlc_status_mainpage;
  /* 2 byte timestamp, 8 MLC registers output, 1 MCL output state */
  uint8_t dim_byte;
  uint8_t buff[2 + 8 + 1];
  read_request_machine_learning_core_function(mlc_out, &mlc_status_mainpage);

  STORE_LE_16(buff, (HAL_GetTick() / 10U));

  if (ble_num_reg == BLE_MLC_8_REG)
  {
    /* MLC outputs registers */
    memcpy(buff + 2, mlc_out, 8U * sizeof(uint8_t));

    /* Status bit for MLC from 1 to 8   */
    buff[10] = mlc_status_mainpage;
    dim_byte = 2 + 8 + 1;
  }
  else
  {
    /* MLC outputs registers */
    memcpy(buff + 2, mlc_out, 4U * sizeof(uint8_t));

    /* Status bit for MLC from 1 to 4   */
    buff[6] = mlc_status_mainpage;
    dim_byte = 2 + 4 + 1;
  }
  ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, dim_byte, buff);
  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating MachineLearningCore Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating MachineLearningCore Char\r\n");
    }
  }

#if (BLUE_CORE == STM32WB05N)
  ret = aci_gatt_srv_authorize_resp_nwk(connection_handle, 0x0004, handle,
                                        operation_type, 0, attr_val_offset,
                                        data_length, data);
#else /* (BLUE_CORE == STM32WB05N) */
  ret = aci_gatt_srv_authorize_resp_nwk(connection_handle, handle,
                                        operation_type, 0, attr_val_offset,
                                        data_length, data);
#endif /* (BLUE_CORE == STM32WB05N) */

  if (ret != BLE_STATUS_SUCCESS)
  {
    BLE_MANAGER_PRINTF("aci_gatt_srv_authorize_resp_nwk() failed: 0x%02x\r\n", ret);
  }
}
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t Event Sub/Unsub
  * @retval None
  */
__weak void notify_event_machine_learning_core(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify machine learning core function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_machine_learning_core could be implemented in the user file
   */
}

/********************************************************
  * Callback function to manage the read request events *
  *******************************************************/
/**
  * @brief  Callback Function for machine learning core read request.
  * @param  uint8_t *mlc_out
  * @param  uint8_t *mlc_status_mainpage
  * @retval None
  */
__weak void read_request_machine_learning_core_function(uint8_t *mlc_out, uint8_t *mlc_status_mainpage)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(mlc_out);
  BLE_UNUSED(mlc_status_mainpage);

  BLE_MANAGER_PRINTF("\r\nRead request machine learning core function not defined (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the read_request_machine_learning_core_function could be implemented in the user file
   */
}
