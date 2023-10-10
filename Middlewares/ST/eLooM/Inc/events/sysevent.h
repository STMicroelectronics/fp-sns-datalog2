/**
  ******************************************************************************
  * @file    sysevent.h
  * @author  STMicroelectronics - ST-Korea - MCD Team
  * @version 3.0.0
  * @date    Aug 7, 2017
  *
  * @brief
  *
  * <DESCRIPTIOM>
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */
#ifndef SYSEVENT_H_
#define SYSEVENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/eloom_sysbase.h"


/**
  * Create a type name for ::_SysEvent.
  */
typedef union _SysEvent SysEvent;

/**
  * Specifies the structure of a system event. A ::SysEvent is used to request a service to the the INIT task.
  */
union _SysEvent
{
  /**
    * Specifies the semantics of the bit field of a system event.
    */
  struct Event
  {
    /**
      * Specifies the source of the event.
      */
    uint32_t nSource: 3;

    /**
      *  Specifies the event parameter.
      */
    uint32_t nParam: 5;

    /**
      * reserved. It must be zero.
      */
    uint32_t reserved: 23;

    /**
      * Specifies the type of the system event. For an error event it must be set to 1.
      */
    uint32_t nEventType: 1;

  } xEvent;

  /**
    * Convenient field to operate the data type.
    */
  uint32_t nRawEvent;
};

// Public API declaration
//***********************


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* SYSEVENT_H_ */
