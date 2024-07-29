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
  * Copyright (c) 2022 STMicroelectronics.
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

// Forward function declaration.
void Def_EXTI_Callback(uint16_t nPin) {};
void EXT_INT1_EXTI_Callback(uint16_t nPin);
void LSM6DSV16XTask_EXTI_Callback(uint16_t nPin);
void EXT_INT2_EXTI_Callback(uint16_t nPin);
void hci_tl_lowlevel_isr(uint16_t nPin);
void Util_USR_EXTI_Callback(uint16_t nPin);
void LIS2DU12Task_EXTI_Callback(uint16_t nPin);

EXTI_BEGIN_P2F_MAP()
EXTI_P2F_MAP_ENTRY(GPIO_PIN_2, EXT_INT1_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_4, LSM6DSV16XTask_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_6, EXT_INT2_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_11, hci_tl_lowlevel_isr)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_13, Util_USR_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_15, LIS2DU12Task_EXTI_Callback)
EXTI_END_P2F_MAP()
