/**
  ******************************************************************************
  * @file    IBootVtbl.h
  * @author  STMicroelectronics - ST-Korea - MCD Team
  * @version 3.0.0
  * @date    Nov 22, 2017
  *
  * @brief
  *
  * <DESCRIPTIOM>
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */
#ifndef INCLUDE_SERVICES_IBOOTVTBL_H_
#define INCLUDE_SERVICES_IBOOTVTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/eloom_sysbase.h"

/**
  * Create a type name for _IBoot_vtbl.
  */
typedef struct _IBoot_vtbl IBoot_vtbl;

/**
  * Specifies the virtual table for the ::IBoot class.
  */
struct _IBoot_vtbl
{
  sys_error_code_t (*Init)(IBoot *_this);
  boolean_t (*CheckJumpTrigger)(IBoot *_this);
  uint32_t (*GetJumpAddress)(IBoot *_this);
  sys_error_code_t (*OnJump)(IBoot *_this, uint32_t nAddress);
};

/**
  * IBoot interface internal state.
  * It declares only the virtual table used to implement the inheritance.
  */
struct _IBoot
{
  /**
    * Pointer to the virtual table for the class.
    */
  const IBoot_vtbl *vptr;
};


// Inline functions definition
// ***************************

SYS_DEFINE_STATIC_INLINE
sys_error_code_t IBootInit(IBoot *_this)
{
  return _this->vptr->Init(_this);
}

SYS_DEFINE_STATIC_INLINE
boolean_t IBootCheckJumpTrigger(IBoot *_this)
{
  return _this->vptr->CheckJumpTrigger(_this);
}

SYS_DEFINE_STATIC_INLINE
uint32_t IBootGetJumpAddress(IBoot *_this)
{
  return _this->vptr->GetJumpAddress(_this);
}

SYS_DEFINE_STATIC_INLINE
sys_error_code_t IBootOnJump(IBoot *_this, uint32_t nAddress)
{
  return _this->vptr->OnJump(_this, nAddress);
}


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERVICES_IBOOTVTBL_H_ */
