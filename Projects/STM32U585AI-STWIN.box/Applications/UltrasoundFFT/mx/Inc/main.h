/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics..
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
#define CS_DHCX_Pin GPIO_PIN_15
#define CS_DHCX_GPIO_Port GPIOH
#define DCDC_2_EN_Pin GPIO_PIN_5
#define DCDC_2_EN_GPIO_Port GPIOI
#define C_EN_Pin GPIO_PIN_0
#define C_EN_GPIO_Port GPIOD
#define EX_SPI_NSS_Pin GPIO_PIN_0
#define EX_SPI_NSS_GPIO_Port GPIOI
#define LED1_Pin GPIO_PIN_12
#define LED1_GPIO_Port GPIOH
#define CS_ICLX_Pin GPIO_PIN_7
#define CS_ICLX_GPIO_Port GPIOI
#define BLE_SPI_CS_Pin GPIO_PIN_1
#define BLE_SPI_CS_GPIO_Port GPIOE
#define BOOT0_Pin GPIO_PIN_3
#define BOOT0_GPIO_Port GPIOH
#define LED2_Pin GPIO_PIN_10
#define LED2_GPIO_Port GPIOH
#define USR_BUTTON_Pin GPIO_PIN_0
#define USR_BUTTON_GPIO_Port GPIOE
#define USR_BUTTON_EXTI_IRQn EXTI0_IRQn
#define SPI2_MISO_p2_Pin GPIO_PIN_2
#define SPI2_MISO_p2_GPIO_Port GPIOI
#define INT1_DHCX_Pin GPIO_PIN_8
#define INT1_DHCX_GPIO_Port GPIOB
#define INT1_DHCX_EXTI_IRQn EXTI8_IRQn
#define STSAFE_RESET_Pin GPIO_PIN_9
#define STSAFE_RESET_GPIO_Port GPIOH
#define CS_DLPC_Pin GPIO_PIN_6
#define CS_DLPC_GPIO_Port GPIOH
#define GPIO2_EX_Pin GPIO_PIN_8
#define GPIO2_EX_GPIO_Port GPIOF
#define INT1_DLPC_Pin GPIO_PIN_1
#define INT1_DLPC_GPIO_Port GPIOF
#define INT1_DLPC_EXTI_IRQn EXTI1_IRQn
#define INT2_DLPC_Pin GPIO_PIN_2
#define INT2_DLPC_GPIO_Port GPIOF
#define INT2_DLPC_EXTI_IRQn EXTI2_IRQn
#define WIFI_CS_Pin GPIO_PIN_7
#define WIFI_CS_GPIO_Port GPIOH
#define BLE_TEST9_Pin GPIO_PIN_7
#define BLE_TEST9_GPIO_Port GPIOF
#define INT_MAG_Pin GPIO_PIN_9
#define INT_MAG_GPIO_Port GPIOF
#define INT_MAG_EXTI_IRQn EXTI9_IRQn
#define INT_STT_Pin GPIO_PIN_5
#define INT_STT_GPIO_Port GPIOF
#define INT_STT_EXTI_IRQn EXTI5_IRQn
#define INT1_ICLX_Pin GPIO_PIN_3
#define INT1_ICLX_GPIO_Port GPIOF
#define INT1_ICLX_EXTI_IRQn EXTI3_IRQn
#define INT2_DHCX_Pin GPIO_PIN_4
#define INT2_DHCX_GPIO_Port GPIOF
#define INT2_DHCX_EXTI_IRQn EXTI4_IRQn
#define SW_SEL_Pin GPIO_PIN_8
#define SW_SEL_GPIO_Port GPIOA
#define GPIO3_EX_Pin GPIO_PIN_10
#define GPIO3_EX_GPIO_Port GPIOF
#define BLE_TEST8_Pin GPIO_PIN_6
#define BLE_TEST8_GPIO_Port GPIOF
#define SD_DETECT_Pin GPIO_PIN_1
#define SD_DETECT_GPIO_Port GPIOG
#define INT_EX_Pin GPIO_PIN_6
#define INT_EX_GPIO_Port GPIOG
#define INT_EX_EXTI_IRQn EXTI6_IRQn
#define TIM3_PWM_Pin GPIO_PIN_6
#define TIM3_PWM_GPIO_Port GPIOC
#define uC_ADC_BATT_Pin GPIO_PIN_2
#define uC_ADC_BATT_GPIO_Port GPIOC
#define INT_EXG5_Pin GPIO_PIN_5
#define INT_EXG5_GPIO_Port GPIOG
#define INT_EXD15_Pin GPIO_PIN_15
#define INT_EXD15_GPIO_Port GPIOD
#define ADC_EX_Pin GPIO_PIN_3
#define ADC_EX_GPIO_Port GPIOC
#define CHRG_Pin GPIO_PIN_0
#define CHRG_GPIO_Port GPIOA
#define CS_DWB_Pin GPIO_PIN_12
#define CS_DWB_GPIO_Port GPIOF
#define BUTTON_PWR_Pin GPIO_PIN_10
#define BUTTON_PWR_GPIO_Port GPIOD
#define BUTTON_PWR_EXTI_IRQn EXTI10_IRQn
#define BLE_RST_Pin GPIO_PIN_13
#define BLE_RST_GPIO_Port GPIOD
#define BLE_INT_Pin GPIO_PIN_14
#define BLE_INT_GPIO_Port GPIOF
#define BLE_INT_EXTI_IRQn EXTI14_IRQn
#define WIFI_NOTIFY_Pin GPIO_PIN_7
#define WIFI_NOTIFY_GPIO_Port GPIOE
#define WIFI_NOTIFY_EXTI_IRQn EXTI7_IRQn
#define SPI2_MOSI_p2_Pin GPIO_PIN_15
#define SPI2_MOSI_p2_GPIO_Port GPIOB
#define WIFI_BOOT_Pin GPIO_PIN_9
#define WIFI_BOOT_GPIO_Port GPIOD
#define INT2_ICLX_Pin GPIO_PIN_11
#define INT2_ICLX_GPIO_Port GPIOF
#define INT2_ICLX_EXTI_IRQn EXTI11_IRQn
#define LDO_EN_Pin GPIO_PIN_15
#define LDO_EN_GPIO_Port GPIOE
#define GPIO1_EX_Pin GPIO_PIN_1
#define GPIO1_EX_GPIO_Port GPIOB
#define INT1_DWB_Pin GPIO_PIN_15
#define INT1_DWB_GPIO_Port GPIOF
#define INT1_DWB_EXTI_IRQn EXTI15_IRQn
#define WIFI_EN_Pin GPIO_PIN_12
#define WIFI_EN_GPIO_Port GPIOE
#define NFC_INT_Pin GPIO_PIN_13
#define NFC_INT_GPIO_Port GPIOB
#define NFC_INT_EXTI_IRQn EXTI13_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
