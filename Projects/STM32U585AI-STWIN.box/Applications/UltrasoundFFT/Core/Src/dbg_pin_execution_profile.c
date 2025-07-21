/**
  ******************************************************************************
  * @file    dbg_pin_execution_profile.c
  * @author  STMicroelectronics
  * @author  SRA
  * @brief
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

#ifdef ENABLE_THREADX_DBG_PIN

/* Include */
/***********/
#include "dbg_pin_execution_profile.h"
#include "STWIN.box_debug_pins.h"
#include "tx_api.h"
#include "tx_thread.h"

/* Private variable declaration */
/********************************/
static TX_THREAD *thread_ptr;

/* Public function definition */
/******************************/
/**
  * @brief  _tx_execution_thread_enter definition.
  *         This function will be called in the PendSV during context switch
  * @param  None
  * @retval None
  */
void _tx_execution_thread_enter(void)
{
  TX_THREAD_GET_CURRENT(thread_ptr);

  DEBUG_PIN_PORT[thread_ptr->pxTaskTag]->BSRR = (uint32_t) DEBUG_PIN[thread_ptr->pxTaskTag];
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
    DEBUG_PIN_PORT[thread_ptr->pxTaskTag]->BRR = (uint32_t) DEBUG_PIN[thread_ptr->pxTaskTag];
  }
}
#endif
