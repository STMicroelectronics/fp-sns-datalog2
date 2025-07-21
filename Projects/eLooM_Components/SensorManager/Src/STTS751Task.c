/**
  ******************************************************************************
  * @file    STTS751Task.c
  * @author  SRA - MCD
  * @brief
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
#include "STTS751Task.h"
#include "STTS751Task_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "stts751_reg.h"
#include <string.h>
#include "services/sysdebug.h"

/* Private includes ----------------------------------------------------------*/
#ifndef STTS751_TASK_CFG_STACK_DEPTH
#define STTS751_TASK_CFG_STACK_DEPTH        (TX_MINIMUM_STACK*8)
#endif

#ifndef STTS751_TASK_CFG_PRIORITY
#define STTS751_TASK_CFG_PRIORITY           4
#endif

#ifndef STTS751_TASK_CFG_IN_QUEUE_LENGTH
#define STTS751_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define STTS751_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef STTS751_TASK_CFG_TIMER_PERIOD_MS
#define STTS751_TASK_CFG_TIMER_PERIOD_MS          250
#endif

#ifndef STTS751_TASK_CFG_MAX_INSTANCES_COUNT
#define STTS751_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_STTS751, level, message)

#ifndef STTS751_TASK_CFG_I2C_ADDRESS
#define STTS751_TASK_CFG_I2C_ADDRESS              STTS751_I2C_ADD_L
#endif

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter = 0;
#endif


/**
  * Class object declaration
  */
typedef struct _STTS751TaskClass
{
  /**
    * STTS751Task class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * Temperature IF virtual table.
    */
  const ISensorMems_vtbl sensor_if_vtbl;

  /**
    * Specifies temperature sensor capabilities.
    */
  const SensorDescriptor_t class_descriptor;

  /**
    * STTS751Task (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[STTS751_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} STTS751TaskClass_t;


/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t STTS751TaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t STTS751TaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t STTS751TaskSensorInit(STTS751Task *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t STTS751TaskSensorReadData(STTS751Task *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t STTS751TaskSensorRegister(STTS751Task *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t STTS751TaskSensorInitTaskParams(STTS751Task *_this);

/**
  * Private implementation of sensor interface methods for STTS751 sensor
  */
static sys_error_code_t STTS751TaskSensorSetODR(STTS751Task *_this, SMMessage report);
static sys_error_code_t STTS751TaskSensorSetFS(STTS751Task *_this, SMMessage report);
static sys_error_code_t STTS751TaskSensorEnable(STTS751Task *_this, SMMessage report);
static sys_error_code_t STTS751TaskSensorDisable(STTS751Task *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t STTS751TaskSensorIsActive(const STTS751Task *_this);

static sys_error_code_t STTS751TaskEnterLowPowerMode(const STTS751Task *_this);

static sys_error_code_t STTS751TaskConfigureIrqPin(const STTS751Task *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void STTS751TaskTimerCallbackFunction(ULONG param);


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
static inline sys_error_code_t STTS751TaskPostReportToFront(STTS751Task *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t STTS751TaskPostReportToBack(STTS751Task *_this, SMMessage *pReport);


/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static STTS751Task sTaskObj;

/**
  * The class object.
  */
static STTS751TaskClass_t sTheClass =
{
  /* class virtual table */
  {
    STTS751Task_vtblHardwareInit,
    STTS751Task_vtblOnCreateTask,
    STTS751Task_vtblDoEnterPowerMode,
    STTS751Task_vtblHandleError,
    STTS751Task_vtblOnEnterTaskControlLoop,
    STTS751Task_vtblForceExecuteStep,
    STTS751Task_vtblOnEnterPowerMode
  },

  /* class::sensor_if_vtbl virtual table */
  {
    {
      {
        STTS751Task_vtblTempGetId,
        STTS751Task_vtblTempGetEventSourceIF,
        STTS751Task_vtblTempGetDataInfo
      },
      STTS751Task_vtblSensorEnable,
      STTS751Task_vtblSensorDisable,
      STTS751Task_vtblSensorIsEnabled,
      STTS751Task_vtblSensorGetDescription,
      STTS751Task_vtblSensorGetStatus,
      STTS751Task_vtblSensorGetStatusPointer
    },
    STTS751Task_vtblTempGetODR,
    STTS751Task_vtblTempGetFS,
    STTS751Task_vtblTempGetSensitivity,
    STTS751Task_vtblSensorSetODR,
    STTS751Task_vtblSensorSetFS,
    STTS751Task_vtblSensorSetFifoWM,
  },

  /* TEMPERATURE DESCRIPTOR */
  {
    "stts751",
    COM_TYPE_TEMP
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    STTS751TaskExecuteStepState1,
    NULL,
    STTS751TaskExecuteStepDatalog,
  },
  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *STTS751TaskGetTempSensorIF(STTS751Task *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *STTS751TaskAlloc(const void *pIRQConfig, const void *pCSConfig)
{
  STTS751Task *p_new_obj = SysAlloc(sizeof(STTS751Task));

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

AManagedTaskEx *STTS751TaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *p_name)
{
  STTS751Task *p_new_obj = (STTS751Task *)STTS751TaskAlloc(pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *STTS751TaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig)
{
  STTS751Task *p_obj = (STTS751Task *)p_mem_block;

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

AManagedTaskEx *STTS751TaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                              const char *p_name)
{
  STTS751Task *p_obj = (STTS751Task *)STTS751TaskStaticAlloc(p_mem_block, pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *STTS751TaskGetSensorIF(STTS751Task *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *STTS751TaskGetTempEventSrcIF(STTS751Task *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_temp_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t STTS751Task_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STTS751Task *p_obj = (STTS751Task *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t STTS751Task_vtblOnCreateTask(
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
  STTS751Task *p_obj = (STTS751Task *) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t)STTS751_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(STTS751_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "STTS751_Q", item_size / 4u, p_queue_items_buff,
                                    STTS751_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* create the software timer*/
  if (TX_SUCCESS != tx_timer_create(
        &p_obj->read_fifo_timer,
        "STTS751_T",
        STTS751TaskTimerCallbackFunction,
        (ULONG)_this,
        AMT_MS_TO_TICKS(STTS751_TASK_CFG_TIMER_PERIOD_MS),
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
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(STTS751_ID_MAN, p_obj->pCSConfig->port, (uint16_t)p_obj->pCSConfig->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(STTS751_ID_MAN, STTS751_0xxxx_ADD_7K5, 0);
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
  p_obj->p_temp_event_src = DataEventSrcAlloc();
  if (p_obj->p_temp_event_src == NULL)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }
  IEventSrcInit(p_obj->p_temp_event_src);

  if (!MTMap_IsInitialized(&sTheClass.task_map))
  {
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, STTS751_TASK_CFG_MAX_INSTANCES_COUNT);
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

  p_obj->temp_id = 0;
  p_obj->prev_timestamp = 0.0f;
  p_obj->temperature = 0.0f;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "STTS751";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = STTS751_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = STTS751_TASK_CFG_PRIORITY;
  *pPreemptThreshold = STTS751_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = STTS751TaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = STTS751TaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("STTS751: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t STTS751Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STTS751Task *p_obj = (STTS751Task *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (STTS751TaskSensorIsActive(p_obj))
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
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("STTS751: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      if (STTS751TaskSensorIsActive(p_obj))
      {
        /* Deactivate the sensor */
        stts751_temp_data_rate_set(p_sensor_drv, STTS751_TEMP_ODR_OFF);
      }
      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_fifo_timer);
      }
      else
      {
        STTS751TaskConfigureIrqPin(p_obj, TRUE);
      }
    }
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("STTS751: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = STTS751TaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("STTS751 - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->pIRQConfig != NULL)
    {
      STTS751TaskConfigureIrqPin(p_obj, TRUE);
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("STTS751: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t STTS751Task_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  // STTS751Task *p_obj = (STTS751Task*)_this;

  return res;
}

sys_error_code_t STTS751Task_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("STTS751: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (STTS751_TASK_CFG_TAG)
  STTS751Task *p_obj = (STTS751Task *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = STTS751_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t STTS751Task_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STTS751Task *p_obj = (STTS751Task *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = STTS751TaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t STTS751Task_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  STTS751Task *p_obj = (STTS751Task*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t STTS751Task_vtblTempGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  STTS751Task *p_if_owner = (STTS751Task *)((uint32_t) _this - offsetof(STTS751Task, sensor_if));
  uint8_t res = p_if_owner->temp_id;

  return res;
}

IEventSrc *STTS751Task_vtblTempGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  STTS751Task *p_if_owner = (STTS751Task *)((uint32_t) _this - offsetof(STTS751Task, sensor_if));
  return p_if_owner->p_temp_event_src;
}

sys_error_code_t STTS751Task_vtblTempGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  STTS751Task *p_if_owner = (STTS751Task *)((uint32_t) _this - offsetof(STTS751Task, sensor_if));
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

float_t STTS751Task_vtblTempGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  STTS751Task *p_if_owner = (STTS751Task *)((uint32_t) _this - offsetof(STTS751Task, sensor_if));
  float_t res = p_if_owner->sensor_status.type.mems.fs;

  return res;
}

float_t STTS751Task_vtblTempGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  STTS751Task *p_if_owner = (STTS751Task *)((uint32_t) _this - offsetof(STTS751Task, sensor_if));
  float_t res = p_if_owner->sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t STTS751Task_vtblTempGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  STTS751Task *p_if_owner = (STTS751Task *)((uint32_t)_this - offsetof(STTS751Task, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t STTS751Task_vtblSensorSetODR(ISensorMems_t *_this, float_t odr)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STTS751Task *p_if_owner = (STTS751Task *)((uint32_t) _this - offsetof(STTS751Task, sensor_if));
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
    res = STTS751TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STTS751Task_vtblSensorSetFS(ISensorMems_t *_this, float_t fs)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STTS751Task *p_if_owner = (STTS751Task *)((uint32_t) _this - offsetof(STTS751Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    if (fs == 100.0f)
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
    res = STTS751TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t STTS751Task_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  /* Does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
  SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("STTS751: warning - SetFifoWM() not supported.\r\n"));
  return SYS_INVALID_FUNC_CALL_ERROR_CODE;
}

sys_error_code_t STTS751Task_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STTS751Task *p_if_owner = (STTS751Task *)((uint32_t) _this - offsetof(STTS751Task, sensor_if));
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
    res = STTS751TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STTS751Task_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STTS751Task *p_if_owner = (STTS751Task *)((uint32_t) _this - offsetof(STTS751Task, sensor_if));
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
    res = STTS751TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t STTS751Task_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  STTS751Task *p_if_owner = (STTS751Task *)((uint32_t) _this - offsetof(STTS751Task, sensor_if));

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->temp_id)
  {
    res = p_if_owner->sensor_status.is_active;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

SensorDescriptor_t STTS751Task_vtblSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  STTS751Task *p_if_owner = (STTS751Task *)((uint32_t) _this - offsetof(STTS751Task, sensor_if));
  return *p_if_owner->sensor_descriptor;

}

SensorStatus_t STTS751Task_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  STTS751Task *p_if_owner = (STTS751Task *)((uint32_t) _this - offsetof(STTS751Task, sensor_if));

  return p_if_owner->sensor_status;
}

SensorStatus_t *STTS751Task_vtblSensorGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  STTS751Task *p_if_owner = (STTS751Task *)((uint32_t) _this - offsetof(STTS751Task, sensor_if));

  return &p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t STTS751TaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STTS751Task *p_obj = (STTS751Task *) _this;
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
            res = STTS751TaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = STTS751TaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = STTS751TaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = STTS751TaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("STTS751: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("STTS751: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t STTS751TaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STTS751Task *p_obj = (STTS751Task *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("STTS751: new data.\r\n"));
//        if (p_obj->pIRQConfig == NULL)
//        {
//          if (TX_SUCCESS != tx_timer_change(&p_obj->read_fifo_timer, AMT_MS_TO_TICKS(p_obj->task_delay),
//                                            AMT_MS_TO_TICKS(p_obj->task_delay)))
//          {
//            return SYS_UNDEFINED_ERROR_CODE;
//          }
//        }

        res = STTS751TaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          // notify the listeners...
          double_t timestamp = report.sensorDataReadyMessage.fTimestamp;
          double_t delta_timestamp = timestamp - p_obj->prev_timestamp;
          p_obj->prev_timestamp = timestamp;

          /* update measuredODR */
          p_obj->sensor_status.type.mems.measured_odr = 1.0f / (float_t) delta_timestamp;

          EMD_1dInit(&p_obj->data, (uint8_t *)&p_obj->temperature, E_EM_FLOAT, 1);

          DataEvent_t evt;

          DataEventInit((IEvent *)&evt, p_obj->p_temp_event_src, &p_obj->data, timestamp, p_obj->temp_id);
          IEventSrcSendEvent(p_obj->p_temp_event_src, (IEvent *) &evt, NULL);

          SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("STTS751: ts = %f\r\n", (float_t)timestamp));
        }
//          if (p_obj->pIRQConfig == NULL)
//          {
//            if (TX_SUCCESS != tx_timer_activate(&p_obj->read_fifo_timer))
//            {
//              res = SYS_UNDEFINED_ERROR_CODE;
//            }
//          }
        break;
      }

      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
            res = STTS751TaskSensorInit(p_obj);
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
                  STTS751TaskConfigureIrqPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = STTS751TaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = STTS751TaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = STTS751TaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = STTS751TaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("STTS751: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("STTS751: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t STTS751TaskPostReportToFront(STTS751Task *_this, SMMessage *pReport)
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

static inline sys_error_code_t STTS751TaskPostReportToBack(STTS751Task *_this, SMMessage *pReport)
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

static sys_error_code_t STTS751TaskSensorInit(STTS751Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  stts751_id_t STTS751_Id;
  int32_t ret_val = 0;
  stts751_odr_t stts751_odr_temp = STTS751_TEMP_ODR_OFF;

  ret_val = stts751_device_id_get(p_sensor_drv, &STTS751_Id);
  if (!ret_val)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, STTS751_Id.manufacturer_id);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("STTS751: sensor - I am 0x%x.\r\n", STTS751_Id));

  stts751_resolution_set(p_sensor_drv,  STTS751_12bit);

  if (_this->sensor_status.type.mems.odr < 2.0f)
  {
    stts751_odr_temp = STTS751_TEMP_ODR_1Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 3.0f)
  {
    stts751_odr_temp = STTS751_TEMP_ODR_2Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 5.0f)
  {
    stts751_odr_temp = STTS751_TEMP_ODR_4Hz;
  }
  else
  {
    stts751_odr_temp = STTS751_TEMP_ODR_8Hz;
  }

  if (_this->sensor_status.is_active)
  {
    stts751_temp_data_rate_set(p_sensor_drv, stts751_odr_temp);
  }
  else
  {
    stts751_temp_data_rate_set(p_sensor_drv, STTS751_TEMP_ODR_OFF);
    _this->sensor_status.is_active = false;
  }

  if (_this->sensor_status.is_active)
  {
    _this->task_delay = (uint16_t)(1000.0f / _this->sensor_status.type.mems.odr);
  }

  return res;
}

static sys_error_code_t STTS751TaskSensorReadData(STTS751Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  int16_t temperature_celsius;

  res = stts751_temperature_raw_get(p_sensor_drv, (int16_t *) &temperature_celsius);

  if (!SYS_IS_ERROR_CODE(res))
  {
    _this->temperature = (float_t) temperature_celsius / 256.0f;

#if (HSD_USE_DUMMY_DATA == 1)
    _this->temperature = (float_t) dummyDataCounter++;
#endif
  }

  return res;
}

static sys_error_code_t STTS751TaskSensorRegister(STTS751Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *temp_if = (ISensor_t *) STTS751TaskGetTempSensorIF(_this);
  _this->temp_id = SMAddSensor(temp_if);

  return res;
}

static sys_error_code_t STTS751TaskSensorInitTaskParams(STTS751Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* TEMPERATURE SENSOR STATUS */
  _this->sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->sensor_status.is_active = TRUE;
  _this->sensor_status.type.mems.fs = 100.0f;
  _this->sensor_status.type.mems.sensitivity = 1.0f;
  _this->sensor_status.type.mems.odr = 8.0f;
  _this->sensor_status.type.mems.measured_odr = 0.0f;
  EMD_1dInit(&_this->data, (uint8_t *)&_this->temperature, E_EM_FLOAT, 1);

  return res;
}

static sys_error_code_t STTS751TaskSensorSetODR(STTS751Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float_t odr = (float_t) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->temp_id)
  {
    if (odr < 1.0f)
    {
      stts751_temp_data_rate_set(p_sensor_drv, STTS751_TEMP_ODR_OFF);
      /* Do not update the model in case of odr = 0 */
      odr = _this->sensor_status.type.mems.odr;
    }
    else if (odr < 2.0f)
    {
      odr = 1.0f;
    }
    else if (odr < 3.0f)
    {
      odr = 2.0f;
    }
    else if (odr < 5.0f)
    {
      odr = 4.0f;
    }
    else
    {
      odr = 8.0f;
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

static sys_error_code_t STTS751TaskSensorSetFS(STTS751Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  float_t fs = (float_t) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->temp_id)
  {
    if (fs != 100.0f)
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->sensor_status.type.mems.fs = fs;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STTS751TaskSensorEnable(STTS751Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->temp_id)
  {
    _this->sensor_status.is_active = TRUE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STTS751TaskSensorDisable(STTS751Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->temp_id)
  {
    _this->sensor_status.is_active = FALSE;
    stts751_temp_data_rate_set(p_sensor_drv, STTS751_TEMP_ODR_OFF);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t STTS751TaskSensorIsActive(const STTS751Task *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.is_active;
}

static sys_error_code_t STTS751TaskEnterLowPowerMode(const STTS751Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  if (stts751_temp_data_rate_set(p_sensor_drv, STTS751_TEMP_ODR_OFF))
  {
    res = SYS_SENSOR_TASK_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_OP_ERROR_CODE);
  }
  return res;
}

static sys_error_code_t STTS751TaskConfigureIrqPin(const STTS751Task *_this, boolean_t LowPower)
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
    GPIO_InitStruct.Pin =  _this->pIRQConfig->pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(_this->pIRQConfig->port, &GPIO_InitStruct);
  }

  return res;
}

static void STTS751TaskTimerCallbackFunction(ULONG param)
{
  STTS751Task *p_obj = (STTS751Task *) param;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  if (TX_SUCCESS != tx_queue_send(&p_obj->in_queue, &report, TX_NO_WAIT))
  {
    /* unable to send the report. Signal the error */
    sys_error_handler();
  }
}

/* CubeMX integration */

void STTS751Task_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((STTS751Task *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}

