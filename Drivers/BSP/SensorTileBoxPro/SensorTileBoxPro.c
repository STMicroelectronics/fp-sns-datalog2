/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SensorTileBoxPro.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.1.0
  * @date    20-July-2023
  * @brief   Source file for the BSP Common driver
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

/* Includes ------------------------------------------------------------------*/
#include "SensorTileBoxPro.h"
#include "stm32u5xx_hal_exti.h"

/** @defgroup BSP BSP
 * @{
 */

/** @defgroup SENSORTILEBOXPRO SENSORTILEBOXPRO
 * @{
 */

/** @defgroup SENSORTILEBOXPRO_LOW_LEVEL SENSORTILEBOXPRO LOW LEVEL
 *  @brief This file provides set of firmware functions to manage Leds and push-button
 *         available on STM32U5xx-Nucleo Kit from STMicroelectronics.
 * @{
 */

/**
 * @}
 */

/** @defgroup SENSORTILEBOXPRO_LOW_LEVEL_Private_Defines SENSORTILEBOXPRO LOW LEVEL Private Defines
 * @{
 */

/** @defgroup SENSORTILEBOXPRO_LOW_LEVEL_FunctionPrototypes SENSORTILEBOXPRO LOW LEVEL Private Function Prototypes
 * @{
 */
typedef void (* BSP_EXTI_LineCallback) (void);
typedef void (* BSP_BUTTON_GPIO_Init) (void);

/**
 * @}
 */

/** @defgroup SENSORTILEBOXPRO_LOW_LEVEL_Private_Variables SENSORTILEBOXPRO LOW LEVEL Private Variables
 * @{
 */
typedef void (* BSP_LED_GPIO_Init) (void);
static GPIO_TypeDef*  LED_PORT[LEDn] = {LED1_GPIO_PORT,LED2_GPIO_PORT,LED3_GPIO_PORT,LED4_GPIO_PORT};
static const uint16_t LED_PIN[LEDn]  = {LED1_PIN,LED2_PIN,LED3_PIN,LED4_PIN};

static GPIO_TypeDef*   BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT};
static const uint16_t  BUTTON_PIN[BUTTONn]  = {USER_BUTTON_PIN};
static const IRQn_Type BUTTON_IRQn[BUTTONn] = {USER_BUTTON_EXTI_IRQn};
EXTI_HandleTypeDef hpb_exti[BUTTONn] = {{.Line = EXTI_LINE_13}};
USART_TypeDef* COM_USART[COMn] = {COM1_UART};
UART_HandleTypeDef hcom_uart[COMn];
#if (USE_COM_LOG > 0)
static COM_TypeDef COM_ActiveLogPort;
#endif
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1U)
static uint32_t IsUart4MspCbValid = 0;
#endif
__weak HAL_StatusTypeDef MX_UART4_Init(UART_HandleTypeDef* huart);

#ifndef BUS_UART4_BAUDRATE
  #define BUS_UART4_BAUDRATE                  115200U
#endif /* BUS_UART4_BAUDRATE */
/**
 * @}
 */

/** @defgroup SENSORTILEBOXPRO_LOW_LEVEL_Private_Functions SENSORTILEBOXPRO LOW LEVEL Private Functions
 * @{
 */
static void BUTTON_USER_EXTI_Callback(void);
static void BUTTON_USER_GPIO_Init(void);
#if (USE_BSP_COM_FEATURE > 0)
static void UART4_MspInit(UART_HandleTypeDef *huart);
static void UART4_MspDeInit(UART_HandleTypeDef *huart);
#endif
/**
 * @brief  This method returns the STM32U5xx NUCLEO BSP Driver revision
 * @retval version: 0xXYZR (8bits for each decimal, R for RC)
 */
int32_t BSP_GetVersion(void)
{
  return (int32_t)__SENSORTILEBOXPRO_BSP_VERSION;
}

/**
 * @brief  This method the Finish Good type
 * @retval FinishGood value
 */
FinishGood_TypeDef BSP_CheckFinishGood(void) {
  
  #define ST25_ADDR_DATA_I2C                ((uint8_t)0xAE)
  #define ST25_ICREF_REG                    ((uint16_t)0x0017)
  /* ST25DVxxKC 4Kbits ICref */
  #define IAM_ST25DV04KC                        0x50U
  /* ST25DVxxKC 16/64Kbits ICref */
  #define IAM_ST25DV64KC                        0x51U
  /* @brief ST25DV 4Kbits ICref */
  #define IAM_ST25DV04                        0x24
  /* @brief ST25DV 16/64Kbits ICref */
  #define IAM_ST25DV64                        0x26
  
  FinishGood_TypeDef FinishGood = FINISH_ERROR;
  uint8_t nfctag_id;
  BSP_I2C2_Init();
  
  BSP_I2C2_ReadReg16(ST25_ADDR_DATA_I2C, ST25_ICREF_REG, &nfctag_id, 1);
  
  if((nfctag_id == IAM_ST25DV04KC) | (nfctag_id == IAM_ST25DV64KC)) {
    FinishGood = FINISHB;
  } else if((nfctag_id == IAM_ST25DV04) | (nfctag_id == IAM_ST25DV64)) {
    FinishGood = FINISHA;
  }
     
  BSP_I2C2_DeInit();
  
  #undef ST25_ADDR_DATA_I2C
  #undef ST25_ICREF_REG
  #undef IAM_ST25DV04KC
  #undef IAM_ST25DV64KC
  #undef IAM_ST25DV04
  #undef IAM_ST25DV64
  
  return FinishGood;
}

/**
 * @brief  Configures LED on GPIO and/or on MFX.
 * @param  Led: LED to be configured.
 *              This parameter can be one of the following values:
 *              @arg  LED2, LED4, ...
 * @retval HAL status
 */
int32_t BSP_LED_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  switch(Led) {
  case LED1:

    /* Enable the GPIO_LED clock */
    LED1_GPIO_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitStruct.Pin = LED1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStruct);
    break;
  case LED2:

    /* Enable the GPIO_LED clock */
    LED2_GPIO_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitStruct.Pin = LED2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStruct);
    break;
  case LED3:

    /* Enable the GPIO_LED clock */
    LED3_GPIO_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitStruct.Pin = LED3_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStruct);
    break;
  case LED4:

    /* Enable the GPIO_LED clock */
    LED3_GPIO_CLK_ENABLE();

    /* Configure the GPIO_LED pin */
    GPIO_InitStruct.Pin = LED4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(LED4_GPIO_PORT, &GPIO_InitStruct);
    break;
  }
  return BSP_ERROR_NONE;
}

/**
 * @brief  DeInit LEDs.
 * @param  Led: LED to be configured.
 *              This parameter can be one of the following values:
 *              @arg  LED2, LED4, ...
 * @note Led DeInit does not disable the GPIO clock nor disable the Mfx
 * @retval HAL status
 */
int32_t BSP_LED_DeInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Turn off LED */
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
  /* DeInit the GPIO_LED pin */
  gpio_init_structure.Pin = LED_PIN[Led];
  HAL_GPIO_DeInit(LED_PORT[Led], gpio_init_structure.Pin);

  return BSP_ERROR_NONE;
}

/**
 * @brief  Turns selected LED On.
 * @param  Led: LED to be set on
 *              This parameter can be one of the following values:
 *              @arg  LED_GREEN
 *              @arg  LED_BLUE
 *              @arg  LED_YELLOW
 *              @arg  LED_RED
 * @retval HAL status
 */
int32_t BSP_LED_On(Led_TypeDef Led)
{

  HAL_GPIO_WritePin(LED_PORT [Led], LED_PIN [Led], GPIO_PIN_SET);

  return BSP_ERROR_NONE;
}

/**
 * @brief  Turns selected LED Off.
 * @param  Led: LED to be set off
 *              This parameter can be one of the following values:
 *              @arg  LED_GREEN
 *              @arg  LED_BLUE
 *              @arg  LED_YELLOW
 *              @arg  LED_RED
 * @retval HAL status
 */
int32_t BSP_LED_Off(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(LED_PORT [Led], LED_PIN [Led], GPIO_PIN_RESET);

  return BSP_ERROR_NONE;
}

/**
 * @brief  Toggles the selected LED.
 * @param  Led: LED to be toggled
 *              This parameter can be one of the following values:
 *              @arg  LED_GREEN
 *              @arg  LED_BLUE
 *              @arg  LED_YELLOW
 *              @arg  LED_RED
 * @retval HAL status
 */
int32_t BSP_LED_Toggle(Led_TypeDef Led)
{
  HAL_GPIO_TogglePin(LED_PORT[Led], LED_PIN[Led]);

  return BSP_ERROR_NONE;
}


/**
 * @brief  Turns All LED On.
 * @retval HAL status
 */
int32_t BSP_LED_AllOn(void)
{
  BSP_LED_On(LED_GREEN);
  BSP_LED_On(LED_RED);
  BSP_LED_On(LED_BLUE);
  BSP_LED_On(LED_YELLOW);

  return BSP_ERROR_NONE;
}


/**
 * @brief  Turns All LED On.
 * @retval HAL status
 */
int32_t BSP_LED_AllOff(void)
{
  BSP_LED_Off(LED_GREEN);
  BSP_LED_Off(LED_RED);
  BSP_LED_Off(LED_BLUE);
  BSP_LED_Off(LED_YELLOW);

  return BSP_ERROR_NONE;
}


/**
 * @brief  Get the status of the LED.
 * @param  Led: LED for which get the status
 *              This parameter can be one of the following values:
 *              @arg  LED_GREEN
 *              @arg  LED_BLUE
 *              @arg  LED_YELLOW
 *              @arg  LED_RED
 * @retval HAL status (1=high, 0=low)
 */
int32_t BSP_LED_GetState(Led_TypeDef Led)
{
  return (int32_t)(HAL_GPIO_ReadPin (LED_PORT [Led], LED_PIN [Led]) == GPIO_PIN_RESET);
}

/**
  * @brief  Configures button GPIO and EXTI Line.
  * @param  Button: Button to be configured
  *                 This parameter can be one of the following values:
  *                 @arg  BUTTON_USER: User Push Button
  * @param  ButtonMode Button mode
  *                    This parameter can be one of the following values:
  *                    @arg  BUTTON_MODE_GPIO: Button will be used as simple IO
  *                    @arg  BUTTON_MODE_EXTI: Button will be connected to EXTI line
  *                                            with interrupt generation capability
  * @retval BSP status
  */
int32_t BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  int32_t ret = BSP_ERROR_NONE;

  static const BSP_EXTI_LineCallback ButtonCallback[BUTTONn] ={BUTTON_USER_EXTI_Callback};
  static const uint32_t  BSP_BUTTON_PRIO [BUTTONn] ={BSP_BUTTON_USER_IT_PRIORITY};
  static const uint32_t BUTTON_EXTI_LINE[BUTTONn] ={USER_BUTTON_EXTI_LINE};
  static const BSP_BUTTON_GPIO_Init ButtonGpioInit[BUTTONn] = {BUTTON_USER_GPIO_Init};

  ButtonGpioInit[Button]();

  if (ButtonMode == BUTTON_MODE_EXTI)
  {
    if(HAL_EXTI_GetHandle(&hpb_exti[Button], BUTTON_EXTI_LINE[Button]) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_EXTI_RegisterCallback(&hpb_exti[Button],  HAL_EXTI_COMMON_CB_ID, ButtonCallback[Button]) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }else {
      /* Enable and set Button EXTI Interrupt to the lowest priority */
      HAL_NVIC_SetPriority((BUTTON_IRQn[Button]), BSP_BUTTON_PRIO[Button], 0x00);
      HAL_NVIC_EnableIRQ((BUTTON_IRQn[Button]));
    }
  }

  return ret;
}

/**
 * @brief  Push Button DeInit.
 * @param  Button Button to be configured
 *                This parameter can be one of the following values:
 *                @arg  BUTTON_USER: Wakeup Push Button
 * @note PB DeInit does not disable the GPIO clock
 * @retval BSP status
 */
int32_t BSP_PB_DeInit(Button_TypeDef Button)
{
  GPIO_InitTypeDef gpio_init_structure;

  gpio_init_structure.Pin = BUTTON_PIN[Button];
  HAL_NVIC_DisableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
  HAL_GPIO_DeInit(BUTTON_PORT[Button], gpio_init_structure.Pin);

  return BSP_ERROR_NONE;
}

/**
 * @brief  Returns the selected button state.
 * @param  Button Button to be addressed
 *                This parameter can be one of the following values:
 *                @arg  BUTTON_USER
 * @retval The Button GPIO pin value (GPIO_PIN_RESET = button pressed)
 */
int32_t BSP_PB_GetState(Button_TypeDef Button)
{
  return (int32_t)(HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]) == GPIO_PIN_RESET);
}

/**
 * @brief  User EXTI line detection callbacks.
 * @retval None
 */
void BSP_PB_IRQHandler (Button_TypeDef Button)
{
  HAL_EXTI_IRQHandler( &hpb_exti[Button] );
}

/**
 * @brief  BSP Push Button callback
 * @param  Button Specifies the pin connected EXTI line
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
  * @brief  User EXTI line detection callbacks.
  * @retval None
  */
static void BUTTON_USER_EXTI_Callback(void)
{
  BSP_PB_Callback(BUTTON_USER);
}

/**
  * @brief Enable the User Button GPIO
  * @retval None
  */
static void BUTTON_USER_GPIO_Init(void) {

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  /*Configure GPIO pin : PTPIN */
  GPIO_InitStruct.Pin = BUS_BSP_BUTTON_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUS_BSP_BUTTON_GPIO_PORT, &GPIO_InitStruct);
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI13_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI13_IRQn);
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
  int32_t ret = BSP_ERROR_NONE;

   hcom_uart[COM].Instance = COM_USART[COM];
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0U)
  /* Init the UART Msp */
  UART4_MspInit(&hcom_uart[COM]);
#else
  if(IsUart4MspCbValid == 0U)
  {
    if(BSP_COM_RegisterDefaultMspCallbacks(COM) != BSP_ERROR_NONE)
    {
      return BSP_ERROR_MSP_FAILURE;
    }
  }
#endif

  if (MX_UART4_Init(&hcom_uart[COM]))
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }

  return ret;
}

/**
 * @brief  DeInit COM port.
 * @param  COM COM port to be configured.
 *             This parameter can be COM1
 * @retval BSP status
 */
int32_t BSP_COM_DeInit(COM_TypeDef COM)
{
  int32_t ret = BSP_ERROR_NONE;

  /* USART configuration */
  hcom_uart[COM].Instance = COM_USART[COM];

  #if (USE_HAL_UART_REGISTER_CALLBACKS == 0U)
    UART4_MspDeInit(&hcom_uart[COM]);
  #endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 0U) */

  if(HAL_UART_DeInit(&hcom_uart[COM]) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }

  return ret;
}

/**
 * @brief  Configures COM port.
 * @param  huart USART handle
 *               This parameter can be COM1
 * @param  COM_Init Pointer to a UART_HandleTypeDef structure that contains the
 *                  configuration information for the specified USART peripheral.
 * @retval HAL error code
 */

/* UART4 init function */

__weak HAL_StatusTypeDef MX_UART4_Init(UART_HandleTypeDef* huart)
{
  HAL_StatusTypeDef ret = HAL_OK;

  huart->Instance = BUS_UART4_INSTANCE;
  huart->Init.BaudRate = BUS_UART4_BAUDRATE;
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = UART_STOPBITS_1;
  huart->Init.Parity = UART_PARITY_NONE;
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
  huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(huart) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if (HAL_UARTEx_SetTxFifoThreshold(huart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if (HAL_UARTEx_SetRxFifoThreshold(huart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if (HAL_UARTEx_DisableFifoMode(huart) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

#endif
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1U)
/**
 * @brief Register Default UART4 Bus Msp Callbacks
 * @retval BSP status
 */
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {

    __HAL_UART_RESET_HANDLE_STATE(&hcom_uart[COM]);

    /* Register default MspInit/MspDeInit Callback */
    if(HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPINIT_CB_ID, UART4_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPDEINIT_CB_ID, UART4_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsUart4MspCbValid = 1U;
    }
  }

  /* BSP status */
  return ret;
}

/**
 * @brief Register UART4 Bus Msp Callback registering
 * @param Callbacks pointer to UART4 MspInit/MspDeInit callback functions
 * @retval BSP status
 */
int32_t BSP_COM_RegisterMspCallbacks (COM_TypeDef COM , BSP_COM_Cb_t *Callback)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    __HAL_UART_RESET_HANDLE_STATE(&hcom_uart[COM]);

    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPINIT_CB_ID, Callback->pMspInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPDEINIT_CB_ID, Callback->pMspDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsUart4MspCbValid = 1U;
    }
  }

  /* BSP status */
  return ret;
}
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

#if (USE_COM_LOG > 0)
/**
 * @brief  Select the active COM port.
 * @param  COM COM port to be activated.
 *             This parameter can be COM1
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

#if defined(__CC_ARM) /* For arm compiler 5 */
#if !defined(__MICROLIB) /* If not Microlib */

struct __FILE
{
  int dummyVar; //Just for the sake of redefining __FILE, we won't we using it anyways ;)
};

FILE __stdout;

#endif /* If not Microlib */
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) /* For arm compiler 6 */
#if !defined(__MICROLIB) /* If not Microlib */

FILE __stdout;

#endif /* If not Microlib */
#endif /* For arm compiler 5 */
#if defined(__ICCARM__) /* For IAR */
size_t __write(int Handle, const unsigned char *Buf, size_t Bufsize)
{
  size_t i;

  for(i=0; i<Bufsize; i++)
  {
    (void)HAL_UART_Transmit(&hcom_uart[COM_ActiveLogPort], (uint8_t *)&Buf[i], 1, COM_POLL_TIMEOUT);
  }

  return Bufsize;
}
#elif defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) /* For ARM Compiler 5 and 6 */
int fputc (int ch, FILE *f)
{
  (void)HAL_UART_Transmit(&hcom_uart[COM_ActiveLogPort], (uint8_t *)&ch, 1, COM_POLL_TIMEOUT);
  return ch;
}
#else /* For GCC Toolchains */
int __io_putchar (int ch)
{
  (void)HAL_UART_Transmit(&hcom_uart[COM_ActiveLogPort], (uint8_t *)&ch, 1, COM_POLL_TIMEOUT);
  return ch;
}
#endif /* For IAR */
#endif /* USE_COM_LOG */
/**
 * @brief  Initializes UART4 MSP.
 * @param  huart UART4 handle
 * @retval None
 */

static void UART4_MspInit(UART_HandleTypeDef* uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    /** Initializes the peripherals clock
    */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_UART4;
    PeriphClkInit.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
  /* USER CODE BEGIN UART4_MspInit 0 */

  /* USER CODE END UART4_MspInit 0 */
    /* Enable Peripheral clock */
    __HAL_RCC_UART4_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**UART4 GPIO Configuration
    PA0     ------> UART4_TX
    PA1     ------> UART4_RX
    */
    GPIO_InitStruct.Pin = BUS_UART4_TX_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = BUS_UART4_TX_GPIO_AF;
    HAL_GPIO_Init(BUS_UART4_TX_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BUS_UART4_RX_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = BUS_UART4_RX_GPIO_AF;
    HAL_GPIO_Init(BUS_UART4_RX_GPIO_PORT, &GPIO_InitStruct);

  /* USER CODE BEGIN UART4_MspInit 1 */

  /* USER CODE END UART4_MspInit 1 */
}

static void UART4_MspDeInit(UART_HandleTypeDef* uartHandle)
{
  /* USER CODE BEGIN UART4_MspDeInit 0 */

  /* USER CODE END UART4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART4_CLK_DISABLE();

    /**UART4 GPIO Configuration
    PA0     ------> UART4_TX
    PA1     ------> UART4_RX
    */
    HAL_GPIO_DeInit(BUS_UART4_TX_GPIO_PORT, BUS_UART4_TX_GPIO_PIN);

    HAL_GPIO_DeInit(BUS_UART4_RX_GPIO_PORT, BUS_UART4_RX_GPIO_PIN);

  /* USER CODE BEGIN UART4_MspDeInit 1 */

  /* USER CODE END UART4_MspDeInit 1 */
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

