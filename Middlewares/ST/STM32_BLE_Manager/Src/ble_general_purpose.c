/**
  ******************************************************************************
  * @file    ble_general_purpose.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add General Purpose info services using vendor specific profiles.
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
#define COPY_GP_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x00,\
                                                     0x00,0x03,0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for General Purpose info service */
static ble_char_object_t ble_char_general_purpose[BLE_GENERAL_PURPOSE_MAX_CHARS_NUM];

/* Mapping bertween the GP number allocation and GP number used with the UUID */
static uint8_t ble_char_general_purpose_mapping[BLE_GENERAL_PURPOSE_MAX_CHARS_NUM];

static int32_t number_allocated_gp = 0;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_general_purpose(uint8_t ble_char_uuid, ble_notify_event_t event);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_general_purpose(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                             uint8_t data_length, uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init General Purpose Service
  * @brief  uint8_t ble_char_uuid Number of General Purpose Char (UUID[14] byte)
  * @param  uint8_t size Dimensions of the BLE chars without counting the 2 bytes used for TimeStamp
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for General Purpose info service
  */
extern ble_char_object_t *ble_init_general_purpose_service(uint8_t ble_char_uuid, uint8_t size)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Some Controls */
  if ((uint8_t)number_allocated_gp >= BLE_GENERAL_PURPOSE_MAX_CHARS_NUM)
  {
    BLE_MANAGER_PRINTF("Error already Allocated %d GP BLE Chars\r\n", BLE_GENERAL_PURPOSE_MAX_CHARS_NUM);
    return NULL;
  }

  ble_char_general_purpose_mapping[number_allocated_gp] = ble_char_uuid;

  /* Init data structure pointer for General Purpose info service */
  ble_char_pointer = &ble_char_general_purpose[number_allocated_gp];
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_general_purpose;
  COPY_GP_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->uuid[14] = ble_char_uuid;
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = ((uint16_t)size) + 2U;
  ble_char_pointer->char_properties = (uint8_t)CHAR_PROP_NOTIFY;
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 0;

  BLE_MANAGER_PRINTF("BLE General Purpose feature [%ld] ok\r\n", number_allocated_gp);

  /* Increment the Number of Allocated General Purpose Features */
  number_allocated_gp++;

  return ble_char_pointer;
}


/**
  * @brief  Update General Purpose characteristic value
  * @param  uint8_t ble_char_uuid Number of General Purpose Char (UUID[14] byte)
  * @param  uint8_t *data Data to Update
  * @retval ble_status_t   Status
  */
ble_status_t ble_general_purpose_status_update(uint8_t ble_char_uuid, uint8_t *data)
{
  ble_status_t ret = BLE_ERROR_UNSPECIFIED;

  uint8_t gp_char_num = BLE_GENERAL_PURPOSE_MAX_CHARS_NUM;
  /* Search the right BLE_GP char */
  uint32_t search;
  for (search = 0;
       ((search < (uint32_t)number_allocated_gp) && (gp_char_num == BLE_GENERAL_PURPOSE_MAX_CHARS_NUM));
       search++)
  {
    if (ble_char_uuid == ble_char_general_purpose_mapping[search])
    {
      gp_char_num = (uint8_t)search;
    }
  }

  if (gp_char_num != BLE_GENERAL_PURPOSE_MAX_CHARS_NUM)
  {
    uint8_t buff[BLE_GENERAL_PURPOSE_MAX_CHARS_DIM];

    STORE_LE_16(buff, (HAL_GetTick() / 10U));
    memcpy(buff + 2, data, (uint32_t)ble_char_general_purpose[gp_char_num].char_value_length - 2U);

    ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_general_purpose[gp_char_num],
                                     0,
                                     (uint8_t)ble_char_general_purpose[gp_char_num].char_value_length,
                                     buff);

    if (ret != (ble_status_t)BLE_STATUS_SUCCESS)
    {
      if (ble_std_err_service == BLE_SERV_ENABLE)
      {
        bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write, "Error Updating GP[%d] Char\n", gp_char_num);
        std_err_update(buffer_to_write, bytes_to_write);
      }
      else
      {
        BLE_MANAGER_PRINTF("Error Updating GP[%d] Char\r\n", gp_char_num);
      }
    }
  }
  else
  {
    BLE_MANAGER_PRINTF(" Error: GP Not Found for UUID[14] =%d\r\n", ble_char_uuid);
  }
  return ret;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if led is subscribed or not to the one service
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
static void attr_mod_request_general_purpose(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                             uint8_t data_length, uint8_t *att_data)
{
  uint8_t gp_char_num = BLE_GENERAL_PURPOSE_MAX_CHARS_NUM;
  uint8_t ble_char_uuid;

  /* find the right GP feature */
  ble_char_object_t *local_ble_char = (ble_char_object_t *)void_char_pointer;
  uint32_t search;
  for (search = 0;
       ((search < (uint32_t)number_allocated_gp) && (gp_char_num == BLE_GENERAL_PURPOSE_MAX_CHARS_NUM));
       search++)
  {
    if (local_ble_char->uuid[14] == ble_char_general_purpose[search].uuid[14])
    {
      gp_char_num = (uint8_t)search;
    }
  }

  if (gp_char_num != BLE_GENERAL_PURPOSE_MAX_CHARS_NUM)
  {
    /* Retrieve the GPE Char Number */
    ble_char_uuid = ble_char_general_purpose_mapping[gp_char_num];
    /* if we had found the corresponding General Purpose Feature */
    if (att_data[0] == 01U)
    {
      notify_event_general_purpose(ble_char_uuid, BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      notify_event_general_purpose(ble_char_uuid, BLE_NOTIFY_UNSUB);
    }
  }

#if (BLE_DEBUG_LEVEL>1)
  if (gp_char_num != BLE_GENERAL_PURPOSE_MAX_CHARS_NUM)
  {
    if (ble_std_term_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t) sprintf((char *)buffer_to_write,
                                         "--->GP[%d]=%s\n",
                                         ble_char_uuid,
                                         (att_data[0] == 01U) ? " ON" : " OFF");
      term_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("--->GP[%d]=%s", ble_char_uuid, (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
    }
  }
  else
  {
    /* if we didn't find a suitable General Purpose char */
    if (ble_std_term_service == BLE_SERV_ENABLE)
    {
      bytes_to_write = (uint8_t) sprintf((char *)buffer_to_write,
                                         "--->GP[Not Found]=%s\n",
                                         (att_data[0] == 01U) ? " ON" : " OFF");
      term_update(buffer_to_write, bytes_to_write);
    }
    else
    {
      BLE_MANAGER_PRINTF("--->GP[Not Found]=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
    }
  }
#endif /* (BLE_DEBUG_LEVEL>1) */
}

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t event Sub/Unsub
  * @retval None
  */
__weak void notify_event_general_purpose(uint8_t ble_char_uuid, ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);
  BLE_UNUSED(ble_char_uuid);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify general purpose function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_general_purpose could be implemented in the user file
   */
}
