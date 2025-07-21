/**
  ******************************************************************************
  * @file    FFT_DPU2.h
  * @author  SRA - MCD
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

#ifndef FFT_DPU2_H_
#define FFT_DPU2_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "ADPU2.h"
#include "ADPU2_vtbl.h"

/* Type declaration      */
/*****************************/

typedef struct
{
  uint32_t fft_input_buf_dim;    /** FFT input buffer dimension.
                  *  It's the buffer dimension expressed in number of sample
                  *  on which will be computed the FFT.
                  */

  uint32_t input_signal_len;     /** It's the length of input signal expressed in number of sample.
                  *  This is equal to the number of sample sended by the data source.
                  *  For example, in case of a sensor sending N sample at a time to the FFT_DPU2.0:
                  *  fft_input_signal_len = N;
                  */

  uint32_t input_signal_n_axis;   /** Input signal number of axis.
                   */

  uint8_t  n_average;        /** Number of average on which will be computed the FFT.
                  *  When nAverage is different from zero the FFT will be computed nAverage time,
                  *  sum it and averaged. In this case the FFT_DPU2 output will be the average FFT.
                  */

  float_t overlap;            /** FFT input overlap expressed in percentage  between 0 and 1 */


} FFT_DPU2_Input_param_t;

/**
  * Create  type name for _FFT_DPUTask.
 */
typedef struct _FFT_DPU2_t FFT_DPU2_t;

// Public API declaration
//***********************

/**
  * @Brief Allocate an instance of FFT_DPU2.
  *
  * @return a pointer to the interface IDPU2_t
  * or NULL if out of memory error occurs.
  */
IDPU2_t *FFT_DPU2Alloc(void);

/**
  * @Brief FFTDPU2 Initialization function.
  *
  *      This function initializes in_data and out_data EMDData_t
  *      based on input param: @input_dim and @axis
  *
  *      Call the base class initialization: ADPU2_Init(ADPU2_t *_this, EMData_t in_data, EMData_t out_data)
  *
  *      Initialize the FFT library by calling: FFT_Init(FFT_instance_t *instance)
  *
  * @return a pointer to the interface IDPU2_t
  * or NULL if out of memory error occurs.
  */
sys_error_code_t FFT_DPU2Init(FFT_DPU2_t *const _this, FFT_DPU2_Input_param_t *const FFT_DPU2_Input_param);


#ifdef __cplusplus
}
#endif

#endif /* FFT_DPU2_H_ */
