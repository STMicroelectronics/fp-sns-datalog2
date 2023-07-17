/**
 ******************************************************************************
 * @file    ux_device_class_sensor_streaming_bulkin_thread.c
 * @author  STMicroelectronics - SRA
 * @version 1.0.0
 * @date    July 15, 2022
 *
 * @brief Device Sensor Streaming Class thread for bulk IN endpoints.
 *        One tasks for each EP
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
#include "ux_device_class_sensor_streaming.h"

/**
 * @brief  _ux_device_class_sensor_streaming_bulkin_entry
 *
 * Entry function for each EP thread. Each EP performs the data
 * transmission through its own the thread.
 * @param _this [IN] Pointer to EP specific parameter set in initialization phase.
 */
VOID _ux_device_class_sensor_streaming_bulkin_entry(ULONG ss)
{
  UX_SLAVE_DEVICE *device;
  UINT status;
  ULONG actual_length = 0;
  UX_THREAD_EP_BULKIN_PARAM *param = (UX_THREAD_EP_BULKIN_PARAM*) ss;

  /* Get the pointer to the device.  */
  device = &_ux_system_slave->ux_system_slave_device;

  for(;;)
  {
    /* As long as the device is in the CONFIGURED state.  */
    if(device->ux_slave_device_state == (ULONG)UX_DEVICE_CONFIGURED)
    {
      status = _ux_utility_semaphore_get(&param->semaphore, TX_WAIT_FOREVER);
      /* Check completion status and make sure the message is what we expected. */
      if(status == TX_SUCCESS)
      {
#if defined(ENABLE_THREADX_DBG_PIN) && defined(ENABLE_USBX_EP_DBG_PIN)
        switch(param->endpoint->ux_slave_endpoint_descriptor.bEndpointAddress)
        {
          case 0x81U:
            BSP_DEBUG_PIN_On(USB_EP1_BULKIN_CFG_TAG);
            break;
          case 0x82U:
            BSP_DEBUG_PIN_On(USB_EP2_BULKIN_CFG_TAG);
            break;
          case 0x83U:
            BSP_DEBUG_PIN_On(USB_EP3_BULKIN_CFG_TAG);
            break;
          case 0x84U:
            BSP_DEBUG_PIN_On(USB_EP4_BULKIN_CFG_TAG);
            break;
          case 0x85U:
            BSP_DEBUG_PIN_On(USB_EP5_BULKIN_CFG_TAG);
            break;
          default:
            break;
        }
#endif

        (void)_ux_device_class_sensor_streaming_write(param->endpoint, (uint8_t*) CB_GetItemData(param->ready_item), CB_GetItemSize((CircularBuffer *)param->cbdl2), &actual_length);

#if defined(ENABLE_THREADX_DBG_PIN) && defined(ENABLE_USBX_EP_DBG_PIN)
        switch(param->endpoint->ux_slave_endpoint_descriptor.bEndpointAddress)
        {
          case 0x81U:
            BSP_DEBUG_PIN_Off(USB_EP1_BULKIN_CFG_TAG);
            break;
          case 0x82U:
            BSP_DEBUG_PIN_Off(USB_EP2_BULKIN_CFG_TAG);
            break;
          case 0x83U:
            BSP_DEBUG_PIN_Off(USB_EP3_BULKIN_CFG_TAG);
            break;
          case 0x84U:
            BSP_DEBUG_PIN_Off(USB_EP4_BULKIN_CFG_TAG);
            break;
          case 0x85U:
            BSP_DEBUG_PIN_Off(USB_EP5_BULKIN_CFG_TAG);
            break;
          default:
            break;
        }
#endif
        /* Release the buffer item and reset tx_state */
        CB_ReleaseItem((CircularBuffer *)param->cbdl2, param->ready_item);
        param->tx_state = 0;
      }
    }
  }
}

