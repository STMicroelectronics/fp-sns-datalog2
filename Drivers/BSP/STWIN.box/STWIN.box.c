/**
 ******************************************************************************
 * @file    STWIN.box.c
 * @author  SRA
 * @brief   This file provides a set of firmware functions to manage
 *          LEDs
 *          USER push-buttons
 *          COM port
 *          Voltage regulators
 *          on STWIN.box board (STEVAL-STWINBX1) from STMicroelectronics.
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

/* Includes ------------------------------------------------------------------*/
#include "STWIN.box.h"
#include "stdio.h"

/** @addtogroup BSP
 * @{
 */

/** @addtogroup STWIN_BOX
 * @{
 */

/** @addtogroup STWIN_BOX_LOW_LEVEL
 * @brief This file provides a set of low level firmware functions
 * @{
 */

/** @defgroup STWIN_BOX_LOW_LEVEL_Private_TypesDefinitions STWIN_BOX_LOW_LEVEL Private Typedef
 * @{
 */
typedef void (*BSP_EXTI_LineCallback)(void);
/**
 * @}
 */

/** @defgroup STWIN_BOX_LOW_LEVEL__Private_Defines STWIN_BOX_LOW_LEVEL Private Defines
 * @{
 */

/**
 * @}
 */

/** @defgroup STWIN_BOX_LOW_LEVEL_Private_Macros STWIN_BOX_LOW_LEVEL Private Macros
 * @{
 */

/**
 * @}
 */

/** @defgroup STWIN_BOX_LOW_LEVEL_FunctionPrototypes STWIN_BOX_LOW_LEVEL Private Function Prototypes
 * @{
 */
#if (USE_BSP_COM_FEATURE > 0)
static void USART2_MspInit(UART_HandleTypeDef *huart);
static void USART2_MspDeInit(UART_HandleTypeDef *huart);
#endif
__weak HAL_StatusTypeDef MX_USART2_Init(UART_HandleTypeDef *huart);
static void BUTTON_USER_EXTI_Callback(void);
static void BUTTON_PWR_EXTI_Callback(void);
void BSP_PB_Callback(Button_TypeDef Button);

/**
 * @}
 */

/** @defgroup STWIN_BOX_LOW_LEVEL_Private_Variables STWIN_BOX_LOW_LEVEL Private Variables
 * @{
 */
/* Private Variables -----------------------------------------------------------*/
static GPIO_TypeDef *BUTTON_PORT[BUTTONn] =
{
    BUTTON_USER_GPIO_PORT,
    BUTTON_PWR_GPIO_PORT };
static const uint16_t BUTTON_PIN[BUTTONn] =
{
    BUTTON_USER_PIN,
    BUTTON_PWR_PIN };
static const IRQn_Type BUTTON_IRQn[BUTTONn] =
{
    BUTTON_USER_EXTI_IRQn,
    BUTTON_PWR_EXTI_IRQn };
static const BSP_EXTI_LineCallback BUTTON_CALLBACK[BUTTONn] =
{
    BUTTON_USER_EXTI_Callback,
    BUTTON_PWR_EXTI_Callback };
static const uint32_t BUTTON_PRIO[BUTTONn] =
{
    BUTTON_USER_IT_PRIORITY,
    BUTTON_PWR_IT_PRIORITY };
static const uint32_t BUTTON_EXTI_LINE[BUTTONn] =
{
    BUTTON_USER_EXTI_LINE,
    BUTTON_PWR_EXTI_LINE };


EXTI_HandleTypeDef hpb_exti[BUTTONn] =
{
    {
        .Line = BUTTON_USER_EXTI_LINE },
    {
        .Line = BUTTON_PWR_EXTI_LINE } };

static GPIO_TypeDef *GPIO_PORT[LEDn] =
{
    LED1_GPIO_PORT,
    LED2_GPIO_PORT };

static const uint16_t GPIO_PIN[LEDn] =
{
    LED1_PIN,
    LED2_PIN };

#if (USE_BSP_COM_FEATURE > 0)
UART_HandleTypeDef hcom_uart[COMn];

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
static uint32_t IsUsart2MspCbValid = 0;
#endif
#if (USE_BSP_COM_FEATURE > 0)
COM_TypeDef COM_ActiveLogPort = COM1;
#endif
#endif /* USE_BSP_COM_FEATURE */

/**
 * @}
 */

/** @defgroup STWIN_BOX_LOW_LEVEL_Private_Functions STWIN_BOX_LOW_LEVEL Private Functions
 * @{
 */

/**
 * @}
 */

/** @defgroup STWIN_BOX_LOW_LEVEL_Exported_Variables STWIN_BOX_LOW_LEVEL Exported Variables
 * @{
 */

/**
 * @}
 */

/** @defgroup STWIN_BOX_LOW_LEVEL_Exported_Functions STWIN_BOX_LOW_LEVEL Exported Functions
 * @{
 */


/**
 * @brief  Initialize the DCDC2.
 * @param  None
 * @retval None
 */
void BSP_Enable_DCDC2(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOI_CLK_ENABLE();

  /* Configure the DCDC2 Enable pin */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_5, GPIO_PIN_SET);
}

/**
 * @brief  Disable DCDC2.

 * @param  None
 * @retval None
 */
void BSP_Disable_DCDC2(void)
{
  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_5, GPIO_PIN_RESET);
}

/**
 * @brief  Initialize the LDO: analog power supply.
 * @param  None
 * @retval None
 */
void BSP_Enable_LDO(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOE_CLK_ENABLE();

  /* Configure the DCDC2 Enable pin */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);
}

/**
 * @brief  DeInitialize the DCDC MSP.
 * @param  None
 * @retval None
 */
void BSP_Disable_LDO(void)
{
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
}

/**
 * @brief  Configures button GPIO and EXTI Line.
 * @param  Button: Button to be configured
 *          This parameter can be one of the following values:
 *            @arg  BUTTON_USER: User Push Button
 *            @arg  BUTTON_TAMPER: Tamper Push Button
 * @param  ButtonMode Button mode
 *          This parameter can be one of the following values:
 *            @arg  BUTTON_MODE_GPIO: Button will be used as simple IO
 *            @arg  BUTTON_MODE_EXTI: Button will be connected to EXTI line
 *                                    with interrupt generation capability
 * @retval BSP status
 */
int32_t BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  GPIO_InitTypeDef gpio_init_structure;
  GPIO_TypeDef* button_port = (GPIO_TypeDef*)BUTTON_PORT[Button];

  /* Enable the BUTTON clock*/
  if(Button == BUTTON_USER)
  {
    BUTTON_USER_GPIO_CLK_ENABLE();
  }
  else
  {
    BUTTON_PWR_GPIO_CLK_ENABLE();
  }
  gpio_init_structure.Pin = BUTTON_PIN[Button];
  gpio_init_structure.Pull = GPIO_PULLDOWN;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

  if(ButtonMode == BUTTON_MODE_GPIO)
  {
    /* Configure Button pin as input */
    gpio_init_structure.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(button_port, &gpio_init_structure);
  }
  else /* (ButtonMode == BUTTON_MODE_EXTI) */
  {
    /* Configure Button pin as input with External interrupt */
    gpio_init_structure.Mode = GPIO_MODE_IT_RISING;

    HAL_GPIO_Init(button_port, &gpio_init_structure);

    (void) HAL_EXTI_GetHandle(&hpb_exti[Button], BUTTON_EXTI_LINE[Button]);
    (void) HAL_EXTI_RegisterCallback(&hpb_exti[Button], HAL_EXTI_COMMON_CB_ID, BUTTON_CALLBACK[Button]);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority((BUTTON_IRQn[Button]), BUTTON_PRIO[Button], 0x00);
    HAL_NVIC_EnableIRQ(BUTTON_IRQn[Button]);
  }
  return BSP_ERROR_NONE;
}


/**
 * @brief  DeInitialize Push Button.
 * @param  Button: Button to be configured
 *   This parameter should be: BUTTON_USER
 * @note BSP_PB_DeInit() does not disable the GPIO clock
 * @retval None
 */
int32_t BSP_PB_DeInit(Button_TypeDef Button)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_TypeDef* button_port = (GPIO_TypeDef*)BUTTON_PORT[Button];

  GPIO_InitStruct.Pin = BUTTON_PIN[Button];
  HAL_NVIC_DisableIRQ((IRQn_Type) (BUTTON_IRQn[Button]));
  
  HAL_GPIO_DeInit(button_port, GPIO_InitStruct.Pin);

  return BSP_ERROR_NONE;
}

/**
 * @brief  Return the selected Button state.
 * @param  Button: Specifies the Button to be checked.
 *   This parameter should be: BUTTON_USER
 * @retval The Button GPIO pin value.
 */
int32_t BSP_PB_GetState(Button_TypeDef Button)
{
  GPIO_PinState ret_state = HAL_GPIO_ReadPin((GPIO_TypeDef*) BUTTON_PORT[Button], BUTTON_PIN[Button]);
  return (int32_t) (ret_state);
}

/**
 * @brief  User EXTI line detection callbacks.
 * @retval None
 */
static void BUTTON_USER_EXTI_Callback(void)
{
  BSP_PB_Callback(BUTTON_USER);
}

/**
 * @brief  Pwr EXTI line detection callbacks.
 * @retval None
 */
static void BUTTON_PWR_EXTI_Callback(void)
{
  BSP_PB_Callback(BUTTON_PWR);
}

/**
 * @brief  This method returns the STM32446E EVAL BSP Driver revision
 * @param  None
 * @retval version: 0xXYZR (8bits for each decimal, R for RC)
 */
int32_t BSP_GetVersion(void)
{
  return (int32_t) STWIN_BOX_BSP_VERSION;
}

/**
 * @brief  Configures LEDs.
 * @param  Led: LED to be configured.
 *          This parameter can be one of the following values:
 *            @arg  LED1
 * @retval None
 */
int32_t BSP_LED_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable the GPIO_LED clock */
  LEDx_GPIO_CLK_ENABLE((int8_t )Led);

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = GPIO_PIN[Led];
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(GPIO_PORT[Led], &GPIO_InitStruct);

  return BSP_ERROR_NONE;
}

/**
 * @brief  DeInit LEDs.
 * @param  Led: LED to be configured.
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 * @note Led DeInit does not disable the GPIO clock nor disable the Mfx
 * @retval None
 */
int32_t BSP_LED_DeInit(Led_TypeDef Led)
{
  UNUSED(Led);
  return BSP_ERROR_NONE;
}

/**
 * @brief  Turns selected LED On.
 * @param  Led: LED to be set on
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 * @retval None
 */
int32_t BSP_LED_On(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET);
  return BSP_ERROR_NONE;
}

/**
 * @brief  Turns selected LED Off.
 * @param  Led: LED to be set off
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 * @retval None
 */
int32_t BSP_LED_Off(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
  return BSP_ERROR_NONE;
}

/**
 * @brief  Toggles the selected LED.
 * @param  Led: LED to be toggled
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 * @retval None
 */
int32_t BSP_LED_Toggle(Led_TypeDef Led)
{
  HAL_GPIO_TogglePin(GPIO_PORT[Led], GPIO_PIN[Led]);

  return BSP_ERROR_NONE;
}

/**
 * @brief  Get the selected LED state.
 * @param  Led LED to be get its state
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 * @retval LED status
 */
int32_t BSP_LED_GetState(Led_TypeDef Led)
{
  if((Led != LED1) && (Led != LED2))
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else if(HAL_GPIO_ReadPin(GPIO_PORT[Led], (uint16_t) GPIO_PIN[Led]) == GPIO_PIN_RESET)
  {
    /* Led is On */
    return 1;
  }
  else
  {
    /* Led is Off */
    return 0;
  }
}

#if (USE_BSP_COM_FEATURE > 0)
/**
 * @brief  Configures COM port.
 * @param  COM: COM port to be configured.
 *              This parameter can be COM1
 * @param  UART_Init: Pointer to a UART_HandleTypeDef structure that contains the
 *                    configuration information for the specified USART peripheral.
 * @retval BSP error code
 */
int32_t BSP_COM_Init(COM_TypeDef COM)
{
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
  /* Init the UART Msp */
  USART2_MspInit(&hcom_uart[COM]);
#else
  if(IsUsart2MspCbValid == 0U)
  {
    if(BSP_USART2_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
    {
      return BSP_ERROR_MSP_FAILURE;
    }
  }
#endif

  (void) MX_USART2_Init(&hcom_uart[COM]);

  return BSP_ERROR_NONE;
}

/**
 * @brief  DeInit COM port.
 * @param  COM COM port to be configured.
 *             This parameter can be COM1
 * @retval BSP status
 */
int32_t BSP_COM_DeInit(COM_TypeDef COM)
{
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)  
  USART2_MspDeInit(&hcom_uart[COM]);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 0) */

  if(HAL_UART_DeInit(&hcom_uart[COM]) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Configures COM port.
 * @param  huart USART handle
 *               This parameter can be COM1
 * @param  COM_Init Pointer to a UART_HandleTypeDef structure that contains the
 *                  configuration information for the specified USART peripheral.
 * @retval HAL error code
 */

/* USART2 init function */

__weak HAL_StatusTypeDef MX_USART2_Init(UART_HandleTypeDef *huart)
{
  HAL_StatusTypeDef ret = HAL_OK;
  huart->Instance = USART2;
  huart->Init.BaudRate = BSP_COM_BAUDRATE;
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = UART_STOPBITS_1;
  huart->Init.Parity = UART_PARITY_NONE;
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
  huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  huart->FifoMode = UART_FIFOMODE_DISABLE;

  if(HAL_UART_Init(huart) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if(HAL_UARTEx_SetTxFifoThreshold(huart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if(HAL_UARTEx_SetRxFifoThreshold(huart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if(HAL_UARTEx_DisableFifoMode(huart) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

/**
 * @brief  Initializes USART2 MSP.
 * @param  huart USART2 handle
 * @retval None
 */

static void USART2_MspInit(UART_HandleTypeDef *uartHandle)
{
  UNUSED(uartHandle);
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable Peripheral clock */
  __HAL_RCC_USART2_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /**USART2 GPIO Configuration    
   PD6     ------> USART2_RX
   PD5     ------> USART2_TX
   */
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

static void USART2_MspDeInit(UART_HandleTypeDef *uartHandle)
{
  UNUSED(uartHandle);

  /* Peripheral clock disable */
  __HAL_RCC_USART2_CLK_DISABLE();

  /**USART2 GPIO Configuration    
   PD6     ------> USART2_RX
   PD4     ------> USART2_DE
   PD5     ------> USART2_TX
   */
  HAL_GPIO_DeInit(GPIOD, GPIO_PIN_6 | GPIO_PIN_4 | GPIO_PIN_5);
}

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1) 
/**
 * @brief Register Default USART2 Bus Msp Callbacks
 * @retval BSP status
 */
int32_t BSP_USART2_RegisterDefaultMspCallbacks(void)
{
  int32_t ret = BSP_ERROR_NONE;
  
  __HAL_UART_RESET_HANDLE_STATE(&hcom_uart[COM1]);
  
  /* Register default MspInit/MspDeInit Callback */
  if(HAL_UART_RegisterCallback(&hcom_uart[COM1], HAL_UART_MSPINIT_CB_ID, USART2_MspInit) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else if(HAL_UART_RegisterCallback(&hcom_uart[COM1], HAL_UART_MSPDEINIT_CB_ID, USART2_MspDeInit) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    IsUsart2MspCbValid = 1U;
  }
  
  /* BSP status */  
  return ret;
}

/**
 * @brief Register USART2 Bus Msp Callback registering
 * @param Callbacks pointer to USART2 MspInit/MspDeInit callback functions
 * @retval BSP status
 */
int32_t BSP_USART2_RegisterMspCallbacks(BSP_UART_Cb_t *Callback)
{
  int32_t ret = BSP_ERROR_NONE;
  
  __HAL_UART_RESET_HANDLE_STATE(&hcom_uart[COM1]);
  
  /* Register MspInit/MspDeInit Callbacks */
  if(HAL_UART_RegisterCallback(&hcom_uart[COM1], HAL_UART_MSPINIT_CB_ID, Callback->pMspUsartInitCb) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else if(HAL_UART_RegisterCallback(&hcom_uart[COM1], HAL_UART_MSPDEINIT_CB_ID, Callback->pMspUsartDeInitCb) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    IsUsart2MspCbValid = 1U;
  }
  
  /* BSP status */  
  return ret; 
}
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

#if (USE_COM_LOG > 0)
/**
 * @brief  Select the active COM port.
 * @param  COM port to be activated.
 * @retval BSP status
 */
int32_t BSP_COM_SelectLogPort(COM_TypeDef COM)
{
  if(COM_ActiveLogPort != COM)
  {
    COM_ActiveLogPort = COM;
  }
  return BSP_ERROR_NONE;
}

#ifdef __GNUC__
int __io_putchar(int ch)
{
#else
int32_t fputc(int32_t ch, FILE *f)
{
  UNUSED(f);
#endif /* __GNUC__ */
  (void) HAL_UART_Transmit(&hcom_uart[COM_ActiveLogPort], (uint8_t*) &ch, 1, COM_POLL_TIMEOUT);
  return ch;
}
#endif /* USE_COM_LOG */
#endif /* USE_BSP_COM_FEATURE */

/**
 * @brief  Set all sensor Chip Select high. To be called before any SPI read/write
 * @param  None
 * @retval HAL_StatusTypeDef HAL Status
 */
uint8_t Sensor_IO_SPI_CS_Init_All(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Set all the pins before init to avoid glitch */
  BSP_IIS2DLPC_CS_GPIO_CLK_ENABLE();
  BSP_IIS3DWB_CS_GPIO_CLK_ENABLE();
  BSP_ISM330DHCX_CS_GPIO_CLK_ENABLE();
  BSP_IIS2ICLX_CS_GPIO_CLK_ENABLE();
  BSP_EXT_SPI_CS_GPIO_CLK_ENABLE();

  HAL_GPIO_WritePin(BSP_IIS2DLPC_CS_PORT, BSP_IIS2DLPC_CS_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(BSP_IIS3DWB_CS_PORT, BSP_IIS3DWB_CS_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(BSP_ISM330DHCX_CS_PORT, BSP_ISM330DHCX_CS_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(BSP_IIS2ICLX_CS_PORT, BSP_IIS2ICLX_CS_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(BSP_EXT_SPI_CS_PORT, BSP_EXT_SPI_CS_PIN, GPIO_PIN_SET);

  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct.Pin = BSP_IIS2DLPC_CS_PIN;
  HAL_GPIO_Init(BSP_IIS2DLPC_CS_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BSP_IIS3DWB_CS_PIN;
  HAL_GPIO_Init(BSP_IIS3DWB_CS_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BSP_ISM330DHCX_CS_PIN;
  HAL_GPIO_Init(BSP_ISM330DHCX_CS_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BSP_IIS2ICLX_CS_PIN;
  HAL_GPIO_Init(BSP_IIS2ICLX_CS_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BSP_EXT_SPI_CS_PIN;
  HAL_GPIO_Init(BSP_EXT_SPI_CS_PORT, &GPIO_InitStruct);

  return 0;
}

/**
* @brief BSP Push Button callback
* @param Button Specifies the pin connected EXTI line
* @retval None.
*/
__weak void BSP_PB_Callback(Button_TypeDef Button)
{
/* Prevent unused argument(s) compilation warning */
UNUSED(Button);
/* This function should be implemented by the user application.
It is called into this driver when an event on Button is triggered. */
}

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
