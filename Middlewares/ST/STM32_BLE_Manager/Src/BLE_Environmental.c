/**
  ******************************************************************************
  * @file    ble_environmental.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add environmental info services using vendor specific profiles.
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
#define COPY_ENVIRONMENTAL_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x00,0x00,0x01,\
                                                                0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define ENVIRONMENTAL_ADVERTISE_DATA_POSITION  16

/* Private Types ----------------------------------------------------------- */
typedef struct
{
  uint8_t pressure_is_enable;
  uint8_t humidity_is_enable;
  uint8_t number_temperatures_enabled;
} ble_manager_env_features_enabled_t;

/* Private variables ---------------------------------------------------------*/
/* Data structure for identify environmental info services enabled */
ble_manager_env_features_enabled_t env_features_enabled;
/* Data structure pointer for environmental info service */
static ble_char_object_t ble_char_env;
/* Size for Environmental BLE characteristic */
static uint8_t environmental_char_size;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_env(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the read request events *
  *****************************************************************/
__weak void read_request_env_function(int32_t *press, uint16_t *hum, int16_t *temp1, int16_t *temp2);

/* Private functions prototypes ----------------------------------------------*/
static void attr_mod_request_env(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset, uint8_t data_length,
                                 uint8_t *att_data);
#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
static void read_request_env(void *ble_char_pointer, uint16_t handle);
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
static void read_request_env(void *ble_char_pointer,
                             uint16_t handle,
                             uint16_t connection_handle,
                             uint8_t operation_type,
                             uint16_t attr_val_offset,
                             uint8_t data_length,
                             uint8_t data[]);
#endif /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init environmental info service
  * @param  uint8_t press_enable:    1 for enabling the BLE pressure feature, 0 otherwise.
  * @param  uint8_t hum_enable:      1 for enabling the BLE humidity feature, 0 otherwise.
  * @param  uint8_t num_temp_enabled: 0 for disabling BLE temperature feature
  *                                 1 for enabling only one BLE temperature feature
  *                                 2 for enabling two BLE temperatures feature
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for environmental info service
  */
ble_char_object_t *ble_init_env_service(uint8_t press_enable, uint8_t hum_enable, uint8_t num_temp_enabled)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer = NULL;
  environmental_char_size = 2;

  /* Init data structure for identify environmental info services enabled */
  env_features_enabled.pressure_is_enable = press_enable;
  env_features_enabled.humidity_is_enable = hum_enable;
  env_features_enabled.number_temperatures_enabled = num_temp_enabled;

  if ((press_enable == 1U) ||
      (hum_enable == 1U)   ||
      (num_temp_enabled > 0U))
  {
    /* Init data structure pointer for environmental info service */
    ble_char_pointer = &ble_char_env;
    memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
    ble_char_pointer->attr_mod_request_cb = attr_mod_request_env;
    ble_char_pointer->read_request_cb = read_request_env;
    COPY_ENVIRONMENTAL_CHAR_UUID((ble_char_pointer->uuid));

    /* Enables BLE Pressure feature */
    if (press_enable == 1U)
    {
      ble_char_pointer->uuid[14] |= 0x10U;
      environmental_char_size += 4U;
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("\t--> Pressure feature enabled\r\n");
#endif /* BLE_DEBUG_LEVEL>1 */
    }

    /* Enables BLE Humidity feature */
    if (hum_enable == 1U)
    {
      ble_char_pointer->uuid[14] |= 0x08U;
      environmental_char_size += 2U;
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("\t--> Humidity feature enabled\r\n");
#endif /* BLE_DEBUG_LEVEL>1 */
    }

    if (num_temp_enabled == 1U)
    {
      /* Enables only one BLE temperature feature */
      ble_char_pointer->uuid[14] |= 0x04U;
      environmental_char_size += 2U;
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("\t--> Only one temperature feature enabled\r\n");
#endif /* BLE_DEBUG_LEVEL>1 */
    }
    else if (num_temp_enabled == 2U)
    {
      /* Enables two BLE temperatures feature */
      ble_char_pointer->uuid[14] |= 0x05U;
      environmental_char_size += 4U;
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("\t--> Two temperatures features enabled\r\n");
#endif /* BLE_DEBUG_LEVEL>1 */
    }

    ble_char_pointer->char_uuid_type = UUID_TYPE_128;
    ble_char_pointer->char_value_length = environmental_char_size;
    ble_char_pointer->char_properties = ((uint8_t)(CHAR_PROP_NOTIFY)) | ((uint8_t)(CHAR_PROP_READ));
    ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
    ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
    ble_char_pointer->enc_key_size = 16;
    ble_char_pointer->is_variable = 0;

    BLE_MANAGER_PRINTF("BLE Environmental features ok\r\n");
  }
  else
  {
    BLE_MANAGER_PRINTF(" ERROR: None environmental features is enabled\r\n");
  }

  return ble_char_pointer;
}

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Environmental Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
void ble_set_env_advertise_data(uint8_t *manuf_data)
{
  /* Setting Pressure Advertise Data */
  if (env_features_enabled.pressure_is_enable == 1U)
  {
    manuf_data[ENVIRONMENTAL_ADVERTISE_DATA_POSITION] |= 0x10U;
  }

  /* Setting Humidity Advertise Data */
  if (env_features_enabled.humidity_is_enable == 1U)
  {
    manuf_data[ENVIRONMENTAL_ADVERTISE_DATA_POSITION] |= 0x08U;
  }

  /* Setting One Temperature Advertise Data */
  if (env_features_enabled.number_temperatures_enabled == 1U)
  {
    manuf_data[ENVIRONMENTAL_ADVERTISE_DATA_POSITION] |= 0x04U;
  }

  /* Setting Two Temperature Advertise Data */
  if (env_features_enabled.number_temperatures_enabled == 2U)
  {
    manuf_data[ENVIRONMENTAL_ADVERTISE_DATA_POSITION] |= 0x05U;
  }
}
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Environmental characteristic value
  * @param  int32_t press:       Pressure in mbar (Set 0 if not used)
  * @param  uint16_t hum:        humidity RH (Relative Humidity) in thenths of % (Set 0 if not used)
  * @param  int16_t temp1:       Temperature in tenths of degree (Set 0 if not used)
  * @param  int16_t temp2:       Temperature in tenths of degree (Set 0 if not used)
  * @retval ble_status_t:          Status
  */
ble_status_t ble_environmental_update(int32_t press, uint16_t hum, int16_t temp1, int16_t temp2)
{
  ble_status_t ret;
  uint8_t buff_pos;

  uint8_t buff[2 + 4/*press*/ + 2/*hum*/ + 2/*temp1*/ + 2/*temp2*/];

  /* Time Stamp */
  STORE_LE_16(buff, (HAL_GetTick() / 10U));
  buff_pos = 2;

  if (env_features_enabled.pressure_is_enable == 1U)
  {
    STORE_LE_32((buff + buff_pos), ((uint32_t)press));
    buff_pos += 4U;
  }

  if (env_features_enabled.humidity_is_enable == 1U)
  {
    STORE_LE_16((buff + buff_pos), hum);
    buff_pos += 2U;
  }

  if (env_features_enabled.number_temperatures_enabled >= 1U)
  {
    STORE_LE_16((buff + buff_pos), ((uint16_t)temp1));
    buff_pos += 2U;
  }

  if (env_features_enabled.number_temperatures_enabled == 2U)
  {
    STORE_LE_16((buff + buff_pos), ((uint16_t)temp2));
    buff_pos += 2U;
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_env, 0, environmental_char_size, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating Environmental Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating Environmental Char\r\n");
    }
  }
  return ret;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if environmental is subscribed or not to the one service
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
static void attr_mod_request_env(void *void_char_pointer, uint16_t attr_handle, uint16_t offset, uint8_t data_length,
                                 uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_env(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_env(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "--->Env=%s\n", (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->Env=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* BLE_DEBUG_LEVEL>1 */
}

#if ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N))
/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *void_char_pointer
  * @param  uint16_t handle Handle of the attribute
  * @retval None
  */
static void read_request_env(void *void_char_pointer, uint16_t handle)
{
  int32_t press;
  uint16_t hum;
  int16_t temp1;
  int16_t temp2;
  read_request_env_function(&press, &hum, &temp1, &temp2);
  ble_environmental_update(press, hum, temp1, temp2);
}
#else /* ((BLUE_CORE != BLUENRG_LP) && (BLUE_CORE != STM32WB07_06) && (BLUE_CORE != STM32WB05N)) */
/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *void_char_pointer
  * @param  uint16_t handle Handle of the attribute
  * @param  uint16_t connection_handle Connection handle
  * @param  uint8_t operation_type see aci_gatt_srv_authorize_nwk_event
  * @param  uint16_t attr_val_offset ffset from which the value needs to be read or write
  * @param  uint8_t data_length Length of Data field
  * @param  uint8_t data[] The data that the client has requested to write
  * @retval None
  */
static void read_request_env(void *ble_char_pointer,
                             uint16_t handle,
                             uint16_t connection_handle,
                             uint8_t operation_type,
                             uint16_t attr_val_offset,
                             uint8_t data_length,
                             uint8_t data[])
{
  ble_status_t ret;

  int32_t press;
  uint16_t hum;
  int16_t temp1;
  int16_t temp2;
  uint8_t buff_pos;
  uint8_t buff[2 + 4/*press*/ + 2/*hum*/ + 2/*temp1*/ + 2/*temp2*/];

  read_request_env_function(&press, &hum, &temp1, &temp2);

  /* Time Stamp */
  STORE_LE_16(buff, (HAL_GetTick() / 10U));
  buff_pos = 2;

  if (env_features_enabled.pressure_is_enable == 1U)
  {
    STORE_LE_32((buff + buff_pos), ((uint32_t)press));
    buff_pos += 4U;
  }

  if (env_features_enabled.humidity_is_enable == 1U)
  {
    STORE_LE_16((buff + buff_pos), hum);
    buff_pos += 2U;
  }

  if (env_features_enabled.number_temperatures_enabled >= 1U)
  {
    STORE_LE_16((buff + buff_pos), ((uint16_t)temp1));
    buff_pos += 2U;
  }

  if (env_features_enabled.number_temperatures_enabled == 2U)
  {
    STORE_LE_16((buff + buff_pos), ((uint16_t)temp2));
    buff_pos += 2U;
  }

  ret = aci_gatt_srv_write_handle_value_nwk(handle, 0, environmental_char_size, buff);
  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ble_std_err_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating Environmental Char\n");
      std_err_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("Error: Updating Environmental Char\r\n");
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
__weak void notify_event_env(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify environmental function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_env could be implemented in the user file
   */
}

/********************************************************
  * Callback function to manage the read request events *
  *******************************************************/
/**
  * @brief  Callback Function for Environmental read request.
  * @param  int32_t *press Pressure Value
  * @param  uint16_t *hum Humidity Value
  * @param  int16_t *temp1 Temperature Number 1
  * @param  int16_t *temp2 Temperature Number 2
  * @retval None
  */
__weak void read_request_env_function(int32_t *press, uint16_t *hum, int16_t *temp1, int16_t *temp2)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(press);
  BLE_UNUSED(hum);
  BLE_UNUSED(temp1);
  BLE_UNUSED(temp2);

  BLE_MANAGER_PRINTF("\r\nRead request environmental function not defined (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the read_request_env_function could be implemented in the user file
   */
}
