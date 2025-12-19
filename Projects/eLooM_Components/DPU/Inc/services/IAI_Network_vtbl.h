/**
  ********************************************************************************
  * @file    IAI_Network_vtbl.h
  * @author  STMicroelectronics - AIS - MCD Team
  * @version 1.0.0
  * @date    Oct 21, 2022
  *
  * @brief
  *
  * <ADD_FILE_DESCRIPTION>
  *
  ********************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ********************************************************************************
  */
#ifndef DPU_INC_SERVICES_IAI_NETWORK_VTBL_H_
#define DPU_INC_SERVICES_IAI_NETWORK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "services/systp.h"
#include "services/systypes.h"
#include "services/syserror.h"


/**
  * Create  type name for _IAI_Network_vtbl.
  */
typedef struct _IAI_Network_vtbl IAI_Network_vtbl;

/**
  * Specifies the virtual table for the  class.
  */
struct _IAI_Network_vtbl
{
  ai_error(*Create)(IAI_Network_t *_this, const ai_buffer *p_network_config);
  ai_error(*Create_and_Init)(IAI_Network_t *_this, const ai_handle *p_activations, const ai_handle *p_weights);
  ai_handle(*Destroy)(IAI_Network_t *_this);
  ai_bool(*Init)(IAI_Network_t *_this, const ai_network_params *p_params);
  ai_bool(*GetReport)(IAI_Network_t *_this, ai_network_report *p_report);
  ai_buffer *(*InputGet)(IAI_Network_t *_this, ai_u16 *p_buffer);
  ai_buffer *(*OutputGet)(IAI_Network_t *_this, ai_u16 *p_buffer);
  ai_i32(*Run)(IAI_Network_t *_this, const ai_buffer *p_input, ai_buffer *p_output);
  ai_error(*GetError)(IAI_Network_t *_this);
};

/**
  * IAI_Network interface internal state.
  * It declares only the virtual table used to implement the inheritance.
  */
struct _IAI_Network
{
  /**
    * Pointer to the virtual table for the class.
    */
  const IAI_Network_vtbl *vptr;
};


/* Inline functions definition */
/*******************************/

static inline ai_error IAI_Network_Create(IAI_Network_t *_this, const ai_buffer *p_network_config)
{
  return _this->vptr->Create(_this, p_network_config);
}

static inline ai_error IAI_Network_Create_and_Init(IAI_Network_t *_this, const ai_handle *p_activations,
                                                   const ai_handle *p_weights)
{
  return _this->vptr->Create_and_Init(_this, p_activations, p_weights);
}

static inline ai_handle IAI_Network_Destroy(IAI_Network_t *_this)
{
  return _this->vptr->Destroy(_this);
}

static inline ai_bool IAI_Network_Init(IAI_Network_t *_this, const ai_network_params *p_params)
{
  return _this->vptr->Init(_this, p_params);
}

static inline ai_bool IAI_Network_GetReport(IAI_Network_t *_this, ai_network_report *p_report)
{
  return _this->vptr->GetReport(_this, p_report);
}

static inline ai_buffer *IAI_Network_InputsGet(IAI_Network_t *_this, ai_u16 *p_buffer)
{
  return _this->vptr->InputGet(_this, p_buffer);
}

static inline ai_buffer *IAI_Network_OutputsGet(IAI_Network_t *_this, ai_u16 *p_buffer)
{
  return _this->vptr->OutputGet(_this, p_buffer);
}

static inline ai_i32 IAI_Network_Run(IAI_Network_t *_this, const ai_buffer *p_input, ai_buffer *p_output)
{
  return _this->vptr->Run(_this, p_input, p_output);
}

static inline ai_error IAI_Network_GetError(IAI_Network_t *_this)
{
  return _this->vptr->GetError(_this);
}

#ifdef __cplusplus
}
#endif

#endif /* DPU_INC_SERVICES_IAI_NETWORK_VTBL_H_ */
