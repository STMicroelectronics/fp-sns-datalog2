/**
 ******************************************************************************
 * @file    ux_device_class_sensor_streaming_entry.c
 * @author  STMicroelectronics - SRA
 * @version 1.0.0
 * @date    July 15, 2022
 *
 * @brief Device Sensor Streaming Class
 *        This function is the entry point of the device sensor_streaming class.
 *        It will be called by the device stack enumeration module when the
 *        host has sent a SET_CONFIGURATION command and the sensor_streaming interface
 *        needs to be mounted
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
 * @brief  _ux_device_class_sensor_streaming_entry
 * This function is the entry point of the device sensor_streaming class. It
 * will be called by the device stack enumeration module when the
 * host has sent a SET_CONFIGURATION command and the sensor_streaming interface
 * needs to be mounted.
 * @param _this [IN] Pointer to sensor_streaming command.
 *
 * @return Completion Status
 */
UINT _ux_device_class_sensor_streaming_entry(UX_SLAVE_CLASS_COMMAND *command)
{
  UINT ret;

  /* The command request will tell us we need to do here, either a enumeration
   query, an activation or a deactivation.  */
  switch(command->ux_slave_class_command_request)
  {
    case UX_SLAVE_CLASS_COMMAND_INITIALIZE:

      /* Call the init function of the SENSOR_STREAMING class.  */
      ret = _ux_device_class_sensor_streaming_initialize(command);
      break;

    case UX_SLAVE_CLASS_COMMAND_QUERY:

      /* Check the CLASS definition in the interface descriptor. */
      if(command->ux_slave_class_command_class == UX_SLAVE_CLASS_SENSOR_STREAMING_CLASS)
      {
        ret = (UINT) UX_SUCCESS;
      }
      else
      {
        ret = (UINT) UX_NO_CLASS_MATCH;
      }
      break;

    case UX_SLAVE_CLASS_COMMAND_ACTIVATE:

      /* The activate command is used when the host has sent a SET_CONFIGURATION command
       and this interface has to be mounted. Both Bulk endpoints have to be mounted
       and the dpump thread needs to be activated.  */
      ret = _ux_device_class_sensor_streaming_activate(command);
      break;

    case UX_SLAVE_CLASS_COMMAND_CHANGE:

      /* The change command is used when the host has sent a SET_INTERFACE command
         to go from Alternate Setting 0 to 1 or revert to the default mode.  */

      ret = (UINT) UX_SUCCESS;
      break;

    case UX_SLAVE_CLASS_COMMAND_DEACTIVATE:

      /* The deactivate command is used when the device has been extracted.
         The device endpoints have to be dismounted and the dpump thread canceled.  */
      ret = _ux_device_class_sensor_streaming_deactivate(command);
      break;

    case UX_SLAVE_CLASS_COMMAND_REQUEST:

      ret = _ux_device_class_sensor_streaming_control_request(command);
      break;

    default:

      /* Error trap. */
      _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_CLASS, UX_FUNCTION_NOT_SUPPORTED);

      /* If trace is enabled, insert this event into the trace buffer.  */
      UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_FUNCTION_NOT_SUPPORTED, 0, 0, 0, UX_TRACE_ERRORS, 0, 0)

      /* Return an error.  */
      ret = (UINT) UX_FUNCTION_NOT_SUPPORTED;
      break;

  }
  return ret;
}

