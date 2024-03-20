/**
  ******************************************************************************
  * @file    BCTimerDriver.c
  * @author  STMicroelectronics
  * @version 2.0.0
  * @date    July 25, 2022
  *
  * @brief  BCTimerDriver implementation.
  *
  * This file implements the Battery Charger Timer driver for SWire protocol
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#include "drivers/BCTimerDriver.h"
#include "drivers/BCTimerDriver_vtbl.h"
#include "services/sysmem.h"
#include "services/sysdebug.h"

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)


/**
  * BCDriver Driver virtual table.
  */
static const IDriver_vtbl sBCTimerDriver_vtbl =
{
  BCTimerDriver_vtblInit,
  BCTimerDriver_vtblStart,
  BCTimerDriver_vtblStop,
  BCTimerDriver_vtblDoEnterPowerMode,
  BCTimerDriver_vtblReset
};

/* Private member function declaration */
/***************************************/


/* Public API definition */
/*************************/

IDriver *BCTimerDriverAlloc(void)
{
  IDriver *p_new_obj = (IDriver *)SysAlloc(sizeof(BCTimerDriver_t));

  if (p_new_obj == NULL)
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("BCTimerDriver - alloc failed.\r\n"));
  }
  else
  {
    p_new_obj->vptr = &sBCTimerDriver_vtbl;
  }

  return p_new_obj;
}

sys_error_code_t BCTimerDriverRegisterElapsedCallback(BCTimerDriver_t *_this, pTIM_CallbackTypeDef callback_f)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (HAL_OK != HAL_TIM_RegisterCallback(_this->mx_handle.p_mx_tim_cfg->p_tim, HAL_TIM_PERIOD_ELAPSED_CB_ID, callback_f))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
  }

  return res;
}

/* IDriver virtual functions definition */
/****************************************/

sys_error_code_t BCTimerDriver_vtblInit(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  assert_param(p_params != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  BCTimerDriver_t *p_obj = (BCTimerDriver_t *)_this;
  BCTimerDriverParams_t *p_init_params = (BCTimerDriverParams_t *)p_params;
  p_obj->mx_handle.p_mx_tim_cfg = p_init_params->p_mx_tim_cfg;
  p_obj->mx_handle.p_mx_gpio_sw_cfg = p_init_params->p_mx_gpio_sw_cfg;

  /* Initialize the timer for the 1-wire communication  */
  p_obj->mx_handle.p_mx_tim_cfg->p_mx_init_f();

  /* Initialize the GPIO used for the 1-wire communication */
  p_obj->mx_handle.p_mx_gpio_sw_cfg->p_mx_init_f();

  return res;
}

sys_error_code_t BCTimerDriver_vtblStart(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  BCTimerDriver_t *p_obj = (BCTimerDriver_t *)_this;
  TIM_HandleTypeDef *p_tim = p_obj->mx_handle.p_mx_tim_cfg->p_tim;

  if ((HAL_TIM_Base_Start_IT(p_tim)) != HAL_OK)
  {
    sys_error_handler();
  }
  return res;
}

sys_error_code_t BCTimerDriver_vtblStop(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  BCTimerDriver_t *p_obj = (BCTimerDriver_t *)_this;
  TIM_HandleTypeDef *p_tim = p_obj->mx_handle.p_mx_tim_cfg->p_tim;

  if ((HAL_TIM_Base_Stop_IT(p_tim)) != HAL_OK)
  {
    sys_error_handler();
  }

  return res;
}

sys_error_code_t BCTimerDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode active_power_mode,
                                                    const EPowerMode new_power_mode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t BCTimerDriver_vtblReset(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}


/* Private function definition */
/*******************************/
