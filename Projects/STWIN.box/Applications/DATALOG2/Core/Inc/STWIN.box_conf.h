/**
  ******************************************************************************
  * @file    STWIN.box_conf.h
  * @author  SRA
  * @brief   This file contains definitions for the components bus interfaces
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STWIN_BOX_CONF_H__
#define STWIN_BOX_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"
#include "STWIN.box_errno.h"


/* SD card interrupt priority */
#define BSP_SD_IT_PRIORITY            14U  /* Default is lowest priority level */
#define BSP_SD_RX_IT_PRIORITY         14U  /* Default is lowest priority level */
#define BSP_SD_TX_IT_PRIORITY         15U  /* Default is lowest priority level */

#define BUTTON_USER_IT_PRIORITY       14U
#define BUTTON_PWR_IT_PRIORITY        14U


#define USE_MOTION_SENSOR_IIS2DLP_0     0U
#define USE_MOTION_SENSOR_IIS2MDC_0     0U
#define USE_MOTION_SENSOR_IIS3DWB_0     0U
#define USE_MOTION_SENSOR_ISM330DHCX_0  0U
#define USE_MOTION_SENSOR_IIS2ICLX_0    0U

#define USE_ENV_SENSOR_ILPS22QS_0       0U
#define USE_ENV_SENSOR_STTS22H_0        0U


/* IIS2DLP */
#define BSP_IIS2DLPC_CS_GPIO_CLK_ENABLE()       __GPIOH_CLK_ENABLE()
#define BSP_IIS2DLPC_CS_PORT                    GPIOH
#define BSP_IIS2DLPC_CS_PIN                     GPIO_PIN_6

#define BSP_IIS2DLPC_INT1_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_IIS2DLPC_INT1_PORT                  GPIOF
#define BSP_IIS2DLPC_INT1_PIN                   GPIO_PIN_1
#define BSP_IIS2DLPC_INT1_EXTI_IRQn             EXTI1_IRQn
#define BSP_IIS2DLPC_INT1_EXTI_IRQ_PP           1
#define BSP_IIS2DLPC_INT1_EXTI_IRQ_SP           0
#define BSP_IIS2DLPC_INT1_EXTI_IRQHandler       EXTI1_IRQHandler

#define BSP_IIS2DLPC_INT2_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_IIS2DLPC_INT2_PORT                  GPIOF
#define BSP_IIS2DLPC_INT2_PIN                   GPIO_PIN_2
#define BSP_IIS2DLPC_INT2_EXTI_IRQn             EXTI2_IRQn
#define BSP_IIS2DLPC_INT2_EXTI_IRQ_PP           1
#define BSP_IIS2DLPC_INT2_EXTI_IRQ_SP           0
#define BSP_IIS2DLPC_INT2_EXTI_IRQHandler       EXTI2_IRQHandler


/* IIS3DWB */
#define BSP_IIS3DWB_CS_GPIO_CLK_ENABLE()        __GPIOF_CLK_ENABLE()
#define BSP_IIS3DWB_CS_PORT                     GPIOF
#define BSP_IIS3DWB_CS_PIN                      GPIO_PIN_12

#define BSP_IIS3DWB_INT1_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_IIS3DWB_INT1_PORT                   GPIOF
#define BSP_IIS3DWB_INT1_PIN                    GPIO_PIN_15
#define BSP_IIS3DWB_INT1_EXTI_IRQn              EXTI15_IRQn
#define BSP_IIS3DWB_INT1_EXTI_IRQ_PP            1
#define BSP_IIS3DWB_INT1_EXTI_IRQ_SP            0
#define BSP_IIS3DWB_INT1_EXTI_IRQHandler        EXTI15_IRQHandler


/* ISM330DHCX */
#define BSP_ISM330DHCX_CS_GPIO_CLK_ENABLE()   __GPIOH_CLK_ENABLE()
#define BSP_ISM330DHCX_CS_PORT                  GPIOH
#define BSP_ISM330DHCX_CS_PIN                   GPIO_PIN_15

#define BSP_ISM330DHCX_INT1_GPIO_CLK_ENABLE()   __GPIOB_CLK_ENABLE()
#define BSP_ISM330DHCX_INT1_PORT                GPIOB
#define BSP_ISM330DHCX_INT1_PIN                 GPIO_PIN_8
#define BSP_ISM330DHCX_INT1_EXTI_IRQn           EXTI8_IRQn
#define BSP_ISM330DHCX_INT1_EXTI_IRQ_PP         1
#define BSP_ISM330DHCX_INT1_EXTI_IRQ_SP         0
#define BSP_ISM330DHCX_INT1_EXTI_IRQHandler     EXTI18_IRQHandler

#define BSP_ISM330DHCX_INT2_GPIO_CLK_ENABLE()   __GPIOF_CLK_ENABLE()
#define BSP_ISM330DHCX_INT2_PORT                GPIOF
#define BSP_ISM330DHCX_INT2_PIN                 GPIO_PIN_4
#define BSP_ISM330DHCX_INT2_EXTI_IRQn           EXTI4_IRQn
#define BSP_ISM330DHCX_INT2_EXTI_IRQ_PP         1
#define BSP_ISM330DHCX_INT2_EXTI_IRQ_SP         0
#define BSP_ISM330DHCX_INT2_EXTI_IRQHandler     EXTI14_IRQHandler


/* IIS2ICLX */
#define BSP_IIS2ICLX_CS_GPIO_CLK_ENABLE()       __GPIOI_CLK_ENABLE()
#define BSP_IIS2ICLX_CS_PORT                    GPIOI
#define BSP_IIS2ICLX_CS_PIN                     GPIO_PIN_7

#define BSP_IIS2ICLX_INT1_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_IIS2ICLX_INT1_PORT                  GPIOF
#define BSP_IIS2ICLX_INT1_PIN                   GPIO_PIN_3
#define BSP_IIS2ICLX_INT1_EXTI_IRQn             EXTI3_IRQn
#define BSP_IIS2ICLX_INT1_EXTI_IRQ_PP           1
#define BSP_IIS2ICLX_INT1_EXTI_IRQ_SP           0
#define BSP_IIS2ICLX_INT1_EXTI_IRQHandler       EXTI3_IRQHandler

#define BSP_IIS2ICLX_INT2_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_IIS2ICLX_INT2_PORT                  GPIOF
#define BSP_IIS2ICLX_INT2_PIN                   GPIO_PIN_11
#define BSP_IIS2ICLX_INT2_EXTI_IRQn             EXTI11_IRQn
#define BSP_IIS2ICLX_INT2_EXTI_IRQ_PP           1
#define BSP_IIS2ICLX_INT2_EXTI_IRQ_SP           0
#define BSP_IIS2ICLX_INT2_EXTI_IRQHandler       EXTI11_IRQHandler

/* ILPS22QS */
#define BSP_ILPS22QS_INT_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOG_CLK_ENABLE()
#define BSP_ILPS22QS_INT_PORT                   GPIOG
#define BSP_ILPS22QS_INT_PIN                    GPIO_PIN_12
#define BSP_ILPS22QS_INT_EXTI_IRQn              EXTI12_IRQn
#define BSP_ILPS22QS_INT_EXTI_IRQ_PP            1
#define BSP_ILPS22QS_INT_EXTI_IRQ_SP            0
#define BSP_ILPS22QS_INT_EXTI_IRQHandler        EXTI12_IRQHandler

/* STTS22H */
#define BSP_STTS22H_INT_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_STTS22H_INT_PORT                    GPIOF
#define BSP_STTS22H_INT_PIN                     GPIO_PIN_5
#define BSP_STTS22H_INT_EXTI_IRQn               EXTI5_IRQn
#define BSP_STTS22H_INT_EXTI_IRQ_PP             1
#define BSP_STTS22H_INT_EXTI_IRQ_SP             0
#define BSP_STTS22H_INT_EXTI_IRQHandler         EXTI5_IRQHandler


#ifdef __cplusplus
}
#endif

#endif /* STWIN_BOX_CONF_H__*/



