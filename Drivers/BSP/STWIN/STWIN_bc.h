/**
  ******************************************************************************
  * @file    STWIN_bc.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STWIN_BC_H
#define STWIN_BC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "STWIN.h"


/** @addtogroup BSP
  * @{
  */

/** @addtogroup STWIN
  * @{
  */

/** @addtogroup STWIN_BATTERY_CHARGER
  * @{
  */

/** @addtogroup STWIN_BATTERY_CHARGER_Public_Defines
  * @{
  */

#define MAX_VOLTAGE 4225
#define MIN_VOLTAGE 3250

#define STBC02_GetTick()                      HAL_GetTick()  //!< Get SysTick macro

/*! \name Timer Peripheral
*/
//@{
#define STBC02_USED_TIM_PERIOD                (float)5e-6 // s
#define STBC02_USED_TIM                       TIM7
#define STBC02_USED_TIM_CLK_ENABLE()          __HAL_RCC_TIM7_CLK_ENABLE()
#define STBC02_USED_TIM_CLK_DISABLE()         __HAL_RCC_TIM7_CLK_DISABLE()
#define STBC02_USED_TIM_CLKFreq               (HAL_RCC_GetPCLK1Freq() * (((READ_BIT(RCC->CFGR, RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos) < 4U) ? 1U : 2U))
#define STBC02_USED_TIM_IRQn                  TIM7_IRQn
#define STBC02_USED_TIM_IRQHandler            TIM7_IRQHandler

#ifndef STBC02_USED_TIM_IRQ_PP
#define STBC02_USED_TIM_IRQ_PP                3
#endif

#ifndef STBC02_USED_TIM_IRQ_SP
#define STBC02_USED_TIM_IRQ_SP                0
#endif
//@}

/*! \name ADC Peripheral
*/
//@{
#define STBC02_USED_ADC                       ADC1
#define STBC02_USED_ADC_CLK_ENABLE()          __HAL_RCC_ADC_CLK_ENABLE()
#define STBC02_USED_ADC_CLK_DISABLE()         __HAL_RCC_ADC_CLK_DISABLE()
#define STBC02_USED_ADC_CHANNEL               ADC_CHANNEL_13
//@}

/*! \name GPIO for STBC02 Load switch selection input
*/
//@{
#define STBC02_SW_SEL_PIN                     GPIO_PIN_6
#define STBC02_SW_SEL_GPIO_PORT               GPIOF
#define STBC02_SW_SEL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE()
#define STBC02_SW_SEL_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOF_CLK_DISABLE()

//@}

/*! \name GPIO for STBC02 Charging/fault flag
*/
//@{
#define STBC02_CHG_PIN                        GPIO_PIN_0
#define STBC02_CHG_GPIO_PORT                  GPIOB
#define STBC02_CHG_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define STBC02_CHG_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOB_CLK_DISABLE()

//@}

/*! \name GPIO for STBC02 Battery voltage measurement
*/
//@{
#define STBC02_BATMS_PIN                      GPIO_PIN_4
#define STBC02_BATMS_GPIO_PORT                GPIOC
#define STBC02_BATMS_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define STBC02_BATMS_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOC_CLK_DISABLE()
#define STBC02_BATMS_GPIO_MODE                GPIO_MODE_ANALOG_ADC_CONTROL
//@}

/**
  * @}
  */

/** @addtogroup STWIN_BATTERY_CHARGER_Public_Types
  * @{
  */

/**
  * @brief Device state
  */
typedef enum
{
  NotValidInput = 0,
  ValidInput,
  VbatLow,
  EndOfCharge,
  ChargingPhase,
  OverchargeFault,
  ChargingTimeout,
  BatteryVoltageBelowVpre,
  ChargingThermalLimitation,
  BatteryTemperatureFault
} stbc02_ChgState_TypeDef;

/**
  * @brief SW selection pulse number
  */
typedef enum
{
  SW1_OA_OFF              = 1,
  SW1_OA_ON               = 2,
  SW1_OB_OFF              = 3,
  SW1_OB_ON               = 4,
  SW2_OA_OFF              = 5,
  SW2_OA_ON               = 6,
  SW2_OB_OFF              = 7,
  SW2_OB_ON               = 8,
  BATMS_OFF               = 9,
  BATMS_ON                = 10,
  IEND_OFF                = 11,
  IEND_5_PC_IFAST         = 12,
  IEND_2_5_PC_IFAST       = 13,
  IBAT_OCP_900_mA         = 14,
  IBAT_OCP_450_mA         = 15,
  IBAT_OCP_250_mA         = 16,
  IBAT_OCP_100_mA         = 17,
  VFLOAT_ADJ_OFF          = 18,
  VFLOAT_ADJ_PLUS_50_mV   = 19,
  VFLOAT_ADJ_PLUS_100_mV  = 20,
  VFLOAT_ADJ_PLUS_150_mV  = 21,
  VFLOAT_ADJ_PLUS_200_mV  = 22,
  SHIPPING_MODE_ON        = 23,
  AUTORECH_OFF            = 24,
  AUTORECH_ON             = 25,
  WATCHDOG_OFF            = 26,
  WATCHDOG_ON             = 27,
  IFAST_IPRE_50_PC_OFF    = 28,
  IFAST_IPRE_50_PC_ON     = 29
} stbc02_SwCmd_TypeDef;

/**
  * @brief SW pulse state
  */
typedef enum
{
  idle,
  start,
  pulse_l,
  pulse_h,
  stop_l,
  stop_h,
  wait
} stbc02_SwState_TypeDef;

/**
  * @brief Device state structure as name and nominal frequency
  */
typedef struct
{
  char *name;
  float freq;
} stbc02_ChgStateNameAndFreq_TypeDef;

/**
  * @}
  */

/** @addtogroup STWIN_BATTERY_CHARGER_Public_Variables
  * @{
  */
typedef struct
{
  stbc02_ChgState_TypeDef Id;
  uint8_t Name[32];
} stbc02_State_TypeDef;

extern TIM_HandleTypeDef hstbc02_UsedTim;

/**
  * @}
  */

/* Public Function_Prototypes -----------------------------------------------*/
int32_t BSP_BC_BatMS_Init(void);
int32_t BSP_BC_BatMS_DeInit(void);
void BSP_BC_Init(void);
void BSP_BC_Chrg_Init(void);
int32_t BSP_BC_CmdSend(stbc02_SwCmd_TypeDef stbc02_SwCmd);
void BSP_BC_ChgPinHasToggled(void);
int32_t BSP_BC_IsChgPinToggling(void);

void BSP_BC_GetState(stbc02_State_TypeDef *BC_State);
void STBC02_CHG_EXTI_IRQHandler(void);
void TIM1_BRK_TIM15_IRQHandler(void);

int32_t BSP_BC_GetVoltage(uint32_t *mV);
int32_t BSP_BC_GetVoltageAndLevel(uint32_t *mV, uint32_t *BatteryLevel);
void STBC02_CHG_PIN_Callback(void);

void BC_CmdMng(void);
void TIM3_IRQHandler(void);
void EXTI15_10_IRQHandler(void);


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif
#endif /* STWIN_BC_H */


