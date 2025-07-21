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
#ifndef INC_UTILTASK_H_
#define INC_UTILTASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/AManagedTaskEx.h"
#include "services/AManagedTaskEx_vtbl.h"
#include "tx_api.h"

#define UTIL_CMD_ID_START_LP_TIMER                    ((uint16_t)0x0001)              ///< START Low Power timer command ID.
#define UTIL_CMD_ID_STOP_LP_TIMER                     ((uint16_t)0x0002)              ///< STOP Low Power timer command ID.
#define UTIL_CMD_ID_RESET_LP_TIMER                    ((uint16_t)0x0003)              ///< RESET Low Power timer command ID.

/**
  * Create  type name for _UtilTask_t.
  */
typedef struct _UtilTask_t UtilTask_t;

/**
  *  UtilTask_t internal structure.
  */
struct _UtilTask_t
{
  /**
    * Base class object.
    */
  AManagedTaskEx super;

  /* Task variables should be added here. */

  /**
    * HAL driver configuration parameters.
    */
  const void *p_mx_drv_cfg;

  /**
    * Input queue used by other task to request services.
    */
  TX_QUEUE in_queue;

  /**
    * Software timer used to generate a transaction into low-power mode.
    */
  TX_TIMER auto_low_power_timer;
};

/* Public API declaration */
/**************************/

/**
  * Allocate an instance of UtilTask.
  *
  * @param p_mx_drv_cfg [IN] specifies a ::MX_TIMParams_t instance declared in the mx.h file.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *UtilTaskAlloc(const void *p_mx_drv_cfg);

/* Inline functions definition */
/*******************************/

#ifdef __cplusplus
}
#endif

#endif /* INC_UTILTASK_H_ */
