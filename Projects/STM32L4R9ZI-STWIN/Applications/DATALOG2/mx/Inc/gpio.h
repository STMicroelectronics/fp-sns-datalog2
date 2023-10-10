/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "mx.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
void MX_GPIO_PA2_Init(void);

void MX_GPIO_PD0_Init(void);
void MX_GPIO_PB0_Init(void);
void MX_GPIO_PD15_Init(void);
void MX_GPIO_PE0_Init(void);
void MX_GPIO_PE1_Init(void);
void MX_GPIO_PE8_Init(void);
void MX_GPIO_PE14_Init(void);
void MX_GPIO_PF4_Init(void);
void MX_GPIO_PF5_Init(void);
void MX_GPIO_PF6_Init(void);
void MX_GPIO_PF8_Init(void);
void MX_GPIO_PF9_Init(void);
void MX_GPIO_PF13_Init(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

