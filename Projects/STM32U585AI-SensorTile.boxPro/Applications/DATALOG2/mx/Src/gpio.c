/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics..
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */
#define Error_Handler sys_error_handler
void sys_error_handler(void);
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
     PA14 (JTCK/SWCLK)   ------> DEBUG_JTCK-SWCLK
     PC14-OSC32_IN (PC14)   ------> RCC_OSC32_IN
     PB3 (JTDO/TRACESWO)   ------> DEBUG_JTDO-SWO
     PC15-OSC32_OUT (PC15)   ------> RCC_OSC32_OUT
     PA13 (JTMS/SWDIO)   ------> DEBUG_JTMS-SWDIO
     PH0-OSC_IN (PH0)   ------> RCC_OSC_IN
     PH1-OSC_OUT (PH1)   ------> RCC_OSC_OUT
     PB0   ------> ADC1_IN15
     PB10   ------> S_TIM2_CH3
     PE13   ------> S_TIM1_CH3
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, SPI_SEN_CS_G_Pin|MCU_SEL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, LED3_Pin|EN_SD_Pin|LED2_Pin|SEL_SD_V_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI_SEN_CS_A_GPIO_Port, SPI_SEN_CS_A_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(V_BT_E1_GPIO_Port, V_BT_E1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, BLE_RST_Pin|BT_BOOT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, SPI_DUT_CS2_Pin|SPI_DUT_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, USB_ENUM_LED_Pin|LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BLE_SPI_CS_GPIO_Port, BLE_SPI_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = V4_GOOD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(V4_GOOD_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PGPin PGPin PG4 PG0
                           PG3 PG5 */
  GPIO_InitStruct.Pin = WIFI_FLOW_Pin|DIL_INT2_Pin|GPIO_PIN_4|GPIO_PIN_0
                          |GPIO_PIN_3|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : PA15 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PH15 PH3 PH14 PH13
                           PH9 PH4 PH6 PH7
                           PH5 PH2 */
  GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_3|GPIO_PIN_14|GPIO_PIN_13
                          |GPIO_PIN_9|GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_5|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PIPin PIPin PIPin */
  GPIO_InitStruct.Pin = SPI_SEN_CS_G_Pin|MCU_SEL_Pin|SPI_SEN_CS_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PBPin PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|VBATT_Pin|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PD0 PD7 PD14 PD15
                           PD10 PD8 PD9 PD11 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_7|GPIO_PIN_14|GPIO_PIN_15
                          |GPIO_PIN_10|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PI4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pins : PHPin PHPin PHPin PHPin */
  GPIO_InitStruct.Pin = LED3_Pin|EN_SD_Pin|LED2_Pin|SEL_SD_V_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PEPin PEPin PEPin */
  GPIO_InitStruct.Pin = MDF1_SDI3_Pin|GPIO_JP4_Pin|MDF1_SDI2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = V_BT_E1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(V_BT_E1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin */
  GPIO_InitStruct.Pin = BT_BOOT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PEPin PEPin PEPin PEPin
                           PEPin */
  GPIO_InitStruct.Pin = WC_INTB_Pin|SW2_Pin|MAG_DRDY_Pin|PRESS_INT_Pin
                          |NFC_GPO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = MDF1_SDI0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MDF1_SDI0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PF0 PF1 PF5 PF3
                           PF4 PF12 PF14 PF11
                           PF13 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_5|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_12|GPIO_PIN_14|GPIO_PIN_11
                          |GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = SW1_Pin|BUTTON_PWR_Pin|SD_DETECT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PGPin PGPin */
  GPIO_InitStruct.Pin = SPI_DUT_CS2_Pin|SPI_DUT_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : PFPin PFPin PFPin */
  GPIO_InitStruct.Pin = SW_ON_Pin|WC_RSTB_Pin|MDF1_CCK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = ACC_INT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(ACC_INT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PFPin PFPin */
  GPIO_InitStruct.Pin = USB_ENUM_LED_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = LOCK_1v8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LOCK_1v8_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PC2 PC4 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PE14 PE11 PEPin */
  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_11|LDO_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = DIL_INT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DIL_INT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = PWM_OUT_T1_C3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(PWM_OUT_T1_C3_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PB1 */
  GPIO_InitStruct.Pin = MDF1_SDI1_Pin|TEMP_INT_Pin|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = IMU_INT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IMU_INT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = ACC_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ACC_INT2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 7, 0);
  HAL_NVIC_SetPriority(EXTI1_IRQn, 7, 0);
  HAL_NVIC_SetPriority(EXTI2_IRQn, 7, 0);
  HAL_NVIC_SetPriority(EXTI3_IRQn, 7, 0);
  HAL_NVIC_SetPriority(EXTI4_IRQn, 7, 0);
  HAL_NVIC_SetPriority(EXTI5_IRQn, 7, 0);
  HAL_NVIC_SetPriority(EXTI6_IRQn, 7, 0);
  HAL_NVIC_SetPriority(EXTI7_IRQn, 7, 0);
  HAL_NVIC_SetPriority(EXTI8_IRQn, 7, 0);
  HAL_NVIC_SetPriority(EXTI9_IRQn, 7, 0);
  HAL_NVIC_SetPriority(EXTI10_IRQn, 7, 0);
  HAL_NVIC_SetPriority(EXTI11_IRQn, 7, 0);
  HAL_NVIC_SetPriority(EXTI13_IRQn, 7, 0);
  HAL_NVIC_SetPriority(EXTI14_IRQn, 7, 0);
  HAL_NVIC_SetPriority(EXTI15_IRQn, 7, 0);

}

/* USER CODE BEGIN 2 */

void MX_GPIO_PE8_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_PWREx_EnableVddIO2();
  /* GPIO Ports Clock Enable */
   __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin : PE8Pin */
  GPIO_InitStruct.Pin = PRESS_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PRESS_INT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(PRESS_INT_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(PRESS_INT_EXTI_IRQn);
}

void MX_GPIO_PE6_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_PWREx_EnableVddIO2();
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin : PE6Pin */
  GPIO_InitStruct.Pin = MAG_DRDY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MAG_DRDY_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(MAG_DRDY_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(MAG_DRDY_EXTI_IRQn);
}

void MX_GPIO_PF2_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_PWREx_EnableVddIO2();
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /*Configure GPIO pin : PF2Pin */
  GPIO_InitStruct.Pin = ACC_INT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(ACC_INT1_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(ACC_INT1_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(ACC_INT1_EXTI_IRQn);
}

void MX_GPIO_PI7_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  HAL_PWREx_EnableVddIO2();
  __HAL_RCC_GPIOI_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI_SEN_CS_A_GPIO_Port, SPI_SEN_CS_A_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PI7 */
  GPIO_InitStruct.Pin = SPI_SEN_CS_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SPI_SEN_CS_A_GPIO_Port, &GPIO_InitStruct);
}

void MX_GPIO_PA4_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : PA4Pin */
  GPIO_InitStruct.Pin = IMU_INT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IMU_INT1_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(IMU_INT1_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(IMU_INT1_EXTI_IRQn);
}

void MX_GPIO_PI5_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  HAL_PWREx_EnableVddIO2();
  __HAL_RCC_GPIOI_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI_SEN_CS_G_GPIO_Port, SPI_SEN_CS_G_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PI5 */
  GPIO_InitStruct.Pin = SPI_SEN_CS_G_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SPI_SEN_CS_G_GPIO_Port, &GPIO_InitStruct);
}

/* User Button SW1 */
void MX_GPIO_PC13_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin : PC13Pin */
  GPIO_InitStruct.Pin = SW1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SW1_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(SW1_EXTI_IRQn, 15, 0);
}

/* User Button SW2 */
void MX_GPIO_PE0_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin : PE0Pin */
  GPIO_InitStruct.Pin = SW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SW2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(SW2_EXTI_IRQn, 15, 0);
}

/* LED1 */
void MX_GPIO_PF6_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();

  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin : PF6Pin */
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);
}

/* LED2 */
void MX_GPIO_PH11_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();

  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin : PH11Pin */
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);
}

/* LED3 */
void MX_GPIO_PH12_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();

  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
  /*Configure GPIO pin : PH12Pin */
  GPIO_InitStruct.Pin = LED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED3_GPIO_Port, &GPIO_InitStruct);
}

#if 0
void MX_GPIO_PI6_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_PWREx_EnableVddIO2();
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOI_CLK_ENABLE();

  /*Configure GPIO pin : PI6Pin */
  GPIO_InitStruct.Pin = INT_HTS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INT_HTS_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT_HTS_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(INT_HTS_EXTI_IRQn);
}

void MX_GPIO_PB1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PB1Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

void MX_GPIO_PG6_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_PWREx_EnableVddIO2();
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin : PG6Pin */
  GPIO_InitStruct.Pin = INT_EX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INT_EX_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT_EX_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(INT_EX_EXTI_IRQn);
}

void MX_GPIO_PF3_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_PWREx_EnableVddIO2();
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /*Configure GPIO pin : PF3Pin */
  GPIO_InitStruct.Pin = INT1_ICLX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INT1_ICLX_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT1_ICLX_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(INT1_ICLX_EXTI_IRQn);
}

void MX_GPIO_PH6_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  HAL_PWREx_EnableVddIO2();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_DLPC_GPIO_Port, CS_DLPC_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PH6 */
  GPIO_InitStruct.Pin = CS_DLPC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(CS_DLPC_GPIO_Port, &GPIO_InitStruct);
}

void MX_GPIO_PB8_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_PWREx_EnableVddIO2();
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PB8Pin */
  GPIO_InitStruct.Pin = INT1_DHCX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INT1_DHCX_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT1_DHCX_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(INT1_DHCX_EXTI_IRQn);
}

void MX_GPIO_PF4_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_PWREx_EnableVddIO2();
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /*Configure GPIO pin : PF4Pin */
  GPIO_InitStruct.Pin = INT2_DHCX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INT2_DHCX_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT2_DHCX_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(INT2_DHCX_EXTI_IRQn);
}

void MX_GPIO_PH15_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  HAL_PWREx_EnableVddIO2();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_DHCX_GPIO_Port, CS_DHCX_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PH15 */
  GPIO_InitStruct.Pin = CS_DHCX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(CS_DHCX_GPIO_Port, &GPIO_InitStruct);
}

void MX_GPIO_PF15_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_PWREx_EnableVddIO2();
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /*Configure GPIO pin : PF15Pin */
  GPIO_InitStruct.Pin = INT1_DWB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INT1_DWB_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT1_DWB_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(INT1_DWB_EXTI_IRQn);
}

void MX_GPIO_PF12_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  HAL_PWREx_EnableVddIO2();
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_DWB_GPIO_Port, CS_DWB_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PF12 */
  GPIO_InitStruct.Pin = CS_DWB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(CS_DWB_GPIO_Port, &GPIO_InitStruct);
}

void MX_GPIO_PD15_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  HAL_PWREx_EnableVddIO2();
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin : PF15Pin */
  GPIO_InitStruct.Pin = INT_EXD15_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INT_EXD15_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT1_DWB_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(INT1_DWB_EXTI_IRQn);
}

void MX_GPIO_PI0_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  HAL_PWREx_EnableVddIO2();
  __HAL_RCC_GPIOI_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(EX_SPI_NSS_GPIO_Port, EX_SPI_NSS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PF12 */
  GPIO_InitStruct.Pin = EX_SPI_NSS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(EX_SPI_NSS_GPIO_Port, &GPIO_InitStruct);
}

void MX_GPIO_PA8_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, SW_SEL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA8 */
  GPIO_InitStruct.Pin = SW_SEL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void MX_GPIO_PA0_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pins : PA0 */
  GPIO_InitStruct.Pin = CHRG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void MX_GPIO_PD0_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, C_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PD0 */
  GPIO_InitStruct.Pin = C_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}
#endif
/* USER CODE END 2 */