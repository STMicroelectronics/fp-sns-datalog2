/**
  ******************************************************************************
  * @file    STWIN_bc.c
  * @author  SRA
  *
  *
  * @brief   This file provides code for the configuration of the STBC02
  *          battery charger
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
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "STWIN_bc.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STWIN
  * @{
  */

/** @addtogroup STWIN_BATTERY_CHARGER
  * @{
  */

/** @addtogroup STWIN_BATTERY_CHARGER_Private_Constants
  * @{
  */

/**
  * @brief STBC02 status name and related toggling frequency (in Hz) of the nCHG pin
  * @note  The sequence must respect the order of stbc02_ChgState_TypeDef
  */
static const stbc02_ChgStateNameAndFreq_TypeDef stbc02_ChgStateNameAndFreq[10] =
{
  {"Not Valid Input", 0.0},
  {"Valid Input", 0.0},
  {"Vbat Low", 0.0},
  {"End Of Charge", 4.1},
  {"Charging Phase", 6.2},
  {"Overcharge Fault", 8.2},
  {"Charging Timeout", 10.2},
  {"Battery Voltage Below Vpre", 12.8},
  {"Charging Thermal Limitation", 14.2},
  {"Battery Temperature Fault", 16.2},
};


/**
  * @}
  */

/** @addtogroup STWIN_BATTERY_CHARGER_Private_Variables
  * @{
  */

static stbc02_SwCmd_TypeDef stbc02_SwCmdSel;                    //!< The selected STBC02 SW Cmd
static stbc02_SwState_TypeDef stbc02_SwState = idle;            //!< The state during STBC02 SW Cmd sending
static stbc02_ChgState_TypeDef stbc02_ChgState = NotValidInput; //!< The state of the STBC02
static uint32_t stbc02_ChgPinToggledTime = 0;                          //!< SysTick value when STBC02 nCHG pin is toggling
TIM_HandleTypeDef hstbc02_UsedTim;                              //!< Handler of the used timer
static TIM_HandleTypeDef    TimBCHandle;                               //!< Timer used to read BC status

/* Captured Values */
static uint32_t uwIC2Value1 = 0;
static uint32_t uwIC2Value2 = 0;
static uint32_t uwDiffCapture = 0;
static uint16_t uhCaptureIndex = 0;      /* Capture index */
static uint32_t uwFrequency = 0;         /* Frequency Value */

/**
  * @}
  */

/* Private Function_Prototypes -----------------------------------------------*/
void BC_InitCommon(void);
void BC_IO_Init(void);
void BC_IO_SW_SEL_Init(void);
void BC_IO_CHG_Init(void);
void BC_IO_CHG_DeInit(void);
void BC_ChgPinFreqGet(void);
void BC_ChgPinFreq2ChgSts(void);
void BC_TIMx_Init(void);
void BC_Chg_TIMx_Init(void);

void STBC02_USED_TIM_IRQHandler(void);
void HAL_TIM_BC_MspInit(TIM_HandleTypeDef *htim);

void HAL_ADC_BC_MspInit(void);
static int32_t Channel_ADC1_Init(void);
void HAL_ADC_BC_MspDeInit(void);
void HAL_TIM_BC_MspDeInit(TIM_HandleTypeDef *htim);

/** @addtogroup STWIN_BATTERY_CHARGER_Public_Functions
  * @{
  */


/**
  * @brief Initialize the STBC02
  * @retval None
  */
void BSP_BC_Init(void)
{
  BC_InitCommon();
  /* Init the related GPIOs */
  BC_IO_Init();
}


void BSP_BC_Chrg_Init(void)
{
  BC_Chg_TIMx_Init();
}


/**
  * @brief Send a single wire command to the STBC02
  * @param stbc02_SwCmd The command to be sent
  * @retval 0 in case of success
  * @retval 1 in case of failure
  */
int32_t BSP_BC_CmdSend(stbc02_SwCmd_TypeDef stbc02_SwCmd)
{
  uint32_t tk = STBC02_GetTick();

  stbc02_SwCmdSel = stbc02_SwCmd;
  stbc02_SwState = start;

  /* Start the time base */
  (void)HAL_TIM_Base_Start_IT(&hstbc02_UsedTim);

  while (stbc02_SwState != idle)
  {
    if ((STBC02_GetTick() - tk) > 10U)
    {
      return 1;
    }
  }
  /* Stop the time base */
  (void)HAL_TIM_Base_Stop_IT(&hstbc02_UsedTim);

  return 0;
}


/**
  * @brief Call the function for measuring the toggling frequency of the charging pin of the STBC02
  * @note Must be called when the charging pin of the STBC02 has toggled
  * @retval None
  */
void BSP_BC_ChgPinHasToggled(void)
{
  BC_ChgPinFreqGet();
  stbc02_ChgPinToggledTime = STBC02_GetTick();
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

  if ((STBC02_GetTick() - stbc02_ChgPinToggledTime) > 500U)
  {
    stbc02_ChgPinState = HAL_GPIO_ReadPin(STBC02_CHG_GPIO_PORT, STBC02_CHG_PIN);
    if (stbc02_ChgPinState == GPIO_PIN_RESET)
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
int32_t BSP_BC_BatMS_Init(void)
{
  /* Enable ADC1 if it's necessary */
  (void)BSP_ADC1_Initialization(ADC1_FOR_BC);
  /* Configure the Injection Channel for Voltage ADC convertion */
  (void)Channel_ADC1_Init();
  return 0;
}


/**
  * @brief  This method reset the peripherals used to get the current voltage of battery
  * @param  None
  * @retval BSP_ERROR_NONE in case of success
  * @retval BSP_ERROR_COMPONENT_FAILURE in case of failures
  */
int32_t BSP_BC_BatMS_DeInit(void)
{
  int32_t retValue;

  retValue = BSP_ADC1_DeInitialization(ADC1_FOR_BC);

  if (retValue != BSP_ERROR_NONE)
  {
    return retValue;
  }

  HAL_ADC_BC_MspDeInit();

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
  if (BSP_BC_IsChgPinToggling() == 1)
  {
    /* Get the status of the STBC02 */
    BC_ChgPinFreq2ChgSts();
  }

  BC_State->Id = stbc02_ChgState;
  (void)strncpy((char *) BC_State->Name, (char *)stbc02_ChgStateNameAndFreq[stbc02_ChgState].name, 32);
}

/**
  * @brief  This method gets the current voltage of battery
  * @param  mV pointer to destination variable
  * @retval BSP_ERROR_NONE in case of success
  * @retval BSP_ERROR_COMPONENT_FAILURE in case of failures
  */
int32_t BSP_BC_GetVoltage(uint32_t *mV)
{
  uint32_t uhADCxConvertedValue = 0;
  uint32_t Voltage;

  if (HAL_ADCEx_InjectedStart(&ADC1_Handle) != HAL_OK)
  {
    /* Start Conversation Error */
    return BSP_ERROR_COMPONENT_FAILURE;
  }

  if (HAL_ADCEx_InjectedPollForConversion(&ADC1_Handle, 10) != HAL_OK)
  {
    /* End Of Conversion flag not set on time */
    return BSP_ERROR_CLOCK_FAILURE;
  }

  /* Check if the continuous conversion of regular channel is finished */
  if ((HAL_ADC_GetState(&ADC1_Handle) & HAL_ADC_STATE_INJ_EOC) == HAL_ADC_STATE_INJ_EOC)
  {
    /*##-5- Get the converted value of regular channel  ########################*/
    uhADCxConvertedValue = HAL_ADCEx_InjectedGetValue(&ADC1_Handle, ADC_INJECTED_RANK_1);
  }

  Voltage = (2700U * (uint32_t)uhADCxConvertedValue) / (4095U);  // [0-2.7V]
  Voltage = ((56U + 100U) * Voltage) / 100U; // [0-4.2V]

  *mV = Voltage;
  return BSP_ERROR_NONE;
}

/**
  * @brief  This method gets the current voltage of battery and one estimation of Battery % Level
  * @param  uint32_t *Volt battery Voltage Value
  * @param  uint32_t *BatteryLevel Battery % Level
  * @retval BSP status
  */
int32_t BSP_BC_GetVoltageAndLevel(uint32_t *mV, uint32_t *BatteryLevel)
{
  uint32_t Voltage = 0;

  (void)BSP_BC_GetVoltage(&Voltage);

  /* Limits check */
  if (Voltage > (uint32_t)MAX_VOLTAGE)
  {
    Voltage = MAX_VOLTAGE;
  }
  if (Voltage < (uint32_t)MIN_VOLTAGE)
  {
    Voltage = MIN_VOLTAGE;
  }

  *BatteryLevel = (((Voltage - (uint32_t)MIN_VOLTAGE) * 100U) / (uint32_t)(MAX_VOLTAGE - MIN_VOLTAGE));
  *mV = Voltage;

  return BSP_ERROR_NONE;
}

/**
  * @}
  */


/** @addtogroup STWIN_BATTERY_CHARGER_Private_Functions
  * @{
  */

/**
  * @brief Initialize the STBC02
  * @retval None
  */
void BC_InitCommon(void)
{
  /* Init the time base */
  BC_TIMx_Init();
}


/**
  * @brief  Initializes the GPIO used for the Li-Ion Battery Charger
  * @retval None
  */
void BC_IO_Init(void)
{
  BC_IO_SW_SEL_Init();
}


/**
  * @brief  Initializes the SW_SEL GPIO used for the Li-Ion Battery Charger
  * @retval None
  */
void BC_IO_SW_SEL_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  STBC02_SW_SEL_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin = STBC02_SW_SEL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_WritePin(STBC02_SW_SEL_GPIO_PORT, STBC02_SW_SEL_PIN, GPIO_PIN_RESET);
  HAL_GPIO_Init(STBC02_SW_SEL_GPIO_PORT, &GPIO_InitStruct);
}


/**
  * @brief  Initializes the nCHG GPIO used for the Li-Ion Battery Charger
  * @retval None
  */
void BC_IO_CHG_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  STBC02_CHG_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin = STBC02_CHG_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  HAL_GPIO_Init(STBC02_CHG_GPIO_PORT, &GPIO_InitStruct);
}


/**
  * @brief  De-Initializes the nCHG GPIO used for the Li-Ion Battery Charger
  * @retval None
  */
void BC_IO_CHG_DeInit(void)
{
  HAL_GPIO_DeInit(STBC02_CHG_GPIO_PORT, STBC02_CHG_PIN);
}


/**
  * @brief Single wire command manager for the STBC02
  * @note This function have to be called every 5 us
  * @retval None
  */
void BC_CmdMng(void)
{
  static uint16_t TIMx_Pulse = 0;                           //! Actual timer pulse number
  static uint16_t TIMx_PulseTCS = 0;                        //! Timer pulse number to change state
  static stbc02_SwState_TypeDef stbc02_sw_state_prv = idle; //! Previous state
  static uint8_t CmdPulse = 0;                              //! Cmd pulse number

  TIMx_Pulse++;
  switch (stbc02_SwState)
  {
    case idle:
      break;
    case start:
      HAL_GPIO_TogglePin(STBC02_SW_SEL_GPIO_PORT, STBC02_SW_SEL_PIN);
      TIMx_PulseTCS = TIMx_Pulse + (uint16_t)(350 / 5);
      stbc02_sw_state_prv = stbc02_SwState;
      stbc02_SwState = wait;
      break;
    case pulse_l:
      HAL_GPIO_TogglePin(STBC02_SW_SEL_GPIO_PORT, STBC02_SW_SEL_PIN);
      TIMx_PulseTCS = TIMx_Pulse + (uint16_t)(100 / 5);
      stbc02_sw_state_prv = stbc02_SwState;
      stbc02_SwState = wait;
      break;
    case pulse_h:
      HAL_GPIO_TogglePin(STBC02_SW_SEL_GPIO_PORT, STBC02_SW_SEL_PIN);
      TIMx_PulseTCS = TIMx_Pulse + (uint16_t)(100 / 5);
      stbc02_sw_state_prv = stbc02_SwState;
      stbc02_SwState = wait;
      break;
    case stop_l:
      HAL_GPIO_TogglePin(STBC02_SW_SEL_GPIO_PORT, STBC02_SW_SEL_PIN);
      TIMx_PulseTCS = TIMx_Pulse + (uint16_t)(100 / 5);
      stbc02_sw_state_prv = stbc02_SwState;
      stbc02_SwState = wait;
      break;
    case stop_h:
      HAL_GPIO_TogglePin(STBC02_SW_SEL_GPIO_PORT, STBC02_SW_SEL_PIN);
      TIMx_PulseTCS = TIMx_Pulse + (uint16_t)(500 / 5);
      stbc02_sw_state_prv = stbc02_SwState;
      stbc02_SwState = wait;
      break;
    case wait:
      if (TIMx_Pulse > TIMx_PulseTCS)
      {
        if (stbc02_sw_state_prv == stop_h)
        {
          CmdPulse = 0;
          stbc02_SwState = idle;
          HAL_GPIO_WritePin(STBC02_SW_SEL_GPIO_PORT, STBC02_SW_SEL_PIN, GPIO_PIN_RESET);
        }
        else
        {
          if (stbc02_sw_state_prv == pulse_h)
          {
            CmdPulse++;
            if (CmdPulse < (uint8_t)stbc02_SwCmdSel)
            {
              stbc02_sw_state_prv = start;
            }
            else
              __NOP();
          }
          stbc02_SwState = (stbc02_SwState_TypeDef)(stbc02_sw_state_prv + 1U);
          __NOP();
        }
      }
      break;
    default:
      break;
  }
}


/**
  * @brief Measure the toggling frequency of the charging pin of the STBC02
  * @note This function works with 1 ms as time base
  * @retval None
  */
void BC_ChgPinFreqGet(void)
{
  if (uhCaptureIndex == 0U)
  {
    /* Get the 1st Input Capture value */
    uwIC2Value1 = HAL_GetTick();
    uhCaptureIndex = 1;
  }
  if (uhCaptureIndex == 1U)
  {
    /* Get the 2nd Input Capture value */
    uwIC2Value2 = HAL_GetTick();

    /* Capture computation */
    if (uwIC2Value2 > uwIC2Value1)
    {
      uwDiffCapture = (uwIC2Value2 - uwIC2Value1);
    }
    else
    {
      uwDiffCapture = ((0xFFFFFFFFU - uwIC2Value1) + uwIC2Value2) + 1U;
    }

    if (uwDiffCapture != 0U)
    {
      /* Frequency computation */
      uwFrequency = 10000U / uwDiffCapture;
      uhCaptureIndex = 0U;
    }
  }
}


/**
  * @brief Get the status of the STBC02 checking the toggling frequency of the charging pin of the STBC02
  * @retval None
  */
void BC_ChgPinFreq2ChgSts(void)
{
  float Batt_Freq;
  stbc02_ChgState_TypeDef ChgState;
  Batt_Freq = (float)uwFrequency / (float)10;

  if (Batt_Freq > (float)0)
  {
    for (ChgState =  EndOfCharge; ChgState < BatteryTemperatureFault; ChgState++)
    {
      if ((Batt_Freq < ((stbc02_ChgStateNameAndFreq[ChgState].freq + stbc02_ChgStateNameAndFreq[(uint8_t)ChgState +
                         1U].freq) / (float)2)))
      {
        stbc02_ChgState = ChgState;
        break;
      }
    }
    if ((Batt_Freq > ((stbc02_ChgStateNameAndFreq[(uint8_t)ChgState - 1U].freq +
                       stbc02_ChgStateNameAndFreq[ChgState].freq) / (float)2)))
    {
      stbc02_ChgState = ChgState;
    }
  }
}


/**
  * @brief  Initializes the used timer
  * @retval None
  */
void BC_Chg_TIMx_Init(void)
{
  /*##-1- Configure the TIM peripheral #######################################*/
  /* TIM1 configuration: Input Capture mode ---------------------
  The external signal is connected to TIM3 CH3 (PB0)
  The Rising edge is used as active edge
  ------------------------------------------------------------ */
  /* Timer Input Capture Configuration Structure declaration */
  TIM_IC_InitTypeDef     sICConfig;

  /* Set TIMx instance */
  TimBCHandle.Instance = TIM3;

  /* Initialize TIMx peripheral as follows:
  + Period = 0xFFFF
  + Prescaler = 0
  + ClockDivision = 0
  + Counter direction = Up
  */
  TimBCHandle.Init.Period            = 0xFFFF;
  TimBCHandle.Init.Prescaler         = 0;
  TimBCHandle.Init.ClockDivision     = 0;
  TimBCHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimBCHandle.Init.RepetitionCounter = 0;
  (void)HAL_TIM_IC_Init(&TimBCHandle);

  /*##-2- Configure the Input Capture channel ################################*/
  /* Configure the Input Capture of channel 3*/
  sICConfig.ICPolarity  = TIM_ICPOLARITY_RISING;
  sICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sICConfig.ICPrescaler = TIM_ICPSC_DIV1;
  sICConfig.ICFilter    = 0;
  (void)HAL_TIM_IC_ConfigChannel(&TimBCHandle, &sICConfig, TIM_CHANNEL_3);

  /*##-3- Start the Input Capture in interrupt mode ##########################*/
  (void)HAL_TIM_IC_Start_IT(&TimBCHandle, TIM_CHANNEL_3);
}


/**
  * @brief  Initializes the used timer
  * @retval None
  */
void BC_TIMx_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  uint32_t freq = STBC02_USED_TIM_CLKFreq;
  float prescaler = 0.0f;
  float period;
  uint32_t loop_counter;

  do
  {
    prescaler++;
    period = (((float)freq) / (prescaler) * STBC02_USED_TIM_PERIOD);
    loop_counter = (uint32_t) period;
  } while (loop_counter > 65535U);

  hstbc02_UsedTim.Instance = STBC02_USED_TIM;
  hstbc02_UsedTim.Init.Prescaler = ((uint32_t)prescaler - 1U);
  hstbc02_UsedTim.Init.CounterMode = TIM_COUNTERMODE_UP;
  hstbc02_UsedTim.Init.Period = (uint32_t)period;
#if USE_TIM_AUTORELOAD_PRELOAD
  hstbc02_UsedTim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
#endif //USE_TIM_AUTORELOAD_PRELOAD

  HAL_TIM_BC_MspInit(&hstbc02_UsedTim);
  (void)HAL_TIM_Base_Init(&hstbc02_UsedTim);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  (void)HAL_TIMEx_MasterConfigSynchronization(&hstbc02_UsedTim, &sMasterConfig);
}

/**
  * @}
  */


/** @addtogroup STWIN_BATTERY_CHARGER_MSP_Functions
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
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
  UNUSED(htim);
  GPIO_InitTypeDef   GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIM3 Peripheral clock enable */
  __HAL_RCC_TIM3_CLK_ENABLE();
  /* Enable GPIO channels Clock */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure  (TIMx_Channel) in Alternate function, push-pull and high speed */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*##-2- Configure the NVIC for TIMx #########################################*/

  HAL_NVIC_SetPriority(TIM3_IRQn, 0, 1);

  /* Enable the TIMx global Interrupt */
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
}


/**
  * @brief  Initializes the TIM Base MSP.
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_BC_MspInit(TIM_HandleTypeDef *htim)
{

  if (htim->Instance == STBC02_USED_TIM)
  {
    /* STBC02_USED_TIM clock enable */
    STBC02_USED_TIM_CLK_ENABLE();

    /* STBC02_USED_TIM interrupt Init */
    HAL_NVIC_SetPriority(STBC02_USED_TIM_IRQn, STBC02_USED_TIM_IRQ_PP, STBC02_USED_TIM_IRQ_SP);
    HAL_NVIC_EnableIRQ(STBC02_USED_TIM_IRQn);
  }
}

/**
  * @brief  DeInitialize TIM Base MSP.
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_BC_MspDeInit(TIM_HandleTypeDef *htim)
{

  if (htim->Instance == STBC02_USED_TIM)
  {
    /* Peripheral clock disable */
    STBC02_USED_TIM_CLK_DISABLE();

    /* STBC02_USED_TIM interrupt Deinit */
    HAL_NVIC_DisableIRQ(STBC02_USED_TIM_IRQn);
  }
}


/**
  * @brief  Initialize the ADC MSP.
  * @param BC_AdcHandle ADC handle
  * @retval None
  */
void HAL_ADC_BC_MspInit()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* STBC02_USED_ADC GPIO Configuration */
  STBC02_BATMS_GPIO_CLK_ENABLE();
  GPIO_InitStruct.Pin = STBC02_BATMS_PIN;
  GPIO_InitStruct.Mode = STBC02_BATMS_GPIO_MODE;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(STBC02_BATMS_GPIO_PORT, &GPIO_InitStruct);

}

/**
  * @brief  DeInitialize the ADC MSP.
  * @param BC_AdcHandle ADC handle
  * @note   All ADC instances use the same core clock at RCC level, disabling
  *         the core clock reset all ADC instances).
  * @retval None
  */
void HAL_ADC_BC_MspDeInit(void)
{
  /* STBC02_USED_ADC GPIO Configuration */
  HAL_GPIO_DeInit(STBC02_BATMS_GPIO_PORT, STBC02_BATMS_PIN);
}

/**
  * @brief  Initializes the used ADC
  * @retval 0 in case of success
  * @retval 1 in case of failure
  */
static int32_t Channel_ADC1_Init(void)
{
  ADC_InjectionConfTypeDef sConfigInjected = {0};
  static uint8_t ADC_stopped = 0;
  uint32_t ADC_Status;


  ADC_Status = HAL_ADC_GetState(&ADC1_Handle);
  if ((ADC_Status & HAL_ADC_STATE_REG_BUSY) == HAL_ADC_STATE_REG_BUSY)
  {
    (void)HAL_ADC_Stop(&ADC1_Handle);
    ADC_stopped = 1;
  }


  /* Configure ADC injected channel */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_13;
  sConfigInjected.InjectedRank = ADC_INJECTED_RANK_1;
  sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_640CYCLES_5;
  sConfigInjected.InjectedSingleDiff = ADC_SINGLE_ENDED;
  sConfigInjected.InjectedOffsetNumber = ADC_OFFSET_NONE;
  sConfigInjected.InjectedOffset = 0;
  sConfigInjected.InjectedNbrOfConversion = 1;
  sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
  sConfigInjected.AutoInjectedConv = DISABLE;
  sConfigInjected.QueueInjectedContext = DISABLE;
  sConfigInjected.ExternalTrigInjecConv = ADC_INJECTED_SOFTWARE_START;
  sConfigInjected.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONV_EDGE_NONE;
  sConfigInjected.InjecOversamplingMode = DISABLE;

  HAL_ADC_BC_MspInit();


  (void)HAL_ADCEx_InjectedConfigChannel(&ADC1_Handle, &sConfigInjected);

  if (ADC_stopped == 1U)
  {
    (void)HAL_ADC_Start(&ADC1_Handle);
  }

  return 0;
}

/**
  * @}
  */


/** @addtogroup STWIN_BATTERY_CHARGER_Interrupt_Callback_Functions
  * @{
  */

#if USE_BC_TIM_IRQ_CALLBACK
/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == STBC02_USED_TIM)
  {
    BC_CmdMng();
  }
}

/**
  * @brief  Conversion complete callback in non blocking mode
  * @param  htim : hadc handle
  * @retval None
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
  {
    BSP_BC_ChgPinHasToggled();
  }
}
#endif //USE_BC_TIM_IRQ_CALLBACK

#if USE_BC_GPIO_IRQ_CALLBACK
/**
  * @brief  EXTI line detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void STBC02_CHG_PIN_Callback(void)
{
  BSP_BC_ChgPinHasToggled();
}


void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
}
#endif //USE_BC_GPIO_IRQ_CALLBACK

/**
  * @}
  */

/** @addtogroup STWIN_BATTERY_CHARGER_Interrupt_Service_Routines
  * @{
  */
/**
  * @brief This function handles STBC02_USED_TIM global interrupt.
  */
void STBC02_USED_TIM_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&hstbc02_UsedTim);
}

#if USE_BC_GPIO_IRQ_HANDLER
/**
  * @brief This function handles EXTI for STBC02_CHG_PIN interrupts.
  */
void STBC02_CHG_EXTI_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(STBC02_CHG_PIN);
}

/**
  * @brief  This function handles TIMx global interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimBCHandle);
}
#endif // USE_BC_GPIO_IRQ_HANDLER

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


