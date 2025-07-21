/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32h7xx_hal.h"

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
#define H7_OSC_IN_Pin GPIO_PIN_0
#define H7_OSC_IN_GPIO_Port GPIOH
#define H7_OSC_OUT_Pin GPIO_PIN_1
#define H7_OSC_OUT_GPIO_Port GPIOH
#define H7_CH2_Pin GPIO_PIN_2
#define H7_CH2_GPIO_Port GPIOC
#define H7_2_TX_Pin GPIO_PIN_2
#define H7_2_TX_GPIO_Port GPIOA
#define H7_2_RX_Pin GPIO_PIN_3
#define H7_2_RX_GPIO_Port GPIOA
#define H7_OPOUT_Pin GPIO_PIN_4
#define H7_OPOUT_GPIO_Port GPIOC
#define H7_OPVN_Pin GPIO_PIN_5
#define H7_OPVN_GPIO_Port GPIOC
#define H7_CH1_Pin GPIO_PIN_0
#define H7_CH1_GPIO_Port GPIOB
#define LED_4_Pin GPIO_PIN_2
#define LED_4_GPIO_Port GPIOB
#define LED_5_Pin GPIO_PIN_10
#define LED_5_GPIO_Port GPIOB
#define H7_USB_DM_Pin GPIO_PIN_11
#define H7_USB_DM_GPIO_Port GPIOA
#define H7_USB_DP_Pin GPIO_PIN_12
#define H7_USB_DP_GPIO_Port GPIOA
#define BUTTON1_Pin GPIO_PIN_5
#define BUTTON1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
