/**
 ******************************************************************************
 * @file    UtilTask.h
 * @author  SRA
 * @brief  Utility task declaration.
 *
 * The Utility task is in export minor functionalities of the application:
 * - Time stamp for the Sensor Manager
 * - Manage the user button and the user LED1
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
 *                             
 *
 ******************************************************************************
 */
#ifndef INC_UTILTASK_H_
#define INC_UTILTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "services/AManagedTaskEx.h"
#include "services/AManagedTaskEx_vtbl.h"


#define UTIL_CMD_ID_DATALOG_LED                       ((uint16_t)0x0006)              ///< Button event. Toogle led


/**
 * Create  type name for _UtilTask_t.
 */
typedef struct _UtilTask_t UtilTask_t;

/**
 *  UtilTask_t internal structure.
 */
struct _UtilTask_t {
  /**
   * Base class object.
   */
  AManagedTaskEx super;

  /* Task variables should be added here. */

  /**
   * HAL driver configuration parameters. This instance is used to
   * drive the user button 1.
   */
  const void *p_mx_sw1_drv_cfg;

  /**
   * HAL driver configuration parameters. This instance is used to
   * drive the LD1 LED.
   */
  const void *p_mx_led1_drv_cfg;

  /**
   * HAL driver configuration parameters. This instance is used to
   * drive the LD3 LED.
   */
  const void *p_mx_led2_drv_cfg;

  /**
   * HAL driver configuration parameters. This instance is used to
   * drive the LD3 LED.
   */
  const void *p_mx_led3_drv_cfg;

  /**
   * Input queue used by other task to request services.
   */
  TX_QUEUE in_queue;

  /**
   * Software timer used make user led blinking
   */
  TX_TIMER user_led_timer;

};


/* Public API declaration */
/**************************/

/**
 * Allocate an instance of UtilTask.
 *
 * @param p_mx_sm_drv_cfg [IN] specifies a ::MX_TIMParams_t instance declared in the mx.h file.
 * @param p_mx_ld_drv_cfg [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file. a GPIO connected to a LED. It can be NULL.
 * @return a pointer to the generic object ::AManagedTaskEx if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx *UtilTaskAlloc(const void *p_mx_sw2_drv_cfg, const void *p_mx_led1_drv_cfg, const void *p_mx_led2_drv_cfg, const void *p_mx_led3_drv_cfg);


/**
 * Switch memory bank
 *
 *
 * @return error value.
 */
void SwitchBank(void);

/* Inline functions definition */
/*******************************/


#ifdef __cplusplus
}
#endif

#endif /* INC_UTILTASK_H_ */
