/**
  ******************************************************************************
  * @file    ble_fft_amplitude.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.1.0
  * @date    11-March-2025
  * @brief   Add BLE FFT amplitude info services using vendor specific profiles.
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
#define COPY_FFT_AMPLITUDE_CHAR_UUID(uuid_struct) COPY_UUID_128(uuid_struct,0x00,0x00,0x00,0x05,0x00,0x02,\
                                                                0x11,0xe1,0xac,0x36,0x00,0x02,0xa5,0xd5,0xc5,0x1b)

/* Private variables ---------------------------------------------------------*/
/* Data structure pointer for FFT Amplitude info service */
static ble_char_object_t ble_char_fft_amplitude;

/************************************************************
  * Callback function prototype to manage the notify events *
  ***********************************************************/
__weak void notify_event_fft_amplitude(ble_notify_event_t event);

/* Private functions prototype -----------------------------------------------*/
static void attr_mod_request_fft_amplitude(void *ble_char_pointer, uint16_t attr_handle, uint16_t offset,
                                           uint8_t data_length, uint8_t *att_data);

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Init FFT Amplitude info service
  * @param  None
  * @retval ble_char_object_t* ble_char_pointer: Data structure pointer for FFT Amplitude info service
  */
ble_char_object_t *ble_init_fft_amplitude_service(void)
{
  /* Data structure pointer for BLE service */
  ble_char_object_t *ble_char_pointer;

  /* Init data structure pointer for FFT Amplitude info service */
  ble_char_pointer = &ble_char_fft_amplitude;
  memset(ble_char_pointer, 0, sizeof(ble_char_object_t));
  ble_char_pointer->attr_mod_request_cb = attr_mod_request_fft_amplitude;
  COPY_FFT_AMPLITUDE_CHAR_UUID((ble_char_pointer->uuid));
  ble_char_pointer->char_uuid_type = UUID_TYPE_128;
  ble_char_pointer->char_value_length = 20;
  ble_char_pointer->char_properties = CHAR_PROP_NOTIFY;
  ble_char_pointer->security_permissions = ATTR_PERMISSION_NONE;
  ble_char_pointer->gatt_evt_mask = GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP;
  ble_char_pointer->enc_key_size = 16;
  ble_char_pointer->is_variable = 1;

  BLE_MANAGER_PRINTF("BLE FFT Amplitude features ok\r\n");

  return ble_char_pointer;
}

/*
 * @brief  Update FFT Amplitude characteristic value
 * @param  uint8_t *data_to_send:
 *                  - 2 bytes for number of samples
 *                  - 1 byte for number of components (up to 3 components)
 *                  - 4 bytes for frequency steps
 *                  - 4 bytes for each component
 *                     (if number of components is more 1, for example 3, send the data in this format:
 *                      X1,X2,X3,...Xn,Y1,Y2,Y3,...Yn,X1,Z2,Z3,...Zn)
 * @param  uint16_t data_number Number of samples
 * @param  uint8_t *sending_fft
 * @param  uint16_t *count_send_data
 * @retval ble_status_t   Status
 */
ble_status_t ble_fft_amplitude_update(uint8_t *data_to_send, uint16_t data_number, uint8_t *sending_fft,
                                      uint16_t *count_send_data)
{
  ble_status_t ret;

  uint16_t total_size;

  uint16_t index;
  uint16_t index_start;
  uint16_t index_stop;

  uint8_t buff[20];

  uint8_t  num_byte_to_sent;

  /* nSample + nComponents + Frequency Steps + Samples */
  total_size = 2U + 1U + 4U + ((data_to_send[2] * data_number) * 4U) ;

  index_start = 20U * (*count_send_data);
  index_stop =  20U * ((*count_send_data) + 1U);

  num_byte_to_sent = 20;

  if (index_stop > total_size)
  {
    index_stop = total_size;
    num_byte_to_sent = (uint8_t)(total_size % num_byte_to_sent);
  }

  for (index = index_start; index < index_stop; index++)
  {
    buff[index - index_start] = data_to_send[index];
  }

  ret = ACI_GATT_UPDATE_CHAR_VALUE(&ble_char_fft_amplitude, 0, num_byte_to_sent, buff);

  if (ret == (ble_status_t)BLE_STATUS_SUCCESS)
  {
    (*count_send_data)++;

    if (index_stop == total_size)
    {
      *sending_fft = 0;
      *count_send_data = 0;
    }
  }

  return ret;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function is called when there is a change on the gatt attribute
  *         With this function it's possible to understand if FFT Amplitude is subscribed or not to the one service
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
static void attr_mod_request_fft_amplitude(void *void_char_pointer, uint16_t attr_handle, uint16_t offset,
                                           uint8_t data_length, uint8_t *att_data)
{
  if (att_data[0] == 01U)
  {
    notify_event_fft_amplitude(BLE_NOTIFY_SUB);
  }
  else if (att_data[0] == 0U)
  {
    notify_event_fft_amplitude(BLE_NOTIFY_UNSUB);
  }

#if (BLE_DEBUG_LEVEL>1)
  if (ble_std_term_service == BLE_SERV_ENABLE)
  {
    bytes_to_write = (uint8_t) sprintf((char *)buffer_to_write,
                                       "--->FFT Amplitude=%s\n",
                                       (att_data[0] == 01U) ? " ON" : " OFF");
    term_update(buffer_to_write, bytes_to_write);
  }
  else
  {
    BLE_MANAGER_PRINTF("--->FFT Amplitude=%s", (att_data[0] == 01U) ? " ON\r\n" : " OFF\r\n");
  }
#endif /* BLE_DEBUG_LEVEL>1 */
}

/**************************************************
  * Callback function to manage the notify events *
  *************************************************/
/**
  * @brief  Callback Function for Un/Subscription Feature
  * @param  ble_notify_event_t event Sub/Unsub
  * @retval None
  */
__weak void notify_event_fft_amplitude(ble_notify_event_t event)
{
  /* Prevent unused argument(s) compilation warning */
  BLE_UNUSED(event);

  if (event == BLE_NOTIFY_SUB)
  {
    BLE_MANAGER_PRINTF("\r\nNotify FFT amplitude function not defined (It is a weak function)\r\n");
  }

  /* NOTE: This function Should not be modified, when the callback is needed,
           the notify_event_fft_amplitude could be implemented in the user file
   */
}
