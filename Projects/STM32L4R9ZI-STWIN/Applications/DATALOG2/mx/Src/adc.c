/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
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
#include "adc.h"

/* USER CODE BEGIN 0 */

#define Error_Handler sys_error_handler
void sys_error_handler(void);

static ADC_InitUsedDef ADC_UsedFor = ADC1_NOT_USED;
/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;

/* ADC1 init function */
void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.NbrOfDiscConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.DFSDMConfig = ADC_DFSDM_MODE_ENABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_1;
  sConfig.Offset = 0x800;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}


void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PC0     ------> ADC1_IN1
    PC1     ------> ADC1_IN2
    PA5     ------> ADC1_IN10
    PC4     ------> ADC1_IN13
    */
    GPIO_InitStruct.Pin = ADC1_IN1_Pin|ADC1_IN2_Pin|uC_ADC_BATT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = EX_ADC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(EX_ADC_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PC0     ------> ADC1_IN1
    PC1     ------> ADC1_IN2
    PA5     ------> ADC1_IN10
    PC4     ------> ADC1_IN13
    */
    HAL_GPIO_DeInit(GPIOC, ADC1_IN1_Pin|ADC1_IN2_Pin|uC_ADC_BATT_Pin);

    HAL_GPIO_DeInit(EX_ADC_GPIO_Port, EX_ADC_Pin);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* ADC1 Custom init function to manage both channel for microphone and battery charger */
void MX_ADC1_CustomInit(ADC_InitUsedDef ADC_InitFor)
{
  /* If the ADC is not yet initialized */
   if (ADC_UsedFor == ADC1_NOT_USED)
   {

     /**Common config */
     hadc1.Instance = ADC1;
     hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
     hadc1.Init.Resolution = ADC_RESOLUTION_12B;
     hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
     hadc1.Init.ScanConvMode = (uint32_t)DISABLE;
     hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
     hadc1.Init.LowPowerAutoWait = DISABLE;
     hadc1.Init.ContinuousConvMode = ENABLE;
     hadc1.Init.NbrOfConversion = 1;
     hadc1.Init.NbrOfDiscConversion      = 1;
     hadc1.Init.DiscontinuousConvMode = DISABLE;
     hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
     hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;

     hadc1.Init.DMAContinuousRequests = DISABLE;

     hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;

     /* Oversampling enabled */
     hadc1.Init.OversamplingMode = DISABLE;

     hadc1.Init.Oversampling.RightBitShift         = ADC_RIGHTBITSHIFT_NONE;         /* Right shift of the oversampled summation */
     hadc1.Init.Oversampling.TriggeredMode         = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;         /* Specifies whether or not a trigger is needed for each sample */
     hadc1.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE; /* Specifies whether or not the oversampling buffer is maintained during injection sequence */
     hadc1.Init.DFSDMConfig = ADC_DFSDM_MODE_ENABLE;

     if (HAL_ADC_Init(&hadc1) != HAL_OK)
     {
       Error_Handler();
     }

     /* ### Start calibration ############################################ */
     if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
     {
       Error_Handler();
     }
   }

   /* Set that we had Initiliazed the ADC for Audio or For Battery Charger */
   ADC_UsedFor |= ADC_InitFor;

   if(ADC_InitFor == ADC1_FOR_AUDIO)
   {
     ADC_ChannelConfTypeDef sConfig = {0};

     /* Configure Regular Channel */
     sConfig.Channel = ADC_CHANNEL_2;
     sConfig.Rank = ADC_REGULAR_RANK_1;
     sConfig.SamplingTime = ADC_SAMPLETIME_12CYCLES_5;
     sConfig.SingleDiff = ADC_SINGLE_ENDED;
     sConfig.OffsetNumber = ADC_OFFSET_1;
     sConfig.Offset = 0x800;
     if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
     {
       Error_Handler();
     }
   }

   if(ADC_InitFor == ADC1_FOR_BC)
   {
     ADC_InjectionConfTypeDef sConfigInjected = {0};
     static uint8_t ADC_stopped = 0;
     uint32_t ADC_Status;

     ADC_Status = HAL_ADC_GetState(&hadc1);
     if ((ADC_Status & HAL_ADC_STATE_REG_BUSY) == HAL_ADC_STATE_REG_BUSY)
     {
       (void)HAL_ADC_Stop(&hadc1);
       ADC_stopped = 1;
     }

     /* Configure ADC injected channel */
     sConfigInjected.InjectedChannel = ADC_CHANNEL_13;
     sConfigInjected.InjectedRank = ADC_INJECTED_RANK_1;
     sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_640CYCLES_5;
     sConfigInjected.InjectedSingleDiff = ADC_SINGLE_ENDED;
     sConfigInjected.InjectedOffsetNumber = ADC_OFFSET_NONE;
     sConfigInjected.InjectedOffset = 0;
     sConfigInjected.InjectedNbrOfConversion = 1;
     sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
     sConfigInjected.AutoInjectedConv = DISABLE;
     sConfigInjected.QueueInjectedContext = DISABLE;
     sConfigInjected.ExternalTrigInjecConv = ADC_INJECTED_SOFTWARE_START;
     sConfigInjected.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONV_EDGE_NONE;
     sConfigInjected.InjecOversamplingMode = DISABLE;

     GPIO_InitTypeDef GPIO_InitStruct = {0};

     /* uC_ADC_BATT_Pin GPIO Configuration */
     __HAL_RCC_GPIOC_CLK_ENABLE();
     GPIO_InitStruct.Pin = uC_ADC_BATT_Pin;
     GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
     GPIO_InitStruct.Pull = GPIO_NOPULL;
     HAL_GPIO_Init(uC_ADC_BATT_GPIO_Port, &GPIO_InitStruct);

     (void)HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected);

     if (ADC_stopped == 1U)
     {
       (void)HAL_ADC_Start(&hadc1);
     }
   }

}

/* USER CODE END 1 */
