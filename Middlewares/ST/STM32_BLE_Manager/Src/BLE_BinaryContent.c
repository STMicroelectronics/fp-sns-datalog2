/**
  ******************************************************************************
  * @file    BLE_BinaryContent.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.11.0
  * @date    15-February-2024
  * @brief   Add BinaryContent info services using vendor specific profile.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#define COPY_BINARYCONTENT_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x22,0x00,0x02,0x11,\
                                                                0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Exported Variables ------------------------------------------------------- */
/* Identifies the notification Events */
CustomNotifyEventBinaryContent_t CustomNotifyEventBinaryContent = NULL;
CustomWriteRequestBinaryContent_t CustomWriteRequestBinaryContent = NULL;

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for BinaryContent info service */
static BleCharTypeDef BleCharBinaryContent;
/* Buffer used to save the complete command received via BLE*/
static uint8_t *ble_command_buffer = NULL;

static uint16_t BinaryContentMaxCharLength = DEFAULT_MAX_BINARY_CONTENT_CHAR_LEN;

static uint32_t TotalLenDecodePacket = 0;
static BLE_COMM_TP_Status_Typedef StatusDecodingPacket = BLE_COMM_TP_WAIT_START;

/* Private functions ---------------------------------------------------------*/
static void AttrMod_Request_BinaryContent(void *BleCharPointer, uint16_t attr_handle, uint16_t Offset,
                                          uint8_t data_length, uint8_t *att_data);

/**
  * @brief  Init BinaryContent info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for BinaryContent info service
  */
BleCharTypeDef *BLE_InitBinaryContentService(void)
{
  /* Data structure pointer for BLE service */
  BleCharTypeDef *BleCharPointer;

  /* Init data structure pointer for BinaryContent info service */
  BleCharPointer = &BleCharBinaryContent;
  memset(BleCharPointer, 0, sizeof(BleCharTypeDef));
  BleCharPointer->AttrMod_Request_CB = AttrMod_Request_BinaryContent;
  BleCharPointer->Write_Request_CB = Write_Request_BinaryContent;
  COPY_BINARYCONTENT_CHAR_UUID((BleCharPointer->uuid));
  BleCharPointer->Char_UUID_Type = UUID_TYPE_128;
  BleCharPointer->Char_Value_Length = BinaryContentMaxCharLength;
  BleCharPointer->Char_Properties = ((uint8_t)CHAR_PROP_NOTIFY) | ((uint8_t)CHAR_PROP_WRITE_WITHOUT_RESP);
  BleCharPointer->Security_Permissions = ATTR_PERMISSION_NONE;
  BleCharPointer->GATT_Evt_Mask = GATT_NOTIFY_ATTRIBUTE_WRITE;
  BleCharPointer->Enc_Key_Size = 16;
  BleCharPointer->Is_Variable = 1;

  BLE_MANAGER_PRINTF("BLE BinaryContent features ok\r\n");

  return BleCharPointer;
}

/**
  * @brief  BinaryContent Set Max Char Length
  * @param  uint16_t MaxCharLength
  * @retval none
  */
void BLE_BinaryContentSetMaxCharLength(uint16_t MaxCharLength)
{
  BinaryContentMaxCharLength = MaxCharLength;
}

/**
  * @brief  BinaryContent Get Max Char Length
  * @param  None
  * @retval uint16_t MaxCharLength
  */
uint16_t BLE_BinaryContentGetMaxCharLength(void)
{
  return BinaryContentMaxCharLength;
}

/**
  * @brief  BinaryContent Send Buffer
  * @param  uint8_t* buffer
  * @param  uint32_t len
  * @retval tBleStatus Status
  */
tBleStatus BLE_BinaryContentUpdate(uint8_t *buffer, uint8_t len)
{
  tBleStatus ret;
  ret = ACI_GATT_UPDATE_CHAR_VALUE(&BleCharBinaryContent, 0, len, buffer);

  return ret;

}

/**
  * @brief  This function is called when there is a change on the GATT attribute
  *         With this function it's possible to understand if BinaryContent is subscribed or not to the one service
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
static void AttrMod_Request_BinaryContent(void *VoidCharPointer, uint16_t attr_handle, uint16_t Offset,
                                          uint8_t data_length, uint8_t *att_data)
{
  if (CustomNotifyEventBinaryContent != NULL)
  {
    if (att_data[0] == 01U)
    {
      CustomNotifyEventBinaryContent(BLE_NOTIFY_SUB);
    }
    else if (att_data[0] == 0U)
    {
      CustomNotifyEventBinaryContent(BLE_NOTIFY_UNSUB);
    }
  }

#if (BLE_DEBUG_LEVEL>1)
  if (BLE_StdTerm_Service == BLE_SERV_ENABLE)
  {
    BytesToWrite = (uint8_t)sprintf((char *)BufferToWrite,
                                    "--->BinaryContent=%s\n",
                                    (att_data[0] == BLE_NOTIFY_SUB) ? " ON" : " OFF");
    Term_Update(BufferToWrite, BytesToWrite);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->BinaryContent=%s", (att_data[0] == BLE_NOTIFY_SUB) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* BLE_DEBUG_LEVEL */
}

/**
  * @brief  This event is given when a read request is received by the server from the client.
  * @param  void *BleCharPointer
  * @param  uint16_t handle Handle of the attribute
  * @param  uint16_t Offset
  * @param  uint8_t data_length
  * @param  uint8_t *att_data
  * @retval None
  */
__weak void Write_Request_BinaryContent(void *BleCharPointer, uint16_t handle, uint16_t Offset, uint8_t data_length,
                                        uint8_t *att_data)
{
  uint32_t CommandBufLen = 0;

  if (CustomWriteRequestBinaryContent != NULL)
  {
    CommandBufLen = BLE_BinaryContent_Parse(&ble_command_buffer, att_data, data_length);

    if (CommandBufLen > 0U)
    {
      CustomWriteRequestBinaryContent(ble_command_buffer, CommandBufLen);

      BLE_FREE_FUNCTION(ble_command_buffer);
      ble_command_buffer = NULL;
    }
  }
  else
  {
    BLE_MANAGER_PRINTF("\r\n\nWrite request BinaryContent function not defined\r\n\n");
  }
}

/**
  * @brief  This function is called to parse a Binary Content packet.
  * @param  buffer_out: pointer to the output buffer.
  * @param  buffer_in: pointer to the input data.
  * @param  len: buffer in length
  * @retval Buffer out length.
  */
__weak uint32_t BLE_BinaryContent_Parse(uint8_t **buffer_out, uint8_t *buffer_in, uint32_t len)
{
  TotalLenDecodePacket = 0;
  uint32_t buff_out_len = 0;
  StatusDecodingPacket = BLE_COMM_TP_WAIT_START;
  BLE_COMM_TP_Packet_Typedef packet_type;

  packet_type = (BLE_COMM_TP_Packet_Typedef) buffer_in[0];

  switch (StatusDecodingPacket)
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

        memcpy(*buffer_out + TotalLenDecodePacket, (uint8_t *) &buffer_in[3], (len - 3U));


        TotalLenDecodePacket += len - 3U;
        StatusDecodingPacket = BLE_COMM_TP_WAIT_END;
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

        memcpy(*buffer_out + TotalLenDecodePacket, (uint8_t *) &buffer_in[5], (len - 5U));


        TotalLenDecodePacket += len - 5U;
        StatusDecodingPacket = BLE_COMM_TP_WAIT_END;
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

        memcpy(*buffer_out + TotalLenDecodePacket, (uint8_t *) &buffer_in[3], (len - 3U));


        TotalLenDecodePacket += len - 3U;
        /*number of bytes of the output packet*/
        buff_out_len = TotalLenDecodePacket;
        /*total length set to zero*/
        TotalLenDecodePacket = 0;
        /*reset StatusDecodingPacket*/
        StatusDecodingPacket = BLE_COMM_TP_WAIT_START;
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

        memcpy(*buffer_out + TotalLenDecodePacket, (uint8_t *) &buffer_in[1], (len - 1U));

        TotalLenDecodePacket += len - 1U;

        buff_out_len = 0;
      }
      else if (packet_type == BLE_COMM_TP_END_PACKET)
      {
        /*Final part of an BLE Command packet*/
        /*packet is enqueued*/
        memcpy(*buffer_out + TotalLenDecodePacket, (uint8_t *) &buffer_in[1], (len - 1U));

        TotalLenDecodePacket += len - 1U;
        /*number of bytes of the output packet*/
        buff_out_len = TotalLenDecodePacket;
        /*total length set to zero*/
        TotalLenDecodePacket = 0;
        /*reset StatusDecodingPacket*/
        StatusDecodingPacket = BLE_COMM_TP_WAIT_START;
      }
      else
      {
        /*reset StatusDecodingPacket*/
        StatusDecodingPacket = BLE_COMM_TP_WAIT_START;
        /*total length set to zero*/
        TotalLenDecodePacket = 0;

        buff_out_len = 0; /* error */
      }
      break;
  }
  return buff_out_len;
}

/**
  * @brief  BinaryContent Reset Status
  * @param  None
  * @retval None
  */
void BLE_BinaryContentReset(void)
{
  TotalLenDecodePacket = 0;
  StatusDecodingPacket = BLE_COMM_TP_WAIT_START;
  
  if(ble_command_buffer!=NULL) {
    BLE_FREE_FUNCTION(ble_command_buffer);
    ble_command_buffer = NULL;
  }
}
