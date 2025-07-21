/**
  ******************************************************************************
  * @file    Dummy_DPU2.h
  * @author  STMicroelectronics - AIS - MCD Team
  * @version 2.0.0
  * @date    May 20, 2022
  *
  * @brief   Dummy DPU used as template.
  *
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
  ******************************************************************************
  */

#ifndef DUMMY_DPU2_H_
#define DUMMY_DPU2_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "ADPU2.h"
#include "ADPU2_vtbl.h"


/**
  * Create  type name for _Dummy_DPUTask.
  */
typedef struct _Dummy_DPU2 Dummy_DPU2_t;


/* Public API declaration */
/**************************/

/**
  * Allocate an instance of Dummy_DPU2.
  *
  * @return a pointer to the generic object ::IDPU2 if success,
  *         or NULL if out of memory error occurs.
  */
IDPU2_t *Dummy_DPU2Alloc(void);

/**
  * Initialize the DPU.
  *
  * @param _this [IN] specifies an object instance.
  * @param samples [IN] specifies the number of data sample of the signal.
  * @param axis [IN] specifies the number of axis of the signal.
  * @return SYS_NO_ERROR_CODE if success, an application specific error code otherwise.
  */
sys_error_code_t Dummy_DPU2Init(Dummy_DPU2_t *_this, uint16_t samples, uint8_t axis);


/* Inline functions definition */
/*******************************/

#ifdef __cplusplus
}
#endif

#endif /* DUMMY_DPU2_H_ */
