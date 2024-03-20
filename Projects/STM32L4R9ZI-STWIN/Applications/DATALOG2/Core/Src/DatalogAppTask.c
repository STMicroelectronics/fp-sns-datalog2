/**
  ******************************************************************************
  * @file    DatalogAppTask.c
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

#include "DatalogAppTask.h"
#include "DatalogAppTask_vtbl.h"
#include "services/sysdebug.h"
#include "services/sysmem.h"
#include "services/SysTimestamp.h"

#include "PnPLCompManager.h"
#include "UtilTask.h"
#include "ISM330DHCXTask.h"

#include "automode.h"
#include "rtc.h"


#ifndef DT_TASK_CFG_STACK_DEPTH
#define DT_TASK_CFG_STACK_DEPTH                   (TX_MINIMUM_STACK*2)
#endif

#ifndef DT_TASK_CFG_PRIORITY
#define DT_TASK_CFG_PRIORITY                      (TX_MAX_PRIORITIES-1)
#endif

#ifndef DT_TASK_CFG_IN_QUEUE_LENGTH
#define DT_TASK_CFG_IN_QUEUE_LENGTH               20
#endif

#define DT_TASK_CFG_IN_QUEUE_ITEM_SIZE            sizeof(ULONG)

#define DATALOG_APP_TASK_CFG_TIMER_PERIOD_MS      5000U

// BLE Advertise option byte
#define ADV_OB_BATTERY                            0U
#define ADV_OB_ALARM                              1U
#define ADV_OB_ICON                               2U

#define COMM_ID_SDCARD                            0U
#define COMM_ID_USB                               1U
#define COMM_ID_BLE                               2U

#define BOOTLOADER_ADDRESS                        0x1FFF0000

#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_DT, level, message)

#if defined(DEBUG) || defined (SYS_DEBUG)
#define sTaskObj                                  sDatalogAppTaskObj
#endif


/**
  * Class object declaration
  */
typedef struct _DatalogAppTaskClass
{
  /**
    * DatalogAppTask class virtual table.
    */
  AManagedTaskEx_vtbl vtbl;

  /**
    * ICommandParse virtual table.
    */
  ICommandParse_vtbl parser_vtbl;

  /**
    * IDataEventListener virtual table.
    */
  IDataEventListener_vtbl Listener_vtbl;

  /**
    * ILogController virtual table.
    */
  ILog_Controller_vtbl logCtrl_vtbl;

  /**
    * IIsm330dhcx_Mlc virtual table.
    */
  IIsm330dhcx_Mlc_vtbl MLCCtrl_vtbl;

  /**
    * DatalogAppTask (PM_STATE, ExecuteStepFunc) map.
    */
  pExecuteStepFunc_t p_pm_state2func_map[];
} DatalogAppTaskClass_t;

// Private member function declaration
// ***********************************

/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t DatalogAppTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t DatalogAppTaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Callback function called when the software timer, dedicated to the advertise option bytes update, expires.
  *
  * @param xTimer [IN] specifies the handle of the expired timer.
  */
static void DatalogAppTaskAdvOBTimerCallbackFunction(ULONG timer);

/**
  * Update streaming status and allocate/deallocate iStream instances
  *
  * @param p_obj [IN] specifies a pointer to a DatalogAppTask object.
  * @param p_istream [IN] specifies a pointer to a IStream_t object.
  * @param streaming_status [IN] true in case o "Start streaming", false otherwise
  * @param interface [IN] interface type
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t DatalogAppTask_UpdateStreamingStatus(DatalogAppTask *p_obj, IStream_t *p_istream,
                                                             bool streaming_status, int8_t interface);


/* Objects instance */
/********************/

ULONG message;

/**
  * The only instance of the task object.
  */
static DatalogAppTask sTaskObj;

/**
  * The class object.
  */
static const DatalogAppTaskClass_t sTheClass =
{
  /* class virtual table */
  {
    DatalogAppTask_vtblHardwareInit,
    DatalogAppTask_vtblOnCreateTask,
    DatalogAppTask_vtblDoEnterPowerMode,
    DatalogAppTask_vtblHandleError,
    DatalogAppTask_vtblOnEnterTaskControlLoop,
    DatalogAppTask_vtblForceExecuteStep,
    DatalogAppTask_vtblOnEnterPowerMode
  },
  {
    DatalogAppTask_vtblICommandParse_t_parse_cmd,
    DatalogAppTask_vtblICommandParse_t_serialize_response,
    DatalogAppTask_vtblICommandParse_t_send_ctrl_msg
  },
  {
    DatalogAppTask_OnStatusChange_vtbl,
    DatalogAppTask_SetOwner_vtbl,
    DatalogAppTask_GetOwner_vtbl,
    DatalogAppTask_OnNewDataReady_vtbl
  },
  {
    DatalogAppTask_save_config_vtbl,
    DatalogAppTask_start_vtbl,
    DatalogAppTask_stop_vtbl,
    DatalogAppTask_set_time_vtbl,
    DatalogAppTask_switch_bank_vtbl,
    DatalogAppTask_set_dfu_mode
  },
  {
    DatalogAppTask_load_ism330dhcx_ucf_vtbl
  },
  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    DatalogAppTaskExecuteStepState1,
    NULL,
    DatalogAppTaskExecuteStepDatalog
  }
};

// Public API definition
// *********************

AManagedTaskEx *DatalogAppTaskAlloc()
{
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&sTaskObj.super);

  sTaskObj.super.vptr = &sTheClass.vtbl;
  sTaskObj.parser.vptr = &sTheClass.parser_vtbl;
  sTaskObj.sensorListener.vptr = &sTheClass.Listener_vtbl;
  sTaskObj.pnplLogCtrl.vptr = &sTheClass.logCtrl_vtbl;
  sTaskObj.pnplMLCCtrl.vptr = &sTheClass.MLCCtrl_vtbl;

  memset(&sTaskObj.outPnPLCommand, 0, sizeof(PnPLCommand_t));

  return (AManagedTaskEx *) &sTaskObj;
}

IEventListener *DatalogAppTask_GetEventListenerIF(DatalogAppTask *_this)
{
  assert_param(_this);

  return (IEventListener *) &_this->sensorListener;
}

ICommandParse_t *DatalogAppTask_GetICommandParseIF(DatalogAppTask *_this)
{
  assert_param(_this != NULL);
  DatalogAppTask *p_obj = (DatalogAppTask *) _this;

  return &p_obj->parser;

}

ILog_Controller_t *DatalogAppTask_GetILogControllerIF(DatalogAppTask *_this)
{
  assert_param(_this != NULL);
  DatalogAppTask *p_obj = (DatalogAppTask *) _this;

  return &p_obj->pnplLogCtrl;
}

IIsm330dhcx_Mlc_t *DatalogAppTask_GetIMLCControllerIF(DatalogAppTask *_this, AManagedTask *task_obj)
{
  assert_param(_this != NULL);
  DatalogAppTask *p_obj = (DatalogAppTask *) _this;

  /* Store SensorLL interface for ISM330DHCX Sensor */
  p_obj->mlc_sensor_ll = ISM330DHCXTaskGetSensorLLIF((ISM330DHCXTask *) task_obj);

  return &p_obj->pnplMLCCtrl;
}

sys_error_code_t DatalogAppTask_msg(ULONG msg)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ULONG message = msg;

  if (tx_queue_send(&sTaskObj.in_queue, &message, TX_NO_WAIT) != TX_SUCCESS)
  {
    res = 1;
  }

  return res;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t DatalogAppTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  // DatalogAppTask *p_obj = (DatalogAppTask*) _this;

#ifdef ENABLE_THREADX_DBG_PIN
  /* Configure DEBUG PIN */
  BSP_DEBUG_PIN_Off(CON34_PIN_5);
  BSP_DEBUG_PIN_Off(CON34_PIN_12);
  BSP_DEBUG_PIN_Off(CON34_PIN_14);
  BSP_DEBUG_PIN_Off(CON34_PIN_16);
  BSP_DEBUG_PIN_Off(CON34_PIN_22);
  BSP_DEBUG_PIN_Off(CON34_PIN_24);
  BSP_DEBUG_PIN_Off(CON34_PIN_26);
  BSP_DEBUG_PIN_Off(CON34_PIN_28);
  BSP_DEBUG_PIN_Off(CON34_PIN_30);
  BSP_DEBUG_PIN_Off(CON34_PIN_30);

  BSP_DEBUG_PIN_Init(CON34_PIN_5);
  BSP_DEBUG_PIN_Init(CON34_PIN_12);
  BSP_DEBUG_PIN_Init(CON34_PIN_14);
  BSP_DEBUG_PIN_Init(CON34_PIN_16);
  BSP_DEBUG_PIN_Init(CON34_PIN_22);
  BSP_DEBUG_PIN_Init(CON34_PIN_24);
  BSP_DEBUG_PIN_Init(CON34_PIN_26);
  BSP_DEBUG_PIN_Init(CON34_PIN_28);
  BSP_DEBUG_PIN_Init(CON34_PIN_30);
  BSP_DEBUG_PIN_Init(CON34_PIN_30);
#endif

  return res;
}

sys_error_code_t DatalogAppTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                                 VOID **pvStackStart,
                                                 ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                                 ULONG *pParams)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask *) _this;

  // Create task specific sw resources.

  uint16_t item_size = DT_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(DT_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "DatalogApp_Q", TX_1_ULONG, p_queue_items_buff,
                                    DT_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* create the software timer for advertise messages*/
  if (TX_SUCCESS != tx_timer_create(&p_obj->ble_advertise_timer, "BLE_ADV_T", DatalogAppTaskAdvOBTimerCallbackFunction,
                                    (ULONG)TX_NULL,
                                    AMT_MS_TO_TICKS(DATALOG_APP_TASK_CFG_TIMER_PERIOD_MS), AMT_MS_TO_TICKS(DATALOG_APP_TASK_CFG_TIMER_PERIOD_MS),
                                    TX_NO_ACTIVATE))
  {
    res = SYS_APP_TASK_INIT_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  uint8_t ii;
  for (ii = 0; ii < SM_MAX_SENSORS; ii++)
  {
    p_obj->sensorContext[ii].n_samples_to_timestamp = 0;
    p_obj->sensorContext[ii].old_time_stamp = -1.0f;
  }

  p_obj->datalog_model = getAppModel();

  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "DatalogApp";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = DT_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = DT_TASK_CFG_PRIORITY;
  *pPreemptThreshold = DT_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  IEventListenerSetOwner((IEventListener *) &p_obj->sensorListener, (void *) p_obj);

  p_obj->usbx_device = (usbx_dctrl_class_t *) usbx_dctrl_class_alloc((void *) &MX_PCDInitParams);
  IStream_init((IStream_t *) p_obj->usbx_device, COMM_ID_USB, 0);
  IStream_set_parse_IF((IStream_t *) p_obj->usbx_device, DatalogAppTask_GetICommandParseIF(p_obj));

  p_obj->filex_device = (filex_dctrl_class_t *) filex_dctrl_class_alloc();
  IStream_init((IStream_t *) p_obj->filex_device, COMM_ID_SDCARD, 0);
  IStream_set_parse_IF((IStream_t *) p_obj->filex_device, DatalogAppTask_GetICommandParseIF(p_obj));

  p_obj->ble_device = (ble_stream_class_t *) ble_stream_class_alloc();
  IStream_init((IStream_t *) p_obj->ble_device, COMM_ID_BLE, 0);
  IStream_set_parse_IF((IStream_t *) p_obj->ble_device, DatalogAppTask_GetICommandParseIF(p_obj));

  return res;
}


sys_error_code_t DatalogAppTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask *) _this;
  int8_t interface = p_obj->datalog_model->log_controller_model.interface;

  if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE && NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (interface == LOG_CTRL_MODE_SD) /*stop command from SD*/
    {
      /* Stop the SD interface */
      IStream_stop((IStream_t *) p_obj->filex_device);
      DatalogAppTask_UpdateStreamingStatus(p_obj, (IStream_t *) p_obj->filex_device, FALSE, interface);

      p_obj->datalog_model->log_controller_model.interface = -1;
      /* Reactivate USB interface */
      IStream_enable((IStream_t *) p_obj->usbx_device);
    }
    else if (interface == LOG_CTRL_MODE_USB) /*stop command from USB*/
    {
      /* Stop the USB interface */
      IStream_stop((IStream_t *) p_obj->usbx_device);
      DatalogAppTask_UpdateStreamingStatus(p_obj, (IStream_t *) p_obj->usbx_device, FALSE, interface);
      p_obj->datalog_model->log_controller_model.interface = -1;
      /* Reactivate SD interface */
      IStream_enable((IStream_t *) p_obj->filex_device);
    }
    SysTsStop(SysGetTimestampSrv());
  }
  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    SysTsStart(SysGetTimestampSrv(), TRUE);
  }

  return res;
}

sys_error_code_t DatalogAppTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  DatalogAppTask *p_obj = (DatalogAppTask*)_this;

  return res;
}

sys_error_code_t DatalogAppTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask *) _this;
  ULONG msg = FILEX_DCTRL_CMD_INIT;

#ifdef ENABLE_THREADX_DBG_PIN
  p_obj->super.m_xTaskHandle.pxTaskTag = DT_TASK_CFG_TAG;
#endif

  IStream_enable((IStream_t *) p_obj->usbx_device);
  IStream_enable((IStream_t *) p_obj->filex_device);
  IStream_enable((IStream_t *) p_obj->ble_device);

  filex_dctrl_msg(p_obj->filex_device, &msg);

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("DatalogApp: start.\r\n"));

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t DatalogAppTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask *) _this;

  message = DT_FORCE_STEP;

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      if (tx_queue_front_send(&p_obj->in_queue, &message, AMT_MS_TO_TICKS(100)) != TX_SUCCESS)
      {
        res = SYS_APP_TASK_MSG_LOST_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_APP_TASK_MSG_LOST_ERROR_CODE);
      }
    }
  }
  else
  {
    UINT state;
    if (TX_SUCCESS == tx_thread_info_get(&_this->m_xTaskHandle, TX_NULL, &state, TX_NULL, TX_NULL, TX_NULL, TX_NULL,
                                         TX_NULL, TX_NULL))
    {
      if (state == TX_SUSPENDED)
      {
        tx_thread_resume(&_this->m_xTaskHandle);
      }
    }
  }

  return res;
}

sys_error_code_t DatalogAppTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  DatalogAppTask *p_obj = (DatalogAppTask*)_this;

  AMTExSetPMClass(_this, E_PM_CLASS_2);

  return res;
}

//// IIListener virtual functions
sys_error_code_t DatalogAppTask_OnStatusChange_vtbl(IListener *_this)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

// ISensorEventListener virtual functions
void *DatalogAppTask_GetOwner_vtbl(IEventListener *_this)
{
  DatalogAppTask *p_obj = (DatalogAppTask *)((uint32_t) _this - offsetof(DatalogAppTask, sensorListener));

  return p_obj->owner;
}

void DatalogAppTask_SetOwner_vtbl(IEventListener *_this, void *p_owner)
{
  DatalogAppTask *p_obj = (DatalogAppTask *)((uint32_t) _this - offsetof(DatalogAppTask, sensorListener));

  p_obj->owner = p_owner;
}

sys_error_code_t DatalogAppTask_OnNewDataReady_vtbl(IEventListener *_this, const DataEvent_t *p_evt)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask *) IEventListenerGetOwner(_this);

  uint8_t nPMState = (uint8_t) AMTGetTaskPowerMode((AManagedTask *) p_obj);
  uint16_t sId = p_evt->tag;
  uint32_t nBytesPerSample = SMGetnBytesPerSample(sId);
  uint8_t *data_buf = EMD_Data(p_evt->p_data);
  uint32_t samplesToSend = EMD_GetPayloadSize(p_evt->p_data) / nBytesPerSample;

  if (nPMState == E_POWER_MODE_SENSORS_ACTIVE)
  {
    uint8_t stream_id = p_obj->datalog_model->s_models[sId]->stream_params.stream_id;
    if (res != 0)
    {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
      return res;
    }

    if (p_obj->sensorContext[sId].old_time_stamp == -1.0f)
    {
      p_obj->datalog_model->s_models[sId]->stream_params.ioffset = p_evt->timestamp;
      p_obj->sensorContext[sId].old_time_stamp = p_evt->timestamp;
      p_obj->sensorContext[sId].n_samples_to_timestamp = p_obj->datalog_model->s_models[sId]->stream_params.spts;
    }

    while (samplesToSend > 0)
    {
      /* n_samples_to_timestamp = 0 if user setup spts = 0 (no timestamp needed) */
      if (p_obj->sensorContext[sId].n_samples_to_timestamp == 0
          || samplesToSend < p_obj->sensorContext[sId].n_samples_to_timestamp)
      {
        if (IStream_is_enabled((IStream_t *) p_obj->usbx_device))
        {
          res = IStream_post_data((IStream_t *) p_obj->usbx_device, stream_id, data_buf, samplesToSend * nBytesPerSample);
        }
        if (IStream_is_enabled((IStream_t *) p_obj->filex_device))
        {
          res = IStream_post_data((IStream_t *) p_obj->filex_device, stream_id, data_buf, samplesToSend * nBytesPerSample);
        }
        if (IStream_is_enabled((IStream_t *) p_obj->ble_device))
        {
          res = IStream_post_data((IStream_t *) p_obj->ble_device,  sId, data_buf, samplesToSend * nBytesPerSample);
        }
        if (p_obj->sensorContext[sId].n_samples_to_timestamp != 0)
        {
          p_obj->sensorContext[sId].n_samples_to_timestamp -= samplesToSend;
        }
        samplesToSend = 0;
      }
      else
      {
        if (IStream_is_enabled((IStream_t *) p_obj->usbx_device))
        {
          res = IStream_post_data((IStream_t *) p_obj->usbx_device, stream_id, data_buf, p_obj->sensorContext[sId].n_samples_to_timestamp * nBytesPerSample);
        }
        if (IStream_is_enabled((IStream_t *) p_obj->filex_device))
        {
          res = IStream_post_data((IStream_t *) p_obj->filex_device, stream_id, data_buf, p_obj->sensorContext[sId].n_samples_to_timestamp * nBytesPerSample);
        }
        if (IStream_is_enabled((IStream_t *) p_obj->ble_device))
        {
          res = IStream_post_data((IStream_t *) p_obj->ble_device,  sId, data_buf, p_obj->sensorContext[sId].n_samples_to_timestamp * nBytesPerSample);
        }

        data_buf += p_obj->sensorContext[sId].n_samples_to_timestamp * nBytesPerSample;
        samplesToSend -= p_obj->sensorContext[sId].n_samples_to_timestamp;

        float measuredODR;
        double newTS;

        SensorStatus_t sensor_status = SMSensorGetStatus(sId);
        if (sensor_status.isensor_class == ISENSOR_CLASS_MEMS)
        {
          measuredODR = sensor_status.type.mems.measured_odr;
        }
        else if (sensor_status.isensor_class == ISENSOR_CLASS_AUDIO)
        {
          measuredODR = sensor_status.type.audio.frequency;
        }
        else
        {
          SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
          return res;
        }

        if (measuredODR != 0.0f)
        {
          newTS = p_evt->timestamp - ((1.0 / (double) measuredODR) * samplesToSend);
        }
        else
        {
          newTS = p_evt->timestamp;
        }

        if (IStream_is_enabled((IStream_t *) p_obj->usbx_device))
        {
          res = IStream_post_data((IStream_t *) p_obj->usbx_device, stream_id, (uint8_t *) &newTS, 8);
        }
        if (IStream_is_enabled((IStream_t *) p_obj->filex_device))
        {
          res = IStream_post_data((IStream_t *) p_obj->filex_device, stream_id, (uint8_t *) &newTS, 8);
        }
        if (res != 0)
        {
          SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
          return res;
        }
        p_obj->sensorContext[sId].n_samples_to_timestamp = p_obj->datalog_model->s_models[sId]->stream_params.spts;
      }
    }
  }
  return res;
}

/* ICommandParse_t virtual functions */
sys_error_code_t DatalogAppTask_vtblICommandParse_t_parse_cmd(ICommandParse_t *_this, char *commandString,
                                                              uint8_t comm_interface_id)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  DatalogAppTask *p_obj = (DatalogAppTask *)((uint32_t) _this - offsetof(DatalogAppTask, parser));

  int pnp_res = PnPLParseCommand(commandString, &p_obj->outPnPLCommand);
  if (pnp_res == SYS_NO_ERROR_CODE)
  {
    if (IStream_is_enabled((IStream_t *) p_obj->usbx_device) && (comm_interface_id == COMM_ID_USB))
    {
      IStream_set_mode((IStream_t *) p_obj->usbx_device, TRANSMIT);
    }
    else if (IStream_is_enabled((IStream_t *) p_obj->filex_device) && (comm_interface_id == COMM_ID_SDCARD))
    {
      IStream_set_mode((IStream_t *) p_obj->filex_device, TRANSMIT);
    }
    else if (IStream_is_enabled((IStream_t *) p_obj->ble_device) && (comm_interface_id == COMM_ID_BLE))
    {
      if (p_obj->outPnPLCommand.comm_type != PNPL_CMD_GET && p_obj->outPnPLCommand.comm_type != PNPL_CMD_SYSTEM_INFO)
      {
        /* No need to send response to SET or COMMAND messages, with the current version of the BLE App */
        /* Since the serialize_response is not called in this case, the outPnPLCommand.response is deallocated here */
        pnpl_free(p_obj->outPnPLCommand.response);
      }
      else
      {
        IStream_set_mode((IStream_t *) p_obj->ble_device, TRANSMIT);
      }
    }
    else
    {
      /**/
    }
  }
  else
  {
    if (p_obj->outPnPLCommand.comm_type == PNPL_CMD_ERROR)
    {
      IStream_set_mode((IStream_t *) p_obj->usbx_device, TRANSMIT);
    }
  }

  return res;
}

sys_error_code_t DatalogAppTask_vtblICommandParse_t_serialize_response(ICommandParse_t *_this, char **response_name,
                                                                       char **buff, uint32_t *size,
                                                                       uint8_t pretty)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask *)((uint32_t) _this - offsetof(DatalogAppTask, parser));

  PnPLSerializeResponse(&p_obj->outPnPLCommand, buff, size, pretty);

  *response_name = p_obj->outPnPLCommand.comp_name;

  return res;
}


sys_error_code_t DatalogAppTask_vtblICommandParse_t_send_ctrl_msg(ICommandParse_t *_this,  uint32_t *msg,
                                                                  uint32_t length)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask *)((uint32_t) _this - offsetof(DatalogAppTask, parser));

  switch (*msg)
  {
    case BLE_ISTREAM_MSG_START_ADV_OB:

      if (TX_SUCCESS != tx_timer_activate(&p_obj->ble_advertise_timer))
      {
        res = SYS_APP_TASK_TIMER_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
      }

      break;

    case BLE_ISTREAM_MSG_STOP_ADV_OB:

      if (TX_SUCCESS != tx_timer_deactivate(&p_obj->ble_advertise_timer))
      {
        res = SYS_APP_TASK_TIMER_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
      }

      break;

    default:
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      break;
  }

  return res;
}

// ILogController_t virtual functions
uint8_t DatalogAppTask_start_vtbl(ILog_Controller_t *_this, int32_t interface)
{
  DatalogAppTask *p_obj = (DatalogAppTask *)((uint32_t) _this - offsetof(DatalogAppTask, pnplLogCtrl));
  bool status;

  log_controller_get_log_status(&status);

  if (!status)
  {

    p_obj->datalog_model->acquisition_info_model.interface = interface;

    if (interface == LOG_CTRL_MODE_SD) /*Start log on SD*/
    {

      IStream_disable((IStream_t *) p_obj->usbx_device);
      if (IStream_is_enabled((IStream_t *) p_obj->filex_device) == FALSE)
      {
        if (IStream_enable((IStream_t *) p_obj->filex_device) != SYS_NO_ERROR_CODE)
        {
          /* TODO: send msg to util task or error led;*/
          return 1;
        }
      }

      IStream_start((IStream_t *) p_obj->filex_device, 0);

      DatalogAppTask_UpdateStreamingStatus(p_obj, (IStream_t *) p_obj->filex_device, TRUE, interface);
      p_obj->datalog_model->log_controller_model.status = TRUE;

      /* generate the system event.*/
      SysEvent evt =
      {
        .nRawEvent = SYS_PM_MAKE_EVENT(SYS_PM_EVT_SRC_DATALOG, 0)
      };
      SysPostPowerModeEvent(evt);
    }
    else if (interface == LOG_CTRL_MODE_USB) /*Start log on USB*/
    {

      IStream_disable((IStream_t *) p_obj->filex_device);
      if (IStream_is_enabled((IStream_t *) p_obj->usbx_device) == FALSE)
      {
        if (IStream_enable((IStream_t *) p_obj->usbx_device) != SYS_NO_ERROR_CODE)
        {
          /* TODO: send msg to util task or error led;*/
          return 1;
        }
      }

      IStream_start((IStream_t *) p_obj->usbx_device, 0);
      DatalogAppTask_UpdateStreamingStatus(p_obj, (IStream_t *) p_obj->usbx_device, TRUE, interface);

      p_obj->datalog_model->log_controller_model.status = TRUE;

      /* generate the system event.*/
      SysEvent evt =
      {
        .nRawEvent = SYS_PM_MAKE_EVENT(SYS_PM_EVT_SRC_DATALOG, 0)
      };
      SysPostPowerModeEvent(evt);
    }
    else
    {
      /*Start command from other interfaces: not implemented*/
    }
  }

  return 0;
}

uint8_t DatalogAppTask_stop_vtbl(ILog_Controller_t *_this)
{
  DatalogAppTask *p_obj = (DatalogAppTask *)((uint32_t) _this - offsetof(DatalogAppTask, pnplLogCtrl));
  bool status;
  log_controller_get_log_status(&status);
  int8_t interface = p_obj->datalog_model->log_controller_model.interface;

  if (status)
  {
    if (interface == LOG_CTRL_MODE_SD) /*stop command from SD*/
    {
      /* Update the status */
      p_obj->datalog_model->log_controller_model.status = FALSE;
    }
    else if (interface == LOG_CTRL_MODE_USB) /*stop command from USB*/
    {
      /* Update the status */
      p_obj->datalog_model->log_controller_model.status = FALSE;
    }
    SysEvent evt =
    {
      .nRawEvent = SYS_PM_MAKE_EVENT(SYS_PM_EVT_SRC_DATALOG, 0)
    };
    SysPostPowerModeEvent(evt);
  }

  return 0;
}

uint8_t DatalogAppTask_save_config_vtbl(ILog_Controller_t *_this)
{
  assert_param(_this != NULL);
  DatalogAppTask *p_obj = (DatalogAppTask *)((uint32_t) _this - offsetof(DatalogAppTask, pnplLogCtrl));
  //TODO Save current board configuration into the mounted SD Card
  ULONG msg = FILEX_DCTRL_CMD_SET_DEFAULT_STATUS;
  filex_dctrl_msg(p_obj->filex_device, &msg);
  return 0;
}

uint8_t DatalogAppTask_set_time_vtbl(ILog_Controller_t *_this, const char *datetime)
{
  assert_param(_this != NULL);

  char datetimeStr[3];

  //internal input format: yyyyMMdd_hh_mm_ss

  RTC_DateTypeDef sdate;
  RTC_TimeTypeDef stime;

  /** extract year string (only the last two digit). It will be necessary to add 2000*/
  datetimeStr[0] = datetime[2];
  datetimeStr[1] = datetime[3];
  datetimeStr[2] = '\0';
  sdate.Year = atoi(datetimeStr);

  /** extract month string */
  datetimeStr[0] = datetime[4];
  datetimeStr[1] = datetime[5];
  sdate.Month = atoi(datetimeStr);

  /** extract day string */
  datetimeStr[0] = datetime[6];
  datetimeStr[1] = datetime[7];
  sdate.Date = atoi(datetimeStr);

  /** Week day initialization (not used)*/
  sdate.WeekDay = RTC_WEEKDAY_MONDAY; //Not used

  /** extract hour string */
  datetimeStr[0] = datetime[9];
  datetimeStr[1] = datetime[10];
  stime.Hours = atoi(datetimeStr);

  /** extract minute string */
  datetimeStr[0] = datetime[12];
  datetimeStr[1] = datetime[13];
  stime.Minutes = atoi(datetimeStr);

  /** extract second string */
  datetimeStr[0] = datetime[15];
  datetimeStr[1] = datetime[16];
  stime.Seconds = atoi(datetimeStr);

  /** not used */
  //stime.TimeFormat = RTC_HOURFORMAT12_AM;
  stime.SecondFraction = 0;
  stime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  stime.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&hrtc, &stime, RTC_FORMAT_BIN) != HAL_OK)
  {
    while (1)
      ;
  }
  if (HAL_RTC_SetDate(&hrtc, &sdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    while (1)
      ;
  }

  return 0;
}

uint8_t DatalogAppTask_switch_bank_vtbl(ILog_Controller_t *_this)
{
  assert_param(_this != NULL);
  SwitchBank();
  HAL_NVIC_SystemReset();
  return 0;
}

uint8_t DatalogAppTask_set_dfu_mode(ILog_Controller_t *_this)
{
  /*  Disable interrupts for timers */
  HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

  __enable_irq();
  HAL_RCC_DeInit();
  HAL_DeInit();
  SysTick->CTRL = SysTick->LOAD = SysTick->VAL = 0;
  __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();

  /* Jump to user application */
  typedef  void (*pFunction)(void);
  pFunction JumpToApplication;
  uint32_t JumpAddress;
  JumpAddress = *(__IO uint32_t *)(BOOTLOADER_ADDRESS + 4);
  JumpToApplication = (pFunction) JumpAddress;

  /* Initialize user application's Stack Pointer */
  __set_MSP(*(__IO uint32_t *) BOOTLOADER_ADDRESS);
  JumpToApplication();
  return 0;
}

/* IMLCController_t virtual functions */
uint8_t DatalogAppTask_load_ism330dhcx_ucf_vtbl(IIsm330dhcx_Mlc_t *_this, const char *ucf_data, int32_t ucf_size)
{
  assert_param(_this != NULL);
  DatalogAppTask *p_obj = (DatalogAppTask *)((uint32_t) _this - offsetof(DatalogAppTask, pnplMLCCtrl));
  SQuery_t q1, q2, q3, q4;
  uint16_t id;
  SensorStatus_t sensor_status;

  /* Create and initialize a new instance of UCF Protocol service */
  SUcfProtocol_t ucf_protocol;
  UCFP_Init(&ucf_protocol, p_obj->mlc_sensor_ll);

  /* Load the compressed UCF using the specified ISensorLL interface */
  UCFP_LoadCompressedUcf(&ucf_protocol, ucf_data, ucf_size);

  /* Enable MLC */
  SQInit(&q1, SMGetSensorManager());
  id = SQNextByNameAndType(&q1, "ism330dhcx", COM_TYPE_MLC);
  SMSensorEnable((uint8_t)id);

  /* Get ISM330DHCX status from SM and update app_model */
  SQInit(&q2, SMGetSensorManager());
  id = SQNextByNameAndType(&q2, "ism330dhcx", COM_TYPE_ACC);
  sensor_status = SMSensorGetStatus(id);
  p_obj->datalog_model->s_models[id]->sensor_status.is_active = sensor_status.is_active;
  p_obj->datalog_model->s_models[id]->sensor_status.type.mems.odr = sensor_status.type.mems.odr;
  p_obj->datalog_model->s_models[id]->sensor_status.type.mems.fs = sensor_status.type.mems.fs;
  p_obj->datalog_model->s_models[id]->stream_params.spts = (int32_t)sensor_status.type.mems.odr;

  SQInit(&q3, SMGetSensorManager());
  id = SQNextByNameAndType(&q3, "ism330dhcx", COM_TYPE_GYRO);
  sensor_status = SMSensorGetStatus(id);
  p_obj->datalog_model->s_models[id]->sensor_status.is_active = sensor_status.is_active;
  p_obj->datalog_model->s_models[id]->sensor_status.type.mems.odr = sensor_status.type.mems.odr;
  p_obj->datalog_model->s_models[id]->sensor_status.type.mems.fs = sensor_status.type.mems.fs;
  p_obj->datalog_model->s_models[id]->stream_params.spts = (int32_t)sensor_status.type.mems.odr;

  SQInit(&q4, SMGetSensorManager());
  id = SQNextByNameAndType(&q4, "ism330dhcx", COM_TYPE_MLC);
  sensor_status = SMSensorGetStatus(id);
  p_obj->datalog_model->s_models[id]->sensor_status.is_active = sensor_status.is_active;

  return 0;
}

uint8_t DatalogAppTask_load_ucf(const char *p_ucf_data, uint32_t ucf_size, const char *p_output_data,
                                int32_t output_size)
{

  ism330dhcx_mlc_load_file(&sTaskObj.pnplMLCCtrl, p_ucf_data, ucf_size);

  return 0;
}

// Private function definition
// ***************************

static sys_error_code_t DatalogAppTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask *) _this;
  AMTExSetInactiveState((AManagedTaskEx *) _this, TRUE);
  if (TX_SUCCESS == tx_queue_receive(&p_obj->in_queue, &message, TX_WAIT_FOREVER))
  {
    AMTExSetInactiveState((AManagedTaskEx *) _this, FALSE);

    if (message == DT_USER_BUTTON)
    {
      res = SYS_NO_ERROR_CODE;

      log_controller_start_log(&p_obj->pnplLogCtrl, LOG_CTRL_MODE_SD);
    }
    else if (message == DT_FORCE_STEP)
    {
      __NOP();
    }
  }

  return res;
}

static sys_error_code_t DatalogAppTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask *) _this;

  AMTExSetInactiveState((AManagedTaskEx *) _this, TRUE);
  if (TX_SUCCESS == tx_queue_receive(&p_obj->in_queue, &message, TX_WAIT_FOREVER))
  {
    AMTExSetInactiveState((AManagedTaskEx *) _this, FALSE);

    if (message == DT_USER_BUTTON)
    {
      if (IStream_is_enabled((IStream_t *) p_obj->filex_device))
      {
        /* Logging through SD card, so generate the USR BUTTON system event to pass from Datalog to State1 */

        p_obj->mode = 0;

        log_controller_stop_log(&p_obj->pnplLogCtrl);
      }
    }
    else if (message == DT_FORCE_STEP)
    {
      __NOP();
    }
    else /* ??? is it correct to forward all messages to SDCard?*/
    {
      filex_dctrl_msg(p_obj->filex_device, &message);
    }
  }

  return res;
}

static VOID DatalogAppTaskAdvOBTimerCallbackFunction(ULONG timer)
{

  /* update BLE advertise option bytes */
  uint8_t adv_option_bytes[3];
  uint8_t battery_level = 0, status = 0;
  bool log_status = 0;
  bool SD_Detected;
  log_controller_get_sd_mounted(&SD_Detected);

  UtilTask_GetBatteryStatus(&battery_level, &status);
  log_controller_get_log_status(&log_status);

  if (((battery_level < 10) && (status == UTIL_BATTERY_STATUS_DISCHARGING)) && log_status)
  {
    /* Battery level is low: if SD card is logging, force stop acquisition and close files */
    if (!(sTaskObj.datalog_model->log_controller_model.status
          && (sTaskObj.datalog_model->log_controller_model.interface == 1)))
    {
      bool automode_state;
      automode_get_enabled(&automode_state);
      if (automode_state) /* If automode is running, force automode stopping */
      {
        automode_forced_stop();
        automode_set_enabled(false);
      }

      DatalogAppTask_msg((ULONG) DT_USER_BUTTON);
    }
  }
  else
  {
    adv_option_bytes[ADV_OB_BATTERY] = battery_level;

    if (!SD_IsDetected())
    {
      adv_option_bytes[ADV_OB_ALARM] = 5;
    }
    else if (log_status)
    {
      adv_option_bytes[ADV_OB_ALARM] = 3;
    }
    else
    {
      adv_option_bytes[ADV_OB_ALARM] = 0;
    }

    switch (status)
    {
      case UTIL_BATTERY_STATUS_DISCHARGING:
        if (battery_level <= 20)
        {
          adv_option_bytes[ADV_OB_ICON] = 4;
          adv_option_bytes[ADV_OB_ALARM] = 4;
        }
        else if ((battery_level > 20) && (battery_level <= 60))
        {
          adv_option_bytes[ADV_OB_ICON] = 2;
        }
        else if (battery_level > 60)
        {
          adv_option_bytes[ADV_OB_ICON] = 7;
        }
        break;

      case UTIL_BATTERY_STATUS_CHARGING:
        adv_option_bytes[ADV_OB_ICON] = 1;
        break;

      case UTIL_BATTERY_STATUS_NOT_CONNECTED:
        adv_option_bytes[ADV_OB_ICON] = 0;
        adv_option_bytes[ADV_OB_BATTERY] = 0;
        break;

      case UTIL_BATTERY_STATUS_FULL:
        adv_option_bytes[ADV_OB_ICON] = 7;
        break;

      case UTIL_BATTERY_STATUS_UNKNOWN:
        adv_option_bytes[ADV_OB_ICON] = 8;
        break;
    }

    IStream_post_data((IStream_t *) sTaskObj.ble_device, sTaskObj.ble_device->adv_id, adv_option_bytes, 3);

    streamMsg_t msg;
    msg.messageId = BLE_ISTREAM_MSG_UPDATE_ADV;
    msg.streamID = sTaskObj.ble_device->adv_id;
    ble_stream_msg(&msg);
  }


}

static sys_error_code_t DatalogAppTask_UpdateStreamingStatus(DatalogAppTask *p_obj, IStream_t *p_istream,
                                                             bool streaming_status, int8_t interface)
{
  assert_param(p_obj);
  assert_param(p_istream);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint8_t stream_id;
  uint16_t i;
  uint32_t dps;

#if (SENSOR_NUMBER != 0)
  for (i = 0; i < SENSOR_NUMBER; i++)
  {
    if (p_obj->datalog_model->s_models[i] != NULL)
    {
      if (p_obj->datalog_model->s_models[i]->sensor_status.is_active)
      {
        stream_id = p_obj->datalog_model->s_models[i]->stream_params.stream_id;
        if (streaming_status) /* Start */
        {
          if (interface == LOG_CTRL_MODE_SD)
          {
            dps = p_obj->datalog_model->s_models[i]->stream_params.sd_dps;
            IStream_alloc_resource(p_istream, stream_id, dps, p_obj->datalog_model->s_models[i]->comp_name);
          }
          else if (interface == LOG_CTRL_MODE_USB)
          {
            stream_id = p_obj->datalog_model->s_models[i]->stream_params.stream_id;
            dps = p_obj->datalog_model->s_models[i]->stream_params.usb_dps;

            /* Set USB endpoint for the current stream */
            usbx_dctrl_class_set_ep(p_obj->usbx_device, stream_id, p_obj->datalog_model->s_models[i]->stream_params.usb_ep);
            IStream_alloc_resource((IStream_t *) p_obj->usbx_device, stream_id, dps, p_obj->datalog_model->s_models[i]->comp_name);
          }
          else
          {
            return SYS_INVALID_PARAMETER_ERROR_CODE;
          }
        }
        else /* Stop */
        {
          IStream_dealloc(p_istream, stream_id);
          p_obj->sensorContext[i].n_samples_to_timestamp = 0;
          p_obj->sensorContext[i].old_time_stamp = -1.0f;
        }
      }
    }
  }
#endif
#if (ACTUATOR_NUMBER != 0)
  for (i = 0; i < ACTUATOR_NUMBER; i++)
  {
    if (p_obj->datalog_model->ac_models[i] != NULL)
    {
      if (p_obj->datalog_model->ac_models[i]->actuatorStatus.is_active)
      {
        stream_id = p_obj->datalog_model->ac_models[i]->stream_params.stream_id;
        if (streaming_status) /* Start */
        {
          uint32_t sd_dps;
          sd_dps = p_obj->datalog_model->ac_models[i]->stream_params.sd_dps;
          IStream_alloc_resource(p_istream, stream_id, sd_dps, p_obj->datalog_model->ac_models[i]->comp_name);
        }
        else /* Stop */
        {
          IStream_dealloc(p_istream, stream_id);
          p_obj->sensorContext[i].n_samples_to_timestamp = 0;
          p_obj->sensorContext[i].old_time_stamp = -1.0f;
        }
      }
    }
  }
#endif

  return res;
}


// Interrupt callback
// ***************************
void Util_USR_EXTI_Callback(uint16_t pin)
{
  /* anti debounch */
  static uint32_t t_start = 0;

  if (HAL_GetTick() - t_start > 1000)
  {
    if (pin == USER_BUTTON_Pin)
    {
      if (!(sTaskObj.datalog_model->log_controller_model.status
            && (sTaskObj.datalog_model->log_controller_model.interface == 1)))
      {
        bool automode_state;
        automode_get_enabled(&automode_state);
        if (automode_state) /* if button is pressed during automode, force automode stopping */
        {
          automode_forced_stop();
          automode_set_enabled(false);
        }

        DatalogAppTask_msg((ULONG) DT_USER_BUTTON);
      }
    }
  }
  t_start = HAL_GetTick();
}

