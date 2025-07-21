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
#include "services/sysdebug.h"

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)

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

  if (!_this->is_initialized)
  {
    return SYS_BASE_ERROR_CODE;
  }

  uint16_t remaining_size = total_fifo_size;

  /* OUT EP: not used for this class */
  HAL_PCD_SetRxFiFo(_this->handle.p_mx_pcd_cfg->p_pcd, rx_fifo_size);
  remaining_size -= rx_fifo_size;

  /* Control EP: used for PnPL commands */
  HAL_PCDEx_SetTxFiFo(_this->handle.p_mx_pcd_cfg->p_pcd, 0, ctrl_fifo_size);
  remaining_size -= ctrl_fifo_size;

  /* IN EP: used for raw data transmission */
  for (int i = 0; i < n_in_ep; i++)
  {
    HAL_PCDEx_SetTxFiFo(_this->handle.p_mx_pcd_cfg->p_pcd, i + 1, remaining_size / n_in_ep);
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

