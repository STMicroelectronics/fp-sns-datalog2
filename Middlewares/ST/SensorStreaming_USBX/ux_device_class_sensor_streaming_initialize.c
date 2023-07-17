/**
 ******************************************************************************
 * @file    ux_device_class_sensor_streaming_initialize.c
 * @author  STMicroelectronics - SRA
 * @version 1.0.0
 * @date    July 15, 2022
 *
 * @brief Device Sensor Streaming Class
 *        This function initializes the USB sensor_streaming device.
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

#include "ux_device_class_sensor_streaming.h"

UCHAR _ux_system_slave_class_sensor_streaming_name[24] = "sensors_streaming_class";

/**
 * @brief  _ux_device_class_sensor_streaming_initialize
 * This function initializes the USB sensor_streaming device.
 * @param _this [IN] Pointer to sensor_streaming command.
 *
 * @return Completion Status
 */
UINT _ux_device_class_sensor_streaming_initialize(UX_SLAVE_CLASS_COMMAND *command)
{
  UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming;
  UX_SLAVE_CLASS *slave_class;
  UINT status;
  UINT i;

  /* Get the class container.  */
  slave_class = command->ux_slave_class_command_class_ptr;

  /* Create an instance of the device sensor_streaming class. */
  sensor_streaming = _ux_utility_memory_allocate(UX_NO_ALIGN, UX_REGULAR_MEMORY, sizeof(UX_SLAVE_CLASS_SENSOR_STREAMING));

  /* No need to check for errors as _ux_utility_memory_allocate doesn't return any */

  sensor_streaming->hwcid = _ux_utility_memory_allocate(UX_NO_ALIGN, UX_REGULAR_MEMORY, sizeof(STREAMING_HandleTypeDef));

  /* Save the address of the DPUMP instance inside the DPUMP container.  */
  slave_class->ux_slave_class_instance = (VOID*) sensor_streaming;

  /* Allocate some memory for the thread stack. */
  slave_class->ux_slave_class_thread_stack = _ux_utility_memory_allocate(UX_NO_ALIGN, UX_REGULAR_MEMORY, SS_CLASS_THREAD_STACK_SIZE);

  /* Create a event flag group for the cdc_acm class to synchronize with the application writing event .  */
  status = _ux_utility_event_flags_create(&sensor_streaming->ux_slave_class_sensor_streaming_event_flags_group, "ux_device_class_sensor_streaming_event_flag");

  status = _ux_utility_thread_create(&slave_class->ux_slave_class_thread, "ux_slave_class_thread", _ux_device_class_sensor_streaming_thread,
                                     (ULONG) (ALIGN_TYPE) slave_class, (VOID*) slave_class->ux_slave_class_thread_stack,
                                     SS_CLASS_THREAD_STACK_SIZE,
                                     SS_CLASS_THREAD_PRIO,
                                     SS_CLASS_THREAD_PRIO,
                                     UX_NO_TIME_SLICE, UX_DONT_START);

  /* Check the creation of this thread.  */
  if(status != (UINT) UX_SUCCESS)
  {
    return status;
  }

  UX_THREAD_EXTENSION_PTR_SET(&(slave_class -> ux_slave_class_thread), slave_class)

  for(i = 0; i < SS_N_IN_ENDPOINTS; i++)
  {
    status = _ux_utility_semaphore_create(&sensor_streaming->ux_slave_class_sensor_streaming_bulkin[i].ep_param.semaphore, "semaphore 0", 0);
    if(status != UX_SUCCESS)
    {
      return status;
    }

    /* Allocate some memory for the bulk in thread stack. */
    sensor_streaming->ux_slave_class_sensor_streaming_bulkin[i].thread_stack =
        _ux_utility_memory_allocate(UX_NO_ALIGN, UX_REGULAR_MEMORY, SS_CLASS_THREAD_STACK_SIZE);

    /* Create one thread for each IN endpoint */
    status = _ux_utility_thread_create(&sensor_streaming->ux_slave_class_sensor_streaming_bulkin[i].thread, "ux_slave_bulkin_thread",
                                       _ux_device_class_sensor_streaming_bulkin_entry,
                                       (ULONG) (ALIGN_TYPE) &sensor_streaming->ux_slave_class_sensor_streaming_bulkin[i].ep_param,
                                       (VOID*) sensor_streaming->ux_slave_class_sensor_streaming_bulkin[i].thread_stack,
                                       SS_BULKIN_THREAD_STACK_SIZE,
                                       SS_BULKIN_THREAD_PRIO,
                                       SS_BULKIN_THREAD_PRIO,
                                       UX_NO_TIME_SLICE, UX_DONT_START);
    if(status != (UINT) UX_SUCCESS)
    {
      return status;
    }

#if defined(ENABLE_THREADX_DBG_PIN) && defined(USB_EP_BULKIN_TASK_CFG_TAG)
    sensor_streaming->ux_slave_class_sensor_streaming_bulkin[i].thread.pxTaskTag = USB_EP_BULKIN_TASK_CFG_TAG;
#endif
  }

  /* Reset ep_map */
  for(i = 0; i < SS_N_CHANNELS_MAX; i++)
  {
    sensor_streaming->hwcid->ep_map[i] = SS_ENDPOINT_NOT_ASSIGNED;
  }

  /* Success, complete remaining settings.  */
  (void) _ux_utility_thread_resume(&slave_class->ux_slave_class_thread);

  /* Get the pointer to the application parameters for the cdc class.  */
  sensor_streaming->ux_slave_class_sensor_streaming_parameter = (UX_SLAVE_CLASS_SENSOR_STREAMING_PARAMETER*) command->ux_slave_class_command_parameter;

  /* Return completion status.  */
  return (UINT) UX_SUCCESS;
}

