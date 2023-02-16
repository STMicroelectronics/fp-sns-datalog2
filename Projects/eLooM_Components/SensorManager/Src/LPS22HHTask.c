/**
 ******************************************************************************
 * @file    LPS22HHTask.c
 * @author  SRA - MCD
 * @brief   This file provides a set of functions to handle lps22hh sensor
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
#include "LPS22HHTask.h"
#include "LPS22HHTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "lps22hh_reg.h"
#include <string.h>
#include <stdlib.h>
#include "services/sysdebug.h"
#include "mx.h"

/* Private includes ----------------------------------------------------------*/

#ifndef LPS22HH_TASK_CFG_STACK_DEPTH
#define LPS22HH_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*10)
#endif

#ifndef LPS22HH_TASK_CFG_PRIORITY
#define LPS22HH_TASK_CFG_PRIORITY                 4
#endif

#ifndef LPS22HH_TASK_CFG_IN_QUEUE_LENGTH
#define LPS22HH_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#ifndef LPS22HH_TASK_CFG_TIMER_PERIOD_MS
#define LPS22HH_TASK_CFG_TIMER_PERIOD_MS          1000
#endif

#ifndef LPS22HH_TASK_CFG_I2C_ADDRESS
#define LPS22HH_TASK_CFG_I2C_ADDRESS              LPS22HH_I2C_ADD_H
#endif

#define LPS22HH_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#define SYS_DEBUGF(level, report)                SYS_DEBUGF3(SYS_DBG_LPS22HH, level, report)

#if defined(DEBUG) || defined (SYS_DEBUG)
#define sTaskObj                                  sLPS22HHTaskObj
#endif

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter_press = 0;
#endif

#ifndef FIFO_WATERMARK
#define FIFO_WATERMARK 5
#endif

/**
 *  LPS22HHTask internal structure.
 */
struct _LPS22HHTask
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
   * Implements the temperature ISensor interface.
   */
  ISensor_t temp_sensor_if;

  /**
   * Implements the pressure ISensor interface.
   */
  ISensor_t press_sensor_if;

  /**
   * Specifies temperature sensor capabilities.
   */
  const SensorDescriptor_t *temp_sensor_descriptor;

  /**
   * Specifies temperature sensor configuration.
   */
  SensorStatus_t temp_sensor_status;

  /**
   * Specifies pressure sensor capabilities.
   */
  const SensorDescriptor_t *press_sensor_descriptor;

  /**
   * Specifies pressure sensor configuration.
   */
  SensorStatus_t press_sensor_status;

  EMData_t data;

  /**
   * Specifies the sensor ID for the temperature subsensor.
   */
  uint8_t temp_id;

  /**
   * Specifies the sensor ID for the pressure subsensor.
   */
  uint8_t press_id;

  /**
   * Synchronization object used to send command to the task.
   */
  TX_QUEUE in_queue;

  /**
   * Buffer to store the data read from the sensor FIFO
   */
  uint8_t p_sensor_data_buff[256 * 5];

  /**
   * Temperature data
   */
  float p_temp_data_buff[128 * 2];

  /**
   * Pressure data
   */
  float p_press_data_buff[128 * 2];

  /**
   * ::IEventSrc interface implementation for this class.
   */
  IEventSrc *p_temp_event_src;

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
typedef struct _LPS22HHTaskClass
{
  /**
   * LPS22HHTask class virtual table.
   */
  AManagedTaskEx_vtbl vtbl;

  /**
   * Temperature IF virtual table.
   */
  ISensor_vtbl temp_sensor_if_vtbl;

  /**
   * Pressure IF virtual table.
   */
  ISensor_vtbl press_sensor_if_vtbl;

  /**
   * Specifies temperature sensor capabilities.
   */
  SensorDescriptor_t temp_class_descriptor;

  /**
   * Specifies pressure sensor capabilities.
   */
  SensorDescriptor_t press_class_descriptor;

  /**
   * LPS22HHTask (PM_STATE, ExecuteStepFunc) map.
   */
  pExecuteStepFunc_t p_pm_state2func_map[3];
} LPS22HHTaskClass_t;

/* Private member function declaration */ // ***********************************
/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t LPS22HHTaskExecuteStepState1(AManagedTask *_this);

/**
 * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t LPS22HHTaskExecuteStepDatalog(AManagedTask *_this);

/**
 * Initialize the sensor according to the actual parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t LPS22HHTaskSensorInit(LPS22HHTask *_this);

/**
 * Read the data from the sensor.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t LPS22HHTaskSensorReadData(LPS22HHTask *_this);

/**
 * Register the sensor with the global DB and initialize the default parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise
 */
static sys_error_code_t LPS22HHTaskSensorRegister(LPS22HHTask *_this);

/**
 * Initialize the default parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise
 */
static sys_error_code_t LPS22HHTaskSensorInitTaskParams(LPS22HHTask *_this);

/**
 * Private implementation of sensor interface methods for LPS22HH sensor
 */
static sys_error_code_t LPS22HHTaskSensorSetODR(LPS22HHTask *_this, SMMessage report);
static sys_error_code_t LPS22HHTaskSensorSetFS(LPS22HHTask *_this, SMMessage report);
static sys_error_code_t LPS22HHTaskSensorSetFifoWM(LPS22HHTask *_this, SMMessage report);
static sys_error_code_t LPS22HHTaskSensorEnable(LPS22HHTask *_this, SMMessage report);
static sys_error_code_t LPS22HHTaskSensorDisable(LPS22HHTask *_this, SMMessage report);

/**
 * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
 * @param _this [IN] specifies a pointer to a task object.
 * @return TRUE if the sensor is active, FALSE otherwise.
 */
static boolean_t LPS22HHTaskSensorIsActive(const LPS22HHTask *_this);

static sys_error_code_t LPS22HHTaskEnterLowPowerMode(const LPS22HHTask *_this);

static sys_error_code_t LPS22HHTaskConfigureIrqPin(const LPS22HHTask *_this, boolean_t LowPower);

/**
 * Callback function called when the software timer expires.
 *
 * @param xTimer [IN] specifies the handle of the expired timer.
 */
static void LPS22HHTaskTimerCallbackFunction(ULONG timer);

/**
 * Given a interface pointer it return the instance of the object that implement the interface.
 *
 * @param p_if [IN] specifies a sensor interface implemented by the task object.
 * @return the instance of the task object that implements the given interface.
 */
static inline LPS22HHTask* LPS22HHTaskGetOwnerFromISensorIF(ISensor_t *p_if);

/**
 * IRQ callback
 */
void LPS22HHTask_EXTI_Callback(uint16_t nPin);

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
static inline sys_error_code_t LPS22HHTaskPostReportToFront(LPS22HHTask *_this, SMMessage *pReport);

/**
 * Private function used to post a report into the back of the task queue.
 * Used to resume the task when the required by the INIT task.
 *
 * @param this [IN] specifies a pointer to the task object.
 * @param pReport [IN] specifies a report to send.
 * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
 */
static inline sys_error_code_t LPS22HHTaskPostReportToBack(LPS22HHTask *_this, SMMessage *pReport);

#if defined (__GNUC__)
// Inline function defined inline in the header file LPS22HHTask.h must be declared here as extern function.
#endif

/* Objects instance */
/********************/

/**
 * The only instance of the task object.
 */
static LPS22HHTask *sTaskObj = NULL;

/**
 * The class object.
 */
static const LPS22HHTaskClass_t sTheClass =
{
    /* Class virtual table */
    {
        LPS22HHTask_vtblHardwareInit,
        LPS22HHTask_vtblOnCreateTask,
        LPS22HHTask_vtblDoEnterPowerMode,
        LPS22HHTask_vtblHandleError,
        LPS22HHTask_vtblOnEnterTaskControlLoop,
        LPS22HHTask_vtblForceExecuteStep,
        LPS22HHTask_vtblOnEnterPowerMode },

    /* class::temp_sensor_if_vtbl virtual table */
    {
        LPS22HHTask_vtblTempGetId,
        LPS22HHTask_vtblTempGetEventSourceIF,
        LPS22HHTask_vtblTempGetDataInfo,
        LPS22HHTask_vtblTempGetODR,
        LPS22HHTask_vtblTempGetFS,
        LPS22HHTask_vtblTempGetSensitivity,
        LPS22HHTask_vtblSensorSetODR,
        LPS22HHTask_vtblSensorSetFS,
        LPS22HHTask_vtblSensorSetFifoWM,
        LPS22HHTask_vtblSensorEnable,
        LPS22HHTask_vtblSensorDisable,
        LPS22HHTask_vtblSensorIsEnabled,
        LPS22HHTask_vtblTempGetDescription,
        LPS22HHTask_vtblTempGetStatus },

    /* class::press_sensor_if_vtbl virtual table */
    {
        LPS22HHTask_vtblPressGetId,
        LPS22HHTask_vtblPressGetEventSourceIF,
        LPS22HHTask_vtblPressGetDataInfo,
        LPS22HHTask_vtblPressGetODR,
        LPS22HHTask_vtblPressGetFS,
        LPS22HHTask_vtblPressGetSensitivity,
        LPS22HHTask_vtblSensorSetODR,
        LPS22HHTask_vtblSensorSetFS,
        LPS22HHTask_vtblSensorSetFifoWM,
        LPS22HHTask_vtblSensorEnable,
        LPS22HHTask_vtblSensorDisable,
        LPS22HHTask_vtblSensorIsEnabled,
        LPS22HHTask_vtblPressGetDescription,
        LPS22HHTask_vtblPressGetStatus },

    /* TEMPERATURE DESCRIPTOR */
    {
        "lps22hh",
        COM_TYPE_TEMP,
        {
            1,
            10,
            25,
            50,
            75,
            100,
            200,
            COM_END_OF_LIST_FLOAT, },
        {
            85,
            COM_END_OF_LIST_FLOAT, },
        {
            "temp", },
        "Celsius" },
    /* PRESSURE DESCRIPTOR */
    {
        "lps22hh",
        COM_TYPE_PRESS,
        {
            1,
            10,
            25,
            50,
            75,
            100,
            200,
            COM_END_OF_LIST_FLOAT, },
        {
            1260,
            COM_END_OF_LIST_FLOAT, },
        {
            "prs", },
        "hPa" },

    /* class (PM_STATE, ExecuteStepFunc) map */
    {
        LPS22HHTaskExecuteStepState1,
        NULL,
        LPS22HHTaskExecuteStepDatalog, } };

/* Public API definition */
// *********************
ISourceObservable* LPS22HHTaskGetTempSensorIF(LPS22HHTask *_this)
{
  assert_param(_this != NULL);

  return (ISourceObservable*) &(_this->temp_sensor_if);
}

ISourceObservable* LPS22HHTaskGetPressSensorIF(LPS22HHTask *_this)
{
  assert_param(_this != NULL);

  return (ISourceObservable*) &(_this->press_sensor_if);
}

AManagedTaskEx* LPS22HHTaskAlloc(const void *pIRQConfig, const void *pCSConfig)
{
  /* This allocator implements the singleton design pattern. */
  if(sTaskObj == NULL)
  {
    sTaskObj = SysAlloc(sizeof(LPS22HHTask));

    /* Initialize the super class */
    AMTInitEx(&sTaskObj->super);

    sTaskObj->super.vptr = &sTheClass.vtbl;
    sTaskObj->temp_sensor_if.vptr = &sTheClass.temp_sensor_if_vtbl;
    sTaskObj->press_sensor_if.vptr = &sTheClass.press_sensor_if_vtbl;
    sTaskObj->temp_sensor_descriptor = &sTheClass.temp_class_descriptor;
    sTaskObj->press_sensor_descriptor = &sTheClass.press_class_descriptor;

    sTaskObj->pIRQConfig = (MX_GPIOParams_t*) pIRQConfig;
    sTaskObj->pCSConfig = (MX_GPIOParams_t*) pCSConfig;
  }

  return (AManagedTaskEx*) sTaskObj;
}

ABusIF* LPS22HHTaskGetSensorIF(LPS22HHTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc* LPS22HHTaskGetTempEventSrcIF(LPS22HHTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_temp_event_src;
}

IEventSrc* LPS22HHTaskGetPressEventSrcIF(LPS22HHTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_press_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t LPS22HHTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_obj = (LPS22HHTask*) _this;

  /* Configure CS Pin */
  if(p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t LPS22HHTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
VOID **pvStackStart,
                                              ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                              ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_obj = (LPS22HHTask*) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) LPS22HH_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(LPS22HH_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if(p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if(TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "LPS22HH_Q", item_size / 4u, p_queue_items_buff, LPS22HH_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* create the software timer*/
  if(TX_SUCCESS
      != tx_timer_create(&p_obj->read_fifo_timer, "LPS22HH_T", LPS22HHTaskTimerCallbackFunction, (ULONG )p_obj,
                         AMT_MS_TO_TICKS(LPS22HH_TASK_CFG_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if(p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(LPS22HH_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
    if(p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(LPS22HH_ID, LPS22HH_TASK_CFG_I2C_ADDRESS, 0);
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
  p_obj->p_temp_event_src = DataEventSrcAlloc();
  if(p_obj->p_temp_event_src == NULL)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }
  IEventSrcInit(p_obj->p_temp_event_src);

  p_obj->p_press_event_src = DataEventSrcAlloc();
  if(p_obj->p_press_event_src == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  IEventSrcInit(p_obj->p_press_event_src);

  memset(p_obj->p_sensor_data_buff, 0, sizeof(p_obj->p_sensor_data_buff));
  memset(p_obj->p_temp_data_buff, 0, sizeof(p_obj->p_temp_data_buff));
  memset(p_obj->p_press_data_buff, 0, sizeof(p_obj->p_press_data_buff));
  p_obj->press_id = 0;
  p_obj->temp_id = 1;
  p_obj->prev_timestamp = 0.0f;
  p_obj->fifo_level = 0;
  p_obj->samples_per_it = 0;
  p_obj->task_delay = 0;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "LPS22HH";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = LPS22HH_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = LPS22HH_TASK_CFG_PRIORITY;
  *pPreemptThreshold = LPS22HH_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = LPS22HHTaskSensorInitTaskParams(p_obj);
  if(SYS_IS_ERROR_CODE(res))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }

  res = LPS22HHTaskSensorRegister(p_obj);
  if(SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22HH: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t LPS22HHTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode, const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_obj = (LPS22HHTask*) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &p_obj->p_sensor_bus_if->m_xConnector;

  if(NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if(LPS22HHTaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22HH: -> SENSORS_ACTIVE\r\n"));
  }
  else if(NewPowerMode == E_POWER_MODE_STATE1)
  {
    if(ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {

      /* SM_SENSOR_STATE_SUSPENDING */
      lps22hh_data_rate_set(p_sensor_drv, (lps22hh_odr_t) (LPS22HH_POWER_DOWN | 0x10));
      tx_queue_flush(&p_obj->in_queue);
      if(p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_fifo_timer);
      }
      else
      {
        LPS22HHTaskConfigureIrqPin(p_obj, TRUE);
      }
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22HH: -> STATE1\r\n"));
  }
  else if(NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = LPS22HHTaskEnterLowPowerMode(p_obj);
    if(SYS_IS_ERROR_CODE(res))
    {
      sys_error_handler();
    }
    if(p_obj->pIRQConfig != NULL)
    {
      LPS22HHTaskConfigureIrqPin(p_obj, TRUE);
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22HH: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t LPS22HHTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  LPS22HHTask *p_obj = (LPS22HHTask*)_this;

  return res;
}

sys_error_code_t LPS22HHTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22HH: start.\r\n"));

#ifdef ENABLE_THREADX_DBG_PIN
  LPS22HHTask *p_obj = (LPS22HHTask*) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = LPS22HH_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t LPS22HHTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_obj = (LPS22HHTask*) _this;

  SMMessage report =
  {
      .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
      .internalMessageFE.nData = 0 };

  if((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if(AMTExIsTaskInactive(_this))
    {
      res = LPS22HHTaskPostReportToFront(p_obj, (SMMessage*) &report);
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

sys_error_code_t LPS22HHTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode, const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  LPS22HHTask *p_obj = (LPS22HHTask*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t LPS22HHTask_vtblTempGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask*) ((uint32_t) _this - offsetof(LPS22HHTask, temp_sensor_if));
  uint8_t res = p_if_owner->temp_id;

  return res;
}

uint8_t LPS22HHTask_vtblPressGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask*) ((uint32_t) _this - offsetof(LPS22HHTask, press_sensor_if));
  uint8_t res = p_if_owner->press_id;

  return res;
}

IEventSrc* LPS22HHTask_vtblTempGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask*) ((uint32_t) _this - offsetof(LPS22HHTask, temp_sensor_if));

  return p_if_owner->p_temp_event_src;
}

IEventSrc* LPS22HHTask_vtblPressGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask*) ((uint32_t) _this - offsetof(LPS22HHTask, press_sensor_if));

  return p_if_owner->p_press_event_src;
}

sys_error_code_t LPS22HHTask_vtblPressGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LPS22HHTask *p_if_owner = (LPS22HHTask*) ((uint32_t) _this - offsetof(LPS22HHTask, press_sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if((p_measured == NULL) || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = p_if_owner->press_sensor_status.MeasuredODR;
    *p_nominal = p_if_owner->press_sensor_status.ODR;
  }

  return res;
}

float LPS22HHTask_vtblPressGetFS(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask*) ((uint32_t) _this - offsetof(LPS22HHTask, press_sensor_if));
  float res = p_if_owner->press_sensor_status.FS;

  return res;
}

float LPS22HHTask_vtblPressGetSensitivity(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask*) ((uint32_t) _this - offsetof(LPS22HHTask, press_sensor_if));
  float res = p_if_owner->press_sensor_status.Sensitivity;

  return res;
}

EMData_t LPS22HHTask_vtblPressGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask*) ((uint32_t) _this - offsetof(LPS22HHTask, press_sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t LPS22HHTask_vtblTempGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LPS22HHTask *p_if_owner = (LPS22HHTask*) ((uint32_t) _this - offsetof(LPS22HHTask, temp_sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if((p_measured) == NULL || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = p_if_owner->temp_sensor_status.MeasuredODR;
    *p_nominal = p_if_owner->temp_sensor_status.ODR;
  }

  return res;
}

float LPS22HHTask_vtblTempGetFS(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask*) ((uint32_t) _this - offsetof(LPS22HHTask, temp_sensor_if));
  float res = p_if_owner->temp_sensor_status.FS;

  return res;
}

float LPS22HHTask_vtblTempGetSensitivity(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask*) ((uint32_t) _this - offsetof(LPS22HHTask, temp_sensor_if));
  float res = p_if_owner->temp_sensor_status.Sensitivity;

  return res;
}

EMData_t LPS22HHTask_vtblTempGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask*) ((uint32_t) _this - offsetof(LPS22HHTask, temp_sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t LPS22HHTask_vtblSensorSetODR(ISensor_t *_this, float ODR)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask*) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable*) _this);

  if((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }
  else
  {
    /* Set a new command report in the queue */
    SMMessage report =
    {
        .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
        .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_ODR,
        .sensorMessage.nSensorId = sensor_id,
        .sensorMessage.nParam = (uint32_t) ODR };
    res = LPS22HHTaskPostReportToBack(p_if_owner, (SMMessage*) &report);
  }

  return res;
}

sys_error_code_t LPS22HHTask_vtblSensorSetFS(ISensor_t *_this, float FS)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask*) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable*) _this);

  if((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }
  else
  {
    /* Set a new command report in the queue */
    SMMessage report =
    {
        .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
        .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FS,
        .sensorMessage.nSensorId = sensor_id,
        .sensorMessage.nParam = (uint32_t) FS };
    res = LPS22HHTaskPostReportToBack(p_if_owner, (SMMessage*) &report);
  }

  return res;

}

sys_error_code_t LPS22HHTask_vtblSensorSetFifoWM(ISensor_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask*) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable*) _this);

  if((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    /* Set a new command report in the queue */
    SMMessage report =
    {
        .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
        .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FIFO_WM,
        .sensorMessage.nSensorId = sensor_id,
        .sensorMessage.nParam = (uint16_t) fifoWM };
    res = LPS22HHTaskPostReportToBack(p_if_owner, (SMMessage*) &report);
  }

  return res;

}

sys_error_code_t LPS22HHTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask*) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable*) _this);

  if((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }
  else
  {
    /* Set a new command report in the queue */
    SMMessage report =
    {
        .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
        .sensorMessage.nCmdID = SENSOR_CMD_ID_ENABLE,
        .sensorMessage.nSensorId = sensor_id };
    res = LPS22HHTaskPostReportToBack(p_if_owner, (SMMessage*) &report);
  }

  return res;
}

sys_error_code_t LPS22HHTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask*) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable*) _this);

  if((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }
  else
  {
    /* Set a new command report in the queue */
    SMMessage report =
    {
        .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
        .sensorMessage.nCmdID = SENSOR_CMD_ID_DISABLE,
        .sensorMessage.nSensorId = sensor_id };
    res = LPS22HHTaskPostReportToBack(p_if_owner, (SMMessage*) &report);
  }

  return res;
}

boolean_t LPS22HHTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);

  if(ISourceGetId((ISourceObservable*) _this) == p_if_owner->press_id)
    res = p_if_owner->press_sensor_status.IsActive;
  else if(ISourceGetId((ISourceObservable*) _this) == p_if_owner->temp_id)
    res = p_if_owner->temp_sensor_status.IsActive;
  return res;
}

SensorDescriptor_t LPS22HHTask_vtblTempGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->temp_sensor_descriptor;
}

SensorDescriptor_t LPS22HHTask_vtblPressGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->press_sensor_descriptor;
}

SensorStatus_t LPS22HHTask_vtblTempGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->temp_sensor_status;
}

SensorStatus_t LPS22HHTask_vtblPressGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->press_sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t LPS22HHTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_obj = (LPS22HHTask*) _this;
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
              res = LPS22HHTaskSensorSetODR(p_obj, report);
              break;
            case SENSOR_CMD_ID_SET_FS:
              res = LPS22HHTaskSensorSetFS(p_obj, report);
              break;
            case SENSOR_CMD_ID_SET_FIFO_WM:
              res = LPS22HHTaskSensorSetFifoWM(p_obj, report);
              break;
            case SENSOR_CMD_ID_ENABLE:
              res = LPS22HHTaskSensorEnable(p_obj, report);
              break;
            case SENSOR_CMD_ID_DISABLE:
              res = LPS22HHTaskSensorDisable(p_obj, report);
              break;
            default:
              // unwanted report
              res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
              SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
              ;

              SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22HH: unexpected report in State1: %i\r\n", report.messageID));
              break;
          }
          break;
        }
      default:
        {
          // unwanted report
          res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
          SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

          SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22HH: unexpected report in State1: %i\r\n", report.messageID));
          break;
        }
    }
  }

  return res;
}

static sys_error_code_t LPS22HHTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_obj = (LPS22HHTask*) _this;
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
          SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("LPS22HH: new data.\r\n"));
          if(p_obj->pIRQConfig == NULL)
          {
            if(TX_SUCCESS != tx_timer_change(&p_obj->read_fifo_timer, AMT_MS_TO_TICKS(p_obj->task_delay), AMT_MS_TO_TICKS(p_obj->task_delay)))
            {
              return SYS_UNDEFINED_ERROR_CODE;
            }
          }

          res = LPS22HHTaskSensorReadData(p_obj);

          if(!SYS_IS_ERROR_CODE(res))
          {
#if LPS22HH_FIFO_ENABLED
            if(p_obj->fifo_level != 0)
            {
#endif
            // notify the listeners...
            double timestamp = report.sensorDataReadyMessage.fTimestamp;
            double delta_timestamp = timestamp - p_obj->prev_timestamp;
            p_obj->prev_timestamp = timestamp;

            DataEvent_t evt;

            if(p_obj->press_sensor_status.IsActive)
            {
              /* update measuredODR */
              p_obj->press_sensor_status.MeasuredODR = (float) p_obj->samples_per_it / (float) delta_timestamp;

              EMD_1dInit(&p_obj->data, (uint8_t*) &p_obj->p_press_data_buff[0], E_EM_FLOAT, p_obj->samples_per_it);
              DataEventInit((IEvent*) &evt, p_obj->p_press_event_src, &p_obj->data, timestamp, p_obj->press_id);
              IEventSrcSendEvent(p_obj->p_press_event_src, (IEvent*) &evt, NULL);
            }
            if(p_obj->temp_sensor_status.IsActive)
            {
              /* update measuredODR */
              p_obj->temp_sensor_status.MeasuredODR = (float) p_obj->samples_per_it / (float) delta_timestamp;

              EMD_1dInit(&p_obj->data, (uint8_t*) &p_obj->p_temp_data_buff[0], E_EM_FLOAT, p_obj->samples_per_it);
              DataEventInit((IEvent*) &evt, p_obj->p_temp_event_src, &p_obj->data, timestamp, p_obj->temp_id);
              IEventSrcSendEvent(p_obj->p_temp_event_src, (IEvent*) &evt, NULL);
            }
#if LPS22HH_FIFO_ENABLED
            }
#endif

            if(p_obj->pIRQConfig == NULL)
            {
              if(TX_SUCCESS != tx_timer_activate(&p_obj->read_fifo_timer))
              {
                res = SYS_UNDEFINED_ERROR_CODE;
              }
            }
          }
          break;
        }
      case SM_MESSAGE_ID_SENSOR_CMD:
        {
          switch(report.sensorMessage.nCmdID)
          {
            case SENSOR_CMD_ID_INIT:
              res = LPS22HHTaskSensorInit(p_obj);
              if(!SYS_IS_ERROR_CODE(res))
              {
                if(p_obj->press_sensor_status.IsActive == true || p_obj->temp_sensor_status.IsActive == true)
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
                    LPS22HHTaskConfigureIrqPin(p_obj, FALSE);
                  }
                }
              }
              break;
            case SENSOR_CMD_ID_SET_ODR:
              res = LPS22HHTaskSensorSetODR(p_obj, report);
              break;
            case SENSOR_CMD_ID_SET_FS:
              res = LPS22HHTaskSensorSetFS(p_obj, report);
              break;
            case SENSOR_CMD_ID_SET_FIFO_WM:
              res = LPS22HHTaskSensorSetFifoWM(p_obj, report);
              break;
            case SENSOR_CMD_ID_ENABLE:
              res = LPS22HHTaskSensorEnable(p_obj, report);
              break;
            case SENSOR_CMD_ID_DISABLE:
              res = LPS22HHTaskSensorDisable(p_obj, report);
              break;
            default:
              // unwanted report
              res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
              SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
              ;

              SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22HH: unexpected report in Datalog: %i\r\n", report.messageID));
              break;
          }
          break;
        }
      default:
        // unwanted report
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22HH: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t LPS22HHTaskPostReportToFront(LPS22HHTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t LPS22HHTaskPostReportToBack(LPS22HHTask *_this, SMMessage *pReport)
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

static sys_error_code_t LPS22HHTaskSensorInit(LPS22HHTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t reg0 = 0;
  int32_t ret_val = 0;
  float lps22hh_odr = 0.0f;
  uint8_t rst;

  /* Check device ID */
  ret_val = lps22hh_device_id_get(p_sensor_drv, (uint8_t*) &reg0);
  if(!ret_val)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, reg0);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22HH: sensor - I am 0x%x.\r\n", reg0));

  /* Disable MIPI I3C(SM) interface */
  lps22hh_i3c_interface_set(p_sensor_drv, LPS22HH_I3C_DISABLE);

  /* Power down the device, set Low Noise Enable (bit 5), clear One Shot (bit 4) */
  lps22hh_data_rate_set(p_sensor_drv, (lps22hh_odr_t) (LPS22HH_POWER_DOWN | 0x10));

  /* Disable low-pass filter on LPS22HH pressure data */
  lps22hh_lp_bandwidth_set(p_sensor_drv, LPS22HH_LPF_ODR_DIV_2);

  /* Set block data update mode */
  lps22hh_block_data_update_set(p_sensor_drv, PROPERTY_ENABLE);

  /* Set autoincrement for multi-byte read/write */
  lps22hh_auto_increment_set(p_sensor_drv, PROPERTY_ENABLE);

  lps22hh_reset_set(p_sensor_drv, 1);

  do
  {
    lps22hh_reset_get(p_sensor_drv, &rst);
  }
  while(rst);

#if LPS22HH_FIFO_ENABLED
  /* Set fifo mode */
  lps22hh_fifo_mode_set(p_sensor_drv, LPS22HH_STREAM_MODE);

  if(_this->pIRQConfig != NULL) {
    /* Set FIFO wm */
	lps22hh_fifo_watermark_set(p_sensor_drv, FIFO_WATERMARK);

	/* Enable FIFO stop on watermark */
	lps22hh_fifo_stop_on_wtm_set(p_sensor_drv, PROPERTY_ENABLE);

  }

  lps22hh_ctrl_reg3_t int_route;

  lps22hh_int_notification_set(p_sensor_drv, LPS22HH_INT_LATCHED);

    lps22hh_pin_int_route_get(p_sensor_drv, &int_route);
    int_route.int_f_wtm = PROPERTY_ENABLE;
    lps22hh_pin_int_route_set(p_sensor_drv, &int_route);

    _this->samples_per_it = FIFO_WATERMARK;

#else
    _this->samples_per_it = 1;
#endif

  /* Set ODR */

  if(_this->temp_sensor_status.IsActive == TRUE)
  {
    lps22hh_odr = _this->temp_sensor_status.ODR;
    _this->press_sensor_status.ODR = _this->temp_sensor_status.ODR;
  }
  else
  {
    lps22hh_odr = _this->press_sensor_status.ODR;
    _this->temp_sensor_status.ODR = _this->press_sensor_status.ODR;
  }

  if(lps22hh_odr < 2.0f)
  {
    lps22hh_data_rate_set(p_sensor_drv, LPS22HH_1_Hz);
  }
  else if(lps22hh_odr < 11.0f)
  {
    lps22hh_data_rate_set(p_sensor_drv, LPS22HH_10_Hz);
  }
  else if(lps22hh_odr < 26.0f)
  {
    lps22hh_data_rate_set(p_sensor_drv, LPS22HH_25_Hz);
  }
  else if(lps22hh_odr < 51.0f)
  {
    lps22hh_data_rate_set(p_sensor_drv, LPS22HH_50_Hz);
  }
  else if(lps22hh_odr < 76.0f)
  {
    lps22hh_data_rate_set(p_sensor_drv, LPS22HH_75_Hz);
  }
  else if(lps22hh_odr < 101.0f)
  {
    lps22hh_data_rate_set(p_sensor_drv, LPS22HH_100_Hz);
  }
  else
  {
    lps22hh_data_rate_set(p_sensor_drv, LPS22HH_200_Hz);
  }

#if LPS22HH_FIFO_ENABLED
  _this->task_delay = (uint16_t) ((1000.0f / lps22hh_odr) * (((float) (_this->samples_per_it)) / 2.0f));
#else
  _this->task_delay = (uint16_t) (1000.0f / lps22hh_odr);
#endif

  return res;
}

static sys_error_code_t LPS22HHTaskSensorReadData(LPS22HHTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &_this->p_sensor_bus_if->m_xConnector;

#if LPS22HH_FIFO_ENABLED
  lps22hh_fifo_data_level_get(p_sensor_drv, (uint8_t *)&_this->fifo_level);
  lps22hh_read_reg(p_sensor_drv, LPS22HH_FIFO_DATA_OUT_PRESS_XL, (uint8_t *) _this->p_sensor_data_buff, 5*_this->fifo_level);

  uint16_t i = 0;

  for(i = 0; i < _this->fifo_level; i++)
   {
     uint32_t press = (((uint32_t)_this->p_sensor_data_buff[5 * i + 0])) | (((uint32_t)_this->p_sensor_data_buff[5 * i + 1]) << (8 * 1)) | (((uint32_t)_this->p_sensor_data_buff[5 * i + 2]) << (8 * 2));

     /* convert the 2's complement 24 bit to 2's complement 32 bit */
     if(press & 0x00800000)
       press |= 0xFF000000;

     uint16_t temp = *((uint16_t *)(&_this->p_sensor_data_buff[5 * i + 3]));

     if(_this->press_sensor_status.IsActive && !_this->temp_sensor_status.IsActive) /* Only Pressure */
     {
       _this->p_press_data_buff[i] = (float)press/4096.0f; /* Pressure */
     }
     else if(!_this->press_sensor_status.IsActive && _this->temp_sensor_status.IsActive) /* Only Temperature */
     {
       _this->p_temp_data_buff[i] = (float)temp/100.0f; /* Temperature */
     }
     else if(_this->press_sensor_status.IsActive && _this->temp_sensor_status.IsActive) /* Both Sub Sensors */
     {
       _this->p_press_data_buff[i] = (float)press/4096.0f; /* Pressure */
       _this->p_temp_data_buff[i] = (float)temp/100.0f; /* Temperature */
     }
   }
#else
  lps22hh_reg_t reg;
  uint32_t data_raw_pressure;
  int16_t data_raw_temperature;

  /* Read output only if new value is available */
  lps22hh_read_reg(p_sensor_drv, LPS22HH_STATUS, (uint8_t*) &reg, 1);

  if(reg.status.p_da)
  {
    memset(&data_raw_pressure, 0x00, sizeof(uint32_t));
    lps22hh_pressure_raw_get(p_sensor_drv, &data_raw_pressure);
    _this->p_press_data_buff[0] = lps22hh_from_lsb_to_hpa(data_raw_pressure);
  }
  if(reg.status.t_da)
  {
    memset(&data_raw_temperature, 0x00, sizeof(int16_t));
    lps22hh_temperature_raw_get(p_sensor_drv, &data_raw_temperature);
    _this->p_temp_data_buff[0] = lps22hh_from_lsb_to_celsius(data_raw_temperature);
  }

#endif

#if (HSD_USE_DUMMY_DATA == 1)
  for (i = 0; i < _this->samples_per_it ; i++)
  {
    _this->p_press_data_buff[i]  = (float)(dummyDataCounter_press++);
  }

#endif

  return res;
}

static sys_error_code_t LPS22HHTaskSensorRegister(LPS22HHTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *press_if = (ISensor_t*) LPS22HHTaskGetPressSensorIF(_this);
  ISensor_t *temp_if = (ISensor_t*) LPS22HHTaskGetTempSensorIF(_this);

  _this->press_id = SMAddSensor(press_if);
  _this->temp_id = SMAddSensor(temp_if);

  return res;
}

static sys_error_code_t LPS22HHTaskSensorInitTaskParams(LPS22HHTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* PRESSURE STATUS */
  _this->press_sensor_status.IsActive = TRUE;
  _this->press_sensor_status.FS = 1260.0f;
  _this->press_sensor_status.Sensitivity = 1.0f;
  _this->press_sensor_status.ODR = 200.0f;
  _this->press_sensor_status.MeasuredODR = 0.0f;

  _this->temp_sensor_status.IsActive = TRUE;
  _this->temp_sensor_status.FS = 85.0f;
  _this->temp_sensor_status.Sensitivity = 1.0f;
  _this->temp_sensor_status.ODR = 200.0f;
  _this->temp_sensor_status.MeasuredODR = 0.0f;

  EMD_1dInit(&_this->data, (uint8_t*) &_this->p_press_data_buff[0], E_EM_FLOAT, 1);

  return res;
}

static sys_error_code_t LPS22HHTaskSensorSetODR(LPS22HHTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &_this->p_sensor_bus_if->m_xConnector;
  float ODR = (float) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if(id == _this->temp_id || id == _this->press_id)
  {
    if(ODR < 1.0f)
    {
      /* Power down the device, set Low Noise Enable (bit 5), clear One Shot (bit 4) */
      lps22hh_data_rate_set(p_sensor_drv, (lps22hh_odr_t) (LPS22HH_POWER_DOWN | 0x10));
      /* Do not update the model in case of ODR = 0 */
      ODR = _this->temp_sensor_status.ODR;
      ODR = _this->press_sensor_status.ODR;
    }
    else if(ODR < 2.0f)
    {
      ODR = 1.0f;
    }
    else if(ODR < 11.0f)
    {
      ODR = 10.0f;
    }
    else if(ODR < 26.0f)
    {
      ODR = 25.0f;
    }
    else if(ODR < 51.0f)
    {
      ODR = 50.0f;
    }
    else if(ODR < 76.0f)
    {
      ODR = 75.0f;
    }
    else if(ODR < 101.0f)
    {
      ODR = 100.0f;
    }
    else
    {
      ODR = 200.0f;
    }

    if(!SYS_IS_ERROR_CODE(res))
    {
      if(id == _this->press_id)
      {
        _this->press_sensor_status.ODR = ODR;
        _this->press_sensor_status.MeasuredODR = 0.0f;
      }
      else
      {
        _this->temp_sensor_status.ODR = ODR;
        _this->temp_sensor_status.MeasuredODR = 0.0f;
      }
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }
  _this->samples_per_it = 0;

  return res;
}

static sys_error_code_t LPS22HHTaskSensorSetFS(LPS22HHTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NOT_IMPLEMENTED_ERROR_CODE;

  return res;
}

static sys_error_code_t LPS22HHTaskSensorSetFifoWM(LPS22HHTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NOT_IMPLEMENTED_ERROR_CODE;
  _this->samples_per_it = 0;
  return res;
}

static sys_error_code_t LPS22HHTaskSensorEnable(LPS22HHTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if(id == _this->temp_id)
    _this->temp_sensor_status.IsActive = TRUE;
  else if(id == _this->press_id)
    _this->press_sensor_status.IsActive = TRUE;
  else
    res = SYS_INVALID_PARAMETER_ERROR_CODE;

  return res;
}

static sys_error_code_t LPS22HHTaskSensorDisable(LPS22HHTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if(id == _this->temp_id)
    _this->temp_sensor_status.IsActive = FALSE;
  else if(id == _this->press_id)
    _this->press_sensor_status.IsActive = FALSE;
  else
    res = SYS_INVALID_PARAMETER_ERROR_CODE;

  return res;
}

static boolean_t LPS22HHTaskSensorIsActive(const LPS22HHTask *_this)
{
  assert_param(_this != NULL);
  return (_this->temp_sensor_status.IsActive || _this->press_sensor_status.IsActive);
}

static sys_error_code_t LPS22HHTaskEnterLowPowerMode(const LPS22HHTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &_this->p_sensor_bus_if->m_xConnector;

  if(lps22hh_data_rate_set(p_sensor_drv, (lps22hh_odr_t) (LPS22HH_POWER_DOWN | 0x10)))
  {
    res = SYS_SENSOR_TASK_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_OP_ERROR_CODE);
  }

  return res;
}

static sys_error_code_t LPS22HHTaskConfigureIrqPin(const LPS22HHTask *_this, boolean_t LowPower)
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

static void LPS22HHTaskTimerCallbackFunction(ULONG owner)
{
  SMMessage report;
  LPS22HHTask *p_obj = (LPS22HHTask*) owner;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  // if (sTaskObj.in_queue != NULL ) {//TODO: STF.Port - how to check if the queue has been initialized ??
  if(TX_SUCCESS != tx_queue_send(&p_obj->in_queue, &report, TX_NO_WAIT))
  {
    // unable to send the report. Signal the error
    sys_error_handler();
  }
  //}
}

void LPS22HHTask_EXTI_Callback(uint16_t nPin)
{
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  //  if (sTaskObj.in_queue != NULL) { //TODO: STF.Port - how to check if the queue has been initialized ??
  if(TX_SUCCESS != tx_queue_send(&sTaskObj->in_queue, &report, TX_NO_WAIT))
  {
    /* unable to send the report. Signal the error */
    sys_error_handler();
  }
//  }
}

static inline LPS22HHTask* LPS22HHTaskGetOwnerFromISensorIF(ISensor_t *p_if)
{
  assert_param(p_if != NULL);
  LPS22HHTask *p_if_owner = NULL;

  /* check if the virtual function has been called from the pressure IF */
  p_if_owner = (LPS22HHTask*) ((uint32_t) p_if - offsetof(LPS22HHTask, press_sensor_if));
  if(!(p_if_owner->temp_sensor_if.vptr == &sTheClass.temp_sensor_if_vtbl) || !(p_if_owner->super.vptr == &sTheClass.vtbl))
  {
    /* then the virtual function has been called from the temperature IF  */
    p_if_owner = (LPS22HHTask*) ((uint32_t) p_if - offsetof(LPS22HHTask, temp_sensor_if));
  }

  return p_if_owner;
}
