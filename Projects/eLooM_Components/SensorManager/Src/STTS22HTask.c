/**
  ******************************************************************************
  * @file    STTS22HTask.c
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

#include "STTS22HTask.h"
#include "STTS22HTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "stts22h_reg.h"
#include <string.h>
#include "services/sysdebug.h"
#include "mx.h"

#ifndef STTS22H_TASK_CFG_STACK_DEPTH
#define STTS22H_TASK_CFG_STACK_DEPTH        (TX_MINIMUM_STACK*8)
#endif

#ifndef STTS22H_TASK_CFG_PRIORITY
#define STTS22H_TASK_CFG_PRIORITY           4
#endif

#ifndef STTS22H_TASK_CFG_IN_QUEUE_LENGTH
#define STTS22H_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#ifndef STTS22H_TASK_CFG_TIMER_PERIOD_MS
#define STTS22H_TASK_CFG_TIMER_PERIOD_MS          250
#endif

#ifndef STTS22H_TASK_CFG_I2C_ADDRESS
#define STTS22H_TASK_CFG_I2C_ADDRESS              STTS22H_I2C_ADD_L
#endif

#define STTS22H_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_STTS22H, level, message)

#if defined(DEBUG) || defined (SYS_DEBUG)
#define sTaskObj                                  sSTTS22HTaskObj
#endif

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter = 0;
#endif

/**
  *  STTS22HTask internal structure.
  */
struct _STTS22HTask
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
   * I2C Device Address
   */
  uint8_t i2c_addr;

  /**
    * Bus IF object used to connect the sensor task to the specific bus.
    */
  ABusIF *p_sensor_bus_if;

  /**
    * Implements the temperature ISensor interface.
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
    * Specifies the sensor ID for the temperature sensor.
    */
  uint8_t temp_id;

  /**
    * Synchronization object used to send command to the task.
    */
  TX_QUEUE in_queue;

  /**
    * Buffer to store the data read from the sensor FIFO.
    */
  float temperature;

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_temp_event_src;

  /**
    * Used to update the instantaneous ODR.
    */
  double prev_timestamp;

  /**
    * Software timer used to generate the read command
    */
  TX_TIMER read_fifo_timer;

  /**
    * Specifies the ms delay between 2 consecutive read (it depends from ODR)
    */
  uint16_t task_delay;
};

/**
  * Class object declaration
  */
typedef struct _STTS22HTaskClass
{
  /**
    * STTS22HTask class virtual table.
    */
  AManagedTaskEx_vtbl vtbl;

  /**
    * Temperature IF virtual table.
    */
  ISensor_vtbl sensor_if_vtbl;

  /**
    * Specifies temperature sensor capabilities.
    */
  SensorDescriptor_t class_descriptor;

  /**
    * STTS22HTask (PM_STATE, ExecuteStepFunc) map.
    */
  pExecuteStepFunc_t p_pm_state2func_map[3];
} STTS22HTaskClass_t;

/* Private member function declaration */// ***********************************

/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t STTS22HTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t STTS22HTaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t STTS22HTaskSensorInit(STTS22HTask *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t STTS22HTaskSensorReadData(STTS22HTask *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t STTS22HTaskSensorRegister(STTS22HTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t STTS22HTaskSensorInitTaskParams(STTS22HTask *_this);

/**
  * Private implementation of sensor interface methods for STTS22H sensor
  */

static sys_error_code_t STTS22HTaskSensorSetODR(STTS22HTask *_this, SMMessage report);
static sys_error_code_t STTS22HTaskSensorSetFS(STTS22HTask *_this, SMMessage report);
static sys_error_code_t STTS22HTaskSensorEnable(STTS22HTask *_this, SMMessage report);
static sys_error_code_t STTS22HTaskSensorDisable(STTS22HTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t STTS22HTaskSensorIsActive(const STTS22HTask *_this);

static sys_error_code_t STTS22HTaskEnterLowPowerMode(const STTS22HTask *_this);

static sys_error_code_t STTS22HTaskConfigureIrqPin(const STTS22HTask *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param xTimer [IN] specifies the handle of the expired timer.
  */
static void STTS22HTaskTimerCallbackFunction(ULONG timer);

/**
  * IRQ callback
  */
void STT22HTask_EXTI_Callback(uint16_t nPin);

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
static inline sys_error_code_t STTS22HTaskPostReportToFront(STTS22HTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t STTS22HTaskPostReportToBack(STTS22HTask *_this, SMMessage *pReport);


/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
static STTS22HTask sTaskObj;

/**
  * The class object.
  */
static const STTS22HTaskClass_t sTheClass =
{
  /* Class virtual table */
  {
    STTS22HTask_vtblHardwareInit,
    STTS22HTask_vtblOnCreateTask,
    STTS22HTask_vtblDoEnterPowerMode,
    STTS22HTask_vtblHandleError,
    STTS22HTask_vtblOnEnterTaskControlLoop,
    STTS22HTask_vtblForceExecuteStep,
    STTS22HTask_vtblOnEnterPowerMode
  },

  /* class::sensor_if_vtbl virtual table */
  {
    STTS22HTask_vtblTempGetId,
    STTS22HTask_vtblTempGetEventSourceIF,
    STTS22HTask_vtblTempGetDataInfo,
    STTS22HTask_vtblTempGetODR,
    STTS22HTask_vtblTempGetFS,
    STTS22HTask_vtblTempGetSensitivity,
    STTS22HTask_vtblSensorSetODR,
    STTS22HTask_vtblSensorSetFS,
    STTS22HTask_vtblSensorSetFifoWM,
    STTS22HTask_vtblSensorEnable,
    STTS22HTask_vtblSensorDisable,
    STTS22HTask_vtblSensorIsEnabled,
    STTS22HTask_vtblSensorGetDescription,
    STTS22HTask_vtblSensorGetStatus
  },

  /* TEMPERATURE DESCRIPTOR */
  {
    "stts22h",
    COM_TYPE_TEMP,
    {
      1.0f,
      25.0f,
      50.0f,
      100.0f,
      200.0f,
      COM_END_OF_LIST_FLOAT,
    },
    {
      100.0f,
      COM_END_OF_LIST_FLOAT,
    },
    {
      "temp",
    },
    "Celsius",
    {
      0,
      1000,
    }
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    STTS22HTaskExecuteStepState1,
    NULL,
    STTS22HTaskExecuteStepDatalog,
  }
};

/* Public API definition */
// *********************

ISourceObservable *STTS22HTaskGetTempSensorIF(STTS22HTask *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *STTS22HTaskAlloc(const void *pIRQConfig, const void *pCSConfig, const uint8_t i2c_addr)
{
  /* This allocator implements the singleton design pattern. */

  /* Initialize the super class */
  AMTInitEx(&sTaskObj.super);

  sTaskObj.super.vptr = &sTheClass.vtbl;
  sTaskObj.sensor_if.vptr = &sTheClass.sensor_if_vtbl;
  sTaskObj.sensor_descriptor = &sTheClass.class_descriptor;

  sTaskObj.pIRQConfig = (MX_GPIOParams_t *)pIRQConfig;
  sTaskObj.pCSConfig = (MX_GPIOParams_t *)pCSConfig;
  sTaskObj.i2c_addr = i2c_addr;

  return (AManagedTaskEx *) &sTaskObj;
}

ABusIF *STTS22HTaskGetSensorIF(STTS22HTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *STTS22HTaskGetTempEventSrcIF(STTS22HTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_temp_event_src;
}

/* AManagedTaskEx virtual functions definition */
// *******************************************

sys_error_code_t STTS22HTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STTS22HTask *p_obj = (STTS22HTask *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t STTS22HTask_vtblOnCreateTask(
  AManagedTask *_this,
  tx_entry_function_t *pTaskCode,
  CHAR **pName,
  VOID **pvStackStart,
  ULONG *pStackDepth,
  UINT *pPriority,
  UINT *pPreemptThreshold,
  ULONG *pTimeSlice,
  ULONG *pAutoStart,
  ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STTS22HTask *p_obj = (STTS22HTask *) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t)STTS22H_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(STTS22H_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "STTS22H_Q", item_size / 4u, p_queue_items_buff,
                                    STTS22H_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* create the software timer*/
  if (TX_SUCCESS != tx_timer_create(
        &p_obj->read_fifo_timer,
        "STTS22H_T",
        STTS22HTaskTimerCallbackFunction,
        0,
        AMT_MS_TO_TICKS(STTS22H_TASK_CFG_TIMER_PERIOD_MS),
        0,
        TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(STTS22H_ID, p_obj->pCSConfig->port, (uint16_t)p_obj->pCSConfig->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
//    p_obj->p_sensor_bus_if = I2CBusIFAlloc(STTS22H_ID, STTS22H_TASK_CFG_I2C_ADDRESS, 0);
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(STTS22H_ID, p_obj->i2c_addr, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }

  if (SYS_IS_ERROR_CODE(res))
  {
    return res;
  }

  /* Initialize the EventSrc interface, take the ownership of the interface. */
  p_obj->p_temp_event_src = DataEventSrcAlloc();
  if (p_obj->p_temp_event_src == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  IEventSrcInit(p_obj->p_temp_event_src);

  //TODO: I don't have data buffer
  //memset(p_obj->p_sensor_data_buff, 0, sizeof(p_obj->p_sensor_data_buff));
  p_obj->temp_id = 0;
  p_obj->prev_timestamp = 0.0f;
  p_obj->temperature = 0.0f;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "STTS22H";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = STTS22H_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = STTS22H_TASK_CFG_PRIORITY;
  *pPreemptThreshold = STTS22H_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = STTS22HTaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = STTS22HTaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("STTS22H: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t STTS22HTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STTS22HTask *p_obj = (STTS22HTask *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (STTS22HTaskSensorIsActive(p_obj))
    {
      SMMessage report =
      {
        .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
        .sensorMessage.nCmdID = SENSOR_CMD_ID_INIT
      };

      if (tx_queue_send(&p_obj->in_queue, &report, AMT_MS_TO_TICKS(100)) != TX_SUCCESS)
      {
        res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE);
      }

      // reset the variables for the time stamp computation.
      p_obj->prev_timestamp = 0.0f;
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("STTS22H: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      /* Deactivate the sensor */
      stts22h_temp_data_rate_set(p_sensor_drv, STTS22H_POWER_DOWN);

      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_fifo_timer);
      }
      else
      {
        STTS22HTaskConfigureIrqPin(p_obj, TRUE);
      }
    }
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("STTS22H: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = STTS22HTaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      sys_error_handler();
    }
    if (p_obj->pIRQConfig != NULL)
    {
      STTS22HTaskConfigureIrqPin(p_obj, TRUE);
    }
    /* notify the bus */
    if (p_obj->p_sensor_bus_if->m_pfBusCtrl != NULL)
    {
      p_obj->p_sensor_bus_if->m_pfBusCtrl(p_obj->p_sensor_bus_if, E_BUS_CTRL_DEV_NOTIFY_POWER_MODE, 0);
    }
    if (p_obj->pIRQConfig == NULL)
    {
      tx_timer_deactivate(&p_obj->read_fifo_timer);
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("STTS22H: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t STTS22HTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  // STTS22HTask *p_obj = (STTS22HTask*)_this;

  return res;
}

sys_error_code_t STTS22HTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("STTS22H: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (STTS22H_TASK_CFG_TAG)
  STTS22HTask *p_obj = (STTS22HTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = STTS22H_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t STTS22HTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STTS22HTask *p_obj = (STTS22HTask *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = STTS22HTaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t STTS22HTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  STTS22HTask *p_obj = (STTS22HTask*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t STTS22HTask_vtblTempGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  STTS22HTask *p_if_owner = (STTS22HTask *)((uint32_t) _this - offsetof(STTS22HTask, sensor_if));
  uint8_t res = p_if_owner->temp_id;

  return res;
}

IEventSrc *STTS22HTask_vtblTempGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  STTS22HTask *p_if_owner = (STTS22HTask *)((uint32_t) _this - offsetof(STTS22HTask, sensor_if));
  return p_if_owner->p_temp_event_src;
}

sys_error_code_t STTS22HTask_vtblTempGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  STTS22HTask *p_if_owner = (STTS22HTask *)((uint32_t) _this - offsetof(STTS22HTask, sensor_if));
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

float STTS22HTask_vtblTempGetFS(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  STTS22HTask *p_if_owner = (STTS22HTask *)((uint32_t) _this - offsetof(STTS22HTask, sensor_if));
  float res = p_if_owner->sensor_status.FS;

  return res;
}

float STTS22HTask_vtblTempGetSensitivity(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  STTS22HTask *p_if_owner = (STTS22HTask *)((uint32_t) _this - offsetof(STTS22HTask, sensor_if));
  float res = p_if_owner->sensor_status.Sensitivity;

  return res;
}

EMData_t STTS22HTask_vtblTempGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  STTS22HTask *p_if_owner = (STTS22HTask *)((uint32_t)_this - offsetof(STTS22HTask, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t STTS22HTask_vtblSensorSetODR(ISensor_t *_this, float ODR)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STTS22HTask *p_if_owner = (STTS22HTask *)((uint32_t) _this - offsetof(STTS22HTask, sensor_if));
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
    res = STTS22HTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STTS22HTask_vtblSensorSetFS(ISensor_t *_this, float FS)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STTS22HTask *p_if_owner = (STTS22HTask *)((uint32_t) _this - offsetof(STTS22HTask, sensor_if));
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
    res = STTS22HTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t STTS22HTask_vtblSensorSetFifoWM(ISensor_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  /* Does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("STTS22H: warning - SetFifoWM() not supported.\r\n"));
  return SYS_INVALID_FUNC_CALL_ERROR_CODE;
}

sys_error_code_t STTS22HTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STTS22HTask *p_if_owner = (STTS22HTask *)((uint32_t) _this - offsetof(STTS22HTask, sensor_if));
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
    res = STTS22HTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STTS22HTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STTS22HTask *p_if_owner = (STTS22HTask *)((uint32_t) _this - offsetof(STTS22HTask, sensor_if));
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
    res = STTS22HTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t STTS22HTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;

  STTS22HTask *p_if_owner = (STTS22HTask *)((uint32_t) _this - offsetof(STTS22HTask, sensor_if));

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->temp_id)
  {
    res = p_if_owner->sensor_status.IsActive;
  }

  return res;
}

SensorDescriptor_t STTS22HTask_vtblSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  STTS22HTask *p_if_owner = (STTS22HTask *)((uint32_t) _this - offsetof(STTS22HTask, sensor_if));
  return *p_if_owner->sensor_descriptor;

}

SensorStatus_t STTS22HTask_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  STTS22HTask *p_if_owner = (STTS22HTask *)((uint32_t) _this - offsetof(STTS22HTask, sensor_if));
  return p_if_owner->sensor_status;

}

/* Private function definition */
// ***************************

static sys_error_code_t STTS22HTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STTS22HTask *p_obj = (STTS22HTask *) _this;
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
            res = STTS22HTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = STTS22HTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = STTS22HTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = STTS22HTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("STTS22H: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("STTS22H: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t STTS22HTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STTS22HTask *p_obj = (STTS22HTask *) _this;
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
          SYS_DEBUGF(SYS_DBG_LEVEL_ALL,("STTS22H: new data.\r\n"));
        if (p_obj->pIRQConfig == NULL)
        {
          if (TX_SUCCESS != tx_timer_change(&p_obj->read_fifo_timer, AMT_MS_TO_TICKS(p_obj->task_delay),
                                            AMT_MS_TO_TICKS(p_obj->task_delay)))
          {
            return SYS_UNDEFINED_ERROR_CODE;
          }
        }

        res = STTS22HTaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          // notify the listeners...
          double timestamp = report.sensorDataReadyMessage.fTimestamp;
          double delta_timestamp = timestamp - p_obj->prev_timestamp;
          p_obj->prev_timestamp = timestamp;

            /* update measuredODR */
            p_obj->sensor_status.MeasuredODR = 1.0f / (float) delta_timestamp;

          EMD_1dInit(&p_obj->data, (uint8_t *)&p_obj->temperature, E_EM_FLOAT, 1);

          DataEvent_t evt;

          DataEventInit((IEvent *)&evt, p_obj->p_temp_event_src, &p_obj->data, timestamp, p_obj->temp_id);
          IEventSrcSendEvent(p_obj->p_temp_event_src, (IEvent *) &evt, NULL);

          SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("STTS22H: ts = %f\r\n", (float)timestamp));
        }
          if (p_obj->pIRQConfig == NULL)
          {
            if (TX_SUCCESS != tx_timer_activate(&p_obj->read_fifo_timer))
            {
              res = SYS_UNDEFINED_ERROR_CODE;
            }
          }
        break;
      }

      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
              res = STTS22HTaskSensorInit(p_obj);
              if(!SYS_IS_ERROR_CODE(res))
              {
                if(p_obj->sensor_status.IsActive == true)
                {
                  if(p_obj->pIRQConfig == NULL)
                  {
                    if(TX_SUCCESS != tx_timer_activate(&p_obj->read_fifo_timer))
                    {
                      res = SYS_UNDEFINED_ERROR_CODE;
                    }
                  }
                  else
                  {
                    STTS22HTaskConfigureIrqPin(p_obj, FALSE);
                  }
                }
              }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = STTS22HTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = STTS22HTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = STTS22HTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = STTS22HTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("STTS22H: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("STTS22H: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t STTS22HTaskPostReportToFront(STTS22HTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t STTS22HTaskPostReportToBack(STTS22HTask *_this, SMMessage *pReport)
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

static sys_error_code_t STTS22HTaskSensorInit(STTS22HTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t STTS22H_Id;
  int32_t ret_val = 0;
  stts22h_odr_temp_t stts22h_odr_temp = STTS22H_POWER_DOWN;

  ret_val = stts22h_dev_id_get(p_sensor_drv, &STTS22H_Id);
  if (!ret_val)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, STTS22H_Id);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("STTS22H: sensor - I am 0x%x.\r\n", STTS22H_Id));

  /* Enable BDU */
  stts22h_block_data_update_set(p_sensor_drv, PROPERTY_ENABLE);
  /* Enable Automatic Address Increment */
  stts22h_auto_increment_set(p_sensor_drv, PROPERTY_ENABLE);
  /* Put the component in standby mode. */
  stts22h_temp_data_rate_set(p_sensor_drv, STTS22H_POWER_DOWN);

  /* Unsigned value, the high temperature limit is internally decoded as
   * (TEMP_H_LIMIT-63)*0.64 degC. Writing 0 disables the high limit interrupt.
   */
  if (_this->pIRQConfig != NULL)
  {
    stts22h_temp_trlhd_src_t status;
    /* 1 => Threshold set to -39.68 degC */
    stts22h_temp_trshld_high_set(p_sensor_drv, 1);
    stts22h_temp_trshld_src_get(p_sensor_drv, &status);
  }
  else
  {
    stts22h_temp_trshld_high_set(p_sensor_drv, 0);
  }

  if (_this->sensor_status.ODR < 2.0f)
  {
    stts22h_odr_temp = STTS22H_1Hz;
    _this->task_delay = 1000;
  }
  else if (_this->sensor_status.ODR < 26.0f)
  {
    stts22h_odr_temp = STTS22H_25Hz;
    _this->task_delay = 40;
  }
  else if (_this->sensor_status.ODR < 51.0f)
  {
    stts22h_odr_temp = STTS22H_50Hz;
    _this->task_delay = 20;
  }
  else if (_this->sensor_status.ODR < 101.0f)
  {
    stts22h_odr_temp = STTS22H_100Hz;
    _this->task_delay = 10;
  }
  else
  {
    stts22h_odr_temp = STTS22H_200Hz;
    _this->task_delay = 5;
  }

  if (_this->sensor_status.IsActive)
  {
    stts22h_temp_data_rate_set(p_sensor_drv, stts22h_odr_temp);
  }
  else
  {
    stts22h_temp_data_rate_set(p_sensor_drv, STTS22H_POWER_DOWN);
    _this->sensor_status.IsActive = false;
  }

  return res;
}

static sys_error_code_t STTS22HTaskSensorReadData(STTS22HTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  int16_t temperature_celsius;

  if (_this->pIRQConfig != NULL)
  {
    stts22h_temp_trlhd_src_t status;
    stts22h_temp_trshld_src_get(p_sensor_drv, &status);
  }

  stts22h_temperature_raw_get(p_sensor_drv, (int16_t *) &temperature_celsius);
  _this->temperature = (float) temperature_celsius / 100.0f;

#if (HSD_USE_DUMMY_DATA == 1)
  _this->temperature = (float) dummyDataCounter++;
#endif

  return res;
}

static sys_error_code_t STTS22HTaskSensorRegister(STTS22HTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *temp_if = (ISensor_t *) STTS22HTaskGetTempSensorIF(_this);
  _this->temp_id = SMAddSensor(temp_if);

  return res;
}

static sys_error_code_t STTS22HTaskSensorInitTaskParams(STTS22HTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* TEMPERATURE SENSOR STATUS */
  _this->sensor_status.IsActive = TRUE;
  _this->sensor_status.FS = 100.0f;
  _this->sensor_status.Sensitivity = 1.0f;
  _this->sensor_status.ODR = 200.0f;
  _this->sensor_status.MeasuredODR = 0.0f;
  EMD_1dInit(&_this->data, (uint8_t *)&_this->temperature, E_EM_FLOAT, 1);

  return res;
}

static sys_error_code_t STTS22HTaskSensorSetODR(STTS22HTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float ODR = (float) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->temp_id)
  {
    if (ODR < 1.0f)
    {
      stts22h_temp_data_rate_set(p_sensor_drv, STTS22H_POWER_DOWN);
      /* Do not update the model in case of ODR = 0 */
      ODR = _this->sensor_status.ODR;
    }
    else if (ODR < 2.0f)
    {
      ODR = 1.0f;
      _this->task_delay = 1000;
    }
    else if (ODR < 26.0f)
    {
      ODR = 25.0f;
      _this->task_delay = 40;
    }
    else if (ODR < 51.0f)
    {
      ODR = 50.0f;
      _this->task_delay = 20;
    }
    else if (ODR < 101.0f)
    {
      ODR = 100.0f;
      _this->task_delay = 10;
    }
    else
    {
      ODR = 200.0f;
      _this->task_delay = 5;
    }

    if (!SYS_IS_ERROR_CODE(res))
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

static sys_error_code_t STTS22HTaskSensorSetFS(STTS22HTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  float FS = (float) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->temp_id)
  {
    if (FS != 100.0f)
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

static sys_error_code_t STTS22HTaskSensorEnable(STTS22HTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->temp_id)
  {
    _this->sensor_status.IsActive = TRUE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STTS22HTaskSensorDisable(STTS22HTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->temp_id)
  {
    _this->sensor_status.IsActive = FALSE;
    stts22h_temp_data_rate_set(p_sensor_drv, STTS22H_POWER_DOWN);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t STTS22HTaskSensorIsActive(const STTS22HTask *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.IsActive;
}

static sys_error_code_t STTS22HTaskEnterLowPowerMode(const STTS22HTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  stts22h_temp_data_rate_set(p_sensor_drv, STTS22H_POWER_DOWN);
  return res;
}

static sys_error_code_t STTS22HTaskConfigureIrqPin(const STTS22HTask *_this, boolean_t LowPower)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (!LowPower)
  {
    /* Configure INT Pin */
    _this->pIRQConfig->p_mx_init_f();
  }
  else
  {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // first disable the IRQ to avoid spurious interrupt to wake the MCU up.
    HAL_NVIC_DisableIRQ(_this->pIRQConfig->irq_n);
    HAL_NVIC_ClearPendingIRQ(_this->pIRQConfig->irq_n);
    // then reconfigure the PIN in analog high impedance to reduce the power consumption.
    GPIO_InitStruct.Pin =  _this->pIRQConfig->pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(_this->pIRQConfig->port, &GPIO_InitStruct);
  }

  return res;
}

/* CubeMX integration */

static void STTS22HTaskTimerCallbackFunction(ULONG timer)
{
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

//  if (sTaskObj.in_queue != NULL) { //TODO: STF.Port - how to check if the queue has been initialized ??
  if (TX_SUCCESS != tx_queue_send(&sTaskObj.in_queue, &report, TX_NO_WAIT))
  {
    /* unable to send the report. Signal the error */
    sys_error_handler();
  }
//  }
}

void STTS22HTask_EXTI_Callback(uint16_t nPin)
{
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  //  if (sTaskObj.in_queue != NULL) { //TODO: STF.Port - how to check if the queue has been initialized ??
  if (TX_SUCCESS != tx_queue_send(&sTaskObj.in_queue, &report, TX_NO_WAIT))
  {
    /* unable to send the report. Signal the error */
    sys_error_handler();
  }
//  }
}

