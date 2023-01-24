/**
 ******************************************************************************
 * @file    fft.h
 * @author  SRA
 * @brief   header for fft.c file .
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FFT_H
#define __FFT_H

#ifndef __FPU_PRESENT
#define __FPU_PRESENT (1)
#endif

/* Includes ------------------------------------------------------------------*/
#include "arm_math.h"
#include "math.h"
#include "stdlib.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define FFT_DYNAMIC_ALLOCATION
//#define FFT_STATIC_ALLOCATION

/** @addtogroup X_CUBE_MEMSMIC1_Applications
 * @{
 */

/** @addtogroup Microphones_Acquisition
 * @{
 */

/** @defgroup AUDIO_APPLICATION 
 * @{
 */

/* Exported constants --------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------*/


typedef enum
{
  FFT_RECT_WIN = 0, FFT_HAMMING_WIN, FFT_HANNING_WIN, FFT_BLACKMAN_HARRIS_WIN, FFT_TUKEY_0_25_WIN, FFT_TUKEY_0_75_WIN,
} FFT_windows_t;

typedef enum
{
  COMPLEX = 0, MAGNITUDE,
} FFT_output_type_t;

typedef enum
{
  FLOAT32 = 0, INT32, INT16
} FFT_data_type_t;

typedef enum
{
  FFT_ERROR_NONE = 0, FFT_ERROR_INVALID_PARAMETER, FFT_ERROR_MEMORY, FFT_ERROR_INVALID_EXECUTION
} FFT_error_t;

typedef enum
{
  DIRECT_PROCESS_DISABLED = 0, DIRECT_PROCESS_ENABLED
} FFT_direct_process_t;

typedef void* (*FFT_Malloc_Function)(size_t);
typedef void* (*FFT_Calloc_Function)(size_t, size_t);
typedef void (*FFT_Free_Function)(void*);

typedef struct
{
  arm_rfft_fast_instance_f32 S;
  uint32_t new_data_len;
  uint32_t old_data_len; /* New data Idx */
  uint32_t scratch_idx;
  float32_t *win;
  float32_t *scratch;
  float32_t *dataIn;
  float32_t *fftIn;
  float32_t *fftOut;
  float32_t (*normalizeData)(void *data, uint32_t index);
  FFT_error_t status;
} FFT_context_t;

typedef struct
{
  FFT_direct_process_t use_direct_process;
  uint32_t FFT_len;
  float32_t overlap;
  FFT_windows_t win_type;
  FFT_data_type_t data_type;
  FFT_output_type_t output_type;
#ifdef FFT_STATIC
  float32_t * userBuffer;  /*   used to refer the context buffer pointers, in the follow order:
                            - dataIn
                            - fftIn
                            - win
                            - fftOut
                            - scratch   */
#endif
} FFT_init_params_t;

typedef struct
{
  FFT_init_params_t init_params;
  FFT_context_t context;
} FFT_instance_t;

/* Exported macro ------------------------------------------------------------*/
/** @defgroup AUDIO_APPLICATION_Exported_Defines 
 * @{
 */

/**
 * @}
 */
/* Exported functions ------------------------------------------------------- */
FFT_error_t FFT_Init(FFT_instance_t *instance);
FFT_error_t FFT_DeInit(FFT_instance_t *instance);

int32_t FFT_getMemorySize(FFT_instance_t *instance);
int32_t FFT_Data_Input(void *data, uint32_t len, FFT_instance_t *instance);

FFT_error_t FFT_Process(FFT_instance_t *instance, void *output);
FFT_error_t FFT_Direct_Process(FFT_instance_t *instance, void *input, float32_t *output);
void FFT_set_allocation_functions(FFT_Malloc_Function malloc_fun, FFT_Free_Function free_fun);

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#endif /* __FFT_H */

