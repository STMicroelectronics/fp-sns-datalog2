/**
  ******************************************************************************
  * @file    FFT_DPU2.c
  * @author  SRA - MCD
  * @brief
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

#include "FFT_DPU2.h"
#include "FFT_DPU2_vtbl.h"
#include "services/sysdebug.h"
#include "fft.h"

/* Private definition */
/**********************/
#define FFT_COMPUTE_AVG_SCALE_FACTOR(avg_cnt, fft_len) (( 2.0f ) / (((float)avg_cnt) * ((float)fft_len)))

/**
  * Class object declaration.
  */
typedef struct _FFT_DPU2Class
{
  /**
    * IDPU2_t class virtual table.
    */
  IDPU2_vtbl vtbl;

} FFT_DPU2Class;


/* Objects instance */
/********************/

/**
  * The class object.
  */
static const FFT_DPU2Class sTheClass =
{
  {
    ADPU2_vtblAttachToDataSource,
    ADPU2_vtblDetachFromDataSource,
    ADPU2_vtblAttachToDPU,
    ADPU2_vtblDetachFromDPU,
    ADPU2_vtblDispatchEvents,
    ADPU2_vtblRegisterNotifyCallback,
    FFT_DPU2_vtblProcess

  }
};

/**
  *  FFT_DPU2 internal structure.
  */
struct _FFT_DPU2_t
{
  /**
    * Base class object.
    */
  ADPU2_t super;

  FFT_instance_t fft_instance;

  uint8_t n_average;

  uint8_t avg_counter;

  float *fft_temp_avg_out;

  float *fft_avg_out;

  float overlap;
};


/* Private member function declaration */
/***************************************/
static FFT_error_t FFT_compute_avg_output(FFT_DPU2_t *const pObj);


/*  Public member function declaration */
// ***********************************

IDPU2_t *FFT_DPU2Alloc(void)
{
  IDPU2_t *pxObj = (IDPU2_t *) SysAlloc(sizeof(FFT_DPU2_t));

  if (pxObj != NULL)
  {
    pxObj->vptr = &sTheClass.vtbl;
  }

  return pxObj;
}

sys_error_code_t FFT_DPU2Init(FFT_DPU2_t *const _this, FFT_DPU2_Input_param_t *const FFT_DPU2_Input_param)
{
  assert_param(_this != NULL);
  assert_param(FFT_DPU2_Input_param != NULL);

  sys_error_code_t retVal = SYS_NO_ERROR_CODE;
  FFT_error_t fft_retVal = FFT_ERROR_NONE;
  EMData_t in_data;
  EMData_t out_data;
  uint16_t fft_output_dim = FFT_DPU2_Input_param->fft_input_buf_dim / 2;

  /**
    * Configure input data buffer
    */
  if (1 == FFT_DPU2_Input_param->input_signal_n_axis)
  {
    retVal = EMD_1dInit(&in_data, NULL, E_EM_UINT16, FFT_DPU2_Input_param->input_signal_len);

    if (SYS_IS_ERROR_CODE(retVal))
    {
      sys_error_handler();
    }

    /**
      * Configure output data buffer
      */
    retVal = EMD_1dInit(&out_data, NULL, E_EM_FLOAT, fft_output_dim);

    if (SYS_IS_ERROR_CODE(retVal))
    {
      sys_error_handler();
    }

    /* Allocate intermediate buffer (for averaging output result if nAverage > 0)*/
    if (FFT_DPU2_Input_param->n_average > 0u)
    {
      _this->n_average        = FFT_DPU2_Input_param->n_average;
      _this->avg_counter      = 0u;
      _this->fft_avg_out      = (float *)  SysAlloc(sizeof(float) * FFT_DPU2_Input_param->input_signal_n_axis * fft_output_dim);
      _this->fft_temp_avg_out = (float *)  SysAlloc(sizeof(float) * FFT_DPU2_Input_param->input_signal_n_axis * fft_output_dim);
    }
  }

  /* init fft library */
  _this->fft_instance.init_params.FFT_len = FFT_DPU2_Input_param->fft_input_buf_dim;
  _this->fft_instance.init_params.overlap = FFT_DPU2_Input_param->overlap;
  _this->fft_instance.init_params.win_type = FFT_HANNING_WIN;
  _this->fft_instance.init_params.output_type = MAGNITUDE;
  _this->fft_instance.init_params.data_type = INT16;
  _this->fft_instance.init_params.use_direct_process = DIRECT_PROCESS_DISABLED;

  fft_retVal = FFT_Init(&_this->fft_instance);

  if (FFT_ERROR_NONE != fft_retVal)
  {
    sys_error_handler();
  }

  /**
    * initialize the base class
    */
  retVal = ADPU2_Init((ADPU2_t *)_this, in_data, out_data);

  return retVal;
}

sys_error_code_t FFT_DPU2_vtblProcess(IDPU2_t *_this, EMData_t in_data, EMData_t out_data)
{
  sys_error_code_t retVal = SYS_NO_ERROR_CODE;

  void *p_fft_in_data;
  uint32_t fft_in_data_len;
  uint8_t  fft_data_input_ready;
  FFT_DPU2_t *pObj = (FFT_DPU2_t *)_this;

  /** Set pointer to in_data p_payload */
  p_fft_in_data = (void *)pObj->super.in_data.p_payload;

  /** Get in_data p_payload buffer len */
  fft_in_data_len = in_data.shapes[0];

  if (DIRECT_PROCESS_ENABLED == pObj->fft_instance.init_params.use_direct_process)
  {
    FFT_Direct_Process(&pObj->fft_instance, (float *)pObj->super.in_data.p_payload,
                       (float *)pObj->super.out_data.p_payload);
  }
  else
  {
    /** Pass DPU input data to FFT library */
    fft_data_input_ready = FFT_Data_Input(p_fft_in_data, fft_in_data_len, &pObj->fft_instance);

    /** Internal FFT input buffer full.
      *  Ready to call process function
      */
    if (fft_data_input_ready)
    {
      if (pObj->n_average == 0)
      {
        /** Compute directly FFT and store the result in EMData out_data payload */
        (void)FFT_Process(&pObj->fft_instance, (float *)pObj->super.out_data.p_payload);
      }
      else
      {
        (void)FFT_compute_avg_output(pObj);
      }
    }
    else
    {
      /** FFT Data input buffer not full yet */
      retVal = SYS_FFT_DPU_INPUT_NOT_READY_RET_VAL;
    }
  }

  return retVal;
}

/**************************** Private function definition ***********************************/
static FFT_error_t FFT_compute_avg_output(FFT_DPU2_t *const pObj)
{
  assert_param(pObj != NULL);

  uint32_t fft_out_data_len;
  FFT_error_t fft_retVal;

  /** Get FFT output len */
  fft_out_data_len = pObj->super.out_data.shapes[0];

  /** Compute FFT and save the result in temp average output */
  fft_retVal = FFT_Process(&pObj->fft_instance, pObj->fft_temp_avg_out);

  /** Sum current FFT result in fft_avg_out */
  arm_add_f32(pObj->fft_temp_avg_out, pObj->fft_avg_out, pObj->fft_avg_out, fft_out_data_len);

  /** Increment number of computed FFT */
  pObj->avg_counter++;

  /** Time to perform the average */
  if (pObj->avg_counter >= pObj->n_average)
  {
    /** Compute FFT scale factor */
    float fft_avg_scale_factor = pObj->n_average;//FFT_COMPUTE_AVG_SCALE_FACTOR(pObj->avg_counter, fft_out_data_len);

    /** Compute average and store the result in EMData out_data payload*/
    arm_scale_f32(pObj->fft_avg_out, fft_avg_scale_factor, (float *)pObj->super.out_data.p_payload, fft_out_data_len);

    /** Reset average counter */
    pObj->avg_counter = 0u;

    /** Clear fft out buffer */
    memset(pObj->fft_avg_out, 0, sizeof(float) * fft_out_data_len);
  }

  return fft_retVal;
}

