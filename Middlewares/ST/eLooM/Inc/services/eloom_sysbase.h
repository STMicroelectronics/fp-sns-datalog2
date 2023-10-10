/**
  ******************************************************************************
  * @file    eloom_sysbase.h
  * @author  STMicroelectronics - AIS - MCD Team
  *
  * @brief   Umbrella header for the basic system features.
  *
  * This file include the basic system features API, that are the features
  * exported by the low layer of eLooM framework:
  * - Dynamic memory allocation in the framework heap
  * - Critical section implementation based on the IRQ disabling and enabling.
  * - STM32 target platform definition
  * - Error definition
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#ifndef ST_ELOOM_INC_SERVICES_ELOOM_SYSBASE_H_
#define ST_ELOOM_INC_SERVICES_ELOOM_SYSBASE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/systp.h"
#include "services/systypes.h"
#include "services/sysmem.h"
#include "services/syscs.h"
#include "services/syserror.h"

#ifdef __cplusplus
}
#endif

#endif /* ST_ELOOM_INC_SERVICES_ELOOM_SYSBASE_H_ */
