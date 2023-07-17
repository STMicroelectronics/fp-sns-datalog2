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
#include "services/sysinit.h"
#include "services/SysTimestamp.h"

#include "usbx_dctrl_class.h"
#include "PnPLCompManager.h"
#include "App_model.h"
#include "rtc.h"

#ifndef DT_TASK_CFG_STACK_DEPTH
#define DT_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*2)
#endif

#ifndef DT_TASK_CFG_PRIORITY
#define DT_TASK_CFG_PRIORITY                 (TX_MAX_PRIORITIES-1)
#endif

#ifndef DT_TASK_CFG_IN_QUEUE_LENGTH
#define DT_TASK_CFG_IN_QUEUE_LENGTH          20
#endif

#define DT_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(ULONG)

#define COMM_ID_SDCARD                       0U
#define COMM_ID_USB                          1U
#define COMM_ID_BLE                          2U

#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_DT, level, message)

#if defined(DEBUG) || defined (SYS_DEBUG)
#define sTaskObj                                  sDatalogAppTaskObj
#endif

/**
 *  DatalogAppTask internal structure.
 */
struct _DatalogAppTask
{
  /**
   * Base class object.
   */
  AManagedTaskEx super;

  /**
   * Queue message
   */
  TX_QUEUE in_queue;

  ULONG message;

  /**
   * Algorithm Event Listener
   */
  IDataEventListener_t algorithmListener;

  /**
   * Sensor Event Listener
   */
  IDataEventListener_t sensorListener;

  void *owner;

  /**
   *  USBX ctrl class
   */
  usbx_dctrl_class_t *usbx_device;

  /**
   * Command Parser
   */
  ICommandParse_t parser;

  /**
   *  Out PnPL Command handler
   */
  PnPLCommand_t outPnPLCommand;

  /**
   * PnPL Log controller handler
   */
  ILog_Controller_t pnplLogCtrl;

  AppModel_t *datalog_model;

  SensorContext_t sensorContext[SM_MAX_SENSORS];
};

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
  ICommandParse_vtbl parserVTBL;

  /**
   * ILogController virtual table.
   */
  ILog_Controller_vtbl logCtrlVTBL;

  /**
   * IProcessEventListener virtual table.
   */
  IDataEventListener_vtbl algorithmListener_vtbl;

  /**
   * IProcessEventListener virtual table.
   */
  IDataEventListener_vtbl sensorListener_vtbl;

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
 * Execute one step of the task control loop while the system is in STARTING mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t DatalogAppTaskExecuteStepStarting(AManagedTask *_this);

static sys_error_code_t sensorEvtListener_OnStatusChange_vtbl(IListener *_this);
static void sensorEvtListener_ProcessSetOwner_vtbl(IEventListener *_this, void *pxOwner);
static void* sensorEvtListener_ProcessGetOwner_vtbl(IEventListener *_this);
sys_error_code_t sensorEvtListener_ProcessOnNewDataReady_vtbl(IEventListener *_this, const DataEvent_t *pxEvt);

#if defined (__GNUC__)
/* Inline function defined inline in the header file DatalogAppTask.h must be declared here as extern function. */
#endif

/* Objects instance */
/********************/

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
        DatalogAppTask_vtblOnEnterPowerMode },
    {
        DatalogAppTask_vtblICommandParse_t_parse_cmd,
        DatalogAppTask_vtblICommandParse_t_serialize_response },

    {
        NULL,
        DatalogAppTask_start_vtbl,
        DatalogAppTask_stop_vtbl,
        DatalogAppTask_set_time_vtbl,
        NULL },

    {/* Algorithm Event Listener */
        algorithmEvtListener_OnStatusChange_vtbl,
        algorithmEvtListener_ProcessSetOwner_vtbl,
        algorithmEvtListener_ProcessGetOwner_vtbl,
        algorithmEvtListener_ProcessOnNewDataReady_vtbl },

    {/* Sensor Event Listener */
        sensorEvtListener_OnStatusChange_vtbl,
        sensorEvtListener_ProcessSetOwner_vtbl,
        sensorEvtListener_ProcessGetOwner_vtbl,
        sensorEvtListener_ProcessOnNewDataReady_vtbl },

    /* class (PM_STATE, ExecuteStepFunc) map */
    {
        DatalogAppTaskExecuteStepState1,
        NULL,
        DatalogAppTaskExecuteStepDatalog,
        DatalogAppTaskExecuteStepStarting, } };

/* Public API definition */
/* ***********************/

AManagedTaskEx* DatalogAppTaskAlloc()
{
  /* In this application there is only one Keyboard task,
   * so this allocator implement the singleton design pattern.
   */

  /* Initialize the super class */
  AMTInitEx(&sTaskObj.super);

  sTaskObj.super.vptr = &sTheClass.vtbl;
  sTaskObj.parser.vptr = &sTheClass.parserVTBL;
  sTaskObj.algorithmListener.vptr = &sTheClass.algorithmListener_vtbl;
  sTaskObj.sensorListener.vptr = &sTheClass.sensorListener_vtbl;
  sTaskObj.pnplLogCtrl.vptr = &sTheClass.logCtrlVTBL;

  memset(&sTaskObj.outPnPLCommand, 0, sizeof(PnPLCommand_t));

  return (AManagedTaskEx*) &sTaskObj;
}

IDataEventListener_t* DatalogAppTask_GetEventListenerIF(DatalogAppTask *_this)
{
  assert_param(_this);

  return &_this->algorithmListener;
}

IDataEventListener_t* DatalogAppTask_GetSensorEventListenerIF(DatalogAppTask *_this)
{
  assert_param(_this);
  return &_this->sensorListener;
}

ICommandParse_t* DatalogAppTask_GetICommandParseIF(DatalogAppTask *_this)
{
  assert_param(_this != NULL);
  DatalogAppTask *p_obj = (DatalogAppTask*) _this;

  return &p_obj->parser;

}

ILog_Controller_t* DatalogAppTask_GetILogControllerIF(DatalogAppTask *_this)
{
  assert_param(_this != NULL);
  DatalogAppTask *p_obj = (DatalogAppTask*) _this;

  return &p_obj->pnplLogCtrl;
}

/* AManagedTask virtual functions definition */
/* *******************************************/

sys_error_code_t DatalogAppTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t DatalogAppTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
VOID **pvStackStart,
                                                 ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                                 ULONG *pParams)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask*) _this;

  /* Create task specific sw resources. */

  uint16_t nItemSize = DT_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *pvQueueItemsBuff = SysAlloc(DT_TASK_CFG_IN_QUEUE_LENGTH * nItemSize);
  if(pvQueueItemsBuff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if(TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "DatalogApp_Q", nItemSize / 4, pvQueueItemsBuff, DT_TASK_CFG_IN_QUEUE_LENGTH * nItemSize))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Set Alloc and Free function for JSON class method */
  json_set_allocation_functions(SysAlloc, SysFree);

  uint8_t ii;
  for(ii = 0; ii < SM_MAX_SENSORS; ii++)
  {
    p_obj->sensorContext[ii].n_samples_to_timestamp = 0;
    p_obj->sensorContext[ii].old_time_stamp = -1.0f;
  }

  p_obj->datalog_model = getAppModel();

  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "DatalogApp";
  *pvStackStart = NULL; /* allocate the task stack in the system memory pool. */
  *pStackDepth = DT_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = DT_TASK_CFG_PRIORITY;
  *pPreemptThreshold = DT_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  IEventListenerSetOwner((IEventListener*) &p_obj->algorithmListener, (void*) p_obj);

  p_obj->usbx_device = (usbx_dctrl_class_t*) usbx_dctrl_class_alloc((void*) &MX_PCDInitParams);
  IStream_init((IStream_t*) p_obj->usbx_device, COMM_ID_USB, 0);
  IStream_set_parse_IF((IStream_t*) p_obj->usbx_device, DatalogAppTask_GetICommandParseIF(p_obj));

  return res;
}

sys_error_code_t DatalogAppTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode, const EPowerMode NewPowerMode)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask*) _this;

  if(ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE && NewPowerMode == E_POWER_MODE_STATE1)
  {

    if(p_obj->datalog_model->log_controller_model.interface == LOG_CTRL_MODE_USB) /*stop command from USB*/
    {
      IStream_stop((IStream_t*) p_obj->usbx_device);

      p_obj->datalog_model->log_controller_model.status = FALSE;

      uint8_t stream_id;
      for(int ii = 0; ii < SENSOR_NUMBER; ii++)
      {
        if(p_obj->datalog_model->s_models[ii]->sensorStatus.IsActive)
        {
          stream_id = p_obj->datalog_model->s_models[ii]->streamParams.stream_id;
          IStream_dealloc((IStream_t*) p_obj->usbx_device, stream_id);
          p_obj->sensorContext[ii].n_samples_to_timestamp = 0;
          p_obj->sensorContext[ii].old_time_stamp = -1.0f;
        }
      }

      for(int ii = 0; ii < ALGORITHM_NUMBER; ii++)
      {
        if( NULL != p_obj->datalog_model->a_models[ii] && p_obj->datalog_model->a_models[ii]->enable)
        {
          stream_id = p_obj->datalog_model->a_models[ii]->streamParams.stream_id;
          IStream_dealloc((IStream_t*) p_obj->usbx_device, stream_id);
          p_obj->sensorContext[ii].n_samples_to_timestamp = 0;
          p_obj->sensorContext[ii].old_time_stamp = -1.0f;
        }
      }

      p_obj->datalog_model->log_controller_model.interface = -1;

    }
    SysTsStop(SysGetTimestampSrv());
  }

  return res;
}

sys_error_code_t DatalogAppTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t DatalogAppTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask*) _this;
  IStream_enable((IStream_t*) p_obj->usbx_device);

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("DatalogApp: start.\r\n"));

  /* At this point all system has been initialized.
   * Execute task specific delayed one time initialization.
   */
  return res;
}

sys_error_code_t DatalogAppTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask*) _this;

  p_obj->message = DT_FORCE_STEP;

  if((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE) || (ActivePowerMode == E_POWER_MODE_STARTING))
  {
    if(AMTExIsTaskInactive(_this))
    {
      if(tx_queue_front_send(&p_obj->in_queue, &p_obj->message, AMT_MS_TO_TICKS(100)) != TX_SUCCESS)
      {
        res = SYS_APP_TASK_REPORT_LOST_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_APP_TASK_REPORT_LOST_ERROR_CODE);
      }
    }
  }
  else
  {
    UINT state;
    if(TX_SUCCESS == tx_thread_info_get(&_this->m_xTaskHandle, TX_NULL, &state, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL, TX_NULL))
    {
      if(state == TX_SUSPENDED)
      {
        tx_thread_resume(&_this->m_xTaskHandle);
      }
    }
  }

  return res;
}

sys_error_code_t DatalogAppTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode, const EPowerMode NewPowerMode)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

/**
 *  IIListener virtual functions
 */
sys_error_code_t algorithmEvtListener_OnStatusChange_vtbl(IListener *_this)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

/**
 * IProcessEventListener virtual functions
 */
void* algorithmEvtListener_ProcessGetOwner_vtbl(IEventListener *_this)
{
  DatalogAppTask *p_obj = (DatalogAppTask*) ((uint32_t) _this - offsetof(DatalogAppTask, algorithmListener));

  return p_obj->owner;
}

void algorithmEvtListener_ProcessSetOwner_vtbl(IEventListener *_this, void *pxOwner)
{
  DatalogAppTask *p_obj = (DatalogAppTask*) ((uint32_t) _this - offsetof(DatalogAppTask, algorithmListener));

  p_obj->owner = pxOwner;
}

sys_error_code_t algorithmEvtListener_ProcessOnNewDataReady_vtbl(IEventListener *_this, const DataEvent_t *pxEvt)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask*) IEventListenerGetOwner(_this);

  uint8_t nPMState = (uint8_t) AMTGetTaskPowerMode((AManagedTask*) p_obj);

  /* Get Algorithm id from EMData tag */
  uint16_t aId = pxEvt->tag;

  /* Get pointer to Payload */
  uint8_t *p_payload = EMD_Data(pxEvt->p_data);

  /* Get Payload size in byte */
  uint32_t payload_size = EMD_GetPayloadSize(pxEvt->p_data);

  if(nPMState == E_POWER_MODE_SENSORS_ACTIVE)
  {
    /* Get stream id from data model */
    uint8_t stream_id = p_obj->datalog_model->a_models[aId]->streamParams.stream_id;

    /* Stream data over USB peripheral */
    if(IStream_is_enabled((IStream_t*) p_obj->usbx_device))
    {
      res = IStream_post_data((IStream_t*) p_obj->usbx_device, stream_id, p_payload, payload_size);
    }

    if(res != 0)
    {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
      return res;
    }

  }
  return res;
}

static sys_error_code_t sensorEvtListener_OnStatusChange_vtbl(IListener *_this)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

static void sensorEvtListener_ProcessSetOwner_vtbl(IEventListener *_this, void *pxOwner)
{
  DatalogAppTask *p_obj = (DatalogAppTask*) ((uint32_t) _this - offsetof(DatalogAppTask, sensorListener));

  p_obj->owner = pxOwner;
}

void* sensorEvtListener_ProcessGetOwner_vtbl(IEventListener *_this)
{
  DatalogAppTask *p_obj = (DatalogAppTask*) ((uint32_t) _this - offsetof(DatalogAppTask, sensorListener));

  return p_obj->owner;
}

sys_error_code_t sensorEvtListener_ProcessOnNewDataReady_vtbl(IEventListener *_this, const DataEvent_t *pxEvt)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask*) IEventListenerGetOwner(_this);

  uint8_t nPMState = (uint8_t) AMTGetTaskPowerMode((AManagedTask*) p_obj);
  uint16_t sId = pxEvt->tag;
#if (HSD_USE_DUMMY_DATA != 1)
  uint32_t nBytesPerSample = SMGetnBytesPerSample(sId);
  uint8_t *data_buf = EMD_Data(pxEvt->p_data);
  uint32_t samplesToSend = EMD_GetPayloadSize(pxEvt->p_data) / nBytesPerSample;
#endif

  if(nPMState == E_POWER_MODE_SENSORS_ACTIVE)
  {
    uint8_t stream_id = p_obj->datalog_model->s_models[sId]->streamParams.stream_id;
    if(res != 0)
    {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
      return res;
    }
#if (HSD_USE_DUMMY_DATA == 1)
    if (IStream_is_enabled((IStream_t *)p_obj->usbx_device))
    {
      res = IStream_post_data((IStream_t *)p_obj->usbx_device, stream_id, EMD_Data(pxEvt->p_data), EMD_GetPayloadSize(pxEvt->p_data));
    }
    if (res != 0)
    {
      while (1);
    }
#else

    if(p_obj->sensorContext[sId].old_time_stamp == -1.0f)
    {
      p_obj->datalog_model->s_models[sId]->streamParams.ioffset = pxEvt->timestamp; /*TODO: can I use PnPL Setter? no*/
      p_obj->sensorContext[sId].old_time_stamp = pxEvt->timestamp;
      p_obj->sensorContext[sId].n_samples_to_timestamp = p_obj->datalog_model->s_models[sId]->streamParams.spts;
    }
    else
    {
      while(samplesToSend > 0)
      {
        /* n_samples_to_timestamp = 0 if user setup spts = 0 (no timestamp needed) */
        if(p_obj->sensorContext[sId].n_samples_to_timestamp == 0 || samplesToSend < p_obj->sensorContext[sId].n_samples_to_timestamp)
        {
          if(IStream_is_enabled((IStream_t*) p_obj->usbx_device))
          {
            res = IStream_post_data((IStream_t*) p_obj->usbx_device, stream_id, data_buf, samplesToSend * nBytesPerSample);
          }

          if(res != 0)
          {
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
            return res;
          }
          if(p_obj->sensorContext[sId].n_samples_to_timestamp != 0)
          {
            p_obj->sensorContext[sId].n_samples_to_timestamp -= samplesToSend;
          }
          samplesToSend = 0;
        }
        else
        {
          if(IStream_is_enabled((IStream_t*) p_obj->usbx_device))
          {
            res = IStream_post_data((IStream_t*) p_obj->usbx_device, stream_id, data_buf, p_obj->sensorContext[sId].n_samples_to_timestamp * nBytesPerSample);
          }
          if(res != 0)
          {
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
            return res;
          }

          data_buf += p_obj->sensorContext[sId].n_samples_to_timestamp * nBytesPerSample;
          samplesToSend -= p_obj->sensorContext[sId].n_samples_to_timestamp;

          float measuredODR, ODR;
          double newTS;
          ISourceObservable *p_sensor_observer = SMGetSensorObserver(sId);
          ISourceGetODR(p_sensor_observer, &measuredODR, &ODR);
          if(measuredODR != 0.0f)
          {
            newTS = pxEvt->timestamp - ((1.0 / (double) measuredODR) * samplesToSend);
          }
          else
          {
            newTS = pxEvt->timestamp;
          }

          if(IStream_is_enabled((IStream_t*) p_obj->usbx_device))
          {
            res = IStream_post_data((IStream_t*) p_obj->usbx_device, stream_id, (uint8_t*) &newTS, 8);
          }
          if(res != 0)
          {
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
            return res;
          }
          p_obj->sensorContext[sId].n_samples_to_timestamp = p_obj->datalog_model->s_models[sId]->streamParams.spts;
        }
      }
    }
#endif
  }
  return res;
}

/**
 * ICommandParse_t virtual functions
 */
sys_error_code_t DatalogAppTask_vtblICommandParse_t_parse_cmd(ICommandParse_t *_this, char *commandString, uint8_t mode)
{
  assert_param(_this);
  UNUSED(mode);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  DatalogAppTask *p_obj = (DatalogAppTask*) ((uint32_t) _this - offsetof(DatalogAppTask, parser));

  int pnp_res = PnPLParseCommand(commandString, &p_obj->outPnPLCommand);
  if(pnp_res == SYS_NO_ERROR_CODE)
  {
    if(IStream_is_enabled((IStream_t*) p_obj->usbx_device))
    {
      if(p_obj->outPnPLCommand.comm_type != PNPL_CMD_GET && p_obj->outPnPLCommand.comm_type != PNPL_CMD_SYSTEM_INFO)
      {
        IStream_set_mode((IStream_t*) p_obj->usbx_device, RECEIVE);
      }
      else
      {
        IStream_set_mode((IStream_t*) p_obj->usbx_device, TRANSMIT);
      }
    }
  }
  else
  {
    if(p_obj->outPnPLCommand.comm_type == PNPL_CMD_ERROR)
    {
      IStream_set_mode((IStream_t*) p_obj->usbx_device, TRANSMIT);
    }
  }

  return res;
}

sys_error_code_t DatalogAppTask_vtblICommandParse_t_serialize_response(ICommandParse_t *_this, char **response_name, char **buff, uint32_t *size,
                                                                       uint8_t pretty)
{
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask*) ((uint32_t) _this - offsetof(DatalogAppTask, parser));

  if(p_obj->outPnPLCommand.comm_type == PNPL_CMD_GET || p_obj->outPnPLCommand.comm_type == PNPL_CMD_SYSTEM_INFO)
  {
    PnPLSerializeResponse(&p_obj->outPnPLCommand, buff, size, 0);
  }
  *response_name = p_obj->outPnPLCommand.comp_name;

  return xRes;
}

/**
 * ILogController_t virtual functions
 */
uint8_t DatalogAppTask_start_vtbl(ILog_Controller_t *_this, uint32_t interface)
{
  DatalogAppTask *p_obj = (DatalogAppTask*) ((uint32_t) _this - offsetof(DatalogAppTask, pnplLogCtrl));
  SysTsStart(SysGetTimestampSrv(), TRUE);

  /*Start command from USB*/
  uint8_t stream_id;
  uint16_t usb_dps;
  int8_t usb_ep;

  if(IStream_is_enabled((IStream_t*) p_obj->usbx_device) == FALSE)
  {
    IStream_enable((IStream_t*) p_obj->usbx_device);
  }

  for(int ii = 0; ii < SENSOR_NUMBER; ii++)
  {
    if(p_obj->datalog_model->s_models[ii]->sensorStatus.IsActive)
    {
      stream_id = p_obj->datalog_model->s_models[ii]->streamParams.stream_id;
      usb_ep = p_obj->datalog_model->s_models[ii]->streamParams.usb_ep;
      usb_dps = p_obj->datalog_model->s_models[ii]->streamParams.usb_dps;
      /** use to set ep**/
      usbx_dctrl_class_set_ep(p_obj->usbx_device, stream_id, usb_ep);
      IStream_alloc_resource((IStream_t*) p_obj->usbx_device, stream_id, usb_dps, p_obj->datalog_model->s_models[ii]->comp_name);
    }
  }

  for(int ii = 0; ii < ALGORITHM_NUMBER; ii++)
  {
    if( NULL != p_obj->datalog_model->a_models[ii] && p_obj->datalog_model->a_models[ii]->enable)
    {
      stream_id = p_obj->datalog_model->a_models[ii]->streamParams.stream_id;
      usb_ep = p_obj->datalog_model->a_models[ii]->streamParams.usb_ep;
      usb_dps = p_obj->datalog_model->a_models[ii]->streamParams.usb_dps;
      /** use to set ep**/
      usbx_dctrl_class_set_ep(p_obj->usbx_device, stream_id, usb_ep);
      IStream_alloc_resource((IStream_t*) p_obj->usbx_device, stream_id, usb_dps, p_obj->datalog_model->s_models[ii]->comp_name);
    }
  }
  IStream_start((IStream_t*) p_obj->usbx_device, 0);

  /* generate the system event.*/
  SysEvent evt =
  {
      .nRawEvent = SYS_PM_MAKE_EVENT(SYS_PM_EVT_SRC_DATALOG, 0) };

  SysPostPowerModeEvent(evt);

  return 0;
}

uint8_t DatalogAppTask_stop_vtbl(ILog_Controller_t *_this)
{

  bool status;
  log_controller_get_log_status(&status);

  if(status)
  {
    SysEvent evt =
    {
        .nRawEvent = SYS_PM_MAKE_EVENT(SYS_PM_EVT_SRC_DATALOG, 0) };
    SysPostPowerModeEvent(evt);
  }

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

  if(HAL_RTC_SetTime(&hrtc, &stime, RTC_FORMAT_BIN) != HAL_OK)
  {
    while(1)
      ;
  }
  if(HAL_RTC_SetDate(&hrtc, &sdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    while(1)
      ;
  }

  return 0;
}

/* Private function definition
 *****************************/

static sys_error_code_t DatalogAppTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask*) _this;
  AMTExSetInactiveState((AManagedTaskEx*) _this, TRUE);
  if(TX_SUCCESS == tx_queue_receive(&p_obj->in_queue, &p_obj->message, TX_WAIT_FOREVER))
  {
    AMTExSetInactiveState((AManagedTaskEx*) _this, FALSE);
  }
  else if(p_obj->message == DT_FORCE_STEP)
  {
    __NOP();
  }

  return res;
}

static sys_error_code_t DatalogAppTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask*) _this;

  AMTExSetInactiveState((AManagedTaskEx*) _this, TRUE);
  if(TX_SUCCESS == tx_queue_receive(&p_obj->in_queue, &p_obj->message, TX_WAIT_FOREVER))
  {
    AMTExSetInactiveState((AManagedTaskEx*) _this, FALSE);
  }
  else if(p_obj->message == DT_FORCE_STEP)
  {
    __NOP();
  }

  return res;
}

static sys_error_code_t DatalogAppTaskExecuteStepStarting(AManagedTask *_this)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask*) _this;

  AMTExSetInactiveState((AManagedTaskEx*) _this, TRUE);
  if(TX_SUCCESS == tx_queue_receive(&p_obj->in_queue, &p_obj->message, TX_WAIT_FOREVER))
  {
    AMTExSetInactiveState((AManagedTaskEx*) _this, FALSE);
  }
  else if(p_obj->message == DT_FORCE_STEP)
  {
    __NOP();
  }

  return res;
}

