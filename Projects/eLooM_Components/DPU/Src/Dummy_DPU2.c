/**
 ******************************************************************************
 * @file    Dummy_DPU2.c
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 2.0.0
 * @date    May 20, 2022
 *
 * @brief   Dummy DPU used as template.
 *
 *
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 ******************************************************************************
  */

#include "Dummy_DPU2.h"
#include "Dummy_DPU2_vtbl.h"
#include "services/sysdebug.h"


#define SYS_DEBUGF(level, message)                   SYS_DEBUGF3(SYS_DBG_DPU, level, message)


/**
 * Class object declaration.
 */
typedef struct _Dummy_DPU2Class
{
  /**
   * IDPU2_t class virtual table.
   */
  IDPU2_vtbl vtbl;

} Dummy_DPU2Class_t;

/**
 *  Dummy_DPU internal structure.
 */
struct _Dummy_DPU2 {
  /**
   * Base class object.
   */
  ADPU2_t super;

  uint16_t samples;

  uint8_t axis;
};

/* Objects instance */
/********************/

/**
 * The class object.
 */
static const Dummy_DPU2Class_t sTheClass = {
    /* class virtual table */
    {
        ADPU2_vtblAttachToDataSource,
        ADPU2_vtblDetachFromDataSource,
        ADPU2_vtblAttachToDPU,
        ADPU2_vtblDetachFromDPU,
        ADPU2_vtblDispatchEvents,
        ADPU2_vtblRegisterNotifyCallback,
        Dummy_DPU2_vtblProcess
    }
};


/* Private member function declaration */
/***************************************/


/* Inline function forward declaration */
/***************************************/


/* IDPU2 virtual functions definition */
/**************************************/
sys_error_code_t Dummy_DPU2_vtblProcess(IDPU2_t *_this, EMData_t in_data, EMData_t out_data)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  /*Dummy_DPU2_t *p_obj = (Dummy_DPU2_t*) _this;*/

  float p_data_acc_x = 0.0f;
  float p_data_acc_y = 0.0f;
  float p_data_acc_z = 0.0f;
#if defined (SYS_DEBUG)
  uint32_t elements = EMD_GetElementsCount(&in_data);
#endif

  /**
   * Get first element: Acc_x, Acc_y, Acc_z
   */
  EMD_2dGetValueAt(&in_data, &p_data_acc_x, 0, 0);
  EMD_2dGetValueAt(&in_data, &p_data_acc_y, 0, 1);
  EMD_2dGetValueAt(&in_data, &p_data_acc_z, 0, 2);

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("DUMMY_DPU: processing data \r\n"));
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("DUMMY_DPU: num elements: %d \r\n\n", elements));
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("\033[0;35mDUMMY_DPU: Acc_x: \033[0;39m %f \r\n",   p_data_acc_x));
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("\033[0;33mDUMMY_DPU: Acc_y: \033[0;39m %f \r\n",   p_data_acc_y));
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("\033[0;36mDUMMY_DPU: Acc_z: \033[0;39m %f \r\n\n", p_data_acc_z));

  return res;
}


/* Public functions definition */
/*******************************/

IDPU2_t *Dummy_DPU2Alloc()
{
  IDPU2_t *p_obj = (IDPU2_t*) SysAlloc(sizeof(Dummy_DPU2_t));

  if (p_obj != NULL)
  {
    p_obj->vptr = &sTheClass.vtbl;
  }

  return p_obj;
}

sys_error_code_t Dummy_DPU2Init(Dummy_DPU2_t *_this, uint16_t samples, uint8_t axis) {
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  _this->samples = samples;
  _this->axis = axis;

  EMData_t in_data, out_data;
  if (axis == 1)
  {
    res = EMD_Init(&in_data, NULL, E_EM_FLOAT, E_EM_MODE_LINEAR, 1, samples);

  }
  else
  {
    res = EMD_Init(&in_data, NULL, E_EM_FLOAT, E_EM_MODE_INTERLEAVED, 2, samples, axis);
  }
  if (SYS_IS_ERROR_CODE(res))
  {
    sys_error_handler();
  }

  res = EMD_Init(&out_data, NULL, E_EM_INT16, E_EM_MODE_LINEAR, 1, 3);
  if (SYS_IS_ERROR_CODE(res))
  {
    sys_error_handler();
  }

  /*initialize the base class*/
  res = ADPU2_Init((ADPU2_t*)_this, in_data, out_data);

  return res;
}

/* Private function definition */
/*******************************/


