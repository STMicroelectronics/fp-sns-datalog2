/**
  ******************************************************************************
  * @file    BLE_Inertial.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add inertial info services using vendor specific profiles.
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
#define COPY_INERTIAL_CHAR_UUID(uuid_struct)  COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x00,\
                                                            0x00,0x01,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

#define INERTIAL_ADVERTISE_DATA_POSITION  16

/* Private Types ----------------------------------------------------------- */
typedef struct
{
  uint8_t acc_is_enable;
  uint8_t gyro_is_enable;
  uint8_t mag_is_enable;
} ble_manager_inertial_features_enabled_t;

/* Private variables ---------------------------------------------------------*/
/* Data structure for identify inertial info services enabled */
ble_manager_inertial_features_enabled_t inertial_features_enabled;
/* Data structure pointer for inertial info service */
static ble_char_object_t ble_char_inertial;
/* Size for inertial BLE characteristic */
static uint8_t  inertial_char_size;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_inertial(ble_notify_event_t event);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_inertial(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                      uint8_t data_length,
                                      uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init inertial info service
  * @param  uint8_t acc_enable:   1 for enabling the BLE accelerometer feature, 0 otherwise.
  * @param  uint8_t gyro_enable:  1 for enabling the BLE gyroscope feature, 0 otherwise.
  * @param  uint8_t mag_enable:  1 for esabling the BLE magnetometer feature, 0 otherwise.
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for environmental info service
  */
ble_char_object_t *ble_init_inertial_service(uint8_t acc_enable, uint8_t gyro_enable, uint8_t mag_enable)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer = NULL;
  inertial_char_size = 2;

  /* Init data structure for identify environmental info services enabled */
  inertial_features_enabled.acc_is_enable = acc_enable;
  inertial_features_enabled.gyro_is_enable = gyro_enable;
  inertial_features_enabled.mag_is_enable = mag_enable;

  if ((acc_enable == 1U) ||
      (gyro_enable == 1U)   ||
      (mag_enable == 1U))
  {
    /* Init data structure pointer for inertial info service */
    ble_char_pointer = &ble_char_inertial;
    memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
    ble_char_pointer->attr_mod_request_cb = attr_mod_request_inertial;
    COPY_INERTIAL_CHAR_UUID((ble_char_pointer->uuid));

    /* Enables BLE accelerometer feature */
    if (acc_enable == 1U)
    {
      ble_char_pointer->uuid[14] |= 0x80U;
      inertial_char_size += 3U * 2U;
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("\t--> Accelerometer feature enabled\r\n");
#endif /* (BLE_DEBUG_LEVEL>1) */
    }

    /* Enables BLE gyroscope feature */
    if (gyro_enable == 1U)
    {
      ble_char_pointer->uuid[14] |= 0x40U;
      inertial_char_size += 3U * 2U;
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("\t--> Gyroscope feature enabled\r\n");
#endif /* (BLE_DEBUG_LEVEL>1) */
    }

    /* Enables BLE magnetometer feature */
    if (mag_enable == 1U)
    {
      ble_char_pointer->uuid[14] |= 0x20U;
      inertial_char_size += 3U * 2U;
#if (BLE_DEBUG_LEVEL>1)
      BLE_MANAGER_PRINTF("\t--> Magnetometer feature enabled\r\n");
#endif /* (BLE_DEBUG_LEVEL>1) */
    }

    ble_char_pointer->char_uuid_type = UUID_TYPE_128;
    ble_char_pointer->char_value_length = inertial_char_size;
    ble_char_pointer->char_properties = CHAR_PROP_NOTIFY;
    ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
    ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
    ble_char_pointer->enc_key_size = 16;
    ble_char_pointer->is_variable = 0;

    BLE_MANAGER_PRINTF("BLE Inertial features ok\r\n");
  }
  else
  {
    BLE_MANAGER_PRINTF(" ERROR: None inertial features is enabled\r\n");
  }

  return ble_char_pointer;
}

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Inertial Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
void ble_set_inertial_advertise_data(uint8_t *manuf_data)
{
  /* Setting Accelerometer Advertise Data */
  if (inertial_features_enabled.acc_is_enable == 1U)
  {
    manuf_data[INERTIAL_ADVERTISE_DATA_POSITION] |= 0x80U;
  }

  /* Setting Gyroscope Advertise Data */
  if (inertial_features_enabled.gyro_is_enable == 1U)
  {
    manuf_data[INERTIAL_ADVERTISE_DATA_POSITION] |= 0x40U;
  }

  /* Setting Magnetometer Advertise Data */
  if (inertial_features_enabled.mag_is_enable == 1U)
  {
    manuf_data[INERTIAL_ADVERTISE_DATA_POSITION] |= 0x20U;
  }
}
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update acceleration/Gryoscope and Magneto characteristics value
  * @param  ble_manager_inertial_axes_t acc:     Structure containing acceleration value in mg
  * @param  ble_manager_inertial_axes_t gyro:    Structure containing Gyroscope value
  * @param  ble_manager_inertial_axes_t mag:     Structure containing magneto value
  * @retval ble_status_t      Status
  */
ble_status_t ble_acc_gyro_mag_update(ble_manager_inertial_axes_t *acc,
                                     ble_manager_inertial_axes_t *gyro,
                                     ble_manager_inertial_axes_t *mag)
{
  ble_status_t ret;
  uint8_t buff_pos;

  uint8_t buff[2 + (3 * 2)/*Acc*/ + (3 * 2)/*Gyro*/ + (3 * 2)/*Mag*/];

  /* Time Stamp */
  STORE_LE_16(buff, (HAL_GetTick() / 10U));
  buff_pos = 2;

  if (inertial_features_enabled.acc_is_enable == 1U)
  {
    STORE_LE_16(buff + buff_pos, ((uint16_t)acc->axis_x));
    buff_pos += 2U;
    STORE_LE_16(buff + buff_pos, ((uint16_t)acc->axis_y));
    buff_pos += 2U;
    STORE_LE_16(buff + buff_pos, ((uint16_t)acc->axis_z));
    buff_pos += 2U;
  }

  if (inertial_features_enabled.gyro_is_enable == 1U)
  {
    gyro->axis_x /= 100;
    gyro->axis_y /= 100;
    gyro->axis_z /= 100;

    STORE_LE_16(buff + buff_pos, ((uint16_t)gyro->axis_x));
    buff_pos += 2U;
    STORE_LE_16(buff + buff_pos, ((uint16_t)gyro->axis_y));
    buff_pos += 2U;
    STORE_LE_16(buff + buff_pos, ((uint16_t)gyro->axis_z));
    buff_pos += 2U;
  }

  if (inertial_features_enabled.mag_is_enable == 1U)
  {
    STORE_LE_16(buff + buff_pos, ((uint16_t)mag->axis_x));
    buff_pos += 2U;
    STORE_LE_16(buff + buff_pos, ((uint16_t)mag->axis_y));
    buff_pos += 2U;
    STORE_LE_16(buff + buff_pos, ((uint16_t)mag->axis_z));
    buff_pos += 2U;
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_inertial, 0, inertial_char_size, buff);

  if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
  {
    if (ret != (ble_status_t)BLE_STATUS_INSUFFICIENT_RESOURCES)
    {
      if (ble_std_err_service == BLE_SERV_ENABLE)
      {
        bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating Acc/Gyro/Mag Char\n");
        std_err_update(buffer_to_write, bytes_to_write);
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

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if inertial is subscribed or not to the one service
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
static void attr_mod_request_inertial(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                      uint8_t data_length,
                                      uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_inertial(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_inertial(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write,
                                      "--->Acc/Gyro/Mag=%s\n",
                                      (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->Acc/Gyro/Mag=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t Event Sub/Unsub
  * @retval None
  */
__weak void notify_event_inertial(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify inertial function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_inertial could be implemented in the user file
   */
}