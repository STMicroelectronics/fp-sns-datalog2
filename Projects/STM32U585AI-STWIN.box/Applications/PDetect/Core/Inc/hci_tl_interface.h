/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    hci_tl_interface.h
  * @author  SRA Application Team
  * @brief   Header file for hci_tl_interface.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#ifndef __HCI_TL_INTERFACE_H
#define __HCI_TL_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @addtogroup LOW_LEVEL_INTERFACE LOW_LEVEL_INTERFACE
  * @{
  */

/**
  * @defgroup LL_HCI_TL_INTERFACE HCI_TL_INTERFACE
  * @{
  */

/**
  * @defgroup LL_HCI_TL_INTERFACE_TEMPLATE TEMPLATE
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Exported Defines ----------------------------------------------------------*/

#define HCI_TL_SPI_EXTI_IRQn  EXTI14_IRQn

#define HCI_TL_RST_PORT       GPIOD
#define HCI_TL_RST_PIN        GPIO_PIN_13
/* Exported variables --------------------------------------------------------*/


/* Exported Functions --------------------------------------------------------*/
int32_t is_data_available(void);

/**
  * @brief  Register hci_tl_interface IO bus services and the IRQ handlers.
  * @param  None
  * @retval None
  */
void hci_tl_lowlevel_init(void);

/**
  * @brief HCI Transport Layer Low Level Interrupt Service Routine
  * @param  None
  * @retval None
  */
void hci_tl_lowlevel_isr(void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif
#endif /* __HCI_TL_INTERFACE_H */

