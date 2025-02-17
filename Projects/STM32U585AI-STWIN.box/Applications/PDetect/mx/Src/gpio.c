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
  HAL_GPIO_WritePin(GPIOE, EN_D1V8_Pin | EN_A3V3_Pin | BLE_SPI_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, CS_DHCX_Pin | CS_DLPC_Pin | WIFI_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, EX_SPI_NSS_Pin | CS_ICLX_Pin | EN_A1V8_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TOF_LPn_GPIO_Port, TOF_LPn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIO2_EX_GPIO_Port, GPIO2_EX_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, GPIO3_EX_Pin | CS_DWB_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIO1_EX_GPIO_Port, GPIO1_EX_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : EN_D1V8_Pin EN_A3V3_Pin BLE_SPI_CS_Pin TOF_LPn_Pin */
  GPIO_InitStruct.Pin = EN_D1V8_Pin | EN_A3V3_Pin | BLE_SPI_CS_Pin | TOF_LPn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PI6 DCDC_2_EN_Pin PI4 */
  GPIO_InitStruct.Pin = GPIO_PIN_6 | DCDC_2_EN_Pin | GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pins : PA15 PA10 SW_SEL_Pin PA9
                           PA7 PA5 PA1 PA2
                           PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_10 | SW_SEL_Pin | GPIO_PIN_9
                        | GPIO_PIN_7 | GPIO_PIN_5 | GPIO_PIN_1 | GPIO_PIN_2
                        | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : CS_DHCX_Pin CS_DLPC_Pin WIFI_CS_Pin */
  GPIO_InitStruct.Pin = CS_DHCX_Pin | CS_DLPC_Pin | WIFI_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : C_EN_Pin PD7 PD1 PD14
                           INT_EXD15_Pin PD12 BLE_RST_Pin PD8
                           WIFI_BOOT_Pin PD11 */
  GPIO_InitStruct.Pin = C_EN_Pin | GPIO_PIN_7 | GPIO_PIN_1 | GPIO_PIN_14
                        | INT_EXD15_Pin | GPIO_PIN_12 | BLE_RST_Pin | GPIO_PIN_8
                        | WIFI_BOOT_Pin | GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : EX_SPI_NSS_Pin CS_ICLX_Pin EN_A1V8_Pin */
  GPIO_InitStruct.Pin = EX_SPI_NSS_Pin | CS_ICLX_Pin | EN_A1V8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pins : LED1_Pin BOOT0_Pin PH14 PH13
                           LED2_Pin PH11 PH8 STSAFE_RESET_Pin
                           PH5 PH2 */
  GPIO_InitStruct.Pin = LED1_Pin | BOOT0_Pin | GPIO_PIN_14 | GPIO_PIN_13
                        | LED2_Pin | GPIO_PIN_11 | GPIO_PIN_8 | STSAFE_RESET_Pin
                        | GPIO_PIN_5 | GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PG10 PG12 SD_DETECT_Pin PG0 */
  GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_12 | SD_DETECT_Pin | GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : PE3 PE6 PE8 PE14
                           PE13 PE11 LDO_EN_Pin WIFI_EN_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_14
                        | GPIO_PIN_13 | GPIO_PIN_11 | LDO_EN_Pin | WIFI_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : USR_BUTTON_Pin WIFI_NOTIFY_Pin */
  GPIO_InitStruct.Pin = USR_BUTTON_Pin | WIFI_NOTIFY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PC13 PC7 PC6 PC4
                           PC5 */
  GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_4
                        | GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : INT1_DHCX_Pin NFC_INT_Pin */
  GPIO_InitStruct.Pin = INT1_DHCX_Pin | NFC_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIO2_EX_Pin GPIO3_EX_Pin CS_DWB_Pin */
  GPIO_InitStruct.Pin = GPIO2_EX_Pin | GPIO3_EX_Pin | CS_DWB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : INT1_DLPC_Pin INT2_DLPC_Pin INT_MAG_Pin INT2_DHCX_Pin
                           BLE_INT_Pin INT2_ICLX_Pin INT1_DWB_Pin */
  GPIO_InitStruct.Pin = INT1_DLPC_Pin | INT2_DLPC_Pin | INT_MAG_Pin | INT2_DHCX_Pin
                        | BLE_INT_Pin | INT2_ICLX_Pin | INT1_DWB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : BLE_TEST9_Pin PF5 PF3 BLE_TEST8_Pin
                           PF13 */
  GPIO_InitStruct.Pin = BLE_TEST9_Pin | GPIO_PIN_5 | GPIO_PIN_3 | BLE_TEST8_Pin
                        | GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : INT_EX_Pin INT_TMOS_Pin */
  GPIO_InitStruct.Pin = INT_EX_Pin | INT_TMOS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : INT_TOF_Pin */
  GPIO_InitStruct.Pin = INT_TOF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INT_TOF_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB10 PB2 PB11
                           PB12 PB15 PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_10 | GPIO_PIN_2 | GPIO_PIN_11
                        | GPIO_PIN_12 | GPIO_PIN_15 | GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTTON_PWR_Pin */
  GPIO_InitStruct.Pin = BUTTON_PWR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUTTON_PWR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : GPIO1_EX_Pin */
  GPIO_InitStruct.Pin = GPIO1_EX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIO1_EX_GPIO_Port, &GPIO_InitStruct);

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
void MX_GPIO_PE0_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin : PE0Pin */
  GPIO_InitStruct.Pin = USR_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USR_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 15, 0);
//  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void MX_GPIO_PH12_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin : PH12Pin */
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);
}

void MX_GPIO_PH10_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin : PH10Pin */
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);
}

//void MX_GPIO_PB1_Init(void)
//{
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//
//  /* GPIO Ports Clock Enable */
//  __HAL_RCC_GPIOB_CLK_ENABLE();
//
//  /*Configure GPIO pin : PB1Pin */
//  GPIO_InitStruct.Pin = GPIO_PIN_1;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//  /* EXTI interrupt init*/
//  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
//  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
//}

void MX_GPIO_PG6_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Enable VddIO2 - Needed for GPIOG */
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

void MX_GPIO_PF9_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /*Configure GPIO pin : PF9Pin */
  GPIO_InitStruct.Pin = INT_MAG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INT_MAG_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT_MAG_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(INT_MAG_EXTI_IRQn);
}

void MX_GPIO_PI7_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOI_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_ICLX_GPIO_Port, CS_ICLX_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PI7 */
  GPIO_InitStruct.Pin = CS_ICLX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(CS_ICLX_GPIO_Port, &GPIO_InitStruct);
}

void MX_GPIO_PF2_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /*Configure GPIO pin : PF1Pin */
  GPIO_InitStruct.Pin = INT2_DLPC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INT2_DLPC_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT2_DLPC_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(INT2_DLPC_EXTI_IRQn);
}

void MX_GPIO_PH6_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
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

void MX_GPIO_PI0_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
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

void MX_GPIO_PE7_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pins : PE7 */
  GPIO_InitStruct.Pin = WIFI_NOTIFY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(WIFI_NOTIFY_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(WIFI_NOTIFY_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(WIFI_NOTIFY_EXTI_IRQn);
}

void MX_GPIO_PH7_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, WIFI_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PD0 */
  GPIO_InitStruct.Pin = WIFI_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(WIFI_CS_GPIO_Port, &GPIO_InitStruct);
}

void MX_GPIO_PE12_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, WIFI_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE12 */
  GPIO_InitStruct.Pin = WIFI_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(WIFI_EN_GPIO_Port, &GPIO_InitStruct);
}

void MX_GPIO_PB1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIO1_EX_GPIO_Port, GPIO1_EX_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB1 */
  GPIO_InitStruct.Pin = GPIO1_EX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIO1_EX_GPIO_Port, &GPIO_InitStruct);
}

void MX_GPIO_PF8_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIO2_EX_GPIO_Port, GPIO2_EX_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PF8 */
  GPIO_InitStruct.Pin = GPIO2_EX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIO2_EX_GPIO_Port, &GPIO_InitStruct);
}

void MX_GPIO_PF10_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIO3_EX_GPIO_Port, GPIO3_EX_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PF10 */
  GPIO_InitStruct.Pin = GPIO3_EX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIO3_EX_GPIO_Port, &GPIO_InitStruct);
}

void MX_GPIO_PG5_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Enable VddIO2 - Needed for GPIOG */
  HAL_PWREx_EnableVddIO2();

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin : PG5Pin */
  GPIO_InitStruct.Pin = INT_TMOS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INT_TMOS_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT_TMOS_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(INT_TMOS_EXTI_IRQn);
}

void MX_GPIO_PC3_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin : PC3Pin */
  GPIO_InitStruct.Pin = INT_TOF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INT_TOF_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(INT_TOF_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(INT_TOF_EXTI_IRQn);
}

/* USER CODE END 2 */
