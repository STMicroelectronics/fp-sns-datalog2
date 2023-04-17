/**
  ******************************************************************************
  * @file    sysinit_mx.c
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

#include "services/systp.h"
#include "services/syserror.h"
#include "icache.h"
#include "rtc.h"
#include "gpio.h"

//Select the SystemClock_Config
//#define SystemClock_Config_SensorTile SystemClock_Config
#define SystemClock_Config_MX SystemClock_Config
#define Error_Handler sys_error_handler

/**
  * This type group together the components of the clock three to be modified during
  * a power mode change.
  */
typedef struct _system_clock_t
{
  uint32_t latency;
  RCC_OscInitTypeDef osc;
  RCC_ClkInitTypeDef clock;
  RCC_PeriphCLKInitTypeDef periph_clock;
} system_clock_t;

/**
  * It is used to save and restore the system clock during the power mode switch.
  */
static system_clock_t system_clock;

// Private member function declaration
// ***********************************
static void PeriphCommonClock_Config(void);

// Public API definition
// *********************

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI
                              |RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.LSIDiv = RCC_LSI_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 1;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphCommonClock_Config();
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
static void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the common periph clock
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_MDF1|RCC_PERIPHCLK_ADF1
                              |RCC_PERIPHCLK_ADCDAC;
  PeriphClkInit.Mdf1ClockSelection = RCC_MDF1CLKSOURCE_PLL3;
  PeriphClkInit.Adf1ClockSelection = RCC_ADF1CLKSOURCE_PLL3;
  PeriphClkInit.AdcDacClockSelection = RCC_ADCDACCLKSOURCE_PLL2;
  PeriphClkInit.PLL3.PLL3Source = RCC_PLLSOURCE_HSE;
  PeriphClkInit.PLL3.PLL3M = 2;
  PeriphClkInit.PLL3.PLL3N = 48;
  PeriphClkInit.PLL3.PLL3P = 2;
  PeriphClkInit.PLL3.PLL3Q = 25;
  PeriphClkInit.PLL3.PLL3R = 2;
  PeriphClkInit.PLL3.PLL3RGE = RCC_PLLVCIRANGE_1;
  PeriphClkInit.PLL3.PLL3FRACN = 0;
  PeriphClkInit.PLL3.PLL3ClockOut = RCC_PLL3_DIVQ;
  PeriphClkInit.PLL2.PLL2Source = RCC_PLLSOURCE_HSE;
  PeriphClkInit.PLL2.PLL2M = 2;
  PeriphClkInit.PLL2.PLL2N = 48;
  PeriphClkInit.PLL2.PLL2P = 2;
  PeriphClkInit.PLL2.PLL2Q = 7;
  PeriphClkInit.PLL2.PLL2R = 25;
  PeriphClkInit.PLL2.PLL2RGE = RCC_PLLVCIRANGE_1;
  PeriphClkInit.PLL2.PLL2FRACN = 0;
  PeriphClkInit.PLL2.PLL2ClockOut = RCC_PLL2_DIVR;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

void SystemClock_Backup(void)
{
  HAL_RCC_GetOscConfig(&(system_clock.osc));
  HAL_RCC_GetClockConfig(&(system_clock.clock), &(system_clock.latency));
  HAL_RCCEx_GetPeriphCLKConfig(&(system_clock.periph_clock));
}

/**
  * @brief  Restore original clock parameters
  * @retval Process result
  *         @arg SMPS_OK or SMPS_KO
  */
void SystemClock_Restore(void)
{
  /* SEQUENCE:
   *   1. Set PWR regulator to SCALE1_BOOST
   *   2. PLL ON
   *   3. Set SYSCLK source to PLL
   *
   * NOTE:
   *   Do not change or update the base-clock source (e.g. MSI and LSE)
   */

//  if(HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST) != HAL_OK)
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    sys_error_handler();
  }

//  if (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_PLL) {
  if (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_PLLCLK)
  {
    if (HAL_RCC_OscConfig(&(system_clock.osc)) != HAL_OK)
    {
      sys_error_handler();
    }
  }

  if (HAL_RCC_ClockConfig(&(system_clock.clock), system_clock.latency) != HAL_OK)
  {
    sys_error_handler();
  }
}

void SystemPower_Config()
{
//  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Enable Power Clock
  __HAL_RCC_PWR_CLK_ENABLE();

  // Select MSI as system clock source after Wake Up from Stop mode
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);

  // Init cache and RTC
  MX_ICACHE_Init();
  MX_RTC_Init();

  // This function is called in the early step of the system initialization.
  // All the PINs used by the application are reconfigured later by the application tasks.

  HAL_PWREx_EnableIO2VM();
  while (!(PWR->SVMCR & PWR_SVMCR_IO2VMEN));
  HAL_PWREx_EnableVddIO2();
  while (!(PWR->SVMSR & PWR_SVMSR_VDDIO2RDY));

  MX_GPIO_Init();

  /* GPIO Ports Clock Disable */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();
  __HAL_RCC_GPIOI_CLK_DISABLE();
}

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  // Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
  HAL_PWREx_DisableUCPDDeadBattery();

  // System interrupt init
  // MemoryManagement_IRQn interrupt configuration
  HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
  // BusFault_IRQn interrupt configuration
  HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
  // UsageFault_IRQn interrupt configuration
  HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
  // SVCall_IRQn interrupt configuration
  HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
  // DebugMonitor_IRQn interrupt configuration
  HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
  // PendSV_IRQn interrupt configuration
  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
  // SysTick_IRQn interrupt configuration
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

// Private function definition
// ***************************
