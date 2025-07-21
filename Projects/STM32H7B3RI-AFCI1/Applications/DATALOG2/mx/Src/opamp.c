/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    opamp.c
  * @brief   This file provides code for the configuration
  *          of the OPAMP instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "opamp.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

OPAMP_HandleTypeDef hopamp1;

/* OPAMP1 init function */
void MX_OPAMP1_Init(void)
{

  /* USER CODE BEGIN OPAMP1_Init 0 */

  /* USER CODE END OPAMP1_Init 0 */

  /* USER CODE BEGIN OPAMP1_Init 1 */

  /* USER CODE END OPAMP1_Init 1 */
  hopamp1.Instance = OPAMP1;
  hopamp1.Init.Mode = OPAMP_STANDALONE_MODE;
  hopamp1.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO0;
  hopamp1.Init.InvertingInput = OPAMP_INVERTINGINPUT_IO0;
  hopamp1.Init.PowerMode = OPAMP_POWERMODE_NORMAL;
  hopamp1.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
  if (HAL_OPAMP_Init(&hopamp1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OPAMP1_Init 2 */

  /* USER CODE END OPAMP1_Init 2 */

}

void HAL_OPAMP_MspInit(OPAMP_HandleTypeDef *opampHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (opampHandle->Instance == OPAMP1)
  {
    /* USER CODE BEGIN OPAMP1_MspInit 0 */

    /* USER CODE END OPAMP1_MspInit 0 */
    /* OPAMP1 clock enable */
    __HAL_RCC_OPAMP_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**OPAMP1 GPIO Configuration
    PC4     ------> OPAMP1_VOUT
    PC5     ------> OPAMP1_VINM
    PB0     ------> OPAMP1_VINP
      */
    GPIO_InitStruct.Pin = H7_OPOUT_Pin | H7_OPVN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = H7_CH1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(H7_CH1_GPIO_Port, &GPIO_InitStruct);

    /* USER CODE BEGIN OPAMP1_MspInit 1 */

    /* USER CODE END OPAMP1_MspInit 1 */
  }
}

void HAL_OPAMP_MspDeInit(OPAMP_HandleTypeDef *opampHandle)
{

  if (opampHandle->Instance == OPAMP1)
  {
    /* USER CODE BEGIN OPAMP1_MspDeInit 0 */

    /* USER CODE END OPAMP1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_OPAMP_CLK_DISABLE();

    /**OPAMP1 GPIO Configuration
    PC4     ------> OPAMP1_VOUT
    PC5     ------> OPAMP1_VINM
    PB0     ------> OPAMP1_VINP
      */
    HAL_GPIO_DeInit(GPIOC, H7_OPOUT_Pin | H7_OPVN_Pin);

    HAL_GPIO_DeInit(H7_CH1_GPIO_Port, H7_CH1_Pin);

    /* USER CODE BEGIN OPAMP1_MspDeInit 1 */

    /* USER CODE END OPAMP1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
