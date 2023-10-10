/**
  ******************************************************************************
  * @file    STWIN.h
  * @author  SRA
  *
  *
  * @brief   This file contains definitions for STWIN.c file
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
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STWIN_H
#define STWIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "STWIN_conf.h"
#include "STWIN_bus.h"
#include "STWIN_errno.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STWIN
  * @{
  */

/** @addtogroup STWIN_LOW_LEVEL
  * @{
*/


/**
  * @brief STWIN BSP Driver version number v1.5.0
  */
#define STWIN_BSP_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define STWIN_BSP_VERSION_SUB1   (0x05U) /*!< [23:16] sub1 version */
#define STWIN_BSP_VERSION_SUB2   (0x00U) /*!< [15:8]  sub2 version */
#define STWIN_BSP_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */
#define STWIN_BSP_VERSION         ((STWIN_BSP_VERSION_MAIN << 24)\
                                   |(STWIN_BSP_VERSION_SUB1 << 16)\
                                   |(STWIN_BSP_VERSION_SUB2 << 8 )\
                                   |(STWIN_BSP_VERSION_RC))


/** @defgroup STWIN_LOW_LEVEL_Exported_Types STWIN_LOW_LEVEL Exported Types
  * @{
  */
typedef enum
{
  LED1 = 0,
  LED_GREEN = LED1,
  LED2 = 1,
  LED_ORANGE = LED2
}
Led_TypeDef;

typedef enum
{
  ISM330DHC_X,
  ISM330DHC_G,
  IIS2DH_X,
  IIS3DWB
} SPI_Device_t;

typedef enum
{
  BUTTON_USER = 0,
  /* Alias */
  BUTTON_KEY = BUTTON_USER
} Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;

typedef enum
{
  COM1 = 0U,
} COM_TypeDef;
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
typedef struct
{
  pUART_CallbackTypeDef  pMspUsartInitCb;
  pUART_CallbackTypeDef  pMspUsartDeInitCb;
} BSP_UART_Cb_t;
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 1) */

typedef enum
{
  ADC1_NOT_USED      = 0,
  ADC1_FOR_AUDIO     = 1,
  ADC1_FOR_BC        = 2,
  ADC1_FORCE_RESTART = 4
} ADC_InitUsedDef;


/**
  * @}
  */

/** @defgroup STWIN_LOW_LEVEL_Exported_Constants STWIN_LOW_LEVEL Exported Constants
  * @{
  */


/** @defgroup STWIN_LOW_LEVEL_LED LED
  * @{
  */
#define LEDn                                    2

#define LED1_PIN                                GPIO_PIN_1
#define LED1_GPIO_PORT                          GPIOE
#define LED1_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOE_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOE_CLK_DISABLE()

#define LED2_PIN                                GPIO_PIN_0
#define LED2_GPIO_PORT                          GPIOD
#define LED2_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOD_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOD_CLK_DISABLE()

#define LEDx_GPIO_CLK_ENABLE(__INDEX__)   do { if((__INDEX__) == 0) {LED1_GPIO_CLK_ENABLE();} else\
                                                                    {LED2_GPIO_CLK_ENABLE();   }} while(0)
#define LEDx_GPIO_CLK_DISABLE(__INDEX__)  do { if((__INDEX__) == 0) {LED1_GPIO_CLK_DISABLE();} else\
                                                                    {LED2_GPIO_CLK_DISABLE();   }} while(0)


/**
  * @}
  */

/** @defgroup STWIN_LOW_LEVEL_BUTTON BUTTON
  * @{
  */

#define BUTTONn                                 1

/**
  * @brief Key push-button
  */
#define POWER_BUTTON_PIN                       GPIO_PIN_10
#define POWER_BUTTON_GPIO_PORT                 GPIOD
#define POWER_BUTTON_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOD_CLK_ENABLE()
#define POWER_BUTTON_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOD_CLK_DISABLE()
#define POWER_BUTTON_EXTI_LINE                 GPIO_PIN_10
#define POWER_BUTTON_EXTI_IRQn                 EXTI15_10_IRQn

#define USER_BUTTON_PIN                       GPIO_PIN_0
#define USER_BUTTON_GPIO_PORT                 GPIOE
#define USER_BUTTON_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOE_CLK_ENABLE()
#define USER_BUTTON_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOE_CLK_DISABLE()
#define USER_BUTTON_EXTI_LINE                 GPIO_PIN_0
#define USER_BUTTON_EXTI_IRQn                 EXTI0_IRQn

#define BUTTONx_GPIO_CLK_ENABLE(__INDEX__)    USER_BUTTON_GPIO_CLK_ENABLE()
#define BUTTONx_GPIO_CLK_DISABLE(__INDEX__)   USER_BUTTON_GPIO_CLK_DISABLE()

/**
  * @}
  */

/** @defgroup STWIN_LOW_LEVEL_COM COM
  * @{
  */
/**
  * @brief Definition for COM portx, connected to USART2
  */
#define COMn                             1U
#define COM1_UART                        USART2

#define COM_POLL_TIMEOUT                 1000

#define UartHandle huart2

typedef enum
{
COM_STOPBITS_1 =   UART_STOPBITS_1,
COM_STOPBITS_2 =   UART_STOPBITS_2,
} COM_StopBitsTypeDef;

typedef enum
{
COM_PARITY_NONE =  UART_PARITY_NONE,
COM_PARITY_EVEN =  UART_PARITY_EVEN,
COM_PARITY_ODD  =  UART_PARITY_ODD,
} COM_ParityTypeDef;

typedef enum
{
COM_HWCONTROL_NONE    =  UART_HWCONTROL_NONE,
COM_HWCONTROL_RTS     =  UART_HWCONTROL_RTS,
COM_HWCONTROL_CTS     =  UART_HWCONTROL_CTS,
COM_HWCONTROL_RTS_CTS =  UART_HWCONTROL_RTS_CTS,
} COM_HwFlowCtlTypeDef;

typedef struct
{
uint32_t             BaudRate;
uint32_t             WordLength;
COM_StopBitsTypeDef  StopBits;
COM_ParityTypeDef    Parity;
COM_HwFlowCtlTypeDef HwFlowCtl;
} COM_InitTypeDef;
/**
  * @}
  */



#define HTS221_WHO_AM_I_VAL             (uint8_t)0xBC
#define LPS22HH_WHO_AM_I_VAL            (uint8_t)0xB1
#define IIS2DH_WHO_AM_I_VAL             (uint8_t)0x33




/** @defgroup STWIN_LOW_LEVEL_Exported_Macros STWIN_LOW_LEVEL Exported Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup STWIN_LOW_LEVEL_Exported_Functions STWIN_LOW_LEVEL Exported Functions
  * @{
  */
/* Exported Functions --------------------------------------------------------*/
int32_t  BSP_GetVersion(void);
int32_t  BSP_LED_Init(Led_TypeDef Led);
int32_t  BSP_LED_DeInit(Led_TypeDef Led);
int32_t  BSP_LED_On(Led_TypeDef Led);
int32_t  BSP_LED_Off(Led_TypeDef Led);
int32_t  BSP_LED_Toggle(Led_TypeDef Led);
int32_t  BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
int32_t  BSP_PB_DeInit(Button_TypeDef Button);
int32_t  BSP_PB_GetState(Button_TypeDef Button);
int32_t  BSP_COM_Init(COM_TypeDef COM);
int32_t  BSP_COM_DeInit(COM_TypeDef COM);

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
int32_t BSP_USART2_RegisterDefaultMspCallbacks(void);
int32_t BSP_USART2_RegisterMspCallbacks(BSP_UART_Cb_t *Callback);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

void BSP_Enable_DCDC2(void);
void BSP_Disable_DCDC2(void);
void SD_IO_CS_Init(void);
void SD_IO_CS_DeInit(void);
void BSP_PB_PWR_Init(void);
int32_t BSP_ADC1_Initialization(ADC_InitUsedDef ADC_InitFor);
int32_t BSP_ADC1_DeInitialization(ADC_InitUsedDef ADC_InitFor);

uint8_t Sensor_IO_SPI_CS_Init_All(void);

extern ADC_HandleTypeDef ADC1_Handle;

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

#endif /* STWIN_H */


