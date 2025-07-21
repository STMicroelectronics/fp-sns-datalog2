/**
  ******************************************************************************
  * @file    MP23DB01HPTask.c
  * @author  SRA - MCD
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

/* Includes ------------------------------------------------------------------*/
#include "MP23DB01HPTask.h"
#include "MP23DB01HPTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include <string.h>
#include "services/sysdebug.h"

/* Private includes ----------------------------------------------------------*/

#ifndef MP23DB01HP_TASK_CFG_STACK_DEPTH
#define MP23DB01HP_TASK_CFG_STACK_DEPTH           (TX_MINIMUM_STACK*2)
#endif

#ifndef MP23DB01HP_TASK_CFG_PRIORITY
#define MP23DB01HP_TASK_CFG_PRIORITY              (TX_MAX_PRIORITIES - 1)
#endif

#ifndef MP23DB01HP_TASK_CFG_IN_QUEUE_LENGTH
#define MP23DB01HP_TASK_CFG_IN_QUEUE_LENGTH      20
#endif

#define MP23DB01HP_TASK_CFG_IN_QUEUE_ITEM_SIZE   sizeof(SMMessage)

#ifndef MP23DB01HP_TASK_CFG_MAX_INSTANCES_COUNT
#define MP23DB01HP_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)               SYS_DEBUGF3(SYS_DBG_MP23DB01HP, level, message)

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter = 0;
#endif


/**
  * Class object declaration
  */
typedef struct _MP23DB01HPTaskClass
{
  /**
    * MP23DB01HPTask class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * Microphone IF virtual table.
    */
  const ISensorAudio_vtbl sensor_if_vtbl;

  /**
    * Specifies mic sensor capabilities.
    */
  const SensorDescriptor_t class_descriptor;

  /**
    * MP23DB01HPTask (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[MP23DB01HP_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} MP23DB01HPTaskClass_t;


/* Private member function declaration */// ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t MP23DB01HPTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t MP23DB01HPTaskExecuteStepDatalog(AManagedTask *_this);

#if (HSD_USE_DUMMY_DATA == 1)
/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static void MP23DB01HPTaskWriteDummyData(MP23DB01HPTask *_this);
#endif

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t MP23DB01HPTaskSensorRegister(MP23DB01HPTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t MP23DB01HPTaskSensorInitTaskParams(MP23DB01HPTask *_this);

/**
  * Private implementation of sensor interface methods for MP23DB01HP sensor
  */
static sys_error_code_t MP23DB01HPTaskSensorSetFrequency(MP23DB01HPTask *_this, SMMessage report);
static sys_error_code_t MP23DB01HPTaskSensorSetVolume(MP23DB01HPTask *_this, SMMessage report);
static sys_error_code_t MP23DB01HPTaskSensorEnable(MP23DB01HPTask *_this, SMMessage report);
static sys_error_code_t MP23DB01HPTaskSensorDisable(MP23DB01HPTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t MP23DB01HPTaskSensorIsActive(const MP23DB01HPTask *_this);

/* Inline function forward declaration */
/***************************************/

/**
  * Private function used to post a report into the front of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t MP23DB01HPTaskPostReportToFront(MP23DB01HPTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t MP23DB01HPTaskPostReportToBack(MP23DB01HPTask *_this, SMMessage *pReport);


/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static MP23DB01HPTask sTaskObj;

/**
  * The class object.
  */
static MP23DB01HPTaskClass_t sTheClass =
{
  /* class virtual table */
  {
    MP23DB01HPTask_vtblHardwareInit,
    MP23DB01HPTask_vtblOnCreateTask,
    MP23DB01HPTask_vtblDoEnterPowerMode,
    MP23DB01HPTask_vtblHandleError,
    MP23DB01HPTask_vtblOnEnterTaskControlLoop,
    MP23DB01HPTask_vtblForceExecuteStep,
    MP23DB01HPTask_vtblOnEnterPowerMode
  },

  /* class::sensor_if_vtbl virtual table */
  {
    {
      {
        MP23DB01HPTask_vtblMicGetId,
        MP23DB01HPTask_vtblGetEventSourceIF,
        MP23DB01HPTask_vtblMicGetDataInfo
      },
      MP23DB01HPTask_vtblSensorEnable,
      MP23DB01HPTask_vtblSensorDisable,
      MP23DB01HPTask_vtblSensorIsEnabled,
      MP23DB01HPTask_vtblSensorGetDescription,
      MP23DB01HPTask_vtblSensorGetStatus,
      MP23DB01HPTask_vtblSensorGetStatusPointer
    },
    MP23DB01HPTask_vtblMicGetFrequency,
    MP23DB01HPTask_vtblMicGetVolume,
    MP23DB01HPTask_vtblMicGetResolution,
    MP23DB01HPTask_vtblSensorSetFrequency,
    MP23DB01HPTask_vtblSensorSetVolume,
    MP23DB01HPTask_vtblSensorSetResolution
  },

  /* MIC DESCRIPTOR */
  {
    "mp23db01hp",
    COM_TYPE_MIC
  },
  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    MP23DB01HPTaskExecuteStepState1,
    NULL,
    MP23DB01HPTaskExecuteStepDatalog,
  },

  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *MP23DB01HPTaskGetMicSensorIF(MP23DB01HPTask *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *MP23DB01HPTaskAlloc(const void *p_mx_mdf_cfg)
{
  MP23DB01HPTask *p_new_obj = SysAlloc(sizeof(MP23DB01HPTask));

  if (p_new_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_new_obj->super);

    p_new_obj->super.vptr = &sTheClass.vtbl;
    p_new_obj->sensor_if.vptr = &sTheClass.sensor_if_vtbl;
    p_new_obj->sensor_descriptor = &sTheClass.class_descriptor;

    p_new_obj->p_mx_mdf_cfg = (MX_GPIOParams_t *) p_mx_mdf_cfg;

    strcpy(p_new_obj->sensor_status.p_name, sTheClass.class_descriptor.p_name);
  }

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *MP23DB01HPTaskAllocSetName(const void *p_mx_mdf_cfg, const char *p_name)
{
  MP23DB01HPTask *p_new_obj = (MP23DB01HPTask *)MP23DB01HPTaskAlloc(p_mx_mdf_cfg);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *MP23DB01HPTaskStaticAlloc(void *p_mem_block, const void *p_mx_mdf_cfg)
{
  MP23DB01HPTask *p_obj = (MP23DB01HPTask *)p_mem_block;

  if (p_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_obj->super);
    p_obj->super.vptr = &sTheClass.vtbl;

    p_obj->super.vptr = &sTheClass.vtbl;
    p_obj->sensor_if.vptr = &sTheClass.sensor_if_vtbl;
    p_obj->sensor_descriptor = &sTheClass.class_descriptor;

    p_obj->p_mx_mdf_cfg = (MX_GPIOParams_t *) p_mx_mdf_cfg;
  }

  return (AManagedTaskEx *)p_obj;
}

AManagedTaskEx *MP23DB01HPTaskStaticAllocSetName(void *p_mem_block, const void *p_mx_mdf_cfg, const char *p_name)
{
  MP23DB01HPTask *p_obj = (MP23DB01HPTask *)MP23DB01HPTaskStaticAlloc(p_mem_block, p_mx_mdf_cfg);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}


IEventSrc *MP23DB01HPTaskGetEventSrcIF(MP23DB01HPTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t MP23DB01HPTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MP23DB01HPTask *p_obj = (MP23DB01HPTask *) _this;

  p_obj->p_driver = MDFDriverAlloc();
  if (p_obj == NULL)
  {
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
  }
  else
  {
    MDFDriverParams_t cfg_params =
    {
      .p_mx_mdf_cfg = (void *) p_obj->p_mx_mdf_cfg,
      .param = 7
    };

    res = IDrvInit(p_obj->p_driver, &cfg_params);
    if (!SYS_IS_ERROR_CODE(res))
    {
      MDFDriverFilterRegisterCallback((MDFDriver_t *) p_obj->p_driver, HAL_MDF_ACQ_HALFCOMPLETE_CB_ID,
                                      MDF_Filter_0_HalfComplete_Callback);
      MDFDriverFilterRegisterCallback((MDFDriver_t *) p_obj->p_driver, HAL_MDF_ACQ_COMPLETE_CB_ID,
                                      MDF_Filter_0_Complete_Callback);
    }

    if (!MTMap_IsInitialized(&sTheClass.task_map))
    {
      (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, MP23DB01HP_TASK_CFG_MAX_INSTANCES_COUNT);
    }

    /* Add the managed task to the map.*/
    /* Use the PIN as unique key for the map. */
    MTMapElement_t *p_element = NULL;
//    uint32_t key = (uint32_t) p_obj->pIRQConfig->pin;
    uint32_t key = (uint32_t)((MDFDriver_t *) p_obj->p_driver)->mx_handle.p_mx_mdf_cfg->p_mdf;
    p_element = MTMap_AddElement(&sTheClass.task_map, key, _this);
    if (p_element == NULL)
    {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      return res;
    }
  }

  return res;
}

sys_error_code_t MP23DB01HPTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                                 VOID **pvStackStart,
                                                 ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                                 ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MP23DB01HPTask *p_obj = (MP23DB01HPTask *) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t)MP23DB01HP_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(MP23DB01HP_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "MP23DB01HP_Q", item_size / 4u, p_queue_items_buff,
                                    MP23DB01HP_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Initialize the EventSrc interface */
  p_obj->p_event_src = DataEventSrcAlloc();
  if (p_obj->p_event_src == NULL)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }
  IEventSrcInit(p_obj->p_event_src);

  memset(p_obj->p_sensor_data_buff, 0, sizeof(p_obj->p_sensor_data_buff));
  p_obj->mic_id = 0;
  p_obj->prev_timestamp = 0.0f;
  p_obj->half = 0;
  p_obj->old_in = 0;
  p_obj->old_out = 0;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "MP23DB01HP";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = MP23DB01HP_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = MP23DB01HP_TASK_CFG_PRIORITY;
  *pPreemptThreshold = MP23DB01HP_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = MP23DB01HPTaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = MP23DB01HPTaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("MP23DB01HP: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t MP23DB01HPTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MP23DB01HPTask *p_obj = (MP23DB01HPTask *) _this;

  if (MP23DB01HPTaskSensorIsActive(p_obj))
  {
    IDrvDoEnterPowerMode(p_obj->p_driver, ActivePowerMode, NewPowerMode);
  }

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (MP23DB01HPTaskSensorIsActive(p_obj))
    {
      SMMessage xReport =
      {
        .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
        .sensorMessage.nCmdID = SENSOR_CMD_ID_INIT
      };

      if (tx_queue_send(&p_obj->in_queue, &xReport, AMT_MS_TO_TICKS(50)) != TX_SUCCESS)
      {
        res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE);
      }

      // reset the variables for the actual ODR computation.
      p_obj->prev_timestamp = 0.0f;
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("MP23DB01HP: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("MP23DB01HP: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power (this code is executed from the INIT task).*/

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("MP23DB01HP: -> SLEEP_1\r\n"));
  }
  return res;
}

sys_error_code_t MP23DB01HPTask_vtblHandleError(AManagedTask *_this, SysEvent Error)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  MP23DB01HPTask *p_obj = (MP23DB01HPTask*)_this;

  return res;
}

sys_error_code_t MP23DB01HPTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("MP23DB01HP: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (MP23DB01HP_TASK_CFG_TAG)
  MP23DB01HPTask *p_obj = (MP23DB01HPTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = MP23DB01HP_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t MP23DB01HPTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MP23DB01HPTask *p_obj = (MP23DB01HPTask *) _this;

  SMMessage xReport =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = MP23DB01HPTaskPostReportToFront(p_obj, (SMMessage *) &xReport);
    }
    else
    {
      // do nothing and wait for the step to complete.
      //      _this->m_xStatus.nDelayPowerModeSwitch = 0;
    }
  }
  else
  {
    UINT state;
    if (TX_SUCCESS == tx_thread_info_get(&_this->m_xTaskHandle, TX_NULL, &state, TX_NULL, TX_NULL, TX_NULL, TX_NULL,
                                         TX_NULL,
                                         TX_NULL))
    {
      if (state == TX_SUSPENDED)
      {
        tx_thread_resume(&_this->m_xTaskHandle);
      }
    }
  }
  return res;
}

sys_error_code_t MP23DB01HPTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MP23DB01HPTask *p_obj = (MP23DB01HPTask *) _this;

  if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      if (p_obj->sensor_status.is_active)
      {
        res = IDrvStop(p_obj->p_driver);
      }
    }
  }
  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t MP23DB01HPTask_vtblMicGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
  uint8_t res = p_if_owner->mic_id;

  return res;
}

IEventSrc *MP23DB01HPTask_vtblGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
  return p_if_owner->p_event_src;
}

uint32_t MP23DB01HPTask_vtblMicGetFrequency(ISensorAudio_t *_this)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
  uint32_t res = p_if_owner->sensor_status.type.audio.frequency;

  return res;
}

uint8_t MP23DB01HPTask_vtblMicGetResolution(ISensorAudio_t *_this)
{
  assert_param(_this != NULL);
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
  uint8_t res = p_if_owner->sensor_status.type.audio.resolution;

  return res;
}

uint8_t MP23DB01HPTask_vtblMicGetVolume(ISensorAudio_t *_this)
{
  assert_param(_this != NULL);
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
  uint8_t res = p_if_owner->sensor_status.type.audio.volume;

  return res;
}

EMData_t MP23DB01HPTask_vtblMicGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t)_this - offsetof(MP23DB01HPTask, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t MP23DB01HPTask_vtblSensorSetFrequency(ISensorAudio_t *_this, uint32_t frequency)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.audio.frequency = frequency;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FREQUENCY,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = frequency
    };
    res = MP23DB01HPTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t MP23DB01HPTask_vtblSensorSetVolume(ISensorAudio_t *_this, uint8_t volume)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    if (volume <= 100)
    {
      p_if_owner->sensor_status.type.audio.volume = volume;
    }
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_VOLUME,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = volume
    };
    res = MP23DB01HPTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t MP23DB01HPTask_vtblSensorSetResolution(ISensorAudio_t *_this, uint8_t bit_depth)
{
  assert_param(_this != NULL);
  /* Does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
  SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("MP23DB01HP: warning - SetResolution() not supported.\r\n"));
  return SYS_INVALID_FUNC_CALL_ERROR_CODE;
}

sys_error_code_t MP23DB01HPTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.is_active = TRUE;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_ENABLE,
      .sensorMessage.nSensorId = sensor_id
    };
    res = MP23DB01HPTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t MP23DB01HPTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.is_active = FALSE;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_DISABLE,
      .sensorMessage.nSensorId = sensor_id
    };
    res = MP23DB01HPTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t MP23DB01HPTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->mic_id)
  {
    res = p_if_owner->sensor_status.is_active;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

SensorDescriptor_t MP23DB01HPTask_vtblSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
  return *p_if_owner->sensor_descriptor;
}

SensorStatus_t MP23DB01HPTask_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));

  return p_if_owner->sensor_status;
}

SensorStatus_t *MP23DB01HPTask_vtblSensorGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));

  return &p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t MP23DB01HPTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MP23DB01HPTask *p_obj = (MP23DB01HPTask *) _this;
  SMMessage report =
  {
    0
  };

  AMTExSetInactiveState((AManagedTaskEx *) _this, TRUE);
  if (TX_SUCCESS == tx_queue_receive(&p_obj->in_queue, &report, TX_WAIT_FOREVER))
  {
    AMTExSetInactiveState((AManagedTaskEx *) _this, FALSE);

    switch (report.messageID)
    {
      case SM_MESSAGE_ID_FORCE_STEP:
      {
        // do nothing. I need only to resume.
        __NOP();
        break;
      }
      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_SET_FREQUENCY:
            res = MP23DB01HPTaskSensorSetFrequency(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_VOLUME:
            res = MP23DB01HPTaskSensorSetVolume(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = MP23DB01HPTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = MP23DB01HPTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("MP23DB01HP: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("MP23DB01HP: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t MP23DB01HPTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MP23DB01HPTask *p_obj = (MP23DB01HPTask *) _this;
  SMMessage report =
  {
    0
  };

  AMTExSetInactiveState((AManagedTaskEx *) _this, TRUE);
  if (TX_SUCCESS == tx_queue_receive(&p_obj->in_queue, &report, TX_WAIT_FOREVER))
  {
    AMTExSetInactiveState((AManagedTaskEx *) _this, FALSE);

    switch (report.messageID)
    {
      case SM_MESSAGE_ID_FORCE_STEP:
      {
        // do nothing. I need only to resume.
        __NOP();
        break;
      }
      case SM_MESSAGE_ID_DATA_READY:
      {
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("MP23DB01HP: new data.\r\n"));

        p_obj->half = report.sensorDataReadyMessage.half;

        // notify the listeners...
        double_t timestamp = report.sensorDataReadyMessage.fTimestamp;
        p_obj->prev_timestamp = timestamp;

        uint16_t samples = (uint16_t)(p_obj->sensor_status.type.audio.frequency / 1000u);

        /* Workaround: MP23DB01HP data are unstable for the first samples -> avoid sending data */
        if (timestamp > 0.3f)
        {
#if (HSD_USE_DUMMY_DATA == 1)
          MP23DB01HPTaskWriteDummyData(p_obj);
          EMD_1dInit(&p_obj->data, (uint8_t *) &p_obj->p_dummy_data_buff[0], E_EM_INT16, samples);
#else
          float_t gain = (float_t)p_obj->sensor_status.type.audio.volume * 0.01f; /*volume is expressed as percentage*/
          for (int32_t i = 0; i < samples; i++)
          {
            p_obj->p_sensor_data_buff[((p_obj->half - 1) * samples) + i] *= gain;
          }
          EMD_1dInit(&p_obj->data, (uint8_t *) &p_obj->p_sensor_data_buff[(p_obj->half - 1) * samples], E_EM_INT16, samples);
#endif
          DataEvent_t evt;

          DataEventInit((IEvent *)&evt, p_obj->p_event_src, &p_obj->data, timestamp, p_obj->mic_id);
          IEventSrcSendEvent(p_obj->p_event_src, (IEvent *) &evt, NULL);

          SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("MP23DB01HP: ts = %f\r\n", (float_t)timestamp));
        }
        break;
      }
      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
            res = MDFDrvSetDataBuffer((MDFDriver_t *) p_obj->p_driver, p_obj->p_sensor_data_buff, ((uint32_t)p_obj->sensor_status.type.audio.frequency / 1000) * 2);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->sensor_status.is_active == true)
              {
                res = IDrvStart(p_obj->p_driver);
              }
            }
            break;
          case SENSOR_CMD_ID_SET_FREQUENCY:
            res = MP23DB01HPTaskSensorSetFrequency(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_VOLUME:
            res = MP23DB01HPTaskSensorSetVolume(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = MP23DB01HPTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = MP23DB01HPTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("MP23DB01HP: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("MP23DB01HP: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t MP23DB01HPTaskPostReportToFront(MP23DB01HPTask *_this, SMMessage *pReport)
{
  assert_param(_this != NULL);
  assert_param(pReport);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (SYS_IS_CALLED_FROM_ISR())
  {
    if (TX_SUCCESS != tx_queue_front_send(&_this->in_queue, pReport, TX_NO_WAIT))
    {
      res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
      // this function is private and the caller will ignore this return code.
    }
  }
  else
  {
    if (TX_SUCCESS != tx_queue_front_send(&_this->in_queue, pReport, AMT_MS_TO_TICKS(100)))
    {
      res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
      // this function is private and the caller will ignore this return code.
    }
  }

  return res;
}

static inline sys_error_code_t MP23DB01HPTaskPostReportToBack(MP23DB01HPTask *_this, SMMessage *pReport)
{
  assert_param(_this != NULL);
  assert_param(pReport);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (SYS_IS_CALLED_FROM_ISR())
  {
    if (TX_SUCCESS != tx_queue_send(&_this->in_queue, pReport, TX_NO_WAIT))
    {
      res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
      // this function is private and the caller will ignore this return code.
    }
  }
  else
  {
    if (TX_SUCCESS != tx_queue_send(&_this->in_queue, pReport, AMT_MS_TO_TICKS(100)))
    {
      res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
      // this function is private and the caller will ignore this return code.
    }
  }

  return res;
}

#if (HSD_USE_DUMMY_DATA == 1)
static void MP23DB01HPTaskWriteDummyData(MP23DB01HPTask *_this)
{
  assert_param(_this != NULL);
  int16_t *p16 = _this->p_dummy_data_buff;
  uint16_t idx = 0;
  uint16_t samples = ((uint32_t)_this->sensor_status.type.audio.frequency / 1000);

  for (idx = 0; idx < samples; idx++)
  {
    *p16++ = dummyDataCounter++;
  }
}
#endif

static sys_error_code_t MP23DB01HPTaskSensorRegister(MP23DB01HPTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *mic_if = (ISensor_t *) MP23DB01HPTaskGetMicSensorIF(_this);
  _this->mic_id = SMAddSensor(mic_if);

  return res;
}

static sys_error_code_t MP23DB01HPTaskSensorInitTaskParams(MP23DB01HPTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* MIC STATUS */
  _this->sensor_status.isensor_class = ISENSOR_CLASS_AUDIO;
  _this->sensor_status.is_active = TRUE;
  _this->sensor_status.type.audio.volume = 100;
  _this->sensor_status.type.audio.frequency = 48000;
  _this->sensor_status.type.audio.resolution = 24;
  EMD_1dInit(&_this->data, (uint8_t *) _this->p_sensor_data_buff, E_EM_INT16, 1);

  return res;
}

static sys_error_code_t MP23DB01HPTaskSensorSetFrequency(MP23DB01HPTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MP23DB01HPTask *p_obj = (MP23DB01HPTask *) _this;

  uint32_t ODR = (uint32_t) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->mic_id)
  {
    if (ODR <= 16000)
    {
      ODR = 16000;
    }
    else if (ODR <= 32000)
    {
      ODR = 32000;
    }
    else
    {
      ODR = 48000;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      MDFSetMDFConfig(p_obj->p_driver, ODR);
      MDFDriverFilterRegisterCallback((MDFDriver_t *) p_obj->p_driver, HAL_MDF_ACQ_HALFCOMPLETE_CB_ID,
                                      MDF_Filter_0_HalfComplete_Callback);
      MDFDriverFilterRegisterCallback((MDFDriver_t *) p_obj->p_driver, HAL_MDF_ACQ_COMPLETE_CB_ID,
                                      MDF_Filter_0_Complete_Callback);

      _this->sensor_status.type.audio.frequency = ODR;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t MP23DB01HPTaskSensorSetVolume(MP23DB01HPTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t FS = (uint8_t) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->mic_id)
  {
    if (FS <= 100.0f)
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->sensor_status.type.audio.volume = FS;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t MP23DB01HPTaskSensorEnable(MP23DB01HPTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->mic_id)
  {
    _this->sensor_status.is_active = TRUE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t MP23DB01HPTaskSensorDisable(MP23DB01HPTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->mic_id)
  {
    _this->sensor_status.is_active = FALSE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t MP23DB01HPTaskSensorIsActive(const MP23DB01HPTask *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.is_active;
}

void MDF_Filter_0_Complete_Callback(MDF_HandleTypeDef *hmdf)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.half = 2;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) hmdf);
  if (p_val != NULL)
  {
    p_queue = &((MP23DB01HPTask *) p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}

void MDF_Filter_0_HalfComplete_Callback(MDF_HandleTypeDef *hmdf)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.half = 1;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) hmdf);
  if (p_val != NULL)
  {
    p_queue = &((MP23DB01HPTask *) p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}
