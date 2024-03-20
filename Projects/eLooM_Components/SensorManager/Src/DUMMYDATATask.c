/**
  ******************************************************************************
  * @file    DUMMYSENSORTask.c
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

/* Includes ------------------------------------------------------------------*/
#include "DUMMYSENSORTask.h"
#include "DUMMYSENSORTask_vtbl.h"
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

#ifndef DUMMYSENSOR_TASK_CFG_STACK_DEPTH
#define DUMMYSENSOR_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*8)
#endif

#ifndef DUMMYSENSOR_TASK_CFG_PRIORITY
#define DUMMYSENSOR_TASK_CFG_PRIORITY                 (4)
#endif

#ifndef DUMMYSENSOR_TASK_CFG_IN_QUEUE_LENGTH
#define DUMMYSENSOR_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define DUMMYSENSOR_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef DUMMYSENSOR_TASK_CFG_TIMER_PERIOD_MS
#define DUMMYSENSOR_TASK_CFG_TIMER_PERIOD_MS          500
#endif

#ifndef DUMMYSENSOR_TASK_CFG_MAX_INSTANCES_COUNT
#define DUMMYSENSOR_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_DUMMYSENSOR, level, message)

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter = 0;
#endif


/**
  * Class object declaration
  */
typedef struct _DUMMYSENSORTaskClass
{
  /**
    * DUMMYSENSORTask class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * Sensor IF virtual table.
    */
  const ISensor_vtbl sensor_if_vtbl;

  /**
    * Specifies  sensor capabilities.
    */
  const SensorDescriptor_t class_descriptor;

  /**
    * DUMMYSENSORTask (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[DUMMYSENSOR_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} DUMMYSENSORTaskClass_t;


/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t DUMMYSENSORTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t DUMMYSENSORTaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t DUMMYSENSORTaskSensorInit(DUMMYSENSORTask *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t DUMMYSENSORTaskSensorReadData(DUMMYSENSORTask *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t DUMMYSENSORTaskSensorRegister(DUMMYSENSORTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t DUMMYSENSORTaskSensorInitTaskParams(DUMMYSENSORTask *_this);

/**
  * Private implementation of sensor interface methods for DUMMYSENSOR sensor
  */
static sys_error_code_t DUMMYSENSORTaskSensorSetODR(DUMMYSENSORTask *_this, SMMessage report);
static sys_error_code_t DUMMYSENSORTaskSensorSetFS(DUMMYSENSORTask *_this, SMMessage report);
static sys_error_code_t DUMMYSENSORTaskSensorSetFifoWM(DUMMYSENSORTask *_this, SMMessage report);
static sys_error_code_t DUMMYSENSORTaskSensorEnable(DUMMYSENSORTask *_this, SMMessage report);
static sys_error_code_t DUMMYSENSORTaskSensorDisable(DUMMYSENSORTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t DUMMYSENSORTaskSensorIsActive(const DUMMYSENSORTask *_this);

static sys_error_code_t DUMMYSENSORTaskEnterLowPowerMode(const DUMMYSENSORTask *_this);

static sys_error_code_t DUMMYSENSORTaskConfigureIrqPin(const DUMMYSENSORTask *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void DUMMYSENSORTaskTimerCallbackFunction(ULONG param);


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
static inline sys_error_code_t DUMMYSENSORTaskPostReportToFront(DUMMYSENSORTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t DUMMYSENSORTaskPostReportToBack(DUMMYSENSORTask *_this, SMMessage *pReport);


/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static DUMMYSENSORTask sTaskObj;

/**
  * The class object.
  */
static DUMMYSENSORTaskClass_t sTheClass =
{
  /* class virtual table */
  {
    DUMMYSENSORTask_vtblHardwareInit,
    DUMMYSENSORTask_vtblOnCreateTask,
    DUMMYSENSORTask_vtblDoEnterPowerMode,
    DUMMYSENSORTask_vtblHandleError,
    DUMMYSENSORTask_vtblOnEnterTaskControlLoop,
    DUMMYSENSORTask_vtblForceExecuteStep,
    DUMMYSENSORTask_vtblOnEnterPowerMode
  },
  /* class::sensor_if_vtbl virtual table */
  {
    DUMMYSENSORTask_vtblAccGetId,
    DUMMYSENSORTask_vtblGetEventSourceIF,
    DUMMYSENSORTask_vtblAccGetDataInfo,
    DUMMYSENSORTask_vtblAccGetODR,
    DUMMYSENSORTask_vtblAccGetFS,
    DUMMYSENSORTask_vtblAccGetSensitivity,
    DUMMYSENSORTask_vtblSensorSetODR,
    DUMMYSENSORTask_vtblSensorSetFS,
    DUMMYSENSORTask_vtblSensorSetFifoWM,
    DUMMYSENSORTask_vtblSensorEnable,
    DUMMYSENSORTask_vtblSensorDisable,
    DUMMYSENSORTask_vtblSensorIsEnabled,
    DUMMYSENSORTask_vtblSensorGetDescription,
    DUMMYSENSORTask_vtblSensorGetStatus
  },
  /* ACCELEROMETER DESCRIPTOR */
  {
    "dummy_data",
    COM_TYPE_ACC,
    {
      1.6f,
      12.5f,
      25.0f,
      50.0f,
      100.0f,
      200.0f,
      400.0f,
      800.0f,
      1600.0f,
      COM_END_OF_LIST_FLOAT,
    },
    {
      2.0f,
      4.0f,
      8.0f,
      16.0f,
      COM_END_OF_LIST_FLOAT,
    },
    {
      "x",
      "y",
      "z",
    },
    "g",
    {
      0,
      1000,
    }
  },
  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    DUMMYSENSORTaskExecuteStepState1,
    NULL,
    DUMMYSENSORTaskExecuteStepDatalog,
  },

  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *DUMMYSENSORTaskGetAccSensorIF(DUMMYSENSORTask *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *DUMMYSENSORTaskAlloc(const void *pIRQConfig, const void *pCSConfig)
{
  DUMMYSENSORTask *p_new_obj = SysAlloc(sizeof(DUMMYSENSORTask));

  if (p_new_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_new_obj->super);

    p_new_obj->super.vptr = &sTheClass.vtbl;
    p_new_obj->sensor_if.vptr = &sTheClass.sensor_if_vtbl;
    p_new_obj->sensor_descriptor = &sTheClass.class_descriptor;

    p_new_obj->pIRQConfig = (MX_GPIOParams_t *) pIRQConfig;
    p_new_obj->pCSConfig = (MX_GPIOParams_t *) pCSConfig;

    strcpy(p_new_obj->sensor_status.Name, sTheClass.class_descriptor.Name);
  }

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *DUMMYSENSORTaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *Name)
{
  DUMMYSENSORTask *p_new_obj = (DUMMYSENSORTask *)DUMMYSENSORTaskAlloc(pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->sensor_status.Name, Name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *DUMMYSENSORTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig)
{
  DUMMYSENSORTask *p_obj = (DUMMYSENSORTask *)p_mem_block;

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

AManagedTaskEx *DUMMYSENSORTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                                  const char *Name)
{
  DUMMYSENSORTask *p_obj = (DUMMYSENSORTask *)DUMMYSENSORTaskStaticAlloc(p_mem_block, pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->sensor_status.Name, Name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *DUMMYSENSORTaskGetSensorIF(DUMMYSENSORTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *DUMMYSENSORTaskGetEventSrcIF(DUMMYSENSORTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t DUMMYSENSORTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DUMMYSENSORTask *p_obj = (DUMMYSENSORTask *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t DUMMYSENSORTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                                  VOID **pvStackStart,
                                                  ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                                  ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DUMMYSENSORTask *p_obj = (DUMMYSENSORTask *) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) DUMMYSENSOR_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(DUMMYSENSOR_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "DUMMYSENSOR_Q", item_size / 4u, p_queue_items_buff,
                                    DUMMYSENSOR_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_timer, "DUMMYSENSOR_T", DUMMYSENSORTaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(DUMMYSENSOR_TASK_CFG_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(DUMMYSENSOR_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(DUMMYSENSOR_ID, DUMMYSENSOR_I2C_ADD_H, 0);
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
  p_obj->p_event_src = DataEventSrcAlloc();
  if (p_obj->p_event_src == NULL)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }
  IEventSrcInit(p_obj->p_event_src);

  if (!MTMap_IsInitialized(&sTheClass.task_map))
  {
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, DUMMYSENSOR_TASK_CFG_MAX_INSTANCES_COUNT);
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

  memset(p_obj->p_sensor_data_buff, 0, sizeof(p_obj->p_sensor_data_buff));
  p_obj->acc_id = 0;
  p_obj->prev_timestamp = 0.0f;
  p_obj->fifo_level = 0;
  p_obj->samples_per_it = 0;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "DUMMYSENSOR";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = DUMMYSENSOR_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = DUMMYSENSOR_TASK_CFG_PRIORITY;
  *pPreemptThreshold = DUMMYSENSOR_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = DUMMYSENSORTaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = DUMMYSENSORTaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("DUMMYSENSOR: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t DUMMYSENSORTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                      const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DUMMYSENSORTask *p_obj = (DUMMYSENSORTask *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (DUMMYSENSORTaskSensorIsActive(p_obj))
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

      // reset the variables for the actual ODR computation.
      p_obj->prev_timestamp = 0.0f;
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("DUMMYSENSOR: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      /* Deactivate the sensor */
      dummy_data_data_rate_set(p_sensor_drv, DUMMYSENSOR_XL_ODR_OFF);
      dummy_data_fifo_mode_set(p_sensor_drv, DUMMYSENSOR_BYPASS_MODE);

      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_timer);
      }
      else
      {
        DUMMYSENSORTaskConfigureIrqPin(p_obj, TRUE);
      }
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("DUMMYSENSOR: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = DUMMYSENSORTaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("DUMMYSENSOR - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->pIRQConfig != NULL)
    {
      DUMMYSENSORTaskConfigureIrqPin(p_obj, TRUE);
    }
    /* notify the bus */
    if (p_obj->p_sensor_bus_if->m_pfBusCtrl != NULL)
    {
      p_obj->p_sensor_bus_if->m_pfBusCtrl(p_obj->p_sensor_bus_if, E_BUS_CTRL_DEV_NOTIFY_POWER_MODE, 0);
    }
    if (p_obj->pIRQConfig == NULL)
    {
      tx_timer_deactivate(&p_obj->read_timer);
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("DUMMYSENSOR: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t DUMMYSENSORTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  DUMMYSENSORTask *p_obj = (DUMMYSENSORTask*)_this;

  return res;
}

sys_error_code_t DUMMYSENSORTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("DUMMYSENSOR: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (DUMMYSENSOR_TASK_CFG_TAG)
  DUMMYSENSORTask *p_obj = (DUMMYSENSORTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = DUMMYSENSOR_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t DUMMYSENSORTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DUMMYSENSORTask *p_obj = (DUMMYSENSORTask *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = DUMMYSENSORTaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t DUMMYSENSORTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                      const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  DUMMYSENSORTask *p_obj = (DUMMYSENSORTask*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t DUMMYSENSORTask_vtblAccGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  DUMMYSENSORTask *p_if_owner = (DUMMYSENSORTask *)((uint32_t) _this - offsetof(DUMMYSENSORTask, sensor_if));
  uint8_t res = p_if_owner->acc_id;

  return res;
}

IEventSrc *DUMMYSENSORTask_vtblGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  DUMMYSENSORTask *p_if_owner = (DUMMYSENSORTask *)((uint32_t) _this - offsetof(DUMMYSENSORTask, sensor_if));
  return p_if_owner->p_event_src;
}

sys_error_code_t DUMMYSENSORTask_vtblAccGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  DUMMYSENSORTask *p_if_owner = (DUMMYSENSORTask *)((uint32_t) _this - offsetof(DUMMYSENSORTask, sensor_if));
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

float DUMMYSENSORTask_vtblAccGetFS(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  DUMMYSENSORTask *p_if_owner = (DUMMYSENSORTask *)((uint32_t) _this - offsetof(DUMMYSENSORTask, sensor_if));
  float res = p_if_owner->sensor_status.FS;

  return res;
}

float DUMMYSENSORTask_vtblAccGetSensitivity(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  DUMMYSENSORTask *p_if_owner = (DUMMYSENSORTask *)((uint32_t) _this - offsetof(DUMMYSENSORTask, sensor_if));
  float res = p_if_owner->sensor_status.Sensitivity;

  return res;
}

EMData_t DUMMYSENSORTask_vtblAccGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  DUMMYSENSORTask *p_if_owner = (DUMMYSENSORTask *)((uint32_t) _this - offsetof(DUMMYSENSORTask, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t DUMMYSENSORTask_vtblSensorSetODR(ISensor_t *_this, float ODR)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DUMMYSENSORTask *p_if_owner = (DUMMYSENSORTask *)((uint32_t) _this - offsetof(DUMMYSENSORTask, sensor_if));
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
    res = DUMMYSENSORTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t DUMMYSENSORTask_vtblSensorSetFS(ISensor_t *_this, float FS)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DUMMYSENSORTask *p_if_owner = (DUMMYSENSORTask *)((uint32_t) _this - offsetof(DUMMYSENSORTask, sensor_if));
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
    res = DUMMYSENSORTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t DUMMYSENSORTask_vtblSensorSetFifoWM(ISensor_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

#if DUMMYSENSOR_FIFO_ENABLED
  DUMMYSENSORTask *p_if_owner = (DUMMYSENSORTask *)((uint32_t) _this - offsetof(DUMMYSENSORTask, sensor_if));
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
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FIFO_WM,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint16_t) fifoWM
    };
    res = DUMMYSENSORTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }
#endif

  return res;
}

sys_error_code_t DUMMYSENSORTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DUMMYSENSORTask *p_if_owner = (DUMMYSENSORTask *)((uint32_t) _this - offsetof(DUMMYSENSORTask, sensor_if));
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
    res = DUMMYSENSORTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t DUMMYSENSORTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DUMMYSENSORTask *p_if_owner = (DUMMYSENSORTask *)((uint32_t) _this - offsetof(DUMMYSENSORTask, sensor_if));
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
    res = DUMMYSENSORTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t DUMMYSENSORTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  DUMMYSENSORTask *p_if_owner = (DUMMYSENSORTask *)((uint32_t) _this - offsetof(DUMMYSENSORTask, sensor_if));

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->acc_id)
  {
    res = p_if_owner->sensor_status.IsActive;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

SensorDescriptor_t DUMMYSENSORTask_vtblSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  DUMMYSENSORTask *p_if_owner = (DUMMYSENSORTask *)((uint32_t) _this - offsetof(DUMMYSENSORTask, sensor_if));
  return *p_if_owner->sensor_descriptor;
}

SensorStatus_t DUMMYSENSORTask_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  DUMMYSENSORTask *p_if_owner = (DUMMYSENSORTask *)((uint32_t) _this - offsetof(DUMMYSENSORTask, sensor_if));

  return p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t DUMMYSENSORTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DUMMYSENSORTask *p_obj = (DUMMYSENSORTask *) _this;
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
            res = DUMMYSENSORTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = DUMMYSENSORTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = DUMMYSENSORTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = DUMMYSENSORTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = DUMMYSENSORTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("DUMMYSENSOR: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("DUMMYSENSOR: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }

    }
  }

  return res;
}

static sys_error_code_t DUMMYSENSORTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  DUMMYSENSORTask *p_obj = (DUMMYSENSORTask *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("DUMMYSENSOR: new data.\r\n"));
//          if(p_obj->pIRQConfig == NULL)
//          {
//            if(TX_SUCCESS
//                != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->dummy_data_task_cfg_timer_period_ms),
//                                   AMT_MS_TO_TICKS(p_obj->dummy_data_task_cfg_timer_period_ms)))
//            {
//              return SYS_UNDEFINED_ERROR_CODE;
//            }
//          }

        res = DUMMYSENSORTaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
#if DUMMYSENSOR_FIFO_ENABLED
          if (p_obj->fifo_level != 0)
          {
#endif
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

            DataEventInit((IEvent *) &evt, p_obj->p_event_src, &p_obj->data, timestamp, p_obj->acc_id);
            IEventSrcSendEvent(p_obj->p_event_src, (IEvent *) &evt, NULL);
            SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("DUMMYSENSOR: ts = %f\r\n", (float)timestamp));
#if DUMMYSENSOR_FIFO_ENABLED
          }
#endif
        }
//            if(p_obj->pIRQConfig == NULL)
//            {
//              if(TX_SUCCESS != tx_timer_activate(&p_obj->read_timer))
//              {
//                res = SYS_UNDEFINED_ERROR_CODE;
//              }
//            }
        break;
      }
      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
            res = DUMMYSENSORTaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->sensor_status.IsActive == true)
              {
                if (p_obj->pIRQConfig == NULL)
                {
                  if (TX_SUCCESS
                      != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->dummy_data_task_cfg_timer_period_ms),
                                         AMT_MS_TO_TICKS(p_obj->dummy_data_task_cfg_timer_period_ms)))
                  {
                    res = SYS_UNDEFINED_ERROR_CODE;
                  }
                  if (TX_SUCCESS != tx_timer_activate(&p_obj->read_timer))
                  {
                    res = SYS_UNDEFINED_ERROR_CODE;
                  }
                }
                else
                {
                  DUMMYSENSORTaskConfigureIrqPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = DUMMYSENSORTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = DUMMYSENSORTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = DUMMYSENSORTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = DUMMYSENSORTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = DUMMYSENSORTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("DUMMYSENSOR: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("DUMMYSENSOR: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t DUMMYSENSORTaskPostReportToFront(DUMMYSENSORTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t DUMMYSENSORTaskPostReportToBack(DUMMYSENSORTask *_this, SMMessage *pReport)
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

static sys_error_code_t DUMMYSENSORTaskSensorInit(DUMMYSENSORTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  int32_t ret_val = 0;
  uint8_t reg0;
  dummy_data_odr_t dummy_data_odr = DUMMYSENSOR_XL_ODR_OFF;

  /* FIFO INT setup */
  dummy_data_ctrl5_int2_pad_ctrl_t int2_route =
  {
    0
  };

  /* Read the output registers to reset the interrupt pin */
  /* Without these instructions, INT PIN remains HIGH and */
  /* it never switches anymore */
  int16_t data[3];
  dummy_data_acceleration_raw_get(p_sensor_drv, data);

  ret_val = dummy_data_reset_set(p_sensor_drv, 1);
  do
  {
    dummy_data_reset_get(p_sensor_drv, &reg0);
  } while (reg0);

  dummy_data_boot_set(p_sensor_drv, PROPERTY_ENABLE);

  dummy_data_spi_mode_set(p_sensor_drv, DUMMYSENSOR_SPI_4_WIRE);

  ret_val = dummy_data_device_id_get(p_sensor_drv, &reg0);
  if (ret_val == 0)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, reg0);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("DUMMYSENSOR: sensor - I am 0x%x.\r\n", reg0));

  /* Enable register address automatically incremented during a multiple byte
   access with a serial interface. */
  dummy_data_auto_increment_set(p_sensor_drv, PROPERTY_ENABLE);

  /* Enable BDU */
  dummy_data_block_data_update_set(p_sensor_drv, PROPERTY_ENABLE);

  /* Power mode selection */
  dummy_data_power_mode_set(p_sensor_drv, DUMMYSENSOR_HIGH_PERFORMANCE);

  /* Output data rate selection - power down. */
  dummy_data_data_rate_set(p_sensor_drv, DUMMYSENSOR_XL_ODR_OFF);

  /* Full scale selection. */
  if (_this->sensor_status.FS < 3.0f)
  {
    dummy_data_full_scale_set(p_sensor_drv, DUMMYSENSOR_2g);
  }
  else if (_this->sensor_status.FS < 5.0f)
  {
    dummy_data_full_scale_set(p_sensor_drv, DUMMYSENSOR_4g);
  }
  else if (_this->sensor_status.FS < 9.0f)
  {
    dummy_data_full_scale_set(p_sensor_drv, DUMMYSENSOR_8g);
  }
  else
  {
    dummy_data_full_scale_set(p_sensor_drv, DUMMYSENSOR_16g);
  }

  if (_this->sensor_status.ODR < 2.0f)
  {
    dummy_data_odr = DUMMYSENSOR_XL_ODR_1Hz6_LP_ONLY;
  }
  else if (_this->sensor_status.ODR < 13.0f)
  {
    dummy_data_odr = DUMMYSENSOR_XL_ODR_12Hz5;
  }
  else if (_this->sensor_status.ODR < 26.0f)
  {
    dummy_data_odr = DUMMYSENSOR_XL_ODR_25Hz;
  }
  else if (_this->sensor_status.ODR < 51.0f)
  {
    dummy_data_odr = DUMMYSENSOR_XL_ODR_50Hz;
  }
  else if (_this->sensor_status.ODR < 101.0f)
  {
    dummy_data_odr = DUMMYSENSOR_XL_ODR_100Hz;
  }
  else if (_this->sensor_status.ODR < 201.0f)
  {
    dummy_data_odr = DUMMYSENSOR_XL_ODR_200Hz;
  }
  else if (_this->sensor_status.ODR < 401.0f)
  {
    dummy_data_odr = DUMMYSENSOR_XL_ODR_400Hz;
  }
  else if (_this->sensor_status.ODR < 801.0f)
  {
    dummy_data_odr = DUMMYSENSOR_XL_ODR_800Hz;
  }
  else
  {
    dummy_data_odr = DUMMYSENSOR_XL_ODR_1k6Hz;
  }

  if (_this->sensor_status.IsActive)
  {
    dummy_data_data_rate_set(p_sensor_drv, dummy_data_odr);
  }
  else
  {
    dummy_data_data_rate_set(p_sensor_drv, DUMMYSENSOR_XL_ODR_OFF);
    _this->sensor_status.IsActive = false;
  }

#if DUMMYSENSOR_FIFO_ENABLED

  if (_this->samples_per_it == 0)
  {
    uint16_t dummy_data_wtm_level = 0;

    /* dummy_data_wtm_level of watermark and samples per int*/
    dummy_data_wtm_level = ((uint16_t) _this->sensor_status.ODR * (uint16_t) DUMMYSENSOR_MAX_DRDY_PERIOD);
    if (dummy_data_wtm_level > DUMMYSENSOR_MAX_WTM_LEVEL)
    {
      dummy_data_wtm_level = DUMMYSENSOR_MAX_WTM_LEVEL;
    }

    _this->samples_per_it = dummy_data_wtm_level;
  }

  dummy_data_fifo_mode_set(p_sensor_drv, DUMMYSENSOR_STREAM_MODE);

  dummy_data_fifo_watermark_set(p_sensor_drv, _this->samples_per_it);

  /* FIFO_WTM_IA routing on pin INT2 */
  dummy_data_pin_int2_route_get(p_sensor_drv, &int2_route);
  *(uint8_t *) &(int2_route) = 0;

  if (_this->pIRQConfig != NULL)
  {
    int2_route.int2_fth = PROPERTY_ENABLE;
  }
  else
  {
    int2_route.int2_fth = PROPERTY_DISABLE;
  }

#else
  _this->samples_per_it = 1;
  if (_this->pIRQConfig != NULL)
  {
    int2_route.int2_drdy = PROPERTY_ENABLE;
  }
  else
  {
    int2_route.int2_drdy = PROPERTY_DISABLE;
  }
#endif /* DUMMYSENSOR_FIFO_ENABLED */
  dummy_data_pin_int2_route_set(p_sensor_drv, &int2_route);

#if DUMMYSENSOR_FIFO_ENABLED
  _this->dummy_data_task_cfg_timer_period_ms = (uint16_t)((1000.0f / _this->sensor_status.ODR) * (((float)(_this->samples_per_it)) / 2.0f));
#else
  _this->dummy_data_task_cfg_timer_period_ms = (uint16_t)(1000.0f / _this->sensor_status.ODR);
#endif

  return res;
}

static sys_error_code_t DUMMYSENSORTaskSensorReadData(DUMMYSENSORTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t samples_per_it = _this->samples_per_it;

#if DUMMYSENSOR_FIFO_ENABLED
  dummy_data_fifo_data_level_get(p_sensor_drv, &_this->fifo_level);

  if (_this->fifo_level >= samples_per_it)
  {
    dummy_data_read_reg(p_sensor_drv, DUMMYSENSOR_OUT_X_L, (uint8_t *) _this->p_sensor_data_buff,
                        ((uint16_t) samples_per_it * 6u));
  }
  else
  {
    _this->fifo_level = 0;
  }

#else
  dummy_data_read_reg(p_sensor_drv, DUMMYSENSOR_OUT_X_L, (uint8_t *) _this->p_sensor_data_buff, samples_per_it * 6);
  _this->fifo_level = 1;
#endif /* DUMMYSENSOR_FIFO_ENABLED */

#if (HSD_USE_DUMMY_DATA == 1)
  uint16_t i = 0;
  int16_t *p16 = (int16_t *)_this->p_sensor_data_buff;

  if (_this->fifo_level >= samples_per_it)
  {
    for (i = 0; i < samples_per_it * 3 ; i++)
    {
      *p16++ = dummyDataCounter++;
    }
  }
#endif

  return res;
}

static sys_error_code_t DUMMYSENSORTaskSensorRegister(DUMMYSENSORTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *acc_if = (ISensor_t *) DUMMYSENSORTaskGetAccSensorIF(_this);
  _this->acc_id = SMAddSensor(acc_if);

  return res;
}

static sys_error_code_t DUMMYSENSORTaskSensorInitTaskParams(DUMMYSENSORTask *_this)
{

  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* ACCELEROMETER SENSOR STATUS */
  _this->sensor_status.IsActive = TRUE;
  _this->sensor_status.FS = 16.0f;
  _this->sensor_status.Sensitivity = 0.0000305f * _this->sensor_status.FS;
  _this->sensor_status.ODR = 1600.0f;
  _this->sensor_status.MeasuredODR = 0.0f;
  EMD_Init(&_this->data, _this->p_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);

  return res;
}

static sys_error_code_t DUMMYSENSORTaskSensorSetODR(DUMMYSENSORTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float ODR = (float) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    if (ODR < 1.0f)
    {
      dummy_data_data_rate_set(p_sensor_drv, DUMMYSENSOR_XL_ODR_OFF);
      /* Do not update the model in case of ODR = 0 */
      ODR = _this->sensor_status.ODR;
    }
    else if (ODR < 2.0f)
    {
      ODR = 1.6f;
    }
    else if (ODR < 13.0f)
    {
      ODR = 12.5f;
    }
    else if (ODR < 26.0f)
    {
      ODR = 25.0f;
    }
    else if (ODR < 51.0f)
    {
      ODR = 50.0f;
    }
    else if (ODR < 101.0f)
    {
      ODR = 100.0f;
    }
    else if (ODR < 201.0f)
    {
      ODR = 200.0f;
    }
    else if (ODR < 401.0f)
    {
      ODR = 400.0f;
    }
    else if (ODR < 801.0f)
    {
      ODR = 800.0f;
    }
    else
    {
      ODR = 1600.0f;
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

static sys_error_code_t DUMMYSENSORTaskSensorSetFS(DUMMYSENSORTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float FS = (float) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    if (FS < 3.0f)
    {
      dummy_data_full_scale_set(p_sensor_drv, DUMMYSENSOR_2g);
      FS = 2.0f;
    }
    else if (FS < 5.0f)
    {
      dummy_data_full_scale_set(p_sensor_drv, DUMMYSENSOR_4g);
      FS = 4.0f;
    }
    else if (FS < 9.0f)
    {
      dummy_data_full_scale_set(p_sensor_drv, DUMMYSENSOR_8g);
      FS = 8.0f;
    }
    else
    {
      dummy_data_full_scale_set(p_sensor_drv, DUMMYSENSOR_16g);
      FS = 16.0f;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->sensor_status.FS = FS;
      _this->sensor_status.Sensitivity = 0.0000305f * _this->sensor_status.FS;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t DUMMYSENSORTaskSensorSetFifoWM(DUMMYSENSORTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t dummy_data_wtm_level = report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    if (dummy_data_wtm_level > DUMMYSENSOR_MAX_WTM_LEVEL)
    {
      dummy_data_wtm_level = DUMMYSENSOR_MAX_WTM_LEVEL;
    }
    _this->samples_per_it = dummy_data_wtm_level;

    /* Set fifo in continuous / stream mode*/
    dummy_data_fifo_mode_set(p_sensor_drv, DUMMYSENSOR_STREAM_MODE);

    /* Set FIFO watermark */
    dummy_data_fifo_watermark_set(p_sensor_drv, _this->samples_per_it);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t DUMMYSENSORTaskSensorEnable(DUMMYSENSORTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    _this->sensor_status.IsActive = TRUE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t DUMMYSENSORTaskSensorDisable(DUMMYSENSORTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    _this->sensor_status.IsActive = FALSE;
    dummy_data_data_rate_set(p_sensor_drv, DUMMYSENSOR_XL_ODR_OFF);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t DUMMYSENSORTaskSensorIsActive(const DUMMYSENSORTask *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.IsActive;
}

static sys_error_code_t DUMMYSENSORTaskEnterLowPowerMode(const DUMMYSENSORTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  if (dummy_data_data_rate_set(p_sensor_drv, DUMMYSENSOR_XL_ODR_OFF))
  {
    res = SYS_SENSOR_TASK_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_OP_ERROR_CODE);
  }

  return res;
}

static sys_error_code_t DUMMYSENSORTaskConfigureIrqPin(const DUMMYSENSORTask *_this, boolean_t LowPower)
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

static void DUMMYSENSORTaskTimerCallbackFunction(ULONG param)
{
  DUMMYSENSORTask *p_obj = (DUMMYSENSORTask *) param;
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

void DUMMYSENSORTask_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((DUMMYSENSORTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}
