/**
  ******************************************************************************
  * @file    syscs_threadx.h
  * @author  STMicroelectronics - AIS - MCD Team
  * @version M.m.b
  * @date    June 16, 2022
  *
  * @brief   eLooM critical section.
  *
  * This service provides a critical section with an API independent from
  * the RTOS.
  * For ThreadX we use the API tx_interrupt_control.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */
#ifndef ELOOM_INC_SERVICES_SYSCS_FREERTOS_H_
#define ELOOM_INC_SERVICES_SYSCS_FREERTOS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* MISRA messages linked to FreeRTOS include are ignored */
/*cstat -MISRAC2012-* */
#include "tx_api.h"
/*cstat +MISRAC2012-* */


/* Public API declaration */
/**************************/

#define SYS_DECLARE_CS_IMP(cs)                     UINT cs  /**< Used to declare a critical section (cs) object. */

#define SYS_ENTER_CRITICAL_IMP(cs)                 cs = tx_interrupt_control(TX_INT_DISABLE)  /**< Used to enter a critical section. */

#define SYS_EXIT_CRITICAL_IMP(cs)                  tx_interrupt_control(cs) /**< Used to exit a critical section. */


/* Inline functions definition */
/*******************************/


#ifdef __cplusplus
}
#endif

#endif /* ELOOM_INC_SERVICES_SYSCS_FREERTOS_H_ */
