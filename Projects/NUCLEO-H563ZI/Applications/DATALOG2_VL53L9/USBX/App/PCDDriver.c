/**
  ******************************************************************************
  * @file    PCDDriver.c
  * @author  SRA
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

#include "PCDDriver.h"
#include "PCDDriver_vtbl.h"
#include "ux_device_class_sensor_streaming.h"
#include "services/sysdebug.h"

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)

/* 0x40 = after the 8-endpoint BTable (8×8 bytes), safe from BTable overlap */
#define PCD_EP_START_MEMORY_ADR (0x40)
#define PCD_EP_MEMORY_DIM       (0x40)
#define PCD_EP_CTRL_OUT_ADR     (0x00)
#define PCD_EP_CTRL_IN_ADR      (0x80)

#ifndef H563_USB_EXPERIMENT_B_FORCE_SINGLE_PMA
#define H563_USB_EXPERIMENT_B_FORCE_SINGLE_PMA 1
#endif

/**
  * PCDDriver Driver virtual table.
  */
static const IDriver_vtbl sPCDDriver_vtbl =
{
  PCDDriver_vtblInit,
  PCDDriver_vtblStart,
  PCDDriver_vtblStop,
  PCDDriver_vtblDoEnterPowerMode,
  PCDDriver_vtblReset
};

/* Private member function declaration */
/***************************************/

/* Public API definition */
/*************************/

sys_error_code_t PCDDrvSetFIFO(PCDDriver_t *_this, uint16_t total_fifo_size, uint16_t rx_fifo_size,
                               uint16_t ctrl_fifo_size, uint8_t n_in_ep)
{
  assert_param(_this != NULL);

  (void) total_fifo_size;
  (void) rx_fifo_size;
  (void) ctrl_fifo_size;

  if (!_this->is_initialized)
  {
    return SYS_BASE_ERROR_CODE;
  }

  uint16_t pmaadress = PCD_EP_START_MEMORY_ADR;

  /* Control EP0 OUT */
  HAL_PCDEx_PMAConfig(_this->handle.p_mx_pcd_cfg->p_pcd, PCD_EP_CTRL_OUT_ADR, PCD_SNG_BUF, pmaadress);
  pmaadress += PCD_EP_MEMORY_DIM;

  /* Control EP0 IN */
  HAL_PCDEx_PMAConfig(_this->handle.p_mx_pcd_cfg->p_pcd, PCD_EP_CTRL_IN_ADR, PCD_SNG_BUF, pmaadress);
  pmaadress += PCD_EP_MEMORY_DIM;

  /* SensorStreaming OUT endpoint */
  HAL_PCDEx_PMAConfig(_this->handle.p_mx_pcd_cfg->p_pcd, DATA_OUT_EP1, PCD_SNG_BUF, pmaadress);
  pmaadress += PCD_EP_MEMORY_DIM;

  /* SensorStreaming IN endpoints */
  for (int i = 0; i < n_in_ep; i++)
  {
#if H563_USB_EXPERIMENT_B_FORCE_SINGLE_PMA
    HAL_PCDEx_PMAConfig(_this->handle.p_mx_pcd_cfg->p_pcd, DATA_IN_EP1 + i, PCD_SNG_BUF, pmaadress);
    pmaadress += PCD_EP_MEMORY_DIM;
#else
    uint32_t pma_double_buffer_address = (uint32_t)pmaadress | ((uint32_t)(pmaadress + PCD_EP_MEMORY_DIM) << 16);
    HAL_PCDEx_PMAConfig(_this->handle.p_mx_pcd_cfg->p_pcd, DATA_IN_EP1 + i, PCD_DBL_BUF, pma_double_buffer_address);
    pmaadress += PCD_EP_MEMORY_DIM;
    pmaadress += PCD_EP_MEMORY_DIM;
#endif
  }

  return SYS_NO_ERROR_CODE;
}

sys_error_code_t PCDDrvSetExtDCD(PCDDriver_t *_this, DeviceControlDriver_t fun)
{
  assert_param(_this != NULL);

  if (fun == NULL)
  {
    return SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  fun((unsigned long) _this->handle.p_mx_pcd_cfg->p_pcd->Instance, (unsigned long) _this->handle.p_mx_pcd_cfg->p_pcd);

  return SYS_NO_ERROR_CODE;
}

/* IDriver virtual functions definition */
/****************************************/

IDriver *PCDDriverAlloc(void)
{
  IDriver *p_new_obj = (IDriver *) SysAlloc(sizeof(PCDDriver_t));

  if (p_new_obj == NULL)
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("PCDDriver - alloc failed.\r\n"));
  }
  else
  {
    p_new_obj->vptr = &sPCDDriver_vtbl;
  }

  ((PCDDriver_t *) p_new_obj)->is_initialized = false;

  return p_new_obj;
}

sys_error_code_t PCDDriver_vtblInit(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  assert_param(p_params != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  PCDDriver_t *p_obj = (PCDDriver_t *) _this;

  MX_PCDParams_t *p_init_param = (MX_PCDParams_t *) p_params;
  p_obj->handle.p_mx_pcd_cfg = p_init_param;

  /* Init PCD HW */
  p_obj->handle.p_mx_pcd_cfg->p_mx_init_f();
  p_obj->is_initialized = true;

  return res;
}

sys_error_code_t PCDDriver_vtblStart(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  PCDDriver_t *p_obj = (PCDDriver_t *) _this;

  HAL_NVIC_EnableIRQ(p_obj->handle.p_mx_pcd_cfg->irq_n);

  /* Start USB device */
  HAL_PCD_Start(p_obj->handle.p_mx_pcd_cfg->p_pcd);

  return res;
}

sys_error_code_t PCDDriver_vtblStop(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  PCDDriver_t *p_obj = (PCDDriver_t *) _this;

  /* Stop USB device and disable IRQ */
  HAL_PCD_Stop(p_obj->handle.p_mx_pcd_cfg->p_pcd);
  HAL_NVIC_DisableIRQ(p_obj->handle.p_mx_pcd_cfg->irq_n);
  return res;
}

sys_error_code_t PCDDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode active_power_mode,
                                                const EPowerMode new_power_mode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t PCDDriver_vtblReset(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

