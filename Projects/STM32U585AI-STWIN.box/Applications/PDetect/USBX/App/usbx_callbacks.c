/**
  ******************************************************************************
  * @file    usbx_callbacks.c
  * @author  SRA
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */

#include "usbx_callbacks.h"
#include "ux_device_class_sensor_streaming.h"
#include "ux_dcd_stm32.h"

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)


/* Public API definition */
/*************************/

UINT USBD_ChangeFunction(ULONG Device_State)
{
  UINT status = UX_SUCCESS;

  switch (Device_State)
  {
    case UX_DCD_STM32_SOF_RECEIVED:
      _ux_device_class_sensor_streaming_sof();
      break;
    case UX_DEVICE_ATTACHED:
      break;
    case UX_DEVICE_REMOVED:
      break;
    case UX_DCD_STM32_DEVICE_CONNECTED:
      break;
    case UX_DCD_STM32_DEVICE_DISCONNECTED:
      break;
    case UX_DCD_STM32_DEVICE_SUSPENDED:
      break;
    case UX_DCD_STM32_DEVICE_RESUMED:
      break;
    default:
      break;
  }
  return status;
}

