/**
  ******************************************************************************
  * @file    ILPS22QSTask.c
  * @author  SRA - MCD
  * @brief   This file provides a set of functions to handle ilps22qs sensor
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
#include "ILPS22QSTask.h"
#include "ILPS22QSTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "ilps22qs_reg.h"
#include <string.h>
#include "services/sysdebug.h"

/* Private includes ----------------------------------------------------------*/

#ifndef ILPS22QS_TASK_CFG_STACK_DEPTH
#define ILPS22QS_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*10)
#endif

#ifndef ILPS22QS_TASK_CFG_PRIORITY
#define ILPS22QS_TASK_CFG_PRIORITY                 4
#endif

#ifndef ILPS22QS_TASK_CFG_IN_QUEUE_LENGTH
#define ILPS22QS_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define ILPS22QS_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef ILPS22QS_TASK_CFG_TIMER_PERIOD_MS
#define ILPS22QS_TASK_CFG_TIMER_PERIOD_MS          500
#endif

#ifndef ILPS22QS_TASK_CFG_MAX_INSTANCES_COUNT
#define ILPS22QS_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_ILPS22QS, level, message)

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter_press = 0;
#endif

/**
  * Class object declaration
 */
typedef struct _ILPS22QSTaskClass
{
  /**
    * ILPS22QSTask class virtual table.
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
    * ILPS22QSTask (PM_STATE, ExecuteStepFunc) map.
   */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
      * Memory buffer used to allocate the map (key, value).
   */
  MTMapElement_t task_map_elements[ILPS22QS_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} ILPS22QSTaskClass_t;


/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t ILPS22QSTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t ILPS22QSTaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t ILPS22QSTaskSensorInit(ILPS22QSTask *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t ILPS22QSTaskSensorReadData(ILPS22QSTask *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t ILPS22QSTaskSensorRegister(ILPS22QSTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t ILPS22QSTaskSensorInitTaskParams(ILPS22QSTask *_this);

/**
  * Private implementation of sensor interface methods for ILPS22QS sensor
  */
static sys_error_code_t ILPS22QSTaskSensorSetODR(ILPS22QSTask *_this, SMMessage message);
static sys_error_code_t ILPS22QSTaskSensorSetFS(ILPS22QSTask *_this, SMMessage message);
static sys_error_code_t ILPS22QSTaskSensorSetFifoWM(ILPS22QSTask *_this, SMMessage report);
static sys_error_code_t ILPS22QSTaskSensorEnable(ILPS22QSTask *_this, SMMessage message);
static sys_error_code_t ILPS22QSTaskSensorDisable(ILPS22QSTask *_this, SMMessage message);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t ILPS22QSTaskSensorIsActive(const ILPS22QSTask *_this);

static sys_error_code_t ILPS22QSTaskEnterLowPowerMode(const ILPS22QSTask *_this);

static sys_error_code_t ILPS22QSTaskConfigureIrqPin(const ILPS22QSTask *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void ILPS22QSTaskTimerCallbackFunction(ULONG param);


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
static inline sys_error_code_t ILPS22QSTaskPostReportToFront(ILPS22QSTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t ILPS22QSTaskPostReportToBack(ILPS22QSTask *_this, SMMessage *pReport);


/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static ILPS22QSTask sTaskObj;

/**
  * The class object.
  */
static ILPS22QSTaskClass_t sTheClass =
{
  /* class virtual table */
  {
    ILPS22QSTask_vtblHardwareInit,
    ILPS22QSTask_vtblOnCreateTask,
    ILPS22QSTask_vtblDoEnterPowerMode,
    ILPS22QSTask_vtblHandleError,
    ILPS22QSTask_vtblOnEnterTaskControlLoop,
    ILPS22QSTask_vtblForceExecuteStep,
    ILPS22QSTask_vtblOnEnterPowerMode
  },

  /* class::sensor_if_vtbl virtual table */
  {
    {
      {
        ILPS22QSTask_vtblPressGetId,
        ILPS22QSTask_vtblPressGetEventSourceIF,
        ILPS22QSTask_vtblPressGetDataInfo
      },
      ILPS22QSTask_vtblSensorEnable,
      ILPS22QSTask_vtblSensorDisable,
      ILPS22QSTask_vtblSensorIsEnabled,
      ILPS22QSTask_vtblPressGetDescription,
      ILPS22QSTask_vtblPressGetStatus,
      ILPS22QSTask_vtblPressGetStatusPointer
    },
    ILPS22QSTask_vtblPressGetODR,
    ILPS22QSTask_vtblPressGetFS,
    ILPS22QSTask_vtblPressGetSensitivity,
    ILPS22QSTask_vtblSensorSetODR,
    ILPS22QSTask_vtblSensorSetFS,
    ILPS22QSTask_vtblSensorSetFifoWM
  },

  /* PRESSURE DESCRIPTOR */
  {
    "ilps22qs",
    COM_TYPE_PRESS
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    ILPS22QSTaskExecuteStepState1,
    NULL,
    ILPS22QSTaskExecuteStepDatalog,
  },

  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *ILPS22QSTaskGetPressSensorIF(ILPS22QSTask *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *ILPS22QSTaskAlloc(const void *pIRQConfig, const void *pCSConfig)
{
  ILPS22QSTask *p_new_obj = SysAlloc(sizeof(ILPS22QSTask));

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

AManagedTaskEx *ILPS22QSTaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *p_name)
{
  ILPS22QSTask *p_new_obj = (ILPS22QSTask *)ILPS22QSTaskAlloc(pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *ILPS22QSTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig)
{
  ILPS22QSTask *p_obj = (ILPS22QSTask *)p_mem_block;

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

AManagedTaskEx *ILPS22QSTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                               const char *p_name)
{
  ILPS22QSTask *p_obj = (ILPS22QSTask *)ILPS22QSTaskStaticAlloc(p_mem_block, pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *ILPS22QSTaskGetSensorIF(ILPS22QSTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *ILPS22QSTaskGetPressEventSrcIF(ILPS22QSTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *)_this->p_press_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t ILPS22QSTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ILPS22QSTask *p_obj = (ILPS22QSTask *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t ILPS22QSTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                               VOID **pvStackStart,
                                               ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                               ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ILPS22QSTask *p_obj = (ILPS22QSTask *) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) ILPS22QS_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(ILPS22QS_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "ILPS22QS_Q", item_size / 4u, p_queue_items_buff,
                                    ILPS22QS_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_fifo_timer, "ILPS22WS_T", ILPS22QSTaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(ILPS22QS_TASK_CFG_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(ILPS22QS_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(ILPS22QS_ID, ILPS22QS_I2C_ADD, 0);
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
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, ILPS22QS_TASK_CFG_MAX_INSTANCES_COUNT);
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
  *pName = "ILPS22QS";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = ILPS22QS_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = ILPS22QS_TASK_CFG_PRIORITY;
  *pPreemptThreshold = ILPS22QS_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = ILPS22QSTaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = ILPS22QSTaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ILPS22QS: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t ILPS22QSTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ILPS22QSTask *p_obj = (ILPS22QSTask *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (ILPS22QSTaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ILPS22QS: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      if (ILPS22QSTaskSensorIsActive(p_obj))
      {
        /* Deactivate the sensor */
        ilps22qs_md_t val;
        ilps22qs_mode_get(p_sensor_drv, &val);
        val.odr = ILPS22QS_ONE_SHOT;
        ilps22qs_mode_set(p_sensor_drv, &val);

        ilps22qs_fifo_md_t fifo_md;
        fifo_md.watermark = 1;
        fifo_md.operation = ILPS22QS_BYPASS;
        ilps22qs_fifo_mode_set(p_sensor_drv, &fifo_md);
      }
      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_fifo_timer);
      }
      else
      {
        ILPS22QSTaskConfigureIrqPin(p_obj, TRUE);
      }
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ILPS22QS: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = ILPS22QSTaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("ILPS22QS - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->pIRQConfig != NULL)
    {
      ILPS22QSTaskConfigureIrqPin(p_obj, TRUE);
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ILPS22QS: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t ILPS22QSTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  ILPS22QSTask *p_obj = (ILPS22QSTask*)_this;

  return res;
}

sys_error_code_t ILPS22QSTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("ILPS22QS: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (ILPS22QS_TASK_CFG_TAG)
  ILPS22QSTask *p_obj = (ILPS22QSTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = ILPS22QS_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t ILPS22QSTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ILPS22QSTask *p_obj = (ILPS22QSTask *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = ILPS22QSTaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t ILPS22QSTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  ILPS22QSTask *p_obj = (ILPS22QSTask*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t ILPS22QSTask_vtblPressGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  ILPS22QSTask *p_if_owner = (ILPS22QSTask *)((uint32_t) _this - offsetof(ILPS22QSTask, sensor_if));
  uint8_t res = p_if_owner->press_id;

  return res;
}

IEventSrc *ILPS22QSTask_vtblPressGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  ILPS22QSTask *p_if_owner = (ILPS22QSTask *)((uint32_t) _this - offsetof(ILPS22QSTask, sensor_if));

  return p_if_owner->p_press_event_src;
}

sys_error_code_t ILPS22QSTask_vtblPressGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  ILPS22QSTask *p_if_owner = (ILPS22QSTask *)((uint32_t) _this - offsetof(ILPS22QSTask, sensor_if));
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

float_t ILPS22QSTask_vtblPressGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  ILPS22QSTask *p_if_owner = (ILPS22QSTask *)((uint32_t) _this - offsetof(ILPS22QSTask, sensor_if));
  float_t res = p_if_owner->sensor_status.type.mems.fs;

  return res;
}

float_t ILPS22QSTask_vtblPressGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  ILPS22QSTask *p_if_owner = (ILPS22QSTask *)((uint32_t) _this - offsetof(ILPS22QSTask, sensor_if));
  float_t res = p_if_owner->sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t ILPS22QSTask_vtblPressGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  ILPS22QSTask *p_if_owner = (ILPS22QSTask *)((uint32_t) _this - offsetof(ILPS22QSTask, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t ILPS22QSTask_vtblSensorSetODR(ISensorMems_t *_this, float_t odr)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ILPS22QSTask *p_if_owner = (ILPS22QSTask *)((uint32_t) _this - offsetof(ILPS22QSTask, sensor_if));
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
    res = ILPS22QSTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t ILPS22QSTask_vtblSensorSetFS(ISensorMems_t *_this, float_t fs)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ILPS22QSTask *p_if_owner = (ILPS22QSTask *)((uint32_t) _this - offsetof(ILPS22QSTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.mems.fs = fs;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FS,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.fParam = (float_t) fs
    };
    res = ILPS22QSTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t ILPS22QSTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

#if ILPS22QS_FIFO_ENABLED
  ILPS22QSTask *p_if_owner = (ILPS22QSTask *)((uint32_t) _this - offsetof(ILPS22QSTask, sensor_if));
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
    res = ILPS22QSTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }
#endif

  return res;
}

sys_error_code_t ILPS22QSTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ILPS22QSTask *p_if_owner = (ILPS22QSTask *)((uint32_t) _this - offsetof(ILPS22QSTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
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
    res = ILPS22QSTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t ILPS22QSTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ILPS22QSTask *p_if_owner = (ILPS22QSTask *)((uint32_t) _this - offsetof(ILPS22QSTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
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
    res = ILPS22QSTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t ILPS22QSTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  ILPS22QSTask *p_if_owner = (ILPS22QSTask *)((uint32_t) _this - offsetof(ILPS22QSTask, sensor_if));

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

SensorDescriptor_t ILPS22QSTask_vtblPressGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  ILPS22QSTask *p_if_owner = (ILPS22QSTask *)((uint32_t) _this - offsetof(ILPS22QSTask, sensor_if));
  return *p_if_owner->sensor_descriptor;
}

SensorStatus_t ILPS22QSTask_vtblPressGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  ILPS22QSTask *p_if_owner = (ILPS22QSTask *)((uint32_t) _this - offsetof(ILPS22QSTask, sensor_if));

  return p_if_owner->sensor_status;
}

SensorStatus_t *ILPS22QSTask_vtblPressGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  ILPS22QSTask *p_if_owner = (ILPS22QSTask *)((uint32_t) _this - offsetof(ILPS22QSTask, sensor_if));

  return &p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t ILPS22QSTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ILPS22QSTask *p_obj = (ILPS22QSTask *) _this;
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
            res = ILPS22QSTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = ILPS22QSTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = ILPS22QSTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = ILPS22QSTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = ILPS22QSTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("ILPS22QS: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("ILPS22QS: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t ILPS22QSTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ILPS22QSTask *p_obj = (ILPS22QSTask *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("ILPS22QS: new data.\r\n"));
        res = ILPS22QSTaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          if (p_obj->first_data_ready == 1)
          {
#if ILPS22QS_FIFO_ENABLED
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

              SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("ILPS22QS: ts = %f\r\n", (float_t)timestamp));
#if ILPS22QS_FIFO_ENABLED
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
            res = ILPS22QSTaskSensorInit(p_obj);
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
                  ILPS22QSTaskConfigureIrqPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = ILPS22QSTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = ILPS22QSTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = ILPS22QSTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = ILPS22QSTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = ILPS22QSTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("ILPS22QS: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("ILPS22QS: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t ILPS22QSTaskPostReportToFront(ILPS22QSTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t ILPS22QSTaskPostReportToBack(ILPS22QSTask *_this, SMMessage *pReport)
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

static sys_error_code_t ILPS22QSTaskSensorInit(ILPS22QSTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  int32_t ret_val = 0;

  /* Set bdu and if_inc recommended for driver usage */
  ilps22qs_init_set(p_sensor_drv, ILPS22QS_DRV_RDY);

  /* Select bus interface */
  ilps22qs_bus_mode_t bus_mode;
  bus_mode.interface = ILPS22QS_SEL_BY_HW;
  bus_mode.filter = ILPS22QS_FILTER_AUTO;
  ilps22qs_bus_mode_set(p_sensor_drv, &bus_mode);

  /* Set Output Data Rate in Power Down */
  /* Configure basic parameters */
  ilps22qs_md_t md;
  ilps22qs_mode_get(p_sensor_drv, &md);
  md.odr = ILPS22QS_ONE_SHOT;
  md.avg = ILPS22QS_4_AVG;
  md.lpf = ILPS22QS_LPF_ODR_DIV_4;
  md.fs = ILPS22QS_1260hPa;
  ilps22qs_mode_set(p_sensor_drv, &md);

  /* Read sensor id */
  ilps22qs_id_t reg0;

  ret_val = ilps22qs_id_get(p_sensor_drv, &reg0);
  if (!ret_val)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, reg0.whoami);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ILPS22QS: sensor - I am 0x%x.\r\n", reg0.whoami));

#if ILPS22QS_FIFO_ENABLED

  if (_this->samples_per_it == 0)
  {
    /* Set fifo mode */
    uint16_t ilps22qs_wtm_level = 0;

    /* Calculation of watermark and samples per int*/
    ilps22qs_wtm_level = ((uint16_t) _this->sensor_status.type.mems.odr * (uint16_t) ILPS22QS_MAX_DRDY_PERIOD);
    if (ilps22qs_wtm_level > ILPS22QS_MAX_WTM_LEVEL)
    {
      ilps22qs_wtm_level = ILPS22QS_MAX_WTM_LEVEL;
    }

    _this->samples_per_it = ilps22qs_wtm_level;
  }

  ilps22qs_fifo_md_t fifo_md;
  fifo_md.watermark = _this->samples_per_it;
  fifo_md.operation = ILPS22QS_STREAM;
  ilps22qs_fifo_mode_set(p_sensor_drv, &fifo_md);
#else
  _this->samples_per_it = 1;
#endif /* ILPS22QS_FIFO_ENABLED */

  /* Set odr and fs */
  ilps22qs_mode_get(p_sensor_drv, &md);

  /* Currently, the INT pin is not exposed */
  if (_this->pIRQConfig != NULL)
  {
    /* you should enable the INT here */
  }

  float_t ilps22qs_odr;
  ilps22qs_odr = _this->sensor_status.type.mems.odr;
  if (ilps22qs_odr < 2.0f)
  {
    md.odr = ILPS22QS_1Hz;
  }
  else if (ilps22qs_odr < 5.0f)
  {
    md.odr = ILPS22QS_4Hz;
  }
  else if (ilps22qs_odr < 11.0f)
  {
    md.odr = ILPS22QS_10Hz;
  }
  else if (ilps22qs_odr < 26.0f)
  {
    md.odr = ILPS22QS_25Hz;
  }
  else if (ilps22qs_odr < 51.0f)
  {
    md.odr = ILPS22QS_50Hz;
  }
  else if (ilps22qs_odr < 76.0f)
  {
    md.odr = ILPS22QS_75Hz;
  }
  else if (ilps22qs_odr < 101.0f)
  {
    md.odr = ILPS22QS_100Hz;
  }
  else
  {
    md.odr = ILPS22QS_200Hz;
  }

  float_t ilps22qs_fs;
  ilps22qs_fs = _this->sensor_status.type.mems.fs;
  if (ilps22qs_fs < 1261.0f)
  {
    md.fs = ILPS22QS_1260hPa;
  }
  else
  {
    md.fs = ILPS22QS_4060hPa;
  }

  if (_this->sensor_status.is_active)
  {
    ilps22qs_mode_set(p_sensor_drv, &md);
  }
  else
  {
    md.odr = ILPS22QS_ONE_SHOT;
    ilps22qs_mode_set(p_sensor_drv, &md);
    _this->sensor_status.is_active = false;
  }

  ilps22qs_fifo_level_get(p_sensor_drv, &_this->fifo_level);

  if (_this->fifo_level >= _this->samples_per_it)
  {
    ilps22qs_read_reg(p_sensor_drv, ILPS22QS_FIFO_DATA_OUT_PRESS_XL, (uint8_t *) _this->p_fifo_data_buff,
                      _this->samples_per_it * 3);
  }

  if (_this->sensor_status.is_active)
  {
#if ILPS22QS_FIFO_ENABLED
    _this->task_delay = (uint16_t)((1000.0f / ilps22qs_odr) * (((float_t)(_this->samples_per_it)) / 2.0f));
#else
    _this->task_delay = (uint16_t)(1000.0f / ilps22qs_odr);
#endif
  }

  _this->odr_count = 0;
  _this->delta_timestamp_sum = 0.0f;
  _this->samples_sum = 0;
  return res;
}

static sys_error_code_t ILPS22QSTaskSensorReadData(ILPS22QSTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t samples_per_it = _this->samples_per_it;
  uint8_t i = 0;

#if ILPS22QS_FIFO_ENABLED
  /* Check FIFO_WTM_IA and fifo level */
  ilps22qs_fifo_level_get(p_sensor_drv, &_this->fifo_level);

  if (_this->fifo_level >= samples_per_it)
  {
    res = ilps22qs_read_reg(p_sensor_drv, ILPS22QS_FIFO_DATA_OUT_PRESS_XL, (uint8_t *) _this->p_fifo_data_buff,
                            samples_per_it * 3);
  }
  else
  {
    _this->fifo_level = 0;
  }
#else
  res = ilps22qs_read_reg(p_sensor_drv, ILPS22QS_PRESS_OUT_XL, (uint8_t *) _this->p_fifo_data_buff, samples_per_it * 3);
  _this->fifo_level = 1;
#endif /* ILPS22QS_FIFO_ENABLED */

  if (!SYS_IS_ERROR_CODE(res))
  {
    if (_this->fifo_level >= samples_per_it)
    {
#if (HSD_USE_DUMMY_DATA == 1)
      for (i = 0; i < samples_per_it ; i++)
      {
        _this->p_press_data_buff[i]  = (float_t)(dummyDataCounter_press++);
      }
#else
      /* Arrange Data */
      int32_t data;
      uint8_t *p8_src = _this->p_fifo_data_buff;
      float_t fs = _this->sensor_status.type.mems.fs;

      for (i = 0; i < samples_per_it; i++)
      {
        /* Pressure data conversion to Int32 */
        data = (int32_t) p8_src[2];
        data = (data * 256) + (int32_t) p8_src[1];
        data = (data * 256) + (int32_t) p8_src[0];
        data = (data * 256);
        p8_src = p8_src + 3;

        if (fs <= 1261.0f)
        {
          _this->p_press_data_buff[i] = ilps22qs_from_fs1260_to_hPa(data);
        }
        else
        {
          _this->p_press_data_buff[i] = ilps22qs_from_fs4000_to_hPa(data);
        }
      }
#endif
    }
  }

  return res;
}

static sys_error_code_t ILPS22QSTaskSensorRegister(ILPS22QSTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *press_if = (ISensor_t *) ILPS22QSTaskGetPressSensorIF(_this);

  _this->press_id = SMAddSensor(press_if);

  return res;
}

static sys_error_code_t ILPS22QSTaskSensorInitTaskParams(ILPS22QSTask *_this)
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

static sys_error_code_t ILPS22QSTaskSensorSetODR(ILPS22QSTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  float_t odr = (float_t) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  ilps22qs_md_t md;
  ilps22qs_mode_get(p_sensor_drv, &md);

  if (id == _this->press_id)
  {
    if (odr < 1.0f)
    {
      md.odr = ILPS22QS_ONE_SHOT;
      ilps22qs_mode_set(p_sensor_drv, &md);
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
      _this->sensor_status.type.mems.odr = odr;
      _this->sensor_status.type.mems.measured_odr = 0.0f;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t ILPS22QSTaskSensorSetFS(ILPS22QSTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  float_t fs = (float_t) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->press_id)
  {
    if (fs <= 1261.0f)
    {
      fs = 1260.0f;
    }
    else
    {
      fs = 4060.0f;
    }

    _this->sensor_status.type.mems.fs = fs;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t ILPS22QSTaskSensorSetFifoWM(ILPS22QSTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ilps22qs_fifo_md_t fifo_md;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t ilps22qs_wtm_level = (uint16_t)report.sensorMessage.nParam;

  /* Calculation of watermark and samples per int*/
  if (ilps22qs_wtm_level > ILPS22QS_MAX_WTM_LEVEL)
  {
    ilps22qs_wtm_level = ILPS22QS_MAX_WTM_LEVEL;
  }
  _this->samples_per_it = ilps22qs_wtm_level;

  fifo_md.watermark = _this->samples_per_it;
  fifo_md.operation = ILPS22QS_STREAM;

  ilps22qs_fifo_mode_set(p_sensor_drv, &fifo_md);

  return res;
}

static sys_error_code_t ILPS22QSTaskSensorEnable(ILPS22QSTask *_this, SMMessage report)
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

static sys_error_code_t ILPS22QSTaskSensorDisable(ILPS22QSTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  ilps22qs_md_t md;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->press_id)
  {
    _this->sensor_status.is_active = FALSE;
    md.odr = ILPS22QS_ONE_SHOT;
    ilps22qs_mode_set(p_sensor_drv, &md);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t ILPS22QSTaskSensorIsActive(const ILPS22QSTask *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.is_active;
}

static sys_error_code_t ILPS22QSTaskEnterLowPowerMode(const ILPS22QSTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  ilps22qs_md_t val;
  ilps22qs_mode_get(p_sensor_drv, &val);
  val.odr = ILPS22QS_ONE_SHOT;

  if (ilps22qs_mode_set(p_sensor_drv, &val))
  {
    res = SYS_SENSOR_TASK_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_OP_ERROR_CODE);
  }

  return res;
}

static sys_error_code_t ILPS22QSTaskConfigureIrqPin(const ILPS22QSTask *_this, boolean_t LowPower)
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

static void ILPS22QSTaskTimerCallbackFunction(ULONG param)
{
  ILPS22QSTask *p_obj = (ILPS22QSTask *) param;
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

void ILPS22QSTask_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((ILPS22QSTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}
