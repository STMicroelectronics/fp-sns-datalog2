/**
  ******************************************************************************
  * @file    DatalogAppTask.h
  * @author  SRA
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
#include "filex_dctrl_class.h"

#include "ICommandParse.h"
#include "ICommandParse_vtbl.h"
#include "PnPLCompManager.h"

/* Datalog messages ID */
#define DT_USER_BUTTON                            (0x0010)
#define DT_SWITCH_BANK                            (0x0020)
#define DT_FORCE_STEP                             (0x00F0)


typedef struct
{
  double old_time_stamp;
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

DatalogAppTask *getDatalogAppTask(void);

IEventListener *DatalogAppTask_GetEventListenerIF(DatalogAppTask *_this);

ICommandParse_t *DatalogAppTask_GetICommandParseIF(DatalogAppTask *_this);

uint8_t DatalogAppTask_start_vtbl(int32_t interface);
uint8_t DatalogAppTask_stop_vtbl(void);
uint8_t DatalogAppTask_save_config_vtbl(void);
uint8_t DatalogAppTask_set_time_vtbl(const char *datetime);
uint8_t DatalogAppTask_switch_bank_vtbl(void);
void DatalogApp_Task_command_response_cb(char *response_msg, uint32_t size);

sys_error_code_t DatalogAppTask_msg(ULONG msg);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* DatalogAppTASK_H_ */
