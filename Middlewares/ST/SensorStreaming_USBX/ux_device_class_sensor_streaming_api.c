/**
 ******************************************************************************
 * @file    ux_device_class_sensor_streaming_api.c
 * @author  STMicroelectronics - SRA
 * @version 1.0.0
 * @date    July 15, 2022
 *
 * @brief Device Sensor Streaming Class API
 *
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 ******************************************************************************
 */

#define UX_SOURCE_CODE

/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_stack.h"
#include "ux_device_class_sensor_streaming.h"

#define HEADER_CH_SIZE        1U
#define HEADER_COUNTER_SIZE   4U
#define HEADER_SIZE           (HEADER_CH_SIZE + HEADER_COUNTER_SIZE)

/**
 * @brief  ux_device_class_sensor_streaming_SetTransmissionEP
 *
 * This function maps streaming id and EP.
 * @param _this [IN] Pointer to sensor_streaming class.
 * @param id [IN] streaming id.
 *  @param ep [IN] ep.
 * @return Completion Status
 */
UINT ux_device_class_sensor_streaming_SetTransmissionEP(UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming, uint8_t id, uint8_t ep)
{
  UINT ret = UX_SUCCESS;

  if(sensor_streaming == NULL)
  {
    ret = UX_ERROR;
  }
  else
  {
    if(id > SS_N_CHANNELS_MAX)
    {
      ret = UX_ERROR;
    }
    if(ep > (SS_N_IN_ENDPOINTS - 1U))
    {
      ret = UX_ERROR;
    }
    sensor_streaming->hwcid->ep_map[id] = ep;
  }
  return ret;
}

/**
 * @brief  ux_device_class_sensor_streaming_SetRxDataBuffer
 *
 * This function Sets Rx Buffer.
 * @param _this [IN] Pointer to sensor_streaming class.
 * @param  ptr: Rx Buffer
 * @return Completion Status
 */
UINT ux_device_class_sensor_streaming_SetRxDataBuffer(UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming, uint8_t *ptr)
{
  sensor_streaming->hwcid->rx_buffer = ptr;
  return (UX_SUCCESS);
}

/**
 * @brief  ux_device_class_sensor_streaming_SetTxDataBuffer
 *         Set buffer data for a specific channel.
 *         A data "frame" of size bytes will be sent on the USB endpoints
 * @param _this [IN] Pointer to sensor_streaming class.
 * @param  ch_number: channel number
 * @param  ptr: pointer to data
 *              NOTE! Memory allocated must be at least equal to (size * 2) + 2 + 4;
 * @param  size: length of each packet in bytes
 * @retval status
 */
UINT ux_device_class_sensor_streaming_SetTxDataBuffer(UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming, uint8_t id, uint8_t *ptr, uint16_t size)
{
  STREAMING_HandleTypeDef *hwcid = (STREAMING_HandleTypeDef*) sensor_streaming->hwcid;
  uint8_t **tx_buffer = hwcid->tx_buffer;
  uint32_t *tx_buff_idx = hwcid->tx_buff_idx;
  uint16_t *ch_data_size = hwcid->ch_data_size;
  uint32_t *p_src;

  tx_buffer[id] = ptr;
  p_src = (uint32_t*) tx_buffer[id];
  /* Double buffer contains 2 * usb data packet + 1st byte (id) + pkt counter for each half */
  ch_data_size[id] = (size * 2U) + (HEADER_SIZE * 2U);
  /* write the id at the beginning of the buffer (first byte) */
  ptr[0] = id;
  /* initialize pkt counter */
  p_src = (uint32_t*) &((uint8_t*) (p_src))[HEADER_CH_SIZE];
  *p_src = 0U;
  /* move buffer index after header */
  tx_buff_idx[id] = HEADER_SIZE;

  return (UX_SUCCESS);
}

/**
 * @brief  ux_device_class_sensor_streaming_CleanTxDataBuffer
 *         discard remaining data in the buffer of the specified ch
 * @param _this [IN] Pointer to sensor_streaming class.
 * @param  ch_number: channel number
 * @retval status
 */
UINT ux_device_class_sensor_streaming_CleanTxDataBuffer(UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming, uint8_t ch_number)
{
  STREAMING_HandleTypeDef *hwcid = (STREAMING_HandleTypeDef*) sensor_streaming->hwcid;
  hwcid->tx_buff_reset[ch_number] = 1;
  return (UX_SUCCESS);
}

/**
 * @brief  ux_device_class_sensor_streaming_FillTxDataBuffer
 *         Enqueue data on a specific channel
 * @param _this [IN] Pointer to sensor_streaming class.
 * @param  ch_number: channel number to be used
 * @param  buf: pointer to data to be sent
 * @param  size: length of data in bytes
 * @retval status
 */
UX_INTERRUPT_SAVE_AREA //used by UX_DISABLE and UX_ENABLE isr
UINT ux_device_class_sensor_streaming_FillTxDataBuffer(UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming, uint8_t ch_number, uint8_t *buf, uint32_t size)
{
  STREAMING_HandleTypeDef *hwcid = (STREAMING_HandleTypeDef*) sensor_streaming->hwcid;
  uint8_t *p_dst = hwcid->tx_buffer[ch_number];
  uint32_t *p_byte_counter_dst;
  uint32_t dst_size = hwcid->ch_data_size[ch_number];
  uint32_t dst_idx = hwcid->tx_buff_idx[ch_number];
  uint32_t src_idx = 0;
  uint32_t half_size = dst_size / 2U;
  uint8_t mode;

  if(hwcid->tx_buff_reset[ch_number] == 1U)
  {
    /* write ch_number at the beginning of the buffer (first byte) */
    p_dst[0] = ch_number;

    /* initialize pkt counter to zero */
    p_byte_counter_dst = (uint32_t*) &p_dst[1];
    *p_byte_counter_dst = 0U;

    /* Move buffer index to position 5
     * 1 Byte: channel number
     * 4 Bytes: Byte counter to detect data loss on the host
     * */
    dst_idx = HEADER_SIZE;

    UX_DISABLE
    hwcid->tx_buff_status[ch_number] = 0U;
    UX_RESTORE

    hwcid->tx_byte_counter[ch_number] = 0U;
    hwcid->tx_buff_reset[ch_number] = 0U;
  }

  /* if the half or end buffer is not reached after the copy, use memcpy */
  uint32_t size_after_copy = dst_idx + size;

  if(size_after_copy < half_size) /* data to be copied won't exceed the first half of the dest buffer */
  {
    mode = 1U; /* use memcpy*/
  }
  else if(size_after_copy < dst_size) /* data to be copied won't exceed the end of the buffer*/
  {
    if(dst_idx < half_size) /* data to be copied exceeds the first half of the dest buffer*/
    {
      mode = 0U; /* bytewise copy */
    }
    else /* data to be copied won't exceed the end of the buffer*/
    {
      mode = 1U; /* use memcpy*/
    }
  }
  else /* data to be copied exceeds the end of the buffer*/
  {
    mode = 0U; /* bytewise copy */
  }

  if(mode == 0U) /* bytewise copy */
  {
    while(src_idx < size)
    {
      p_dst[dst_idx++] = buf[src_idx++];

      if(dst_idx >= dst_size)
      {
        UX_DISABLE
        hwcid->tx_buff_status[ch_number] = 2U;
        UX_RESTORE

        /* write ch_number at the beginning of the buffer (first byte) */
        p_dst[0] = ch_number;

        /* incrementing pkt counter by the size of data in half buffer */
        hwcid->tx_byte_counter[ch_number] += (half_size - HEADER_SIZE);
        p_byte_counter_dst = (uint32_t*) &p_dst[HEADER_CH_SIZE];
        *p_byte_counter_dst = hwcid->tx_byte_counter[ch_number];

        /* move buffer index after header */
        dst_idx = HEADER_SIZE;
      }
      else if(dst_idx==half_size)
      {
        UX_DISABLE
        hwcid->tx_buff_status[ch_number] = 1U;
        UX_RESTORE

        /* write ch_number at the beginning of the second half of the buffer */
        p_dst[dst_idx] = ch_number;

        /* incrementing pkt counter by the size of data in half buffer */
        hwcid->tx_byte_counter[ch_number] += (half_size - HEADER_SIZE);
        p_byte_counter_dst = (uint32_t*) &p_dst[dst_idx + HEADER_CH_SIZE];
        *p_byte_counter_dst = hwcid->tx_byte_counter[ch_number];

        /* move buffer index after header */
        dst_idx = (dst_idx + HEADER_SIZE);
      }
      else
      {
        /* Nothing to do*/
      }
    }
  }
  else
  {
    (void) memcpy(&p_dst[dst_idx], buf, size);
    dst_idx += size;
  }
  hwcid->tx_buff_idx[ch_number] = dst_idx;
  return (UX_SUCCESS);
}

/**
 * @brief  ux_device_class_sensor_streaming_StartStreaming
 *         Enable USB streaming.
 * @param _this [IN] Pointer to sensor_streaming class.
 * @retval status
 */
UINT ux_device_class_sensor_streaming_StartStreaming(UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming)
{
  STREAMING_HandleTypeDef *hwcid = (STREAMING_HandleTypeDef*) sensor_streaming->hwcid;
  uint8_t *status = &(hwcid->streaming_status);

  for(uint8_t i = 0; i < (SS_N_IN_ENDPOINTS); i++)
  {
    sensor_streaming->ux_slave_class_sensor_streaming_bulkin[i].ep_param.last_packet_sent = 1;
    (void) _ux_utility_thread_resume(&sensor_streaming->ux_slave_class_sensor_streaming_bulkin[i].thread);
  }
  *status = STREAMING_STATUS_STARTED;

  return (UX_SUCCESS);
}

/**
 * @brief  ux_device_class_sensor_streaming_StopStreaming
 *         Disable USB streaming.
 * @param _this [IN] Pointer to sensor_streaming class.
 * @retval status
 */
UINT ux_device_class_sensor_streaming_StopStreaming(UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming)
{
  STREAMING_HandleTypeDef *hwcid = (STREAMING_HandleTypeDef*) sensor_streaming->hwcid;

  uint8_t *status = &(hwcid->streaming_status);
  *status = STREAMING_STATUS_STOPPING;

  return (UX_SUCCESS);
}

/**
 * @brief  ux_device_class_sensor_streaming_stack_class_register
 *         Store class parameters
 * @param _this [IN] Pointer to sensor_streaming class.
 * @retval status
 */
UINT ux_device_class_sensor_streaming_stack_class_register(void *ss_class_parameter)
{
  return _ux_device_stack_class_register(_ux_system_slave_class_sensor_streaming_name, _ux_device_class_sensor_streaming_entry, 1, 0,
                                         (VOID*) ss_class_parameter);
}
