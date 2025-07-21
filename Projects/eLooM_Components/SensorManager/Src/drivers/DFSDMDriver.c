/**
  ******************************************************************************
  * @file    DFSDMDriver.c
  * @author  SRA - MCD
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */

#include "drivers/DFSDMDriver.h"
#include "drivers/DFSDMDriver_vtbl.h"
#include "services/sysdebug.h"

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)

/**
  * DFSDMDriver Driver virtual table.
  */
static const IDriver_vtbl sDFSDMDriver_vtbl =
{
  DFSDMDriver_vtblInit,
  DFSDMDriver_vtblStart,
  DFSDMDriver_vtblStop,
  DFSDMDriver_vtblDoEnterPowerMode,
  DFSDMDriver_vtblReset
};


/* Private member function declaration */
/***************************************/

/* Public API definition */
/*************************/

sys_error_code_t DFSDMDrvSetDataBuffer(DFSDMDriver_t *_this, int32_t *p_buffer, uint32_t buffer_size)
{
  assert_param(_this != NULL);

  /* Set buffer and buffer_size for the driver */
  _this->p_buffer = p_buffer;
  _this->buffer_size = buffer_size;

  return SYS_NO_ERROR_CODE;
}

sys_error_code_t DFSDMSetDFSDMConfig(IDriver *_this, float_t ODR)
{
  DFSDMDriver_t *p_obj = (DFSDMDriver_t *) _this;

  if (HAL_DFSDM_FilterDeInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter) != HAL_OK)
  {
    Error_Handler();
  }
  p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Instance = NULL;

  if (HAL_DFSDM_ChannelDeInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel) != HAL_OK)
  {
    Error_Handler();
  }
  p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Instance = NULL;

  if (p_obj->mx_handle.param == 7u)  /* IMP34DT05 - Digital mic */
  {
    /* Enable DFSDM clock */
    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    __HAL_RCC_DFSDM_CLK_ENABLE();
    /* Enable the DMA clock */
    __HAL_RCC_DMA1_CLK_ENABLE();

    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Instance = DMA1_Channel5;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Init.Direction = DMA_PERIPH_TO_MEMORY;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Init.PeriphInc = DMA_PINC_DISABLE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Init.MemInc = DMA_MINC_ENABLE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Init.Mode = DMA_CIRCULAR;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Init.Priority = DMA_PRIORITY_HIGH;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Init.Request = DMA_REQUEST_DFSDM1_FLT0;

    /* Several peripheral DMA handle pointers point to the same DMA handle.
    Be aware that there is only one channel to perform all the requested DMAs. */
    __HAL_LINKDMA(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter, hdmaReg, *p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma);
    /* Reset DMA handle state */
    __HAL_DMA_RESET_HANDLE_STATE(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma);
    /* Configure the DMA Channel */
    if (HAL_DMA_Init(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma) != HAL_OK)
    {
      Error_Handler();
    }
//    /* DMA IRQ Channel configuration */
//    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 5, 0);
//    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_DFSDM1_CLK_ENABLE();
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

    __HAL_DFSDM_CHANNEL_RESET_HANDLE_STATE(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel);
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Instance = DFSDM1_Channel5;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.OutputClock.Activation = ENABLE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.OutputClock.Divider = 4;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.Input.Multiplexer = DFSDM_CHANNEL_EXTERNAL_INPUTS;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.Awd.Oversampling = 10;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.Offset = 0;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.Input.Pins = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_FALLING;

    switch ((uint32_t)ODR)
    {
      case 8000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x0E;
        break;
      case 16000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x0A;
        break;
      case 32000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x0D;
        break;
      case 48000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x0A;
        break;
      default:
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x0A;
        break;
    }

    if (HAL_DFSDM_ChannelInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_DFSDM_FILTER_RESET_HANDLE_STATE(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter);
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Instance = DFSDM1_Filter0;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.RegularParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.RegularParam.FastMode = ENABLE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.RegularParam.DmaMode = ENABLE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.InjectedParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.InjectedParam.ScanMode = DISABLE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.InjectedParam.DmaMode = DISABLE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.InjectedParam.ExtTrigger     = DFSDM_FILTER_EXT_TRIG_TIM1_TRGO;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_RISING_EDGE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.IntOversampling = 1;

    switch ((uint32_t)ODR)
    {
      case 8000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC4_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 384;
        break;
      case 16000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC4_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 192;
        break;
      case 32000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC5_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 96;
        break;
      case 48000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC5_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 64;
        break;
      default:
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC5_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 64;
        break;
    }

    if (HAL_DFSDM_FilterInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_DFSDM_FilterConfigRegChannel(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter, DFSDM_CHANNEL_5,
                                         DFSDM_CONTINUOUS_CONV_ON) != HAL_OK)
    {
      Error_Handler();
    }
  }
  else  /* IMP23ABSU - Analog mic */
  {
    /* Peripheral clock enable */
    __HAL_RCC_DFSDM1_CLK_ENABLE();
    /* DMA controller clock enable */
    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Instance = DMA1_Channel6;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Init.Request = DMA_REQUEST_DFSDM1_FLT1;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Init.Direction = DMA_PERIPH_TO_MEMORY;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Init.PeriphInc = DMA_PINC_DISABLE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Init.MemInc = DMA_MINC_ENABLE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Init.Mode = DMA_CIRCULAR;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma->Init.Priority = DMA_PRIORITY_HIGH;

    /* Several peripheral DMA handle pointers point to the same DMA handle.
    Be aware that there is only one channel to perform all the requested DMAs. */
    __HAL_LINKDMA(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter, hdmaReg, *p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma);
    /* Reset DMA handle state */
    __HAL_DMA_RESET_HANDLE_STATE(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma);
    /* Configure the DMA Channel */
    if (HAL_DMA_Init(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma) != HAL_OK)
    {
      Error_Handler();
    }

//    /* DMA1_Channel_IRQn interrupt configuration */
//    HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 5, 0);
//    HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

    __HAL_DFSDM_CHANNEL_RESET_HANDLE_STATE(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel);
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Instance = DFSDM1_Channel0;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.OutputClock.Activation = DISABLE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.OutputClock.Selection = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.OutputClock.Divider = 1;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.Input.Multiplexer = DFSDM_CHANNEL_ADC_OUTPUT;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.Input.DataPacking = DFSDM_CHANNEL_STANDARD_MODE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.Awd.FilterOrder = DFSDM_CHANNEL_FASTSINC_ORDER;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.Awd.Oversampling = 10;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.Offset = 0x00;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.Input.Pins = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.SerialInterface.Type = DFSDM_CHANNEL_SPI_FALLING;

    switch ((uint32_t)ODR)
    {
      case 8000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x09;
        break;
      case 16000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x07;
        break;
      case 32000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x05;
        break;
      case 48000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x0A;
        break;
      case 96000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x07;
        break;
      case 192000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x08;
        break;
      default :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x08;
        break;
    }

    if (HAL_DFSDM_ChannelInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_DFSDM_FILTER_RESET_HANDLE_STATE(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter);
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Instance = DFSDM1_Filter1;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.RegularParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.RegularParam.FastMode = ENABLE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.RegularParam.DmaMode = ENABLE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.InjectedParam.Trigger = DFSDM_FILTER_SW_TRIGGER;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.InjectedParam.ScanMode = DISABLE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.InjectedParam.DmaMode = DISABLE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.InjectedParam.ExtTrigger = DFSDM_FILTER_EXT_TRIG_TIM1_TRGO;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_RISING_EDGE;
    p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.IntOversampling = 1;

    switch ((uint32_t)ODR)
    {
      case 8000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC2_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 384;
        break;
      case 16000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC2_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 192;
        break;
      case 32000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC2_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 96;
        break;
      case 48000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC3_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 64;
        break;
      case 96000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC3_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 32;
        break;
      case 192000 :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC4_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 16;
        break;
      default :
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC4_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 16;
        break;
    }

    if (HAL_DFSDM_FilterInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_DFSDM_FilterConfigRegChannel(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter, DFSDM_CHANNEL_0,
                                         DFSDM_CONTINUOUS_CONV_ON) != HAL_OK)
    {
      Error_Handler();
    }
  }
  return SYS_NO_ERROR_CODE;
}

/* IDriver virtual functions definition */
/****************************************/

IDriver *DFSDMDriverAlloc(void)
{
  IDriver *p_new_obj = (IDriver *) SysAlloc(sizeof(DFSDMDriver_t));

  if (p_new_obj == NULL)
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("DFSDMDriver - alloc failed.\r\n"));
  }
  else
  {
    p_new_obj->vptr = &sDFSDMDriver_vtbl;
  }

  return p_new_obj;
}

sys_error_code_t DFSDMDriver_vtblInit(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  assert_param(p_params != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DFSDMDriver_t *p_obj = (DFSDMDriver_t *) _this;
  DFSDMDriverParams_t *p_init_param = (DFSDMDriverParams_t *) p_params;
  p_obj->mx_handle.p_mx_dfsdm_cfg = p_init_param->p_mx_dfsdm_cfg;

  /* Initialize the DMA IRQ */
  p_obj->mx_handle.p_mx_dfsdm_cfg->p_mx_dma_init_f();

  /* Initialize the DFSM */
  p_obj->mx_handle.p_mx_dfsdm_cfg->p_mx_init_f();

  /* Save optional param */
  p_obj->mx_handle.param = p_init_param->param;

  return res;
}

sys_error_code_t DFSDMDriver_vtblStart(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DFSDMDriver_t *p_obj = (DFSDMDriver_t *) _this;

  /* check if the buffer as been set, otherwise I cannot start the operation */
  if (p_obj->p_buffer != NULL)
  {
    HAL_NVIC_EnableIRQ(p_obj->mx_handle.p_mx_dfsdm_cfg->irq_n);
    if (HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter, p_obj->p_buffer,
                                                   p_obj->buffer_size))
    {
      res = SYS_DFSDM_DRV_GENERIC_ERROR_CODE;
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_DFSDM_DRV_GENERIC_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
  }

  return res;
}

sys_error_code_t DFSDMDriver_vtblStop(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DFSDMDriver_t *p_obj = (DFSDMDriver_t *) _this;

  if (p_obj->p_buffer != NULL)
  {
    if (HAL_OK != HAL_DFSDM_FilterRegularStop_DMA(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter))
    {
      res = SYS_DFSDM_DRV_GENERIC_ERROR_CODE;
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_DFSDM_DRV_GENERIC_ERROR_CODE);
    }
    else
    {
      HAL_NVIC_DisableIRQ(p_obj->mx_handle.p_mx_dfsdm_cfg->irq_n);
    }
  }
  else
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
  }

  return res;
}

sys_error_code_t DFSDMDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode active_power_mode,
                                                  const EPowerMode new_power_mode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DFSDMDriver_t *p_obj = (DFSDMDriver_t *)_this;

  if (!SYS_IS_ERROR_CODE(res))
  {
    if (new_power_mode == E_POWER_MODE_STATE1)
    {
      if (active_power_mode == E_POWER_MODE_SENSORS_ACTIVE)
      {
        if (HAL_OK != HAL_DFSDM_FilterRegularStop_DMA(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter))
        {
          res = SYS_DFSDM_DRV_GENERIC_ERROR_CODE;
          SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_DFSDM_DRV_GENERIC_ERROR_CODE);
        }
        else
        {
          HAL_NVIC_DisableIRQ(p_obj->mx_handle.p_mx_dfsdm_cfg->irq_n);
        }
      }
    }
  }

  return res;
}

sys_error_code_t DFSDMDriver_vtblReset(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  /*  DFSDMDriver_t *p_obj = (DFSDMDriver_t*)_this; */

  return res;
}

/* Private function definition */
/*******************************/
