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
#include "ble_stream_class.h"

#include "ICommandParse.h"
#include "ICommandParse_vtbl.h"
#include "PnPLCompManager.h"

#include "App_model.h"

#include "ISM330DHCXTask.h"
#include "services/SQuery.h"
#include "services/SUcfProtocol.h"


/* Datalog messages ID */
#define DT_USER_BUTTON                            (0x0010)
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

/**
  *  DatalogAppTask internal structure.
  */
struct _DatalogAppTask
{
  /**
    * Base class object.
    */
  AManagedTaskEx super;

  TX_QUEUE in_queue;

  /** Software timer used to send periodical ble advertise messages **/
  TX_TIMER ble_advertise_timer;

  /**
    * Data Event Listener
    */
  IDataEventListener_t sensorListener;
  void *owner;

  /**
    * USBX ctrl class
    */
  usbx_dctrl_class_t *usbx_device;

  /**
    * FileX ctrl class
    */
  filex_dctrl_class_t *filex_device;

  /** FILEX ctrl class **/
  ble_stream_class_t *ble_device;

  ICommandParse_t parser;

//TODO could be more useful to have a CommandParse Class? (ICommandParse + PnPLCommand_t)
  PnPLCommand_t outPnPLCommand;

  /** SensorLL interface for MLC
   */
  ISensorLL_t *mlc_sensor_ll;

  AppModel_t *datalog_model;

  SensorContext_t sensorContext[SM_MAX_SENSORS];

  uint32_t mode;  /* logging interface */

  filex_threshold_config_t filex_threshold_config;

};


// Public API declaration
//***********************

/**
  * Allocate an instance of DatalogAppTask.
  *
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *DatalogAppTaskAlloc(void);

IEventListener *DatalogAppTask_GetEventListenerIF(DatalogAppTask *_this);

ICommandParse_t *DatalogAppTask_GetICommandParseIF(DatalogAppTask *_this);

uint8_t DatalogAppTask_start_vtbl(int32_t interface);
uint8_t DatalogAppTask_stop_vtbl(void);
uint8_t DatalogAppTask_save_config_vtbl(void);
uint8_t DatalogAppTask_set_time_vtbl(const char *datetime);
uint8_t DatalogAppTask_switch_bank_vtbl(void);
uint8_t DatalogAppTask_set_dfu_mode(void);
uint8_t DatalogAppTask_enable_all(bool);

void DatalogApp_Task_command_response_cb(char *response_msg, uint32_t size);

uint8_t DatalogAppTask_SetMLCIF(AManagedTask *task_obj);
uint8_t DatalogAppTask_load_ism330dhcx_ucf_vtbl(const char *ucf_data, int32_t ucf_size);

sys_error_code_t DatalogAppTask_msg(ULONG msg);

uint8_t DatalogAppTask_load_ucf(const char *ucf_data, uint32_t ucf_size, const char *output_data, int32_t output_size);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* DatalogAppTASK_H_ */
