/**
 ******************************************************************************
 * @file    STWIN.box.h
 * @author  SRA
 * @brief   This file contains definitions for STWIN.box.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STWIN_BOX_H
#define STWIN_BOX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"
#include "STWIN.box_conf.h"
#include "STWIN.box_bus.h"
#include "STWIN.box_errno.h"

/** @addtogroup BSP
 * @{
 */

/** @addtogroup STWIN_BOX
 * @{
 */

/** @addtogroup STWIN_BOX_LOW_LEVEL
 * @{
 */

/**
 * @brief STWIN BSP Driver version number v1.0.0
 */
#define STWIN_BOX_BSP_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define STWIN_BOX_BSP_VERSION_SUB1   (0x00U) /*!< [23:16] sub1 version */
#define STWIN_BOX_BSP_VERSION_SUB2   (0x02U) /*!< [15:8]  sub2 version */
#define STWIN_BOX_BSP_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */
#define STWIN_BOX_BSP_VERSION         ((STWIN_BOX_BSP_VERSION_MAIN << 24)\
                                    |(STWIN_BOX_BSP_VERSION_SUB1 << 16)\
                                      |(STWIN_BOX_BSP_VERSION_SUB2 << 8 )\
                                        |(STWIN_BOX_BSP_VERSION_RC))

/** @defgroup STWIN_BOX_LOW_LEVEL_Exported_Types STWIN_BOX_LOW_LEVEL Exported Types
 * @{
 */
typedef enum
{
  LED1 = 0,
  LED_GREEN = LED1,
  LED2 = 1,
  LED_ORANGE = LED2
} Led_TypeDef;

typedef enum
{
  ISM330DHC_X,
  ISM330DHC_G,
  IIS2DH_X,
  IIS3DWB
} SPI_Device_t;

typedef enum
{
  BUTTON_USER = 0U,
  /* Alias */
  BUTTON_KEY = BUTTON_USER,
  BUTTON_PWR = 1U
} Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0U,
  BUTTON_MODE_EXTI = 1U
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
  ADC1_NOT_USED = 0,
  ADC1_FOR_AUDIO = 1,
  ADC1_FOR_BC = 2,
  ADC1_FORCE_RESTART = 4
} ADC_InitUsedDef;

/**
 * @}
 */

/** @defgroup STWIN_BOX_LOW_LEVEL_Exported_Constants STWIN_BOX_LOW_LEVEL Exported Constants
 * @{
 */

/** @defgroup STWIN_BOX_LOW_LEVEL_LED LED
 * @{
 */
#define LEDn                                    2

#define LED1_PIN                                GPIO_PIN_12
#define LED1_GPIO_PORT                          GPIOH
#define LED1_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOH_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOH_CLK_DISABLE()

#define LED2_PIN                                GPIO_PIN_10
#define LED2_GPIO_PORT                          GPIOH
#define LED2_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOH_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOH_CLK_DISABLE()

#define LEDx_GPIO_CLK_ENABLE(__INDEX__)   do { if((__INDEX__) == 0) {LED1_GPIO_CLK_ENABLE();} else\
                                                                    {LED2_GPIO_CLK_ENABLE();   }} while(0)
#define LEDx_GPIO_CLK_DISABLE(__INDEX__)  do { if((__INDEX__) == 0) {LED1_GPIO_CLK_DISABLE();} else\
                                                                    {LED2_GPIO_CLK_DISABLE();   }} while(0)

  
/* IIS2DLPC */
#define BSP_IIS2DLPC_CS_GPIO_CLK_ENABLE()       __GPIOH_CLK_ENABLE()  
#define BSP_IIS2DLPC_CS_PORT                    GPIOH
#define BSP_IIS2DLPC_CS_PIN                     GPIO_PIN_6
  
#define BSP_IIS2DLPC_INT1_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_IIS2DLPC_INT1_PORT                  GPIOF
#define BSP_IIS2DLPC_INT1_PIN                   GPIO_PIN_1
#define BSP_IIS2DLPC_INT1_EXTI_IRQn             EXTI1_IRQn
#ifndef BSP_IIS2DLPC_INT1_EXTI_IRQ_PP
#define BSP_IIS2DLPC_INT1_EXTI_IRQ_PP           7
#endif
#ifndef BSP_IIS2DLPC_INT1_EXTI_IRQ_SP
#define BSP_IIS2DLPC_INT1_EXTI_IRQ_SP           0
#endif
#define BSP_IIS2DLPC_INT1_EXTI_IRQHandler       EXTI1_IRQHandler

#define BSP_IIS2DLPC_INT2_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_IIS2DLPC_INT2_PORT                  GPIOF
#define BSP_IIS2DLPC_INT2_PIN                   GPIO_PIN_2
#define BSP_IIS2DLPC_INT2_EXTI_IRQn             EXTI2_IRQn
#ifndef BSP_IIS2DLPC_INT2_EXTI_IRQ_PP
#define BSP_IIS2DLPC_INT2_EXTI_IRQ_PP           7
#endif
#ifndef BSP_IIS2DLPC_INT2_EXTI_IRQ_SP
#define BSP_IIS2DLPC_INT2_EXTI_IRQ_SP           0
#endif
#define BSP_IIS2DLPC_INT2_EXTI_IRQHandler       EXTI2_IRQHandler

/* IIS3DWB */
#define BSP_IIS3DWB_CS_GPIO_CLK_ENABLE()        __GPIOF_CLK_ENABLE()
#define BSP_IIS3DWB_CS_PORT                     GPIOF
#define BSP_IIS3DWB_CS_PIN                      GPIO_PIN_12

#define BSP_IIS3DWB_INT1_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_IIS3DWB_INT1_PORT                   GPIOF
#define BSP_IIS3DWB_INT1_PIN                    GPIO_PIN_15
#define BSP_IIS3DWB_INT1_EXTI_IRQn              EXTI15_IRQn
#ifndef BSP_IIS3DWB_INT1_EXTI_IRQ_PP
#define BSP_IIS3DWB_INT1_EXTI_IRQ_PP            7
#endif
#ifndef BSP_IIS3DWB_INT1_EXTI_IRQ_SP
#define BSP_IIS3DWB_INT1_EXTI_IRQ_SP            0
#endif
#define BSP_IIS3DWB_INT1_EXTI_IRQHandler        EXTI15_IRQHandler
  
/* ISM330DHCX */
#define BSP_ISM330DHCX_CS_GPIO_CLK_ENABLE() 	__GPIOH_CLK_ENABLE()
#define BSP_ISM330DHCX_CS_PORT                  GPIOH
#define BSP_ISM330DHCX_CS_PIN                   GPIO_PIN_15

#define BSP_ISM330DHCX_INT1_GPIO_CLK_ENABLE()   __GPIOB_CLK_ENABLE()
#define BSP_ISM330DHCX_INT1_PORT                GPIOB
#define BSP_ISM330DHCX_INT1_PIN                 GPIO_PIN_8  
#define BSP_ISM330DHCX_INT1_EXTI_IRQn           EXTI8_IRQn
#ifndef BSP_ISM330DHCX_INT1_EXTI_IRQ_PP
#define BSP_ISM330DHCX_INT1_EXTI_IRQ_PP         7
#endif
#ifndef BSP_ISM330DHCX_INT1_EXTI_IRQ_SP
#define BSP_ISM330DHCX_INT1_EXTI_IRQ_SP         0
#endif
#define BSP_ISM330DHCX_INT1_EXTI_IRQHandler     EXTI8_IRQHandler
 
#define BSP_ISM330DHCX_INT2_GPIO_CLK_ENABLE()   __GPIOF_CLK_ENABLE()
#define BSP_ISM330DHCX_INT2_PORT                GPIOF
#define BSP_ISM330DHCX_INT2_PIN                 GPIO_PIN_4  
#define BSP_ISM330DHCX_INT2_EXTI_IRQn           EXTI4_IRQn
#ifndef BSP_ISM330DHCX_INT2_EXTI_IRQ_PP
#define BSP_ISM330DHCX_INT2_EXTI_IRQ_PP         7
#endif
#ifndef BSP_ISM330DHCX_INT2_EXTI_IRQ_SP
#define BSP_ISM330DHCX_INT2_EXTI_IRQ_SP         0
#endif
#define BSP_ISM330DHCX_INT2_EXTI_IRQHandler     EXTI4_IRQHandler
  
/* IIS2ICLX */
#define BSP_IIS2ICLX_CS_GPIO_CLK_ENABLE()       __GPIOI_CLK_ENABLE()
#define BSP_IIS2ICLX_CS_PORT                    GPIOI
#define BSP_IIS2ICLX_CS_PIN                     GPIO_PIN_7

#define BSP_IIS2ICLX_INT1_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_IIS2ICLX_INT1_PORT                  GPIOF
#define BSP_IIS2ICLX_INT1_PIN                   GPIO_PIN_3
#define BSP_IIS2ICLX_INT1_EXTI_IRQn             EXTI3_IRQn
#ifndef BSP_IIS2ICLX_INT1_EXTI_IRQ_PP
#define BSP_IIS2ICLX_INT1_EXTI_IRQ_PP           7
#endif
#ifndef BSP_IIS2ICLX_INT1_EXTI_IRQ_SP
#define BSP_IIS2ICLX_INT1_EXTI_IRQ_SP           0
#endif
#define BSP_IIS2ICLX_INT1_EXTI_IRQHandler       EXTI3_IRQHandler

#define BSP_IIS2ICLX_INT2_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_IIS2ICLX_INT2_PORT                  GPIOF
#define BSP_IIS2ICLX_INT2_PIN                   GPIO_PIN_11
#define BSP_IIS2ICLX_INT2_EXTI_IRQn             EXTI11_IRQn
#ifndef BSP_IIS2ICLX_INT2_EXTI_IRQ_PP
#define BSP_IIS2ICLX_INT2_EXTI_IRQ_PP           7
#endif
#ifndef BSP_IIS2ICLX_INT2_EXTI_IRQ_SP
#define BSP_IIS2ICLX_INT2_EXTI_IRQ_SP           0
#endif
#define BSP_IIS2ICLX_INT2_EXTI_IRQHandler       EXTI11_IRQHandler

/* ILPS22QS */  
#define BSP_ILPS22QS_INT_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOG_CLK_ENABLE()
#define BSP_ILPS22QS_INT_PORT                   GPIOG
#define BSP_ILPS22QS_INT_PIN                    GPIO_PIN_12
#define BSP_ILPS22QS_INT_EXTI_IRQn              EXTI12_IRQn
#ifndef BSP_ILPS22QS_INT_EXTI_IRQ_PP
#define BSP_ILPS22QS_INT_EXTI_IRQ_PP            7
#endif
#ifndef BSP_ILPS22QS_INT_EXTI_IRQ_SP
#define BSP_ILPS22QS_INT_EXTI_IRQ_SP            0
#endif
#define BSP_ILPS22QS_INT_EXTI_IRQHandler        EXTI12_IRQHandler

/* STTS22H */  
#define BSP_STTS22H_INT_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_STTS22H_INT_PORT                    GPIOF
#define BSP_STTS22H_INT_PIN                     GPIO_PIN_5
#define BSP_STTS22H_INT_EXTI_IRQn               EXTI5_IRQn
#ifndef BSP_STTS22H_INT_EXTI_IRQ_PP
#define BSP_STTS22H_INT_EXTI_IRQ_PP             7
#endif
#ifndef BSP_STTS22H_INT_EXTI_IRQ_SP
#define BSP_STTS22H_INT_EXTI_IRQ_SP             0
#endif
#define BSP_STTS22H_INT_EXTI_IRQHandler         EXTI5_IRQHandler

/* EXT_SPI */
#define BSP_EXT_SPI_CS_GPIO_CLK_ENABLE()       __GPIOA_CLK_ENABLE()
#define BSP_EXT_SPI_CS_PORT                    GPIOA
#define BSP_EXT_SPI_CS_PIN                     GPIO_PIN_15
                                                                      
/**
 * @}
 */

/** @defgroup STWIN_BOX_LOW_LEVEL_BUTTON BUTTON
 * @{
 */

#define BUTTONn                               2

/**
 * @brief Key push-button
 */
#define BUTTON_USER_PIN                       GPIO_PIN_0
#define BUTTON_USER_GPIO_PORT                 GPIOE
#define BUTTON_USER_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOE_CLK_ENABLE()
#define BUTTON_USER_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOE_CLK_DISABLE()
#define BUTTON_USER_EXTI_LINE                 EXTI_LINE_0
#define BUTTON_USER_EXTI_IRQn                 EXTI0_IRQn
#define H_EXTI_0			                        hpb_exti[BUTTON_USER]

#define BUTTON_PWR_PIN                        GPIO_PIN_10
#define BUTTON_PWR_GPIO_PORT                  GPIOD
#define BUTTON_PWR_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOD_CLK_ENABLE()
#define BUTTON_PWR_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOD_CLK_DISABLE()
#define BUTTON_PWR_EXTI_LINE                  EXTI_LINE_10
#define BUTTON_PWR_EXTI_IRQn                  EXTI10_IRQn
#define H_EXTI_10			                        hpb_exti[BUTTON_PWR]

#define BUTTONx_GPIO_CLK_ENABLE(__INDEX__)   do { if((__INDEX__) == 0) {USER_BUTTON_GPIO_CLK_ENABLE();} else\
                                                                    {POWER_BUTTON_GPIO_CLK_ENABLE();   }} while(0)
#define BUTTONx_GPIO_CLK_DISABLE(__INDEX__)   do { if((__INDEX__) == 0) {USER_BUTTON_GPIO_CLK_DISABLE();} else\
                                                                    {POWER_BUTTON_GPIO_CLK_DISABLE();   }} while(0)

#ifndef BUTTON_USER_IT_PRIORITY
#define BUTTON_USER_IT_PRIORITY     14U
#endif

#ifndef BUTTON_PWR_IT_PRIORITY
#define BUTTON_PWR_IT_PRIORITY      14U
#endif

/**
 * @}
 */

/** @defgroup STWIN_BOX_LOW_LEVEL_COM COM
 * @{
 */
/**
 * @brief Definition for COM portx, connected to USART2
 */
#define COMn                             1U 
#define COM1_UART                        USART2

#define COM_POLL_TIMEOUT                 1000

#define UartHandle huart2

#ifndef BSP_COM_BAUDRATE
 #define BSP_COM_BAUDRATE                921600
#endif

typedef enum
{
  COM_STOPBITS_1 = UART_STOPBITS_1,
  COM_STOPBITS_2 = UART_STOPBITS_2,
} COM_StopBitsTypeDef;

typedef enum
{
  COM_PARITY_NONE = UART_PARITY_NONE,
  COM_PARITY_EVEN = UART_PARITY_EVEN,
  COM_PARITY_ODD = UART_PARITY_ODD,
} COM_ParityTypeDef;

typedef enum
{
  COM_HWCONTROL_NONE = UART_HWCONTROL_NONE,
  COM_HWCONTROL_RTS = UART_HWCONTROL_RTS,
  COM_HWCONTROL_CTS = UART_HWCONTROL_CTS,
  COM_HWCONTROL_RTS_CTS = UART_HWCONTROL_RTS_CTS,
} COM_HwFlowCtlTypeDef;

typedef struct
{
  uint32_t BaudRate;
  uint32_t WordLength;
  COM_StopBitsTypeDef StopBits;
  COM_ParityTypeDef Parity;
  COM_HwFlowCtlTypeDef HwFlowCtl;
} COM_InitTypeDef;
/**
 * @}
 */

#define ILPS22QS_WHO_AM_I_VAL           (uint8_t)0xB1
#define IIS2DH_WHO_AM_I_VAL             (uint8_t)0x33 

extern EXTI_HandleTypeDef hpb_exti[BUTTONn];

/** @defgroup STWIN_BOX_LOW_LEVEL_Exported_Macros STWIN_BOX_LOW_LEVEL Exported Macros
 * @{
 */

/**
 * @}
 */

/** @defgroup STWIN_BOX_LOW_LEVEL_Exported_Functions STWIN_BOX_LOW_LEVEL Exported Functions
 * @{
 */
/* Exported Functions --------------------------------------------------------*/
int32_t BSP_GetVersion(void);
int32_t BSP_LED_Init(Led_TypeDef Led);
int32_t BSP_LED_DeInit(Led_TypeDef Led);
int32_t BSP_LED_On(Led_TypeDef Led);
int32_t BSP_LED_Off(Led_TypeDef Led);
int32_t BSP_LED_Toggle(Led_TypeDef Led);
int32_t BSP_LED_GetState(Led_TypeDef Led);
int32_t BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
int32_t BSP_PB_DeInit(Button_TypeDef Button);
int32_t BSP_PB_GetState(Button_TypeDef Button);
int32_t BSP_COM_Init(COM_TypeDef COM);
int32_t BSP_COM_DeInit(COM_TypeDef COM);
int32_t BSP_COM_SelectLogPort(COM_TypeDef COM);

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1) 
int32_t BSP_USART2_RegisterDefaultMspCallbacks(void);
int32_t BSP_USART2_RegisterMspCallbacks(BSP_UART_Cb_t *Callback);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

void BSP_Enable_DCDC2(void);
void BSP_Disable_DCDC2(void);
void BSP_Enable_LDO(void);
void BSP_Disable_LDO(void);
void SD_IO_CS_Init(void);
void SD_IO_CS_DeInit(void);
void BSP_PB_PWR_Init(void);

uint8_t Sensor_IO_SPI_CS_Init_All(void);

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

#endif /* STWIN_BOX_H */
