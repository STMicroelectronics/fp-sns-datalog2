/**
  ******************************************************************************
  * @file    usbx_callbacks.h
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
#ifndef USBX_CALLBACKS_H_
#define USBX_CALLBACKS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ux_api.h"

/**
  * @brief  USBD_ChangeFunction
  *         This function is called when the device state changes.
  * @param  Device_State: USB Device State
  * @retval status
  */
UINT USBD_ChangeFunction(ULONG Device_State);


#ifdef __cplusplus
}
#endif

#endif /* USBX_CALLBACKS_H_ */
