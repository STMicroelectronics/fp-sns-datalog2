/**
  ******************************************************************************
  * @file    IDataEventListener.h
  * @author  STMicroelectronics - AIS - MCD Team
  * @version M.m.b
  * @date    May 13, 2022
  *
  * @brief
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
#ifndef EMDATA_INC_EVENTS_IDATAEVENTLISTENER_H_
#define EMDATA_INC_EVENTS_IDATAEVENTLISTENER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "events/IEventListener.h"
#include "events/IEventListener_vtbl.h"
#include "events/DataEvent.h"


/**
  * Create  type name for _IDataEventListener.
  */
typedef struct _IDataEventListener IDataEventListener_t;


/* Public API declaration */
/**************************/


/* Inline functions definition */
/*******************************/


#ifdef __cplusplus
}
#endif


#endif /* EMDATA_INC_EVENTS_IDATAEVENTLISTENER_H_ */
