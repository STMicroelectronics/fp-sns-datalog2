/**
  ******************************************************************************
  * @file    AiUSC_DPU.h
  * @author  STMicroelectronics - AIS - MCD Team
  * @brief   Digital processing Unit specialized for the Cube.AI library
  *
  * This DPU process the data using Ai library generated by Cube.AI.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef DPU_INC_AI_USC_DPU_H_
#define DPU_INC_AI_USC_DPU_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "ADPU2.h"
#include "ADPU2_vtbl.h"
#include "aiApp.h"


#ifndef AI_USC_DPU_CFG_NAME
#define AI_USC_DPU_CFG_NAME             "usc_network"
#endif

/**
 * Create  type name for _AiUSC_DPU_t.
 */
typedef struct _AiUSC_DPU_t AiUSC_DPU_t;

/**
 * AiUSC_DPU_t internal state.
 * It declares only the virtual table used to implement the inheritance.
 */
struct _AiUSC_DPU_t {
  /**
   * Base class object.
   */
  ADPU2_t super;

  /**
   * Specifies AI processing function to use in order to process the signals.
   */
  int (*ai_processing_f)(const char*, float *, float[2]);

};


/* Public API declaration */
/**************************/

/**
 * Allocate an instance of AiUSC_DPU_t in the heap.
 *
 * @return a pointer to the generic object ::IDPU2_t if success,
 * or NULL if out of memory error occurs.
 */
IDPU2_t *AiUSC_DPUAlloc(void);

/**
 * Allocate an instance of AiUSC_DPU_t in a memory block specified by the application.
 * The size of the memory block must be greater or equal to sizeof(AiUSC_DPU_t).
 * This allocator allows the application to avoid the dynamic allocation.
 *
 * \code
 * AiUSC_DPU_t dpu;
 * AiUSC_DPUStaticAlloc(&dpu);
 * \endcode
 *
 * @param p_mem_block [IN] specify a memory block allocated by the application.
 *        The size of the memory block must be greater or equal to sizeof(AiUSC_DPU_t).
 * @return a pointer to the generic object ::IDPU if success,
 * or NULL if out of memory error occurs.
 */
IDPU2_t *AiUSC_DPUStaticAlloc(void *p_mem_block);

/**
 * Initialize the DPU.
 *
 * @param _this [IN] specifies an object instance.
 * @param mfcc_rows [IN]
 * @param mfcc_columns [IN]
 * @return SYS_NO_ERROR_CODE if success, an application specific error code otherwise.
 */
sys_error_code_t AiUSC_DPUInit(AiUSC_DPU_t *_this, uint16_t mfcc_rows, uint16_t mfcc_columns);

/**
 * Partial reset of the DPU internal state: all input and output buffers are re-initialized to prepare
 * the DPU to process a new stream of data.
 *
 * @param _this [IN] specifies a pointer to the object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
sys_error_code_t AiUSC_DPUPrepareToProcessData(AiUSC_DPU_t *_this);


/* Inline functions definition */
/*******************************/

#ifdef __cplusplus
}
#endif

#endif /* DPU_INC_AiUSC_DPU_H_ */
