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
void LIS2DUXS12Task_EXTI_Callback(uint16_t pin);
void STTS22HTask_EXTI_Callback(uint16_t nPin);
void LIS2MDLTask_EXTI_Callback(uint16_t nPin);
void LPS22DFTask_EXTI_Callback(uint16_t nPin);
void LSM6DSV16XTask_EXTI_Callback(uint16_t nPin);
/* External variables */
/**********************/

/* Map definition */
/******************/

EXTI_BEGIN_P2F_MAP()

EXTI_P2F_MAP_ENTRY(GPIO_PIN_1, LIS2DUXS12Task_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_2, STTS22HTask_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_3, LIS2MDLTask_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_8, LPS22DFTask_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_11, LSM6DSV16XTask_EXTI_Callback)

EXTI_END_P2F_MAP()
