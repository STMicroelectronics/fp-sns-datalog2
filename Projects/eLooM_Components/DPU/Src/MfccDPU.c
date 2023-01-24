/**
  ******************************************************************************
  * @file    MfccDPU.c
  * @author  STMicroelectronics - AIS - MCD Team
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

#include "MfccDPU.h"
#include "MfccDPU_vtbl.h"
#include "MfccDPU_Tables.h"
#include "services/sysmem.h"
#include "services/sysdebug.h"
#include <stdio.h>

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_MFCC, level, message)


/**
 * Class object declaration.
 */
typedef struct _MfccDPUClass
{
  /**
   * IDPU2_t class virtual table.
   */
  IDPU2_vtbl vtbl;

} MfccDPUClass_t;


/* Objects instance */
/********************/

/**
 * The class object.
 */
static const MfccDPUClass_t sTheClass = {
    /* class virtual table */
    {
        ADPU2_vtblAttachToDataSource,
        ADPU2_vtblDetachFromDataSource,
        ADPU2_vtblAttachToDPU,
        ADPU2_vtblDetachFromDPU,
        ADPU2_vtblDispatchEvents,
        ADPU2_vtblRegisterNotifyCallback,
        MfccDPU_vtblProcess
    }
};


/* Private member functions declaration */
/****************************************/


/* Public API functions definition */
/***********************************/

IDPU2_t *MfccDPUAlloc() {
  IDPU2_t *p_obj = (IDPU2_t*) SysAlloc(sizeof(MfccDPU_t));

  if (p_obj != NULL)
  {
    p_obj->vptr = &sTheClass.vtbl;
  }

  return p_obj;
}

IDPU2_t *MfccDPUStaticAlloc(void *p_mem_block)
{
  IDPU2_t *p_obj = (IDPU2_t*)p_mem_block;
  if (p_obj != NULL)
  {
    p_obj->vptr = &sTheClass.vtbl;
  }

  return p_obj;
}

sys_error_code_t MfccDPUInit(MfccDPU_t *_this, uint16_t mfcc_data_input_user)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /*initialize the base class*/
  EMData_t in_data, out_data;
  res = EMD_1dInit(&in_data, NULL, E_EM_FLOAT, mfcc_data_input_user);
  if (SYS_IS_ERROR_CODE(res))
  {
    sys_error_handler();
  }
  res = EMD_Init(&out_data, NULL, E_EM_FLOAT, E_EM_MODE_LINEAR, 2U, MFCCDPU_NUM_MFCC_RAW, MFCCDPU_NUM_MFCC);
  if (SYS_IS_ERROR_CODE(res))
  {
    sys_error_handler();
  }
  res = ADPU2_Init((ADPU2_t*)_this, in_data, out_data);
  if (SYS_IS_ERROR_CODE(res))
  {
    sys_error_handler();
  }

  // take the ownership of the Sensor Event IF
  (void)IEventListenerSetOwner((IEventListener *) ADPU2_GetEventListenerIF(&_this->super), &_this->super);

  /*initialize other members of the object*/
  _this->mfccRawIdx = 0;

  /*initialize AI preprocessing (MFCC computation)*/
#ifdef MFCC_GEN_LUT
  genLUT();
#endif
  /* Init RFFT */
  arm_rfft_fast_init_f32(&_this->S_Rfft, MFCCDPU_FFT_LEN);

  /* Init Spectrogram */
  _this->S_Spectr.pRfft                    = &_this->S_Rfft;
  _this->S_Spectr.Type                     = SPECTRUM_TYPE_POWER;
  _this->S_Spectr.pWindow                  = (float32_t *) hannWin_4096;
  _this->S_Spectr.SampRate                 = MFCCDPU_SAMPLE_RATE;
  _this->S_Spectr.FrameLen                 = MFCCDPU_FRAME_LEN;
  _this->S_Spectr.FFTLen                   = MFCCDPU_FFT_LEN;
  _this->S_Spectr.pScratch                 = _this->pSpectrScratchBuffer;

  /* Init mel filterbank */
  _this->S_MelFilter.pStartIndices         = (uint32_t *) melFiltersStartIndices_4096_32;
  _this->S_MelFilter.pStopIndices          = (uint32_t *) melFiltersStopIndices_4096_32;
  _this->S_MelFilter.pCoefficients         = (float32_t *) melFilterLut_4096_32;
  _this->S_MelFilter.NumMels               = MFCCDPU_NUM_MFCC;
  _this->S_MelFilter.FFTLen                = MFCCDPU_FFT_LEN;
  _this->S_MelFilter.SampRate              = MFCCDPU_SAMPLE_RATE;
  _this->S_MelFilter.FMin                  = 0.0f;
  _this->S_MelFilter.FMax                  = MFCCDPU_SAMPLE_RATE / 2.0f;
  _this->S_MelFilter.Formula               = MEL_SLANEY;
  _this->S_MelFilter.Normalize             = 1U;
  _this->S_MelFilter.Mel2F                 = 1U;

  /* Init MelSpectrogram */
  _this->S_MelSpectr.SpectrogramConf       = &_this->S_Spectr;
  _this->S_MelSpectr.MelFilter             = &_this->S_MelFilter;


  /* Init LogMelSpectrogram */
  _this->S_LogMelSpectr.MelSpectrogramConf = &_this->S_MelSpectr;
  _this->S_LogMelSpectr.LogFormula         = LOGMELSPECTROGRAM_SCALE_DB;
  _this->S_LogMelSpectr.Ref                = 1.0f;
  _this->S_LogMelSpectr.TopdB              = HUGE_VALF;

  /* Init DCT operation */
  _this->S_DCT.NumFilters                  = MFCCDPU_NUM_MFCC;
  _this->S_DCT.NumInputs                   = MFCCDPU_NUM_MFCC;
  _this->S_DCT.Type                        = DCT_TYPE_II_ORTHO;
  _this->S_DCT.RemoveDCTZero               = 0;
  _this->S_DCT.pDCTCoefs                   = (float32_t *) dct2_32_32;

  /* Init MFCC */
  _this->S_Mfcc.LogMelConf                 = &_this->S_LogMelSpectr;
  _this->S_Mfcc.pDCT                       = &_this->S_DCT;
  _this->S_Mfcc.NumMfccCoefs               = MFCCDPU_NUM_MFCC;
  _this->S_Mfcc.pScratch                   = _this->pMfccScratchBuffer;
  _this->S_MelFilter.NumMels               = MFCCDPU_NUM_MFCC;

  return res;
}

sys_error_code_t MfccDPUPrepareToProcessData(MfccDPU_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ADPU2_Reset((ADPU2_t*)_this);
  _this->mfccRawIdx = 0;

  return res;
}
//#define MFCC_GEN_LUT
#ifdef MFCC_GEN_LUT
static void genLUT(void)
{
#define NUM_MEL      32
#define NUM_MEL_COEF 3772
#define NUM_MFCC     32
#define FFT_LEN      4096
#define SMP_RATE     192000

  float32_t Win [FFT_LEN];
  uint32_t  start_indices[NUM_MEL];
  uint32_t  stop_indices [NUM_MEL];
  float32_t melFilterLut [NUM_MEL_COEF];
  float32_t dct[NUM_MEL*NUM_MFCC];
  MelFilterTypeDef           S_MelFilter;
  DCT_InstanceTypeDef        S_DCT;
  int i;

  /* Init window function */
  if (Window_Init(Win, FFT_LEN , WINDOW_HANN) != 0){
	while(1);
  }
  printf("Hanning window: %d \n\r",FFT_LEN);
  for (i=0;i<FFT_LEN;i++)	{
	printf("%.10e,",Win[i]);
	if(!((i+1)%8)) printf("\n");
  }

  S_MelFilter.pStartIndices = &start_indices[0];
  S_MelFilter.pStopIndices  = &stop_indices[0];
  S_MelFilter.pCoefficients = &melFilterLut[0];
  S_MelFilter.NumMels       = NUM_MEL;
  S_MelFilter.FFTLen        = FFT_LEN;
  S_MelFilter.SampRate      = SMP_RATE;
  S_MelFilter.FMin          = 0.0;
  S_MelFilter.FMax          = SMP_RATE / 2.0;
  S_MelFilter.Formula       = MEL_SLANEY;
  S_MelFilter.Normalize     = 1;
  S_MelFilter.Mel2F         = 1;

  MelFilterbank_Init(&S_MelFilter);
  if (S_MelFilter.CoefficientsLength != NUM_MEL_COEF){
	while(1); /* Adjust NUM_MEL_COEFS to match S_MelFilter.CoefficientsLength */
  }
  printf("Mel coefs : \n");
  for (i=0;i<NUM_MEL_COEF;i++)
  {
	  printf("%.10e,",melFilterLut[i]);
	  if(!((i+1)%8)) printf("\n");
  }
  printf("\nstart idx : \n");
  for (i=0;i<NUM_MEL;i++)
  {
	  printf("%4lu,",start_indices[i]);
	  if(!((i+1)%8)) printf("\n");
  }
  printf("stop  idx : \n");
  for (i=0;i<NUM_MEL;i++)
  {
	  printf("%4lu,",stop_indices[i]);
	  if(!((i+1)%8)) printf("\n");
  }
  printf("\n DCT table \n");

  S_DCT.NumFilters      = NUM_MFCC;
  S_DCT.NumInputs       = NUM_MEL;
  S_DCT.Type            = DCT_TYPE_II_ORTHO;
  S_DCT.RemoveDCTZero   = 0;
  S_DCT.pDCTCoefs       = dct;
  if (DCT_Init(&S_DCT) != 0)
  {
	while(1);
  }
  for (i=0;i<NUM_MEL * NUM_MFCC;i++)
  {
	  printf("%.10e,",dct[i]);
	  if(!((i+1)%8)) printf("\n");
  }
  printf("\n");
}
#endif


/* IDPU2 virtual functions definition */
/**************************************/

sys_error_code_t MfccDPU_vtblProcess(IDPU2_t *_this, EMData_t in_data, EMData_t out_data)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MfccDPU_t *p_obj = (MfccDPU_t*)_this;

  float *p_in =  (float *)EMD_Data(&in_data);
  float *p_out = (float *)EMD_2dDataAt(&out_data, p_obj->mfccRawIdx++, 0U);
  MfccColumn(&p_obj->S_Mfcc, p_in , p_out);   	  /* call Mfcc library. */

  if (p_obj->mfccRawIdx >= MFCCDPU_NUM_MFCC_RAW)
  {
    p_obj->mfccRawIdx = 0;
  }
  else
  {
    /*notify the caller that the processed output data is not ready yet.*/
    res = SYS_ADPU2_PROC_DATA_NOT_READY_ERROR_CODE;
  }

  return res;
}

/* Private member function definition */
/**************************************/

