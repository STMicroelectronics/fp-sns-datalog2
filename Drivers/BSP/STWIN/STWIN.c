/**
  ******************************************************************************
  * @file    STWIN.c
  * @author  SRA
  *
  *
  * @brief   This file provides low level functionalities for STWIN board
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

/* Includes ------------------------------------------------------------------*/
#include "STWIN.h"


/** @addtogroup BSP
  * @{
  */

/** @addtogroup STWIN
  * @{
  */

/** @addtogroup STWIN_LOW_LEVEL
  * @brief This file provides a set of low level firmware functions
  * @{
*/

/** @defgroup STWIN_LOW_LEVEL_Private_TypesDefinitions STWIN_LOW_LEVEL Private Typedef
  * @{
  */

/**
  * @}
  */

/** @defgroup STWIN_LOW_LEVEL__Private_Defines STWIN_LOW_LEVEL Private Defines
  * @{
  */


/**
  * @}
  */

/** @defgroup STWIN_LOW_LEVEL_Private_Macros STWIN_LOW_LEVEL Private Macros
  * @{
  */


/**
  * @}
  */

/** @defgroup STWIN_LOW_LEVEL_FunctionPrototypes STWIN_LOW_LEVEL Private Function Prototypes
  * @{
  */
static void USART2_MspInit(UART_HandleTypeDef *huart);
static void USART2_MspDeInit(UART_HandleTypeDef *huart);
__weak HAL_StatusTypeDef MX_USART2_UART_Init(UART_HandleTypeDef *huart);
void HAL_ADC_DeMspInit(ADC_HandleTypeDef *hadc);

/**
  * @}
  */

/** @defgroup STWIN_LOW_LEVEL_Private_Variables STWIN_LOW_LEVEL Private Variables
  * @{
  */
/* Private Variables -----------------------------------------------------------*/
static GPIO_TypeDef *BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT};
static const uint16_t BUTTON_PIN[BUTTONn] = {USER_BUTTON_PIN};
static const uint8_t BUTTON_IRQn[BUTTONn] = {USER_BUTTON_EXTI_IRQn};

static GPIO_TypeDef *GPIO_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT};

static const uint16_t GPIO_PIN[LEDn] = {LED1_PIN, LED2_PIN};

static USART_TypeDef *COM_USART[COMn] = {COM1_UART};
static UART_HandleTypeDef hComHandle[COMn];
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
static uint32_t IsUsart2MspCbValid = 0;
#endif


/**
  * @}
  */


/** @defgroup STWIN_LOW_LEVEL_Private_Functions STWIN_LOW_LEVEL Private Functions
  * @{
  */


/**
  * @}
  */


/** @defgroup STWIN_LOW_LEVEL_Exported_Variables STWIN_LOW_LEVEL Exported Variables
  * @{
  */

ADC_HandleTypeDef ADC1_Handle;

/**
  * @}
  */


/** @defgroup STWIN_LOW_LEVEL_Exported_Functions STWIN_LOW_LEVEL Exported Functions
  * @{
  */

static ADC_InitUsedDef ADC_UsedFor = ADC1_NOT_USED;
static __IO uint16_t VREFINT_DATA = 0;

/**
  * @brief  This method initializes the ADC peripheral used for Analog Mic and Battery Voltage Conversion
  * @param  ADC_InitFor who wants to Init the ADC: Audio or Battery Charger
  * @retval BSP_ERROR_NONE in case of success
  * @retval BSP_ERROR_PERIPH_FAILURE in case of failures
  */
int32_t BSP_ADC1_Initialization(ADC_InitUsedDef ADC_InitFor)
{
  /* If the ADC is not yet initialized */
  if (ADC_UsedFor == ADC1_NOT_USED)
  {

    /**Common config */
    ADC1_Handle.Instance = ADC1;
    ADC1_Handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    ADC1_Handle.Init.Resolution = ADC_RESOLUTION_12B;
    ADC1_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    ADC1_Handle.Init.ScanConvMode = (uint32_t)DISABLE;
    ADC1_Handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    ADC1_Handle.Init.LowPowerAutoWait = DISABLE;
    ADC1_Handle.Init.ContinuousConvMode = ENABLE;
    ADC1_Handle.Init.NbrOfConversion = 1;
    ADC1_Handle.Init.NbrOfDiscConversion      = 1;
    ADC1_Handle.Init.DiscontinuousConvMode = DISABLE;
    ADC1_Handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    ADC1_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;

    ADC1_Handle.Init.DMAContinuousRequests = DISABLE;

    ADC1_Handle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;

    /* Oversampling enabled */
    ADC1_Handle.Init.OversamplingMode = DISABLE;


    ADC1_Handle.Init.Oversampling.RightBitShift         = ADC_RIGHTBITSHIFT_NONE;         /* Right shift of the oversampled summation */
    ADC1_Handle.Init.Oversampling.TriggeredMode         = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;         /* Specifies whether or not a trigger is needed for each sample */
    ADC1_Handle.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE; /* Specifies whether or not the oversampling buffer is maintained during injection sequence */
    ADC1_Handle.Init.DFSDMConfig = ADC_DFSDM_MODE_ENABLE;

    if (HAL_ADC_Init(&ADC1_Handle) != HAL_OK)
    {
      return BSP_ERROR_PERIPH_FAILURE;
    }

    /* ### Start calibration ############################################ */
    if (HAL_ADCEx_Calibration_Start(&ADC1_Handle, ADC_SINGLE_ENDED) != HAL_OK)
    {
      return BSP_ERROR_PERIPH_FAILURE;
    }
  }

  /* Set that we had Initiliazed the ADC for Audio or For Battery Charger */
  ADC_UsedFor |= ADC_InitFor;

  return BSP_ERROR_NONE;
}

/**
  * @brief  Initialize the ADC MSP.
  * @param  hDfsdmFilter ADC handle
  * @retval None
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  UNUSED(hadc);
  /*##-1- Enable peripherals  ################################################*/
  /* ADC Periph clock enable */
  __HAL_RCC_ADC_CLK_ENABLE();
  /* ADC Periph interface clock configuration */
  __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_PLLSAI1);
}

/**
  * @brief  DeInitialize the ADC MSP.
  * @param  hDfsdmFilter ADC handle
  * @retval None
  */
void HAL_ADC_DeMspInit(ADC_HandleTypeDef *hadc)
{
  UNUSED(hadc);
  /* ADC Periph clock Disable */
  __HAL_RCC_ADC_CLK_DISABLE();
}

/**
  * @brief  This method De initializes the ADC peripheral used for Analog Mic and Battery Voltage Conversion
  * @param  ADC_InitFor who wants to Init the ADC: Audio or Battery Charger
  * @retval BSP_ERROR_NONE in case of success
  * @retval BSP_ERROR_PERIPH_FAILURE in case of failures
  */
int32_t BSP_ADC1_DeInitialization(ADC_InitUsedDef ADC_InitFor)
{
  /* Set that we had DeInitiliazed the ADC for Audio or for Battery Charger */
  ADC_UsedFor &= ~ADC_InitFor;

  /* If the ADC it's not more necessary for Audio&Battery Charger */
  if (ADC_UsedFor == ADC1_NOT_USED)
  {
    if (HAL_ADC_DeInit(&ADC1_Handle) != HAL_OK)
    {
      return BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return BSP_ERROR_NONE;
}

/**
  * @brief  Initialize the Power button PWR.
  * @param  None
  * @retval None
  */
void BSP_PB_PWR_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  POWER_BUTTON_GPIO_CLK_ENABLE();

  /* Configure Button pin as input with External interrupt */
  GPIO_InitStruct.Pin = POWER_BUTTON_PIN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;

  HAL_GPIO_Init(POWER_BUTTON_GPIO_PORT, &GPIO_InitStruct);

  /* Enable and set Button EXTI Interrupt to the lowest priority */
  HAL_NVIC_SetPriority((IRQn_Type) POWER_BUTTON_EXTI_IRQn, 0x0F, 0x00);
  HAL_NVIC_EnableIRQ((IRQn_Type) POWER_BUTTON_EXTI_IRQn);

}

/**
  * @brief  Initialize the DCDC MSP.
  * @param  None
  * @retval None
  */
void BSP_Enable_DCDC2(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  __HAL_RCC_GPIOE_CLK_ENABLE();

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;

  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET);
}

/**
  * @brief  DeInitialize the DCDC MSP.
  * @param  None
  * @retval None
  */
void BSP_Disable_DCDC2(void)
{
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_RESET);
}


/**
  * @brief  Configure Button GPIO and EXTI Line.
  * @param  Button: Specifies the Button to be configured.
  *   This parameter should be: BUTTON_USER
  * @param  ButtonMode: Specifies Button mode.
  *   This parameter can be one of following parameters:
  *     @arg BUTTON_MODE_GPIO: Button will be used as simple IO
  *     @arg BUTTON_MODE_EXTI: Button will be connected to EXTI line with interrupt
  *                            generation capability
  * @retval None
  */
int32_t BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable the BUTTON Clock */
  BUTTONx_GPIO_CLK_ENABLE(Button);

  if (ButtonMode == BUTTON_MODE_GPIO)
  {
    /* Configure Button pin as input */
    GPIO_InitStruct.Pin = BUTTON_PIN[Button];
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStruct);
  }
  else /* (ButtonMode == BUTTON_MODE_EXTI) */
  {
    /* Configure Button pin as input with External interrupt */
    GPIO_InitStruct.Pin = BUTTON_PIN[Button];
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    HAL_GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStruct);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority((IRQn_Type)(BUTTON_IRQn[Button]), 0x0F, 0x00);
    HAL_NVIC_EnableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
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

  GPIO_InitStruct.Pin = BUTTON_PIN[Button];
  HAL_NVIC_DisableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
  HAL_GPIO_DeInit(BUTTON_PORT[Button], GPIO_InitStruct.Pin);

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
  return (int32_t)(HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]) == GPIO_PIN_RESET);
}

/**
  * @brief  This method returns the STM32446E EVAL BSP Driver revision
  * @param  None
  * @retval version: 0xXYZR (8bits for each decimal, R for RC)
  */
int32_t BSP_GetVersion(void)
{
  return (int32_t)STWIN_BSP_VERSION;
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
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable the GPIO_LED clock */
  LEDx_GPIO_CLK_ENABLE((int8_t)Led);

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = GPIO_PIN[Led];
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;

  HAL_GPIO_Init(GPIO_PORT[Led], &GPIO_InitStruct);

  return BSP_ERROR_NONE;
}


/**
  * @brief  DeInit LEDs.
  * @param  Led: LED to be configured.
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
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
  *            @arg  LED3
  *            @arg  LED4
  * @retval None
  */
int32_t BSP_LED_Toggle(Led_TypeDef Led)
{
  HAL_GPIO_TogglePin(GPIO_PORT[Led], GPIO_PIN[Led]);

  return BSP_ERROR_NONE;
}

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
  USART2_MspInit(&hComHandle[COM]);
#else
  if (IsUsart2MspCbValid == 0U)
  {
    if (BSP_USART2_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
    {
      return BSP_ERROR_MSP_FAILURE;
    }
  }
#endif

  (void)MX_USART2_UART_Init(&hComHandle[COM]);

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
  /* USART configuration */
  hComHandle[COM].Instance = COM_USART[COM];

#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
  USART2_MspDeInit(&hComHandle[COM]);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 0) */

  if (HAL_UART_DeInit(&hComHandle[COM]) != HAL_OK)
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

__weak HAL_StatusTypeDef MX_USART2_UART_Init(UART_HandleTypeDef *huart)
{
  HAL_StatusTypeDef ret = HAL_OK;
  huart->Instance = USART2;
  huart->Init.BaudRate = 115200;
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
  if (HAL_RS485Ex_Init(huart, UART_DE_POLARITY_HIGH, 0, 0) != HAL_OK)
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

  /**USART2 GPIO Configuration
  PD6     ------> USART2_RX
  PD4     ------> USART2_DE
  PD5     ------> USART2_TX
    */
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_4 | GPIO_PIN_5;
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

  __HAL_UART_RESET_HANDLE_STATE(&hComHandle[COM1]);

  /* Register default MspInit/MspDeInit Callback */
  if (HAL_UART_RegisterCallback(&hComHandle[COM1], HAL_UART_MSPINIT_CB_ID, USART2_MspInit) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else if (HAL_UART_RegisterCallback(&hComHandle[COM1], HAL_UART_MSPDEINIT_CB_ID, USART2_MspDeInit) != HAL_OK)
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

  __HAL_UART_RESET_HANDLE_STATE(&hComHandle[COM1]);

  /* Register MspInit/MspDeInit Callbacks */
  if (HAL_UART_RegisterCallback(&hComHandle[COM1], HAL_UART_MSPINIT_CB_ID, Callback->pMspUsartInitCb) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else if (HAL_UART_RegisterCallback(&hComHandle[COM1], HAL_UART_MSPDEINIT_CB_ID, Callback->pMspUsartDeInitCb) != HAL_OK)
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



/**
  * @brief  Set all sensor Chip Select high. To be called before any SPI read/write
  * @param  None
  * @retval HAL_StatusTypeDef HAL Status
  */
uint8_t Sensor_IO_SPI_CS_Init_All(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Set all the pins before init to avoid glitch */
  BSP_IIS2DH_CS_GPIO_CLK_ENABLE();
  BSP_IIS3DWB_CS_GPIO_CLK_ENABLE();
  BSP_ISM330DHCX_CS_GPIO_CLK_ENABLE();

  HAL_GPIO_WritePin(BSP_IIS2DH_CS_PORT, BSP_IIS2DH_CS_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(BSP_IIS3DWB_CS_PORT, BSP_IIS3DWB_CS_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(BSP_ISM330DHCX_CS_PORT, BSP_ISM330DHCX_CS_PIN, GPIO_PIN_SET);

  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct.Pin = BSP_IIS2DH_CS_PIN;
  HAL_GPIO_Init(BSP_IIS2DH_CS_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(BSP_IIS2DH_CS_PORT, BSP_IIS2DH_CS_PIN, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = BSP_IIS3DWB_CS_PIN;
  HAL_GPIO_Init(BSP_IIS3DWB_CS_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(BSP_IIS3DWB_CS_PORT, BSP_IIS3DWB_CS_PIN, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = BSP_ISM330DHCX_CS_PIN;
  HAL_GPIO_Init(BSP_ISM330DHCX_CS_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(BSP_ISM330DHCX_CS_PORT, BSP_ISM330DHCX_CS_PIN, GPIO_PIN_SET);

  return 0;
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


