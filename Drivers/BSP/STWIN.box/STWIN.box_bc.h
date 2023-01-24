/**
 ******************************************************************************
 * @file    STWIN.box_bc.h
 * @author  SRA
 * @brief   This file contains definitions for STWIN.box_bc.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STWIN_BOX_BC_H
#define STWIN_BOX_BC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "STWIN.box.h"

/** @addtogroup BSP
 * @{
 */

/** @addtogroup STWIN_BOX 
 * @{
 */

/** @addtogroup STWIN_BOX_BATTERY_CHARGER 
 * @{
 */

/** @addtogroup STWIN_BOX_BATTERY_CHARGER_Public_Defines
 * @{
 */

#define BC_BATTERY_MAX_VOLTAGE 4225
#define BC_BATTERY_MIN_VOLTAGE 3250
#define WINDOW_VOLTAGE_DIM 16

#define STBC02_GetTick()                      HAL_GetTick()  //!< Get SysTick macro

/*! \name Timer Peripheral
 */
//@{
//#define STBC02_SW_TIM_PERIOD                (float_t)50e-6 // s
#define STBC02_SW_50US                      (float_t)50e-6 // s
#define STBC02_SW_TIM                       TIM4
#define STBC02_SW_TIM_CLK_ENABLE()          __HAL_RCC_TIM4_CLK_ENABLE()
#define STBC02_SW_TIM_CLK_DISABLE()         __HAL_RCC_TIM4_CLK_DISABLE()
////#define STBC02_SW_TIM_CLKFreq               (HAL_RCC_GetPCLK1Freq() * (((READ_BIT(RCC->CFGR, RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos) < 4U) ? 1U : 2U))
//#define STBC02_SW_TIM_CLKFreq               (HAL_RCC_GetPCLK1Freq() * (((READ_BIT(RCC->CFGR2, RCC_CFGR2_PPRE1) >> RCC_CFGR2_PPRE1_Pos) < 2U) ? 1U : 2U))

#define BC_SW_TIM_IRQn                      TIM4_IRQn
#define BC_Sw_TIM_IRQHandler                TIM4_IRQHandler

#ifndef BSP_BC_SW_TIM_IT_PRIORITY
#define BSP_BC_SW_TIM_IT_PRIORITY           3U
#endif

//@}

/*! \name GPIO for STBC02 Load switch selection input
 */
//@{
#define STBC02_SW_PIN                     GPIO_PIN_8
#define STBC02_SW_GPIO_PORT               GPIOA
#define STBC02_SW_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define STBC02_SW_GPIO_CLK_DISABLE()      __HAL_RCC_GPIOA_CLK_DISABLE()

//@}

/*! \name GPIO for STBC02 Charging/fault flag
 */
//@{
#define STBC02_CHG_PIN                        GPIO_PIN_0
#define STBC02_CHG_GPIO_PORT                  GPIOA
#define STBC02_CHG_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOA_CLK_ENABLE()
#define STBC02_CHG_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOA_CLK_DISABLE()

//@}

/**
 * @}
 */

/** @addtogroup STWIN_BOX_BATTERY_CHARGER_Public_Types
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
  SW1_OA_OFF = 1,
  SW1_OA_ON = 2,
  SW1_OB_OFF = 3,
  SW1_OB_ON = 4,
  SW2_OA_OFF = 5,
  SW2_OA_ON = 6,
  SW2_OB_OFF = 7,
  SW2_OB_ON = 8,
  BATMS_OFF = 9,
  BATMS_ON = 10,
  IEND_OFF = 11,
  IEND_5_PC_IFAST = 12,
  IEND_2_5_PC_IFAST = 13,
  IBAT_OCP_900_mA = 14,
  IBAT_OCP_450_mA = 15,
  IBAT_OCP_250_mA = 16,
  IBAT_OCP_100_mA = 17,
  VFLOAT_ADJ_OFF = 18,
  VFLOAT_ADJ_PLUS_50_mV = 19,
  VFLOAT_ADJ_PLUS_100_mV = 20,
  VFLOAT_ADJ_PLUS_150_mV = 21,
  VFLOAT_ADJ_PLUS_200_mV = 22,
  SHIPPING_MODE_ON = 23,
  AUTORECH_OFF = 24,
  AUTORECH_ON = 25,
  WATCHDOG_OFF = 26,
  WATCHDOG_ON = 27,
  IFAST_IPRE_50_PC_OFF = 28,
  IFAST_IPRE_50_PC_ON = 29
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
  float_t freq;
} stbc02_ChgStateNameAndFreq_TypeDef;

/**
 * @}
 */

/** @addtogroup STWIN_BOX_BATTERY_CHARGER_Public_Variables
 * @{
 */
typedef struct
{
  stbc02_ChgState_TypeDef Id;
  uint8_t Name[32];
} stbc02_State_TypeDef;

extern TIM_HandleTypeDef hstbc02_SWTim;

/**
 * @}
 */

/* Public Function_Prototypes -----------------------------------------------*/
void    BSP_BC_Sw_Init(void);
int32_t BSP_BC_Sw_CmdSend(stbc02_SwCmd_TypeDef stbc02_SwCmd);

int32_t BSP_BC_Chg_Init(void);
int32_t BSP_BC_Chg_DeInit(void);
void    BSP_BC_ChgPinHasToggled(TIM_HandleTypeDef *htim);
void    BSP_BC_GetState(stbc02_State_TypeDef *BC_State);
int32_t BSP_BC_IsChgPinToggling(void);

int32_t BSP_BC_BatMs_Init(void);
int32_t BSP_BC_BatMs_DeInit(void);
int32_t BC_BatMs_ADC_Initialization(void);
int32_t BC_BatMs_ADC_DeInitialization(void);

void STBC02_CHG_EXTI_IRQHandler(void);

int32_t BSP_BC_GetVoltage(uint32_t *BatteryVoltage);
int32_t BSP_BC_GetVoltageAndLevel(uint32_t *BatteryVoltage, uint32_t *BatteryLevel);
void STBC02_CHG_PIN_Callback(void);

void TIM5_IRQHandler(void);

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
#endif /* STWIN_BOX_BC_H */
