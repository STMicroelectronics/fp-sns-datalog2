/**
  ******************************************************************************
  * @file    DatalogAppTask.h
  * @author  SRA
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */
#ifndef DatalogAppTASK_H_
#define DatalogAppTASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/AManagedTaskEx.h"
#include "services/AManagedTaskEx_vtbl.h"
#include "drivers/IDriver.h"
#include "drivers/IDriver_vtbl.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "usbx_dctrl_class.h"

#include "ICommandParse.h"
#include "ICommandParse_vtbl.h"
#include "PnPLCompManager.h"

#include "ILog_Controller.h"
#include "ILog_Controller_vtbl.h"

/* Datalog messages ID */
#define DT_USER_BUTTON                            (0x0010)
#define DT_FORCE_STEP                             (0x00F0)

typedef struct
{
  double_t old_time_stamp;
  uint16_t n_samples_to_timestamp;
} SensorContext_t;

/**
  * Create  type name for _DatalogAppTask.
  */
typedef struct _DatalogAppTask DatalogAppTask;

// Public API declaration
//***********************

/**
  * Allocate an instance of DatalogAppTask.
  *
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *DatalogAppTaskAlloc(void);

IDataEventListener_t *DatalogAppTask_GetEventListenerIF(DatalogAppTask *_this);

IDataEventListener_t *DatalogAppTask_GetSensorEventListenerIF(DatalogAppTask *_this);

ICommandParse_t *DatalogAppTask_GetICommandParseIF(DatalogAppTask *_this);

ILog_Controller_t *DatalogAppTask_GetILogControllerIF(DatalogAppTask *_this);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* DatalogAppTASK_H_ */
