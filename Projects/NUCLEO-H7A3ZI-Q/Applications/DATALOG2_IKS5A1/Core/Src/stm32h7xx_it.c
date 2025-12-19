/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
  * @author  SRA
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_it.h"
#include "drivers/EXTIPinMap.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/

/**
  * Map one EXTI to n callback based on the GPIO PIN.
  */
static inline void ExtiDefISR(uint16_t exti_pin);

extern DMA_HandleTypeDef hdma_i2c1_rx;
extern DMA_HandleTypeDef hdma_i2c1_tx;
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim7;
extern PCD_HandleTypeDef hpcd_USB_OTG_HS;
extern TIM_HandleTypeDef htim6;

EXTI_DECLARE_PIN2F_MAP()

// Forward function declarations
// ****************************

extern void xPortSysTickHandler(void);

// Private function definition
// ***************************

void ExtiDefISR(uint16_t exti_pin)
{
  EXTIPin2CallbckMap xMap = EXTI_GET_P2F_MAP();
  for (int32_t i = 0; xMap[i].pfCallback != NULL; i++)
  {
    if (__HAL_GPIO_EXTI_GET_IT(xMap[i].nPin) && exti_pin == xMap[i].nPin)
    {
      /* EXTI line interrupt detected */
      __HAL_GPIO_EXTI_CLEAR_IT(xMap[i].nPin);
      xMap[i].pfCallback(xMap[i].nPin);
    }
  }
}

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line1 interrupt.
  */
void EXTI1_IRQHandler(void)
{
  ExtiDefISR(IIS2MDC_DRDY_Pin);
}

/**
  * @brief This function handles EXTI line3 interrupt.
  */
void EXTI3_IRQHandler(void)
{
  ExtiDefISR(IIS2DULPX_INT1_Pin);
}

/**
  * @brief This function handles DMA1 stream0 global interrupt.
  */
void DMA1_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_i2c1_rx);
}

/**
  * @brief This function handles DMA1 stream1 global interrupt.
  */
void DMA1_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_i2c1_tx);
}


/**
  * @brief This function handles I2C1 event interrupt.
  */
void I2C1_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&hi2c1);
}

/**
  * @brief This function handles I2C1 error interrupt.
  */
void I2C1_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&hi2c1);
}
/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  ExtiDefISR(ISM6HG256X_INT1_Pin);
  ExtiDefISR(ISM330IS_INT2_Pin);
  ExtiDefISR(ISM6HG256X_INT2_Pin);
}

/**
  * @brief This function handles TIM6 global interrupt, DAC1_CH1 and DAC1_CH2 underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim6);
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim7);
}

/**
  * @brief This function handles USB On The Go HS global interrupt.
  */
void OTG_HS_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_HS);
}

/* USER CODE BEGIN 1 */

