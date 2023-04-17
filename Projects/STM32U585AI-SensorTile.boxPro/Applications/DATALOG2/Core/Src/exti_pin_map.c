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
void Util_USR_EXTI_Callback(uint16_t nPin);
void LIS2DU12Task_EXTI_Callback(uint16_t nPin);
//void INT2_DHCX_EXTI_Callback(uint16_t nPin);
//void WIFI_NOTIFY_EXTI_Callback(uint16_t nPin);
void LSM6DSV16XTask_EXTI_Callback(uint16_t nPin);
void LIS2MDLTask_EXTI_Callback(uint16_t nPin);
//void Util_PWR_EXTI_Callback(uint16_t nPin);
//void NFC_INT_EXTI_Callback(uint16_t nPin);
void hci_tl_isr(uint16_t nPin);

EXTI_BEGIN_P2F_MAP()
EXTI_P2F_MAP_ENTRY(GPIO_PIN_13, Util_USR_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_2, LIS2DU12Task_EXTI_Callback)
//EXTI_P2F_MAP_ENTRY(GPIO_PIN_4, INT2_DHCX_EXTI_Callback)
//  EXTI_P2F_MAP_ENTRY(GPIO_PIN_7, WIFI_NOTIFY_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_4, LSM6DSV16XTask_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_6, LIS2MDLTask_EXTI_Callback)
//  EXTI_P2F_MAP_ENTRY(GPIO_PIN_10, Util_PWR_EXTI_Callback)
//  EXTI_P2F_MAP_ENTRY(GPIO_PIN_13, NFC_INT_EXTI_Callback)
EXTI_P2F_MAP_ENTRY(GPIO_PIN_11, hci_tl_isr)
EXTI_END_P2F_MAP()
