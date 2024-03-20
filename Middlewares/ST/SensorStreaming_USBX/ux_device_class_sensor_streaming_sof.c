/**
 ******************************************************************************
 * @file    ux_device_class_sensor_streaming_sof.c
 * @author  STMicroelectronics - SRA
 * @version 1.0.0
 * @date    May 10, 2023
 *
 * @brief USB SOF callback implementation
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
#define UX_DCD_STM32_SOURCE_CODE

/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_dcd_stm32.h"
#include "ux_device_stack.h"
#include "ux_utility.h"

#include "ux_device_class_sensor_streaming.h"

static UINT _SetTxBuffer(UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming, CircularBufferDL2 *cbdl2, CBItem *item, uint8_t epVectorIndex);
static UINT _TransmitPacket(UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming, uint8_t epVectorIndex);

UINT _ux_device_class_sensor_streaming_sof(void)
{
  UX_SLAVE_DEVICE *device;
  UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming;
  UINT ret = UX_SUCCESS;

  /* Get the pointer to the device.  */
  device = &_ux_system_slave->ux_system_slave_device;

  /* As long as the device is in the CONFIGURED state.  */
  if(device->ux_slave_device_state == (ULONG) UX_DEVICE_CONFIGURED)
  {
    /* Get the sensor_streaming instance from this class container.  */
    sensor_streaming = (UX_SLAVE_CLASS_SENSOR_STREAMING*) device->ux_slave_device_first_interface->ux_slave_interface_class_instance;

    if(sensor_streaming != NULL)
    {
      uint8_t i, ep;
      CircularBufferDL2 *cbdl2;
      CBItem *p_item;
      STREAMING_HandleTypeDef *hwcid = sensor_streaming->hwcid;
      uint8_t *status = &hwcid->streaming_status;

      UX_THREAD_EP_BULKIN_PARAM *ep_bulk;

      if(*status == STREAMING_STATUS_STARTED)
      {
        for(i = 0; i < SS_N_CHANNELS_MAX; i++)
        {
          cbdl2 = hwcid->tx_cbdl2[i];
          ep = hwcid->ep_map[i];
          if((cbdl2 != NULL) && (ep != SS_ENDPOINT_NOT_ASSIGNED))
          {
            ep_bulk = &sensor_streaming->ux_slave_class_sensor_streaming_bulkin[ep].ep_param;
            if(ep_bulk->tx_state != 1U)
            {
              if(CB_GetReadyItemFromTail((CircularBuffer*) cbdl2, &p_item) == SYS_NO_ERROR_CODE)
              {
                /* Send data buffer */
                (void) _SetTxBuffer(sensor_streaming, cbdl2, p_item, ep);

                if(_TransmitPacket(sensor_streaming, ep) != (UINT) UX_SUCCESS)
                {
                  ret = UX_TRANSFER_ERROR;
                }
              }
            }
          }
        }
      }
      else if(*status == STREAMING_STATUS_STOPPING)
      {
        ;
      }
      else
      {
        /* Nothing to do */
      }
    }
  }
  return ret; /* USBD_OK */
}


/**
 * @brief  _SetTxBuffer
 * enqueue a packet on the specific endpoint.
 * @param _this [IN] Pointer to sensor_streaming command.
 * @param pbuff [IN] pointer to buffer
 * @param length [IN] size of buffer
 * @param epVectorIndex [IN] endpoint index number
 * @return Completion Status
 */
static UINT _SetTxBuffer(UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming, CircularBufferDL2 *cbdl2, CBItem *item, uint8_t epVectorIndex)
{
  UX_THREAD_EP_BULKIN_PARAM *ep_bulk;
  ep_bulk = &sensor_streaming->ux_slave_class_sensor_streaming_bulkin[epVectorIndex].ep_param;
  ep_bulk->cbdl2 = cbdl2;
  ep_bulk->ready_item = item;

  return UX_SUCCESS; /* USBD_OK */
}

/**
 * @brief  _TransmitPacket
 * enqueue a packet on the specific endpoint.
 * @param _this [IN] Pointer to sensor_streaming command.
 * @param epVectorIndex [IN] endpoint index number
 *
 * @return Completion Status
 */
static UINT _TransmitPacket(UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming, uint8_t epVectorIndex)
{
  UINT status;
  UINT ret = UX_SUCCESS;
  UX_THREAD_EP_BULKIN_PARAM *ep_bulk;
  ep_bulk = &sensor_streaming->ux_slave_class_sensor_streaming_bulkin[epVectorIndex].ep_param;

  if(ep_bulk->tx_state == 0U)
  {
    /* Tx Transfer in progress */
    ep_bulk->tx_state = 1U;
    /* Unlock the EP thread */
    status = _ux_utility_semaphore_put(&ep_bulk->semaphore);
    /* Check completion status. */
    if(status != TX_SUCCESS)
    {
      ret = UX_TRANSFER_NOT_READY;
    }
  }
  else
  {
    ret = UX_TRANSFER_NOT_READY;
  }
  return ret;
}
