/**
  ******************************************************************************
  * @file    STWIN_debug_pins.c
  * @author  SRA
  *
  *
  * @brief   This file provides low level functionalities for STWIN board
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
#include "STWIN_debug_pins.h"


/** @addtogroup BSP
  * @{
  */

/** @addtogroup STWIN
  * @{
  */

/** @defgroup STWIN_LOW_LEVEL_Private_Variables STWIN_LOW_LEVEL Private Variables
  * @{
  */
static const uint16_t  DEBUG_PIN[DEBUG_PINn] =  {DEBUG_PIN_7_PIN,
                                                 DEBUG_PIN_8_PIN,
                                                 DEBUG_PIN_9_PIN,
                                                 DEBUG_PIN_10_PIN,
                                                 DEBUG_PIN_11_PIN,
                                                 DEBUG_PIN_12_PIN,
                                                 DEBUG_PIN_13_PIN,
                                                 DEBUG_PIN_14_PIN,
                                                 DEBUG_PIN_17_PIN,
                                                 DEBUG_PIN_18_PIN,
                                                 DEBUG_PIN_19_PIN,
                                                 DEBUG_PIN_20_PIN
                                                };

static GPIO_TypeDef  *DEBUG_PIN_PORT[DEBUG_PINn] =   {DEBUG_PIN_7_GPIO_PORT,
                                                      DEBUG_PIN_8_GPIO_PORT,
                                                      DEBUG_PIN_9_GPIO_PORT,
                                                      DEBUG_PIN_10_GPIO_PORT,
                                                      DEBUG_PIN_11_GPIO_PORT,
                                                      DEBUG_PIN_12_GPIO_PORT,
                                                      DEBUG_PIN_13_GPIO_PORT,
                                                      DEBUG_PIN_14_GPIO_PORT,
                                                      DEBUG_PIN_17_GPIO_PORT,
                                                      DEBUG_PIN_18_GPIO_PORT,
                                                      DEBUG_PIN_19_GPIO_PORT,
                                                      DEBUG_PIN_20_GPIO_PORT
                                                     };

/**
  * @}
  */


/** @defgroup STWIN_LOW_LEVEL_Exported_Functions STWIN_LOW_LEVEL Exported Functions
  * @{
  */

/**
  * @brief  Configure general purpose debug pins on STWIN STMOD+ connector
  *         Pin name also identifies the pin number on STMOD+.
  * @param  None
  * @retval None
  */
void BSP_DEBUG_PIN_Init_All(void)
{
  BSP_DEBUG_PIN_Off(DEBUG_PIN7);
  BSP_DEBUG_PIN_Off(DEBUG_PIN8);
  BSP_DEBUG_PIN_Off(DEBUG_PIN9);
  BSP_DEBUG_PIN_Off(DEBUG_PIN10);
  BSP_DEBUG_PIN_Off(DEBUG_PIN11);
  BSP_DEBUG_PIN_Off(DEBUG_PIN12);
  BSP_DEBUG_PIN_Off(DEBUG_PIN13);
  BSP_DEBUG_PIN_Off(DEBUG_PIN14);
  BSP_DEBUG_PIN_Off(DEBUG_PIN17);
  BSP_DEBUG_PIN_Off(DEBUG_PIN18);
  BSP_DEBUG_PIN_Off(DEBUG_PIN19);
  BSP_DEBUG_PIN_Off(DEBUG_PIN20);

  BSP_DEBUG_PIN_Init(DEBUG_PIN7);
  BSP_DEBUG_PIN_Init(DEBUG_PIN8);
  BSP_DEBUG_PIN_Init(DEBUG_PIN9);
  BSP_DEBUG_PIN_Init(DEBUG_PIN10);
  BSP_DEBUG_PIN_Init(DEBUG_PIN11);
  BSP_DEBUG_PIN_Init(DEBUG_PIN12);
  BSP_DEBUG_PIN_Init(DEBUG_PIN13);
  BSP_DEBUG_PIN_Init(DEBUG_PIN14);
  BSP_DEBUG_PIN_Init(DEBUG_PIN17);
  BSP_DEBUG_PIN_Init(DEBUG_PIN18);
  BSP_DEBUG_PIN_Init(DEBUG_PIN19);
  BSP_DEBUG_PIN_Init(DEBUG_PIN20);
}


void BSP_DEBUG_PIN_Init(Debug_Pin_TypeDef Pin)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Enable the GPIO clock */
  DEBUG_PINx_GPIO_CLK_ENABLE(Pin);

  /* Configure the GPIO pin */
  GPIO_InitStructure.Pin = DEBUG_PIN[Pin];
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  HAL_GPIO_WritePin(DEBUG_PIN_PORT[Pin], DEBUG_PIN[Pin], GPIO_PIN_RESET);

  HAL_GPIO_Init(DEBUG_PIN_PORT[Pin], &GPIO_InitStructure);
}

void  BSP_DEBUG_PIN_DeInit(Debug_Pin_TypeDef Pin)
{
  HAL_GPIO_DeInit(DEBUG_PIN_PORT[Pin], DEBUG_PIN[Pin]);
  DEBUG_PINx_GPIO_CLK_DISABLE(Pin);
}

void BSP_DEBUG_PIN_On(Debug_Pin_TypeDef Pin)
{
  HAL_GPIO_WritePin(DEBUG_PIN_PORT[Pin], DEBUG_PIN[Pin], GPIO_PIN_SET);
}

void BSP_DEBUG_PIN_Off(Debug_Pin_TypeDef Pin)
{
  HAL_GPIO_WritePin(DEBUG_PIN_PORT[Pin], DEBUG_PIN[Pin], GPIO_PIN_RESET);
}

void BSP_DEBUG_PIN_Toggle(Debug_Pin_TypeDef Pin)
{
  HAL_GPIO_TogglePin(DEBUG_PIN_PORT[Pin], DEBUG_PIN[Pin]);
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


