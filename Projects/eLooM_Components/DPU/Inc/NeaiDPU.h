/**
  ******************************************************************************
  * @file    NeaiDPU.h
  * @author  SRA - MCD
  * @brief   Digital processing Unit specialized for the NanoEdgeAI library.
  *
  * This DPU process the data using neai library generated by NanoEdgeAI studio.
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

#ifndef DPU_INC_NEAIDPU_H_
#define DPU_INC_NEAIDPU_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "ADPU2.h"
#include "ADPU2_vtbl.h"
#include "NanoEdgeAI.h"
#include "NanoEdgeAI_ncc.h"


/**
 * Specifies the execution mode for the DPU. The execution mode tells the DPU what to do when it is running
 * and new signals provided by the data source is ready to be processed by the AI library.
 */
typedef enum _ENeaiMode {
  E_NEAI_MODE_NONE = 0,  //!< no execution mode is selected.
  E_NEAI_ANOMALY_LEARN,
  E_NEAI_ANOMALY_DETECT,
  E_NEAI_ONE_CLASS,
  E_NEAI_CLASSIFICATION,
  E_NEAI_EXTRAPOLATION
} ENeaiMode_t;

/**
 * Function pointers to the processing functions generated by NanoEdge AI Studio.
 */
typedef struct
{
  enum neai_state (*anomalyLearn)(float *);
  enum neai_state (*anomalyDetect)(float *,uint8_t *);
  enum neai_state (*oneClass)(float *, uint8_t *);
  enum neai_state (*classification)(float *,float *,uint16_t *);
  enum neai_state (*extrapolation)(float *, float *);
} NEAI_ProcFunction_t;

/**
 * Function pointers to the knowledge initialization functions generated by NanoEdge AI Studio.
 */
typedef struct
{
  enum neai_state (*anomalyInit)(void);
  enum neai_state (*oneClassInit)(const float *);
  enum neai_state (*classificationInit)(const float *);
  enum neai_state (*extrapolationInit)(const float *);
} NEAI_ProcInitFunction_t;



/**
 * Create  type name for _NeaiDPU_t.
 */
typedef struct _NeaiDPU_t NeaiDPU_t;

/**
 * NeaiDPU_t internal state.
 * It declares only the virtual table used to implement the inheritance.
 */
struct _NeaiDPU_t {
  /**
   * Base class object.
   */
  ADPU2_t super;

  /**
   * Specifies the sensitivity of the model in detection mode. It can be tuned at any time without having to go through a new learning phase.
   */
  float sensitivity;

  /**
   * Specifies NanoEdgeAI knowledge initialization and processing function to use in order to process the signals.
   */
  ENeaiMode_t proc_mode;

  /**
   * Link with the function in NanoEdge AI library.
   */
  NEAI_ProcInitFunction_t proc_init;

  /**
   * Link with the function in NanoEdge AI library.
   */
  NEAI_ProcFunction_t proc;
};


/* Public API declaration */
/**************************/

/**
 * Allocate an instance of NeaiDPU_t in the heap.
 *
 * @return a pointer to the generic object ::IDPU if success,
 * or NULL if out of memory error occurs.
 */
IDPU2_t *NeaiDPU_Alloc(void);

/**
 * Allocate an instance of NeaiDPU_t in a memory block specified by the application.
 * The size of the memory block must be greater or equal to sizeof(NeaiDPU_t).
 * This allocator allows the application to avoid the dynamic allocation.
 *
 * \code
 * NeaiDPU_t dpu;
 * NeaiDPU_StaticAlloc(&dpu);
 * \endcode
 *
 * @param p_mem_block [IN] specify a memory block allocated by the application.
 *        The size of the memory block must be greater or equal to sizeof(NeaiDPU_t).
 * @return a pointer to the generic object ::IDPU2_t if success,
 * or NULL if out of memory error occurs.
 */
IDPU2_t *NeaiDPU_StaticAlloc(void *p_mem_block);


/**
 * Initialize the DPU. It must called before using the instance.
 * After the initialization the DPU needs two memory buffers to manage the input and output data.
 * The memory allocation is delegated to the application. The application use the following methods
 * to set the buffers needed by the DPU:
 * - ADPU2_SetInDataBuffer()
 * - ADPU2_SetOutDataBuffer()
 *
 * @param _this [IN] specifies a pointer to the object.
 * @param mode [IN] specifies the NanoEdge AI operation mode. It depends on the type of library generated by NanoEdge AI Studio:
 *   - Anomaly Detection (for learn or detect)
 *   - One Class Classification
 *   - N-Class Classification
 *   - Extrapolation
 *   Depending on the mode the DPU will use the constant value defined in the proper NanoEdge AI Library header
 *   generated by NanoEdge AI Studio to initialize the (::EMData_t) input and output.
 * @return SYS_NO_ERROR_CODE if success, an application specific error code otherwise.
 */
sys_error_code_t NeaiDPU_Init(NeaiDPU_t *_this, ENeaiMode_t mode);

/**
 * Switch the processing mode for the NanoEdge AI Library. It specifies to the DPU if a new signal is used
 * to learn and improve the model, or to detect anomalies, or to classify the signal.
 *
 * @param _this [IN] specifies a pointer to the object.
 * @param mode [IN] specifies the processing mode. Valid value are:
 *  - E_NEAI_ANOMALY_LEARN
 *  - E_NEAI_ANOMALY_DETECT
 *  - E_NEAI_CLASSIFICATION
 *  Other type of Nanoedge AI libraries will be supported in a next release.
 * @return SYS_NO_ERROR_CODE.
 */
sys_error_code_t NeaiDPU_SetProcessingMode(NeaiDPU_t *_this, ENeaiMode_t mode);

/**
 * Initializes the DPU for the processing mode selected by NeaiDPUSetProcessingMode
 *
 * @param _this [IN] specifies a pointer to the object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
sys_error_code_t NeaiDPU_KnowledgeInit(NeaiDPU_t *_this);

/**
 * This function method specific for a NanoEdge AI library for anomaly detection (AD).
 * It sets the sensitivity of the model in detection mode.
 * It can be tuned at any time without having to go through a new learning phase.
 *
 * @param _this [IN] specifies a pointer to the object.
 * @param sensitivity [IN] specifies the sensitivity of the model.
 * The default sensitivity value is 1. A sensitivity value between 0 and 1 (excluded)
 * decreases the sensitivity of the model, while a value in between 1 and 100 increases it.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
sys_error_code_t NeaiDPU_ADSetSensitivity(NeaiDPU_t *_this, float sensitivity);

/**
 * Get the actual processing mode for the DPU.
 *
 * @param _this [IN] specifies a pointer to the object.
 * @return the actual processing mode of the DPU.
 */
static inline ENeaiMode_t NeaiDPU_GetProcessingMode(NeaiDPU_t *_this);

/**
 * Partial reset of the DPU internal state: all input and output buffers are re-initialized to prepare
 * the DPU to process a new stream of data.
 *
 * @param _this [IN] specifies a pointer to the object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
sys_error_code_t NeaiDPU_PrepareToProcessData(NeaiDPU_t *_this);


/* Inline functions definition */
/*******************************/

static inline
ENeaiMode_t NeaiDPU_GetProcessingMode(NeaiDPU_t *_this)
{
  assert_param(_this != NULL);
  return _this->proc_mode;
}


#ifdef __cplusplus
}
#endif

#endif /* DPU_INC_NEAIDPU_H_ */