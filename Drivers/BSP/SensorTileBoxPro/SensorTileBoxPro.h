/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SensorTileBoxPro.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.1.0
  * @date    20-July-2023
  * @brief   header file for the BSP Common driver
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
#ifndef __SENSORTILEBOXPRO_H
#define __SENSORTILEBOXPRO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "SensorTileBoxPro_conf.h"
#include "SensorTileBoxPro_errno.h"

#if (USE_BSP_COM_FEATURE > 0)
  #if (USE_COM_LOG > 0)
    #if defined(__ICCARM__) || defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) /* For IAR and ARM Compiler 5 and 6*/
      #include <stdio.h>
    #endif
  #endif
#endif
/** @addtogroup BSP
 * @{
 */

/** @defgroup SENSORTILEBOXPRO
 * @{
 */

/** @defgroup SENSORTILEBOXPRO_LOW_LEVEL
 * @{
 */

/** @defgroup STM32L4XX_NUCLEO_LOW_LEVEL_Exported_Constants LOW LEVEL Exported Constants
  * @{
  */
/**
 * @brief STM32U5XX NUCLEO BSP Driver version number V1.0.0
 */
#define __SENSORTILEBOXPRO_BSP_VERSION_MAIN   (uint32_t)(0x01) /*!< [31:24] main version */
#define __SENSORTILEBOXPRO_BSP_VERSION_SUB1   (uint32_t)(0x01) /*!< [23:16] sub1 version */
#define __SENSORTILEBOXPRO_BSP_VERSION_SUB2   (uint32_t)(0x02) /*!< [15:8]  sub2 version */
#define __SENSORTILEBOXPRO_BSP_VERSION_RC     (uint32_t)(0x00) /*!< [7:0]  release candidate */
#define __SENSORTILEBOXPRO_BSP_VERSION        ((__SENSORTILEBOXPRO_BSP_VERSION_MAIN << 24)\
                                                    |(__SENSORTILEBOXPRO_BSP_VERSION_SUB1 << 16)\
                                                    |(__SENSORTILEBOXPRO_BSP_VERSION_SUB2 << 8 )\
                                                    |(__SENSORTILEBOXPRO_BSP_VERSION_RC))

/** @defgroup SENSORTILEBOXPRO_LOW_LEVEL_Exported_Types SENSORTILEBOXPRO LOW LEVEL Exported Types
 * @{
 */

 /**
  * @brief Define for SENSORTILEBOXPRO board
  */
#if !defined (USE_SENSORTILEBOXPRO)
 #define USE_SENSORTILEBOXPRO
#endif
#ifndef USE_BSP_COM_FEATURE
   #define USE_BSP_COM_FEATURE                  0U
#endif

/** @defgroup SENSORTILEBOXPRO_LOW_LEVEL_LED SENSORTILEBOXPRO LOW LEVEL LED
 * @{
 */
/** Define number of LED            **/
#define LEDn                              4U
/**  Definition for BSP USER LED 2   **/
#define LED1_PIN                     	  GPIO_PIN_6
#define LED1_GPIO_PORT                    GPIOF
#define LED1_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOF_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOF_CLK_DISABLE()

#define LED2_PIN                     	  GPIO_PIN_11
#define LED2_GPIO_PORT                    GPIOH
#define LED2_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOH_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOH_CLK_DISABLE()

#define LED3_PIN                     	  GPIO_PIN_12
#define LED3_GPIO_PORT                    GPIOH
#define LED3_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOH_CLK_ENABLE()
#define LED3_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOH_CLK_DISABLE())

#define LED4_PIN                     	  GPIO_PIN_9
#define LED4_GPIO_PORT                    GPIOF
#define LED4_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOF_CLK_ENABLE()
#define LED4_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOF_CLK_DISABLE())

/**
 * @}
 */

/** @defgroup SENSORTILEBOXPRO_LOW_LEVEL_BUTTON SENSORTILEBOXPRO LOW LEVEL BUTTON
 * @{
 */
/* Button state */
#define BUTTON_RELEASED                   0U
#define BUTTON_PRESSED                    1U
/** Define number of BUTTON            **/
#define BUTTONn                           1U

/**
 * @brief User push-button
 */
  /**  Definition for BSP USER BUTTON   **/

#define BUS_GPIO_INSTANCE GPIO
#define BUS_BSP_BUTTON_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define BUS_BSP_BUTTON_GPIO_PIN GPIO_PIN_13
#define BUS_BSP_BUTTON_GPIO_CLK_DISABLE() __HAL_RCC_GPIOC_CLK_DISABLE()
#define BUS_BSP_BUTTON_GPIO_PORT GPIOC

#define USER_BUTTON_PIN	                  GPIO_PIN_13
#define USER_BUTTON_GPIO_PORT              GPIOC
#define USER_BUTTON_EXTI_IRQn              EXTI13_IRQn
#define USER_BUTTON_EXTI_LINE              EXTI_LINE_13
#define H_EXTI_13			  hpb_exti[BUTTON_USER]
/**
 * @}
 */
/** @defgroup SENSORTILEBOXPRO_LOW_LEVEL_COM SENSORTILEBOXPRO LOW LEVEL COM
 * @{
 */
/**
 * @brief Definition for COM portx, connected to UART4
 */

#define BUS_UART4_INSTANCE UART4
#define BUS_UART4_TX_GPIO_CLK_DISABLE() __HAL_RCC_GPIOA_CLK_DISABLE()
#define BUS_UART4_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define BUS_UART4_TX_GPIO_PIN GPIO_PIN_0
#define BUS_UART4_TX_GPIO_AF GPIO_AF8_UART4
#define BUS_UART4_TX_GPIO_PORT GPIOA
#define BUS_UART4_RX_GPIO_PIN GPIO_PIN_1
#define BUS_UART4_RX_GPIO_PORT GPIOA
#define BUS_UART4_RX_GPIO_CLK_DISABLE() __HAL_RCC_GPIOA_CLK_DISABLE()
#define BUS_UART4_RX_GPIO_AF GPIO_AF8_UART4
#define BUS_UART4_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

/**
 * @}
 */

/** @defgroup SENSORTILEBOXPRO_LOW_LEVEL_Exported_Types LOW LEVEL Exported Types
  * @{
  */
#ifndef USE_BSP_COM
  #define USE_BSP_COM                           0U
#endif

#ifndef USE_COM_LOG
  #define USE_COM_LOG                           1U
#endif

#ifndef BSP_BUTTON_USER_IT_PRIORITY
  #define BSP_BUTTON_USER_IT_PRIORITY            15U
#endif

typedef enum
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3,
  LED_GREEN = LED1,
  LED_RED = LED2,
  LED_YELLOW = LED3,
  LED_BLUE = LED4
}Led_TypeDef;

/* FINISHA --> BOARD_ID = 0x0DU
// FINISHB --> BOARD_ID = 0x11U */
typedef enum
{
  FINISHA = 0,
  FINISHB = 1,
  FINISH_ERROR =2
}FinishGood_TypeDef;

typedef enum
{
  BUTTON_USER = 0U,
}Button_TypeDef;

/* Keep compatibility with CMSIS Pack already delivered */
#define BUTTON_KEY BUTTON_USER

typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;

#if (USE_BSP_COM_FEATURE > 0)
typedef enum
{
  COM1 = 0U,
  COMn
}COM_TypeDef;

typedef enum
{
 COM_WORDLENGTH_8B     =   UART_WORDLENGTH_8B,
 COM_WORDLENGTH_9B     =   UART_WORDLENGTH_9B,
}COM_WordLengthTypeDef;

typedef enum
{
 COM_STOPBITS_1     =   UART_STOPBITS_1,
 COM_STOPBITS_2     =   UART_STOPBITS_2,
}COM_StopBitsTypeDef;

typedef enum
{
 COM_PARITY_NONE     =  UART_PARITY_NONE,
 COM_PARITY_EVEN     =  UART_PARITY_EVEN,
 COM_PARITY_ODD      =  UART_PARITY_ODD,
}COM_ParityTypeDef;

typedef enum
{
 COM_HWCONTROL_NONE    =  UART_HWCONTROL_NONE,
 COM_HWCONTROL_RTS     =  UART_HWCONTROL_RTS,
 COM_HWCONTROL_CTS     =  UART_HWCONTROL_CTS,
 COM_HWCONTROL_RTS_CTS =  UART_HWCONTROL_RTS_CTS,
}COM_HwFlowCtlTypeDef;

typedef struct
{
  uint32_t             BaudRate;
  COM_WordLengthTypeDef  WordLength;
  COM_StopBitsTypeDef  StopBits;
  COM_ParityTypeDef    Parity;
  COM_HwFlowCtlTypeDef HwFlowCtl;
}COM_InitTypeDef;
#endif

#define MX_UART_InitTypeDef          COM_InitTypeDef
#define MX_UART_StopBitsTypeDef      COM_StopBitsTypeDef
#define MX_UART_ParityTypeDef        COM_ParityTypeDef
#define MX_UART_HwFlowCtlTypeDef     COM_HwFlowCtlTypeDef
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1U)
typedef struct
{
  void (* pMspInitCb)(UART_HandleTypeDef *);
  void (* pMspDeInitCb)(UART_HandleTypeDef *);
} BSP_COM_Cb_t;
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 1U) */

/**
 * @}
 */

#define COMn                             1U
#define COM1_UART                        UART4

#define COM_POLL_TIMEOUT                 1000
extern UART_HandleTypeDef hcom_uart[COMn];
#define  huart4 hcom_uart[COM1]

/**
 * @}
 */

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup SENSORTILEBOXPRO_LOW_LEVEL_Exported_Variables LOW LEVEL Exported Constants
  * @{
  */
extern EXTI_HandleTypeDef hpb_exti[BUTTONn];
/**
  * @}
  */

/** @defgroup SENSORTILEBOXPRO_LOW_LEVEL_Exported_Functions SENSORTILEBOXPRO LOW LEVEL Exported Functions
 * @{
 */

int32_t  BSP_GetVersion(void);
int32_t  BSP_LED_Init(Led_TypeDef Led);
int32_t  BSP_LED_DeInit(Led_TypeDef Led);
int32_t  BSP_LED_On(Led_TypeDef Led);
int32_t  BSP_LED_Off(Led_TypeDef Led);
int32_t  BSP_LED_Toggle(Led_TypeDef Led);
int32_t  BSP_LED_AllOn(void);
int32_t  BSP_LED_AllOff(void);
int32_t  BSP_LED_GetState(Led_TypeDef Led);
int32_t  BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
int32_t  BSP_PB_DeInit(Button_TypeDef Button);
int32_t  BSP_PB_GetState(Button_TypeDef Button);
void     BSP_PB_Callback(Button_TypeDef Button);
void     BSP_PB_IRQHandler (Button_TypeDef Button);
FinishGood_TypeDef BSP_CheckFinishGood(void);

#if (USE_BSP_COM_FEATURE > 0)
int32_t  BSP_COM_Init(COM_TypeDef COM);
int32_t  BSP_COM_DeInit(COM_TypeDef COM);
#endif

#if (USE_COM_LOG > 0)
int32_t  BSP_COM_SelectLogPort(COM_TypeDef COM);
#endif

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1U)
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM);
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM , BSP_COM_Cb_t *Callback);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */


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

#endif /* __SENSORTILEBOXPRO__H */

