/**
  ******************************************************************************
  * @file    exti_pin_map.c
  * @author  SRA
  * @brief   Application level file. It defines the PIN to callback
  *          mapping function for the external interrupt.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */

#include "drivers/EXTIPinMap.h"

/* Forward function declaration. */
/*********************************/
void Def_EXTI_Callback(uint16_t nPin) {};
void Util_USR_EXTI_Callback(uint16_t nPin);
void IIS2MDCTask_EXTI_Callback(uint16_t nPin);
void ISM330DHCXTask_EXTI_Callback(uint16_t nPin);
void INT2_DHCX_EXTI_Callback(uint16_t nPin);
void IIS3DWBTask_EXTI_Callback(uint16_t nPin);
void Util_PWR_EXTI_Callback(uint16_t nPin);
void hci_tl_lowlevel_isr(uint16_t nPin);

/* External variables */
/**********************/

/* Map definition */
/******************/

EXTI_BEGIN_P2F_MAP()
EXTI_P2F_MAP_ENTRY(GPIO_PIN_0, Util_USR_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_1, hci_tl_lowlevel_isr)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_4, INT2_DHCX_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_8, ISM330DHCXTask_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_9, IIS2MDCTask_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_10, Util_PWR_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_14, IIS3DWBTask_EXTI_Callback)
EXTI_END_P2F_MAP()
