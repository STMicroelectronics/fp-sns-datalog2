/**
  ******************************************************************************
  * @file    task_execution_profile.c
  * @author  STMicroelectronics
  * @brief   Application Controller Task
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

#ifdef ENABLE_THREADX_DBG_PIN

/* Include */
/***********/
#include "STWIN.box_debug_pins.h"
#include "tx_api.h"
#include "tx_thread.h"
#include "tx_execution_profile.h"

/* Private variable declaration */
/********************************/
static TX_THREAD *thread_ptr;
static uint8_t pxTaskTag;

/* Public function definition */
/******************************/

void _tx_execution_initialize(void)
{

}

/**
  * @brief  _tx_execution_thread_enter definition.
  *         This function will be called in the PendSV during context switch
  * @param  None
  * @retval None
  */
void _tx_execution_thread_enter(void)
{
  TX_THREAD_GET_CURRENT(thread_ptr);

  pxTaskTag = thread_ptr->pxTaskTag;

  DEBUG_PIN_PORT[pxTaskTag]->BSRR = (uint32_t) DEBUG_PIN[pxTaskTag];
}

/**
  * @brief  _tx_execution_thread_exit definition.
  *         This function will be called in the PendSV during context switch
  * @param  None
  * @retval None
  */
void _tx_execution_thread_exit(void)
{
  TX_THREAD_GET_CURRENT(thread_ptr);

  if (thread_ptr)
  {
    pxTaskTag = thread_ptr->pxTaskTag;

    DEBUG_PIN_PORT[pxTaskTag]->BRR = (uint32_t) DEBUG_PIN[pxTaskTag];
  }
}
#endif
