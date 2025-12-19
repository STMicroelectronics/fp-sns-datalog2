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
//void Util_USR_EXTI_Callback(uint16_t pin);
void IIS2DULPXTask_EXTI_Callback(uint16_t Pin);
void IIS2MDCTask_EXTI_Callback(uint16_t nPin);
void INT2_ISM330IS_EXTI_Callback(uint16_t Pin);
void ISM6HG256XTask_EXTI_Callback(uint16_t Pin);
void INT2_HG256X_EXTI_Callback(uint16_t Pin);
/* External variables */
/**********************/

/* Map definition */
/******************/

EXTI_BEGIN_P2F_MAP()

EXTI_P2F_MAP_ENTRY(GPIO_PIN_3, IIS2DULPXTask_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_0, IIS2MDCTask_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_11, ISM6HG256XTask_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_13, INT2_ISM330IS_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_14, INT2_HG256X_EXTI_Callback)
//EXTI_P2F_MAP_ENTRY(GPIO_PIN_15, Util_USR_EXTI_Callback)

EXTI_END_P2F_MAP()
