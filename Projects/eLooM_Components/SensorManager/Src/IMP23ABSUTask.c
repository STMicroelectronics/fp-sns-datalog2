/**
  ******************************************************************************
  * @file    IMP23ABSUTask.c
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

#include "IMP23ABSUTask.h"
#include "IMP23ABSUTask_vtbl.h"
#include "drivers/AnalogMicDriver.h"
#include "drivers/AnalogMicDriver_vtbl.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "SMMessageParser.h"
#include "services/sysdebug.h"
#include <string.h>

#ifndef IMP23ABSU_TASK_CFG_STACK_DEPTH
#define IMP23ABSU_TASK_CFG_STACK_DEPTH           (TX_MINIMUM_STACK*2)
#endif

#ifndef IMP23ABSU_TASK_CFG_PRIORITY
#define IMP23ABSU_TASK_CFG_PRIORITY              (TX_MAX_PRIORITIES - 1)
#endif

#ifndef IMP23ABSU_TASK_CFG_IN_QUEUE_LENGTH
#define IMP23ABSU_TASK_CFG_IN_QUEUE_LENGTH      20
#endif

#define IMP23ABSU_TASK_CFG_IN_QUEUE_ITEM_SIZE   sizeof(SMMessage)

#define MAX_AMIC_SAMPLING_FREQUENCY             (uint32_t)(192000)

#define SYS_DEBUGF(level, message)               SYS_DEBUGF3(SYS_DBG_IMP23ABSU, level, message)

#if defined(DEBUG) || defined (SYS_DEBUG)
#define sTaskObj                                 sIMP23ABSUTaskObj
#endif

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static int16_t dummyDataCounter = 0;
#endif

/**
  *  IMP23ABSUTask internal structure.
  */
struct _IMP23ABSUTask
{
  /**
    * Base class object.
    */
  AManagedTaskEx super;

  /**
    * Driver object.
    */
  IDriver *p_driver;

  /**
    * HAL MDF driver configuration parameters.
    */
  const void *p_mx_mdf_cfg;

  /**
    *
    */
  const void *p_mx_adc_cfg;

  /**
    * Implements the mic ISensor interface.
    */
  ISensor_t sensor_if;

  /**
    * Specifies mic sensor capabilities.
    */
  const SensorDescriptor_t *sensor_descriptor;

  /**
    * Specifies mic sensor configuration.
    */
  SensorStatus_t sensor_status;

  EMData_t data;
  /**
    * Specifies the sensor ID for the microphone subsensor.
    */
  uint8_t mic_id;

  /**
    * Synchronization object used to send command to the task.
    */
  TX_QUEUE in_queue;

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_event_src;

  /**
    * Buffer to store the data read from the sensor
    */
  int16_t p_sensor_data_buff[((MAX_AMIC_SAMPLING_FREQUENCY / 1000) * 2)];

#if (HSD_USE_DUMMY_DATA == 1)
  /**
    * Buffer to store dummy data buffer
    */
  int16_t p_dummy_data_buff[((MAX_AMIC_SAMPLING_FREQUENCY / 1000))];
#endif

  /**
    * Used to update the instantaneous ODR.
    */
  double prev_timestamp;

  uint8_t half;

};

/**
  * Class object declaration
  */
typedef struct _IMP23ABSUTaskClass
{
  /**
    * IMP23ABSUTask class virtual table.
    */
  AManagedTaskEx_vtbl vtbl;

  /**
    * Microphone IF virtual table.
    */
  ISensor_vtbl sensor_if_vtbl;

  /**
    * Specifies mic sensor capabilities.
    */
  SensorDescriptor_t class_descriptor;

  /**
    * IMP23ABSUTask (PM_STATE, ExecuteStepFunc) map.
    */
  pExecuteStepFunc_t p_pm_state2func_map[3];
} IMP23ABSUTaskClass_t;

/**
  * STM32 HAL callback function.
  *
  * @param hmdf [IN] specifies a MDF instance.
  */
void MDF_Filter_1_Complete_Callback(MDF_HandleTypeDef *hmdf);

/**
  * STM32 HAL callback function.
  *
  * @param hmdf  [IN] specifies a MDF instance.
  */
void MDF_Filter_1_HalfComplete_Callback(MDF_HandleTypeDef *hmdf);

/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IMP23ABSUTaskExecuteStepRun(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IMP23ABSUTaskExecuteStepDatalog(AManagedTask *_this);

#if (HSD_USE_DUMMY_DATA == 1)
/**
  * Read dummy data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static void IMP23ABSUTaskWriteDummyData(IMP23ABSUTask *_this);
#endif

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t IMP23ABSUTaskSensorRegister(IMP23ABSUTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t IMP23ABSUTaskSensorInitTaskParams(IMP23ABSUTask *_this);

/**
  * Private implementation of sensor interface methods for IMP23ABSU sensor
  */
static sys_error_code_t IMP23ABSUTaskSensorSetODR(IMP23ABSUTask *_this, SMMessage report);
static sys_error_code_t IMP23ABSUTaskSensorSetFS(IMP23ABSUTask *_this, SMMessage report);
static sys_error_code_t IMP23ABSUTaskSensorEnable(IMP23ABSUTask *_this, SMMessage report);
static sys_error_code_t IMP23ABSUTaskSensorDisable(IMP23ABSUTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t IMP23ABSUTaskSensorIsActive(const IMP23ABSUTask *_this);

/* Inline function forward declaration */
// ***********************************
/**
  * Private function used to post a report into the front of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t IMP23ABSUTaskPostReportToFront(IMP23ABSUTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t IMP23ABSUTaskPostReportToBack(IMP23ABSUTask *_this, SMMessage *pReport);


/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
static IMP23ABSUTask sTaskObj;

/**
  * The class object.
  */
static const IMP23ABSUTaskClass_t sTheClass =
{
  /* Class virtual table */
  {
    IMP23ABSUTask_vtblHardwareInit,
    IMP23ABSUTask_vtblOnCreateTask,
    IMP23ABSUTask_vtblDoEnterPowerMode,
    IMP23ABSUTask_vtblHandleError,
    IMP23ABSUTask_vtblOnEnterTaskControlLoop,
    IMP23ABSUTask_vtblForceExecuteStep,
    IMP23ABSUTask_vtblOnEnterPowerMode
  },

  /* class::sensor_if_vtbl virtual table */
  {
    IMP23ABSUTask_vtblMicGetId,
    IMP23ABSUTask_vtblGetEventSourceIF,
    IMP23ABSUTask_vtblMicGetDataInfo,
    IMP23ABSUTask_vtblMicGetODR,
    IMP23ABSUTask_vtblMicGetFS,
    IMP23ABSUTask_vtblMicGetSensitivity,
    IMP23ABSUTask_vtblSensorSetODR,
    IMP23ABSUTask_vtblSensorSetFS,
    IMP23ABSUTask_vtblSensorSetFifoWM,
    IMP23ABSUTask_vtblSensorEnable,
    IMP23ABSUTask_vtblSensorDisable,
    IMP23ABSUTask_vtblSensorIsEnabled,
    IMP23ABSUTask_vtblSensorGetDescription,
    IMP23ABSUTask_vtblSensorGetStatus
  },

  /* MIC DESCRIPTOR */
  {
    "imp23absu",
    COM_TYPE_MIC,
    {
      16000.0,
      32000.0,
      48000.0,
      96000.0,
      192000.0,
      COM_END_OF_LIST_FLOAT,
    },
    {
      130.0,
      COM_END_OF_LIST_FLOAT,
    },
    {
      "aud",
    },
    "Waveform",
    {
      0,
      1000,
    }
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    IMP23ABSUTaskExecuteStepRun,
    NULL,
    IMP23ABSUTaskExecuteStepDatalog,
  }
};

/* Public API definition */
// *********************
ISourceObservable *IMP23ABSUTaskGetMicSensorIF(IMP23ABSUTask *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *IMP23ABSUTaskAlloc(const void *p_mx_mdf_cfg, const void *p_mx_adc_cfg)
{
  /* This allocator implements the singleton design pattern. */

  /* Initialize the super class */
  AMTInitEx(&sTaskObj.super);

  sTaskObj.super.vptr = &sTheClass.vtbl;
  sTaskObj.p_mx_adc_cfg = p_mx_adc_cfg;
  sTaskObj.p_mx_mdf_cfg = p_mx_mdf_cfg;
  sTaskObj.sensor_if.vptr = &sTheClass.sensor_if_vtbl;
  sTaskObj.sensor_descriptor = &sTheClass.class_descriptor;

  strcpy(sTaskObj.sensor_status.Name, sTheClass.class_descriptor.Name);

  return (AManagedTaskEx *) &sTaskObj;
}

IEventSrc *IMP23ABSUTaskGetEventSrcIF(IMP23ABSUTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_event_src;
}

// AManagedTask virtual functions definition
// *****************************************

sys_error_code_t IMP23ABSUTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IMP23ABSUTask *p_obj = (IMP23ABSUTask *) _this;

  p_obj->p_driver = AnalogMicDriverAlloc();
  if (p_obj == NULL)
  {
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
  }
  else
  {
    AnalogMicDriverParams_t cfg_params =
    {
      .p_mx_adc_cfg = (void *) p_obj->p_mx_adc_cfg,
      .p_mx_mdf_cfg = (void *) p_obj->p_mx_mdf_cfg
    };

    res = IDrvInit(p_obj->p_driver, &cfg_params);
    if (!SYS_IS_ERROR_CODE(res))
    {
      MDFDriverFilterRegisterCallback((MDFDriver_t *) p_obj->p_driver, HAL_MDF_ACQ_HALFCOMPLETE_CB_ID,
                                      MDF_Filter_1_HalfComplete_Callback);
      MDFDriverFilterRegisterCallback((MDFDriver_t *) p_obj->p_driver, HAL_MDF_ACQ_COMPLETE_CB_ID,
                                      MDF_Filter_1_Complete_Callback);
    }
  }

  return res;
}

sys_error_code_t IMP23ABSUTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                                VOID **pvStackStart,
                                                ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                                ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IMP23ABSUTask *p_obj = (IMP23ABSUTask *) _this;

  *pTaskCode = AMTExRun;
  *pName = "IMP23ABSU";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = IMP23ABSU_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = IMP23ABSU_TASK_CFG_PRIORITY;
  *pPreemptThreshold = IMP23ABSU_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  /* Create task specific sw resources. */
  uint32_t item_size = (uint32_t) IMP23ABSU_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(IMP23ABSU_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "IMP23ABSU_Q", item_size / 4u, p_queue_items_buff,
                                    IMP23ABSU_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  p_obj->p_event_src = (IEventSrc *) DataEventSrcAlloc();
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
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  res = IMP23ABSUTaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }

  res = IMP23ABSUTaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IMP23ABSU: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t IMP23ABSUTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                    const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IMP23ABSUTask *p_obj = (IMP23ABSUTask *) _this;

  if (IMP23ABSUTaskSensorIsActive(p_obj))
  {
    IDrvDoEnterPowerMode(p_obj->p_driver, ActivePowerMode, NewPowerMode);
  }

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (IMP23ABSUTaskSensorIsActive(p_obj))
    {
      SMMessage xReport =
      {
        .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
        .sensorMessage.nCmdID = SENSOR_CMD_ID_INIT
      };

      if (tx_queue_send(&p_obj->in_queue, &xReport, AMT_MS_TO_TICKS(50)) != TX_SUCCESS)
      {
        //TD FF pdMS_TO_TICKS(100)
        res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE);
      }

      // reset the variables for the time stamp computation.
      p_obj->prev_timestamp = 0.0f;
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IMP23ABSU: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      tx_queue_flush(&p_obj->in_queue);
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IMP23ABSU: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power (this code is executed from the INIT task).*/

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IMP23ABSU: -> SLEEP_1\r\n"));
  }
  return res;
}

sys_error_code_t IMP23ABSUTask_vtblHandleError(AManagedTask *_this, SysEvent Error)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  IMP23ABSUTask *p_obj = (IMP23ABSUTask*)_this;

  return res;
}

sys_error_code_t IMP23ABSUTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IMP23ABSU: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (IMP23ABSU_TASK_CFG_TAG)
  IMP23ABSUTask *p_obj = (IMP23ABSUTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = IMP23ABSU_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

/* AManagedTaskEx virtual functions definition */
// *******************************************
sys_error_code_t IMP23ABSUTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IMP23ABSUTask *p_obj = (IMP23ABSUTask *) _this;

  SMMessage xReport =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = IMP23ABSUTaskPostReportToFront(p_obj, (SMMessage *) &xReport);
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

sys_error_code_t IMP23ABSUTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                    const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IMP23ABSUTask *p_obj = (IMP23ABSUTask *) _this;

  if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      if (p_obj->sensor_status.IsActive)
      {
        res = IDrvStop(p_obj->p_driver);
      }
    }
  }
  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t IMP23ABSUTask_vtblMicGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IMP23ABSUTask *p_if_owner = (IMP23ABSUTask *)((uint32_t) _this - offsetof(IMP23ABSUTask, sensor_if));
  uint8_t res = p_if_owner->mic_id;

  return res;
}

IEventSrc *IMP23ABSUTask_vtblGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IMP23ABSUTask *p_if_owner = (IMP23ABSUTask *)((uint32_t) _this - offsetof(IMP23ABSUTask, sensor_if));
  return p_if_owner->p_event_src;
}

sys_error_code_t IMP23ABSUTask_vtblMicGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  IMP23ABSUTask *p_if_owner = (IMP23ABSUTask *)((uint32_t) _this - offsetof(IMP23ABSUTask, sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if ((p_measured == NULL) || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = p_if_owner->sensor_status.MeasuredODR;
    *p_nominal = p_if_owner->sensor_status.ODR;
  }

  return res;
}

float IMP23ABSUTask_vtblMicGetFS(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IMP23ABSUTask *p_if_owner = (IMP23ABSUTask *)((uint32_t) _this - offsetof(IMP23ABSUTask, sensor_if));
  float res = p_if_owner->sensor_status.FS;

  return res;
}

float IMP23ABSUTask_vtblMicGetSensitivity(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IMP23ABSUTask *p_if_owner = (IMP23ABSUTask *)((uint32_t) _this - offsetof(IMP23ABSUTask, sensor_if));
  float res = p_if_owner->sensor_status.Sensitivity;

  return res;
}

EMData_t IMP23ABSUTask_vtblMicGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IMP23ABSUTask *p_if_owner = (IMP23ABSUTask *)((uint32_t) _this - offsetof(IMP23ABSUTask, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t IMP23ABSUTask_vtblSensorSetODR(ISensor_t *_this, float ODR)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IMP23ABSUTask *p_if_owner = (IMP23ABSUTask *)((uint32_t) _this - offsetof(IMP23ABSUTask, sensor_if));

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_ODR,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) ODR
    };
    res = IMP23ABSUTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t IMP23ABSUTask_vtblSensorSetFS(ISensor_t *_this, float FS)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IMP23ABSUTask *p_if_owner = (IMP23ABSUTask *)((uint32_t) _this - offsetof(IMP23ABSUTask, sensor_if));

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FS,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) FS
    };
    res = IMP23ABSUTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t IMP23ABSUTask_vtblSensorSetFifoWM(ISensor_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  /* Does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
  SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("IMP23ABSU: warning - SetFifoWM() not supported.\r\n"));
  return SYS_INVALID_FUNC_CALL_ERROR_CODE;
}

sys_error_code_t IMP23ABSUTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IMP23ABSUTask *p_if_owner = (IMP23ABSUTask *)((uint32_t) _this - offsetof(IMP23ABSUTask, sensor_if));

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_ENABLE,
      .sensorMessage.nSensorId = sensor_id
    };
    res = IMP23ABSUTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t IMP23ABSUTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IMP23ABSUTask *p_if_owner = (IMP23ABSUTask *)((uint32_t) _this - offsetof(IMP23ABSUTask, sensor_if));

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_DISABLE,
      .sensorMessage.nSensorId = sensor_id
    };
    res = IMP23ABSUTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t IMP23ABSUTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  IMP23ABSUTask *p_if_owner = (IMP23ABSUTask *)((uint32_t) _this - offsetof(IMP23ABSUTask, sensor_if));

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->mic_id)
  {
    res = p_if_owner->sensor_status.IsActive;
  }

  return res;
}

SensorDescriptor_t IMP23ABSUTask_vtblSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IMP23ABSUTask *p_if_owner = (IMP23ABSUTask *)((uint32_t) _this - offsetof(IMP23ABSUTask, sensor_if));

  return *p_if_owner->sensor_descriptor;
}

SensorStatus_t IMP23ABSUTask_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IMP23ABSUTask *p_if_owner = (IMP23ABSUTask *)((uint32_t) _this - offsetof(IMP23ABSUTask, sensor_if));

  return p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t IMP23ABSUTaskExecuteStepRun(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IMP23ABSUTask *p_obj = (IMP23ABSUTask *) _this;
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
          case SENSOR_CMD_ID_SET_ODR:
            res = IMP23ABSUTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = IMP23ABSUTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = IMP23ABSUTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = IMP23ABSUTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IMP23ABSU: unexpected report in Run: %i\r\n", report.messageID));
            SYS_DEBUGF3(SYS_DBG_APP, SYS_DBG_LEVEL_WARNING, ("IMP23ABSU: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IMP23ABSU: unexpected report in Run: %i\r\n", report.messageID));
        SYS_DEBUGF3(SYS_DBG_APP, SYS_DBG_LEVEL_WARNING, ("IMP23ABSU: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t IMP23ABSUTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IMP23ABSUTask *p_obj = (IMP23ABSUTask *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("IMP23ABSU: new data.\r\n"));

        p_obj->half = report.sensorDataReadyMessage.half;

        // notify the listeners...
        double timestamp = report.sensorDataReadyMessage.fTimestamp;
        double delta_timestamp = timestamp - p_obj->prev_timestamp;
        p_obj->prev_timestamp = timestamp;

        /* update measuredODR */
        p_obj->sensor_status.MeasuredODR = (float)((p_obj->sensor_status.ODR / 1000.0f)) / (float) delta_timestamp;
        uint16_t samples = (uint16_t)(p_obj->sensor_status.ODR / 1000u);

#if (HSD_USE_DUMMY_DATA == 1)
        IMP23ABSUTaskWriteDummyData(p_obj);
        EMD_1dInit(&p_obj->data, (uint8_t *) &p_obj->p_dummy_data_buff[0], E_EM_INT16, samples);
#else
        EMD_1dInit(&p_obj->data, (uint8_t *) &p_obj->p_sensor_data_buff[(p_obj->half - 1) * samples], E_EM_INT16, samples);
#endif
        DataEvent_t evt;

        DataEventInit((IEvent *) &evt, p_obj->p_event_src, &p_obj->data, timestamp, p_obj->mic_id);
        IEventSrcSendEvent(p_obj->p_event_src, (IEvent *) &evt, NULL);


        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("IMP23ABSU: ts = %f\r\n", (float)timestamp));
        break;
      }
      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
            res = AnalogMicDrvSetDataBuffer((AnalogMicDriver_t *) p_obj->p_driver, p_obj->p_sensor_data_buff, ((uint32_t)p_obj->sensor_status.ODR / 1000) * 2);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->sensor_status.IsActive == true)
              {
                res = IDrvStart(p_obj->p_driver);
              }
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = IMP23ABSUTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = IMP23ABSUTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = IMP23ABSUTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = IMP23ABSUTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IMP23ABSU: unexpected report in Datalog: %i\r\n", report.messageID));
            SYS_DEBUGF3(SYS_DBG_APP, SYS_DBG_LEVEL_WARNING, ("IMP23ABSU: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IMP23ABSU: unexpected report in Datalog: %i\r\n", report.messageID));
        SYS_DEBUGF3(SYS_DBG_APP, SYS_DBG_LEVEL_WARNING, ("IMP23ABSU: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static inline sys_error_code_t IMP23ABSUTaskPostReportToFront(IMP23ABSUTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t IMP23ABSUTaskPostReportToBack(IMP23ABSUTask *_this, SMMessage *pReport)
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
static void IMP23ABSUTaskWriteDummyData(IMP23ABSUTask *_this)
{
  assert_param(_this != NULL);
  int16_t *p16 = _this->p_dummy_data_buff;
  uint16_t idx = 0;
  uint16_t samples = ((uint32_t)_this->sensor_status.ODR / 1000);

  for (idx = 0; idx < samples; idx++)
  {
    *p16++ = dummyDataCounter++;
  }
}
#endif

static sys_error_code_t IMP23ABSUTaskSensorRegister(IMP23ABSUTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *mic_if = (ISensor_t *) IMP23ABSUTaskGetMicSensorIF(_this);
  _this->mic_id = SMAddSensor(mic_if);

  return res;
}

static sys_error_code_t IMP23ABSUTaskSensorInitTaskParams(IMP23ABSUTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* MIC STATUS */
  _this->sensor_status.IsActive = TRUE;
  _this->sensor_status.FS = 130.0f;
  _this->sensor_status.Sensitivity = 1.0f;
  _this->sensor_status.ODR = 192000.0f;
  _this->sensor_status.MeasuredODR = 0.0f;
  EMD_1dInit(&_this->data, (uint8_t *) _this->p_sensor_data_buff, E_EM_INT16, 1);

  return res;
}

static sys_error_code_t IMP23ABSUTaskSensorSetODR(IMP23ABSUTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IMP23ABSUTask *p_obj = (IMP23ABSUTask *) _this;

  float ODR = (float) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->mic_id)
  {
    if (ODR <= 16000.0f)
    {
      ODR = 16000.0f;
    }
    else if (ODR <= 32000.0f)
    {
      ODR = 32000.0f;
    }
    else if (ODR <= 48000.0f)
    {
      ODR = 48000.0f;
    }
    else if (ODR <= 96000.0f)
    {
      ODR = 96000.0f;
    }
    else
    {
      ODR = 192000.0f;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      MDFSetMDFConfig(p_obj->p_driver, ODR);
      _this->sensor_status.ODR = ODR;
      _this->sensor_status.MeasuredODR = 0.0f;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t IMP23ABSUTaskSensorSetFS(IMP23ABSUTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  float FS = (float) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->mic_id)
  {
    if (FS != 130.0f)
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->sensor_status.FS = FS;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t IMP23ABSUTaskSensorEnable(IMP23ABSUTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->mic_id)
  {
    _this->sensor_status.IsActive = TRUE;
  }
  else
    res = SYS_INVALID_PARAMETER_ERROR_CODE;

  return res;
}

static sys_error_code_t IMP23ABSUTaskSensorDisable(IMP23ABSUTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->mic_id)
  {
    _this->sensor_status.IsActive = FALSE;
  }
  else
    res = SYS_INVALID_PARAMETER_ERROR_CODE;

  return res;
}

static boolean_t IMP23ABSUTaskSensorIsActive(const IMP23ABSUTask *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.IsActive;
}

void MDF_Filter_1_Complete_Callback(MDF_HandleTypeDef *hmdf)
{
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.half = 2;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  //  if (sTaskObj.in_queue != NULL) { //TODO: STF.Port - how to check if the queue has been initialized ??
  if (TX_SUCCESS != tx_queue_send(&sTaskObj.in_queue, &report, TX_NO_WAIT))
  {
    /* unable to send the report. Signal the error */
    sys_error_handler();
  }

  // }
}

void MDF_Filter_1_HalfComplete_Callback(MDF_HandleTypeDef *hmdf)
{
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.half = 1;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  //  if (sTaskObj.in_queue != NULL) { //TODO: STF.Port - how to check if the queue has been initialized ??
  if (TX_SUCCESS != tx_queue_send(&sTaskObj.in_queue, &report, TX_NO_WAIT))
  {
    /* unable to send the report. Signal the error */
    sys_error_handler();
  }

  // }
}

