/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "spi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef handle_GPDMA1_Channel9;
DMA_HandleTypeDef handle_GPDMA1_Channel8;

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  SPI_AutonomousModeConfTypeDef HAL_SPI_AutonomousMode_Cfg_Struct = {0};

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x7;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi1.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  hspi1.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerState = SPI_AUTO_MODE_DISABLE;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerSelection = SPI_GRP1_GPDMA_CH0_TCF_TRG;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
  if (HAL_SPIEx_SetConfigAutonomousMode(&hspi1, &HAL_SPI_AutonomousMode_Cfg_Struct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef *spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if (spiHandle->Instance == SPI1)
  {
    /* USER CODE BEGIN SPI1_MspInit 0 */

    /* USER CODE END SPI1_MspInit 0 */

    /** Initializes the peripherals clock
      */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SPI1;
    PeriphClkInit.Spi1ClockSelection = RCC_SPI1CLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    PE13     ------> SPI1_SCK
      */
    GPIO_InitStruct.Pin = SPI_MISO_Pin | SPI_MOSI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SPI_CK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(SPI_CK_GPIO_Port, &GPIO_InitStruct);

    /* SPI1 DMA Init */
    /* GPDMA1_REQUEST_SPI1_TX Init */
    handle_GPDMA1_Channel9.Instance = GPDMA1_Channel9;
    handle_GPDMA1_Channel9.Init.Request = GPDMA1_REQUEST_SPI1_TX;
    handle_GPDMA1_Channel9.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    handle_GPDMA1_Channel9.Init.Direction = DMA_MEMORY_TO_PERIPH;
    handle_GPDMA1_Channel9.Init.SrcInc = DMA_SINC_INCREMENTED;
    handle_GPDMA1_Channel9.Init.DestInc = DMA_DINC_FIXED;
    handle_GPDMA1_Channel9.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    handle_GPDMA1_Channel9.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    handle_GPDMA1_Channel9.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
    handle_GPDMA1_Channel9.Init.SrcBurstLength = 1;
    handle_GPDMA1_Channel9.Init.DestBurstLength = 1;
    handle_GPDMA1_Channel9.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
    handle_GPDMA1_Channel9.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_GPDMA1_Channel9.Init.Mode = DMA_NORMAL;
    if (HAL_DMA_Init(&handle_GPDMA1_Channel9) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(spiHandle, hdmatx, handle_GPDMA1_Channel9);

    if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel9, DMA_CHANNEL_NPRIV) != HAL_OK)
    {
      Error_Handler();
    }

    /* GPDMA1_REQUEST_SPI1_RX Init */
    handle_GPDMA1_Channel8.Instance = GPDMA1_Channel8;
    handle_GPDMA1_Channel8.Init.Request = GPDMA1_REQUEST_SPI1_RX;
    handle_GPDMA1_Channel8.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    handle_GPDMA1_Channel8.Init.Direction = DMA_PERIPH_TO_MEMORY;
    handle_GPDMA1_Channel8.Init.SrcInc = DMA_SINC_FIXED;
    handle_GPDMA1_Channel8.Init.DestInc = DMA_DINC_INCREMENTED;
    handle_GPDMA1_Channel8.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    handle_GPDMA1_Channel8.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    handle_GPDMA1_Channel8.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
    handle_GPDMA1_Channel8.Init.SrcBurstLength = 1;
    handle_GPDMA1_Channel8.Init.DestBurstLength = 1;
    handle_GPDMA1_Channel8.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
    handle_GPDMA1_Channel8.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_GPDMA1_Channel8.Init.Mode = DMA_NORMAL;
    if (HAL_DMA_Init(&handle_GPDMA1_Channel8) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(spiHandle, hdmarx, handle_GPDMA1_Channel8);

    if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel8, DMA_CHANNEL_NPRIV) != HAL_OK)
    {
      Error_Handler();
    }

    /* SPI1 interrupt Init */
    HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SPI1_IRQn);
    /* USER CODE BEGIN SPI1_MspInit 1 */

    /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *spiHandle)
{

  if (spiHandle->Instance == SPI1)
  {
    /* USER CODE BEGIN SPI1_MspDeInit 0 */

    /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    PE13     ------> SPI1_SCK
      */
    HAL_GPIO_DeInit(GPIOA, SPI_MISO_Pin | SPI_MOSI_Pin);

    HAL_GPIO_DeInit(SPI_CK_GPIO_Port, SPI_CK_Pin);

    /* SPI1 DMA DeInit */
    HAL_DMA_DeInit(spiHandle->hdmatx);
    HAL_DMA_DeInit(spiHandle->hdmarx);

    /* SPI1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(SPI1_IRQn);
    /* USER CODE BEGIN SPI1_MspDeInit 1 */

    /* USER CODE END SPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
