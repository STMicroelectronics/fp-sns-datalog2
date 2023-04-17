/**
 ******************************************************************************
 * @file    LIS2MDLTask.c
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

#include "LIS2MDLTask.h"
#include "LIS2MDLTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "lis2mdl_reg.h"
#include <string.h>
#include "services/sysdebug.h"
#include "mx.h"

#ifndef LIS2MDL_TASK_CFG_STACK_DEPTH
#define LIS2MDL_TASK_CFG_STACK_DEPTH        (TX_MINIMUM_STACK*8)
#endif

#ifndef LIS2MDL_TASK_CFG_PRIORITY
#define LIS2MDL_TASK_CFG_PRIORITY           (4)
#endif

#ifndef LIS2MDL_TASK_CFG_IN_QUEUE_LENGTH
#define LIS2MDL_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define LIS2MDL_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef LIS2MDL_TASK_CFG_TIMER_PERIOD_MS
#define LIS2MDL_TASK_CFG_TIMER_PERIOD_MS          500
#endif

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_LIS2MDL, level, message)

#if defined(DEBUG) || defined (SYS_DEBUG)
#define sTaskObj                                  sIIS2MDTaskObj
#endif

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter = 0;
#endif

/**
 *  LIS2MDLTask internal structure.
 */
struct _LIS2MDLTask
{
  /**
   * Base class object.
   */
  AManagedTaskEx super;

  // Task variables should be added here.

  /**
   * IRQ GPIO configuration parameters.
   */
  const MX_GPIOParams_t *pIRQConfig;

  /**
   * SPI CS GPIO configuration parameters.
   */
  const MX_GPIOParams_t *pCSConfig;

  /**
   * Bus IF object used to connect the sensor task to the specific bus.
   */
  ABusIF *p_sensor_bus_if;

  /**
   * Implements the magnetometer ISensor interface.
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
   * Specifies the sensor ID for the magnetometer.
   */
  uint8_t mag_id;

  /**
   * Synchronization object used to send command to the task.
   */
  TX_QUEUE in_queue;

  /**
   * Buffer to store the data read from the sensor FIFO.
   */
  uint8_t p_sensor_data_buff[6];

  /**
   * Specifies the FIFO watermark level (it depends from ODR)
   */
  uint16_t samples_per_it;

  /**
   * ::IEventSrc interface implementation for this class.
   */
  IEventSrc *p_mag_event_src;

  /**
   * Software timer used to generate the read command
   */
  TX_TIMER read_timer;

  /**
   * Timer period used to schedule the read command
   */
  ULONG lis2mdl_task_cfg_timer_period_ms;

  /**
   * Used to update the instantaneous ODR.
   */
  double prev_timestamp;
};

/**
 * Class object declaration
 */
typedef struct _LIS2MDLTaskClass
{
  /**
   * LIS2MDLTask class virtual table.
   */
  AManagedTaskEx_vtbl vtbl;

  /**
   * Magnetometer IF virtual table.
   */
  ISensor_vtbl sensor_if_vtbl;

  /**
   * Specifies sensor capabilities.
   */
  SensorDescriptor_t class_descriptor;

  /**
   * LIS2MDLTask (PM_STATE, ExecuteStepFunc) map.
   */
  pExecuteStepFunc_t p_pm_state2func_map[3];
} LIS2MDLTaskClass_t;

/* Private member function declaration */ // ***********************************
/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t LIS2MDLTaskExecuteStepState1(AManagedTask *_this);

/**
 * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t LIS2MDLTaskExecuteStepDatalog(AManagedTask *_this);

/**
 * Initialize the sensor according to the actual parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t LIS2MDLTaskSensorInit(LIS2MDLTask *_this);

/**
 * Read the data from the sensor.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t LIS2MDLTaskSensorReadData(LIS2MDLTask *_this);

/**
 * Register the sensor with the global DB and initialize the default parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise
 */
static sys_error_code_t LIS2MDLTaskSensorRegister(LIS2MDLTask *_this);

/**
 * Initialize the default parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise
 */
static sys_error_code_t LIS2MDLTaskSensorInitTaskParams(LIS2MDLTask *_this);

/**
 * Private implementation of sensor interface methods for LIS2MDL sensor
 */
static sys_error_code_t LIS2MDLTaskSensorSetODR(LIS2MDLTask *_this, SMMessage report);
static sys_error_code_t LIS2MDLTaskSensorSetFS(LIS2MDLTask *_this, SMMessage report);
static sys_error_code_t LIS2MDLTaskSensorEnable(LIS2MDLTask *_this, SMMessage report);
static sys_error_code_t LIS2MDLTaskSensorDisable(LIS2MDLTask *_this, SMMessage report);

/**
 * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
 * @param _this [IN] specifies a pointer to a task object.
 * @return TRUE if the sensor is active, FALSE otherwise.
 */
static boolean_t LIS2MDLTaskSensorIsActive(const LIS2MDLTask *_this);

static sys_error_code_t LIS2MDLTaskEnterLowPowerMode(const LIS2MDLTask *_this);

static sys_error_code_t LIS2MDLTaskConfigureIrqPin(const LIS2MDLTask *_this, boolean_t LowPower);

/**
 * Callback function called when the software timer expires.
 *
 * @param timer [IN] specifies the handle of the expired timer.
 */
static void LIS2MDLTaskTimerCallbackFunction(ULONG timer);

/**
 * IRQ callback
 */
void LIS2MDLTask_EXTI_Callback(uint16_t Pin);

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
static inline sys_error_code_t LIS2MDLTaskPostReportToFront(LIS2MDLTask *_this, SMMessage *pReport);

/**
 * Private function used to post a report into the back of the task queue.
 * Used to resume the task when the required by the INIT task.
 *
 * @param this [IN] specifies a pointer to the task object.
 * @param pReport [IN] specifies a report to send.
 * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
 */
static inline sys_error_code_t LIS2MDLTaskPostReportToBack(LIS2MDLTask *_this, SMMessage *pReport);


/* Objects instance */
/********************/

/**
 * The only instance of the task object.
 */
static LIS2MDLTask sTaskObj;

/**
 * The class object.
 */
static const LIS2MDLTaskClass_t sTheClass =
{
    /* Class virtual table */
    {
        LIS2MDLTask_vtblHardwareInit,
        LIS2MDLTask_vtblOnCreateTask,
        LIS2MDLTask_vtblDoEnterPowerMode,
        LIS2MDLTask_vtblHandleError,
        LIS2MDLTask_vtblOnEnterTaskControlLoop,
        LIS2MDLTask_vtblForceExecuteStep,
        LIS2MDLTask_vtblOnEnterPowerMode },

    /* class::sensor_if_vtbl virtual table */
    {
        LIS2MDLTask_vtblMagGetId,
        LIS2MDLTask_vtblMagGetEventSourceIF,
        LIS2MDLTask_vtblMagGetDataInfo,
        LIS2MDLTask_vtblMagGetODR,
        LIS2MDLTask_vtblMagGetFS,
        LIS2MDLTask_vtblMagGetSensitivity,
        LIS2MDLTask_vtblSensorSetODR,
        LIS2MDLTask_vtblSensorSetFS,
        LIS2MDLTask_vtblSensorSetFifoWM,
        LIS2MDLTask_vtblSensorEnable,
        LIS2MDLTask_vtblSensorDisable,
        LIS2MDLTask_vtblSensorIsEnabled,
        LIS2MDLTask_vtblSensorGetDescription,
        LIS2MDLTask_vtblSensorGetStatus },

    /* MAGNETOMETER DESCRIPTOR */
    {
        "lis2mdl",
        COM_TYPE_MAG,
        {
            10.0,
            20.0,
            50.0,
            100.0,
            COM_END_OF_LIST_FLOAT, },
        {
            50.0f,
            COM_END_OF_LIST_FLOAT, },
        {
            "x",
            "y",
            "z", },
        "gauss",
        {
            0,
            1000, } },

    /* class (PM_STATE, ExecuteStepFunc) map */
    {
        LIS2MDLTaskExecuteStepState1,
        NULL,
        LIS2MDLTaskExecuteStepDatalog, } };

/* Public API definition */
// *********************
ISourceObservable* LIS2MDLTaskGetMagSensorIF(LIS2MDLTask *_this)
{
  return (ISourceObservable*) &(_this->sensor_if);
}

AManagedTaskEx* LIS2MDLTaskAlloc(const void *pIRQConfig, const void *pCSConfig)
{
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&sTaskObj.super);

  sTaskObj.super.vptr = &sTheClass.vtbl;
  sTaskObj.sensor_if.vptr = &sTheClass.sensor_if_vtbl;
  sTaskObj.sensor_descriptor = &sTheClass.class_descriptor;

  sTaskObj.pIRQConfig = (MX_GPIOParams_t*) pIRQConfig;
  sTaskObj.pCSConfig = (MX_GPIOParams_t*) pCSConfig;

  return (AManagedTaskEx*) &sTaskObj;
}

ABusIF* LIS2MDLTaskGetSensorIF(LIS2MDLTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc* LIS2MDLTaskGetMagEventSrcIF(LIS2MDLTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc*) _this->p_mag_event_src;
}

// AManagedTask virtual functions definition
// *******************************************

sys_error_code_t LIS2MDLTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2MDLTask *p_obj = (LIS2MDLTask*) _this;

  /* Configure CS Pin */
  if(p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t LIS2MDLTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
VOID **pvStackStart,
                                              ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                              ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2MDLTask *p_obj = (LIS2MDLTask*) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) LIS2MDL_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(LIS2MDL_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if(p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if(TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "LIS2MDL_Q", item_size / 4u, p_queue_items_buff, LIS2MDL_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* create the software timer*/
  if(TX_SUCCESS
      != tx_timer_create(&p_obj->read_timer, "LIS2MDL_T", LIS2MDLTaskTimerCallbackFunction, (ULONG)TX_NULL, AMT_MS_TO_TICKS(LIS2MDL_TASK_CFG_TIMER_PERIOD_MS),
                         0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if(p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(LIS2MDL_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
    if(p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(LIS2MDL_ID, LIS2MDL_I2C_ADD, 0);
    if(p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }

  if(SYS_IS_ERROR_CODE(res))
  {
    return res;
  }
  /* Initialize the EventSrc interface */
  p_obj->p_mag_event_src = DataEventSrcAlloc();
  if(p_obj->p_mag_event_src == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  IEventSrcInit(p_obj->p_mag_event_src);

  memset(p_obj->p_sensor_data_buff, 0, sizeof(p_obj->p_sensor_data_buff));
  p_obj->mag_id = 0;
  p_obj->prev_timestamp = 0.0f;
  p_obj->samples_per_it = 0;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "LIS2MDL";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = LIS2MDL_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = LIS2MDL_TASK_CFG_PRIORITY;
  *pPreemptThreshold = LIS2MDL_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = LIS2MDLTaskSensorInitTaskParams(p_obj);
  if(SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = LIS2MDLTaskSensorRegister(p_obj);
  if(SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LIS2MDL: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t LIS2MDLTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode, const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2MDLTask *p_obj = (LIS2MDLTask*) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &p_obj->p_sensor_bus_if->m_xConnector;

  if(NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if(LIS2MDLTaskSensorIsActive(p_obj))
    {
      SMMessage report =
      {
          .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
          .sensorMessage.nCmdID = SENSOR_CMD_ID_INIT };

      if(tx_queue_send(&p_obj->in_queue, &report, AMT_MS_TO_TICKS(100)) != TX_SUCCESS)
      {
        res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE);
      }

      // reset the variables for the time stamp computation.
      p_obj->prev_timestamp = 0.0f;
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LIS2MDL: -> SENSORS_ACTIVE\r\n"));
  }
  else if(NewPowerMode == E_POWER_MODE_STATE1)
  {
    if(ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      /* SM_SENSOR_STATE_SUSPENDED */
      lis2mdl_power_mode_set(p_sensor_drv, LIS2MDL_HIGH_RESOLUTION); //TODO: SO: Disable the low power
      tx_queue_flush(&p_obj->in_queue);
      if(p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_timer);
      }
      else
      {
        LIS2MDLTaskConfigureIrqPin(p_obj, TRUE);
      }
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LIS2MDL: -> STATE1\r\n"));
  }
  else if(NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = LIS2MDLTaskEnterLowPowerMode(p_obj);
    if(SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LIS2MDL - Enter Low Power Mode failed.\r\n"));
    }
    if(p_obj->pIRQConfig != NULL)
    {
      LIS2MDLTaskConfigureIrqPin(p_obj, TRUE);
    }
    /* notify the bus */
    if(p_obj->p_sensor_bus_if->m_pfBusCtrl != NULL)
    {
      p_obj->p_sensor_bus_if->m_pfBusCtrl(p_obj->p_sensor_bus_if, E_BUS_CTRL_DEV_NOTIFY_POWER_MODE, 0);
    }
    if(p_obj->pIRQConfig == NULL)
    {
      tx_timer_deactivate(&p_obj->read_timer);
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LIS2MDL: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t LIS2MDLTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //LIS2MDLTask *p_obj = (LIS2MDLTask*)_this;

  return res;
}

sys_error_code_t LIS2MDLTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LIS2MDL: start.\r\n"));

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t LIS2MDLTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2MDLTask *p_obj = (LIS2MDLTask*) _this;

  SMMessage report =
  {
      .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
      .internalMessageFE.nData = 0 };

  if((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if(AMTExIsTaskInactive(_this))
    {
      res = LIS2MDLTaskPostReportToFront(p_obj, (SMMessage*) &report);
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

sys_error_code_t LIS2MDLTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode, const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  LIS2MDLTask *p_obj = (LIS2MDLTask*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t LIS2MDLTask_vtblMagGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LIS2MDLTask *p_if_owner = (LIS2MDLTask*) ((uint32_t) _this - offsetof(LIS2MDLTask, sensor_if));
  uint8_t res = p_if_owner->mag_id;

  return res;
}

IEventSrc* LIS2MDLTask_vtblMagGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LIS2MDLTask *p_if_owner = (LIS2MDLTask*) ((uint32_t) _this - offsetof(LIS2MDLTask, sensor_if));

  return p_if_owner->p_mag_event_src;
}

sys_error_code_t LIS2MDLTask_vtblMagGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LIS2MDLTask *p_if_owner = (LIS2MDLTask*) ((uint32_t) _this - offsetof(LIS2MDLTask, sensor_if));
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

float LIS2MDLTask_vtblMagGetFS(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LIS2MDLTask *p_if_owner = (LIS2MDLTask*) ((uint32_t) _this - offsetof(LIS2MDLTask, sensor_if));
  float res = p_if_owner->sensor_status.FS;

  return res;
}

float LIS2MDLTask_vtblMagGetSensitivity(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LIS2MDLTask *p_if_owner = (LIS2MDLTask*) ((uint32_t) _this - offsetof(LIS2MDLTask, sensor_if));
  float res = p_if_owner->sensor_status.Sensitivity;

  return res;
}

EMData_t LIS2MDLTask_vtblMagGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LIS2MDLTask *p_if_owner = (LIS2MDLTask*) ((uint32_t) _this - offsetof(LIS2MDLTask, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t LIS2MDLTask_vtblSensorSetODR(ISensor_t *_this, float ODR)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2MDLTask *p_if_owner = (LIS2MDLTask*) ((uint32_t) _this - offsetof(LIS2MDLTask, sensor_if));

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
    res = LIS2MDLTaskPostReportToBack(p_if_owner, (SMMessage*) &report);
  }

  return res;
}

sys_error_code_t LIS2MDLTask_vtblSensorSetFS(ISensor_t *_this, float FS)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2MDLTask *p_if_owner = (LIS2MDLTask*) ((uint32_t) _this - offsetof(LIS2MDLTask, sensor_if));

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
    res = LIS2MDLTaskPostReportToBack(p_if_owner, (SMMessage*) &report);
  }

  return res;

}

sys_error_code_t LIS2MDLTask_vtblSensorSetFifoWM(ISensor_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  /* Does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LIS2MDL: warning - SetFifoWM() not supported.\r\n"));
  return SYS_INVALID_FUNC_CALL_ERROR_CODE;
}

sys_error_code_t LIS2MDLTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2MDLTask *p_if_owner = (LIS2MDLTask*) ((uint32_t) _this - offsetof(LIS2MDLTask, sensor_if));

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
    res = LIS2MDLTaskPostReportToBack(p_if_owner, (SMMessage*) &report);
  }

  return res;
}

sys_error_code_t LIS2MDLTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2MDLTask *p_if_owner = (LIS2MDLTask*) ((uint32_t) _this - offsetof(LIS2MDLTask, sensor_if));

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
    res = LIS2MDLTaskPostReportToBack(p_if_owner, (SMMessage*) &report);
  }

  return res;
}

boolean_t LIS2MDLTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  LIS2MDLTask *p_if_owner = (LIS2MDLTask*) ((uint32_t) _this - offsetof(LIS2MDLTask, sensor_if));

  if(ISourceGetId((ISourceObservable*) _this) == p_if_owner->mag_id)
  {
    res = p_if_owner->sensor_status.IsActive;
  }

  return res;
}

SensorDescriptor_t LIS2MDLTask_vtblSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LIS2MDLTask *p_if_owner = (LIS2MDLTask*) ((uint32_t) _this - offsetof(LIS2MDLTask, sensor_if));

  return *p_if_owner->sensor_descriptor;
}

SensorStatus_t LIS2MDLTask_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LIS2MDLTask *p_if_owner = (LIS2MDLTask*) ((uint32_t) _this - offsetof(LIS2MDLTask, sensor_if));

  return p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t LIS2MDLTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2MDLTask *p_obj = (LIS2MDLTask*) _this;
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
      case SM_MESSAGE_ID_SENSOR_CMD:
        {
          switch(report.sensorMessage.nCmdID)
          {
            case SENSOR_CMD_ID_SET_ODR:
              res = LIS2MDLTaskSensorSetODR(p_obj, report);
              break;
            case SENSOR_CMD_ID_SET_FS:
              res = LIS2MDLTaskSensorSetFS(p_obj, report);
              break;
            case SENSOR_CMD_ID_ENABLE:
              res = LIS2MDLTaskSensorEnable(p_obj, report);
              break;
            case SENSOR_CMD_ID_DISABLE:
              res = LIS2MDLTaskSensorDisable(p_obj, report);
              break;
            default:
              /* unwanted report */
              res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
              SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
              ;

              SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LIS2MDL: unexpected report in Run: %i\r\n", report.messageID));
              break;
          }
          break;
        }
      default:
        {
          /* unwanted report */
          res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
          SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

          SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LIS2MDL: unexpected report in Run: %i\r\n", report.messageID));
          break;
        }
    }
  }

  return res;
}

static sys_error_code_t LIS2MDLTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2MDLTask *p_obj = (LIS2MDLTask*) _this;
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
          SYS_DEBUGF(SYS_DBG_LEVEL_ALL,("LIS2MDL: new data.\r\n"));
          if(p_obj->pIRQConfig == NULL)
          {
            if(TX_SUCCESS
                != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->lis2mdl_task_cfg_timer_period_ms),
                                   AMT_MS_TO_TICKS(p_obj->lis2mdl_task_cfg_timer_period_ms)))
            {
              return SYS_UNDEFINED_ERROR_CODE;
            }
          }

          res = LIS2MDLTaskSensorReadData(p_obj);
          if(!SYS_IS_ERROR_CODE(res))
          {
            // notify the listeners...
            double timestamp = report.sensorDataReadyMessage.fTimestamp;
            double delta_timestamp = timestamp - p_obj->prev_timestamp;
            p_obj->prev_timestamp = timestamp;

            /* update measuredODR */
            p_obj->sensor_status.MeasuredODR = (float) p_obj->samples_per_it / (float) delta_timestamp;

            /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue (samples_per_it):
             * [X0, Y0, Z0]
             * [X1, Y1, Z1]
             * ...
             * [Xm-1, Ym-1, Zm-1]
             */
            EMD_Init(&p_obj->data, p_obj->p_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, p_obj->samples_per_it, 3);

            DataEvent_t evt;

            DataEventInit((IEvent*) &evt, p_obj->p_mag_event_src, &p_obj->data, timestamp, p_obj->mag_id);
            IEventSrcSendEvent(p_obj->p_mag_event_src, (IEvent*) &evt, NULL);

          SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("LIS2MDL: ts = %f\r\n", (float)timestamp));
          }
            if(p_obj->pIRQConfig == NULL)
            {
              if(TX_SUCCESS != tx_timer_activate(&p_obj->read_timer))
              {
                res = SYS_UNDEFINED_ERROR_CODE;
              }
            }
          break;
        }
      case SM_MESSAGE_ID_SENSOR_CMD:
        {
          switch(report.sensorMessage.nCmdID)
          {
            case SENSOR_CMD_ID_INIT:
              res = LIS2MDLTaskSensorInit(p_obj);
              if(!SYS_IS_ERROR_CODE(res))
              {
                if(p_obj->sensor_status.IsActive == true)
                {
                  if(p_obj->pIRQConfig == NULL)
                  {
                    if(TX_SUCCESS != tx_timer_activate(&p_obj->read_timer))
                    {
                      res = SYS_UNDEFINED_ERROR_CODE;
                    }
                  }
                  else
                  {
                    LIS2MDLTaskConfigureIrqPin(p_obj, FALSE);
                  }
                }
              }
              break;
            case SENSOR_CMD_ID_SET_ODR:
              res = LIS2MDLTaskSensorSetODR(p_obj, report);
              break;
            case SENSOR_CMD_ID_SET_FS:
              res = LIS2MDLTaskSensorSetFS(p_obj, report);
              break;
            case SENSOR_CMD_ID_ENABLE:
              res = LIS2MDLTaskSensorEnable(p_obj, report);
              break;
            case SENSOR_CMD_ID_DISABLE:
              res = LIS2MDLTaskSensorDisable(p_obj, report);
              break;
            default:
              /* unwanted report */
              res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
              SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
              ;

              SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LIS2MDL: unexpected report in Datalog: %i\r\n", report.messageID));
              break;
          }
          break;
        }
      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LIS2MDL: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t LIS2MDLTaskPostReportToFront(LIS2MDLTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t LIS2MDLTaskPostReportToBack(LIS2MDLTask *_this, SMMessage *pReport)
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

static sys_error_code_t LIS2MDLTaskSensorInit(LIS2MDLTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t reg0 = 0;
  int32_t ret_val = 0;
  lis2mdl_odr_t lis2mdl_odr = LIS2MDL_ODR_10Hz;

  /* Read the output registers to reset the interrupt pin */
  /* Without these instructions, INT PIN remains HIGH and */
  /* it never switches anymore */
  lis2mdl_magnetic_raw_get(p_sensor_drv, (int16_t*) &_this->p_sensor_data_buff);

  lis2mdl_reset_set(p_sensor_drv, 1);
  lis2mdl_block_data_update_set(p_sensor_drv, 1);
  lis2mdl_self_test_set(p_sensor_drv, 0);

  ret_val = lis2mdl_device_id_get(p_sensor_drv, (uint8_t*) &reg0);
  if(!ret_val)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, reg0);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LIS2MDL: sensor - I am 0x%x.\r\n", reg0));

  /* Enable Interrupt */
  if(_this->pIRQConfig != NULL)
  {
    lis2mdl_drdy_on_pin_set(p_sensor_drv, 1);
  }
  else
  {
    lis2mdl_drdy_on_pin_set(p_sensor_drv, 0);
  }

  _this->samples_per_it = 1;

  /*Read first data to start it generation */
  int16_t buff[] =
  {
      0,
      0,
      0 };
  lis2mdl_mag_user_offset_set(p_sensor_drv, buff);
  lis2mdl_operating_mode_set(p_sensor_drv, LIS2MDL_CONTINUOUS_MODE);

  if(_this->sensor_status.ODR < 11.0f)
  {
    lis2mdl_odr = LIS2MDL_ODR_10Hz;
  }
  else if(_this->sensor_status.ODR < 21.0f)
  {
    lis2mdl_odr = LIS2MDL_ODR_20Hz;
  }
  else if(_this->sensor_status.ODR < 51.0f)
  {
    lis2mdl_odr = LIS2MDL_ODR_50Hz;
  }
  else if(_this->sensor_status.ODR < 101.0f)
  {
    lis2mdl_odr = LIS2MDL_ODR_100Hz;
  }
  _this->lis2mdl_task_cfg_timer_period_ms = (uint16_t) (1000.0f / _this->sensor_status.ODR);

  if(_this->sensor_status.IsActive)
  {
    lis2mdl_data_rate_set(p_sensor_drv, lis2mdl_odr);
  }
  else
  {
    lis2mdl_power_mode_set(p_sensor_drv, LIS2MDL_HIGH_RESOLUTION);
    lis2mdl_operating_mode_set(p_sensor_drv, LIS2MDL_POWER_DOWN);
    _this->sensor_status.IsActive = false;
  }

  return res;
}

static sys_error_code_t LIS2MDLTaskSensorReadData(LIS2MDLTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &_this->p_sensor_bus_if->m_xConnector;

  lis2mdl_magnetic_raw_get(p_sensor_drv, (int16_t*) &_this->p_sensor_data_buff);

#if (HSD_USE_DUMMY_DATA == 1)
    uint16_t i;
    int16_t * p16 = (int16_t *)_this->p_sensor_data_buff;

    for (i = 0; i < _this->samples_per_it; i++)
    {
      *p16++ = dummyDataCounter++;
      *p16++ = dummyDataCounter++;
      *p16++ = dummyDataCounter++;
    }
#endif
  //TODO: Other things to add?? --> Add a callback for the ready data?
  // LIS2MDL_Data_Ready(0, (uint8_t *)lis2mdl_mem, 6, TimeStamp_lis2mdl);

  return res;
}

static sys_error_code_t LIS2MDLTaskSensorRegister(LIS2MDLTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *mag_if = (ISensor_t*) LIS2MDLTaskGetMagSensorIF(_this);
  _this->mag_id = SMAddSensor(mag_if);

  return res;
}

static sys_error_code_t LIS2MDLTaskSensorInitTaskParams(LIS2MDLTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* MAGNETOMETER SENSOR STATUS */
  _this->sensor_status.IsActive = TRUE;
  _this->sensor_status.FS = 50.0f;
  _this->sensor_status.Sensitivity = 0.0015f;
  _this->sensor_status.ODR = 100.0f;
  _this->sensor_status.MeasuredODR = 0.0f;
  EMD_Init(&_this->data, _this->p_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);

  return res;
}

static sys_error_code_t LIS2MDLTaskSensorSetODR(LIS2MDLTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &_this->p_sensor_bus_if->m_xConnector;
  float ODR = (float) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if(id == _this->mag_id)
  {
    if(ODR < 1.0f)
    {
      lis2mdl_power_mode_set(p_sensor_drv, LIS2MDL_HIGH_RESOLUTION);
      lis2mdl_operating_mode_set(p_sensor_drv, LIS2MDL_POWER_DOWN);
      /* Do not update the model in case of ODR = 0 */
      ODR = _this->sensor_status.ODR;
    }
    else if(ODR < 11.0f)
    {
      ODR = 10.0f;
    }
    else if(ODR < 21.0f)
    {
      ODR = 20.0f;
    }
    else if(ODR < 51.0f)
    {
      ODR = 50.0f;
    }
    else if(ODR < 101.0f)
    {
      ODR = 100.0f;
    }
    else
      res = SYS_INVALID_PARAMETER_ERROR_CODE;

    if(!SYS_IS_ERROR_CODE(res))
    {
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

static sys_error_code_t LIS2MDLTaskSensorSetFS(LIS2MDLTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  float FS = (float) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if(id == _this->mag_id)
  {
    if(FS != 50.0f)
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }

    if(!SYS_IS_ERROR_CODE(res))
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

static sys_error_code_t LIS2MDLTaskSensorEnable(LIS2MDLTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if(id == _this->mag_id)
  {
    _this->sensor_status.IsActive = TRUE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t LIS2MDLTaskSensorDisable(LIS2MDLTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t id = report.sensorMessage.nSensorId;

  if(id == _this->mag_id)
  {
    _this->sensor_status.IsActive = FALSE;
    lis2mdl_power_mode_set(p_sensor_drv, LIS2MDL_HIGH_RESOLUTION);
    lis2mdl_operating_mode_set(p_sensor_drv, LIS2MDL_POWER_DOWN);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t LIS2MDLTaskSensorIsActive(const LIS2MDLTask *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.IsActive;
}

static sys_error_code_t LIS2MDLTaskEnterLowPowerMode(const LIS2MDLTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &_this->p_sensor_bus_if->m_xConnector;

  if(lis2mdl_power_mode_set(p_sensor_drv, LIS2MDL_LOW_POWER))
  {
    res = SYS_SENSOR_TASK_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_OP_ERROR_CODE);
  }

  return res;
}

static sys_error_code_t LIS2MDLTaskConfigureIrqPin(const LIS2MDLTask *_this, boolean_t LowPower)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if(!LowPower)
  {
    /* Configure INT Pin */
    _this->pIRQConfig->p_mx_init_f();
  }
  else
  {
    GPIO_InitTypeDef GPIO_InitStruct =
    {
        0 };

    // first disable the IRQ to avoid spurious interrupt to wake the MCU up.
    HAL_NVIC_DisableIRQ(_this->pIRQConfig->irq_n);
    HAL_NVIC_ClearPendingIRQ(_this->pIRQConfig->irq_n);
    // then reconfigure the PIN in analog high impedance to reduce the power consumption.
    GPIO_InitStruct.Pin = _this->pIRQConfig->pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(_this->pIRQConfig->port, &GPIO_InitStruct);
  }

  return res;
}

static void LIS2MDLTaskTimerCallbackFunction(ULONG timer)
{
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  // if (sTaskObj.in_queue != NULL ) {//TODO: STF.Port - how to check if the queue has been initialized ??
  if(TX_SUCCESS != tx_queue_send(&sTaskObj.in_queue, &report, TX_NO_WAIT))
  {
    // unable to send the message. Signal the error
    sys_error_handler();
  }
  //}
}

/* CubeMX integration */
// ******************
void LIS2MDLTask_EXTI_Callback(uint16_t Pin)
{
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

//  if (sTaskObj.in_queue != NULL) { //TODO: STF.Port - how to check if the queue has been initialized ??
  if(TX_SUCCESS != tx_queue_send(&sTaskObj.in_queue, &report, TX_NO_WAIT))
  {
    /* unable to send the report. Signal the error */
    sys_error_handler();
  }
//  }
}
