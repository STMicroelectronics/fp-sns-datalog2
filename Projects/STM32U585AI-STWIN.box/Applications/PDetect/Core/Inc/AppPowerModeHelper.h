/**
  ******************************************************************************
  * @file    AppPowerModeHelper.h
  * @author  SRA
  * @brief   It implements the Power Mode State Machine.
  *
  * This object implements the IAppPowerModeHelper IF. The state machine has
  * two states as described in the section \ref tab_s4_power_management
  * "Power Management"
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

#ifndef APPPOWERMODEHELPER_H_
#define APPPOWERMODEHELPER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/IAppPowerModeHelper.h"
#include "services/IAppPowerModeHelper_vtbl.h"

/**
  * Create  type name for _AppPowermodeHelper.
  */
typedef struct _AppPowerModeHelper AppPowerModeHelper;

// Public API declaration
//***********************

/**
  * Allocate an instance of AppPowerModeHelper. It is allocated in the FreeRTOS heap.
  *
  * @return a pointer to the generic interface ::IApplicationErrorDelegate if success,
  * or SYS_OUT_OF_MEMORY_ERROR_CODE otherwise.
  */
IAppPowerModeHelper *AppPowerModeHelperAlloc();

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* APPPOWERMODEHELPER_H_ */
