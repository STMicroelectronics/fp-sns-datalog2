/**
 ******************************************************************************
 * @file    ux_device_class_sensor_streaming_thread.c
 * @author  STMicroelectronics - SRA
 * @version 1.0.0
 * @date    July 15, 2022
 *
 * @brief Device Sensor Streaming Class
 *        This function is the thread of the sensor_streaming class.
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
#include "ux_device_stack.h"

/* Remove compiling warning. */
VOID _ux_device_class_sensor_streaming_thread(ULONG sensor_streaming_class);

/**
 * @brief  _ux_device_class_sensor_streaming_thread
 * This function is the thread of the sensor_streaming class.
 * @param _this [IN] Address of sensor_streaming class container .
 * @return Completion Status
 */
VOID _ux_device_class_sensor_streaming_thread(ULONG sensor_streaming_class)
{

  UX_SLAVE_CLASS *class;
  UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming;
  UX_SLAVE_TRANSFER *transfer_request;
  UX_SLAVE_DEVICE *device;
  ULONG request;
  ULONG value;
  ULONG request_length;
  uint8_t request_type;
  uint16_t request_index;
  ULONG actual_flags;

  for(;;)
  {
    UX_THREAD_EXTENSION_PTR_GET(class, UX_SLAVE_CLASS, sensor_streaming_class)

    /* Get the sensor_streaming instance from this class container.  */
    sensor_streaming = (UX_SLAVE_CLASS_SENSOR_STREAMING*) class->ux_slave_class_instance;

    /* Wait forever on the thread */
    _ux_utility_event_flags_get(&sensor_streaming->ux_slave_class_sensor_streaming_event_flags_group,
                                UX_DEVICE_CLASS_SENSOR_STREAMING_WRITE_EVENT,
                                UX_OR_CLEAR, &actual_flags, UX_WAIT_FOREVER);

    /* Get the pointer to the device.  */
    device = &_ux_system_slave->ux_system_slave_device;

    /* As long as the device is in the CONFIGURED state.  */
    if(device->ux_slave_device_state == (ULONG) UX_DEVICE_CONFIGURED)
    {

      /* Cast properly the dpump instance.  */
      UX_THREAD_EXTENSION_PTR_GET(class, UX_SLAVE_CLASS, sensor_streaming_class);

      /* Get the dpump instance from this class container.  */
      sensor_streaming = (UX_SLAVE_CLASS_SENSOR_STREAMING*) class->ux_slave_class_instance;

      /* Get the pointer to the transfer request associated with the control endpoint.  */
      transfer_request = &device->ux_slave_device_control_endpoint.ux_slave_endpoint_transfer_request;

      /* Extract all necessary fields of the request.  */

      request_type = *(transfer_request->ux_slave_transfer_request_setup + UX_SETUP_REQUEST_TYPE);
      request = *(transfer_request->ux_slave_transfer_request_setup + UX_SETUP_REQUEST);
      value = _ux_utility_short_get(transfer_request->ux_slave_transfer_request_setup + UX_SETUP_VALUE);
      request_length = _ux_utility_short_get(transfer_request->ux_slave_transfer_request_setup + UX_SETUP_LENGTH);
      request_index = (USHORT) _ux_utility_short_get(transfer_request->ux_slave_transfer_request_setup + UX_SETUP_INDEX);

      if(request_length != 0U)
      {
        if((request_type & 0x80U) != 0U) /* GET */
        {
          /* Invoke the application.  */
          sensor_streaming->ux_slave_class_sensor_streaming_parameter->ux_slave_class_sensor_streaming_instance_control(
              sensor_streaming->ux_slave_class_sensor_streaming_parameter->app_obj_ctrl, 0, request, value, request_index,
              (uint8_t*) transfer_request->ux_slave_transfer_request_data_pointer, request_length);

          /* Set the phase of the transfer to data out.  */
          transfer_request->ux_slave_transfer_request_phase = UX_TRANSFER_PHASE_DATA_OUT;

          /* Perform the data transfer.  */
          ULONG transmit_length = request_length; //TODO check difference between transmit and request (here could be the same value)
          (void)_ux_device_stack_transfer_request(transfer_request, transmit_length, request_length);
        }
        else /* SET */
        {
          transfer_request->ux_slave_transfer_request_phase = UX_TRANSFER_PHASE_DATA_IN;

          /* Perform the data transfer.  */
          (void)_ux_device_stack_transfer_request(transfer_request, request_length, request_length);

          /* Invoke the application.  */
          sensor_streaming->ux_slave_class_sensor_streaming_parameter->ux_slave_class_sensor_streaming_instance_control(
              sensor_streaming->ux_slave_class_sensor_streaming_parameter->app_obj_ctrl, 1, request, value, request_index,
              (uint8_t*) transfer_request->ux_slave_transfer_request_data_pointer, request_length);
        }
      }
    }
  }
}

