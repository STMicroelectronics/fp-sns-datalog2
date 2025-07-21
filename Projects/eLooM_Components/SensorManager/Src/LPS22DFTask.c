/**
  ******************************************************************************
  * @file    LPS22DFTask.c
  * @author  SRA - MCD
  * @brief   This file provides a set of functions to handle lps22df sensor
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
#include "LPS22DFTask.h"
#include "LPS22DFTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "lps22df_reg.h"
#include <string.h>
#include "services/sysdebug.h"

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

#define LPS22DF_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef LPS22DF_TASK_CFG_TIMER_PERIOD_MS
#define LPS22DF_TASK_CFG_TIMER_PERIOD_MS          500
#endif

#ifndef LPS22DF_TASK_CFG_MAX_INSTANCES_COUNT
#define LPS22DF_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_LPS22DF, level, message)

#ifndef LPS22DF_TASK_CFG_I2C_ADDRESS
#define LPS22DF_TASK_CFG_I2C_ADDRESS              LPS22DF_I2C_ADD_H
#endif

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter_press = 0;
#endif


/**
  * Class object declaration
  */
typedef struct _LPS22DFTaskClass
{
  /**
    * LPS22DFTask class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * Pressure IF virtual table.
    */
  const ISensorMems_vtbl sensor_if_vtbl;

  /**
    * Specifies pressure sensor capabilities.
    */
  const SensorDescriptor_t class_descriptor;

  /**
    * LPS22DFTask (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[LPS22DF_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

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
  * @param param [IN] specifies an application defined parameter.
  */
static void LPS22DFTaskTimerCallbackFunction(ULONG param);


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
static inline sys_error_code_t LPS22DFTaskPostReportToFront(LPS22DFTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t LPS22DFTaskPostReportToBack(LPS22DFTask *_this, SMMessage *pReport);


/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static LPS22DFTask sTaskObj;

/**
  * The class object.
  */
static LPS22DFTaskClass_t sTheClass =
{
  /* class virtual table */
  {
    LPS22DFTask_vtblHardwareInit,
    LPS22DFTask_vtblOnCreateTask,
    LPS22DFTask_vtblDoEnterPowerMode,
    LPS22DFTask_vtblHandleError,
    LPS22DFTask_vtblOnEnterTaskControlLoop,
    LPS22DFTask_vtblForceExecuteStep,
    LPS22DFTask_vtblOnEnterPowerMode
  },

  /* class::sensor_if_vtbl virtual table */
  {
    {
      {
        LPS22DFTask_vtblPressGetId,
        LPS22DFTask_vtblPressGetEventSourceIF,
        LPS22DFTask_vtblPressGetDataInfo
      },
      LPS22DFTask_vtblSensorEnable,
      LPS22DFTask_vtblSensorDisable,
      LPS22DFTask_vtblSensorIsEnabled,
      LPS22DFTask_vtblPressGetDescription,
      LPS22DFTask_vtblPressGetStatus,
      LPS22DFTask_vtblPressGetStatusPointer
    },
    LPS22DFTask_vtblPressGetODR,
    LPS22DFTask_vtblPressGetFS,
    LPS22DFTask_vtblPressGetSensitivity,
    LPS22DFTask_vtblSensorSetODR,
    LPS22DFTask_vtblSensorSetFS,
    LPS22DFTask_vtblSensorSetFifoWM
  },

  /* PRESSURE DESCRIPTOR */
  {
    "lps22df",
    COM_TYPE_PRESS
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    LPS22DFTaskExecuteStepState1,
    NULL,
    LPS22DFTaskExecuteStepDatalog,
  },

  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *LPS22DFTaskGetPressSensorIF(LPS22DFTask *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *LPS22DFTaskAlloc(const void *pIRQConfig, const void *pCSConfig)
{
  LPS22DFTask *p_new_obj = SysAlloc(sizeof(LPS22DFTask));

  if (p_new_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_new_obj->super);

    p_new_obj->super.vptr = &sTheClass.vtbl;
    p_new_obj->sensor_if.vptr = &sTheClass.sensor_if_vtbl;
    p_new_obj->sensor_descriptor = &sTheClass.class_descriptor;

    p_new_obj->pIRQConfig = (MX_GPIOParams_t *) pIRQConfig;
    p_new_obj->pCSConfig = (MX_GPIOParams_t *) pCSConfig;

    strcpy(p_new_obj->sensor_status.p_name, sTheClass.class_descriptor.p_name);
  }

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *LPS22DFTaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *p_name)
{
  LPS22DFTask *p_new_obj = (LPS22DFTask *)LPS22DFTaskAlloc(pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *LPS22DFTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig)
{
  LPS22DFTask *p_obj = (LPS22DFTask *)p_mem_block;

  if (p_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_obj->super);
    p_obj->super.vptr = &sTheClass.vtbl;

    p_obj->super.vptr = &sTheClass.vtbl;
    p_obj->sensor_if.vptr = &sTheClass.sensor_if_vtbl;
    p_obj->sensor_descriptor = &sTheClass.class_descriptor;

    p_obj->pIRQConfig = (MX_GPIOParams_t *) pIRQConfig;
    p_obj->pCSConfig = (MX_GPIOParams_t *) pCSConfig;
  }

  return (AManagedTaskEx *)p_obj;
}

AManagedTaskEx *LPS22DFTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                              const char *p_name)
{
  LPS22DFTask *p_obj = (LPS22DFTask *)LPS22DFTaskStaticAlloc(p_mem_block, pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *LPS22DFTaskGetSensorIF(LPS22DFTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *LPS22DFTaskGetPressEventSrcIF(LPS22DFTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *)_this->p_press_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t LPS22DFTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_obj = (LPS22DFTask *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
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
  LPS22DFTask *p_obj = (LPS22DFTask *) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) LPS22DF_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(LPS22DF_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "LPS22DF_Q", item_size / 4u, p_queue_items_buff,
                                    LPS22DF_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_fifo_timer, "LPS22DF_T", LPS22DFTaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(LPS22DF_TASK_CFG_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(LPS22DF_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(LPS22DF_ID, LPS22DF_TASK_CFG_I2C_ADDRESS, 0);
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

  /* Initialize the EventSrc interface */
  p_obj->p_press_event_src = DataEventSrcAlloc();
  if (p_obj->p_press_event_src == NULL)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }
  IEventSrcInit(p_obj->p_press_event_src);

  if (!MTMap_IsInitialized(&sTheClass.task_map))
  {
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, LPS22DF_TASK_CFG_MAX_INSTANCES_COUNT);
  }

  /* Add the managed task to the map.*/
  if (p_obj->pIRQConfig != NULL)
  {
    /* Use the PIN as unique key for the map. */
    MTMapElement_t *p_element = NULL;
    uint32_t key = (uint32_t) p_obj->pIRQConfig->pin;
    p_element = MTMap_AddElement(&sTheClass.task_map, key, _this);
    if (p_element == NULL)
    {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      return res;
    }
  }

  memset(p_obj->p_press_data_buff, 0, sizeof(p_obj->p_press_data_buff));
  p_obj->press_id = 0;
  p_obj->prev_timestamp = 0.0f;
  p_obj->fifo_level = 0;
  p_obj->samples_per_it = 0;
  p_obj->task_delay = 0;
  p_obj->first_data_ready = 0;
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
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = LPS22DFTaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22DF: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t LPS22DFTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_obj = (LPS22DFTask *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (LPS22DFTaskSensorIsActive(p_obj))
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

      // reset the variables for the actual odr computation.
      p_obj->prev_timestamp = 0.0f;
      p_obj->first_data_ready = 0;
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22DF: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      if (LPS22DFTaskSensorIsActive(p_obj))
      {
        /* Deactivate the sensor */
        lps22df_md_t val;
        lps22df_mode_get(p_sensor_drv, &val);
        val.odr = LPS22DF_ONE_SHOT;
        lps22df_mode_set(p_sensor_drv, &val);

        lps22df_fifo_md_t fifo_md;
        fifo_md.watermark = 1;
        fifo_md.operation = LPS22DF_BYPASS;
        lps22df_fifo_mode_set(p_sensor_drv, &fifo_md);
      }
      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
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
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = LPS22DFTaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22DF - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->pIRQConfig != NULL)
    {
      LPS22DFTaskConfigureIrqPin(p_obj, TRUE);
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

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("LPS22DF: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (LPS22DF_TASK_CFG_TAG)
  LPS22DFTask *p_obj = (LPS22DFTask *) _this;
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
  LPS22DFTask *p_obj = (LPS22DFTask *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = LPS22DFTaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t LPS22DFTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode)
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
  LPS22DFTask *p_if_owner = (LPS22DFTask *)((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
  uint8_t res = p_if_owner->press_id;

  return res;
}

IEventSrc *LPS22DFTask_vtblPressGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22DFTask *p_if_owner = (LPS22DFTask *)((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
  return p_if_owner->p_press_event_src;
}

sys_error_code_t LPS22DFTask_vtblPressGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LPS22DFTask *p_if_owner = (LPS22DFTask *)((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if ((p_measured == NULL) || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = p_if_owner->sensor_status.type.mems.measured_odr;
    *p_nominal = p_if_owner->sensor_status.type.mems.odr;
  }

  return res;
}

float_t LPS22DFTask_vtblPressGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LPS22DFTask *p_if_owner = (LPS22DFTask *)((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
  float_t res = p_if_owner->sensor_status.type.mems.fs;

  return res;
}

float_t LPS22DFTask_vtblPressGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LPS22DFTask *p_if_owner = (LPS22DFTask *)((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
  float_t res = p_if_owner->sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t LPS22DFTask_vtblPressGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22DFTask *p_if_owner = (LPS22DFTask *)((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t LPS22DFTask_vtblSensorSetODR(ISensorMems_t *_this, float_t odr)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_if_owner = (LPS22DFTask *)((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    if (odr > 0.0f)
    {
      p_if_owner->sensor_status.type.mems.odr = odr;
      p_if_owner->sensor_status.type.mems.measured_odr = 0.0f;
    }
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_ODR,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.fParam = (float_t) odr
    };
    res = LPS22DFTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t LPS22DFTask_vtblSensorSetFS(ISensorMems_t *_this, float_t fs)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_if_owner = (LPS22DFTask *)((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    if (fs == 4060.0f)
    {
      p_if_owner->sensor_status.type.mems.fs = fs;
    }
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FS,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.fParam = (float_t) fs
    };
    res = LPS22DFTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t LPS22DFTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

#if LPS22DF_FIFO_ENABLED
  LPS22DFTask *p_if_owner = (LPS22DFTask *)((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
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
      .sensorMessage.nParam = (uint16_t) fifoWM
    };
    res = LPS22DFTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }
#endif

  return res;
}

sys_error_code_t LPS22DFTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_if_owner = (LPS22DFTask *)((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
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
    res = LPS22DFTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t LPS22DFTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_if_owner = (LPS22DFTask *)((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
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
    res = LPS22DFTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t LPS22DFTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  LPS22DFTask *p_if_owner = (LPS22DFTask *)((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->press_id)
  {
    res = p_if_owner->sensor_status.is_active;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

SensorDescriptor_t LPS22DFTask_vtblPressGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LPS22DFTask *p_if_owner = (LPS22DFTask *)((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));
  return *p_if_owner->sensor_descriptor;
}

SensorStatus_t LPS22DFTask_vtblPressGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LPS22DFTask *p_if_owner = (LPS22DFTask *)((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));

  return p_if_owner->sensor_status;
}

SensorStatus_t *LPS22DFTask_vtblPressGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LPS22DFTask *p_if_owner = (LPS22DFTask *)((uint32_t) _this - offsetof(LPS22DFTask, sensor_if));

  return &p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t LPS22DFTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22DFTask *p_obj = (LPS22DFTask *) _this;
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
            res = LPS22DFTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = LPS22DFTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = LPS22DFTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = LPS22DFTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = LPS22DFTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22DF: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22DF: unexpected report in Run: %i\r\n", report.messageID));
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
  LPS22DFTask *p_obj = (LPS22DFTask *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("LPS22DF: new data.\r\n"));
        res = LPS22DFTaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          if (p_obj->first_data_ready == 1)
          {
#if LPS22DF_FIFO_ENABLED
            if (p_obj->fifo_level != 0)
            {
#endif
              // notify the listeners...
              double_t timestamp = report.sensorDataReadyMessage.fTimestamp;
              double_t delta_timestamp = timestamp - p_obj->prev_timestamp;
              p_obj->prev_timestamp = timestamp;

              /* update measuredODR */
              // Update the sums
              p_obj->delta_timestamp_sum += delta_timestamp;
              p_obj->samples_sum += p_obj->samples_per_it;
              if (p_obj->odr_count < MEAS_ODR_AVG)
              {
                p_obj->odr_count++;
              }
              // Calculate the average
              if (p_obj->odr_count == MEAS_ODR_AVG)
              {
                p_obj->sensor_status.type.mems.measured_odr = (float_t) p_obj->samples_sum / p_obj->delta_timestamp_sum;
                p_obj->delta_timestamp_sum = 0.0f;
                p_obj->samples_sum = 0;
                p_obj->odr_count = 0;
              }

              EMD_1dInit(&p_obj->data, (uint8_t *) &p_obj->p_press_data_buff[0], E_EM_FLOAT, p_obj->samples_per_it);

              DataEvent_t evt;

              DataEventInit((IEvent *) &evt, p_obj->p_press_event_src, &p_obj->data, timestamp, p_obj->press_id);
              IEventSrcSendEvent(p_obj->p_press_event_src, (IEvent *) &evt, NULL);
#if LPS22DF_FIFO_ENABLED
            }
#endif
          }
          else
          {
            p_obj->first_data_ready = 1;
          }
        }
        break;
      }
      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
            res = LPS22DFTaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->sensor_status.is_active == true)
              {
                if (p_obj->pIRQConfig == NULL)
                {
                  if (TX_SUCCESS != tx_timer_change(&p_obj->read_fifo_timer, AMT_MS_TO_TICKS(p_obj->task_delay),
                                                    AMT_MS_TO_TICKS(p_obj->task_delay)))
                  {
                    res = SYS_UNDEFINED_ERROR_CODE;
                  }
                  if (TX_SUCCESS != tx_timer_activate(&p_obj->read_fifo_timer))
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
            res = LPS22DFTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = LPS22DFTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = LPS22DFTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = LPS22DFTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = LPS22DFTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22DF: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22DF: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t LPS22DFTaskPostReportToFront(LPS22DFTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t LPS22DFTaskPostReportToBack(LPS22DFTask *_this, SMMessage *pReport)
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

static sys_error_code_t LPS22DFTaskSensorInit(LPS22DFTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  int32_t ret_val = 0;
  lps22df_pin_int_route_t int_route;

  /* Set bdu and if_inc recommended for driver usage */
  lps22df_init_set(p_sensor_drv, LPS22DF_DRV_RDY);

  /* Select bus interface */
  lps22df_bus_mode_t bus_mode;
  bus_mode.interface = LPS22DF_SEL_BY_HW;
  bus_mode.filter = LPS22DF_FILTER_AUTO;
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
  if (!ret_val)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, reg0.whoami);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22DF: sensor - I am 0x%x.\r\n", reg0.whoami));

#if LPS22DF_FIFO_ENABLED

  if (_this->samples_per_it == 0)
  {
    /* Set fifo mode */
    uint16_t lps22df_wtm_level = 0;

    /* Calculation of watermark and samples per int*/
    lps22df_wtm_level = ((uint16_t) _this->sensor_status.type.mems.odr * (uint16_t) LPS22DF_MAX_DRDY_PERIOD);
    if (lps22df_wtm_level > LPS22DF_MAX_WTM_LEVEL)
    {
      lps22df_wtm_level = LPS22DF_MAX_WTM_LEVEL;
    }

    _this->samples_per_it = lps22df_wtm_level;
  }

  lps22df_fifo_md_t fifo_md;
  fifo_md.watermark = _this->samples_per_it;
  fifo_md.operation = LPS22DF_STREAM;
  lps22df_fifo_mode_set(p_sensor_drv, &fifo_md);
#else
  _this->samples_per_it = 1;
#endif /* LPS22DF_FIFO_ENABLED */

  /* Set odr and fs */
  lps22df_mode_get(p_sensor_drv, &md);

  float_t lps22df_odr;
  lps22df_odr = _this->sensor_status.type.mems.odr;
  if (lps22df_odr < 2.0f)
  {
    md.odr = LPS22DF_1Hz;
  }
  else if (lps22df_odr < 5.0f)
  {
    md.odr = LPS22DF_4Hz;
  }
  else if (lps22df_odr < 11.0f)
  {
    md.odr = LPS22DF_10Hz;
  }
  else if (lps22df_odr < 26.0f)
  {
    md.odr = LPS22DF_25Hz;
  }
  else if (lps22df_odr < 51.0f)
  {
    md.odr = LPS22DF_50Hz;
  }
  else if (lps22df_odr < 76.0f)
  {
    md.odr = LPS22DF_75Hz;
  }
  else if (lps22df_odr < 101.0f)
  {
    md.odr = LPS22DF_100Hz;
  }
  else
  {
    md.odr = LPS22DF_200Hz;
  }


  if (_this->sensor_status.is_active)
  {
    lps22df_mode_set(p_sensor_drv, &md);
  }
  else
  {
    md.odr = LPS22DF_ONE_SHOT;
    lps22df_mode_set(p_sensor_drv, &md);
    _this->sensor_status.is_active = false;
  }

  if (_this->pIRQConfig != NULL)
  {
    lps22df_pin_int_route_get(p_sensor_drv, &int_route);
#if LPS22DF_FIFO_ENABLED
    int_route.fifo_th   = PROPERTY_ENABLE;
#else
    int_route.drdy_pres   = PROPERTY_ENABLE;
#endif
    lps22df_pin_int_route_set(p_sensor_drv, &int_route);
  }

//  lps22df_fifo_level_get(p_sensor_drv, &_this->fifo_level);
//
//  if (_this->fifo_level >= _this->samples_per_it)
//  {
//    lps22df_read_reg(p_sensor_drv, LPS22DF_FIFO_DATA_OUT_PRESS_XL, (uint8_t *) _this->p_fifo_data_buff,
//                     _this->samples_per_it * 3);
//  }

  if (_this->sensor_status.is_active)
  {
#if LPS22DF_FIFO_ENABLED
    _this->task_delay = (uint16_t)((1000.0f / lps22df_odr) * (((float_t)(_this->samples_per_it)) / 2.0f));
#else
    _this->task_delay = (uint16_t)(1000.0f / lps22df_odr);
#endif
  }

  _this->odr_count = 0;
  _this->delta_timestamp_sum = 0.0f;
  _this->samples_sum = 0;
  return res;
}

static sys_error_code_t LPS22DFTaskSensorReadData(LPS22DFTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t samples_per_it = _this->samples_per_it;
  uint8_t i = 0;

#if LPS22DF_FIFO_ENABLED
  /* Check FIFO_WTM_IA and fifo level */
  lps22df_fifo_level_get(p_sensor_drv, &_this->fifo_level);

  if (_this->fifo_level >= samples_per_it)
  {
    res = lps22df_read_reg(p_sensor_drv, LPS22DF_FIFO_DATA_OUT_PRESS_XL, (uint8_t *) _this->p_fifo_data_buff,
                           samples_per_it * 3);
  }
  else
  {
    _this->fifo_level = 0;
  }
#else
  res = lps22df_read_reg(p_sensor_drv, LPS22DF_PRESS_OUT_XL, (uint8_t *) _this->p_fifo_data_buff, samples_per_it * 3);
  _this->fifo_level = 1;
#endif /* LPS22DF_FIFO_ENABLED */

  if (!SYS_IS_ERROR_CODE(res))
  {
    if (_this->fifo_level >= samples_per_it)
    {
#if (HSD_USE_DUMMY_DATA == 1)
      for (i = 0; i < samples_per_it ; i++)
      {
        _this->p_press_data_buff[i]  = (float_t)(dummyDataCounter_press++);
      }
    }
#else
      /* Arrange Data */
      int32_t data;
      uint8_t *p8_src = _this->p_fifo_data_buff;

      for (i = 0; i < samples_per_it; i++)
      {
        /* Pressure data conversion to Int32 */
        data = (int32_t) p8_src[2];
        data = (data * 256) + (int32_t) p8_src[1];
        data = (data * 256) + (int32_t) p8_src[0];
        data = (data * 256);
        p8_src = p8_src + 3;

        _this->p_press_data_buff[i] = lps22df_from_lsb_to_hPa(data);
      }
    }
#endif /* HSD_USE_DUMMY_DATA */
  }
  return res;
}

static sys_error_code_t LPS22DFTaskSensorRegister(LPS22DFTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *press_if = (ISensor_t *) LPS22DFTaskGetPressSensorIF(_this);

  _this->press_id = SMAddSensor(press_if);

  return res;
}

static sys_error_code_t LPS22DFTaskSensorInitTaskParams(LPS22DFTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* PRESSURE STATUS */
  _this->sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->sensor_status.is_active = TRUE;
  _this->sensor_status.type.mems.fs = 4060.0f;
  _this->sensor_status.type.mems.sensitivity = 1.0f;
  _this->sensor_status.type.mems.odr = 200.0f;
  _this->sensor_status.type.mems.measured_odr = 0.0f;
  EMD_1dInit(&_this->data, (uint8_t *) &_this->p_press_data_buff[0], E_EM_FLOAT, 1);

  return res;
}

static sys_error_code_t LPS22DFTaskSensorSetODR(LPS22DFTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  float_t odr = (float_t) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  lps22df_md_t md;
  lps22df_mode_get(p_sensor_drv, &md);

  if (id == _this->press_id)
  {
    if (odr < 1.0f)
    {
      md.odr = LPS22DF_ONE_SHOT;
      lps22df_mode_set(p_sensor_drv, &md);
      /* Do not update the model in case of odr = 0 */
      odr = _this->sensor_status.type.mems.odr;
    }
    else if (odr < 2.0f)
    {
      odr = 1;
    }
    else if (odr < 5.0f)
    {
      odr = 4;
    }
    else if (odr < 11.0f)
    {
      odr = 10;
    }
    else if (odr < 26.0f)
    {
      odr = 25;
    }
    else if (odr < 51.0f)
    {
      odr = 50;
    }
    else if (odr < 76.0f)
    {
      odr = 75;
    }
    else if (odr < 101.0f)
    {
      odr = 100;
    }
    else
    {
      odr = 200;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      if (id == _this->press_id)
      {
        _this->sensor_status.type.mems.odr = odr;
        _this->sensor_status.type.mems.measured_odr = 0.0f;
      }
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t LPS22DFTaskSensorSetFS(LPS22DFTask *_this, SMMessage report)
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

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t lps22df_wtm_level = (uint16_t)report.sensorMessage.nParam;

  if (lps22df_wtm_level > LPS22DF_MAX_WTM_LEVEL)
  {
    lps22df_wtm_level = LPS22DF_MAX_WTM_LEVEL;
  }
  _this->samples_per_it = lps22df_wtm_level;

  fifo_md.watermark = _this->samples_per_it;
  fifo_md.operation = LPS22DF_STREAM;

  lps22df_fifo_mode_set(p_sensor_drv, &fifo_md);

  return res;
}

static sys_error_code_t LPS22DFTaskSensorEnable(LPS22DFTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->press_id)
  {
    _this->sensor_status.is_active = TRUE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t LPS22DFTaskSensorDisable(LPS22DFTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  lps22df_md_t md;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->press_id)
  {
    _this->sensor_status.is_active = FALSE;
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
  return _this->sensor_status.is_active;
}

static sys_error_code_t LPS22DFTaskEnterLowPowerMode(const LPS22DFTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  lps22df_md_t val;
  lps22df_mode_get(p_sensor_drv, &val);
  val.odr = LPS22DF_ONE_SHOT;

  if (lps22df_mode_set(p_sensor_drv, &val))
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

  if (!LowPower)
  {
    /* Configure INT Pin */
    _this->pIRQConfig->p_mx_init_f();
  }
  else
  {
    GPIO_InitTypeDef GPIO_InitStruct =
    {
      0
    };

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

static void LPS22DFTaskTimerCallbackFunction(ULONG param)
{
  LPS22DFTask *p_obj = (LPS22DFTask *) param;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  // if (sTaskObj.in_queue != NULL ) {//TODO: STF.Port - how to check if the queue has been initialized ??
  if (TX_SUCCESS != tx_queue_send(&p_obj->in_queue, &report, TX_NO_WAIT))
  {
    // unable to send the message. Signal the error
    sys_error_handler();
  }
  //}
}

/* CubeMX integration */

void LPS22DFTask_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((LPS22DFTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}
