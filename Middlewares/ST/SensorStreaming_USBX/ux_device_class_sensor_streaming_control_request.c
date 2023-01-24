/**
 ******************************************************************************
 * @file    ux_device_class_sensor_streaming_control_request.c
 * @author  STMicroelectronics - SRA
 * @version 1.0.0
 * @date    July 15, 2022
 *
 * @brief Device Sensor Streaming Class.
 *        This function manages the based sent by the host on the control ep
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

/**
 * @brief  _ux_device_class_sensor_streaming_control_request
 *
 *	This function manages the based sent by the host on the control ep
 *   endpoints with a CLASS or VENDOR SPECIFIC type.
 *  @param _this [IN] Pointer to sensor_streaming class.
 */
UINT _ux_device_class_sensor_streaming_control_request(UX_SLAVE_CLASS_COMMAND *command)
{
  UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming;
  UX_SLAVE_CLASS *slave_class;
  UINT ret;

  /* Get the class container.  */
  slave_class = command->ux_slave_class_command_class_ptr;

  /* Get the class instance in the container.  */
  sensor_streaming = (UX_SLAVE_CLASS_SENSOR_STREAMING*) slave_class->ux_slave_class_instance;

  ret = _ux_utility_event_flags_set(&sensor_streaming->ux_slave_class_sensor_streaming_event_flags_group, UX_DEVICE_CLASS_SENSOR_STREAMING_WRITE_EVENT, UX_OR);

  return ret;
}
