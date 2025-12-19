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
#define IIS2DULPX_INT1_Pin GPIO_PIN_3
#define IIS2DULPX_INT1_GPIO_Port GPIOC
#define IIS2DULPX_INT1_EXTI_IRQn EXTI3_IRQn
#define SPI_MISO_Pin GPIO_PIN_6
#define SPI_MISO_GPIO_Port GPIOA
#define SPI_MOSI_Pin GPIO_PIN_7
#define SPI_MOSI_GPIO_Port GPIOA
#define IIS2MDC_DRDY_Pin GPIO_PIN_0
#define IIS2MDC_DRDY_GPIO_Port GPIOB
#define IIS2MDC_DRDY_EXTI_IRQn EXTI0_IRQn
#define ISM330IS_INT2_Pin GPIO_PIN_13
#define ISM330IS_INT2_GPIO_Port GPIOF
#define ISM330IS_INT2_EXTI_IRQn EXTI13_IRQn
#define ISM6HG256X_INT2_Pin GPIO_PIN_14
#define ISM6HG256X_INT2_GPIO_Port GPIOF
#define ISM6HG256X_INT2_EXTI_IRQn EXTI14_IRQn
#define USER_INT_Pin GPIO_PIN_15
#define USER_INT_GPIO_Port GPIOF
#define USER_INT_EXTI_IRQn EXTI15_IRQn
#define ISM6HG256X_INT1_Pin GPIO_PIN_11
#define ISM6HG256X_INT1_GPIO_Port GPIOE
#define ISM6HG256X_INT1_EXTI_IRQn EXTI11_IRQn
#define SPI_CK_Pin GPIO_PIN_13
#define SPI_CK_GPIO_Port GPIOE
#define SPI_CS_Pin GPIO_PIN_14
#define SPI_CS_GPIO_Port GPIOD
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
