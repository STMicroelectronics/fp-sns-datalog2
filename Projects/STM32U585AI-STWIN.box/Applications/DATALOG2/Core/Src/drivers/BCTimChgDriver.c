/**
  ******************************************************************************
  * @file    BCTimChgDriver.c
  * @author  STMicroelectronics
  * @version 2.0.0
  * @date    July 25, 2022
  *
  * @brief  BCTimChgDriver implementation.
  *
  * This file implements the Timer Driver to detect the CHG pin frequency of STBC02.
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

#include "drivers/BCTimChgDriver.h"
#include "drivers/BCTimChgDriver_vtbl.h"
#include "services/sysmem.h"
#include "services/sysdebug.h"

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)


/**
  * BCDriver Driver virtual table.
  */
static const IDriver_vtbl sBCTimChgDriver_vtbl =
{
  BCTimChgDriver_vtblInit,
  BCTimChgDriver_vtblStart,
  BCTimChgDriver_vtblStop,
  BCTimChgDriver_vtblDoEnterPowerMode,
  BCTimChgDriver_vtblReset
};

/* Private member function declaration */
/***************************************/


/* Public API definition */
/*************************/

IDriver *BCTimChgDriverAlloc(void)
{
  IDriver *p_new_obj = (IDriver *)SysAlloc(sizeof(BCTimChgDriver_t));

  if (p_new_obj == NULL)
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("BCTimChgDriver - alloc failed.\r\n"));
  }
  else
  {
    p_new_obj->vptr = &sBCTimChgDriver_vtbl;
  }

  return p_new_obj;
}

sys_error_code_t BCTimChgDriverRegisterCaptureCallback(BCTimChgDriver_t *_this, pTIM_CallbackTypeDef callback_f)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (HAL_OK != HAL_TIM_RegisterCallback(_this->mx_handle.p_mx_tim_cfg->p_tim, HAL_TIM_IC_CAPTURE_CB_ID, callback_f))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
  }

  return res;
}

/* IDriver virtual functions definition */
/****************************************/

sys_error_code_t BCTimChgDriver_vtblInit(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  assert_param(p_params != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  BCTimChgDriver_t *p_obj = (BCTimChgDriver_t *)_this;
  BCTimChgDriverParams_t *p_init_params = (BCTimChgDriverParams_t *)p_params;
  p_obj->mx_handle.p_mx_tim_cfg = p_init_params->p_mx_tim_cfg;
  p_obj->mx_handle.p_mx_gpio_chg_cfg = p_init_params->p_mx_gpio_chg_cfg;
  p_obj->mx_handle.p_mx_gpio_cen_cfg = p_init_params->p_mx_gpio_cen_cfg;

  /* Initialize the timer for the CHG pint frequency detection  */
  p_obj->mx_handle.p_mx_tim_cfg->p_mx_init_f();

  /* Initialize the GPIO used to read STBC02 state */
  p_obj->mx_handle.p_mx_gpio_chg_cfg->p_mx_init_f();

  /* Initialize the GPIO used to enable charger block */
  p_obj->mx_handle.p_mx_gpio_cen_cfg->p_mx_init_f();

  return res;
}

sys_error_code_t BCTimChgDriver_vtblStart(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  BCTimChgDriver_t *p_obj = (BCTimChgDriver_t *)_this;

  if (HAL_TIM_IC_Start_IT(p_obj->mx_handle.p_mx_tim_cfg->p_tim, TIM_CHANNEL_1) != HAL_OK)
  {
    sys_error_handler();
  }

  return res;
}

sys_error_code_t BCTimChgDriver_vtblStop(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  BCTimChgDriver_t *p_obj = (BCTimChgDriver_t *)_this;

  if ((HAL_TIM_IC_Stop_IT(p_obj->mx_handle.p_mx_tim_cfg->p_tim, TIM_CHANNEL_1)) != HAL_OK)
  {
    sys_error_handler();
  }

  return res;
}

sys_error_code_t BCTimChgDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode active_power_mode,
                                                     const EPowerMode new_power_mode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t BCTimChgDriver_vtblReset(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

/* Private function definition */
/*******************************/
