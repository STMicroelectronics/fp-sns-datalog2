/**
  ******************************************************************************
  * @file    MfccDPU.h
  * @author  STMicroelectronics - AIS - MCD Team
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef DPU_INC_MFCCDPU_H_
#define DPU_INC_MFCCDPU_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "ADPU2.h"
#include "ADPU2_vtbl.h"
#include "mfccDPU_tables.h"
#include "feature_extraction.h"

#define MFCCDPU_SAMPLE_RATE      (192000U)  /* Input signal sampling rate */
#define MFCCDPU_FFT_LEN          (4096U)     /* Number of FFT points. Must be greater or equal to FRAME_LEN */
#define MFCCDPU_FRAME_LEN        (MFCCDPU_FFT_LEN) /* Window length and then padded with zeros to match FFT_LEN */
#define MFCCDPU_NUM_MFCC         (32U)     /* Number of MFCCs to return */
#define MFCCDPU_NUM_MFCC_RAW     (46U)     /* Number of MFCC raw to return */


/**
  * Create  type name for _MfccDPU_t.
  */
typedef struct _MfccDPU MfccDPU_t;

/**
  * MfccDPU_t internal state.
  * It declares only the virtual table used to implement the inheritance.
  */
struct _MfccDPU
{
  /**
    * Base class object.
    */
  ADPU2_t super;

  arm_rfft_fast_instance_f32 S_Rfft;
  MelFilterTypeDef           S_MelFilter;
  SpectrogramTypeDef         S_Spectr;
  MelSpectrogramTypeDef      S_MelSpectr;
  LogMelSpectrogramTypeDef   S_LogMelSpectr;
  DCT_InstanceTypeDef        S_DCT;
  MfccTypeDef                S_Mfcc;

  float32_t pSpectrScratchBuffer[MFCCDPU_FFT_LEN];
  float32_t pMfccScratchBuffer[MFCCDPU_NUM_MFCC];

  int32_t mfccRawIdx;
};


/* Public API declaration */
/**************************/

/**
  * Allocate an instance of AiDPU_t in the eLooM framework heap.
  *
  * @return a pointer to the generic object ::IDPU2_t if success,
  * or NULL if out of memory error occurs.
  */
IDPU2_t *MfccDPUAlloc(void);

/**
  * Allocate an instance of MfccDPU_t in a memory block specified by the application.
  * The size of the memory block must be greater or equal to sizeof(MfccDPU_t).
  * This allocator allows the application to avoid the dynamic allocation.
  *
  * \code
  * MfccDPU_t dpu;
  * MfccDPUStaticAlloc(&dpu);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to sizeof(MfccDPU_t).
  * @return a pointer to the generic object ::IDPU if success,
  * or NULL if out of memory error occurs.
  */
IDPU2_t *MfccDPUStaticAlloc(void *p_mem_block);

/**
  * Initialize the DPU. Most of the DPU parameters are constant and defined at the beginning of this header file:
  * - MFCCDPU_SAMPLE_RATE
  * - MFCCDPU_FFT_LEN
  * - MFCCDPU_FRAME_LEN
  * - MFCCDPU_NUM_MFCC
  * - MFCCDPU_NUM_MFCC_COL
  *
  * @param _this [IN] specifies a pointer to the object.
  * @param mfcc_data_input_user [IN] specifies the size of the ... [TBD]
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
  */
sys_error_code_t MfccDPUInit(MfccDPU_t *_this, uint16_t mfcc_data_input_user);

/**
  * Partial reset of the DPU internal state: all input and output buffers are re-initialized to prepare
  * the DPU to process a new stream of data.
  *
  * @param _this [IN] specifies a pointer to the object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
  */
sys_error_code_t MfccDPUPrepareToProcessData(MfccDPU_t *_this);


/* Inline functions definition */
/*******************************/

#ifdef __cplusplus
}
#endif

#endif /* DPU_INC_MFCCDPU_H_ */

