/**
  ******************************************************************************
  * @file    BCProtocol.c
  * @author  STMicroelectronics
  * @version 2.0.0
  * @date    July 25, 2022
  *
  * @brief STBC02 Battery Charger service.
  *
  * @description
  * This file implements the needed logic to support the STBC02 Battery Charger.
  *
  * For more information look at the BCProtocol.h file.
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

#include "services/BCProtocol.h"
#include "drivers/BCTimerDriver.h"
#include "drivers/BCTimChgDriver.h"
#include "drivers/BCAdcDriver.h"
#include "services/sysmem.h"
#include "services/sysdebug.h"
#include "services/syscs.h"

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_BCP, level, message)

/* Pulse length for SWire protocol */
#define BC_STBC02_SW_START_PULSE_US   360U
#define BC_STBC02_SW_SHORT_PULSE_US   105U
#define BC_STBC02_SW_STOP_PULSE_US    505U

/* This is the frequency of the Timer used to detect CHG freq [Hz] */
#define BC_CHG_TIM_COUNTING_FREQ      10000U


/**
  * This is a why to link a HAL TIM callback to the protocol object that is using the timer.
  */
typedef struct _TimCallbakcMapEntry_t
{
//  TIM_TypeDef *tim_instance;
  BCProtocol_t *p_owner;
} TimCallbackMapEntry_t;

static TimCallbackMapEntry_t sTimCallbackMap[1U] =
{
  {NULL}
};

/**
  * @brief Device state structure info
  */
typedef struct
{
  uint8_t id;
  char *name;
  float freq;
} _STBC02_ChgStateInfo_t;

/**
  * @brief STBC02 status name and related toggling frequency (in Hz) of the nCHG pin
  * @note  The sequence must respect the order of ESTBC02_STATE_t
  */
static const _STBC02_ChgStateInfo_t sSTBC02_ChgStateInfo[] =
{
  {NOT_VALID_INPUT, "Not Valid Input", 0.0},
  {VALID_INPUT, "Valid Input", 0.0},
  {VBAT_LOW, "Vbat Low", 0.0},
  {END_OF_CHARGE, "End Of Charge", 4.1},
  {CHARGING_PHASE, "Charging Phase", 6.2},
  {OVERCHARGE_FAULT, "Overcharge Fault", 8.2},
  {CHARGING_TIMEOUT, "Charging Timeout", 10.2},
  {BATTERY_VOLTAGE_BE_LOW_VPRE, "Battery Voltage Below Vpre", 12.8},
  {CHARGING_THERMAL_LIMITATION, "Charging Thermal Limitation", 14.2},
  {BATTERY_TEMPERATURE_FAULT, "Battery Temperature Fault", 16.2},
};


/* Private member function declaration */
/***************************************/

/**
  * TIM Elapsed Callback function.
  * State Machine for Single Wire protocol implementation (STBC02)
  *
  * \code
  *
  *   Start  ...... N Pulses .......    Stop
  *    ____    __    __    __    __    ______
  *   |    |  |  |  |  |  |  |  |  |  |      |
  *   |    |  |  |  |  |  |  |  |  |  |      |
  * __|    |__|  |__|  |__|  |__|  |__|      |__
  *
  * \endcode
  *
  * @param htim [IN] specifies the hardware timer that has triggered the ISR.
  */
static void BCP1WStateMachine(TIM_HandleTypeDef *htim);

/**
  * TIM Capture Callback function.
  *
  * @param htim [IN] specifies the hardware timer that has triggered the ISR.
  */
static void BCPComputeFrequency(TIM_HandleTypeDef *htim);

/**
  * Compute the Status from the frequency.
  *
  * @param freq [IN] CHG pin toggling frequency.
  */
static void BCPFreq2Status(float freq);

/* Public API definition */
/*************************/

BCProtocol_t *BCPAlloc(void)
{
  BCProtocol_t *p_new_obj = (BCProtocol_t *)SysAlloc(sizeof(BCProtocol_t));

  if (p_new_obj == NULL)
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("BCProtocol - alloc failed.\r\n"));
  }

  return p_new_obj;
}

sys_error_code_t BCPInit(BCProtocol_t *_this, IDriver *p_bc_tim_sw_driver, IDriver *p_bc_tim_chg_driver,
                         IDriver *p_bc_adc_driver)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if ((p_bc_tim_sw_driver == NULL) || (p_bc_adc_driver == NULL) || (p_bc_tim_chg_driver == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    _this->p_bc_tim_sw_driver = p_bc_tim_sw_driver;
    _this->p_bc_tim_chg_driver = p_bc_tim_chg_driver;
    _this->p_bc_adc_driver = p_bc_adc_driver;
    _this->sw_state = E_BC_IDLE;
    _this->pulse = 0;
    _this->ic_flag = 0;
    _this->chg_irq_counter = 0;
    _this->chg_irq_counter_prev = 0;

    BCTimerDriverRegisterElapsedCallback((BCTimerDriver_t *)_this->p_bc_tim_sw_driver, BCP1WStateMachine);

    BCTimChgDriverRegisterCaptureCallback((BCTimChgDriver_t *)_this->p_bc_tim_chg_driver, BCPComputeFrequency);

    /* for the moment I assume there is only one instance of this protocol in the system*/
    if (sTimCallbackMap[0].p_owner == NULL)
    {
      sTimCallbackMap[0].p_owner = _this;
    }

    /* Enable auto-recharge function */
    BCPSendCmd(_this, E_AUTORECH_ON);

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("BCP: initialization done.\r\n"));
  }

  return res;
}

sys_error_code_t BCPSendCmd(BCProtocol_t *_this, ESTBC02_SW_CMD_t cmd)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint32_t start_tick = HAL_GetTick();
  bool timeout = false;
  BCTimerDriver_t *p_driver = NULL;
  SYS_DECLARE_CS(cs);

  _this->cmd = cmd;
  _this->sw_state = E_BC_START;

  p_driver = (BCTimerDriver_t *) _this->p_bc_tim_sw_driver;
  BCDriverSetTimARR(p_driver, BC_STBC02_SW_START_PULSE_US - 1U);

  /* The timer should start immediately after the pin is SET --> Critical Section */
  SYS_ENTER_CRITICAL(cs);

  /* SW Pin set */
  BCDriverSetLine1W(p_driver);

  /* Start the driver for Single Wire communication */
  IDrvStart(_this->p_bc_tim_sw_driver);

  /* Clear the first irq flag which is set immediately */
  __HAL_TIM_CLEAR_FLAG(p_driver->mx_handle.p_mx_tim_cfg->p_tim, TIM_FLAG_UPDATE);

  SYS_EXIT_CRITICAL(cs);


  while ((_this->sw_state != E_BC_IDLE) && (!timeout))
  {
    /* 10ms timeout */
    if (HAL_GetTick() - start_tick > 10U)
    {
      timeout = true;
      res = SYS_BCP_CMD_EXECUTION_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_BCP_CMD_EXECUTION_ERROR_CODE);
    }
  }

  /* stop the time base*/
  IDrvStop(_this->p_bc_tim_sw_driver);

  return res;
}

sys_error_code_t BCPPowerOff(BCProtocol_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("BCP: power-off cmd.\r\n"));

  res = BCPSendCmd(_this, E_SHIPPING_MODE_ON);

  return res;
}

sys_error_code_t BCPAcquireBatteryVoltage(BCProtocol_t *_this, uint16_t *voltage)
{

  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* start adc */
  IDrvStart(_this->p_bc_adc_driver);

  /* do a measure */
  BCAdcDriver_GetValue(_this->p_bc_adc_driver, voltage);

  return res;
}

sys_error_code_t BCPAcquireState(BCProtocol_t *_this, uint16_t *voltage, ESTBC02_STATE_t *state)
{
  assert_param(_this != NULL);
  assert_param(voltage != NULL);
  assert_param(state != NULL);

  sys_error_code_t res = SYS_NO_ERROR_CODE;
  BCProtocol_t *p_owner = sTimCallbackMap[0].p_owner;

  /* Enable Li-Ion battery measurement */
  BCPSendCmd(_this, E_BATMS_ON);

  /* Get Battery Voltage */
  BCPAcquireBatteryVoltage(_this, voltage);

  /* Enable Li-Ion battery measurement */
  BCPSendCmd(_this, E_BATMS_OFF);

  if (p_owner->chg_irq_counter == p_owner->chg_irq_counter_prev)
  {
    /* CHG pin not toggling */
    *state = DISCHARGING;
  }
  else if (*voltage < 100U)
  {
    /* Battery not connected */
    *voltage = 0U;
    *state = BATTERY_NOT_CONNECTED;
  }
  else
  {
    /* All other cases */
    switch (_this->chg_state)
    {
      case NOT_VALID_INPUT:
        *state = DISCHARGING;
        break;
      case VALID_INPUT:
        *state = PLUGGED_NOT_CHARGING;
        break;
      case VBAT_LOW:
        *state = LOW_BATTERY;
        break;
      case END_OF_CHARGE:
        *state = PLUGGED_NOT_CHARGING;
        break;
      case CHARGING_PHASE:
        *state = CHARGING;
        break;
      case OVERCHARGE_FAULT:
        *state = STATE_ERROR;
        break;
      case CHARGING_TIMEOUT:
        *state = STATE_ERROR;
        HAL_GPIO_WritePin(C_EN_GPIO_Port, C_EN_Pin, GPIO_PIN_RESET);
        break;
      case BATTERY_VOLTAGE_BE_LOW_VPRE:
        *state = STATE_ERROR;
        HAL_GPIO_WritePin(C_EN_GPIO_Port, C_EN_Pin, GPIO_PIN_RESET);
        break;
      case CHARGING_THERMAL_LIMITATION:
        *state = STATE_ERROR;
        break;
      case BATTERY_TEMPERATURE_FAULT:
        *state = UNKNOWN;
        break;
      default:
        *state = UNKNOWN;
        break;
    }
    p_owner->chg_irq_counter_prev = p_owner->chg_irq_counter;
  }

  return res;
}


/* Private API definition */
/**************************/
static void BCP1WStateMachine(TIM_HandleTypeDef *htim)
{
  assert_param(htim != NULL);

  BCProtocol_t *p_owner = sTimCallbackMap[0].p_owner;
  BCTimerDriver_t *p_driver = (BCTimerDriver_t *) p_owner->p_bc_tim_sw_driver;

  switch (p_owner->sw_state)
  {
    case E_BC_IDLE:
      break;
    case E_BC_START: /* End of the start pulse */
      p_owner->pulse = 0;
      BCDriverResetLine1W(p_driver);
      BCDriverSetTimARR(p_driver, BC_STBC02_SW_SHORT_PULSE_US - 1U);
      p_owner->sw_state = E_BC_PULSE_H;
      break;
    case E_BC_PULSE_L: /* Beginning of short-low pulse (except first one) */
      BCDriverSetTimARR(p_driver, BC_STBC02_SW_SHORT_PULSE_US - 1U);
      BCDriverResetLine1W(p_driver);
      if (p_owner->pulse < (uint8_t) p_owner->cmd)
      {
        p_owner->sw_state = E_BC_PULSE_H;
      }
      else
      {
        p_owner->sw_state = E_BC_STOP_H;
      }
      break;
    case E_BC_PULSE_H: /* Beginning of short-high pulse */
      BCDriverSetTimARR(p_driver, BC_STBC02_SW_SHORT_PULSE_US - 1U);
      BCDriverSetLine1W(p_driver);
      p_owner->pulse++;
      p_owner->sw_state = E_BC_PULSE_L;
      break;
    case E_BC_STOP_H:
      BCDriverSetTimARR(p_driver, BC_STBC02_SW_STOP_PULSE_US - 1U);
      BCDriverSetLine1W(p_driver);
      p_owner->sw_state = E_BC_STOP_L;
      break;
    case E_BC_STOP_L:
      BCDriverResetLine1W(p_driver);
      IDrvStop(p_owner->p_bc_tim_sw_driver);
      p_owner->sw_state = E_BC_IDLE;
      break;
    default:
      break;
  }
}


static void BCPComputeFrequency(TIM_HandleTypeDef *htim)
{
  assert_param(htim != NULL);

  BCProtocol_t *p_owner = sTimCallbackMap[0].p_owner;
  float uwFrequency = 0; /* Frequency Value [Hz] */

  p_owner->chg_irq_counter++;
  if (p_owner->ic_flag == 0)
  {
    /* Get the 1st Input Capture value */
    p_owner->ic_value1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
    p_owner->ic_flag = 1;
  }
  else if (p_owner->ic_flag == 1)
  {
    uint32_t uwDiffCapture;

    /* Get the 2nd Input Capture value */
    p_owner->ic_value2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

    /* Capture computation */
    if (p_owner->ic_value2 > p_owner->ic_value1)
    {
      uwDiffCapture = (p_owner->ic_value2 - p_owner->ic_value1);
    }
    else if (p_owner->ic_value2 < p_owner->ic_value1)
    {
      /* 0xFFFF is max TIM1_CCRx value */
      uwDiffCapture = ((0xFFFFU - p_owner->ic_value1) + p_owner->ic_value2) + 1U;
    }
    else
    {
      /* IC values are the same: it should never happen */
      uwDiffCapture = 0xFFFFFFFFU;
    }

    /* Frequency computation */
    uwFrequency = (float) BC_CHG_TIM_COUNTING_FREQ / (float) uwDiffCapture;
    p_owner->ic_flag = 0;

    BCPFreq2Status(uwFrequency);
  }
}


/**
  * @brief Get the status of the STBC02 checking the toggling frequency of the charging pin of the STBC02
  * @retval None
  */
static void BCPFreq2Status(float freq)
{
  BCProtocol_t *p_owner = sTimCallbackMap[0].p_owner;
  ESTBC02_CHG_STATE_t ChgState;
  float half_step;

  if (freq > (float) 0)
  {
    for (ChgState = END_OF_CHARGE; ChgState < BATTERY_TEMPERATURE_FAULT; ChgState++)
    {
      half_step = (sSTBC02_ChgStateInfo[ChgState].freq + sSTBC02_ChgStateInfo[(uint8_t) ChgState + 1U].freq) / (float) 2;
      if (freq < half_step)
      {
        p_owner->chg_state = ChgState;
        break;
      }
    }
    half_step = (sSTBC02_ChgStateInfo[ChgState - 1U].freq + sSTBC02_ChgStateInfo[(uint8_t) ChgState].freq) / (float) 2;
    if (freq > half_step)
    {
      p_owner->chg_state = ChgState;
    }
  }
}

