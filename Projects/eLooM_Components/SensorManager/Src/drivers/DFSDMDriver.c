/**
  ******************************************************************************
  * @file    DFSDMDriver.c
  * @author  SRA - MCD
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

sys_error_code_t DFSDMSetDFSDMConfig(IDriver *_this, float ODR)
{
  DFSDMDriver_t *p_obj = (DFSDMDriver_t *) _this;

  __HAL_LINKDMA(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter, hdmaReg, *p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma);
  __HAL_DMA_RESET_HANDLE_STATE(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma);
  (void)HAL_DMA_Init(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dma);

  if (p_obj->mx_handle.param == 7u)  /* IMP34DT05 - Digital mic */
  {
    if (ODR <= 16000.0f)
    {
      p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x0A;
      (void)HAL_DFSDM_ChannelInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel);
      p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC4_ORDER;
      p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 192;
      (void)HAL_DFSDM_FilterInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter);
    }
    else if (ODR <= 32000.0f)
    {
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x0D;
        (void)HAL_DFSDM_ChannelInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel);
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC5_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 96;
        (void)HAL_DFSDM_FilterInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter);
    }
    else
    {
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x0A;
        (void)HAL_DFSDM_ChannelInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel);
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC5_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 64;
        (void)HAL_DFSDM_FilterInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter);
    }
    (void)HAL_DFSDM_FilterConfigRegChannel(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter, DFSDM_CHANNEL_5, DFSDM_CONTINUOUS_CONV_ON);
  }
  else  /* IMP23ABSU - Analog mic */
  {
    if (ODR <= 16000.0f)
    {
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x07;
        (void)HAL_DFSDM_ChannelInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel);
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC2_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 192;
        (void)HAL_DFSDM_FilterInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter);
    }
    else if (ODR <= 32000.0f)
    {
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x05;
        (void)HAL_DFSDM_ChannelInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel);
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC2_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 96;
        (void)HAL_DFSDM_FilterInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter);
    }
    else if (ODR <= 48000.0f)
    {
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x0A;
        (void)HAL_DFSDM_ChannelInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel);
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC3_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 64;
        (void)HAL_DFSDM_FilterInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter);
    }
    else if (ODR <= 96000.0f)
    {
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x07;
        (void)HAL_DFSDM_ChannelInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel);
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC3_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 32;
        (void)HAL_DFSDM_FilterInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter);
    }
    else
    {
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel->Init.RightBitShift = 0x08;
        (void)HAL_DFSDM_ChannelInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_channel);
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.SincOrder = DFSDM_FILTER_SINC4_ORDER;
        p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter->Init.FilterParam.Oversampling = 16;
        (void)HAL_DFSDM_FilterInit(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter);
    }
    (void)HAL_DFSDM_FilterConfigRegChannel(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter, DFSDM_CHANNEL_0, DFSDM_CONTINUOUS_CONV_ON);
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
    if (HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(p_obj->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter, p_obj->p_buffer, p_obj->buffer_size))
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
  DFSDMDriver_t *p_obj = (DFSDMDriver_t*)_this;

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
