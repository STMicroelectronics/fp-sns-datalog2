/**
  ******************************************************************************
  * @file    FAKESENSORTask.h
  * @author  SRA - MCD
  * @brief
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
#ifndef FAKESENSORTASK_H_
#define FAKESENSORTASK_H_

#ifdef __cplusplus
extern "C" {
#endif



#include "services/systp.h"
#include "services/syserror.h"
#include "services/AManagedTaskEx.h"
#include "services/AManagedTaskEx_vtbl.h"
#include "events/DataEventSrc.h"
#include "events/DataEventSrc_vtbl.h"
#include "ISensor.h"
#include "ISensor_vtbl.h"

/**
  * Create  type name for _FAKESENSORTask.
  */
typedef struct _FAKESENSORTask FAKESENSORTask;




// Public API declaration
//***********************

/**
  * Get the ISourceObservable interface for the accelerometer.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *FAKESENSORTaskGetMicSensorIF(FAKESENSORTask *_this);

/**
  * Allocate an instance of FAKESENSORTask.
  *
  * @param p_mx_mdf_cfg [IN] specifies a ::MX_MDFParams_t instance declared in the mx.h file.
  * @param p_mx_adc_cfg [IN] specifies a ::MX_ADCParams_t instance declared in the mx.h file.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *FAKESENSORTaskAlloc(const void *pParams);

IEventSrc *FAKESENSORTaskGetEventSrcIF(FAKESENSORTask *_this);

// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* FAKESENSORTASK_H_ */
