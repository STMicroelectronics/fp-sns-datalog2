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

#include "usbx_dctrl_class.h"
#include "PnPLCompManager.h"


//#include "STWIN.box_debug_pins.h"


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
        DatalogAppTask_vtblOnEnterPowerMode },
    {
        DatalogAppTask_vtblICommandParse_t_parse_cmd,
        DatalogAppTask_vtblICommandParse_t_serialize_response,
        DatalogAppTask_vtblICommandParse_t_send_ctrl_msg },
    {
        DatalogAppTask_OnStatusChange_vtbl,
        DatalogAppTask_SetOwner_vtbl,
        DatalogAppTask_GetOwner_vtbl,
        DatalogAppTask_OnNewDataReady_vtbl },
    {
        DatalogAppTask_save_config_vtbl,
        DatalogAppTask_start_vtbl,
        DatalogAppTask_stop_vtbl,
        NULL },
    {
        DatalogAppTask_load_ism330dhcx_ucf_vtbl },
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
  BSP_DEBUG_PIN_Off( CON34_PIN_30 );

  BSP_DEBUG_PIN_Init(CON34_PIN_5);
  BSP_DEBUG_PIN_Init(CON34_PIN_12);
  BSP_DEBUG_PIN_Init(CON34_PIN_14);
  BSP_DEBUG_PIN_Init(CON34_PIN_16);
  BSP_DEBUG_PIN_Init(CON34_PIN_22);
  BSP_DEBUG_PIN_Init(CON34_PIN_24);
  BSP_DEBUG_PIN_Init(CON34_PIN_26);
  BSP_DEBUG_PIN_Init(CON34_PIN_28);
  BSP_DEBUG_PIN_Init(CON34_PIN_30);
  BSP_DEBUG_PIN_Init( CON34_PIN_30 );
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

  return res;
}


sys_error_code_t DatalogAppTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode, const EPowerMode NewPowerMode)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask *) _this;

  if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE && NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (p_obj->datalog_model->log_controller_model.interface == LOG_CTRL_MODE_USB) /*stop command from USB*/
    {
      IStream_stop((IStream_t*) p_obj->usbx_device);

      p_obj->datalog_model->log_controller_model.status = FALSE;

      uint8_t stream_id;
      for (int i = 0; i < SENSOR_NUMBER; i++)
      {
        if(p_obj->datalog_model->s_models[i] != NULL)
        {
          if(p_obj->datalog_model->s_models[i]->sensorStatus.IsActive)
          {
            stream_id = p_obj->datalog_model->s_models[i]->streamParams.stream_id;
            IStream_dealloc((IStream_t*) p_obj->usbx_device, stream_id);
            p_obj->sensorContext[i].n_samples_to_timestamp = 0;
            p_obj->sensorContext[i].old_time_stamp = -1.0f;
          }
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
  //  DatalogAppTask *p_obj = (DatalogAppTask*)_this;

  return res;
}

sys_error_code_t DatalogAppTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DatalogAppTask *p_obj = (DatalogAppTask *) _this;

#ifdef ENABLE_THREADX_DBG_PIN
  p_obj->super.m_xTaskHandle.pxTaskTag = DT_TASK_CFG_TAG;
#endif

  IStream_enable((IStream_t *) p_obj->usbx_device);

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
#if (HSD_USE_DUMMY_DATA != 1)
  uint32_t nBytesPerSample = SMGetnBytesPerSample(sId);
  uint8_t *data_buf = EMD_Data(p_evt->p_data);
  uint32_t samplesToSend = EMD_GetPayloadSize(p_evt->p_data)/nBytesPerSample;
#endif

  if (nPMState == E_POWER_MODE_SENSORS_ACTIVE)
  {
    uint8_t stream_id = p_obj->datalog_model->s_models[sId]->streamParams.stream_id;
    if (res != 0)
    {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
      return res;
    }
#if (HSD_USE_DUMMY_DATA == 1)
    if (IStream_is_enabled((IStream_t *)p_obj->usbx_device))
    {
      res = IStream_post_data((IStream_t *)p_obj->usbx_device, stream_id, EMD_Data(p_evt->p_data), EMD_GetPayloadSize(p_evt->p_data));
    }
    if (res != 0)
    {
      while (1);
    }
#else

    if (p_obj->sensorContext[sId].old_time_stamp == -1.0f)
    {
      p_obj->datalog_model->s_models[sId]->streamParams.ioffset = p_evt->timestamp; /*TODO: can I use PnPL Setter? no*/
      p_obj->sensorContext[sId].old_time_stamp = p_evt->timestamp;
      p_obj->sensorContext[sId].n_samples_to_timestamp = p_obj->datalog_model->s_models[sId]->streamParams.spts;
    }
    else
    {
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
          if (res != 0)
          {
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
            return res;
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
          if (res != 0)
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
          if (res != 0)
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

// ICommandParse_t virtual functions
sys_error_code_t DatalogAppTask_vtblICommandParse_t_parse_cmd(ICommandParse_t *_this, char *commandString, uint8_t comm_interface_id)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  DatalogAppTask *p_obj = (DatalogAppTask *)((uint32_t) _this - offsetof(DatalogAppTask, parser));

  int pnp_res = PnPLParseCommand(commandString, &p_obj->outPnPLCommand);
  if (pnp_res == SYS_NO_ERROR_CODE)
  {
    if (IStream_is_enabled((IStream_t *) p_obj->usbx_device) && (comm_interface_id == COMM_ID_USB))
    {
      if (p_obj->outPnPLCommand.comm_type != PNPL_CMD_GET && p_obj->outPnPLCommand.comm_type != PNPL_CMD_SYSTEM_INFO)
      {
        IStream_set_mode((IStream_t *) p_obj->usbx_device, RECEIVE);
      }
      else
      {
        IStream_set_mode((IStream_t *) p_obj->usbx_device, TRANSMIT);
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

  if (p_obj->outPnPLCommand.comm_type == PNPL_CMD_GET || p_obj->outPnPLCommand.comm_type == PNPL_CMD_SYSTEM_INFO
      || p_obj->outPnPLCommand.comm_type == PNPL_CMD_ERROR)
  {
    PnPLSerializeResponse(&p_obj->outPnPLCommand, buff, size, pretty);
  }
  *response_name = p_obj->outPnPLCommand.comp_name;

  return res;
}


sys_error_code_t DatalogAppTask_vtblICommandParse_t_send_ctrl_msg(ICommandParse_t *_this,  uint32_t *msg, uint32_t length)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  /*DatalogAppTask *p_obj = (DatalogAppTask *)((uint32_t) _this - offsetof(DatalogAppTask, parser));*/

  switch(*msg)
  {
    default:
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      break;
  }

  return res;
}

// ILogController_t virtual functions
uint8_t DatalogAppTask_start_vtbl(ILog_Controller_t *_this, uint32_t interface)
{
  DatalogAppTask *p_obj = (DatalogAppTask *)((uint32_t) _this - offsetof(DatalogAppTask, pnplLogCtrl));
  SysTsStart(SysGetTimestampSrv(), TRUE);
  bool status;

  log_controller_get_log_status(&status);

  if(!status)
  {

    p_obj->datalog_model->acquisition_info_model.interface = interface;

    if (interface == LOG_CTRL_MODE_USB) /*Start log on USB*/
    {
      uint8_t stream_id;
      uint16_t usb_dps;
      int8_t usb_ep;

      if (IStream_is_enabled((IStream_t *) p_obj->usbx_device) == FALSE)
      {
        IStream_enable((IStream_t *) p_obj->usbx_device);
      }

      for (int i = 0; i < SENSOR_NUMBER; i++)
      {
        if(p_obj->datalog_model->s_models[i] != NULL)
        {
          if(p_obj->datalog_model->s_models[i]->sensorStatus.IsActive)
          {
            stream_id = p_obj->datalog_model->s_models[i]->streamParams.stream_id;
            usb_ep = p_obj->datalog_model->s_models[i]->streamParams.usb_ep;
            usb_dps = p_obj->datalog_model->s_models[i]->streamParams.usb_dps;
            /** use to set ep**/
            usbx_dctrl_class_set_ep(p_obj->usbx_device, stream_id, usb_ep);
            IStream_alloc_resource((IStream_t*) p_obj->usbx_device, stream_id, usb_dps, p_obj->datalog_model->s_models[i]->comp_name);
          }
        }
      }

      IStream_start((IStream_t*) p_obj->usbx_device, 0);

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

  bool status;
  log_controller_get_log_status(&status);

  if(status)
  {
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
  /*DatalogAppTask *p_obj = (DatalogAppTask *)((uint32_t) _this - offsetof(DatalogAppTask, pnplLogCtrl));*/
  //TODO Save current board configuration into the mounted SD Card
  // NO SDCARD
  return 0;
}

// IMLCController_t virtual functions
uint8_t DatalogAppTask_load_ism330dhcx_ucf_vtbl(IIsm330dhcx_Mlc_t *_this, const char *ucf_data, uint32_t ucf_size)
{
  assert_param(_this != NULL);
  DatalogAppTask *p_obj = (DatalogAppTask *)((uint32_t) _this - offsetof(DatalogAppTask, pnplMLCCtrl));
  SQuery_t q1, q2, q3, q4;
  uint16_t id;
  SensorStatus_t sensorStatus;

  /* Create and initialize a new instance of UCF Protocol service */
  //TODO: STF ?? what we do with this ?
//  SUcfProtocol_t ucf_protocol;
//  UCFP_Init(&ucf_protocol, p_obj->mlc_sensor_ll);

  /* Load the compressed UCF using the specified ISensorLL interface */
//  UCFP_LoadCompressedUcf(&ucf_protocol, ucf_data, ucf_size);

  /* Enable MLC */
  SQInit(&q1, SMGetSensorManager());
  id = SQNextByNameAndType(&q1, "ism330dhcx", COM_TYPE_MLC);
  SMSensorEnable((uint8_t)id);

  /* Get ISM330DHCX status from SM and update app_model */
  SQInit(&q2, SMGetSensorManager());
  id = SQNextByNameAndType(&q2, "ism330dhcx", COM_TYPE_ACC);
  sensorStatus = SMSensorGetStatus(id);
  p_obj->datalog_model->s_models[id]->sensorStatus.IsActive = sensorStatus.IsActive;
  p_obj->datalog_model->s_models[id]->sensorStatus.ODR = sensorStatus.ODR;
  p_obj->datalog_model->s_models[id]->sensorStatus.FS = sensorStatus.FS;

  SQInit(&q3, SMGetSensorManager());
  id = SQNextByNameAndType(&q3, "ism330dhcx", COM_TYPE_GYRO);
  sensorStatus = SMSensorGetStatus(id);
  p_obj->datalog_model->s_models[id]->sensorStatus.IsActive = sensorStatus.IsActive;
  p_obj->datalog_model->s_models[id]->sensorStatus.ODR = sensorStatus.ODR;
  p_obj->datalog_model->s_models[id]->sensorStatus.FS = sensorStatus.FS;

  SQInit(&q4, SMGetSensorManager());
  id = SQNextByNameAndType(&q4, "ism330dhcx", COM_TYPE_MLC);
  sensorStatus = SMSensorGetStatus(id);
  p_obj->datalog_model->s_models[id]->sensorStatus.IsActive = sensorStatus.IsActive;

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

    if (message == DT_FORCE_STEP)
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

    if (message == DT_FORCE_STEP)
    {
      __NOP();
    }
  }

  return res;
}


