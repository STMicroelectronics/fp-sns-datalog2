/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dfsdm.c
  * @brief   This file provides code for the configuration
  *          of the DFSDM instances.
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
#include "dfsdm.h"

/* USER CODE BEGIN 0 */
#define Error_Handler sys_error_handler
void sys_error_handler(void);
static uint32_t MX_DFSDM1_initialized = 0;
/* USER CODE END 0 */

DFSDM_Filter_HandleTypeDef hdfsdm1_filter0;
DFSDM_Filter_HandleTypeDef hdfsdm1_filter1;
DFSDM_Channel_HandleTypeDef hdfsdm1_channel0;
DFSDM_Channel_HandleTypeDef hdfsdm1_channel5;
DMA_HandleTypeDef hdma_dfsdm1_flt1;
DMA_HandleTypeDef hdma_dfsdm1_flt0;

/* DFSDM1 init function */
void MX_DFSDM1_Init(void)
{
  if (MX_DFSDM1_initialized == 0)
  {
    /* Enable DFSDM clock */
    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    __HAL_RCC_DFSDM_CLK_ENABLE();
    /* Enable the DMA clock */
    __HAL_RCC_DMA1_CLK_ENABLE();

    hdma_dfsdm1_flt0.Instance = DMA1_Channel5;
    hdma_dfsdm1_flt0.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_dfsdm1_flt0.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dfsdm1_flt0.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dfsdm1_flt0.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_dfsdm1_flt0.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_dfsdm1_flt0.Init.Mode = DMA_CIRCULAR;
    hdma_dfsdm1_flt0.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_dfsdm1_flt0.Init.Request = DMA_REQUEST_DFSDM1_FLT0;

    /* Several peripheral DMA handle pointers point to the same DMA handle.
    Be aware that there is only one channel to perform all the requested DMAs. */
    __HAL_LINKDMA(&hdfsdm1_filter0, hdmaReg, hdma_dfsdm1_flt0);
    /* Reset DMA handle state */
    __HAL_DMA_RESET_HANDLE_STATE(&hdma_dfsdm1_flt0);
    /* Configure the DMA Channel */
    if (HAL_DMA_Init(&hdma_dfsdm1_flt0) != HAL_OK)
    {
      Error_Handler();
    }
    /* DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);


    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**DFSDM1 GPIO Configuration
    PB6     ------> DFSDM1_DATIN5
    PE9     ------> DFSDM1_CKOUT
    PB10     ------> DFSDM1_DATIN7
      */
    GPIO_InitStruct.Pin = DFSDM1_DATIN5_Pin | DFSDM1_D7_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_DFSDM1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DFSDM1_CKOUT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_DFSDM1;
    HAL_GPIO_Init(DFSDM1_CKOUT_GPIO_Port, &GPIO_InitStruct);

    __HAL_DFSDM_CHANNEL_RESET_HANDLE_STATE(&hdfsdm1_channel5);
    hdfsdm1_channel5.Instance = DFSDM1_Channel5;
    hdfsdm1_channel5.Init.OutputClock.Activation = ENABLE;
    hdfsdm1_channel5.Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
    hdfsdm1_channel5.Init.OutputClock.Divider = 4;
    hdfsdm1_channel5.Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
    hdfsdm1_channel5.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
    hdfsdm1_channel5.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
    hdfsdm1_channel5.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
    hdfsdm1_channel5.Init.Awd.Oversampling = 10;
    hdfsdm1_channel5.Init.Offset = 0;
    hdfsdm1_channel5.Init.Input.Pins = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
    hdfsdm1_channel5.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_FALLING;
    hdfsdm1_channel5.Init.RightBitShift = 0x0A;

    if (HAL_DFSDM_ChannelInit(&hdfsdm1_channel5) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_DFSDM_FILTER_RESET_HANDLE_STATE(&hdfsdm1_filter0);
    hdfsdm1_filter0.Instance = DFSDM1_Filter0;
    hdfsdm1_filter0.Init.RegularParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
    hdfsdm1_filter0.Init.RegularParam.FastMode = ENABLE;
    hdfsdm1_filter0.Init.RegularParam.DmaMode = ENABLE;
    hdfsdm1_filter0.Init.InjectedParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
    hdfsdm1_filter0.Init.InjectedParam.ScanMode = DISABLE;
    hdfsdm1_filter0.Init.InjectedParam.DmaMode = DISABLE;
    hdfsdm1_filter0.Init.InjectedParam.ExtTrigger     = DFSDM_FILTER_EXT_TRIG_TIM1_TRGO;
    hdfsdm1_filter0.Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_RISING_EDGE;
    hdfsdm1_filter0.Init.FilterParam.SincOrder = DFSDM_FILTER_SINC5_ORDER;
    hdfsdm1_filter0.Init.FilterParam.Oversampling = 64;
    hdfsdm1_filter0.Init.FilterParam.IntOversampling = 1;

    if (HAL_DFSDM_FilterInit(&hdfsdm1_filter0) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_DFSDM_FilterConfigRegChannel(&hdfsdm1_filter0, DFSDM_CHANNEL_5, DFSDM_CONTINUOUS_CONV_ON) != HAL_OK)
    {
      Error_Handler();
    }

    hdma_dfsdm1_flt1.Instance = DMA1_Channel6;
    hdma_dfsdm1_flt1.Init.Request = DMA_REQUEST_DFSDM1_FLT1;
    hdma_dfsdm1_flt1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_dfsdm1_flt1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dfsdm1_flt1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dfsdm1_flt1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_dfsdm1_flt1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_dfsdm1_flt1.Init.Mode = DMA_CIRCULAR;
    hdma_dfsdm1_flt1.Init.Priority = DMA_PRIORITY_HIGH;

    /* Several peripheral DMA handle pointers point to the same DMA handle.
    Be aware that there is only one channel to perform all the requested DMAs. */
    __HAL_LINKDMA(&hdfsdm1_filter1, hdmaReg, hdma_dfsdm1_flt1);
    /* Reset DMA handle state */
    __HAL_DMA_RESET_HANDLE_STATE(&hdma_dfsdm1_flt1);
    /* Configure the DMA Channel */
    if (HAL_DMA_Init(&hdma_dfsdm1_flt1) != HAL_OK)
    {
      Error_Handler();
    }

    /* DMA1_Channel_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

    __HAL_DFSDM_CHANNEL_RESET_HANDLE_STATE(&hdfsdm1_channel0);
    hdfsdm1_channel0.Instance = DFSDM1_Channel0;
    hdfsdm1_channel0.Init.OutputClock.Activation = DISABLE;
    hdfsdm1_channel0.Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
    hdfsdm1_channel0.Init.OutputClock.Divider = 1;
    hdfsdm1_channel0.Init.Input.Multiplexer = DFSDM_CHANNEL_ADC_OUTPUT;
    hdfsdm1_channel0.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
    hdfsdm1_channel0.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
    hdfsdm1_channel0.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
    hdfsdm1_channel0.Init.Awd.Oversampling = 10;
    hdfsdm1_channel0.Init.Offset = 0x00;
    hdfsdm1_channel0.Init.Input.Pins = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
    hdfsdm1_channel0.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_FALLING;
    hdfsdm1_channel0.Init.RightBitShift = 0x08;

    if (HAL_DFSDM_ChannelInit(&hdfsdm1_channel0) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_DFSDM_FILTER_RESET_HANDLE_STATE(&hdfsdm1_filter1);
    hdfsdm1_filter1.Instance = DFSDM1_Filter1;
    hdfsdm1_filter1.Init.RegularParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
    hdfsdm1_filter1.Init.RegularParam.FastMode = ENABLE;
    hdfsdm1_filter1.Init.RegularParam.DmaMode = ENABLE;
    hdfsdm1_filter1.Init.InjectedParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
    hdfsdm1_filter1.Init.InjectedParam.ScanMode = DISABLE;
    hdfsdm1_filter1.Init.InjectedParam.DmaMode = DISABLE;
    hdfsdm1_filter1.Init.InjectedParam.ExtTrigger = DFSDM_FILTER_EXT_TRIG_TIM1_TRGO;
    hdfsdm1_filter1.Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_RISING_EDGE;
    hdfsdm1_filter1.Init.FilterParam.SincOrder = DFSDM_FILTER_SINC4_ORDER;
    hdfsdm1_filter1.Init.FilterParam.Oversampling = 16;
    hdfsdm1_filter1.Init.FilterParam.IntOversampling = 1;

    if (HAL_DFSDM_FilterInit(&hdfsdm1_filter1) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_DFSDM_FilterConfigRegChannel(&hdfsdm1_filter1, DFSDM_CHANNEL_0, DFSDM_CONTINUOUS_CONV_ON) != HAL_OK)
    {
      Error_Handler();
    }

    MX_DFSDM1_initialized++;
  }
  /* USER CODE BEGIN DFSDM1_Init 0 */

  /* USER CODE END DFSDM1_Init 0 */

  /* USER CODE BEGIN DFSDM1_Init 1 */
//  if(MX_DFSDM1_initialized == 0)
//  {
//  /* USER CODE END DFSDM1_Init 1 */
//  hdfsdm1_filter0.Instance = DFSDM1_Filter0;
//  hdfsdm1_filter0.Init.RegularParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
//  hdfsdm1_filter0.Init.RegularParam.FastMode = ENABLE;
//  hdfsdm1_filter0.Init.RegularParam.DmaMode = ENABLE;
//  hdfsdm1_filter0.Init.FilterParam.SincOrder = DFSDM_FILTER_SINC5_ORDER;
//  hdfsdm1_filter0.Init.FilterParam.Oversampling = 64;
//  hdfsdm1_filter0.Init.FilterParam.IntOversampling = 1;
//  if (HAL_DFSDM_FilterInit(&hdfsdm1_filter0) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  hdfsdm1_filter1.Instance = DFSDM1_Filter1;
//  hdfsdm1_filter1.Init.RegularParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
//  hdfsdm1_filter1.Init.RegularParam.FastMode = ENABLE;
//  hdfsdm1_filter1.Init.RegularParam.DmaMode = ENABLE;
//  hdfsdm1_filter1.Init.FilterParam.SincOrder = DFSDM_FILTER_SINC4_ORDER;
//  hdfsdm1_filter1.Init.FilterParam.Oversampling = 16;
//  hdfsdm1_filter1.Init.FilterParam.IntOversampling = 1;
//  if (HAL_DFSDM_FilterInit(&hdfsdm1_filter1) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  hdfsdm1_channel0.Instance = DFSDM1_Channel0;
//  hdfsdm1_channel0.Init.OutputClock.Activation = DISABLE;
//  hdfsdm1_channel0.Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
//  hdfsdm1_channel0.Init.OutputClock.Divider = 1;
//  hdfsdm1_channel0.Init.Input.Multiplexer = DFSDM_CHANNEL_ADC_OUTPUT;
//  hdfsdm1_channel0.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
//  hdfsdm1_channel0.Init.Input.Pins = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
//  hdfsdm1_channel0.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_FALLING;
//  hdfsdm1_channel0.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
//  hdfsdm1_channel0.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
//  hdfsdm1_channel0.Init.Awd.Oversampling = 10;
//  hdfsdm1_channel0.Init.Offset = 0x00;
//  hdfsdm1_channel0.Init.RightBitShift = 0x08;
//  if (HAL_DFSDM_ChannelInit(&hdfsdm1_channel0) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  hdfsdm1_channel5.Instance = DFSDM1_Channel5;
//  hdfsdm1_channel5.Init.OutputClock.Activation = ENABLE;
//  hdfsdm1_channel5.Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
//  hdfsdm1_channel5.Init.OutputClock.Divider = 4;
//  hdfsdm1_channel5.Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
//  hdfsdm1_channel5.Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
//  hdfsdm1_channel5.Init.Input.Pins = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
//  hdfsdm1_channel5.Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_FALLING;
//  hdfsdm1_channel5.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
//  hdfsdm1_channel5.Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
//  hdfsdm1_channel5.Init.Awd.Oversampling = 10;
//  hdfsdm1_channel5.Init.Offset = 0;
//  hdfsdm1_channel5.Init.RightBitShift = 0x0A;
//  if (HAL_DFSDM_ChannelInit(&hdfsdm1_channel5) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  if (HAL_DFSDM_FilterConfigRegChannel(&hdfsdm1_filter0, DFSDM_CHANNEL_5, DFSDM_CONTINUOUS_CONV_ON) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  if (HAL_DFSDM_FilterConfigRegChannel(&hdfsdm1_filter1, DFSDM_CHANNEL_0, DFSDM_CONTINUOUS_CONV_ON) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN DFSDM1_Init 2 */
//  MX_DFSDM1_initialized++;
//  }
  /* USER CODE END DFSDM1_Init 2 */

}

static uint32_t HAL_RCC_DFSDM1_CLK_ENABLED = 0;

static uint32_t DFSDM1_Init = 0;

void HAL_DFSDM_FilterMspInit(DFSDM_Filter_HandleTypeDef *dfsdm_filterHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if (DFSDM1_Init == 0)
  {
    /* USER CODE BEGIN DFSDM1_MspInit 0 */

    /* USER CODE END DFSDM1_MspInit 0 */

    /** Initializes the peripherals clock
      */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_DFSDM1;
    PeriphClkInit.Dfsdm1ClockSelection = RCC_DFSDM1CLKSOURCE_PCLK;
    PeriphClkInit.Dfsdm1AudioClockSelection = RCC_DFSDM1AUDIOCLKSOURCE_SAI1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* DFSDM1 clock enable */
    HAL_RCC_DFSDM1_CLK_ENABLED++;
    if (HAL_RCC_DFSDM1_CLK_ENABLED == 1)
    {
      __HAL_RCC_DFSDM1_CLK_ENABLE();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**DFSDM1 GPIO Configuration
    PB6     ------> DFSDM1_DATIN5
    PE9     ------> DFSDM1_CKOUT
    PB10     ------> DFSDM1_DATIN7
      */
    GPIO_InitStruct.Pin = DFSDM1_DATIN5_Pin | DFSDM1_D7_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_DFSDM1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DFSDM1_CKOUT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_DFSDM1;
    HAL_GPIO_Init(DFSDM1_CKOUT_GPIO_Port, &GPIO_InitStruct);

    /* USER CODE BEGIN DFSDM1_MspInit 1 */

    /* USER CODE END DFSDM1_MspInit 1 */
    DFSDM1_Init++;
  }

//    /* DFSDM1 DMA Init */
//    /* DFSDM1_FLT1 Init */
//  if(dfsdm_filterHandle->Instance == DFSDM1_Filter1){
//    hdma_dfsdm1_flt1.Instance = DMA1_Channel6;
//    hdma_dfsdm1_flt1.Init.Request = DMA_REQUEST_DFSDM1_FLT1;
//    hdma_dfsdm1_flt1.Init.Direction = DMA_PERIPH_TO_MEMORY;
//    hdma_dfsdm1_flt1.Init.PeriphInc = DMA_PINC_DISABLE;
//    hdma_dfsdm1_flt1.Init.MemInc = DMA_MINC_ENABLE;
//    hdma_dfsdm1_flt1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
//    hdma_dfsdm1_flt1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
//    hdma_dfsdm1_flt1.Init.Mode = DMA_CIRCULAR;
//    hdma_dfsdm1_flt1.Init.Priority = DMA_PRIORITY_HIGH;
//    if (HAL_DMA_Init(&hdma_dfsdm1_flt1) != HAL_OK)
//    {
//      Error_Handler();
//    }
//
//    /* Several peripheral DMA handle pointers point to the same DMA handle.
//     Be aware that there is only one channel to perform all the requested DMAs. */
//    __HAL_LINKDMA(dfsdm_filterHandle,hdmaInj,hdma_dfsdm1_flt1);
//    __HAL_LINKDMA(dfsdm_filterHandle,hdmaReg,hdma_dfsdm1_flt1);
//  }
//
//    /* DFSDM1_FLT0 Init */
//  if(dfsdm_filterHandle->Instance == DFSDM1_Filter0){
//    hdma_dfsdm1_flt0.Instance = DMA1_Channel5;
//    hdma_dfsdm1_flt0.Init.Request = DMA_REQUEST_DFSDM1_FLT0;
//    hdma_dfsdm1_flt0.Init.Direction = DMA_PERIPH_TO_MEMORY;
//    hdma_dfsdm1_flt0.Init.PeriphInc = DMA_PINC_DISABLE;
//    hdma_dfsdm1_flt0.Init.MemInc = DMA_MINC_ENABLE;
//    hdma_dfsdm1_flt0.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
//    hdma_dfsdm1_flt0.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
//    hdma_dfsdm1_flt0.Init.Mode = DMA_CIRCULAR;
//    hdma_dfsdm1_flt0.Init.Priority = DMA_PRIORITY_HIGH;
//    if (HAL_DMA_Init(&hdma_dfsdm1_flt0) != HAL_OK)
//    {
//      Error_Handler();
//    }
//
//    /* Several peripheral DMA handle pointers point to the same DMA handle.
//     Be aware that there is only one channel to perform all the requested DMAs. */
//    __HAL_LINKDMA(dfsdm_filterHandle,hdmaInj,hdma_dfsdm1_flt0);
//    __HAL_LINKDMA(dfsdm_filterHandle,hdmaReg,hdma_dfsdm1_flt0);
//  }

}

void HAL_DFSDM_ChannelMspInit(DFSDM_Channel_HandleTypeDef *dfsdm_channelHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if (DFSDM1_Init == 0)
  {
    /* USER CODE BEGIN DFSDM1_MspInit 0 */

    /* USER CODE END DFSDM1_MspInit 0 */

    /** Initializes the peripherals clock
      */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_DFSDM1;
    PeriphClkInit.Dfsdm1ClockSelection = RCC_DFSDM1CLKSOURCE_PCLK;
    PeriphClkInit.Dfsdm1AudioClockSelection = RCC_DFSDM1AUDIOCLKSOURCE_SAI1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* DFSDM1 clock enable */
    HAL_RCC_DFSDM1_CLK_ENABLED++;
    if (HAL_RCC_DFSDM1_CLK_ENABLED == 1)
    {
      __HAL_RCC_DFSDM1_CLK_ENABLE();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**DFSDM1 GPIO Configuration
    PB6     ------> DFSDM1_DATIN5
    PE9     ------> DFSDM1_CKOUT
    PB10     ------> DFSDM1_DATIN7
      */
    GPIO_InitStruct.Pin = DFSDM1_DATIN5_Pin | DFSDM1_D7_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_DFSDM1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DFSDM1_CKOUT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_DFSDM1;
    HAL_GPIO_Init(DFSDM1_CKOUT_GPIO_Port, &GPIO_InitStruct);

    /* USER CODE BEGIN DFSDM1_MspInit 1 */

    /* USER CODE END DFSDM1_MspInit 1 */
    DFSDM1_Init++;
  }
}

void HAL_DFSDM_FilterMspDeInit(DFSDM_Filter_HandleTypeDef *dfsdm_filterHandle)
{

  DFSDM1_Init-- ;
  if (DFSDM1_Init == 0)
  {
    /* USER CODE BEGIN DFSDM1_MspDeInit 0 */

    /* USER CODE END DFSDM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DFSDM1_CLK_DISABLE();

    /**DFSDM1 GPIO Configuration
    PB6     ------> DFSDM1_DATIN5
    PE9     ------> DFSDM1_CKOUT
    PB10     ------> DFSDM1_DATIN7
      */
    HAL_GPIO_DeInit(GPIOB, DFSDM1_DATIN5_Pin | DFSDM1_D7_Pin);

    HAL_GPIO_DeInit(DFSDM1_CKOUT_GPIO_Port, DFSDM1_CKOUT_Pin);

    /* DFSDM1 DMA DeInit */
    HAL_DMA_DeInit(dfsdm_filterHandle->hdmaInj);
    HAL_DMA_DeInit(dfsdm_filterHandle->hdmaReg);

    /* USER CODE BEGIN DFSDM1_MspDeInit 1 */

    /* USER CODE END DFSDM1_MspDeInit 1 */
  }
}

void HAL_DFSDM_ChannelMspDeInit(DFSDM_Channel_HandleTypeDef *dfsdm_channelHandle)
{

  DFSDM1_Init-- ;
  if (DFSDM1_Init == 0)
  {
    /* USER CODE BEGIN DFSDM1_MspDeInit 0 */

    /* USER CODE END DFSDM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DFSDM1_CLK_DISABLE();

    /**DFSDM1 GPIO Configuration
    PB6     ------> DFSDM1_DATIN5
    PE9     ------> DFSDM1_CKOUT
    PB10     ------> DFSDM1_DATIN7
      */
    HAL_GPIO_DeInit(GPIOB, DFSDM1_DATIN5_Pin | DFSDM1_D7_Pin);

    HAL_GPIO_DeInit(DFSDM1_CKOUT_GPIO_Port, DFSDM1_CKOUT_Pin);

    /* USER CODE BEGIN DFSDM1_MspDeInit 1 */

    /* USER CODE END DFSDM1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
