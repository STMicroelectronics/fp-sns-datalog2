/**
  ******************************************************************************
  * @file    AI_HAR_DPU.c
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

/* Includes ------------------------------------------------------------------*/
#include "AI_HAR_DPU.h"
#include "AI_HAR_DPU_vtbl.h"
#include "services/sysmem.h"
#include "services/sysdebug.h"
#include "filter_gravity.h"

/* Private define ------------------------------------------------------------*/
#define SYS_DEBUGF(level, message)  SYS_DEBUGF3(SYS_DBG_AI_HAR, level, message)
#define AI_HAR_DPU_G_TO_MS_2 (9.8F)

/**
 * Class object declaration.
 */
typedef struct _AI_HAR_DPU_Class
{
  /**
   * IDPU2_t class virtual table.
   */
  IDPU2_vtbl vtbl;

} AI_HAR_DPU_Class_t;

/* Objects instance */
/********************/

/**
 * The class object.
 */
static const AI_HAR_DPU_Class_t sTheClass =
{
  /* class virtual table */
  {
	ADPU2_vtblAttachToDataSource,
	ADPU2_vtblDetachFromDataSource,
	ADPU2_vtblAttachToDPU,
	ADPU2_vtblDetachFromDPU,
	ADPU2_vtblDispatchEvents,
	ADPU2_vtblRegisterNotifyCallback,
	AI_HAR_DPU_vtblProcess
  }
};

/* IDPU2 virtual functions definition */
/**************************************/

sys_error_code_t AI_HAR_DPU_vtblProcess(IDPU2_t *_this, EMData_t in_data, EMData_t out_data)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  AI_HAR_DPU_t *p_obj = (AI_HAR_DPU_t*)_this;

  float scale = p_obj->scale;
  float *p_in  = (float*)EMD_Data(&in_data);
  float *p_out = (float*)EMD_Data(&out_data);
  GRAV_input_t gravIn[AI_HAR_DPU_NB_SAMPLE_IN];
  GRAV_input_t gravOut[AI_HAR_DPU_NB_SAMPLE_IN];

  for (int i=0 ; i < AI_HAR_DPU_NB_SAMPLE_IN ; i++)
  {
    gravIn[i].AccX = *p_in++ * scale;
    gravIn[i].AccY = *p_in++ * scale;
    gravIn[i].AccZ = *p_in++ * scale;
    gravOut[i] = gravity_suppress_rotate (&gravIn[i]);
  }

  /* call Ai library. */
  p_obj->ai_processing_f(AI_HAR_DPU_NAME, (float *)gravOut, p_out);

  return res;
}

/* Exported functions --------------------------------------------------------*/

/* Public API functions definition */
/***********************************/
IDPU2_t *AI_HAR_DPU_Alloc(void)
{

  AI_HAR_DPU_t  *p_obj = (AI_HAR_DPU_t*) SysAlloc(sizeof(AI_HAR_DPU_t));

  if (p_obj != NULL)
  {
    ((IDPU2_t*)p_obj)->vptr = &sTheClass.vtbl;
    p_obj->ai_processing_f = aiProcess ;
  }
  return (IDPU2_t*)p_obj;
}

IDPU2_t *AI_HAR_DPU_StaticAlloc(void *p_mem_block)
{
  AI_HAR_DPU_t  *p_obj = (AI_HAR_DPU_t*) p_mem_block;

  if (p_obj != NULL)
  {
    ((IDPU2_t*)p_obj)->vptr = &sTheClass.vtbl;
    p_obj->ai_processing_f = aiProcess ;
  }
  return (IDPU2_t*)p_obj;
}

sys_error_code_t AI_HAR_DPU_SetSensitivity(AI_HAR_DPU_t *_this, float sensi)
{
  assert_param(_this != NULL);

  _this->scale = sensi * AI_HAR_DPU_G_TO_MS_2;

  return SYS_NO_ERROR_CODE;
}

sys_error_code_t AI_HAR_DPU_PrepareToProcessData(AI_HAR_DPU_t *_this)
{
  assert_param(_this != NULL);
  ADPU2_Reset((ADPU2_t*)_this);
  return SYS_NO_ERROR_CODE;
}

sys_error_code_t AI_HAR_DPU_Init(AI_HAR_DPU_t *_this)
{
  assert_param(_this != NULL);
  assert_param(AI_HAR_DPU_NB_AXIS_IN*AI_HAR_DPU_NB_SAMPLE_IN == AI_HAR_NETWORK_IN_1_SIZE);
  assert_param(AI_HAR_DPU_NB_SAMPLE_OUT == AI_HAR_NETWORK_OUT_1_SIZE + AI_HAR_NETWORK_OUT_2_SIZE);

  /* Prepare the EMData_t to initialize the base class.*/
  EMData_t in_data, out_data;

  if SYS_IS_ERROR_CODE(EMD_Init(&in_data, NULL, E_EM_FLOAT, E_EM_MODE_LINEAR, 2,AI_HAR_DPU_NB_SAMPLE_IN, AI_HAR_DPU_NB_AXIS_IN))
  {
	  sys_error_handler();
  }

  if SYS_IS_ERROR_CODE(EMD_Init(&out_data, NULL, E_EM_FLOAT, E_EM_MODE_LINEAR, 1, AI_HAR_DPU_NB_SAMPLE_OUT))
  {
	  sys_error_handler();
  }

  /*initialize the base class.*/
  if SYS_IS_ERROR_CODE(ADPU2_Init((ADPU2_t*)_this, in_data, out_data))
  {
	  sys_error_handler();
  }

  /* take the ownership of the Sensor Event IF. Note: it is no needed anymore, but we leave it for backward compatibility.*/
  IEventListenerSetOwner((IEventListener *) ADPU2_GetEventListenerIF((ADPU2_t *)_this), _this);

  /* initialize AI library */
  if (aiInit(AI_HAR_NETWORK_MODEL_NAME)==0)
  {
    /* link with the X-CUBE-AI library */
    _this->ai_processing_f = aiProcess;
  }
  else
  {
    sys_error_handler();
  }

  return SYS_NO_ERROR_CODE;
}
