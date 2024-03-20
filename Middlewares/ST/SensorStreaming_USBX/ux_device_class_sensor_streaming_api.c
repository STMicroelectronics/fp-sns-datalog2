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
UINT ux_device_class_sensor_streaming_SetTxDataBuffer(UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming, uint8_t ch_number, uint8_t *ptr, uint16_t item_size, uint8_t items)
{
  STREAMING_HandleTypeDef *hwcid = (STREAMING_HandleTypeDef*) sensor_streaming->hwcid;
  UINT res = UX_SUCCESS;
  CircularBufferDL2 *cbdl2 = CBDL2_Alloc(items);
  if(cbdl2 != NULL)
  {
    hwcid->tx_cbdl2[ch_number] = cbdl2;

    /* Initialize the CircularBuffer with the specified parameters */
    (void)CBDL2_Init(cbdl2, ptr, item_size, true);
  }
  else
  {
    res = UX_ERROR;
  }

  return res;
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

  CBDL2_Free(hwcid->tx_cbdl2[ch_number]);
  hwcid->tx_cbdl2[ch_number] = NULL;
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
  UINT res = UX_SUCCESS;
  CircularBufferDL2 *cbdl2 = hwcid->tx_cbdl2[ch_number];
  bool item_ready;

  if(hwcid->streaming_status == STREAMING_STATUS_STARTED)
  {
    if(CBDL2_FillCurrentItem(cbdl2, ch_number, buf, size, &item_ready) != SYS_NO_ERROR_CODE)
    {
      res = UX_ERROR;
    }
  }
  else
  {
    res = UX_INVALID_STATE;
  }

  return res;
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
