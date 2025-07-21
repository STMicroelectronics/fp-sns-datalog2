/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

#include "stm32u5xx_ll_ucpd.h"
#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_ll_cortex.h"
#include "stm32u5xx_ll_rcc.h"
#include "stm32u5xx_ll_system.h"
#include "stm32u5xx_ll_utils.h"
#include "stm32u5xx_ll_pwr.h"
#include "stm32u5xx_ll_gpio.h"
#include "stm32u5xx_ll_dma.h"

#include "stm32u5xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LIS2MDL_DRDY_Pin GPIO_PIN_3
#define LIS2MDL_DRDY_GPIO_Port GPIOC
#define LIS2MDL_DRDY_EXTI_IRQn EXTI3_IRQn
#define LIS2DUXS12_INT_Pin GPIO_PIN_0
#define LIS2DUXS12_INT_GPIO_Port GPIOB
#define LIS2DUXS12_INT_EXTI_IRQn EXTI0_IRQn
#define LSM6DSO16IS_INT2_Pin GPIO_PIN_13
#define LSM6DSO16IS_INT2_GPIO_Port GPIOF
#define LSM6DSO16IS_INT2_EXTI_IRQn EXTI13_IRQn
#define LSM6DSV16X_INT2_Pin GPIO_PIN_14
#define LSM6DSV16X_INT2_GPIO_Port GPIOF
#define LSM6DSV16X_INT2_EXTI_IRQn EXTI14_IRQn
#define USER_INT_Pin GPIO_PIN_15
#define USER_INT_GPIO_Port GPIOF
#define USER_INT_EXTI_IRQn EXTI15_IRQn
#define LSM6DSV16X_INT1_Pin GPIO_PIN_11
#define LSM6DSV16X_INT1_GPIO_Port GPIOE
#define LSM6DSV16X_INT1_EXTI_IRQn EXTI11_IRQn
#define LED_RED_Pin GPIO_PIN_2
#define LED_RED_GPIO_Port GPIOG
#define LED_GREEN_Pin GPIO_PIN_7
#define LED_GREEN_GPIO_Port GPIOC
#define USART1_TX_Pin GPIO_PIN_9
#define USART1_TX_GPIO_Port GPIOA
#define USART1_RX_Pin GPIO_PIN_10
#define USART1_RX_GPIO_Port GPIOA
#define LED_BLUE_Pin GPIO_PIN_7
#define LED_BLUE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
