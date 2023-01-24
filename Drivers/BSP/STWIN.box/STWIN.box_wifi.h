/**
 ******************************************************************************
 * @file    STWIN.box_wifi.h
 * @author  SRA
 * @brief   This file contains definitions for STWIN.box_wifi.c
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

#ifndef __STWIN_BOX_WIFI_H_
#define __STWIN_BOX_WIFI_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* WiFi IRQ Priorities definition */
#ifndef MXCHIP_NOTIFY_IT_PRIORITY
#define MXCHIP_NOTIFY_IT_PRIORITY     5U
#endif

#ifndef MXCHIP_FLOW_IT_PRIORITY
#define MXCHIP_FLOW_IT_PRIORITY       4U
#endif

/* WiFi Pin definitions */

/* Using LPTIM1 for FLOW IRQ instead of EXTI15 */
#define MXCHIP_FLOW_Pin           GPIO_PIN_15
#define MXCHIP_FLOW_GPIO_Port     GPIOG

#define MXCHIP_NSS_Pin            GPIO_PIN_7
#define MXCHIP_NSS_GPIO_Port      GPIOH

#define MXCHIP_NOTIFY_Pin         GPIO_PIN_7
#define MXCHIP_NOTIFY_GPIO_Port   GPIOE
#define MXCHIP_NOTIFY_EXTI_IRQn   EXTI7_IRQn

#define MXCHIP_RESET_Pin          GPIO_PIN_12
#define MXCHIP_RESET_GPIO_Port    GPIOE
/* Exported functions ------------------------------------------------------- */ 

int32_t BSP_WIFI_MX_GPIO_Init(void);


#ifdef __cplusplus
}
#endif

#endif /* __STWIN_BOX_WIFI_H_ */
