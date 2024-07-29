/**
  ******************************************************************************
  * @file    NeaiNccDPU.c
  * @author  SRA
  * @brief   Define the NeaiDPIU.
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

#include "NeaiNccDPU.h"
#include "services/sysmem.h"
#include "services/sysdebug.h"

/**
  * Private Definition
  */
#define NEAI_NCC_PREDICTION_RETVAL_IDX 0
#define NEAI_NCC_PREDICTION_PREDICTED_CLASS_IDX  1
#define NEAI_NCC_PREDICTION_PREDICTION_CLASSES_IDX  2


#define SYS_DEBUGF(level, message)  SYS_DEBUGF3(SYS_DBG_NEAINCC, level, message)

/**
  * Class object declaration.
  */
typedef struct _NeaiNccDPU_Class
{
  /**
    * IDPU2_t class virtual table.
    */
  IDPU2_vtbl vtbl;

} NeaiNccDPU_Class_t;


/* Objects instance */
/********************/

/**
  * The class object.
  */
static const NeaiNccDPU_Class_t sTheClass =
{
  /* class virtual table */
  {
    ADPU2_vtblAttachToDataSource,
    ADPU2_vtblDetachFromDataSource,
    ADPU2_vtblAttachToDPU,
    ADPU2_vtblDetachFromDPU,
    ADPU2_vtblDispatchEvents,
    ADPU2_vtblRegisterNotifyCallback,
    NeaiNccDPU_vtblProcess
  }
};


/* Private member function declaration */
/***************************************/


/* Inline function forward declaration */
/***************************************/


/* IDPU2 virtual functions definition */
/**************************************/

sys_error_code_t NeaiNccDPU_vtblProcess(IDPU2_t *_this, EMData_t in_data, EMData_t out_data)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  NeaiNccDPU_t *p_obj = (NeaiNccDPU_t *)_this;
  float *p_out = (float *)EMD_Data(&out_data);

  uint8_t status;
  float *p_signal = (float *)EMD_Data(&in_data);

  uint16_t id_class;
  status = INeaiNcc_Classification((INeaiNcc_Model_t *)p_obj->p_ANeaiNcc_Model, p_signal, &p_out[NEAI_NCC_PREDICTION_PREDICTION_CLASSES_IDX], &id_class);

  /* Set NanoEdge classification return value */
  p_out[NEAI_NCC_PREDICTION_RETVAL_IDX] = (float)status;

  /* Set NanoEdge prediction class index */
  p_out[NEAI_NCC_PREDICTION_PREDICTED_CLASS_IDX] = (float)id_class;

  return res;
}


/* Public API functions definition */
/***********************************/

IDPU2_t *NeaiNccDPU_Alloc()
{
  NeaiNccDPU_t *p_obj = (NeaiNccDPU_t *) SysAlloc(sizeof(NeaiNccDPU_t));

  if (p_obj != NULL)
  {
    ((IDPU2_t *)p_obj)->vptr = &sTheClass.vtbl;
  }

  return (IDPU2_t *)p_obj;
}

IDPU2_t *NeaiNccDPU_StaticAlloc(void *p_mem_block)
{
  NeaiNccDPU_t *p_obj = (NeaiNccDPU_t *)p_mem_block;

  if (p_obj != NULL)
  {
    ((IDPU2_t *)p_obj)->vptr = &sTheClass.vtbl;
  }

  return (IDPU2_t *)p_obj;
}

sys_error_code_t NeaiNccDPU_Init(NeaiNccDPU_t *_this, INeaiNcc_Model_t *p_aneaincc_model)
{
  assert_param(_this != NULL);
  assert_param(p_aneaincc_model != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint8_t in_axes;
  uint16_t in_signal_size, out_signal_size;
  ANeaiNcc_Model_t *p_neaiNccModel = (ANeaiNcc_Model_t *)p_aneaincc_model;

  /**
    * Register nanoEdge nCC model interface into the DPU
    */
  _this->p_ANeaiNcc_Model = p_neaiNccModel;

  in_axes = p_neaiNccModel->axis_number;
  in_signal_size = p_neaiNccModel->data_input_user_dim;
  out_signal_size = 2 + p_neaiNccModel->class_number;

  /* Prepare the EMData_t to initialize the base class.*/
  EMData_t in_data, out_data;
  if (in_axes == 1)
  {
    res = EMD_Init(&in_data, NULL, E_EM_FLOAT, E_EM_MODE_LINEAR, 1, in_signal_size);

  }
  else
  {
    res = EMD_Init(&in_data, NULL, E_EM_FLOAT, E_EM_MODE_INTERLEAVED, 2, in_signal_size, in_axes);
  }
  if (SYS_IS_ERROR_CODE(res))
  {
    sys_error_handler();
  }

  res = EMD_Init(&out_data, NULL, E_EM_FLOAT, E_EM_MODE_LINEAR, 1, out_signal_size);
  if (SYS_IS_ERROR_CODE(res))
  {
    sys_error_handler();
  }

  /* initialize NanoEdge nCC AI library */
  res =  INeaiNcc_Init((INeaiNcc_Model_t *)p_neaiNccModel, p_neaiNccModel->p_knowledge);
  if (SYS_IS_ERROR_CODE(res))
  {
    sys_error_handler();
  }

  /*initialize the base class.*/
  res = ADPU2_Init((ADPU2_t *)_this, in_data, out_data);
  if (SYS_IS_ERROR_CODE(res))
  {
    sys_error_handler();
  }

  return res;
}
