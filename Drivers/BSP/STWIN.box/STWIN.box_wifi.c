/**
 ******************************************************************************
 * @file    STWIN.box_wifi.c
 * @author  SRA
 * @brief   This file provides a set of firmware functions to manage
 *          MCU peripherals for MXChip WiFi Module
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
#include "STWIN.box_wifi.h"
#include "mx_wifi.h"
#include <string.h>

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
LPTIM_HandleTypeDef hlptim1;
DMA_HandleTypeDef handle_GPDMA1_Channel4;
DMA_HandleTypeDef handle_GPDMA1_Channel5;

/* Private function prototypes -----------------------------------------------*/
static int32_t WIFI_MX_SPI1_Init(void);
static int32_t WIFI_MX_GPDMA1_Init(void);
static int32_t WIFI_MX_GPIO_Init(void);
static int32_t WIFI_MX_LPTIM1_Init(void);

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
void WIFI_SPI_MspInit(SPI_HandleTypeDef* hspi);
void WIFI_SPI_MspDeInit(SPI_HandleTypeDef* hspi);
void WIFI_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi);
void WIFI_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi);
void WIFI_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi);
#endif

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
void WIFI_LPTIM_MspInit(LPTIM_HandleTypeDef* lptimHandle);
void WIFI_LPTIM_MspDeInit(LPTIM_HandleTypeDef* lptimHandle);
void WIFI_LPTIM_IC_CaptureCallback(LPTIM_HandleTypeDef *hlptim);
#endif

void HAL_SPI_TransferCallback(SPI_HandleTypeDef *hspi);

/* Private functions ---------------------------------------------------------*/


/**
 * @brief  Configures MCU peripherals for MX WiFi Module
 *         GPIO
 *         LPTIM1: for WiFi Flow IRQ without using EXTI15
 *         SPI1
 *         GPDMA1: for SPI1
 * @retval BSP status
 */
int32_t BSP_WIFI_MX_GPIO_Init(void)
{
  if(WIFI_MX_GPIO_Init() != BSP_ERROR_NONE)
  {
    return BSP_ERROR_NO_INIT;
  }

  if(WIFI_MX_LPTIM1_Init() != BSP_ERROR_NONE)
  {
    return BSP_ERROR_NO_INIT;
  }

  if(WIFI_MX_GPDMA1_Init() != BSP_ERROR_NONE)
  {
    return BSP_ERROR_NO_INIT;
  }

  if(WIFI_MX_SPI1_Init() != BSP_ERROR_NONE)
  {
    return BSP_ERROR_NO_INIT;
  }

  return BSP_ERROR_NONE;
}


static int32_t WIFI_MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableIO2VM();
  while(!(PWR->SVMCR & PWR_SVMCR_IO2VMEN));
  HAL_PWREx_EnableVddIO2();
  while(!(PWR->SVMSR & PWR_SVMSR_VDDIO2RDY));

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MXCHIP_NSS_GPIO_Port, MXCHIP_NSS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MXCHIP_RESET_GPIO_Port, MXCHIP_RESET_Pin, GPIO_PIN_RESET);

  /****** Using LPTIM1_IRQ instead of EXTI15_IRQn *****/
//  GPIO_InitStruct.Pin = MXCHIP_FLOW_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(MXCHIP_FLOW_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MXCHIP_NOTIFY_Pin */
  GPIO_InitStruct.Pin = MXCHIP_NOTIFY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MXCHIP_NOTIFY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MXCHIP_NSS_Pin */
  GPIO_InitStruct.Pin = MXCHIP_NSS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(MXCHIP_NSS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MXCHIP_RESET_Pin */
  GPIO_InitStruct.Pin = MXCHIP_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(MXCHIP_RESET_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(MXCHIP_NOTIFY_EXTI_IRQn, MXCHIP_NOTIFY_IT_PRIORITY, 0);
  HAL_NVIC_EnableIRQ(MXCHIP_NOTIFY_EXTI_IRQn);

  /****** Using LPTIM1_IRQ instead of EXTI15_IRQn *****/
//  HAL_NVIC_SetPriority(EXTI15_IRQn, 5, 0);
//  HAL_NVIC_EnableIRQ(EXTI15_IRQn);
  return BSP_ERROR_NONE;
}


static int32_t WIFI_MX_LPTIM1_Init(void)
{
  LPTIM_IC_ConfigTypeDef sConfig = {0};

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
  HAL_LPTIM_RegisterCallback(&hlptim1, HAL_LPTIM_MSPINIT_CB_ID, WIFI_LPTIM_MspInit);
  HAL_LPTIM_RegisterCallback(&hlptim1, HAL_LPTIM_MSPDEINIT_CB_ID, WIFI_LPTIM_MspDeInit);
#endif

  hlptim1.Instance = LPTIM1;
  hlptim1.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
  hlptim1.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;
  hlptim1.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
  hlptim1.Init.Period = 65535;
  hlptim1.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
  hlptim1.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
  hlptim1.Init.Input1Source = LPTIM_INPUT1SOURCE_GPIO;
  hlptim1.Init.Input2Source = LPTIM_INPUT2SOURCE_GPIO;
  hlptim1.Init.RepetitionCounter = 0;
  if (HAL_LPTIM_Init(&hlptim1) != HAL_OK)
  {
    return BSP_ERROR_NO_INIT;
  }

  sConfig.ICInputSource = LPTIM_IC1SOURCE_GPIO;
  sConfig.ICPrescaler = LPTIM_ICPSC_DIV1;
  sConfig.ICPolarity = LPTIM_ICPOLARITY_RISING;
  sConfig.ICFilter = LPTIM_ICFLT_CLOCK_DIV1;
  if (HAL_LPTIM_IC_ConfigChannel(&hlptim1, &sConfig, LPTIM_CHANNEL_1) != HAL_OK)
  {
    return BSP_ERROR_NO_INIT;
  }

  if (HAL_LPTIM_IC_Start_IT(&hlptim1, LPTIM_CHANNEL_1) != HAL_OK)
  {
    return BSP_ERROR_NO_INIT;
  }

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
  HAL_LPTIM_RegisterCallback(&hlptim1, HAL_LPTIM_IC_CAPTURE_CB_ID, WIFI_LPTIM_IC_CaptureCallback);
#endif

  return BSP_ERROR_NONE;
}

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
void WIFI_LPTIM_MspInit(LPTIM_HandleTypeDef* lptimHandle)
#else
void HAL_LPTIM_MspInit(LPTIM_HandleTypeDef* lptimHandle)
#endif
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(lptimHandle->Instance==LPTIM1)
  {
  /** Initializes the peripherals clock */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPTIM1;
    PeriphClkInit.Lptim1ClockSelection = RCC_LPTIM1CLKSOURCE_HSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      while(1);
    }

    /* LPTIM1 clock enable */
    __HAL_RCC_LPTIM1_CLK_ENABLE();

    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**LPTIM1 GPIO Configuration
    PG15     ------> LPTIM1_CH1  */
    GPIO_InitStruct.Pin = MXCHIP_FLOW_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_LPTIM1;
    HAL_GPIO_Init(MXCHIP_FLOW_GPIO_Port, &GPIO_InitStruct);

    /* LPTIM1 interrupt Init */
    HAL_NVIC_SetPriority(LPTIM1_IRQn, MXCHIP_FLOW_IT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(LPTIM1_IRQn);
  }
}

#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
void WIFI_LPTIM_MspDeInit(LPTIM_HandleTypeDef* lptimHandle)
#else
void HAL_LPTIM_MspDeInit(LPTIM_HandleTypeDef* lptimHandle)
#endif
{

  if(lptimHandle->Instance==LPTIM1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_LPTIM1_CLK_DISABLE();

    /**LPTIM1 GPIO Configuration
    PG15     ------> LPTIM1_CH1  */
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_15);

    /* LPTIM1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(LPTIM1_IRQn);
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static int32_t WIFI_MX_SPI1_Init(void)
{
  SPI_AutonomousModeConfTypeDef HAL_SPI_AutonomousMode_Cfg_Struct = {0};

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
  HAL_SPI_RegisterCallback(&hspi1, HAL_SPI_MSPINIT_CB_ID, WIFI_SPI_MspInit);
  HAL_SPI_RegisterCallback(&hspi1, HAL_SPI_MSPDEINIT_CB_ID, WIFI_SPI_MspDeInit);
#endif

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x7;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi1.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  hspi1.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    return BSP_ERROR_NO_INIT;
  }
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerState = SPI_AUTO_MODE_DISABLE;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerSelection = SPI_GRP1_GPDMA_CH0_TCF_TRG;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
  if (HAL_SPIEx_SetConfigAutonomousMode(&hspi1, &HAL_SPI_AutonomousMode_Cfg_Struct) != HAL_OK)
  {
    return BSP_ERROR_NO_INIT;
  }

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
  HAL_SPI_RegisterCallback(&hspi1, HAL_SPI_TX_COMPLETE_CB_ID, WIFI_SPI_TxCpltCallback);
  HAL_SPI_RegisterCallback(&hspi1, HAL_SPI_RX_COMPLETE_CB_ID, WIFI_SPI_RxCpltCallback);
  HAL_SPI_RegisterCallback(&hspi1, HAL_SPI_TX_RX_COMPLETE_CB_ID, WIFI_SPI_TxRxCpltCallback);
#endif

  return BSP_ERROR_NONE;
}

static int32_t WIFI_MX_GPDMA1_Init(void)
{
  __HAL_RCC_GPDMA1_CLK_ENABLE();
  HAL_NVIC_SetPriority(GPDMA1_Channel4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(GPDMA1_Channel4_IRQn);
  HAL_NVIC_SetPriority(GPDMA1_Channel5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(GPDMA1_Channel5_IRQn);

  return BSP_ERROR_NONE;
}



#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
void WIFI_SPI_MspInit(SPI_HandleTypeDef* hspi)
#else
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
#endif
{
  GPIO_InitTypeDef GPIO_InitStruct = { 0 };
  RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };
  if(hspi->Instance == SPI1)
  {
    /* USER CODE BEGIN SPI1_MspInit 0 */
    handle_GPDMA1_Channel5.Init.SrcBurstLength = 1U;
    handle_GPDMA1_Channel5.Init.DestBurstLength = 1U;
    handle_GPDMA1_Channel4.Init.SrcBurstLength = 1U;
    handle_GPDMA1_Channel4.Init.DestBurstLength = 1U;
    /* USER CODE END SPI1_MspInit 0 */

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SPI1;
    PeriphClkInit.Spi1ClockSelection = RCC_SPI1CLKSOURCE_SYSCLK;
    if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      while(1);
    }

    /* Peripheral clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_EnableIO2VM();
    while(!(PWR->SVMCR & PWR_SVMCR_IO2VMEN));
    HAL_PWREx_EnableVddIO2();
    while(!(PWR->SVMSR & PWR_SVMSR_VDDIO2RDY));
    /**SPI1 GPIO Configuration
     PG4     ------> SPI1_MOSI
     PG3     ------> SPI1_MISO
     PG2     ------> SPI1_SCK
     */
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    handle_GPDMA1_Channel4.Instance = GPDMA1_Channel4;
    handle_GPDMA1_Channel4.Init.Request = GPDMA1_REQUEST_SPI1_RX;
    handle_GPDMA1_Channel4.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    handle_GPDMA1_Channel4.Init.Direction = DMA_PERIPH_TO_MEMORY;
    handle_GPDMA1_Channel4.Init.SrcInc = DMA_SINC_FIXED;
    handle_GPDMA1_Channel4.Init.DestInc = DMA_DINC_INCREMENTED;
    handle_GPDMA1_Channel4.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    handle_GPDMA1_Channel4.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    handle_GPDMA1_Channel4.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
    handle_GPDMA1_Channel4.Init.SrcBurstLength = 1U;
    handle_GPDMA1_Channel4.Init.DestBurstLength = 1U;
    handle_GPDMA1_Channel4.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT1;
    handle_GPDMA1_Channel4.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_GPDMA1_Channel4.Init.Mode = DMA_NORMAL;
    if(HAL_DMA_Init(&handle_GPDMA1_Channel4) != HAL_OK)
    {
      while(1);
    }
    if(HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel4, DMA_CHANNEL_NPRIV) != HAL_OK)
    {
      while(1);
    }
    __HAL_LINKDMA(hspi, hdmarx, handle_GPDMA1_Channel4);

    handle_GPDMA1_Channel5.Instance = GPDMA1_Channel5;
    handle_GPDMA1_Channel5.Init.Request = GPDMA1_REQUEST_SPI1_TX;
    handle_GPDMA1_Channel5.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    handle_GPDMA1_Channel5.Init.Direction = DMA_MEMORY_TO_PERIPH;
    handle_GPDMA1_Channel5.Init.SrcInc = DMA_SINC_INCREMENTED;
    handle_GPDMA1_Channel5.Init.DestInc = DMA_DINC_FIXED;
    handle_GPDMA1_Channel5.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    handle_GPDMA1_Channel5.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    handle_GPDMA1_Channel5.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
    handle_GPDMA1_Channel5.Init.SrcBurstLength = 1U;
    handle_GPDMA1_Channel5.Init.DestBurstLength = 1U;
    handle_GPDMA1_Channel5.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT1;
    handle_GPDMA1_Channel5.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_GPDMA1_Channel5.Init.Mode = DMA_NORMAL;
    if(HAL_DMA_Init(&handle_GPDMA1_Channel5) != HAL_OK)
    {
      while(1);
    }
    if(HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel5, DMA_CHANNEL_NPRIV) != HAL_OK)
    {
      while(1);
    }
    __HAL_LINKDMA(hspi, hdmatx, handle_GPDMA1_Channel5);

    /* SPI1 interrupt Init */
    HAL_NVIC_SetPriority(SPI1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(SPI1_IRQn);
  }
}

/**
* @brief SPI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hspi: SPI handle pointer
* @retval None
*/
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
void WIFI_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
#else
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
#endif
{
  if(hspi->Instance==SPI1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PB4 (NJTRST)     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI
    PB3 (JTDO/TRACESWO)     ------> SPI1_SCK
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_3);
  }
}



/**
  * @brief Tx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
void WIFI_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi)
#else
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
#endif
{
  if(hspi->Instance == SPI1)
  {
    HAL_SPI_TransferCallback(hspi);
  }
}

/**
  * @brief Rx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
void WIFI_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi)
#else
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
#endif
{
  if(hspi->Instance == SPI1)
  {
    HAL_SPI_TransferCallback(hspi);
  }
}

/**
  * @brief Tx and Rx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)
void WIFI_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi)
#else
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
#endif
{
  if(hspi->Instance == SPI1)
  {
    HAL_SPI_TransferCallback(hspi);
  }
}


/**
* @brief  Input Capture callback in non-blocking mode
* @param  hlptim LPTIM handle
* @retval None
*/
#if (USE_HAL_LPTIM_REGISTER_CALLBACKS == 1)
void WIFI_LPTIM_IC_CaptureCallback(LPTIM_HandleTypeDef *hlptim)
#else
void HAL_LPTIM_IC_CaptureCallback(LPTIM_HandleTypeDef *hlptim)
#endif
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hlptim);

  mxchip_WIFI_ISR(MXCHIP_FLOW_Pin);
  nx_driver_emw3080_interrupt();
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
  
