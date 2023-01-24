/**
******************************************************************************
* @file    fft.c
* @author  SRA
* @brief   FFT helper functions - based on ARM library.
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

/* Includes ------------------------------------------------------------------*/
#include "fft.h"



#ifdef FFT_DYNAMIC_ALLOCATION
static FFT_Malloc_Function FFT_malloc = malloc;
static FFT_Free_Function FFT_free = free;
#endif

/** @addtogroup X_CUBE_MEMSMIC1_Applications
* @{
*/

/** @addtogroup Microphones_Acquisition
* @{
*/

/** @defgroup AUDIO_APPLICATION
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define FLOAT_NORM_FACTOR 1.0f;
#define INT32_NORM_FACTOR 2147483648.0f;
#define INT16_NORM_FACTOR 32768.0f;

/* Private function prototypes -----------------------------------------------*/

static void TukeyWin(uint32_t len, float32_t ratio, float32_t *dest);
static void BlackmanHarrisWin(uint32_t len, float32_t *dest);
static void HammingWin(uint32_t len, float32_t *dest);
static void FFT_create_window(FFT_instance_t *instance);

static int8_t FFT_Memory_Allocation(FFT_instance_t *instance);

#ifdef FFT_DYNAMIC_ALLOCATION
static int8_t FFT_Dynamic_Allocation(FFT_instance_t *instance);
#endif

#ifdef FFT_STATIC_ALLOCATION
static int8_t FFT_Static_Allocation(FFT_instance_t *instance);
#endif

static void FFT_Set_Normalize_Function(FFT_instance_t *instance);
static float32_t FFT_NormalizeFloat(void *data, uint32_t index);
static float32_t FFT_NormalizeInt32(void *data, uint32_t index);
static float32_t FFT_NormalizeInt16(void *data, uint32_t index);

/* Exported Functions --------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/

/**
* @brief  Initialize the FFT processing depending on the desired configuration
* @param  FFT_instance_t* instance
* @retval 0 if successful, -1 if not
*/
FFT_error_t FFT_Init(FFT_instance_t *instance)
{
  FFT_error_t retVal = FFT_ERROR_NONE;
  
  if(instance == NULL)
  {
    instance->context.status = FFT_ERROR_INVALID_PARAMETER;
    retVal = FFT_ERROR_INVALID_PARAMETER;
  }
  else if(!( (instance->init_params.data_type == FLOAT32) || (instance->init_params.data_type == INT32) || (instance->init_params.data_type == INT16) ))
  {
    instance->context.status = FFT_ERROR_INVALID_PARAMETER;
    retVal = FFT_ERROR_INVALID_PARAMETER;
  }

  if (retVal == FFT_ERROR_NONE)
  {    
  /* Set Handler status */
  instance->context.status = FFT_ERROR_NONE;

  /* Initialize FFT ARM structure */
  (void)arm_rfft_fast_init_f32(&(instance->context.S), (uint16_t)instance->init_params.FFT_len);

  /* Set scratch memory if needed */
  if(instance->init_params.use_direct_process == DIRECT_PROCESS_DISABLED)
  {
    if((instance->init_params.overlap < 0.0f) || (instance->init_params.overlap > 1.0f))
    {
      instance->context.status = FFT_ERROR_INVALID_PARAMETER;
      retVal = FFT_ERROR_INVALID_PARAMETER;
    }
    else
    {
      /*Compute indexes to be used for overlap */
      float32_t float_data_len = ((float32_t) (instance->init_params.FFT_len)) * (1.0f - (float32_t) (instance->init_params.overlap));
      instance->context.new_data_len = (uint32_t) float_data_len;
      instance->context.old_data_len = instance->init_params.FFT_len - instance->context.new_data_len;
    }
  }
    /* Memory allocation */
    if(FFT_Memory_Allocation(instance) != 0)
    {
      instance->context.status = FFT_ERROR_MEMORY;
      retVal = FFT_ERROR_MEMORY;
    }

    /* Create window depending on the user choice */
    FFT_create_window(instance);
    FFT_Set_Normalize_Function(instance);
  }
  
  return retVal;
}

/**
* @brief  Deinitialize FFT library
* @param  FFT_instance_t* instance
* @retval None
*/
FFT_error_t FFT_DeInit(FFT_instance_t *instance)
{
  /* Reset context variables */
  instance->context.new_data_len = 0;
  instance->context.old_data_len = 0;
  instance->context.scratch_idx = 0;
#ifdef FFT_DYNAMIC_ALLOCATION
  /* Memory deallocation */
  FFT_free(instance->context.win);
  FFT_free(instance->context.scratch);
  FFT_free(instance->context.dataIn);
  FFT_free(instance->context.fftIn);
  FFT_free(instance->context.fftOut);
#endif
  instance->context.normalizeData = NULL;
  
  return FFT_ERROR_NONE;
}

/**
* @brief  Fills the "internal_memory_size" of the pHandler parameter passed as argument with a value representing the
*         right amount of memory needed by the library, depending on the specific static parameters adopted.
* @param  FFT_instance_t* instance
* @retval None
*/
int32_t FFT_getMemorySize(FFT_instance_t *instance)
{
  uint32_t counterBytes = 0;
  int32_t retVal;
  
  if(instance == NULL)
  {
    instance->context.status = FFT_ERROR_INVALID_PARAMETER;
    retVal = -1;
  }
  else
  {
    counterBytes += instance->init_params.FFT_len * sizeof(float32_t);
    counterBytes += instance->init_params.FFT_len * sizeof(float32_t);
    
    if(instance->init_params.win_type != FFT_RECT_WIN)
    {
      counterBytes += instance->init_params.FFT_len * sizeof(float32_t);
    }
    if(instance->init_params.output_type == MAGNITUDE)
    {
      counterBytes += instance->init_params.FFT_len * sizeof(float32_t);
    }
    if((instance->init_params.overlap >= 0.0f) && (instance->init_params.overlap <= 1.0f))
    {
      float32_t new_data_len = ((float32_t) (instance->init_params.FFT_len) * (1.0f - (float32_t) (instance->init_params.overlap)));
      counterBytes += (uint32_t)new_data_len * sizeof(float32_t);
    }
    retVal = (int32_t)counterBytes;
  }
  return retVal;
}

/**
* @brief  Pass the input data buffer to the FFT library
* @param  FFT_instance_t* instance
* @param  data: input data buffer
* @param  len: length of input data buffer
* @retval None
*/
int32_t FFT_Data_Input(void *data, uint32_t len, FFT_instance_t *instance)
{
  int32_t ret = 0;
  uint32_t index;
  FFT_data_type_t dataType = instance->init_params.data_type;
  uint32_t s_idx = instance->context.scratch_idx;
  uint32_t new_data_len = instance->context.new_data_len;
  float32_t *scratch_ptr = &instance->context.scratch[s_idx];
  
  for(index = 0; index < len; index++)
  {
    switch(dataType)
    {
    case FLOAT32:
      *scratch_ptr++ = FFT_NormalizeFloat(data, index);
      break;
    case INT32:
      *scratch_ptr++ = FFT_NormalizeInt32(data, index);
      break;
    case INT16:
      *scratch_ptr++ = FFT_NormalizeInt16(data, index);
      break;
    default:
      *scratch_ptr++ = FFT_NormalizeInt16(data, index);      
      break;
    }
    
    s_idx++;
    
    if(s_idx == new_data_len)
    {
      float32_t *data_in = instance->context.dataIn;
      uint32_t old_data_len = instance->context.old_data_len;
      
      (void)memcpy(data_in, &data_in[instance->init_params.FFT_len - old_data_len], old_data_len * sizeof(float32_t));
      (void)memcpy(&data_in[old_data_len], instance->context.scratch, new_data_len * sizeof(float32_t));
      
      ret = 1;
      s_idx = 0;
      scratch_ptr = &instance->context.scratch[0];
    }
  }
  
  instance->context.scratch_idx = s_idx;
  return ret;
}

/**
* @brief  Execute the FFT calculation and returns the FFT amplitude
* @param  FFT_instance_t* instance
* @param  output: FFT amplitude buffer
* @retval None
*/
FFT_error_t FFT_Process(FFT_instance_t *instance, void *output)
{
  FFT_error_t retVal;
  float32_t *win = instance->context.win;
  float32_t *dataIn = instance->context.dataIn;
  float32_t *fftIn = instance->context.fftIn;
  
  if(instance->init_params.win_type != FFT_RECT_WIN)
  {
    arm_mult_f32(dataIn, win, fftIn, instance->init_params.FFT_len);
  }
  else
  {
    (void)memcpy(fftIn, dataIn, sizeof(float32_t) * instance->init_params.FFT_len);
  }
  
  if(instance->init_params.output_type == COMPLEX)
  {
    arm_rfft_fast_f32(&instance->context.S, fftIn, output, 0);
  }
  
  if(instance->init_params.output_type == MAGNITUDE)
  {
    float32_t *fftOut = instance->context.fftOut;
    arm_rfft_fast_f32(&instance->context.S, fftIn, fftOut, 0);
    arm_cmplx_mag_f32(fftOut, output, instance->init_params.FFT_len / 2U);
  }
  
  if((output == NULL) || (instance == NULL))
  {
    instance->context.status = FFT_ERROR_INVALID_PARAMETER;
    retVal = FFT_ERROR_INVALID_PARAMETER;
  }
  else if(instance->context.status != FFT_ERROR_NONE)
  {
    retVal = FFT_ERROR_INVALID_EXECUTION;
  }
  else if(instance->init_params.use_direct_process == DIRECT_PROCESS_ENABLED)
  {
    instance->context.status = FFT_ERROR_INVALID_EXECUTION;
    retVal = FFT_ERROR_INVALID_EXECUTION;
  }
  else
  {
    retVal = FFT_ERROR_NONE;
  }
  return retVal;
}

/**
* @brief  Execute the FFT calculation passing directly the input buffer from the user space and returns the FFT amplitude
* @param  FFT_instance_t* instance
* @param  output: FFT amplitude buffer
* @param  input: input data buffer
* @retval None
*/
FFT_error_t FFT_Direct_Process(FFT_instance_t *instance, void *input, float32_t *output)
{
  float32_t *win = instance->context.win;
  float32_t *fftIn = instance->context.fftIn;
  float32_t *dataIn = instance->context.dataIn;
  
  for(uint32_t i = 0; i < instance->init_params.FFT_len; i++)
  {
    dataIn[i] = instance->context.normalizeData(input, i);
  }
  
  if(instance->init_params.win_type != FFT_RECT_WIN)
  {
    arm_mult_f32(dataIn, win, fftIn, instance->init_params.FFT_len);
  }
  else
  {
    (void)memcpy(fftIn, dataIn, sizeof(float32_t) * instance->init_params.FFT_len);
  }
  
  if(instance->init_params.output_type == COMPLEX)
  {
    arm_rfft_fast_f32(&instance->context.S, fftIn, output, 0);
  }
  
  if(instance->init_params.output_type == MAGNITUDE)
  {
    float32_t *fftOut = instance->context.fftOut;
    arm_rfft_fast_f32(&instance->context.S, fftIn, fftOut, 0);
    arm_cmplx_mag_f32(fftOut, output, instance->init_params.FFT_len / 2U);
  }
  
  return FFT_ERROR_NONE;
}


static void FFT_create_window(FFT_instance_t *instance)
{
  /* Create window depending on the user choice */
  switch(instance->init_params.win_type)
  {
  case FFT_RECT_WIN:
    break;
  case FFT_HAMMING_WIN:
    HammingWin(instance->init_params.FFT_len, instance->context.win);
    break;
  case FFT_HANNING_WIN:
    TukeyWin(instance->init_params.FFT_len, 1.0f, instance->context.win);
    break;
  case FFT_BLACKMAN_HARRIS_WIN:
    BlackmanHarrisWin(instance->init_params.FFT_len, instance->context.win);
    break;
  case FFT_TUKEY_0_25_WIN:
    TukeyWin(instance->init_params.FFT_len, 0.25f, instance->context.win);
    break;
  case FFT_TUKEY_0_75_WIN:
    TukeyWin(instance->init_params.FFT_len, 0.75f, instance->context.win);
    break;
  default:
    break;
  }
}

/**
* @brief  Chck the memory allocation
* @param  FFT_instance_t* instance
* @retval 0 if ok
*/
static int8_t FFT_Memory_Allocation(FFT_instance_t *instance)
{
  int8_t retVal = 0;
#ifdef FFT_DYNAMIC_ALLOCATION
  /* Memory allocation */
  if(FFT_Dynamic_Allocation(instance)!=0)
  {
    instance->context.status = FFT_ERROR_MEMORY;
    retVal = 2;
  }
#endif
#ifdef FFT_STATIC_ALLOCATION
  FFT_Static_Allocation(instance);
#endif
  
  return retVal;
}


/**
* @brief  Initialize dynamically the FFT memory
* @param  FFT_instance_t* instance
* @retval 0 if successful, -1 if not
*/
#ifdef FFT_DYNAMIC_ALLOCATION
static int8_t FFT_Dynamic_Allocation(FFT_instance_t *instance)
{
  int8_t retVal = 0;
  
  /* Memory allocation */
  if(instance->init_params.win_type != FFT_RECT_WIN)
  {
    instance->context.win = (float32_t*) FFT_malloc(instance->init_params.FFT_len * sizeof(float32_t));
    memset((uint8_t *)instance->context.win, 0, instance->init_params.FFT_len * sizeof(float32_t));
    if(!instance->context.win)
    {
      instance->context.status = FFT_ERROR_MEMORY;
    }
  }
  if(instance->init_params.output_type == MAGNITUDE)
  {
    instance->context.fftOut = (float32_t*) FFT_malloc(instance->init_params.FFT_len * sizeof(float32_t));
    memset((uint8_t *)instance->context.fftOut, 0, instance->init_params.FFT_len * sizeof(float32_t));
    if(!instance->context.fftOut)
    {
      instance->context.status = FFT_ERROR_MEMORY;
    }
  }
  instance->context.dataIn = (float32_t*) FFT_malloc(instance->init_params.FFT_len * sizeof(float32_t));
  memset((uint8_t *)instance->context.dataIn, 0, instance->init_params.FFT_len * sizeof(float32_t));
  instance->context.fftIn = (float32_t*) FFT_malloc(instance->init_params.FFT_len * sizeof(float32_t));
  memset((uint8_t *)instance->context.fftIn, 0, instance->init_params.FFT_len * sizeof(float32_t));
  
  if(!instance->context.fftIn || !instance->context.dataIn)
  {
    retVal = -1;
  }  
  else 
  {
    if(instance->init_params.use_direct_process == DIRECT_PROCESS_DISABLED)
    {
      instance->context.scratch = (float32_t*) FFT_malloc(instance->context.new_data_len * sizeof(float32_t));
      memset((uint8_t *)instance->context.scratch, 0, instance->context.new_data_len * sizeof(float32_t));
      if(!instance->context.scratch)
      {
        retVal = -1;
      }
    }
  }
  
  return retVal;
}
#endif

/**
* @brief  Initialize statically the FFT memory using the buffer passed by the user.
* @param  FFT_instance_t* instance
* @retval 0 if successful, -1 if not
*/
#ifdef FFT_STATIC_ALLOCATION
void FFT_Static_Allocation(FFT_instance_t *instance)
{
  uint32_t index = 0;
  
  instance->context.dataIn = instance->init_params.userBuffer;
  index += instance->init_params.FFT_len;
  
  instance->context.fftIn = &instance->init_params.userBuffer[index];
  index += instance->init_params.FFT_len;
  
  if(instance->init_params.win_type != FFT_RECT_WIN)
  {
    instance->context.win = &instance->init_params.userBuffer[index];
    index += instance->init_params.FFT_len;
  }
  if(instance->init_params.output_type == MAGNITUDE)
  {
    instance->context.fftOut = &instance->init_params.userBuffer[index];
    index += instance->init_params.FFT_len;
  }
  if((instance->init_params.overlap > 0) && (instance->init_params.use_direct_process == 0))
    instance->context.scratch = &instance->init_params.userBuffer[index];
}
#endif


static void FFT_Set_Normalize_Function(FFT_instance_t *instance)
{
  switch(instance->init_params.data_type)
  {
  case FLOAT32:
    instance->context.normalizeData = FFT_NormalizeFloat;
    break;
  case INT32:
    instance->context.normalizeData = FFT_NormalizeInt32;
    break;
  case INT16:
    instance->context.normalizeData = FFT_NormalizeInt16;
    break;
  default:
    break;
  }
}

/**
 * @Brief: Tukey window function
 *
 * @Description:
 *
 * 				/
 *				|  1/2 * { 1 + cos( 2PI/r * [ x - r/2 ]) },	    0 ≤ 1 < r/2
 * 			    |
 * 		f(x) = <   1,											r/2 ≤ x < 1 - r/2
 *				|
 *				|  1/2 * { 1 + cos( 2PI/r * [x - 1 + r/2]) },	1 - r/2 ≤ x < 1
 *              \
 *
 *
 * with: r = ratio
 * 		  x = normalized input. In the implementation below x is equal to @tukeyWin_in
 *
 * ref: https://www.mathworks.com/help/signal/ref/tukeywin.html
 *
 * @param[in] len: destination array dimension
 * @param[in] ratio: Cosine fraction, specified as a real scalar.
 * 					 The Tukey window is a rectangular window with the first and
 * 					  last r/2 percent of the samples equal to parts of a cosine.
 * @param[out] dest: pointer to output array containing the Tukey window
 */

static void TukeyWin(uint32_t len, float32_t ratio, float32_t *dest)
{
  uint32_t tukeyWin_cnt = 0;
  float32_t tukeyWin_in = 0.0f;
  float32_t half_ratio = 0.0f;

  /** Check input param */
  if( ((ratio >= 0.0f) && (ratio <= 1.0f)) &&
	  ( dest != NULL))
  {
	/** Compute half ratio */
	half_ratio = ratio / 2.0f;

	for( tukeyWin_cnt = 0; tukeyWin_cnt < len; tukeyWin_cnt++)
	{
	  /** Normalize counter over length.
	   *  tukeyWin_in belongs to: [0, 1]
	   *  We can now apply the f(x) formula described above
	   **/
	  tukeyWin_in = (float32_t)tukeyWin_cnt / (float32_t)len;

	  /** Window left side */
	  if( tukeyWin_in < half_ratio )
	  {
		  dest[tukeyWin_cnt] =  0.5f * ( 1.0f + cosf( (2.0f * M_PI / ratio) * (tukeyWin_in -  (ratio / 2.0f) ) ));
	  }
	  /** Half window position */
	  else if( (tukeyWin_in  >= half_ratio) && ( tukeyWin_in < (1.0f - half_ratio)) )
	  {
		  dest[tukeyWin_cnt] = 1.0f;
	  }
	  /** Window right side */
	  else
	  {
		  dest[tukeyWin_cnt] =  0.5f * ( 1.0f + cosf( (2.0f * M_PI / ratio) * (tukeyWin_in -1.0f + half_ratio) ));
	  }
	}
  }
}

static void HammingWin(uint32_t len, float32_t *dest)
{
  uint32_t x = 0;
  float32_t alpha0 = 0.53836f;
  float32_t alpha1 = 1.0f - alpha0;
  
  for(x = 0; x < len; x++)
  {
    dest[x] = alpha0 - (alpha1 * cosf(2.0f * M_PI * (float32_t) x / (float32_t) len));
  }
}

static void BlackmanHarrisWin(uint32_t len, float32_t *dest)
{
  uint32_t x = 0;
  float32_t alpha0 = 0.42f;
  float32_t alpha1 = 0.5f;
  float32_t alpha2 = 0.08f;
  
  for(x = 0; x < len; x++)
  {
    dest[x] = alpha0 - (alpha1 * cosf(2.0f * M_PI * (float32_t) x / (float32_t) len)) + (-alpha2 * cosf(4.0f * M_PI * (float32_t) x / (float32_t) len));
  }
}

static float32_t FFT_NormalizeFloat(void *data, uint32_t index)
{
  float32_t normData = ((float32_t*) (data))[index] / FLOAT_NORM_FACTOR;
  return normData; 
}

static float32_t FFT_NormalizeInt32(void *data, uint32_t index)
{
  float32_t normData = (float32_t) ((int32_t*) (data))[index] / INT32_NORM_FACTOR;
  return normData;
}

static float32_t FFT_NormalizeInt16(void *data, uint32_t index)
{
  float32_t normData = (float32_t) ((int16_t*) (data))[index] / INT16_NORM_FACTOR;
  return normData;
}


#ifdef FFT_DYNAMIC_ALLOCATION
void FFT_set_allocation_functions(FFT_Malloc_Function malloc_fun, FFT_Free_Function free_fun)
{
  FFT_malloc = malloc_fun;
  FFT_free = free_fun;
}
#endif

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

