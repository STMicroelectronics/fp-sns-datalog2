/**
 ******************************************************************************
 * @file    LPS22DFTask.c
 * @author  SRA - MCD
 * @brief   This file provides a set of functions to handle lps22df sensor
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

/* Includes ------------------------------------------------------------------*/
#include "LPS22DFTask.h"
#include "LPS22DFTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "lps22df_reg.h"
#include <string.h>
#include "services/sysdebug.h"
#include "mx.h"

/* Private includes ----------------------------------------------------------*/

#ifndef LPS22DF_TASK_CFG_STACK_DEPTH
#define LPS22DF_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*10)
#endif

#ifndef LPS22DF_TASK_CFG_PRIORITY
#define LPS22DF_TASK_CFG_PRIORITY                 4
#endif

#ifndef LPS22DF_TASK_CFG_IN_QUEUE_LENGTH
#define LPS22DF_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#ifndef LPS22DF_TASK_CFG_TIMER_PERIOD_MS
#define LPS22DF_TASK_CFG_TIMER_PERIOD_MS          500
#endif

#ifndef LPS22DF_TASK_CFG_I2C_ADDRESS
#define LPS22DF_TASK_CFG_I2C_ADDRESS              LPS22DF_I2C_ADD_H
#endif

#define LPS22DF_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_LPS22DF, level, message)

#if defined(DEBUG) || defined (SYS_DEBUG)
#define sTaskObj                                  sLPS22DFTaskObj
#endif

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter_press = 0;
#endif

/**
 *  LPS22DFTask internal structure.
 */
struct _LPS22DFTask
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
   * Implements the pressure ISensor interface.
   */
  ISensor_t sensor_if;

  /**
   * Specifies pressure sensor capabilities.
   */
  const SensorDescriptor_t *sensor_descriptor;

  /**
   * Specifies pressure sensor configuration.
   */
  SensorStatus_t sensor_status;

  EMData_t data;
  /**
   * Specifies the sensor ID for the pressure subsensor.
   */
  uint8_t press_id;

  /**
   * Synchronization object used to send command to the task.
   */
  TX_QUEUE in_queue;

  /**
   * Pressure data
   */
  float p_press_data_buff[LPS22DF_MAX_WTM_LEVEL];

  /**
   * Sensor data from FIFO
   */
  uint8_t p_fifo_data_buff[LPS22DF_MAX_WTM_LEVEL * 3];

  /**
   * ::IEventSrc interface implementation for this class.
   */
  IEventSrc *p_press_event_src;

  /**
   * Specifies the FIFO level
   */
  uint8_t fifo_level;

  /**
   * Specifies the FIFO watermark level (it depends from ODR)
   */
  uint8_t samples_per_it;

  /**
   * Specifies the ms delay between 2 consecutive read (it depends from ODR)
   */
  uint16_t task_delay;

  /**
   * Software timer used to generate the read command
   */
  TX_TIMER read_fifo_timer;

  /**
   * Used to update the instantaneous ODR.
   */
  double prev_timestamp;
};

/**
 * Class object declaration
 */
typedef struct _LPS22DFTaskClass
{
  /**
   * LPS22DFTask class virtual table.
   */
  AManagedTaskEx_vtbl vtbl;

  /**
   * Pressure IF virtual table.
   */
  ISensor_vtbl sensor_if_vtbl;

  /**
   * Specifies pressure sensor capabilities.
   */
  SensorDescriptor_t class_descriptor;

  /**
   * LPS22DFTask (PM_STATE, ExecuteStepFunc) map.
   */
  pExecuteStepFunc_t p_pm_state2func_map[3];
} LPS22DFTaskClass_t;

/* Private member function declaration */ // ***********************************
/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t LPS22DFTaskExecuteStepState1(AManagedTask *_this);

/**
 * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t LPS22DFTaskExecuteStepDatalog(AManagedTask *_this);

/**
 * Initialize the sensor according to the actual parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t LPS22DFTaskSensorInit(LPS22DFTask *_this);

/**
 * Read the data from the sensor.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t LPS22DFTaskSensorReadData(LPS22DFTask *_this);

/**
 * Register the sensor with the global DB and initialize the default parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise
 */
static sys_error_code_t LPS22DFTaskSensorRegister(LPS22DFTask *_this);

/**
 * Initialize the default parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise
 */
static sys_error_code_t LPS22DFTaskSensorInitTaskParams(LPS22DFTask *_this);

/**
 * Private implementation of sensor interface methods for LPS22DF sensor
 */
static sys_error_code_t LPS22DFTaskSensorSetODR(LPS22DFTask *_this, SMMessage message);
static sys_error_code_t LPS22DFTaskSensorSetFS(LPS22DFTask *_this, SMMessage message);
static sys_error_code_t LPS22DFTaskSensorSetFifoWM(LPS22DFTask *_this, SMMessage report);
static sys_error_code_t LPS22DFTaskSensorEnable(LPS22DFTask *_this, SMMessage message);
static sys_error_code_t LPS22DFTaskSensorDisable(LPS22DFTask *_this, SMMessage message);

/**
 * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
 * @param _this [IN] specifies a pointer to a task object.
 * @return TRUE if the sensor is active, FALSE otherwise.
 */
static boolean_t LPS22DFTaskSensorIsActive(const LPS22DFTask *_this);

static sys_error_code_t LPS22DFTaskEnterLowPowerMode(const LPS22DFTask *_this);

static sys_error_code_t LPS22DFTaskConfigureIrqPin(const LPS22DFTask *_this, boolean_t LowPower);

/**
 * Callback function called when the software timer expires.
 *
 * @param xTimer [IN] specifies the handle of the expired timer.
 */
static void LPS22DFTaskTimerCallbackFunction(ULONG timer);

/**
 * Given a interface pointer it return the instance of the object that implement the interface.
 *
 * @param p_if [IN] specifies a sensor interface implemented by the task object.
 * @return the instance of the task object that implements the given interface.
 */
static inline LPS22DFTask* LPS22DFTaskGetOwnerFromISensorIF(ISensor_t *p_if);

/**
 * IRQ callback
 */
void LPS22DFTask_EXTI_Callback(uint16_t nPin);

/* Inline function forward declaration */
// ***********************************
/**
 * Private function used to post a message into the front of the task queue.
 * Used to resume the task when the required by the INIT task.
 *
 * @param this [IN] specifies a pointer to the task object.
 * @param pMessage [IN] specifies a message to send.
 * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
 */
static inline sys_error_code_t LPS22DFTaskPostMessageToFront(LPS22DFTask *_this, SMMessage *pMessage);

/**
 * Private function used to post a message into the back of the task queue.
 * Used to resume the task when the required by the INIT task.
 *
 * @param this [IN] specifies a pointer to the task object.
 * @param pMessage [IN] specifies a message to send.
 * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
 */
static inline sys_error_code_t LPS22DFTaskPostMessageToBack(LPS22DFTask *_this, SMMessage *pMessage);


/* Objects instance */
/********************/

/**
 * The only instance of the task object.
 */
static LPS22DFTask sTaskObj;

/**
 * The class object.
 */
static const LPS22DFTaskClass_t sTheClass =
{
    /* Class virtual table */
    {
        LPS22DFTask_vtblHardwareInit,
        LPS22DFTask_vtblOnCreateTask,
        LPS22DFTask_vtblDoEnterPowerMode,
        LPS22DFTask_vtblHandleError,
        LPS22DFTask_vtblOnEnterTaskControlLoop,
        LPS22DFTask_vtblForceExecuteStep,
        LPS22DFTask_vtblOnEnterPowerMode },

    /* class::sensor_if_vtbl virtual table */
    {
        LPS22DFTask_vtblPressGetId,
        LPS22DFTask_vtblPressGetEventSourceIF,
        LPS22DFTask_vtblPressGetDataInfo,
        LPS22DFTask_vtblPressGetODR,
        LPS22DFTask_vtblPressGetFS,
        LPS22DFTask_vtblPressGetSensitivity,
        LPS22DFTask_vtblSensorSetODR,
        LPS22DFTask_vtblSensorSetFS,
        LPS22DFTask_vtblSensorSetFifoWM,
        LPS22DFTask_vtblSensorEnable,
        LPS22DFTask_vtblSensorDisable,
        LPS22DFTask_vtblSensorIsEnabled,
        LPS22DFTask_vtblPressGetDescription,
        LPS22DFTask_vtblPressGetStatus },

    /* PRESSURE DESCRIPTOR */
    {
        "lps22df",
        COM_TYPE_PRESS,
        {
            1.0f,
            4.0f,
            10.0f,
            25.0f,
            50.0f,
            75.0f,
            100.0f,
            200.0f,
            COM_END_OF_LIST_FLOAT, },
        {
            1260.0f,
            4060.0f,
            COM_END_OF_LIST_FLOAT, },
        {
            "prs", },
        "hPa",
        {
            0,
            1000, } },

    /* class (PM_STATE, ExecuteStepFunc) map */
    {
        LPS22DFTaskExecuteStepState1,
        NULL,
        LPS22DFTaskExecuteStepDatalog, } };

/* Public API definition */
// *********************
ISourceObservable* LPS22DFTaskGetPressSensorIF(LPS22DFTask *_this)
{
  assert_param(_this != NULL);
  return (ISourceObservable*) &(_this->sensor_if);
}

AManagedTaskEx* LPS22DFTaskAlloc(const void *pIRQConfig, const void *pCSConfig)
{
  /* This allocator implements the singleton design pattern. */

  /* Initialize the super class */
  AMTInitEx(&sTaskObj.super);

  sTaskObj.super.vptr = &sTheClass.vtbl;
  sTaskObj.sensor_if.vptr = &sTheClass.sensor_if_vtbl;
  sTaskObj.sensor_descriptor = &sTheClass.class_descriptor;

  sTaskObj.pIRQConfig = (MX_GPIOParams_t*) pIRQConfig;
  sTaskObj.pCSConfig = (MX_GPIOParams_t*) pCSConfig;

  return (AManagedTaskEx*) &sTaskObj;
}

ABusIF* LPS22DFTaskGetSensorIF(LPS22DFTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc* LPS22DFTaskGetPressEventSrcIF(LPS22DFTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_press_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t LPS22DFTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_obj = (LPS22DFTask*) _this;

  /* Configure CS Pin */
  if(p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t LPS22DFTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
VOID **pvStackStart,
                                              ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                              ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_obj = (LPS22DFTask*) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) LPS22DF_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(LPS22DF_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if(p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if(TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "LPS22DF_Q", item_size / 4u, p_queue_items_buff, LPS22DF_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* create the software timer*/
  if(TX_SUCCESS
      != tx_timer_create(&p_obj->read_fifo_timer, "ILPS22WS_T", LPS22DFTaskTimerCallbackFunction, (ULONG)TX_NULL,
                         AMT_MS_TO_TICKS(LPS22DF_TASK_CFG_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if(p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(LPS22DF_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
    if(p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(LPS22DF_ID, LPS22DF_TASK_CFG_I2C_ADDRESS, 0);
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
  p_obj->p_press_event_src = DataEventSrcAlloc();
  if(p_obj->p_press_event_src == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  IEventSrcInit(p_obj->p_press_event_src);

  memset(p_obj->p_press_data_buff, 0, sizeof(p_obj->p_press_data_buff));
  p_obj->press_id = 0;
  p_obj->prev_timestamp = 0.0f;
  p_obj->fifo_level = 0;
  p_obj->samples_per_it = 0;
  p_obj->task_delay = 0;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "LPS22DF";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = LPS22DF_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = LPS22DF_TASK_CFG_PRIORITY;
  *pPreemptThreshold = LPS22DF_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = LPS22DFTaskSensorInitTaskParams(p_obj);
  if(SYS_IS_ERROR_CODE(res))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }

  res = LPS22DFTaskSensorRegister(p_obj);
  if(SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22DF: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t LPS22DFTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode, const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_obj = (LPS22DFTask*) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &p_obj->p_sensor_bus_if->m_xConnector;

  if(NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if(LPS22DFTaskSensorIsActive(p_obj))
    {
      SMMessage message =
      {
          .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
          .sensorMessage.nCmdID = SENSOR_CMD_ID_INIT };

      if(tx_queue_send(&p_obj->in_queue, &message, AMT_MS_TO_TICKS(100)) != TX_SUCCESS)
      {
        res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE);
      }

      // reset the variables for the time stamp computation.
      p_obj->prev_timestamp = 0.0f;
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22DF: -> SENSORS_ACTIVE\r\n"));
  }
  else if(NewPowerMode == E_POWER_MODE_STATE1)
  {
    if(ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      lps22df_md_t val;
      lps22df_mode_get(p_sensor_drv, &val);
      val.odr = LPS22DF_ONE_SHOT;

      lps22df_fifo_md_t fifo_md;
      fifo_md.watermark = 1;
      fifo_md.operation = LPS22DF_BYPASS;

      lps22df_fifo_mode_set(p_sensor_drv, &fifo_md);

      /* SM_SENSOR_STATE_SUSPENDING */
      lps22df_mode_set(p_sensor_drv, &val);
      tx_queue_flush(&p_obj->in_queue);
      if(p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_fifo_timer);
      }
      else
      {
        LPS22DFTaskConfigureIrqPin(p_obj, TRUE);
      }
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22DF: -> STATE1\r\n"));
  }
  else if(NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = LPS22DFTaskEnterLowPowerMode(p_obj);
    if(SYS_IS_ERROR_CODE(res))
    {
      sys_error_handler();
    }
    if(p_obj->pIRQConfig != NULL)
    {
      LPS22DFTaskConfigureIrqPin(p_obj, TRUE);
    }
    /* notify the bus */
    if(p_obj->p_sensor_bus_if->m_pfBusCtrl != NULL)
    {
      p_obj->p_sensor_bus_if->m_pfBusCtrl(p_obj->p_sensor_bus_if, E_BUS_CTRL_DEV_NOTIFY_POWER_MODE, 0);
    }
    if(p_obj->pIRQConfig == NULL)
    {
      tx_timer_deactivate(&p_obj->read_fifo_timer);
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22DF: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t LPS22DFTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  LPS22DFTask *p_obj = (LPS22DFTask*)_this;

  return res;
}

sys_error_code_t LPS22DFTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22DF: start.\r\n"));

#ifdef ENABLE_THREADX_DBG_PIN
  LPS22DFTask *p_obj = (LPS22DFTask*) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = LPS22DF_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t LPS22DFTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_obj = (LPS22DFTask*) _this;

  SMMessage message =
  {
      .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
      .internalMessageFE.nData = 0 };

  if((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if(AMTExIsTaskInactive(_this))
    {
      res = LPS22DFTaskPostMessageToFront(p_obj, (SMMessage*) &message);
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

sys_error_code_t LPS22DFTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode, const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  LPS22DFTask *p_obj = (LPS22DFTask*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t LPS22DFTask_vtblPressGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22DFTask *p_if_owner = (LPS22DFTask*) ((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
  uint8_t res = p_if_owner->press_id;

  return res;
}

IEventSrc* LPS22DFTask_vtblPressGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22DFTask *p_if_owner = (LPS22DFTask*) ((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));

  return p_if_owner->p_press_event_src;
}

sys_error_code_t LPS22DFTask_vtblPressGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LPS22DFTask *p_if_owner = (LPS22DFTask*) ((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
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

float LPS22DFTask_vtblPressGetFS(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22DFTask *p_if_owner = (LPS22DFTask*) ((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
  float res = p_if_owner->sensor_status.FS;

  return res;
}

float LPS22DFTask_vtblPressGetSensitivity(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22DFTask *p_if_owner = (LPS22DFTask*) ((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
  float res = p_if_owner->sensor_status.Sensitivity;

  return res;
}

EMData_t LPS22DFTask_vtblPressGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22DFTask *p_if_owner = (LPS22DFTask*) ((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t LPS22DFTask_vtblSensorSetODR(ISensor_t *_this, float ODR)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_if_owner = LPS22DFTaskGetOwnerFromISensorIF(_this);

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask*) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable*) _this);

  if((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }
  else
  {
    /* Set a new command message in the queue */
    SMMessage message =
    {
        .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
        .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_ODR,
        .sensorMessage.nSensorId = sensor_id,
        .sensorMessage.nParam = (uint32_t) ODR };
    res = LPS22DFTaskPostMessageToBack(p_if_owner, (SMMessage*) &message);
  }

  return res;
}

sys_error_code_t LPS22DFTask_vtblSensorSetFS(ISensor_t *_this, float FS)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_if_owner = LPS22DFTaskGetOwnerFromISensorIF(_this);

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask*) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable*) _this);

  if((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }
  else
  {
    /* Set a new command message in the queue */
    SMMessage message =
    {
        .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
        .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FS,
        .sensorMessage.nSensorId = sensor_id,
        .sensorMessage.nParam = (uint32_t) FS };
    res = LPS22DFTaskPostMessageToBack(p_if_owner, (SMMessage*) &message);
  }

  return res;
}

sys_error_code_t LPS22DFTask_vtblSensorSetFifoWM(ISensor_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

#if LPS22DF_FIFO_ENABLED
  LPS22DFTask *p_if_owner = (LPS22DFTask*) ((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
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
        .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FIFO_WM,
        .sensorMessage.nSensorId = sensor_id,
        .sensorMessage.nParam = (uint16_t) fifoWM };
    res = LPS22DFTaskPostMessageToBack(p_if_owner, (SMMessage*) &report);
  }
#endif

  return res;
}

sys_error_code_t LPS22DFTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_if_owner = LPS22DFTaskGetOwnerFromISensorIF(_this);

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask*) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable*) _this);

  if((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }
  else
  {
    /* Set a new command message in the queue */
    SMMessage message =
    {
        .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
        .sensorMessage.nCmdID = SENSOR_CMD_ID_ENABLE,
        .sensorMessage.nSensorId = sensor_id };
    res = LPS22DFTaskPostMessageToBack(p_if_owner, (SMMessage*) &message);
  }

  return res;
}

sys_error_code_t LPS22DFTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_if_owner = LPS22DFTaskGetOwnerFromISensorIF(_this);

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask*) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable*) _this);

  if((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }
  else
  {
    /* Set a new command message in the queue */
    SMMessage message =
    {
        .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
        .sensorMessage.nCmdID = SENSOR_CMD_ID_DISABLE,
        .sensorMessage.nSensorId = sensor_id };
    res = LPS22DFTaskPostMessageToBack(p_if_owner, (SMMessage*) &message);
  }

  return res;
}

boolean_t LPS22DFTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  LPS22DFTask *p_if_owner = LPS22DFTaskGetOwnerFromISensorIF(_this);

  if(ISourceGetId((ISourceObservable*) _this) == p_if_owner->press_id)
  {
    res = p_if_owner->sensor_status.IsActive;
  }

  return res;
}

SensorDescriptor_t LPS22DFTask_vtblPressGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LPS22DFTask *p_if_owner = LPS22DFTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->sensor_descriptor;
}

SensorStatus_t LPS22DFTask_vtblPressGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LPS22DFTask *p_if_owner = LPS22DFTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t LPS22DFTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_obj = (LPS22DFTask*) _this;
  SMMessage message =
  {
      0 };

  AMTExSetInactiveState((AManagedTaskEx*) _this, TRUE);
  if(TX_SUCCESS == tx_queue_receive(&p_obj->in_queue, &message, TX_WAIT_FOREVER))
  {
    AMTExSetInactiveState((AManagedTaskEx*) _this, FALSE);

    switch(message.messageID)
    {
      case SM_MESSAGE_ID_FORCE_STEP:
        {
          // do nothing. I need only to resume.
          __NOP();
          break;
        }
      case SM_MESSAGE_ID_SENSOR_CMD:
        {
          switch(message.sensorMessage.nCmdID)
          {
            case SENSOR_CMD_ID_SET_ODR:
              res = LPS22DFTaskSensorSetODR(p_obj, message);
              break;
            case SENSOR_CMD_ID_SET_FS:
              res = LPS22DFTaskSensorSetFS(p_obj, message);
              break;
            case SENSOR_CMD_ID_SET_FIFO_WM:
              res = LPS22DFTaskSensorSetFifoWM(p_obj, message);
              break;
            case SENSOR_CMD_ID_ENABLE:
              res = LPS22DFTaskSensorEnable(p_obj, message);
              break;
            case SENSOR_CMD_ID_DISABLE:
              res = LPS22DFTaskSensorDisable(p_obj, message);
              break;
            default:
              // unwanted message
              res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
              SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
              ;

              SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22DF: unexpected message in State1: %i\r\n", message.messageID));
              break;
          }
          break;
        }
      default:
        {
          // unwanted message
          res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
          SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

          SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22DF: unexpected message in State1: %i\r\n", message.messageID));
          break;
        }
    }
  }

  return res;
}

static sys_error_code_t LPS22DFTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_obj = (LPS22DFTask*) _this;
  SMMessage message =
  {
      0 };

  AMTExSetInactiveState((AManagedTaskEx*) _this, TRUE);
  if(TX_SUCCESS == tx_queue_receive(&p_obj->in_queue, &message, TX_WAIT_FOREVER))
  {
    AMTExSetInactiveState((AManagedTaskEx*) _this, FALSE);

    switch(message.messageID)
    {
      case SM_MESSAGE_ID_FORCE_STEP:
        {
          // do nothing. I need only to resume.
          __NOP();
          break;
        }
      case SM_MESSAGE_ID_DATA_READY:
        {
          SYS_DEBUGF(SYS_DBG_LEVEL_ALL,("LPS22DF: new data.\r\n"));
          if(p_obj->pIRQConfig == NULL)
          {
            if(TX_SUCCESS != tx_timer_change(&p_obj->read_fifo_timer, AMT_MS_TO_TICKS(p_obj->task_delay), AMT_MS_TO_TICKS(p_obj->task_delay)))
            {
              return SYS_UNDEFINED_ERROR_CODE;
            }
          }

          res = LPS22DFTaskSensorReadData(p_obj);
          if(!SYS_IS_ERROR_CODE(res))
          {
#if LPS22DF_FIFO_ENABLED
            if(p_obj->fifo_level != 0)
            {
#endif
            // notify the listeners...
            double timestamp = message.sensorDataReadyMessage.fTimestamp;
            double delta_timestamp = timestamp - p_obj->prev_timestamp;
            p_obj->prev_timestamp = timestamp;

            /* update measuredODR */
            p_obj->sensor_status.MeasuredODR = (float) p_obj->samples_per_it / (float) delta_timestamp;

            EMD_1dInit(&p_obj->data, (uint8_t*) &p_obj->p_press_data_buff[0], E_EM_FLOAT, p_obj->samples_per_it);

            DataEvent_t evt;

            DataEventInit((IEvent*) &evt, p_obj->p_press_event_src, &p_obj->data, timestamp, p_obj->press_id);
            IEventSrcSendEvent(p_obj->p_press_event_src, (IEvent*) &evt, NULL);
#if LPS22DF_FIFO_ENABLED
            }
#endif
          }
            if(p_obj->pIRQConfig == NULL)
            {
              if(TX_SUCCESS != tx_timer_activate(&p_obj->read_fifo_timer))
              {
                res = SYS_UNDEFINED_ERROR_CODE;
              }
            }
          break;
        }
      case SM_MESSAGE_ID_SENSOR_CMD:
        {
          switch(message.sensorMessage.nCmdID)
          {
            case SENSOR_CMD_ID_INIT:
              res = LPS22DFTaskSensorInit(p_obj);
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
                    LPS22DFTaskConfigureIrqPin(p_obj, FALSE);
                  }
                }
              }
              break;
            case SENSOR_CMD_ID_SET_ODR:
              res = LPS22DFTaskSensorSetODR(p_obj, message);
              break;
            case SENSOR_CMD_ID_SET_FS:
              res = LPS22DFTaskSensorSetFS(p_obj, message);
              break;
            case SENSOR_CMD_ID_SET_FIFO_WM:
              res = LPS22DFTaskSensorSetFifoWM(p_obj, message);
              break;
            case SENSOR_CMD_ID_ENABLE:
              res = LPS22DFTaskSensorEnable(p_obj, message);
              break;
            case SENSOR_CMD_ID_DISABLE:
              res = LPS22DFTaskSensorDisable(p_obj, message);
              break;
            default:
              // unwanted message
              res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
              SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
              ;

              SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22DF: unexpected message in Datalog: %i\r\n", message.messageID));
              break;
          }
          break;
        }
      default:
        // unwanted message
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22DF: unexpected message in Datalog: %i\r\n", message.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t LPS22DFTaskPostMessageToFront(LPS22DFTask *_this, SMMessage *pMessage)
{
  assert_param(_this != NULL);
  assert_param(pMessage);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if(SYS_IS_CALLED_FROM_ISR())
  {
    if(TX_SUCCESS != tx_queue_front_send(&_this->in_queue, pMessage, TX_NO_WAIT))
    {
      res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
      // this function is private and the caller will ignore this return code.
    }
  }
  else
  {
    if(TX_SUCCESS != tx_queue_front_send(&_this->in_queue, pMessage, AMT_MS_TO_TICKS(100)))
    {
      res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
      // this function is private and the caller will ignore this return code.
    }
  }

  return res;
}

static inline sys_error_code_t LPS22DFTaskPostMessageToBack(LPS22DFTask *_this, SMMessage *pMessage)
{
  assert_param(_this != NULL);
  assert_param(pMessage);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if(SYS_IS_CALLED_FROM_ISR())
  {
    if(TX_SUCCESS != tx_queue_send(&_this->in_queue, pMessage, TX_NO_WAIT))
    {
      res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
      // this function is private and the caller will ignore this return code.
    }
  }
  else
  {
    if(TX_SUCCESS != tx_queue_send(&_this->in_queue, pMessage, AMT_MS_TO_TICKS(100)))
    {
      res = SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
      // this function is private and the caller will ignore this return code.
    }
  }

  return res;
}

static sys_error_code_t LPS22DFTaskSensorInit(LPS22DFTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &_this->p_sensor_bus_if->m_xConnector;

  int32_t ret_val = 0;

  lps22df_int_mode_t int_mode =
  {
      0 };
  lps22df_pin_int_route_t int_route =
  {
      0 };

  /* Set bdu and if_inc recommended for driver usage */
  lps22df_init_set(p_sensor_drv, LPS22DF_DRV_RDY);

  /* Select bus interface */
  lps22df_bus_mode_t bus_mode;
  bus_mode.interface = LPS22DF_SEL_BY_HW;
  bus_mode.filter = LPS22DF_AUTO;
  lps22df_bus_mode_set(p_sensor_drv, &bus_mode);

  /* Set Output Data Rate in Power Down */
  /* Configure basic parameters */
  lps22df_md_t md;
  lps22df_mode_get(p_sensor_drv, &md);
  md.odr = LPS22DF_ONE_SHOT;
  md.avg = LPS22DF_4_AVG;
  md.lpf = LPS22DF_LPF_ODR_DIV_4;
  lps22df_mode_set(p_sensor_drv, &md);

  /* Read sensor id */
  lps22df_id_t reg0;

  ret_val = lps22df_id_get(p_sensor_drv, &reg0);
  if(!ret_val)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, reg0.whoami);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22DF: sensor - I am 0x%x.\r\n", reg0.whoami));

#if LPS22DF_FIFO_ENABLED

  if(_this->samples_per_it == 0)
   {
     /* Set fifo mode */
     uint16_t lps22df_wtm_level = 0;

     /* Calculation of watermark and samples per int*/
     lps22df_wtm_level = ((uint16_t) _this->sensor_status.ODR * (uint16_t) LPS22DF_MAX_DRDY_PERIOD);
     if(lps22df_wtm_level > LPS22DF_MAX_WTM_LEVEL)
     {
       lps22df_wtm_level = LPS22DF_MAX_WTM_LEVEL;
     }
     else if(lps22df_wtm_level < LPS22DF_MIN_WTM_LEVEL)
     {
       lps22df_wtm_level = LPS22DF_MIN_WTM_LEVEL;
     }

     _this->samples_per_it = lps22df_wtm_level;
   }

   lps22df_fifo_md_t fifo_md;
   fifo_md.watermark = _this->samples_per_it;
   fifo_md.operation = LPS22DF_STREAM;
   lps22df_fifo_mode_set(p_sensor_drv, &fifo_md);;
  /* Configure Interrupt */
  if(_this->pIRQConfig != NULL)
  {
    int_mode.drdy_latched = 0;
    int_route.fifo_th = 1;
  }
  else
  {
    int_mode.drdy_latched = 1;
    int_route.fifo_th = 0;
  }
#else
  _this->samples_per_it = 1;
  /* Configure Interrupt */
  if(_this->pIRQConfig != NULL)
  {
    int_mode.drdy_latched = 0;
    int_route.drdy_pres = 1;
  }
  else
  {
    int_mode.drdy_latched = 1;
    int_route.drdy_pres = 0;
  }
#endif /* LPS22DF_FIFO_ENABLED */
  lps22df_interrupt_mode_set(p_sensor_drv, &int_mode);
  lps22df_pin_int_route_set(p_sensor_drv, &int_route);

  /* Set ODR */
  lps22df_mode_get(p_sensor_drv, &md);

  float lps22df_odr;
  lps22df_odr = _this->sensor_status.ODR;
  if(lps22df_odr < 2.0f)
  {
    md.odr = LPS22DF_1Hz;
  }
  else if(lps22df_odr < 5.0f)
  {
    md.odr = LPS22DF_4Hz;
  }
  else if(lps22df_odr < 11.0f)
  {
    md.odr = LPS22DF_10Hz;
  }
  else if(lps22df_odr < 26.0f)
  {
    md.odr = LPS22DF_25Hz;
  }
  else if(lps22df_odr < 51.0f)
  {
    md.odr = LPS22DF_50Hz;
  }
  else if(lps22df_odr < 76.0f)
  {
    md.odr = LPS22DF_75Hz;
  }
  else if(lps22df_odr < 101.0f)
  {
    md.odr = LPS22DF_100Hz;
  }
  else
  {
    md.odr = LPS22DF_200Hz;
  }
  lps22df_mode_set(p_sensor_drv, &md);

#if LPS22DF_FIFO_ENABLED
  lps22df_fifo_level_get(p_sensor_drv, (uint8_t*) &_this->fifo_level);

  if(_this->fifo_level >= _this->samples_per_it)
  {
    lps22df_read_reg(p_sensor_drv, LPS22DF_FIFO_DATA_OUT_PRESS_XL, (uint8_t *) _this->p_fifo_data_buff, _this->samples_per_it * 3);
  }
#endif

#if LPS22DF_FIFO_ENABLED
  _this->task_delay = (uint16_t)((1000.0f/_this->sensor_status.ODR)*(((float)(_this->samples_per_it))/2.0f));
#else
  _this->task_delay = (uint16_t) (1000.0f / _this->sensor_status.ODR);
#endif

  return res;
}

static sys_error_code_t LPS22DFTaskSensorReadData(LPS22DFTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &_this->p_sensor_bus_if->m_xConnector;

  uint16_t i = 0;

#if LPS22DF_FIFO_ENABLED
  /* Check FIFO_WTM_IA and fifo level */
  lps22df_fifo_level_get(p_sensor_drv, &_this->fifo_level);

  if(_this->fifo_level >= _this->samples_per_it)
  {
    lps22df_read_reg(p_sensor_drv, LPS22DF_FIFO_DATA_OUT_PRESS_XL, (uint8_t*) _this->p_fifo_data_buff, _this->samples_per_it * 3);
  }
  else
  {
    _this->fifo_level = 0;
  }
#else
  lps22df_read_reg(p_sensor_drv, LPS22DF_PRESS_OUT_XL, (uint8_t *) _this->p_fifo_data_buff, _this->samples_per_it * 3);
  _this->fifo_level = 1;
#endif /* LPS22DF_FIFO_ENABLED */

#if (HSD_USE_DUMMY_DATA == 1)
  for (i = 0; i < _this->samples_per_it ; i++)
  {
    _this->p_press_data_buff[i]  = (float)(dummyDataCounter_press++);
  }
#else
  /* Arrange Data */
  int32_t data;
  uint8_t *p8_src = _this->p_fifo_data_buff;
  uint8_t *p8_dest = _this->p_fifo_data_buff;

  if(_this->fifo_level >= _this->samples_per_it)
  {
    for(i = 0; i < _this->samples_per_it; i++)
    {
      data = (int16_t) (p8_src[2]);
      data = (data * 256) + p8_src[1];
      data = (data * 256) + p8_src[0];
      data = (data * 256);
      *p8_dest++ = *p8_src++;
      *p8_dest++ = *p8_src++;
      *p8_dest++ = *p8_src++;

      _this->p_press_data_buff[i] = lps22df_from_lsb_to_hPa(data);
    }
  }
#endif

  return res;
}

static sys_error_code_t LPS22DFTaskSensorRegister(LPS22DFTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *press_if = (ISensor_t*) LPS22DFTaskGetPressSensorIF(_this);

  _this->press_id = SMAddSensor(press_if);

  return res;
}

static sys_error_code_t LPS22DFTaskSensorInitTaskParams(LPS22DFTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* PRESSURE STATUS */
  _this->sensor_status.IsActive = TRUE;
  _this->sensor_status.FS = 4060.0f;
  _this->sensor_status.Sensitivity = 1.0f;
  _this->sensor_status.ODR = 200.0f;
  _this->sensor_status.MeasuredODR = 0.0f;
  EMD_1dInit(&_this->data, (uint8_t*) &_this->p_press_data_buff[0], E_EM_FLOAT, 1);

  return res;
}

static sys_error_code_t LPS22DFTaskSensorSetODR(LPS22DFTask *_this, SMMessage message)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &_this->p_sensor_bus_if->m_xConnector;
  float ODR = (float) message.sensorMessage.nParam;
  uint8_t id = message.sensorMessage.nSensorId;

  lps22df_md_t md;
  lps22df_mode_get(p_sensor_drv, &md);

  if(id == _this->press_id)
  {
    if(ODR < 1.0f)
    {
      md.odr = LPS22DF_ONE_SHOT;
      lps22df_mode_set(p_sensor_drv, &md);
      /* Do not update the model in case of ODR = 0 */
      ODR = _this->sensor_status.ODR;
    }
    else if(ODR < 2.0f)
    {
      ODR = 1;
      _this->task_delay = 2000;
    }
    else if(ODR < 5.0f)
    {
      ODR = 4;
      _this->task_delay = 1000;
    }
    else if(ODR < 11.0f)
    {
      ODR = 10;
      _this->task_delay = 1000;
    }
    else if(ODR < 26.0f)
    {
      ODR = 25;
      _this->task_delay = 1000;
    }
    else if(ODR < 51.0f)
    {
      ODR = 50;
      _this->task_delay = 1000;
    }
    else if(ODR < 76.0f)
    {
      ODR = 75;
      _this->task_delay = 1000;
    }
    else if(ODR < 101.0f)
    {
      ODR = 100;
      _this->task_delay = 1000;
    }
    else
    {
      ODR = 100;
      _this->task_delay = 500;
    }

    if(!SYS_IS_ERROR_CODE(res))
    {
      if(id == _this->press_id)
      {
        _this->sensor_status.ODR = ODR;
        _this->sensor_status.MeasuredODR = 0.0f;
      }
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t LPS22DFTaskSensorSetFS(LPS22DFTask *_this, SMMessage message)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NOT_IMPLEMENTED_ERROR_CODE;

  return res;
}

static sys_error_code_t LPS22DFTaskSensorSetFifoWM(LPS22DFTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  lps22df_fifo_md_t fifo_md;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t lps22df_wtm_level = report.sensorMessage.nParam;

  if(lps22df_wtm_level > LPS22DF_MAX_WTM_LEVEL)
  {
    lps22df_wtm_level = LPS22DF_MAX_WTM_LEVEL;
  }
  _this->samples_per_it = lps22df_wtm_level;

  fifo_md.watermark = _this->samples_per_it;
  fifo_md.operation = LPS22DF_STREAM;

  lps22df_fifo_mode_set(p_sensor_drv, &fifo_md);

  return res;
}

static sys_error_code_t LPS22DFTaskSensorEnable(LPS22DFTask *_this, SMMessage message)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = message.sensorMessage.nSensorId;

  if(id == _this->press_id)
  {
    _this->sensor_status.IsActive = TRUE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t LPS22DFTaskSensorDisable(LPS22DFTask *_this, SMMessage message)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &_this->p_sensor_bus_if->m_xConnector;
  lps22df_md_t md;

  uint8_t id = message.sensorMessage.nSensorId;

  if(id == _this->press_id)
  {
    _this->sensor_status.IsActive = FALSE;
    md.odr = LPS22DF_ONE_SHOT;
    lps22df_mode_set(p_sensor_drv, &md);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t LPS22DFTaskSensorIsActive(const LPS22DFTask *_this)
{
  assert_param(_this != NULL);
  return (_this->sensor_status.IsActive);
}

static sys_error_code_t LPS22DFTaskEnterLowPowerMode(const LPS22DFTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &_this->p_sensor_bus_if->m_xConnector;

  lps22df_md_t val;
  lps22df_mode_get(p_sensor_drv, &val);
  val.odr = LPS22DF_ONE_SHOT;

  if(lps22df_mode_set(p_sensor_drv, &val))
  {
    res = SYS_SENSOR_TASK_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_OP_ERROR_CODE);
  }

  return res;
}

static sys_error_code_t LPS22DFTaskConfigureIrqPin(const LPS22DFTask *_this, boolean_t LowPower)
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

static void LPS22DFTaskTimerCallbackFunction(ULONG timer)
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

void LPS22DFTask_EXTI_Callback(uint16_t nPin)
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

static inline LPS22DFTask* LPS22DFTaskGetOwnerFromISensorIF(ISensor_t *p_if)
{
  assert_param(p_if != NULL);
  LPS22DFTask *p_if_owner = NULL;

  /* check if the virtual function has been called from the pressure IF */
  p_if_owner = (LPS22DFTask*) ((uint32_t) p_if - offsetof(LPS22DFTask, sensor_if));

  return p_if_owner;
}
