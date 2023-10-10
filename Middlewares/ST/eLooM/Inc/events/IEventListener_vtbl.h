/**
  ******************************************************************************
  * @file    IEventListener_vtbl.h
  * @author  STMicroelectronics - ST-Korea - MCD Team
  * @version 3.0.0
  * @date    Apr 6, 2017
  *
  * @brief   IEventListener virtual table definition.
  *
  * This file declares the virtual table for the IEventListener interface.
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
#ifndef INCLUDE_EVENTS_IEVENTLISTENERVTBL_H_

#define INCLUDE_EVENTS_IEVENTLISTENERVTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "events/IListener_vtbl.h"


typedef struct _IEventListener_vtbl IEventListener_vtbl;

/**
  * IEventListener virtual table. This table define all the functions
  * that a subclass must overload.
  */
struct _IEventListener_vtbl
{
  sys_error_code_t (*OnStatusChange)(IListener *_this);    ///< @sa IListenerOnStatusChange
  void (*SetOwner)(IEventListener *_this, void *pxOwner);  ///< @sa IEventListenerSetOwner
  void *(*GetOwner)(IEventListener *_this);                ///< @sa IEventListenerGetOwner
};

/**
  * IEventListener interface definition.
  */
struct _IEventListener
{
  /**
    * Pointer to the class virtual table.
    */
  const IEventListener_vtbl *vptr;
};


/* Public API declaration */
/**************************/


/* Inline functions definition */
/*******************************/

SYS_DEFINE_STATIC_INLINE
void IEventListenerSetOwner(IEventListener *_this, void *pxOwner)
{
  _this->vptr->SetOwner(_this, pxOwner);
}

SYS_DEFINE_STATIC_INLINE
void *IEventListenerGetOwner(IEventListener *_this)
{
  return _this->vptr->GetOwner(_this);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_EVENTS_IEVENTLISTENERVTBL_H_ */
