/**
  ******************************************************************************
  * @file    ble_binary_content.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add BinaryContent info services using vendor specific profile.
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
#define COPY_BINARYCONTENT_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x22,0x00,0x02,0x11,\
                                                                0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for BinaryContent info service */
static ble_char_object_t ble_char_binary_content;
/* Buffer used to save the complete command received via BLE*/
static uint8_t *ble_command_buffer = NULL;

static uint16_t binary_content_max_char_length = DEFAULT_MAX_BINARY_CONTENT_CHAR_LEN;

static uint32_t total_len_decode_packet = 0;
static ble_comm_tp_status_t status_decoding_packet = BLE_COMM_TP_WAIT_START;

/*******************************************************************************************
  * This function is called when a write request is received by the server from the client *
  ******************************************************************************************/
__weak void write_request_binary_content(void *ble_char_pointer, uint16_t handle, uint16_t offset, uint8_t data_length,
                                         uint8_t *att_data);

/*************************************************************
  * This function is called to parse a Binary Content packet *
  ************************************************************/
__weak uint32_t ble_binary_content_parse(uint8_t **buffer_out, uint8_t *buffer_in, uint32_t len);

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_binary_content(ble_notify_event_t event);

/******************************************************************
  * Callback function prototype to manage the write request events *
  *****************************************************************/
__weak void write_request_binary_content_function(uint8_t *received_msg, uint32_t msg_length);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_binary_content(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                            uint8_t data_length, uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init BinaryContent info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for BinaryContent info service
  */
ble_char_object_t *ble_init_binary_content_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Init data structure pointer for BinaryContent info service */
  ble_char_pointer = &ble_char_binary_content;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_binary_content;
  ble_char_pointer->write_request_cb = write_request_binary_content;
  COPY_BINARYCONTENT_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = binary_content_max_char_length;
  ble_char_pointer->char_properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_WRITE_WITHOUT_RESP);
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_ATTRIBUTE_WRITE;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 1;

  BLE_MANAGER_PRINTF("BLE BinaryContent features ok\r\n");

  return ble_char_pointer;
}

/**
  * @brief  BinaryContent Set Max Char Length
  * @param  uint16_t max_char_length
  * @retval none
  */
void ble_binary_content_set_max_char_length(uint16_t max_char_length)
{
  binary_content_max_char_length = max_char_length;
}

/**
  * @brief  BinaryContent Get Max Char Length
  * @param  None
  * @retval uint16_t max_char_length
  */
uint16_t ble_binary_content_get_max_char_length(void)
{
  return binary_content_max_char_length;
}

/**
  * @brief  BinaryContent Send Buffer
  * @param  uint8_t* buffer
  * @param  uint32_t len
  * @retval ble_status_t Status
  */
ble_status_t ble_binary_content_update(uint8_t *buffer, uint8_t len)
{
  ble_status_t ret;
  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_binary_content, 0, len, buffer);

  return ret;

}

/**
  * @brief  BinaryContent Reset Status
  * @param  None
  * @retval None
  */
void ble_binary_content_reset(void)
{
  total_len_decode_packet = 0;
  status_decoding_packet = BLE_COMM_TP_WAIT_START;

  if (ble_command_buffer != NULL)
  {
    BLE_FREE_FUNCTION(ble_command_buffer);
    ble_command_buffer = NULL;
  }
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the GATT attribute
  *         With this function it's possible to understand if BinaryContent is subscribed or not to the one service
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
static void attr_mod_request_binary_content(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                            uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_binary_content(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_binary_content(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t)sprintf((char *)buffer_to_write,
                                      "--->BinaryContent=%s\n",
                                      (att_data[0] == BLE_NOTIFY_SUB) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->BinaryContent=%s", (att_data[0] == BLE_NOTIFY_SUB) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* BLE_DEBUG_LEVEL */
}

/**
  * @brief  This event is given when a write request is received by the server from the client.
  * @param  void *ble_char_pointer
  * @param  uint16_t handle Handle of the attribute
  * @param  uint16_t offset
  * @param  uint8_t data_length
  * @param  uint8_t *att_data
  * @retval None
  */
__weak void write_request_binary_content(void *ble_char_pointer, uint16_t handle, uint16_t offset, uint8_t data_length,
                                         uint8_t *att_data)
{
  uint32_t command_buf_len = 0;

  command_buf_len = ble_binary_content_parse(&ble_command_buffer, att_data, data_length);

  if (command_buf_len > 0U)
  {
    write_request_binary_content_function(ble_command_buffer, command_buf_len);

    BLE_FREE_FUNCTION(ble_command_buffer);
    ble_command_buffer = NULL;
  }
}

/**
  * @brief  This function is called to parse a Binary Content packet.
  * @param  buffer_out: pointer to the output buffer.
  * @param  buffer_in: pointer to the input data.
  * @param  len: buffer in length
  * @retval Buffer out length.
  */
__weak uint32_t ble_binary_content_parse(uint8_t **buffer_out, uint8_t *buffer_in, uint32_t len)
{
  total_len_decode_packet = 0;
  uint32_t buff_out_len = 0;
  status_decoding_packet = BLE_COMM_TP_WAIT_START;
  ble_comm_tp_packet_t packet_type;

  packet_type = (ble_comm_tp_packet_t) buffer_in[0];

  switch (status_decoding_packet)
  {
    case BLE_COMM_TP_WAIT_START:
      if (packet_type == BLE_COMM_TP_START_PACKET)
      {
        /*First part of an BLE Command packet*/
        /*packet is enqueued*/
        uint32_t message_length = buffer_in[1];
        message_length = message_length << 8;
        message_length |= buffer_in[2];

        /*
                To check
                if (*buffer_out != NULL)
                {
                  BLE_FREE_FUNCTION(*buffer_out);
                }
        */

        *buffer_out = (uint8_t *)BLE_MALLOC_FUNCTION((message_length) * sizeof(uint8_t));

        if (*buffer_out == NULL)
        {
          BLE_MANAGER_PRINTF("Error: Mem alloc error [%d]: %d@%s\r\n", message_length, __LINE__, __FILE__);
        }

        memcpy(*buffer_out + total_len_decode_packet, (uint8_t *) &buffer_in[3], (len - 3U));


        total_len_decode_packet += len - 3U;
        status_decoding_packet = BLE_COMM_TP_WAIT_END;
        buff_out_len = 0;
      }
      else if (packet_type == BLE_COMM_TP_START_LONG_PACKET)
      {
        /*First part of an BLE Command packet*/
        /*packet is enqueued*/
        uint32_t message_length = buffer_in[1];
        message_length = message_length << 8;
        message_length |= buffer_in[2];
        message_length = message_length << 8;
        message_length |= buffer_in[3];
        message_length = message_length << 8;
        message_length |= buffer_in[4];


        /*
                To check
                if (*buffer_out != NULL)
                {
                  BLE_FREE_FUNCTION(*buffer_out);
                }
        */

        *buffer_out = (uint8_t *)BLE_MALLOC_FUNCTION((message_length) * sizeof(uint8_t));

        if (*buffer_out == NULL)
        {
          BLE_MANAGER_PRINTF("Error: Mem alloc error [%d]: %d@%s\r\n", message_length, __LINE__, __FILE__);
        }

        memcpy(*buffer_out + total_len_decode_packet, (uint8_t *) &buffer_in[5], (len - 5U));


        total_len_decode_packet += len - 5U;
        status_decoding_packet = BLE_COMM_TP_WAIT_END;
        buff_out_len = 0;
      }
      else if (packet_type == BLE_COMM_TP_START_END_PACKET)
      {
        /*Final part of an BLE Command packet*/
        /*packet is enqueued*/
        uint32_t message_length = buffer_in[1];
        message_length = message_length << 8;
        message_length |= buffer_in[2];

        *buffer_out = (uint8_t *)BLE_MALLOC_FUNCTION((message_length) * sizeof(uint8_t));
        if (*buffer_out == NULL)
        {
          BLE_MANAGER_PRINTF("Error: Mem alloc error [%d]: %d@%s\r\n", message_length, __LINE__, __FILE__);
        }

        memcpy(*buffer_out + total_len_decode_packet, (uint8_t *) &buffer_in[3], (len - 3U));


        total_len_decode_packet += len - 3U;
        /*number of bytes of the output packet*/
        buff_out_len = total_len_decode_packet;
        /*total length set to zero*/
        total_len_decode_packet = 0;
        /*reset status_decoding_packet*/
        status_decoding_packet = BLE_COMM_TP_WAIT_START;
      }
      else
      {
        /* Error */
        buff_out_len = 0;
      }
      break;
    case BLE_COMM_TP_WAIT_END:
      if (packet_type == BLE_COMM_TP_MIDDLE_PACKET)
      {
        /*Central part of an BLE Command packet*/
        /*packet is enqueued*/

        memcpy(*buffer_out + total_len_decode_packet, (uint8_t *) &buffer_in[1], (len - 1U));

        total_len_decode_packet += len - 1U;

        buff_out_len = 0;
      }
      else if (packet_type == BLE_COMM_TP_END_PACKET)
      {
        /*Final part of an BLE Command packet*/
        /*packet is enqueued*/
        memcpy(*buffer_out + total_len_decode_packet, (uint8_t *) &buffer_in[1], (len - 1U));

        total_len_decode_packet += len - 1U;
        /*number of bytes of the output packet*/
        buff_out_len = total_len_decode_packet;
        /*total length set to zero*/
        total_len_decode_packet = 0;
        /*reset status_decoding_packet*/
        status_decoding_packet = BLE_COMM_TP_WAIT_START;
      }
      else
      {
        /*reset status_decoding_packet*/
        status_decoding_packet = BLE_COMM_TP_WAIT_START;
        /*total length set to zero*/
        total_len_decode_packet = 0;

        buff_out_len = 0; /* error */
      }
      break;
  }
  return buff_out_len;
}

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t Event Sub/Unsub
  * @retval None
  */
__weak void notify_event_binary_content(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify binary content function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_binary_content could be implemented in the user file
   */
}

/********************************************************
  * Callback function to manage the write request events *
  *******************************************************/
/**
  * @brief  Callback Function for Binary Content write request.
  * @param  uint8_t *received_msg
  * @param  uint32_t msg_length
  * @retval None
  */
__weak void write_request_binary_content_function(uint8_t *received_msg, uint32_t msg_length)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(received_msg);
  BLE_UNUSED(msg_length);

  BLE_MANAGER_PRINTF("\r\nWrite request binary content function not defined (It is a weak function)\r\n");

  /* NOTE: This function Should not be modified, when the callback is needed,
           the write_request_binary_content_function could be implemented in the user file
   */
}
