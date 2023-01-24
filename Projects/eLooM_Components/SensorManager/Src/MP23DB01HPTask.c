/**
  ******************************************************************************
  * @file    MP23DB01HPTask.c
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

#include "MP23DB01HPTask.h"
#include "MP23DB01HPTask_vtbl.h"
#include "drivers/MDFDriver.h"
#include "drivers/MDFDriver_vtbl.h"
#include "services/sysdebug.h"
#include <string.h>
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "SMMessageParser.h"

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

#define MAX_DMIC_SAMPLING_FREQUENCY              (uint32_t)(48000)

#define SYS_DEBUGF(level, message)               SYS_DEBUGF3(SYS_DBG_MP23DB01HP, level, message)

#if defined(DEBUG) || defined (SYS_DEBUG)
#define sTaskObj                                 sMP23DB01HPTaskObj
#endif

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter = 0;
#endif

/**
  *  MP23DB01HPTask internal structure.
  */
struct _MP23DB01HPTask
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
    * Implements the mic ISensor interface.
    */
  ISensor_t sensor_if;

  /**
    * Specifies sensor capabilities.
    */
  const SensorDescriptor_t *sensor_descriptor;

  /**
    * Specifies sensor configuration.
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
  int16_t p_sensor_data_buff[((MAX_DMIC_SAMPLING_FREQUENCY / 1000) * 2)];

  /*
   * Calibration values, used for adjusting audio gain
   */
  int old_in;
  int old_out;

  /**
    * Used to update the instantaneous ODR.
    */
  double prev_timestamp;

  uint8_t half;

};

/**
  * Class object declaration
  */
typedef struct _MP23DB01HPTaskClass
{
  /**
    * MP23DB01HPTask class virtual table.
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
    * MP23DB01HPTask (PM_STATE, ExecuteStepFunc) map.
    */
  pExecuteStepFunc_t p_pm_state2func_map[3];
} MP23DB01HPTaskClass_t;

/**
  * STM32 HAL callback function.
  *
  * @param hmdf [IN] specifies a MDF instance.
  */
void MDF_Filter_0_Complete_Callback(MDF_HandleTypeDef *hmdf);

/**
  * STM32 HAL callback function.
  *
  * @param hmdf  [IN] specifies a MDF instance.
  */
void MDF_Filter_0_HalfComplete_Callback(MDF_HandleTypeDef *hmdf);

/* Private member function declaration */// ***********************************

/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t MP23DB01HPTaskExecuteStepRun(AManagedTask *_this);

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
static sys_error_code_t MP23DB01HPTaskSensorSetODR(MP23DB01HPTask *_this, SMMessage report);
static sys_error_code_t MP23DB01HPTaskSensorSetFS(MP23DB01HPTask *_this, SMMessage report);
static sys_error_code_t MP23DB01HPTaskSensorEnable(MP23DB01HPTask *_this, SMMessage report);
static sys_error_code_t MP23DB01HPTaskSensorDisable(MP23DB01HPTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t MP23DB01HPTaskSensorIsActive(const MP23DB01HPTask *_this);

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

#if defined (__GNUC__)
// Inline function defined inline in the header file MP23DB01HPTask.h must be declared here as extern function.
#endif

/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
static MP23DB01HPTask sTaskObj;

/**
  * The class object.
  */
static const MP23DB01HPTaskClass_t sTheClass =
{
  /* Class virtual table */
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
    MP23DB01HPTask_vtblMicGetId,
    MP23DB01HPTask_vtblGetEventSourceIF,
    MP23DB01HPTask_vtblMicGetDataInfo,
    MP23DB01HPTask_vtblMicGetODR,
    MP23DB01HPTask_vtblMicGetFS,
    MP23DB01HPTask_vtblMicGetSensitivity,
    MP23DB01HPTask_vtblSensorSetODR,
    MP23DB01HPTask_vtblSensorSetFS,
    NULL,
    MP23DB01HPTask_vtblSensorEnable,
    MP23DB01HPTask_vtblSensorDisable,
    MP23DB01HPTask_vtblSensorIsEnabled,
    MP23DB01HPTask_vtblSensorGetDescription,
    MP23DB01HPTask_vtblSensorGetStatus
  },

  /* MIC DESCRIPTOR */
  {
    "mp23db01hp",
    COM_TYPE_MIC,
    {
      16000.0,
      32000.0,
      48000.0,
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
    MP23DB01HPTaskExecuteStepRun,
    NULL,
    MP23DB01HPTaskExecuteStepDatalog,
  }
};

/* Public API definition */
// *********************

ISourceObservable *MP23DB01HPTaskGetMicSensorIF(MP23DB01HPTask *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *MP23DB01HPTaskAlloc(const void *p_mx_mdf_cfg)
{
  /* This allocator implements the singleton design pattern. */

  /* Initialize the super class */
  AMTInitEx(&sTaskObj.super);

  sTaskObj.super.vptr = &sTheClass.vtbl;
  sTaskObj.p_mx_mdf_cfg = p_mx_mdf_cfg;
  sTaskObj.sensor_if.vptr = &sTheClass.sensor_if_vtbl;
  sTaskObj.sensor_descriptor = &sTheClass.class_descriptor;

  return (AManagedTaskEx *) &sTaskObj;
}

IEventSrc *MP23DB01HPTaskGetEventSrcIF(MP23DB01HPTask *_this)
{
  assert_param(_this);

  return _this->p_event_src;
}

// AManagedTask virtual functions definition
// *****************************************

sys_error_code_t MP23DB01HPTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this);
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
  }

  return res;
}

sys_error_code_t MP23DB01HPTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                                VOID **pvStackStart,
                                                ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                                ULONG *pParams)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MP23DB01HPTask *p_obj = (MP23DB01HPTask *) _this;

  *pTaskCode = AMTExRun;
  *pName = "MP23DB01HP";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = MP23DB01HP_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = MP23DB01HP_TASK_CFG_PRIORITY;
  *pPreemptThreshold = MP23DB01HP_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

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
  p_obj->old_in = 0;
  p_obj->old_out = 0;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  res = MP23DB01HPTaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
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
  assert_param(_this);
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
        //TD FF pdMS_TO_TICKS(100)
        res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE);
      }

      // reset the variables for the time stamp computation.
      p_obj->prev_timestamp = 0.0f;
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("MP23DB01HP: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
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
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  MP23DB01HPTask *p_obj = (MP23DB01HPTask*)_this;

  return res;
}

sys_error_code_t MP23DB01HPTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("MP23DB01HP: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (MP23DB01HP_TASK_CFG_TAG)
  MP23DB01HPTask *p_obj = (MP23DB01HPTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = MP23DB01HP_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

/* AManagedTaskEx virtual functions definition */
// *******************************************

sys_error_code_t MP23DB01HPTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this);
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

sys_error_code_t MP23DB01HPTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                    const EPowerMode NewPowerMode)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MP23DB01HPTask *p_obj = (MP23DB01HPTask *) _this;

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

uint8_t MP23DB01HPTask_vtblMicGetId(ISourceObservable *_this)
{
  assert_param(_this);
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
  uint8_t res = p_if_owner->mic_id;

  return res;
}

IEventSrc *MP23DB01HPTask_vtblGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this);
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
  return p_if_owner->p_event_src;
}

sys_error_code_t MP23DB01HPTask_vtblMicGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
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

float MP23DB01HPTask_vtblMicGetFS(ISourceObservable *_this)
{
  assert_param(_this);
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
  float res = p_if_owner->sensor_status.FS;

  return res;
}

float MP23DB01HPTask_vtblMicGetSensitivity(ISourceObservable *_this)
{
  assert_param(_this);
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
  float res = p_if_owner->sensor_status.Sensitivity;

  return res;
}

EMData_t MP23DB01HPTask_vtblMicGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t)_this - offsetof(MP23DB01HPTask, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t MP23DB01HPTask_vtblSensorSetODR(ISensor_t *_this, float ODR)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
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
    res = MP23DB01HPTaskPostReportToFront(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t MP23DB01HPTask_vtblSensorSetFS(ISensor_t *_this, float FS)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
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
    res = MP23DB01HPTaskPostReportToFront(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t MP23DB01HPTask_vtblSensorEnable(ISensor_t *_this)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
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
    res = MP23DB01HPTaskPostReportToFront(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t MP23DB01HPTask_vtblSensorDisable(ISensor_t *_this)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
  EPowerMode log_status = AMTGetSystemPowerMode();
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
    res = MP23DB01HPTaskPostReportToFront(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t MP23DB01HPTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  boolean_t res = FALSE;

  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->mic_id)
  {
    res = p_if_owner->sensor_status.IsActive;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

SensorDescriptor_t MP23DB01HPTask_vtblSensorGetDescription(ISensor_t *_this)
{

  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));
  return *p_if_owner->sensor_descriptor;
}

SensorStatus_t MP23DB01HPTask_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  MP23DB01HPTask *p_if_owner = (MP23DB01HPTask *)((uint32_t) _this - offsetof(MP23DB01HPTask, sensor_if));

  return p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************

static sys_error_code_t MP23DB01HPTaskExecuteStepRun(AManagedTask *_this)
{
  assert_param(_this);
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
          case SENSOR_CMD_ID_SET_ODR:
            res = MP23DB01HPTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = MP23DB01HPTaskSensorSetFS(p_obj, report);
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

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("MP23DB01HP: unexpected report in Run: %i\r\n", report.messageID));
            SYS_DEBUGF3(SYS_DBG_APP, SYS_DBG_LEVEL_WARNING, ("MP23DB01HP: unexpected report in Run: %i\r\n", report.messageID));
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
        SYS_DEBUGF3(SYS_DBG_APP, SYS_DBG_LEVEL_WARNING, ("MP23DB01HP: unexpected report in Run: %i\r\n", report.messageID));
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
//          SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("MP23DB01HP: new data.\r\n"));

        p_obj->half = report.sensorDataReadyMessage.half;

#if (HSD_USE_DUMMY_DATA == 1)
        MP23DB01HPTaskWriteDummyData(p_obj);
#endif
        // notify the listeners...
        double timestamp = report.sensorDataReadyMessage.fTimestamp;
        double delta_timestamp = timestamp - p_obj->prev_timestamp;
        p_obj->prev_timestamp = timestamp;

        /* update measuredODR */
        p_obj->sensor_status.MeasuredODR = (float)((p_obj->sensor_status.ODR / 1000.0f)) / (float)delta_timestamp;
        uint16_t samples = (uint16_t)(p_obj->sensor_status.ODR / 1000u);

        EMD_1dInit(&p_obj->data, (uint8_t *) &p_obj->p_sensor_data_buff[(p_obj->half - 1) * samples], E_EM_INT16, samples);

        DataEvent_t evt;

        DataEventInit((IEvent *)&evt, p_obj->p_event_src, &p_obj->data, timestamp, p_obj->mic_id);
        IEventSrcSendEvent(p_obj->p_event_src, (IEvent *) &evt, NULL);

        /*SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("MP23DB01HP: ts = %f\r\n", (float)timestamp));*/
        break;
      }
      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
              res = MDFDrvSetDataBuffer((MDFDriver_t*) p_obj->p_driver, p_obj->p_sensor_data_buff, (p_obj->sensor_status.ODR / 1000) * 2);
              if(!SYS_IS_ERROR_CODE(res))
              {
                if(p_obj->sensor_status.IsActive == true)
                {
                  res = IDrvStart(p_obj->p_driver);
                }
              }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = MP23DB01HPTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = MP23DB01HPTaskSensorSetFS(p_obj, report);
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
            SYS_DEBUGF3(SYS_DBG_APP, SYS_DBG_LEVEL_WARNING, ("MP23DB01HP: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("MP23DB01HP: unexpected report in Datalog: %i\r\n", report.messageID));
        SYS_DEBUGF3(SYS_DBG_APP, SYS_DBG_LEVEL_WARNING, ("MP23DB01HP: unexpected report in Datalog: %i\r\n", report.messageID));
      }
    }
  }

  return res;
}

static inline sys_error_code_t MP23DB01HPTaskPostReportToFront(MP23DB01HPTask *_this, SMMessage *pReport)
{
  assert_param(_this);
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
  assert_param(_this);
  int16_t *p16 = &_this->p_sensor_data_buff[(_this->half - 1) * (((uint32_t)_this->sensor_status.ODR / 1000))];
  uint16_t idx = 0;

  for (idx = 0; idx < (((uint32_t)_this->sensor_status.ODR / 1000)); idx++)
  {
    *p16++ = dummyDataCounter++;
  }
}
#endif

static sys_error_code_t MP23DB01HPTaskSensorRegister(MP23DB01HPTask *_this)
{
  assert_param(_this);
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
  _this->sensor_status.IsActive = TRUE;
  _this->sensor_status.FS = 130.0f;
  _this->sensor_status.Sensitivity = 1.0f;
  _this->sensor_status.ODR = 48000.0f;
  _this->sensor_status.MeasuredODR = 0.0f;
  EMD_1dInit(&_this->data, (uint8_t *) _this->p_sensor_data_buff, E_EM_INT16, 1);

  return res;
}

static sys_error_code_t MP23DB01HPTaskSensorSetODR(MP23DB01HPTask *_this, SMMessage report)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MP23DB01HPTask *p_obj = (MP23DB01HPTask *) _this;

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
    else
    {
      ODR = 48000.0f;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      MDFSetMDFConfig(p_obj->p_driver, ODR);
      MDFDriverFilterRegisterCallback((MDFDriver_t *) p_obj->p_driver, HAL_MDF_ACQ_HALFCOMPLETE_CB_ID,
                                      MDF_Filter_0_HalfComplete_Callback);
      MDFDriverFilterRegisterCallback((MDFDriver_t *) p_obj->p_driver, HAL_MDF_ACQ_COMPLETE_CB_ID,
                                      MDF_Filter_0_Complete_Callback);

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

static sys_error_code_t MP23DB01HPTaskSensorSetFS(MP23DB01HPTask *_this, SMMessage report)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  float FS = (float) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->mic_id)
  {
    if (FS != 122.5f)
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

static sys_error_code_t MP23DB01HPTaskSensorEnable(MP23DB01HPTask *_this, SMMessage report)
{
  assert_param(_this);
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

static sys_error_code_t MP23DB01HPTaskSensorDisable(MP23DB01HPTask *_this, SMMessage report)
{
  assert_param(_this);
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

static boolean_t MP23DB01HPTaskSensorIsActive(const MP23DB01HPTask *_this)
{
  assert_param(_this);
  return _this->sensor_status.IsActive;
}

void MDF_Filter_0_Complete_Callback(MDF_HandleTypeDef *hmdf)
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

void MDF_Filter_0_HalfComplete_Callback(MDF_HandleTypeDef *hmdf)
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

