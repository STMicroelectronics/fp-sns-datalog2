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
#define V4_GOOD_Pin GPIO_PIN_6
#define V4_GOOD_GPIO_Port GPIOI
#define WIFI_FLOW_Pin GPIO_PIN_15
#define WIFI_FLOW_GPIO_Port GPIOG
#define SPI_SEN_CS_G_Pin GPIO_PIN_5
#define SPI_SEN_CS_G_GPIO_Port GPIOI
#define MCU_SEL_Pin GPIO_PIN_0
#define MCU_SEL_GPIO_Port GPIOI
#define LED3_Pin GPIO_PIN_12
#define LED3_GPIO_Port GPIOH
#define MDF1_SDI3_Pin GPIO_PIN_4
#define MDF1_SDI3_GPIO_Port GPIOE
#define SPI_SEN_CS_A_Pin GPIO_PIN_7
#define SPI_SEN_CS_A_GPIO_Port GPIOI
#define V_BT_E1_Pin GPIO_PIN_1
#define V_BT_E1_GPIO_Port GPIOE
#define BLE_RST_Pin GPIO_PIN_4
#define BLE_RST_GPIO_Port GPIOD
#define EN_SD_Pin GPIO_PIN_10
#define EN_SD_GPIO_Port GPIOH
#define GPIO_JP4_Pin GPIO_PIN_5
#define GPIO_JP4_GPIO_Port GPIOE
#define WC_INTB_Pin GPIO_PIN_3
#define WC_INTB_GPIO_Port GPIOE
#define WC_INTB_EXTI_IRQn EXTI3_IRQn
#define SW2_Pin GPIO_PIN_0
#define SW2_GPIO_Port GPIOE
#define SW2_EXTI_IRQn EXTI0_IRQn
#define MDF1_SDI0_Pin GPIO_PIN_3
#define MDF1_SDI0_GPIO_Port GPIOD
#define LED2_Pin GPIO_PIN_11
#define LED2_GPIO_Port GPIOH
#define SEL_SD_V_Pin GPIO_PIN_8
#define SEL_SD_V_GPIO_Port GPIOH
#define SW1_Pin GPIO_PIN_13
#define SW1_GPIO_Port GPIOC
#define SW1_EXTI_IRQn EXTI13_IRQn
#define MAG_DRDY_Pin GPIO_PIN_6
#define MAG_DRDY_GPIO_Port GPIOE
#define MAG_DRDY_EXTI_IRQn EXTI6_IRQn
#define SPI_DUT_CS2_Pin GPIO_PIN_12
#define SPI_DUT_CS2_GPIO_Port GPIOG
#define SW_ON_Pin GPIO_PIN_8
#define SW_ON_GPIO_Port GPIOF
#define ACC_INT1_Pin GPIO_PIN_2
#define ACC_INT1_GPIO_Port GPIOF
#define ACC_INT1_EXTI_IRQn EXTI2_IRQn
#define BT_BOOT_Pin GPIO_PIN_1
#define BT_BOOT_GPIO_Port GPIOD
#define WC_RSTB_Pin GPIO_PIN_7
#define WC_RSTB_GPIO_Port GPIOF
#define USB_ENUM_LED_Pin GPIO_PIN_9
#define USB_ENUM_LED_GPIO_Port GPIOF
#define MDF1_CCK_Pin GPIO_PIN_10
#define MDF1_CCK_GPIO_Port GPIOF
#define LED1_Pin GPIO_PIN_6
#define LED1_GPIO_Port GPIOF
#define SPI_DUT_CS_Pin GPIO_PIN_1
#define SPI_DUT_CS_GPIO_Port GPIOG
#define DIL_INT2_Pin GPIO_PIN_6
#define DIL_INT2_GPIO_Port GPIOG
#define LOCK_1v8_Pin GPIO_PIN_0
#define LOCK_1v8_GPIO_Port GPIOC
#define BUTTON_PWR_Pin GPIO_PIN_1
#define BUTTON_PWR_GPIO_Port GPIOC
#define BUTTON_PWR_EXTI_IRQn EXTI1_IRQn
#define VBATT_Pin GPIO_PIN_0
#define VBATT_GPIO_Port GPIOB
#define PRESS_INT_Pin GPIO_PIN_8
#define PRESS_INT_GPIO_Port GPIOE
#define PRESS_INT_EXTI_IRQn EXTI8_IRQn
#define DIL_INT1_Pin GPIO_PIN_2
#define DIL_INT1_GPIO_Port GPIOG
#define MDF1_SDI2_Pin GPIO_PIN_7
#define MDF1_SDI2_GPIO_Port GPIOE
#define PWM_OUT_T1_C3_Pin GPIO_PIN_13
#define PWM_OUT_T1_C3_GPIO_Port GPIOE
#define BLE_INT_Pin GPIO_PIN_11
#define BLE_INT_GPIO_Port GPIOB
#define MDF1_SDI1_Pin GPIO_PIN_12
#define MDF1_SDI1_GPIO_Port GPIOB
#define TEMP_INT_Pin GPIO_PIN_15
#define TEMP_INT_GPIO_Port GPIOB
#define BLE_SPI_CS_Pin GPIO_PIN_2
#define BLE_SPI_CS_GPIO_Port GPIOA
#define SD_DETECT_Pin GPIO_PIN_5
#define SD_DETECT_GPIO_Port GPIOC
#define SD_DETECT_EXTI_IRQn EXTI5_IRQn
#define LDO_EN_Pin GPIO_PIN_15
#define LDO_EN_GPIO_Port GPIOE
#define IMU_INT1_Pin GPIO_PIN_4
#define IMU_INT1_GPIO_Port GPIOA
#define IMU_INT1_EXTI_IRQn EXTI4_IRQn
#define ACC_INT2_Pin GPIO_PIN_15
#define ACC_INT2_GPIO_Port GPIOF
#define ACC_INT2_EXTI_IRQn EXTI15_IRQn
#define NFC_GPO_Pin GPIO_PIN_12
#define NFC_GPO_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */
#define INT_EX_EXTI_IRQn EXTI6_IRQn
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
