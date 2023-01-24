/**
  ******************************************************************************
  * @file    AnalogMicDriver.c
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

#include "drivers/AnalogMicDriver.h"
#include "drivers/AnalogMicDriver_vtbl.h"
#include "services/sysdebug.h"

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)

/**
  * AnalogMicDriver Driver virtual table.
  */
static const IDriver_vtbl sAnalogMicDriver_vtbl =
{
  AnalogMicDriver_vtblInit,
  AnalogMicDriver_vtblStart,
  AnalogMicDriver_vtblStop,
  AnalogMicDriver_vtblDoEnterPowerMode,
  AnalogMicDriver_vtblReset
};

/* Private member function declaration */
/***************************************/

/* Public API definition */
/*************************/

/* IDriver virtual functions definition */
/****************************************/

IDriver *AnalogMicDriverAlloc(void)
{
  IDriver *p_new_obj = (IDriver *) SysAlloc(sizeof(AnalogMicDriver_t));

  if (p_new_obj == NULL)
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("AnalogMicDriver - alloc failed.\r\n"));
  }
  else
  {
    p_new_obj->vptr = &sAnalogMicDriver_vtbl;
  }

  return p_new_obj;
}

sys_error_code_t AnalogMicDriver_vtblInit(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  assert_param(p_params != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  AnalogMicDriver_t *p_obj = (AnalogMicDriver_t *) _this;
  AnalogMicDriverParams_t *p_init_params = (AnalogMicDriverParams_t *) p_params;

  /* initialize the base class */
  MDFDriverParams_t mdf_init_param =
  {
    .p_mx_mdf_cfg = p_init_params->p_mx_mdf_cfg
  };
  res = MDFDriver_vtblInit(_this, &mdf_init_param);

  if (!SYS_IS_ERROR_CODE(res))
  {
    /* initialize the ADC */
    p_obj->p_mx_adc_cfg = p_init_params->p_mx_adc_cfg;
    p_obj->p_mx_adc_cfg->p_mx_init_f();

    /* Start the calibration */
    if (HAL_ADCEx_Calibration_Start(p_obj->p_mx_adc_cfg->p_adc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("AnalogMicDriver - ADC init failed.\r\n"));
      res = SYS_BASE_LOW_LEVEL_ERROR_CODE;
    }
  }

  return res;
}

sys_error_code_t AnalogMicDriver_vtblStart(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  AnalogMicDriver_t *p_obj = (AnalogMicDriver_t *) _this;

  /* first start the ADC... */
  if (HAL_OK == HAL_ADC_Start(p_obj->p_mx_adc_cfg->p_adc))
  {
    /* then call the base method */
    res = MDFDriver_vtblStart(_this);
  }
  else
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("AnalogMicDriver - start failed.\r\n"));
    res = SYS_UNDEFINED_ERROR_CODE;
  }

  return res;
}

sys_error_code_t AnalogMicDriver_vtblStop(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  AnalogMicDriver_t *p_obj = (AnalogMicDriver_t *) _this;

  /* first stop the ADC... */
  if (HAL_OK != HAL_ADC_Stop(p_obj->p_mx_adc_cfg->p_adc))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("AnalogMicDriver - stop failed.\r\n"));
    res = SYS_LL_UNDEFINED_ERROR_CODE;
  }
  else
  {
    /* ... then stop the base class */
    res = MDFDriver_vtblStop(_this);
  }

  return res;
}

sys_error_code_t AnalogMicDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode active_power_mode,
                                                      const EPowerMode new_power_mode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  AnalogMicDriver_t *p_obj = (AnalogMicDriver_t *) _this;

  /* first call the base class implementation */
  res = MDFDriver_vtblDoEnterPowerMode(_this, active_power_mode, new_power_mode);
  if (!SYS_IS_ERROR_CODE(res))
  {
    if (new_power_mode == E_POWER_MODE_STATE1)
    {
      if (active_power_mode == E_POWER_MODE_SENSORS_ACTIVE)
      {
        HAL_ADC_Stop(p_obj->p_mx_adc_cfg->p_adc); //TODO: STF - shall I implement this in the start/stop ??
      }
    }
  }

  return res;
}

sys_error_code_t AnalogMicDriver_vtblReset(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  /*  AnalogMicDriver_t *p_obj = (AnalogMicDriver_t*)_this; */

  return res;
}

/* Private function definition */
/*******************************/
