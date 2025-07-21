/**
  ******************************************************************************
  * @file    hci.h
  * @author  AMS - VMA RF Application team
  * @brief   Header file for BlueNRG's HCI APIs
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef HCI_H
#define HCI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bluenrg1_types.h"

/**
  * @addtogroup HIGH_LEVEL_INTERFACE HIGH_LEVEL_INTERFACE
  * @{
  */

/**
  * @defgroup HCI_TL HCI_TL
  * @{
  */

/**
  * @defgroup HCI_TL_functions HCI_TL functions
  * @{
  */

/**
  * @brief Initialize the Host Controller Interface.
  *        This function must be called before any data can be received
  *        from BLE controller.
  *
  * @param  user_evt_rx: ACI events callback function pointer
  *                This callback is triggered when an user event is received from
  *                the BLE core device.
  * @param  p_conf: Configuration structure pointer
  * @retval None
  */
void hci_init(void(* user_evt_rx)(void *p_data), void *p_conf);

/**
  * @brief  Processing function that must be called after an event is received from
  *             HCI interface.
  *             It must be called outside ISR. It will call user_notify() if necessary.
  *
  * @param  None
  * @retval None
  */
void hci_user_evt_proc(void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* HCI_H */
