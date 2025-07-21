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
    * Simple counter displayed in the debug log message
    */
  uint32_t counter;

  /**
    * Specifies the time stamp of the button down event. It is used to compute the short or log button pressed event.
    */
  uint32_t button_down_ts_tick;

  /**
    * Configuration parameters for a LED. NUL if the LED is not used.
    */
  const void *pLEDConfigMX;

  /**
    * Configuration parameters for an user button. NUL if the user button is not used.
    */
  const void *pUBConfigMX;
};

/* Public API declaration */
/**************************/

/**
  * Allocate an instance of UtilTask.
  *
  * @param pLEDConfigMX [IN] specifies the configuration parameters for a LED. NULL if the LED is not used.
  * @param pUBConfigMX [IN] specifies the configuration parameters for an user button. NULL if the user button is not used.
  * @return a pointer to the generic object ::AManagedTask if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *UtilTaskAlloc(const void *pLEDConfigMX, const void *pUBConfigMX);


/* Inline functions definition */
/*******************************/

#ifdef __cplusplus
}
#endif

#endif /* INC_UTILTASK_H_ */
