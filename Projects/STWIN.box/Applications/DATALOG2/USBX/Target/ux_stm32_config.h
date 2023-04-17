/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_stm32_config.h
  * @author  MCD Application Team
  * @brief   USBX STM32 config header file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#ifndef __UX_STM32_CONFIG_H__
#define __UX_STM32_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private defines -----------------------------------------------------------*/

/* Total number of Endpoints = IN + OUT + CTRL */
#define UX_DCD_STM32_MAX_ED                   (SS_N_IN_ENDPOINTS + SS_N_OUT_ENDPOINTS + 1U)

/* Total USB FIFO SIZE, on U5 is 320 */
#define PCD_FIFO_SIZE                         320U

/* FIFO SIZE for OUT EP (RX)
 * Even if we are not using the OUT EP in our class, it seems to be used during setup time
 * and there should be a minimum of 40 bytes --> Found empirically
 */
#define PCD_FIFO_RX_EP                        40U

/* FIFO SIZE for CTRL EP */
#define PCD_FIFO_CTRL_EP                      64U

/* Product Identification String*/
#define USBD_PRODUCT_STRING                   "STWIN.box_Multi_Sensor_Streaming"


#ifdef __cplusplus
}
#endif
#endif  /* __UX_STM32_CONFIG_H__ */
