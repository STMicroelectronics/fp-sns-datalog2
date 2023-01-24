/**
 ******************************************************************************
 * @file    FAKESENSORTask.c
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

#include "FAKESENSORTask.h"
#include "FAKESENSORTask_vtbl.h"

#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "SMMessageParser.h"
#include "services/sysdebug.h"
#include <string.h>

#ifndef FAKESENSOR_TASK_CFG_STACK_DEPTH
#define FAKESENSOR_TASK_CFG_STACK_DEPTH           (TX_MINIMUM_STACK*2)
#endif

#ifndef FAKESENSOR_TASK_CFG_PRIORITY
#define FAKESENSOR_TASK_CFG_PRIORITY              (TX_MAX_PRIORITIES - 1)
#endif

#ifndef FAKESENSOR_TASK_CFG_IN_QUEUE_LENGTH
#define FAKESENSOR_TASK_CFG_IN_QUEUE_LENGTH      20
#endif

#define FAKESENSOR_DATA_BUFFER_DIM  32u

#define FAKE_TIMER_CFG_LP_TIMER_PERIOD_MS     5u

#define FAKESENSOR_TASK_CFG_IN_QUEUE_ITEM_SIZE   sizeof(SMMessage)

#define MAX_AMIC_SAMPLING_FREQUENCY             (uint32_t)(192000)

#define SYS_DEBUGF(level, message)               SYS_DEBUGF3(SYS_DBG_IMP23ABSU, level, message)

#if defined(DEBUG) || defined (SYS_DEBUG)
#define sTaskObj                                 sFAKESENSORTaskObj
#endif

#define MODEL_OUTPUT_THR  1000u
#define MODEL_NUM_OF_CLASS 4u

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static int16_t dummyDataCounter = 0;
#endif

/**
 *  FAKESENSORTask internal structure.
 */
struct _FAKESENSORTask
{
  /**
   * Base class object.
   */
  AManagedTaskEx super;

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
   * Fake Software timer for publishing data sensor
   */
  TX_TIMER p_fake_data_ready_timer;

  uint32_t model_output_cnt;

  uint32_t model_output_class;

  /**
   * ::IEventSrc interface implementation for this class.
   */
  IEventSrc *p_event_src;

  /**
   * Buffer to store the data read from the sensor
   */
  int16_t p_sensor_data_buff[FAKESENSOR_DATA_BUFFER_DIM];

  /**
   * Used to update the instantaneous ODR.
   */
  double prev_timestamp;

  uint8_t half;

};

/**
 * Class object declaration
 */
typedef struct _FAKESENSORTaskClass
{
  /**
   * FAKESENSORTask class virtual table.
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
   * FAKESENSORTask (PM_STATE, ExecuteStepFunc) map.
   */
  pExecuteStepFunc_t p_pm_state2func_map[3];
} FAKESENSORTaskClass_t;


/* Private member function declaration */ // ***********************************
/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t FAKESENSORTaskExecuteStepRun(AManagedTask *_this);

/**
 * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t FAKESENSORTaskExecuteStepDatalog(AManagedTask *_this);

#if (HSD_USE_DUMMY_DATA == 1)
/**
  * Read dummy data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static void FAKESENSORTaskWriteDummyData(FAKESENSORTask *_this);
#endif

/**
 * Register the sensor with the global DB and initialize the default parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise
 */
static sys_error_code_t FAKESENSORTaskSensorRegister(FAKESENSORTask *_this);

/**
 * Initialize the default parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise
 */
static sys_error_code_t FAKESENSORTaskSensorInitTaskParams(FAKESENSORTask *_this);


/**
 * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
 * @param _this [IN] specifies a pointer to a task object.
 * @return TRUE if the sensor is active, FALSE otherwise.
 */
static boolean_t FAKESENSORTaskSensorIsActive(const FAKESENSORTask *_this);

/* Inline function forward declaration */
// ***********************************
/**
 * Private function used to post a report into the front of the task queue.
 * Used to resume the task when the required by the INIT task.
 *
 * @param this [IN] specifies a pointer to the task object.
 * @param pReport [IN] specifies a report to send.
 * @return SYS_NO_EROR_CODE if success, SYS_APP_TASK_MSG_LOST_ERROR_CODE.
 */
static inline sys_error_code_t FAKESENSORTaskPostReportToFront(FAKESENSORTask *_this, SMMessage *pReport);

/**
 * Private function used to post a report into the back of the task queue.
 * Used to resume the task when the required by the INIT task.
 *
 * @param this [IN] specifies a pointer to the task object.
 * @param pReport [IN] specifies a report to send.
 * @return SYS_NO_EROR_CODE if success, SYS_APP_TASK_MSG_LOST_ERROR_CODE.
 */
static inline sys_error_code_t FAKESENSORTaskPostReportToBack(FAKESENSORTask *_this, SMMessage *pReport);

static void fake_sensor_timer_cb(ULONG param);

#if defined (__GNUC__)
// Inline function defined inline in the header file FAKESENSORTask.h must be declared here as extern function.
#endif

/* Objects instance */
/********************/

bool anomaly_present;

/**
 * The only instance of the task object.
 */
static FAKESENSORTask sTaskObj;

/**
 * The class object.
 */
static const FAKESENSORTaskClass_t sTheClass =
{
    /* Class virtual table */
    {
        FAKESENSORTask_vtblHardwareInit,
        FAKESENSORTask_vtblOnCreateTask,
        FAKESENSORTask_vtblDoEnterPowerMode,
        FAKESENSORTask_vtblHandleError,
        FAKESENSORTask_vtblOnEnterTaskControlLoop,
        FAKESENSORTask_vtblForceExecuteStep,
        FAKESENSORTask_vtblOnEnterPowerMode },

    /* class::sensor_if_vtbl virtual table */
    {
        FAKESENSORTask_vtblMicGetId,
        FAKESENSORTask_vtblGetEventSourceIF,
        FAKESENSORTask_vtblMicGetDataInfo,
        FAKESENSORTask_vtblMicGetODR,
        FAKESENSORTask_vtblMicGetFS,
        FAKESENSORTask_vtblMicGetSensitivity,
        FAKESENSORTask_vtblSensorSetODR,
        FAKESENSORTask_vtblSensorSetFS,
        NULL,
        FAKESENSORTask_vtblSensorEnable,
        FAKESENSORTask_vtblSensorDisable,
        FAKESENSORTask_vtblSensorIsEnabled,
        FAKESENSORTask_vtblSensorGetDescription,
        FAKESENSORTask_vtblSensorGetStatus },

    /* MIC DESCRIPTOR */
    {
        "FAKESENSOR",
        COM_TYPE_MIC,
        {
            16000.0,
            32000.0,
            48000.0,
            96000.0,
            192000.0,
            COM_END_OF_LIST_FLOAT, },
        {
            130.0,
            COM_END_OF_LIST_FLOAT, },
        {
            "aud", },
        "Waveform",
        {
            0,
            1000, } },

    /* class (PM_STATE, ExecuteStepFunc) map */
    {
        FAKESENSORTaskExecuteStepRun,
        NULL,
        FAKESENSORTaskExecuteStepDatalog, } };

/* Public API definition */
// *********************
ISourceObservable* FAKESENSORTaskGetMicSensorIF(FAKESENSORTask *_this)
{
  return (ISourceObservable*) &(_this->sensor_if);
}

AManagedTaskEx* FAKESENSORTaskAlloc(const void *pParams)
{
  /* This allocator implements the singleton design pattern. */

  /* Initialize the super class */
  AMTInitEx(&sTaskObj.super);

  sTaskObj.super.vptr = &sTheClass.vtbl;
  sTaskObj.sensor_if.vptr = &sTheClass.sensor_if_vtbl;
  sTaskObj.sensor_descriptor = &sTheClass.class_descriptor;

  return (AManagedTaskEx*) &sTaskObj;
}

IEventSrc* FAKESENSORTaskGetEventSrcIF(FAKESENSORTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_event_src;
}

// AManagedTask virtual functions definition
// *****************************************

sys_error_code_t FAKESENSORTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  FAKESENSORTask *p_obj = (FAKESENSORTask*) _this;

  if(p_obj == NULL)
  {
  }

  return res;
}

sys_error_code_t FAKESENSORTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
VOID **pvStackStart,
                                                ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                                ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  FAKESENSORTask *p_obj = (FAKESENSORTask*) _this;

  *pTaskCode = AMTExRun;
  *pName = "FAKESENSOR";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = FAKESENSOR_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = FAKESENSOR_TASK_CFG_PRIORITY;
  *pPreemptThreshold = FAKESENSOR_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  /* Create task specific sw resources. */
  uint32_t item_size = (uint32_t) FAKESENSOR_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(FAKESENSOR_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if(p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if(TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "FAKESENSOR_Q", item_size / 4u, p_queue_items_buff, FAKESENSOR_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  p_obj->p_event_src = (IEventSrc*) DataEventSrcAlloc();
  if(p_obj->p_event_src == NULL)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }
  IEventSrcInit(p_obj->p_event_src);

  memset(p_obj->p_sensor_data_buff, 0, sizeof(p_obj->p_sensor_data_buff));
  p_obj->mic_id = 1;
  p_obj->prev_timestamp = 0.0f;
  p_obj->half = 0;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  res = FAKESENSORTaskSensorInitTaskParams(p_obj);
  if(SYS_IS_ERROR_CODE(res))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }

  /*
   * Initialize software timer for publishing data
   */
   tx_timer_create(&p_obj->p_fake_data_ready_timer,
		   	   	       "fakeSensor_timer",
		   	   	       fake_sensor_timer_cb,
				           0u,
				           1u,
				           AMT_MS_TO_TICKS(FAKE_TIMER_CFG_LP_TIMER_PERIOD_MS),
				           TX_NO_ACTIVATE );

  /*res = FAKESENSORTaskSensorRegister(p_obj);*/
  if(SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IMP23ABSU: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t FAKESENSORTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode, const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  FAKESENSORTask *p_obj = (FAKESENSORTask*) _this;

  if(FAKESENSORTaskSensorIsActive(p_obj))
  {

  }

  if(NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if(FAKESENSORTaskSensorIsActive(p_obj))
    {
      SMMessage xReport =
      {
          .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
          .sensorMessage.nCmdID = SENSOR_CMD_ID_INIT };

      if(tx_queue_send(&p_obj->in_queue, &xReport, AMT_MS_TO_TICKS(50)) != TX_SUCCESS)
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
  else if(NewPowerMode == E_POWER_MODE_STATE1)
  {
    if(ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      tx_timer_deactivate(&p_obj->p_fake_data_ready_timer);
      tx_queue_flush(&p_obj->in_queue);
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IMP23ABSU: -> STATE1\r\n"));
  }
  else if(NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power (this code is executed from the INIT task).*/

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IMP23ABSU: -> SLEEP_1\r\n"));
  }
  return res;
}

sys_error_code_t FAKESENSORTask_vtblHandleError(AManagedTask *_this, SysEvent Error)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  FAKESENSORTask *p_obj = (FAKESENSORTask*)_this;

  return res;
}

sys_error_code_t FAKESENSORTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IMP23ABSU: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (IMP23ABSU_TASK_CFG_TAG)
  FAKESENSORTask *p_obj = (FAKESENSORTask*) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = IMP23ABSU_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  p_obj->model_output_cnt = 0u;

  return res;
}

/* AManagedTaskEx virtual functions definition */
// *******************************************
sys_error_code_t FAKESENSORTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  FAKESENSORTask *p_obj = (FAKESENSORTask*) _this;

  SMMessage xReport =
  {
      .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
      .internalMessageFE.nData = 0 };

  if((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if(AMTExIsTaskInactive(_this))
    {
      res = FAKESENSORTaskPostReportToFront(p_obj, (SMMessage*) &xReport);
    }
  }
  else
  {
    UINT state;
    if(TX_SUCCESS == tx_thread_info_get(&_this->m_xTaskHandle, TX_NULL, &state, TX_NULL, TX_NULL, TX_NULL, TX_NULL,
    TX_NULL,
                                        TX_NULL))
    {
      if(state == TX_SUSPENDED)
      {
        tx_thread_resume(&_this->m_xTaskHandle);
      }
    }
  }
  return res;
}

sys_error_code_t FAKESENSORTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode, const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //FAKESENSORTask *p_obj = (FAKESENSORTask*) _this;

  if(NewPowerMode == E_POWER_MODE_STATE1)
  {
    if(ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {

    }
  }
  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t FAKESENSORTask_vtblMicGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  FAKESENSORTask *p_if_owner = (FAKESENSORTask*) ((uint32_t) _this - offsetof(FAKESENSORTask, sensor_if));
  uint8_t res = p_if_owner->mic_id;

  return res;
}

IEventSrc* FAKESENSORTask_vtblGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  FAKESENSORTask *p_if_owner = (FAKESENSORTask*) ((uint32_t) _this - offsetof(FAKESENSORTask, sensor_if));
  return p_if_owner->p_event_src;
}

sys_error_code_t FAKESENSORTask_vtblMicGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  FAKESENSORTask *p_if_owner = (FAKESENSORTask*) ((uint32_t) _this - offsetof(FAKESENSORTask, sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if((p_measured == NULL) || (p_nominal == NULL))
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

float FAKESENSORTask_vtblMicGetFS(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  FAKESENSORTask *p_if_owner = (FAKESENSORTask*) ((uint32_t) _this - offsetof(FAKESENSORTask, sensor_if));
  float res = p_if_owner->sensor_status.FS;

  return res;
}

float FAKESENSORTask_vtblMicGetSensitivity(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  FAKESENSORTask *p_if_owner = (FAKESENSORTask*) ((uint32_t) _this - offsetof(FAKESENSORTask, sensor_if));
  float res = p_if_owner->sensor_status.Sensitivity;

  return res;
}

EMData_t FAKESENSORTask_vtblMicGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  FAKESENSORTask *p_if_owner = (FAKESENSORTask*) ((uint32_t) _this - offsetof(FAKESENSORTask, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t FAKESENSORTask_vtblSensorSetODR(ISensor_t *_this, float ODR)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  FAKESENSORTask *p_if_owner = (FAKESENSORTask*) ((uint32_t) _this - offsetof(FAKESENSORTask, sensor_if));

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask*) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable*) _this);

  if((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
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
        .sensorMessage.nParam = (uint32_t) ODR };
    res = FAKESENSORTaskPostReportToBack(p_if_owner, (SMMessage*) &report);
  }

  return res;
}

sys_error_code_t FAKESENSORTask_vtblSensorSetFS(ISensor_t *_this, float FS)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  FAKESENSORTask *p_if_owner = (FAKESENSORTask*) ((uint32_t) _this - offsetof(FAKESENSORTask, sensor_if));

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask*) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable*) _this);

  if((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
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
        .sensorMessage.nParam = (uint32_t) FS };
    res = FAKESENSORTaskPostReportToBack(p_if_owner, (SMMessage*) &report);
  }

  return res;
}

sys_error_code_t FAKESENSORTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  FAKESENSORTask *p_if_owner = (FAKESENSORTask*) ((uint32_t) _this - offsetof(FAKESENSORTask, sensor_if));

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask*) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable*) _this);

  if((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
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
        .sensorMessage.nSensorId = sensor_id };
    res = FAKESENSORTaskPostReportToBack(p_if_owner, (SMMessage*) &report);
  }

  return res;
}

sys_error_code_t FAKESENSORTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  FAKESENSORTask *p_if_owner = (FAKESENSORTask*) ((uint32_t) _this - offsetof(FAKESENSORTask, sensor_if));

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask*) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable*) _this);

  if((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
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
        .sensorMessage.nSensorId = sensor_id };
    res = FAKESENSORTaskPostReportToBack(p_if_owner, (SMMessage*) &report);
  }

  return res;
}

boolean_t FAKESENSORTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  FAKESENSORTask *p_if_owner = (FAKESENSORTask*) ((uint32_t) _this - offsetof(FAKESENSORTask, sensor_if));

  if(ISourceGetId((ISourceObservable*) _this) == p_if_owner->mic_id)
  {
    res = p_if_owner->sensor_status.IsActive;
  }

  return res;
}

SensorDescriptor_t FAKESENSORTask_vtblSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  FAKESENSORTask *p_if_owner = (FAKESENSORTask*) ((uint32_t) _this - offsetof(FAKESENSORTask, sensor_if));

  return *p_if_owner->sensor_descriptor;
}

SensorStatus_t FAKESENSORTask_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  FAKESENSORTask *p_if_owner = (FAKESENSORTask*) ((uint32_t) _this - offsetof(FAKESENSORTask, sensor_if));

  return p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t FAKESENSORTaskExecuteStepRun(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  FAKESENSORTask *p_obj = (FAKESENSORTask*) _this;
  SMMessage report =
  {
      0 };

  AMTExSetInactiveState((AManagedTaskEx*) _this, TRUE);
  if(TX_SUCCESS == tx_queue_receive(&p_obj->in_queue, &report, TX_WAIT_FOREVER))
  {
    AMTExSetInactiveState((AManagedTaskEx*) _this, FALSE);


  }

  return res;
}

static sys_error_code_t FAKESENSORTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  FAKESENSORTask *p_obj = (FAKESENSORTask*) _this;
  SMMessage report =
  {
      0 };

  AMTExSetInactiveState((AManagedTaskEx*) _this, TRUE);
  if(TX_SUCCESS == tx_queue_receive(&p_obj->in_queue, &report, TX_WAIT_FOREVER))
  {
    AMTExSetInactiveState((AManagedTaskEx*) _this, FALSE);

    switch(report.messageID)
    {
      case SM_MESSAGE_ID_FORCE_STEP:
        {
          // do nothing. I need only to resume.
          __NOP();
          break;
        }
      case SM_MESSAGE_ID_DATA_READY:
        {

          p_obj->half = report.sensorDataReadyMessage.half;

#if (HSD_USE_DUMMY_DATA == 1)
            IMP23ABSUTaskWriteDummyData(p_obj);
    #endif
          // notify the listeners...
          double timestamp = report.sensorDataReadyMessage.fTimestamp;
          double delta_timestamp = timestamp - p_obj->prev_timestamp;
          p_obj->prev_timestamp = timestamp;

          p_obj->model_output_cnt++;

          if( 0 == (p_obj->model_output_cnt % MODEL_OUTPUT_THR))
          {
            p_obj->model_output_cnt = 0;

            p_obj->model_output_class++;

            p_obj->model_output_class = p_obj->model_output_class % MODEL_NUM_OF_CLASS;

            if( p_obj->model_output_class <= 1)
            {
              anomaly_present = false;
            }
            else
            {
              anomaly_present = true;
            }

            /* Set first buffer byte as output model */
            p_obj->p_sensor_data_buff[0] =  (uint16_t)p_obj->model_output_class;
          }

          /* update measuredODR */
          p_obj->sensor_status.MeasuredODR = (float) ((p_obj->sensor_status.ODR / 1000.0f)) / (float) delta_timestamp;
          uint16_t samples = FAKESENSOR_DATA_BUFFER_DIM;

          EMD_1dInit(&p_obj->data, (uint8_t*) &p_obj->p_sensor_data_buff[0], E_EM_INT16, samples);

          DataEvent_t evt;

          DataEventInit((IEvent*) &evt, p_obj->p_event_src, &p_obj->data, timestamp, p_obj->mic_id);
          IEventSrcSendEvent(p_obj->p_event_src, (IEvent*) &evt, NULL);

          /*SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IMP23ABSU: ts = %f\r\n", (float)timestamp));*/
          break;
        }
      case SM_MESSAGE_ID_SENSOR_CMD:
        {
          switch(report.sensorMessage.nCmdID)
          {
            case SENSOR_CMD_ID_INIT:
            {
              tx_timer_activate(&p_obj->p_fake_data_ready_timer);
            }
            break;
            case SENSOR_CMD_ID_SET_ODR:

              break;
            case SENSOR_CMD_ID_SET_FS:

              break;
            case SENSOR_CMD_ID_ENABLE:

              break;
            case SENSOR_CMD_ID_DISABLE:

              break;
            default:
              /* unwanted report */
              res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
              SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
              ;

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

static inline sys_error_code_t FAKESENSORTaskPostReportToFront(FAKESENSORTask *_this, SMMessage *pReport)
{
  assert_param(_this != NULL);
  assert_param(pReport);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if(SYS_IS_CALLED_FROM_ISR())
  {
    if(TX_SUCCESS != tx_queue_front_send(&_this->in_queue, pReport, TX_NO_WAIT))
    {
      res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
      // this function is private and the caller will ignore this return code.
    }
  }
  else
  {
    if(TX_SUCCESS != tx_queue_front_send(&_this->in_queue, pReport, AMT_MS_TO_TICKS(100)))
    {
      res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
      // this function is private and the caller will ignore this return code.
    }
  }

  return res;
}

static inline sys_error_code_t FAKESENSORTaskPostReportToBack(FAKESENSORTask *_this, SMMessage *pReport)
{
  assert_param(_this != NULL);
  assert_param(pReport);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if(SYS_IS_CALLED_FROM_ISR())
  {
    if(TX_SUCCESS != tx_queue_send(&_this->in_queue, pReport, TX_NO_WAIT))
    {
      res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
      // this function is private and the caller will ignore this return code.
    }
  }
  else
  {
    if(TX_SUCCESS != tx_queue_send(&_this->in_queue, pReport, AMT_MS_TO_TICKS(100)))
    {
      res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
      // this function is private and the caller will ignore this return code.
    }
  }

  return res;
}

#if (HSD_USE_DUMMY_DATA == 1)
static void FAKESENSORTaskWriteDummyData(FAKESENSORTask *_this)
{
  assert_param(_this != NULL);
  int16_t *p16 = &_this->p_sensor_data_buff[(_this->half - 1) * (((uint32_t)_this->sensor_status.ODR / 1000))];
  uint16_t idx = 0;

  for (idx = 0; idx < (((uint32_t)_this->sensor_status.ODR / 1000)); idx++)
  {
    *p16++ = dummyDataCounter++;
  }
}
#endif

static sys_error_code_t FAKESENSORTaskSensorRegister(FAKESENSORTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *mic_if = (ISensor_t*) FAKESENSORTaskGetMicSensorIF(_this);
  _this->mic_id = SMAddSensor(mic_if);

  return res;
}

static sys_error_code_t FAKESENSORTaskSensorInitTaskParams(FAKESENSORTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  _this->sensor_status.IsActive = TRUE;
  _this->sensor_status.FS = 130.0f;
  _this->sensor_status.Sensitivity = 1.0f;
  _this->sensor_status.ODR = 192000.0f;
  _this->sensor_status.MeasuredODR = 0.0f;
  EMD_1dInit(&_this->data, (uint8_t*) _this->p_sensor_data_buff, E_EM_INT16, 1);

  return res;
}

static boolean_t FAKESENSORTaskSensorIsActive(const FAKESENSORTask *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.IsActive;
}

static void fake_sensor_timer_cb(ULONG param)
{
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.half = 1;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());
  //  if (sTaskObj.in_queue != NULL) { //TODO: STF.Port - how to check if the queue has been initialized ??
  if(TX_SUCCESS != tx_queue_send(&sTaskObj.in_queue, &report, TX_NO_WAIT))
  {
    /* unable to send the report. Signal the error */
    sys_error_handler();
  }

}





