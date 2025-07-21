/**
  ******************************************************************************
  * @file    BCDriver.c
  * @author  STMicroelectronics
  * @version 2.0.0
  * @date    July 25, 2022
  *
  * @brief  BCAdcDriver implementation.
  *
  * This file implements the Battery Charger ADC driver to read battery voltage.
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

#include "drivers/BCAdcDriver.h"
#include "drivers/BCAdcDriver_vtbl.h"
#include "services/sysmem.h"
#include "services/sysdebug.h"

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)

#define ADC_VREF                        2750                    // ADC reference voltage
#define ADC_CONV_TIMEOUT                1000                    // Timeout value in millisecond for injected conversion

/** We use a divider to scale the battery voltage according TO ADC reference voltage.
  *  For example, if battery voltage is equal to 4.2V, the ADC see 2.5V
  */

#define ADC_BATMS_RUP                   56000.0                 // divider upper resistor
#define ADC_BATMS_RDOWN                 100000.0                // divider lower resistor
#define ADC_BATMS_RATIO                 (float_t)((ADC_BATMS_RUP+ADC_BATMS_RDOWN)/ADC_BATMS_RDOWN)


/**
  * BCDriver Driver virtual table.
  */
static const IDriver_vtbl sBCAdcDriver_vtbl =
{
  BCAdcDriver_vtblInit,
  BCAdcDriver_vtblStart,
  BCAdcDriver_vtblStop,
  BCAdcDriver_vtblDoEnterPowerMode,
  BCAdcDriver_vtblReset
};

/* Public API definition */
/*************************/

IDriver *BCAdcDriverAlloc(void)
{
  IDriver *p_new_obj = (IDriver *)SysAlloc(sizeof(BCAdcDriver_t));

  if (p_new_obj == NULL)
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("BCDriver - alloc failed.\r\n"));
  }
  else
  {
    p_new_obj->vptr = &sBCAdcDriver_vtbl;
  }

  return p_new_obj;
}


/* IDriver virtual functions definition */
/****************************************/

sys_error_code_t BCAdcDriver_vtblInit(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  assert_param(p_params != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  BCAdcDriver_t *p_obj = (BCAdcDriver_t *)_this;
  p_obj->p_mx_adc_cfg = (MX_ADCParams_t *)p_params;
  /* Initialize the adc */
  p_obj->p_mx_adc_cfg->p_mx_init_f();

  return res;
}

sys_error_code_t BCAdcDriver_vtblStart(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  BCAdcDriver_t *p_obj = (BCAdcDriver_t *)_this;
  ADC_HandleTypeDef *p_adc = p_obj->p_mx_adc_cfg->p_adc;

  if (HAL_ADC_Start(p_adc) != HAL_OK)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t BCAdcDriver_vtblStop(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  BCAdcDriver_t *p_obj = (BCAdcDriver_t *)_this;
  ADC_HandleTypeDef *p_adc = p_obj->p_mx_adc_cfg->p_adc;

  if (HAL_ADC_Stop(p_adc) != HAL_OK)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t BCAdcDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode active_power_mode,
                                                  const EPowerMode new_power_mode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t BCAdcDriver_vtblReset(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t BCAdcDriver_GetValue(IDriver *_this, uint16_t *p_value)
{
  assert_param(_this != NULL);
  assert_param(p_value != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  BCAdcDriver_t *p_obj = (BCAdcDriver_t *) _this;
  ADC_HandleTypeDef *p_adc = p_obj->p_mx_adc_cfg->p_adc;
  uint32_t measured_voltage;

  if (HAL_ADC_PollForConversion(p_adc, 10) == HAL_OK)
  {
    measured_voltage = __LL_ADC_CALC_DATA_TO_VOLTAGE(p_adc->Instance, ADC_VREF, HAL_ADC_GetValue(p_adc), LL_ADC_RESOLUTION_12B);
    *p_value = (uint16_t)((float_t)(measured_voltage) * ADC_BATMS_RATIO);
  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}


/* Private function definition */
/*******************************/
