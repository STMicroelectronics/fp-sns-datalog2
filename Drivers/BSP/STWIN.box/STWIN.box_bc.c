/**
 ******************************************************************************
 * @file    STWIN.box_bc.c
 * @author  SRA
 * @brief   This file provides code for the configuration of the STBC02
 *          battery charger
 *
 * This file implements the needed logic to support the STBC02 Battery Charger.
 *
 * Single Wire protocol is implemented using 1 Timer in free running mode
 * That generates an interrupt each time the counter met the ARR value
 * The ARR value is directly updated i the ISR function to so that, in output
 *
 * SWire protocol example:
 * \code
 *
 *   Start  ....   N pulses (Short)  ....   Stop
 *    ____    __    __         __    __    ______
 *   |    |  |  |  |  |       |  |  |  |  |      |
 *   |    |  |  |  |  |       |  |  |  |  |      |
 * __|    |__|  |__|  |_ ... _|  |__|  |__|      |__
 *
 *  Start: 360 us
 *  Short: 105 us
 *  Stop:  505 us
 *
 * \endcode
 *
 * Battery charger state through CHG pin frequency detection
 * The CHG provides status information about VIN voltage level, battery charging
 * status and faults by toggling at different frequencies.
 * The frequency is detected thanks to a Timer set in Input Capture mode.
 *
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
#include "STWIN.box_bc.h"

/** @addtogroup BSP
 * @{
 */

/** @addtogroup STWIN 
 * @{
 */

/** @addtogroup STWIN_BOX_BATTERY_CHARGER 
 * @{
 */

/** @addtogroup STWIN_BOX_BATTERY_CHARGER_Private_Constants
 * @{
 */

#ifndef STWIN_BOX_VDDA_MV
#define STWIN_BOX_VDDA_MV                       (2750UL)
#endif

#define BC_SW_TIM_COUNTING_FREQ    1000000U
#define BC_CHG_TIM_COUNTING_FREQ   10000U

#define STBC02_SW_START_PULSE_US   360U
#define STBC02_SW_SHORT_PULSE_US   105U
#define STBC02_SW_STOP_PULSE_US    500U

/**
 * @brief STBC02 status name and related toggling frequency (in Hz) of the nCHG pin
 * @note  The sequence must respect the order of stbc02_ChgState_TypeDef
 */
static const stbc02_ChgStateNameAndFreq_TypeDef stbc02_ChgStateNameAndFreq[10] =
{
    {
        "Not Valid Input",
        0.0 },
    {
        "Valid Input",
        0.0 },
    {
        "Vbat Low",
        0.0 },
    {
        "End Of Charge",
        4.1 },
    {
        "Charging Phase",
        6.2 },
    {
        "Overcharge Fault",
        8.2 },
    {
        "Charging Timeout",
        10.2 },
    {
        "Battery Voltage Below Vpre",
        12.8 },
    {
        "Charging Thermal Limitation",
        14.2 },
    {
        "Battery Temperature Fault",
        16.2 }, };

/**
 * @}
 */

/** @addtogroup STWIN_BOX_BATTERY_CHARGER_Private_Variables
 * @{
 */

static stbc02_SwCmd_TypeDef stbc02_SwCmdSel;                    //!< The selected STBC02 SW Cmd
static volatile stbc02_SwState_TypeDef stbc02_SwState = idle;   //!< Status during STBC02 SW Cmd sending
static stbc02_ChgState_TypeDef stbc02_ChgState = NotValidInput; //!< Status of STBC02
static uint32_t stbc02_ChgPinToggledTime = 0;                   //!< SysTick value when STBC02 nCHG pin is toggling
static TIM_HandleTypeDef BC_Sw_TIM_Handle;                      //!< Timer for Single Wire communication
static TIM_HandleTypeDef BC_Chg_TIM_Handle;                     //!< Timer used to read BC status

/* Captured Values */
static uint32_t uwIC2Value1 = 0;
static uint32_t uwIC2Value2 = 0;
static uint32_t uwDiffCapture = 0;
static uint16_t uhCaptureIndex = 0; /* Capture index */
static float_t uwFrequency = 0; /* Frequency Value [Hz] */

static ADC_HandleTypeDef ADC4_Handle;
/**
 * @}
 */

/* Private Function_Prototypes -----------------------------------------------*/
void BC_ChgPinFreqGet(void);
void BC_Chg_Freq2Status(void);
void BC_Chg_TIM_CaptureCallback(TIM_HandleTypeDef *htim);
int32_t BC_Chg_TIM_Init(void);

void BC_Sw_IO_Init(void);
void BC_Sw_Cmd_Handler(void);
void BC_Sw_TIM_IRQHandler(void);
void BC_Sw_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
int32_t BC_Sw_TIM_Init(void);

void BC_BatMs_ADC_MspInit(ADC_HandleTypeDef* adcHandle);
void BC_BatMs_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle);
static int32_t BC_BatMs_ADC_Channel_Init(void);

void BC_Chg_TIM_MspInit(TIM_HandleTypeDef *htim);
void BC_Chg_TIM_MspDeInit(TIM_HandleTypeDef *htim);

/** @addtogroup STWIN_BOX_BATTERY_CHARGER_Public_Functions
 * @{
 */

/**
 * @brief Initialize the STBC02 Single Wire Communication
 * @retval None
 */
void BSP_BC_Sw_Init(void)
{
  (void)BC_Sw_TIM_Init();
  /* Init the related GPIOs */
  BC_Sw_IO_Init();
}

/**
 * @brief Send a single wire command to the STBC02
 * @param stbc02_SwCmd The command to be sent
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
int32_t BSP_BC_Sw_CmdSend(stbc02_SwCmd_TypeDef stbc02_SwCmd)
{
  uint32_t tk = STBC02_GetTick();

  stbc02_SwCmdSel = stbc02_SwCmd;
  stbc02_SwState = start;
  BC_Sw_TIM_Handle.Instance->ARR = STBC02_SW_START_PULSE_US-1U;

  /* The timer should start immediately after the pin is SET --> Critical Section */
  __disable_irq();

  /* SW Pin set */
  STBC02_SW_GPIO_PORT->BSRR = STBC02_SW_PIN;

  /* Start the time base */
  (void)HAL_TIM_Base_Start_IT(&BC_Sw_TIM_Handle);

  /* Clear the first irq flag which is set immediately */
  __HAL_TIM_CLEAR_FLAG(&BC_Sw_TIM_Handle, TIM_FLAG_UPDATE);

  __enable_irq();

  while(stbc02_SwState != idle)
  {
    if((STBC02_GetTick() - tk) > 10U)
    {
      return 1;
    }
  }

  return 0;
}

/**
 * @brief Initialize the STBC02 Charge Pin frequency detection
 * @retval None
 */
int32_t BSP_BC_Chg_Init(void)
{
  return BC_Chg_TIM_Init();
}

/**
 * @brief Initialize the STBC02 Charge Pin frequency detection
 * @retval None
 */
int32_t BSP_BC_Chg_DeInit(void)
{
  if(HAL_TIM_IC_DeInit(&BC_Chg_TIM_Handle) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  return BSP_ERROR_NONE;
}

/**
 * @brief Call the function for measuring the toggling frequency of the charging pin of the STBC02
 * @note Must be called when the charging pin of the STBC02 has toggled
 * @retval None
 */
void BSP_BC_ChgPinHasToggled(TIM_HandleTypeDef *htim)
{
  stbc02_ChgPinToggledTime = STBC02_GetTick();

  if(uhCaptureIndex == 0U)
  {
    /* Get the 1st Input Capture value */
    uwIC2Value1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
    uhCaptureIndex = 1;
  }
  else if(uhCaptureIndex == 1U)
  {
    /* Get the 2nd Input Capture value */
    uwIC2Value2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

    /* Capture computation */
    if (uwIC2Value2 > uwIC2Value1)
    {
      uwDiffCapture = (uwIC2Value2 - uwIC2Value1);
    }
    else if (uwIC2Value2 < uwIC2Value1)
    {
      /* 0xFFFF is max TIM1_CCRx value */
      uwDiffCapture = ((0xFFFFU - uwIC2Value1) + uwIC2Value2) + 1U;
    }
    else
    {
      /* If capture values are equal, we have reached the limit of frequency measures */
    }

    /* Frequency computation: for this example TIMx (TIM5) is clocked by APB1Clk */
    uwFrequency = (float_t)BC_CHG_TIM_COUNTING_FREQ / (float_t)uwDiffCapture;
    uhCaptureIndex = 0;
  }
  else
  {

  }
}

/**
 * @brief Check for STBC02 nCHG toggling
 * @note Must be called on SysTick IRQ
 * @retval 0 if not toggling
 * @retval 1 if toggling
 */
int32_t BSP_BC_IsChgPinToggling(void)
{
  GPIO_PinState stbc02_ChgPinState;

  if((STBC02_GetTick() - stbc02_ChgPinToggledTime) > 500U)
  {
    stbc02_ChgPinState = HAL_GPIO_ReadPin(STBC02_CHG_GPIO_PORT, STBC02_CHG_PIN);
    if(stbc02_ChgPinState == GPIO_PIN_RESET)
    {
      stbc02_ChgState = NotValidInput;
    }
    else
    {
      stbc02_ChgState = ValidInput;
    }
    return 0;
  }
  return 1;
}

/**
 * @brief  This method initializes the peripherals used to get the current voltage of battery
 * @param  None
 * @retval BSP_ERROR_NONE in case of success
 * @retval BSP_ERROR_COMPONENT_FAILURE in case of failures
 */
int32_t BSP_BC_BatMs_Init(void)
{
  /* Enable ADC4 */
  (void) BC_BatMs_ADC_Initialization();

  /* Configure Channel for Voltage ADC convertion */
  (void)BC_BatMs_ADC_Channel_Init();

  /* Run the ADC calibration in single-ended mode */
  if (HAL_ADCEx_Calibration_Start(&ADC4_Handle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
  {
    /* Calibration Error */
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  This method reset the peripherals used to get the current voltage of battery
 * @param  None
 * @retval BSP_ERROR_NONE in case of success
 * @retval BSP_ERROR_COMPONENT_FAILURE in case of failures
 */
int32_t BSP_BC_BatMs_DeInit(void)
{
  int32_t retValue;

  retValue = BC_BatMs_ADC_DeInitialization();

  if(retValue != BSP_ERROR_NONE)
  {
    return retValue;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Get the status of the STBC02
 * @param  pstbc02_ChgState To be filled with the new value
 * @retval None
 */
void BSP_BC_GetState(stbc02_State_TypeDef *BC_State)
{
  /* Check for STBC02 nCHG toggling */
  if(BSP_BC_IsChgPinToggling() == 1)
  {
    /* Get the status of the STBC02 */
    BC_Chg_Freq2Status();
  }

  BC_State->Id = stbc02_ChgState;
  (void) strncpy((char*) BC_State->Name, (char*) stbc02_ChgStateNameAndFreq[stbc02_ChgState].name, 32);
}

/**
 * @brief  This method gets the current voltage of battery
 * @param  mV pointer to destination variable
 * @retval BSP_ERROR_NONE in case of success
 * @retval BSP_ERROR_COMPONENT_FAILURE in case of failures
 */
int32_t BSP_BC_GetVoltage(uint32_t *BatteryVoltage)
{
  uint32_t adc_value;
  uint32_t adc_voltage;

  if(HAL_ADC_Start(&ADC4_Handle) != HAL_OK)
  {
    /* Start Conversion Error */
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  if(HAL_ADC_PollForConversion(&ADC4_Handle, 10) != HAL_OK)
  {
    /* End Of Conversion flag not set on time */
    return BSP_ERROR_CLOCK_FAILURE;
  }
  else
  {
    /*##-5- Get the converted value of regular channel  ########################*/
    adc_value = HAL_ADC_GetValue(&ADC4_Handle);
  }

  if(HAL_ADC_Stop(&ADC4_Handle) != HAL_OK)
  {
    /* Start Conversation Error */
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  // [0-2.7V]
  adc_voltage = __LL_ADC_CALC_DATA_TO_VOLTAGE(ADC4, STWIN_BOX_VDDA_MV, adc_value, LL_ADC_RESOLUTION_12B);

  // [0-4.2V]
  *BatteryVoltage = ((56U + 100U) * adc_voltage) / 100U;

  return BSP_ERROR_NONE;
}

/**
 * @brief  This method gets the current voltage of battery and one estimation of Battery % Level
 * @param  uint32_t *Volt battery Voltage Value
 * @param  uint32_t *BatteryLevel Battery % Level
 * @retval BSP status
 */
int32_t BSP_BC_GetVoltageAndLevel(uint32_t *BatteryVoltage, uint32_t *BatteryLevel)
{
  uint32_t Voltage = 0;
  static int32_t VoltageWindowInit = 0;
  static uint32_t WindowVoltage[WINDOW_VOLTAGE_DIM];
  static int32_t WindowPostion = 0;

  (void) BSP_BC_GetVoltage(&Voltage);

  /* We Filter the Voltage for understanding the Battery % Level */

  /* Insert the new Value */
  WindowVoltage[WindowPostion] = Voltage;
  WindowPostion++;

  /* Control if we have reached the end */
  if(WindowPostion == WINDOW_VOLTAGE_DIM)
  {
    if(VoltageWindowInit == 0)
    {
      VoltageWindowInit = 1;
    }
    WindowPostion = 0;
  }

  /* Make the mean of latest voltage values */
  if(VoltageWindowInit == 1)
  {
    int32_t Counter;
    Voltage = 0;
    for(Counter = 0; Counter < WINDOW_VOLTAGE_DIM; Counter++)
    {
      Voltage += WindowVoltage[Counter];
    }
    Voltage >>= 4;
  }

  /* Limits check */
  if(Voltage > (uint32_t) BC_BATTERY_MAX_VOLTAGE)
  {
    Voltage = BC_BATTERY_MAX_VOLTAGE;
  }
  if(Voltage < (uint32_t) BC_BATTERY_MIN_VOLTAGE)
  {
    Voltage = BC_BATTERY_MIN_VOLTAGE;
  }

  *BatteryLevel = (((Voltage - (uint32_t) BC_BATTERY_MIN_VOLTAGE) * 100U) / (uint32_t) (BC_BATTERY_MAX_VOLTAGE - BC_BATTERY_MIN_VOLTAGE));
  *BatteryVoltage = Voltage;

  return BSP_ERROR_NONE;
}

/**
 * @brief  This method initializes the ADC peripheral used for Analog Mic and Battery Voltage Conversion
 * @param  ADC_InitFor who wants to Init the ADC: Audio or Battery Charger
 * @retval BSP_ERROR_NONE in case of success
 * @retval BSP_ERROR_PERIPH_FAILURE in case of failures
 */
int32_t BC_BatMs_ADC_Initialization(void)
{
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableVddA();

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
  HAL_ADC_RegisterCallback(&ADC4_Handle, HAL_ADC_MSPINIT_CB_ID, BC_BatMs_ADC_MspInit);
  HAL_ADC_RegisterCallback(&ADC4_Handle, HAL_ADC_MSPDEINIT_CB_ID, BC_BatMs_ADC_MspDeInit);
#endif

  ADC4_Handle.Instance = ADC4;
  ADC4_Handle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  ADC4_Handle.Init.Resolution = ADC_RESOLUTION_12B;
  ADC4_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  ADC4_Handle.Init.ScanConvMode = ADC4_SCAN_DISABLE;
  ADC4_Handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  ADC4_Handle.Init.LowPowerAutoPowerOff = ADC_LOW_POWER_NONE;
  ADC4_Handle.Init.LowPowerAutoWait = DISABLE;
  ADC4_Handle.Init.ContinuousConvMode = DISABLE;
  ADC4_Handle.Init.NbrOfConversion = 1;
  ADC4_Handle.Init.DiscontinuousConvMode = DISABLE;
  ADC4_Handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  ADC4_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  ADC4_Handle.Init.DMAContinuousRequests = DISABLE;
  ADC4_Handle.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_LOW;
  ADC4_Handle.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  ADC4_Handle.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  ADC4_Handle.Init.SamplingTimeCommon1 = ADC4_SAMPLETIME_79CYCLES_5;
  ADC4_Handle.Init.SamplingTimeCommon2 = ADC4_SAMPLETIME_79CYCLES_5;
  if(HAL_ADC_Init(&ADC4_Handle) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Initialize the ADC MSP.
 * @param  hDfsdmFilter ADC handle
 * @retval None
 */
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
void BC_BatMs_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
#else
void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
#endif
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(adcHandle->Instance==ADC4)
  {

    /* Initializes the peripherals clock  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADCDAC;
    PeriphClkInit.AdcDacClockSelection = RCC_ADCDACCLKSOURCE_HSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      for(;;){}   /* Blocking error */
    }

    /* Enable LDO: switch on analog power supply
     * Without LDO, ADC is switched off */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_EnableVddA();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

    /* Configure the DCDC2 Enable pin */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* ADC4 clock enable */
    __HAL_RCC_ADC4_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**ADC4 GPIO Configuration
    PC2     ------> ADC4_IN3 */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  }
}

/**
 * @brief  DeInitialize the ADC MSP.
 * @param  hDfsdmFilter ADC handle
 * @retval None
 */
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
void BC_BatMs_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
#else
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
#endif
{
  if(adcHandle->Instance==ADC4)
  {
    /* Peripheral clock disable */
    __HAL_RCC_ADC4_CLK_DISABLE();

    /**ADC4 GPIO Configuration
    PC2     ------> ADC4_IN3 */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2);
  }
}

/**
 * @brief  This method De initializes the ADC peripheral used for Analog Mic and Battery Voltage Conversion
 * @param  ADC_InitFor who wants to Init the ADC: Audio or Battery Charger
 * @retval BSP_ERROR_NONE in case of success
 * @retval BSP_ERROR_PERIPH_FAILURE in case of failures
 */
int32_t BC_BatMs_ADC_DeInitialization(void)
{
  if(HAL_ADC_DeInit(&ADC4_Handle) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @}
 */

/** @addtogroup STWIN_BOX_BATTERY_CHARGER_Private_Functions
 * @{
 */


/**
 * @brief  Initializes the Single Wire GPIO used for the Li-Ion Battery Charger
 * @retval None
 */
void BC_Sw_IO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  STBC02_SW_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin = STBC02_SW_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_WritePin(STBC02_SW_GPIO_PORT, STBC02_SW_PIN, GPIO_PIN_RESET);
  HAL_GPIO_Init(STBC02_SW_GPIO_PORT, &GPIO_InitStruct);
}


/**
 * @brief Single wire command manager for the STBC02
 * @note This function have to be called every 5 us
 * @retval None
 */
void BC_Sw_Cmd_Handler(void)
{
  static uint8_t CmdPulse = 0;                              //! Cmd pulse number

  switch(stbc02_SwState)
  {
    case idle:
      break;
    case start: /* End of the start pulse */
      CmdPulse = 0;
      STBC02_SW_GPIO_PORT->BRR = STBC02_SW_PIN; // RESET
      BC_Sw_TIM_Handle.Instance->ARR = STBC02_SW_SHORT_PULSE_US-1U;
      stbc02_SwState = pulse_h;
      break;
    case pulse_l: /* Beginning of short-low pulse (except first one) */
      BC_Sw_TIM_Handle.Instance->ARR = STBC02_SW_SHORT_PULSE_US-1U;
      STBC02_SW_GPIO_PORT->BRR = STBC02_SW_PIN; // RESET
      if(CmdPulse < (uint8_t) stbc02_SwCmdSel)
      {
        stbc02_SwState = pulse_h;
      }
      else
      {
        stbc02_SwState = stop_h;
      }
      break;
    case pulse_h: /* Beginning of short-high pulse */
      BC_Sw_TIM_Handle.Instance->ARR = STBC02_SW_SHORT_PULSE_US-1U;
      STBC02_SW_GPIO_PORT->BSRR = STBC02_SW_PIN; // SET
      CmdPulse++;
      stbc02_SwState = pulse_l;
      break;
    case stop_h: /* Beginning of stop-high pulse */
      BC_Sw_TIM_Handle.Instance->ARR = STBC02_SW_STOP_PULSE_US-1U;
      STBC02_SW_GPIO_PORT->BSRR = STBC02_SW_PIN; // SET
      stbc02_SwState = stop_l;
      break;
    case stop_l: /* End of sequence */
      STBC02_SW_GPIO_PORT->BRR = STBC02_SW_PIN; // RESET
      (void) HAL_TIM_Base_Stop_IT(&BC_Sw_TIM_Handle);
      stbc02_SwState = idle;
      break;
    default:
      break;
  }
}

/**
 * @brief Get the status of the STBC02 checking the toggling frequency of the charging pin of the STBC02
 * @retval None
 */
void BC_Chg_Freq2Status(void)
{
  float_t freq;
  stbc02_ChgState_TypeDef ChgState;
  freq = uwFrequency;

  if(freq > (float_t) 0)
  {
    for(ChgState = EndOfCharge; ChgState < BatteryTemperatureFault; ChgState++)
    {
      if((freq < ((stbc02_ChgStateNameAndFreq[ChgState].freq + stbc02_ChgStateNameAndFreq[(uint8_t) ChgState + 1U].freq) / (float_t) 2)))
      {
        stbc02_ChgState = ChgState;
        break;
      }
    }
    if((freq > ((stbc02_ChgStateNameAndFreq[(uint8_t) ChgState - 1U].freq + stbc02_ChgStateNameAndFreq[ChgState].freq) / (float_t) 2)))
    {
      stbc02_ChgState = ChgState;
    }
  }
}

/**
 * @brief  Initializes the used timer
 * @retval None
 */
int32_t BC_Chg_TIM_Init(void)
{
  RCC_ClkInitTypeDef clkconfig;
  uint32_t uwTimclock, uwAPB1Prescaler;
  uint32_t uwPrescalerValue;
  uint32_t pFLatency;

  /*##-1- Configure the TIM peripheral #######################################*/
  /* TIM5 configuration: Input Capture mode ---------------------
   The external signal is connected to TIM5 CH1 (PA0)
   The Rising edge is used as active edge
   ------------------------------------------------------------ */
  /* Timer Input Capture Configuration Structure declaration */
  TIM_IC_InitTypeDef sICConfig;

  /* Set TIMx instance */
  BC_Chg_TIM_Handle.Instance = TIM5;

  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  /* Get APB1 prescaler */
  uwAPB1Prescaler = clkconfig.APB1CLKDivider;

  /* Compute TIM clock */
  if(uwAPB1Prescaler == RCC_HCLK_DIV1)
  {
    uwTimclock = HAL_RCC_GetPCLK1Freq();
  }
  else
  {
    uwTimclock = 2UL * HAL_RCC_GetPCLK1Freq();
  }

  /* Set the Tim prescaler to obtain 10kHz counting frequency (count every 0.1ms) */
  uwPrescalerValue = (uwTimclock / BC_CHG_TIM_COUNTING_FREQ) - 1U;

  /* Initialize TIMx peripheral as follows:
   * Period = 0xFFFF
   * Prescaler = 0
   * ClockDivision = 0
   * Counter direction = Up
   */
  BC_Chg_TIM_Handle.Init.Period = 0xFFFF;
  BC_Chg_TIM_Handle.Init.Prescaler = uwPrescalerValue;
  BC_Chg_TIM_Handle.Init.ClockDivision = 0;
  BC_Chg_TIM_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
  BC_Chg_TIM_Handle.Init.RepetitionCounter = 0;

#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1U)
  HAL_TIM_RegisterCallback(&BC_Chg_TIM_Handle, HAL_TIM_IC_MSPINIT_CB_ID, BC_Chg_TIM_MspInit);
  HAL_TIM_RegisterCallback(&BC_Chg_TIM_Handle, HAL_TIM_IC_MSPDEINIT_CB_ID, BC_Chg_TIM_MspDeInit);
#endif

  if(HAL_TIM_IC_Init(&BC_Chg_TIM_Handle) != HAL_OK)
  {
    return BSP_ERROR_NO_INIT;
  }

#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1U)
  HAL_TIM_RegisterCallback(&BC_Chg_TIM_Handle, HAL_TIM_IC_CAPTURE_CB_ID, BC_Chg_TIM_CaptureCallback);
#endif

  /*##-2- Configure the Input Capture channel ################################*/
  /* Configure the Input Capture of channel 3*/
  sICConfig.ICPolarity = TIM_ICPOLARITY_RISING;
  sICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sICConfig.ICPrescaler = TIM_ICPSC_DIV1;
  sICConfig.ICFilter = 0;
  if(HAL_TIM_IC_ConfigChannel(&BC_Chg_TIM_Handle, &sICConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    return BSP_ERROR_NO_INIT;
  }

  /*##-3- Start the Input Capture in interrupt mode ##########################*/
  if(HAL_TIM_IC_Start_IT(&BC_Chg_TIM_Handle, TIM_CHANNEL_1) != HAL_OK)
  {
    return BSP_ERROR_NO_INIT;
  }
  return BSP_ERROR_NONE;
}


#if (USE_HAL_TIM_REGISTER_CALLBACKS == 0U)
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
#else
void BC_Chg_TIM_CaptureCallback(TIM_HandleTypeDef *htim)
#endif
{
  if((htim->Instance == TIM5) && (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1))
  {
    BSP_BC_ChgPinHasToggled(htim);
  }
}

/**
 * @brief  This function configures the STBC02_SW_TIM to manage Single Wire communication with STBC02
 * @note
 * @param
 * @retval BSP_ERROR_NONE or BSP_ERROR_PERIPH_FAILURE
 */
int32_t BC_Sw_TIM_Init(void)
{
  RCC_ClkInitTypeDef clkconfig;
  uint32_t uwTimclock, uwAPB1Prescaler;
  uint32_t uwPrescalerValue;
  uint32_t pFLatency;
  int32_t Status = BSP_ERROR_NONE;

  /* Enable TIM clock */
  STBC02_SW_TIM_CLK_ENABLE();

  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  /* Get APB1 prescaler */
  uwAPB1Prescaler = clkconfig.APB1CLKDivider;

  /* Compute TIM clock */
  if(uwAPB1Prescaler == RCC_HCLK_DIV1)
  {
    uwTimclock = HAL_RCC_GetPCLK1Freq();
  }
  else
  {
    uwTimclock = 2UL * HAL_RCC_GetPCLK1Freq();
  }


  /* Set the Tim prescaler to obtain 1MHz counting frequency (count every 1us) */
  uwPrescalerValue = (uwTimclock / BC_SW_TIM_COUNTING_FREQ) - 1U;

  /* Initialize TIM peripheral */
  BC_Sw_TIM_Handle.Instance = STBC02_SW_TIM;
  BC_Sw_TIM_Handle.Init.Period = STBC02_SW_START_PULSE_US - 1U;
  BC_Sw_TIM_Handle.Init.Prescaler = uwPrescalerValue;
  BC_Sw_TIM_Handle.Init.ClockDivision = 0U;
  BC_Sw_TIM_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;

  /* STBC02_SW_TIM clock enable */
  STBC02_SW_TIM_CLK_ENABLE();

  /* STBC02_SW_TIM interrupt Init */
  HAL_NVIC_SetPriority(BC_SW_TIM_IRQn, BSP_BC_SW_TIM_IT_PRIORITY, 0);
  HAL_NVIC_EnableIRQ(BC_SW_TIM_IRQn);

  if(HAL_TIM_Base_Init(&BC_Sw_TIM_Handle) == HAL_OK)
  {
#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1U)
    HAL_TIM_RegisterCallback(&BC_Sw_TIM_Handle, HAL_TIM_PERIOD_ELAPSED_CB_ID, BC_Sw_TIM_PeriodElapsedCallback);
#endif
    HAL_NVIC_EnableIRQ(BC_SW_TIM_IRQn);
  }
  else
  {
    Status = BSP_ERROR_PERIPH_FAILURE;
  }

  /* Return function Status */
  return Status;
}


#if (USE_HAL_TIM_REGISTER_CALLBACKS == 0U)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
#else
void BC_Sw_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
#endif
{
  if(htim->Instance == STBC02_SW_TIM)
  {
    BC_Sw_Cmd_Handler();
  }
}


/**
 * @brief This function handles STBC02_SW_TIM global interrupt.
 */
void BC_Sw_TIM_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&BC_Sw_TIM_Handle);
}

/** @addtogroup STWIN_BOX_BATTERY_CHARGER_Interrupt_Service_Routines
 * @{
 */

/**
 * @}
 */

/** @addtogroup STWIN_BOX_BATTERY_CHARGER_MSP_Functions
 * @{
 */

/**
 * @brief TIM MSP Initialization
 *        This function configures the hardware resources used in this example:
 *           - Peripheral's clock enable
 *           - Peripheral's GPIO Configuration
 * @param htim: TIM handle pointer
 * @retval None
 */
#if (USE_HAL_TIM_REGISTER_CALLBACKS == 0U)
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
#else
void BC_Chg_TIM_MspInit(TIM_HandleTypeDef *htim)
#endif
{
  UNUSED(htim);
  GPIO_InitTypeDef GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIM5 Peripheral clock enable */
  __HAL_RCC_TIM5_CLK_ENABLE();
  /* Enable GPIO channels Clock */
  STBC02_CHG_GPIO_CLK_ENABLE();

  /* Configure  (TIMx_Channel) in Alternate function, push-pull and high speed */
  GPIO_InitStruct.Pin = STBC02_CHG_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
  HAL_GPIO_Init(STBC02_CHG_GPIO_PORT, &GPIO_InitStruct);

  /*##-2- Configure the NVIC for TIMx #########################################*/

  HAL_NVIC_SetPriority(TIM5_IRQn, 5, 0);

  /* Enable the TIMx global Interrupt */
  HAL_NVIC_EnableIRQ(TIM5_IRQn);
}


/**
 * @brief  DeInitialize Chg Pin monitoring function
 * @retval None
 */
#if (USE_HAL_TIM_REGISTER_CALLBACKS == 0U)
void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef *htim)
#else
void BC_Chg_TIM_MspDeInit(TIM_HandleTypeDef *htim)
#endif
{
  UNUSED(htim);
  HAL_NVIC_DisableIRQ(TIM5_IRQn);
  __HAL_RCC_TIM5_CLK_DISABLE();
  HAL_GPIO_DeInit(STBC02_CHG_GPIO_PORT, STBC02_CHG_PIN);
}

/**
 * @brief  Initializes the used ADC
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
static int32_t BC_BatMs_ADC_Channel_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = { 0 };

  /** Configure Regular Channel */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC4_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC4_SAMPLINGTIME_COMMON_1;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if(HAL_ADC_ConfigChannel(&ADC4_Handle, &sConfig) != HAL_OK)
  {
    return BSP_ERROR_NO_INIT;
  }
  return BSP_ERROR_NONE;
}

/**
 * @}
 */

/**
 * @brief  This function handles TIMx global interrupt request.
 * @param  None
 * @retval None
 */
void TIM5_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&BC_Chg_TIM_Handle);
}

/** @addtogroup STWIN_BOX_BATTERY_CHARGER_Interrupt_Callback_Functions
 * @{
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

/**
 * @}
 */

/**
 * @}
 */
