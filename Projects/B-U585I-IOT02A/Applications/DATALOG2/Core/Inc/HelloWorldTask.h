/**
  ******************************************************************************
  * @file    HelloWorldTask.h
  * @author  SRA
  *
  * @brief Display a message in the debug log.
  *
  * This managed task is responsible to display a greeting message
  * in the debug log with a period of 1 second.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef HELLOWORLDTASK_H_
#define HELLOWORLDTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "services/AManagedTask.h"
#include "services/AManagedTask_vtbl.h"


/**
  * Create  type name for _HelloWorldTask.
  */
typedef struct _HelloWorldTask HelloWorldTask_t;

/**
  *  HelloWorldTask internal structure.
  */
struct _HelloWorldTask
{
  /**
    * Base class object.
    */
  AManagedTask super;

  // Task variables should be added here.

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
  * Allocate an instance of HelloWorldTask in the framework heap.
  *
  * @param pLEDConfigMX [IN] specifies the configuration parameters for a LED. NULL if the LED is not used.
  * @param pUBConfigMX [IN] specifies the configuration parameters for an user button. NULL if the user button is not used.
  * @return a pointer to the generic object ::AManagedTask if success,
  * or NULL if out of memory error occurs.
  */
AManagedTask *HelloWorldTaskAlloc(const void *pLEDConfigMX, const void *pUBConfigMX);

/**
  * Allocate an instance of ::HelloWorldTask_t in a memory block specified by the application.
  * The size of the memory block must be greater or equal to `sizeof(HelloWorldTask_t)`.
  * This allocator allows the application to avoid the dynamic allocation.
  *
  * \code
  * HelloWorldTask_t my_task;
  * HelloWorldTaskStaticAlloc(&my_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(HelloWorldTask_t)`.
  * @param pLEDConfigMX [IN] specifies the configuration parameters for a LED. NULL if the LED is not used.
  * @param pUBConfigMX [IN] specifies the configuration parameters for an user button. NULL if the user button is not used.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTask *HelloWorldTaskStaticAlloc(void *p_mem_block, const void *pLEDConfigMX, const void *pUBConfigMX);


/* Inline functions definition */
/*******************************/


#ifdef __cplusplus
}
#endif

#endif /* HELLOWORLDTASK_H_ */
