/**
 ******************************************************************************
 * @file    STWIN.box_debug_pins.c
 * @author  SRA
 * @brief   This file provides low level functionalities for STWIN.box board
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
#include "STWIN.box_debug_pins.h"

/* Private Definition --------------------------------------------------------*/

/** @addtogroup BSP
* @{
*/ 

/** @addtogroup STWIN.box
* @{
*/

/** @defgroup STWIN.box_LOW_LEVEL_Private_Variables STWIN.box_LOW_LEVEL Private Variables 
* @{
*/
const uint16_t  DEBUG_PIN[DEBUG_PIN_MAX_NUM] = {DEBUG_CON34_PIN_5_GPIO_PIN,
                                                DEBUG_CON34_PIN_7_GPIO_PIN,
                                                DEBUG_CON34_PIN_8_GPIO_PIN,
                                                DEBUG_CON34_PIN_9_GPIO_PIN,
                                                DEBUG_CON34_PIN_10_GPIO_PIN,
                                                DEBUG_CON34_PIN_11_GPIO_PIN,
                                                DEBUG_CON34_PIN_12_GPIO_PIN,
                                                DEBUG_CON34_PIN_13_GPIO_PIN,
                                                DEBUG_CON34_PIN_14_GPIO_PIN,
                                                DEBUG_CON34_PIN_16_GPIO_PIN,
                                                DEBUG_CON34_PIN_17_GPIO_PIN,
                                                DEBUG_CON34_PIN_18_GPIO_PIN,
                                                DEBUG_CON34_PIN_19_GPIO_PIN,
                                                DEBUG_CON34_PIN_21_GPIO_PIN,
                                                DEBUG_CON34_PIN_22_GPIO_PIN,
                                                DEBUG_CON34_PIN_23_GPIO_PIN,
                                                DEBUG_CON34_PIN_24_GPIO_PIN,
                                                DEBUG_CON34_PIN_25_GPIO_PIN,
                                                DEBUG_CON34_PIN_26_GPIO_PIN,
                                                DEBUG_CON34_PIN_27_GPIO_PIN,
                                                DEBUG_CON34_PIN_28_GPIO_PIN,
                                                DEBUG_CON34_PIN_30_GPIO_PIN};

GPIO_TypeDef  *DEBUG_PIN_PORT[DEBUG_PIN_MAX_NUM] =   {DEBUG_CON34_PIN_5_GPIO_PORT,
                                                      DEBUG_CON34_PIN_7_GPIO_PORT,
                                                      DEBUG_CON34_PIN_8_GPIO_PORT,
                                                      DEBUG_CON34_PIN_9_GPIO_PORT,
                                                      DEBUG_CON34_PIN_10_GPIO_PORT,
                                                      DEBUG_CON34_PIN_11_GPIO_PORT,
                                                      DEBUG_CON34_PIN_12_GPIO_PORT,
                                                      DEBUG_CON34_PIN_13_GPIO_PORT,
                                                      DEBUG_CON34_PIN_14_GPIO_PORT,
                                                      DEBUG_CON34_PIN_16_GPIO_PORT,
                                                      DEBUG_CON34_PIN_17_GPIO_PORT,
                                                      DEBUG_CON34_PIN_18_GPIO_PORT,
                                                      DEBUG_CON34_PIN_19_GPIO_PORT,
                                                      DEBUG_CON34_PIN_21_GPIO_PORT,
                                                      DEBUG_CON34_PIN_22_GPIO_PORT,
                                                      DEBUG_CON34_PIN_23_GPIO_PORT,
                                                      DEBUG_CON34_PIN_24_GPIO_PORT,
                                                      DEBUG_CON34_PIN_25_GPIO_PORT,
                                                      DEBUG_CON34_PIN_26_GPIO_PORT,
                                                      DEBUG_CON34_PIN_27_GPIO_PORT,
                                                      DEBUG_CON34_PIN_28_GPIO_PORT,
                                                      DEBUG_CON34_PIN_30_GPIO_PORT};
                                     
/**
* @}
*/


/** @defgroup STWIN.box_LOW_LEVEL_Exported_Functions STWIN.box_LOW_LEVEL Exported Functions
* @{
*/

/**
* @brief  Configure general purpose debug pins on STWIN.box DIL24 connector
*         Pin name also identifies the pin number on DIL24.
* @param  None
* @retval None
*/  
void BSP_DEBUG_PIN_Init_All(void)
{
  BSP_DEBUG_PIN_Off( CON34_PIN_5 );
  BSP_DEBUG_PIN_Off( CON34_PIN_7 );
  BSP_DEBUG_PIN_Off( CON34_PIN_8 );
  BSP_DEBUG_PIN_Off( CON34_PIN_9 );
  BSP_DEBUG_PIN_Off( CON34_PIN_10 );
  BSP_DEBUG_PIN_Off( CON34_PIN_11 );
  BSP_DEBUG_PIN_Off( CON34_PIN_12 );
  BSP_DEBUG_PIN_Off( CON34_PIN_13 );
  BSP_DEBUG_PIN_Off( CON34_PIN_14 );
  BSP_DEBUG_PIN_Off( CON34_PIN_16 );
  BSP_DEBUG_PIN_Off( CON34_PIN_17 );
  BSP_DEBUG_PIN_Off( CON34_PIN_18 );
  BSP_DEBUG_PIN_Off( CON34_PIN_19 );
  BSP_DEBUG_PIN_Off( CON34_PIN_21 );
  BSP_DEBUG_PIN_Off( CON34_PIN_22 );
  BSP_DEBUG_PIN_Off( CON34_PIN_23 );
  BSP_DEBUG_PIN_Off( CON34_PIN_24 );
  BSP_DEBUG_PIN_Off( CON34_PIN_25 );
  BSP_DEBUG_PIN_Off( CON34_PIN_26 );
  BSP_DEBUG_PIN_Off( CON34_PIN_27 );
  BSP_DEBUG_PIN_Off( CON34_PIN_28 );
  BSP_DEBUG_PIN_Off( CON34_PIN_30 );

  BSP_DEBUG_PIN_Init( CON34_PIN_5 );
  BSP_DEBUG_PIN_Init( CON34_PIN_7 );
  BSP_DEBUG_PIN_Init( CON34_PIN_8 );
  BSP_DEBUG_PIN_Init( CON34_PIN_9 );
  BSP_DEBUG_PIN_Init( CON34_PIN_10 );
  BSP_DEBUG_PIN_Init( CON34_PIN_11 );
  BSP_DEBUG_PIN_Init( CON34_PIN_12 );
  BSP_DEBUG_PIN_Init( CON34_PIN_13 );
  BSP_DEBUG_PIN_Init( CON34_PIN_14 );
  BSP_DEBUG_PIN_Init( CON34_PIN_16 );
  BSP_DEBUG_PIN_Init( CON34_PIN_17 );
  BSP_DEBUG_PIN_Init( CON34_PIN_18 );
  BSP_DEBUG_PIN_Init( CON34_PIN_19 );
  BSP_DEBUG_PIN_Init( CON34_PIN_21 );
  BSP_DEBUG_PIN_Init( CON34_PIN_22 );
  BSP_DEBUG_PIN_Init( CON34_PIN_23 );
  BSP_DEBUG_PIN_Init( CON34_PIN_24 );
  BSP_DEBUG_PIN_Init( CON34_PIN_25 );
  BSP_DEBUG_PIN_Init( CON34_PIN_26 );
  BSP_DEBUG_PIN_Init( CON34_PIN_27 );
  BSP_DEBUG_PIN_Init( CON34_PIN_28 );
  BSP_DEBUG_PIN_Init( CON34_PIN_30 );
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

