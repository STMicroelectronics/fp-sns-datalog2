/**
  ******************************************************************************
  * @file    TSC1641Task.c
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
#include "TSC1641Task.h"
#include "TSC1641Task_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "TSC1641.h"
#include <string.h>
#include "services/sysdebug.h"

/* Private includes ----------------------------------------------------------*/
#ifndef TSC1641_TASK_CFG_STACK_DEPTH
#define TSC1641_TASK_CFG_STACK_DEPTH        (TX_MINIMUM_STACK*8)
#endif

#ifndef TSC1641_TASK_CFG_PRIORITY
#define TSC1641_TASK_CFG_PRIORITY           4
#endif

#ifndef TSC1641_TASK_CFG_IN_QUEUE_LENGTH
#define TSC1641_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define TSC1641_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef TSC1641_TASK_CFG_TIMER_PERIOD_MS
#define TSC1641_TASK_CFG_TIMER_PERIOD_MS          250
#endif

#ifndef TSC1641_TASK_CFG_MAX_INSTANCES_COUNT
#define TSC1641_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_TSC1641, level, message)

#ifndef TSC1641_TASK_CFG_I2C_ADDRESS
#define TSC1641_TASK_CFG_I2C_ADDRESS              I2C_TSC1641_ADD_W
#endif

#define VLOAD_LSB_TO_MV    2.0f
#define VSHUNT_LSB_TO_MV   0.0025f
#define DCPOWER_LSB_TO_MW  25.0f

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter = 0;
#endif


/**
  * Class object declaration
  */
typedef struct _TSC1641TaskClass
{
  /**
    * TSC1641Task class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * Temperature IF virtual table.
    */
  const ISensorPowerMeter_vtbl sensor_if_vtbl;

  /**
    * Specifies p_sensor_data_buff sensor capabilities.
    */
  const SensorDescriptor_t class_descriptor;

  /**
    * TSC1641Task (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[TSC1641_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} TSC1641TaskClass_t;


/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t TSC1641TaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t TSC1641TaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t TSC1641TaskSensorInit(TSC1641Task *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t TSC1641TaskSensorReadData(TSC1641Task *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t TSC1641TaskSensorRegister(TSC1641Task *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t TSC1641TaskSensorInitTaskParams(TSC1641Task *_this);

/**
  * Private implementation of sensor interface methods for TSC1641 sensor
  */
static sys_error_code_t TSC1641TaskSensorEnable(TSC1641Task *_this, SMMessage report);
static sys_error_code_t TSC1641TaskSensorDisable(TSC1641Task *_this, SMMessage report);

/**
  * Private implementation of sensor interface methods for TSC1641 sensor
  */
static sys_error_code_t TSC1641TaskSensorSetADCConversionTime(TSC1641Task *_this, SMMessage report);
static sys_error_code_t TSC1641TaskSensorSetRShunt(TSC1641Task *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t TSC1641TaskSensorIsActive(const TSC1641Task *_this);

static sys_error_code_t TSC1641TaskConfigureIrqPin(const TSC1641Task *_this, boolean_t LowPower);

static void TSC1641TaskTimerCallbackFunction(ULONG param);


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
static inline sys_error_code_t TSC1641TaskPostReportToFront(TSC1641Task *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t TSC1641TaskPostReportToBack(TSC1641Task *_this, SMMessage *pReport);


/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static TSC1641Task sTaskObj;

/**
  * The class object.
  */
static TSC1641TaskClass_t sTheClass =
{
  /* class virtual table */
  {
    TSC1641Task_vtblHardwareInit,
    TSC1641Task_vtblOnCreateTask,
    TSC1641Task_vtblDoEnterPowerMode,
    TSC1641Task_vtblHandleError,
    TSC1641Task_vtblOnEnterTaskControlLoop,
    TSC1641Task_vtblForceExecuteStep,
    TSC1641Task_vtblOnEnterPowerMode
  },

  /* class::sensor_if_vtbl virtual table */
  {
    {
      {
        TSC1641Task_vtblGetId,
        TSC1641Task_vtblGetEventSourceIF,
        TSC1641Task_vtblGetDataInfo
      },
      TSC1641Task_vtblSensorEnable,
      TSC1641Task_vtblSensorDisable,
      TSC1641Task_vtblSensorIsEnabled,
      TSC1641Task_vtblSensorGetDescription,
      TSC1641Task_vtblSensorGetStatus,
      TSC1641Task_vtblSensorGetStatusPointer
    },
    TSC1641Task_vtblGetADCConversionTime,
    TSC1641Task_vtblGetRShunt,
    TSC1641Task_vtblSensorSetADCConversionTime,
    TSC1641Task_vtblSensorSetRShunt
  },

  /* TEMPERATURE DESCRIPTOR */
  {
    "tsc1641",
    COM_TYPE_POW
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    TSC1641TaskExecuteStepState1,
    NULL,
    TSC1641TaskExecuteStepDatalog,
  },
  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *TSC1641TaskGetPowerMeterSensorIF(TSC1641Task *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *TSC1641TaskAlloc(const void *pIRQConfig, const void *pCSConfig)
{
  TSC1641Task *p_new_obj = SysAlloc(sizeof(TSC1641Task));

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

AManagedTaskEx *TSC1641TaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *p_name)
{
  TSC1641Task *p_new_obj = (TSC1641Task *)TSC1641TaskAlloc(pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *TSC1641TaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig)
{
  TSC1641Task *p_obj = (TSC1641Task *)p_mem_block;

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

AManagedTaskEx *TSC1641TaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                              const char *p_name)
{
  TSC1641Task *p_obj = (TSC1641Task *)TSC1641TaskStaticAlloc(p_mem_block, pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *TSC1641TaskGetSensorIF(TSC1641Task *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *TSC1641TaskGetEventSrcIF(TSC1641Task *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t TSC1641Task_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  TSC1641Task *p_obj = (TSC1641Task *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t TSC1641Task_vtblOnCreateTask(
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
  TSC1641Task *p_obj = (TSC1641Task *) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t)TSC1641_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(TSC1641_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "TSC1641_Q", item_size / 4u, p_queue_items_buff,
                                    TSC1641_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* create the software timer*/
  if (TX_SUCCESS != tx_timer_create(
        &p_obj->read_timer,
        "TSC1641_T",
        TSC1641TaskTimerCallbackFunction,
        (ULONG)_this,
        AMT_MS_TO_TICKS(TSC1641_TASK_CFG_TIMER_PERIOD_MS),
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
    /* Does not support this virtual function.*/
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("TSC1641: warning - SPI not supported.\r\n"));
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(TSC1641_RegAdd_DieID, TSC1641_TASK_CFG_I2C_ADDRESS, 0);
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
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, TSC1641_TASK_CFG_MAX_INSTANCES_COUNT);
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

  p_obj->id = 0;
  p_obj->prev_timestamp = 0.0f;
  memset(p_obj->p_sensor_data_buff, 0, sizeof(p_obj->p_sensor_data_buff));
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "TSC1641";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = TSC1641_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = TSC1641_TASK_CFG_PRIORITY;
  *pPreemptThreshold = TSC1641_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = TSC1641TaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = TSC1641TaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("TSC1641: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t TSC1641Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  TSC1641Task *p_obj = (TSC1641Task *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (TSC1641TaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("TSC1641: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      /* Deactivate the sensor */
      Alert alert;
      alert.TSC1641_SOL = TSC1641_Conf_Off;    // Shunt Voltage Over voltage
      alert.TSC1641_SUL = TSC1641_Conf_Off;    // Shunt Voltage Under voltage
      alert.TSC1641_LOL = TSC1641_Conf_Off;    // Load Voltage Over voltage
      alert.TSC1641_LUL = TSC1641_Conf_Off;    // Load Voltage Under voltage //off
      alert.TSC1641_POL = TSC1641_Conf_Off;    // Power Over Limit
      alert.TSC1641_TOL = TSC1641_Conf_Off;    // Temperature Over Limit
      alert.TSC1641_CNVR = TSC1641_Conf_Off;   // Conversion ready alert enable
      alert.TSC1641_APOL = TSC1641_Conf_Off;   // Alert polarity
      alert.TSC1641_ALEN = TSC1641_Conf_Off;   // Alert Latch Enable
      TSC1641_SetAlert(p_sensor_drv, &alert);

      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_timer);
      }
      else
      {
        TSC1641TaskConfigureIrqPin(p_obj, TRUE);
      }
    }
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("TSC1641: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("TSC1641 - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->pIRQConfig != NULL)
    {
      TSC1641TaskConfigureIrqPin(p_obj, TRUE);
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("TSC1641: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t TSC1641Task_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  // TSC1641Task *p_obj = (TSC1641Task*)_this;

  return res;
}

sys_error_code_t TSC1641Task_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("TSC1641: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (TSC1641_TASK_CFG_TAG)
  TSC1641Task *p_obj = (TSC1641Task *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = TSC1641_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t TSC1641Task_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  TSC1641Task *p_obj = (TSC1641Task *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = TSC1641TaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t TSC1641Task_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  TSC1641Task *p_obj = (TSC1641Task*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t TSC1641Task_vtblGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  TSC1641Task *p_if_owner = (TSC1641Task *)((uint32_t) _this - offsetof(TSC1641Task, sensor_if));
  uint8_t res = p_if_owner->id;

  return res;
}

IEventSrc *TSC1641Task_vtblGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  TSC1641Task *p_if_owner = (TSC1641Task *)((uint32_t) _this - offsetof(TSC1641Task, sensor_if));
  return p_if_owner->p_event_src;
}

uint32_t TSC1641Task_vtblGetADCConversionTime(ISensorPowerMeter_t *_this)
{
  assert_param(_this != NULL);
  TSC1641Task *p_if_owner = (TSC1641Task *)((uint32_t) _this - offsetof(TSC1641Task, sensor_if));
  uint32_t res = p_if_owner->sensor_status.type.power_meter.adc_conversion_time;

  return res;
}

uint32_t TSC1641Task_vtblGetRShunt(ISensorPowerMeter_t *_this)
{
  assert_param(_this != NULL);
  TSC1641Task *p_if_owner = (TSC1641Task *)((uint32_t) _this - offsetof(TSC1641Task, sensor_if));
  uint32_t res = p_if_owner->sensor_status.type.power_meter.r_shunt;

  return res;
}

EMData_t TSC1641Task_vtblGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  TSC1641Task *p_if_owner = (TSC1641Task *)((uint32_t)_this - offsetof(TSC1641Task, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t TSC1641Task_vtblSensorSetADCConversionTime(ISensorPowerMeter_t *_this, uint32_t adc_conversion_time)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  TSC1641Task *p_if_owner = (TSC1641Task *)((uint32_t) _this - offsetof(TSC1641Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.power_meter.adc_conversion_time = adc_conversion_time;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_ADC_CONVERSION_TIME,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t)adc_conversion_time
    };
    res = TSC1641TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t TSC1641Task_vtblSensorSetRShunt(ISensorPowerMeter_t *_this, uint32_t r_shunt)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  TSC1641Task *p_if_owner = (TSC1641Task *)((uint32_t) _this - offsetof(TSC1641Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.power_meter.r_shunt = r_shunt - (r_shunt % 10);
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_R_SHUNT,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t)r_shunt
    };
    res = TSC1641TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t TSC1641Task_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  TSC1641Task *p_if_owner = (TSC1641Task *)((uint32_t) _this - offsetof(TSC1641Task, sensor_if));
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
    res = TSC1641TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t TSC1641Task_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  TSC1641Task *p_if_owner = (TSC1641Task *)((uint32_t) _this - offsetof(TSC1641Task, sensor_if));
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
    res = TSC1641TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t TSC1641Task_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  TSC1641Task *p_if_owner = (TSC1641Task *)((uint32_t) _this - offsetof(TSC1641Task, sensor_if));

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->id)
  {
    res = p_if_owner->sensor_status.is_active;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

SensorDescriptor_t TSC1641Task_vtblSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  TSC1641Task *p_if_owner = (TSC1641Task *)((uint32_t) _this - offsetof(TSC1641Task, sensor_if));
  return *p_if_owner->sensor_descriptor;

}

SensorStatus_t TSC1641Task_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  TSC1641Task *p_if_owner = (TSC1641Task *)((uint32_t) _this - offsetof(TSC1641Task, sensor_if));

  return p_if_owner->sensor_status;
}

SensorStatus_t *TSC1641Task_vtblSensorGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  TSC1641Task *p_if_owner = (TSC1641Task *)((uint32_t) _this - offsetof(TSC1641Task, sensor_if));

  return &p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t TSC1641TaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  TSC1641Task *p_obj = (TSC1641Task *) _this;
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
          case SENSOR_CMD_ID_SET_ADC_CONVERSION_TIME:
            res = TSC1641TaskSensorSetADCConversionTime(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_R_SHUNT:
            res = TSC1641TaskSensorSetRShunt(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = TSC1641TaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = TSC1641TaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("TSC1641: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("TSC1641: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t TSC1641TaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  TSC1641Task *p_obj = (TSC1641Task *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("TSC1641: new data.\r\n"));

        res = TSC1641TaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          // notify the listeners...
          double timestamp = report.sensorDataReadyMessage.fTimestamp;
          p_obj->prev_timestamp = timestamp;

          /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue (samples_per_it):
           * [X0, Y0, Z0]
           * [X1, Y1, Z1]
           * ...
           * [Xm-1, Ym-1, Zm-1]
           */
          EMD_Init(&p_obj->data, (uint8_t *)p_obj->p_sensor_data_buff, E_EM_FLOAT, E_EM_MODE_INTERLEAVED, 2, 1, 4);

          DataEvent_t evt;

          DataEventInit((IEvent *)&evt, p_obj->p_event_src, &p_obj->data, timestamp, p_obj->id);
          IEventSrcSendEvent(p_obj->p_event_src, (IEvent *) &evt, NULL);

          SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("TSC1641: ts = %f\r\n", (float)timestamp));
        }
        break;
      }

      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
            res = TSC1641TaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->sensor_status.is_active == true)
              {
                if (p_obj->pIRQConfig == NULL)
                {
                  if (TX_SUCCESS != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->task_delay),
                                                    AMT_MS_TO_TICKS(p_obj->task_delay)))
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
                  TSC1641TaskConfigureIrqPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_ADC_CONVERSION_TIME:
            res = TSC1641TaskSensorSetADCConversionTime(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_R_SHUNT:
            res = TSC1641TaskSensorSetRShunt(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = TSC1641TaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = TSC1641TaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("TSC1641: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("TSC1641: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t TSC1641TaskPostReportToFront(TSC1641Task *_this, SMMessage *pReport)
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

static inline sys_error_code_t TSC1641TaskPostReportToBack(TSC1641Task *_this, SMMessage *pReport)
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

static sys_error_code_t TSC1641TaskSensorInit(TSC1641Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  Configuration conf;
  Limit limit;
  Alert alert;

//  Configuration CONFDEF;
//  CONFDEF.TSC1641_RESET = (uint8_t) TSC1641_Conf_Off;
//  CONFDEF.TSC1641_NVM = (uint8_t) TSC1641_Conf_Off;
//  CONFDEF.TSC1641_AVG = (uint8_t) TSC1641_Conf_Avg_1;
//  CONFDEF.TSC1641_CT = (uint8_t) TSC1641_Conf_CT_1024;
//  CONFDEF.TSC1641_TEMP = (uint8_t) TSC1641_Conf_On;
//  CONFDEF.TSC1641_MODE = (uint8_t) TSC1641_Mode_VshloadCont;

  alert.TSC1641_SOL = TSC1641_Conf_Off;    // Shunt Voltage Over voltage
  alert.TSC1641_SUL = TSC1641_Conf_Off;    // Shunt Voltage Under voltage
  alert.TSC1641_LOL = TSC1641_Conf_Off;    // Load Voltage Over voltage
  alert.TSC1641_LUL = TSC1641_Conf_Off;    // Load Voltage Under voltage //off
  alert.TSC1641_POL = TSC1641_Conf_Off;    // Power Over Limit
  alert.TSC1641_TOL = TSC1641_Conf_Off;    // Temperature Over Limit
  alert.TSC1641_CNVR = TSC1641_Conf_On;    // Conversion ready alert enable
  alert.TSC1641_APOL = TSC1641_Conf_On;    // Alert polarity
  alert.TSC1641_ALEN = TSC1641_Conf_Off;   // Alert Latch Enable

  limit.VSHUNT_OV_LIM = 0x00;      // Vshunt Over voltage limit value
  limit.VSHUNT_UV_LIM = 0x00;      // Vshunt Under voltage limit value
  limit.VLOAD_OV_LIM = 0x00;       // Vload Over voltage limit value
  limit.VLOAD_UV_LIM = 0x00;       // Vload Under voltage limit value
  limit.POWER_OV_LIM = 0x00;       // Power over limit value
  limit.TEMP_OV_LIM = 0x00;        // Temperature over limit value

  TSC1641_SetRShunt(p_sensor_drv, _this->sensor_status.type.power_meter.r_shunt);
  TSC1641_SetAlert(p_sensor_drv, &alert);
  TSC1641_SetLimit(p_sensor_drv, &limit);

  // Default configuration
  conf.TSC1641_RESET = (uint8_t) TSC1641_Conf_Off;
  conf.TSC1641_NVM = (uint8_t) TSC1641_Conf_Off;
  conf.TSC1641_AVG = (uint8_t) TSC1641_Conf_Avg_1;
  conf.TSC1641_TEMP = (uint8_t) TSC1641_Conf_Off;
  conf.TSC1641_MODE = (uint8_t) TSC1641_Mode_VshloadCont;

  switch (_this->sensor_status.type.power_meter.adc_conversion_time)
  {
    case 128:
      conf.TSC1641_CT = TSC1641_Conf_CT_128;
      break;
    case 256:
      conf.TSC1641_CT = TSC1641_Conf_CT_256;
      break;
    case 512:
      conf.TSC1641_CT = TSC1641_Conf_CT_512;
      break;
    case 1024:
      conf.TSC1641_CT = TSC1641_Conf_CT_1024;
      break;
    case 2048:
      conf.TSC1641_CT = TSC1641_Conf_CT_2048;
      break;
    case 4096:
      conf.TSC1641_CT = TSC1641_Conf_CT_4096;
      break;
    case 8192:
      conf.TSC1641_CT = TSC1641_Conf_CT_8192;
      break;
    case 16384:
      conf.TSC1641_CT = TSC1641_Conf_CT_16384;
      break;
    case 32768:
      conf.TSC1641_CT = TSC1641_Conf_CT_32768;
      break;
    default:
      conf.TSC1641_CT = TSC1641_Conf_CT_8192;
      break;
  }

  TSC1641_SetConf(p_sensor_drv, &conf);

  if (_this->sensor_status.is_active)
  {
    /* timer = adc time in ms --> (adc_conversion_time[us]/1000)*/
    _this->task_delay = (_this->sensor_status.type.power_meter.adc_conversion_time / 1000);
  }

  /* From datasheet: First valid data is available after 3 clock cycles of 128 μs
     and one cycle of 1024 μs --> wait 2ms*/
  tx_thread_sleep(2);

  return res;
}

static sys_error_code_t TSC1641TaskSensorReadData(TSC1641Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t Buffer[6];
  uint16_t vload_lsb;
  uint16_t vshunt_lsb;
  uint16_t power_lsb;

  res = TSC1641_read_reg(p_sensor_drv, TSC1641_RegAdd_ShuntV, Buffer, 6);

  if (!SYS_IS_ERROR_CODE(res))
  {
    /* Current read */
    vshunt_lsb = (Buffer[0] << 8) + Buffer[1];
    /* Voltage read */
    vload_lsb = (Buffer[2] << 8) + Buffer[3];
    /* Power read */
    power_lsb = (Buffer[4] << 8) + Buffer[5];

    _this->p_sensor_data_buff[0] = (float)vload_lsb * VLOAD_LSB_TO_MV; /* Voltage */
    _this->p_sensor_data_buff[1] = (float)vshunt_lsb * VSHUNT_LSB_TO_MV; /* Voltage(VShunt) */
    _this->p_sensor_data_buff[2] = _this->p_sensor_data_buff[1] / _this->sensor_status.type.power_meter.r_shunt * 1000.0f; /* Current */
    _this->p_sensor_data_buff[3] = (float)power_lsb * DCPOWER_LSB_TO_MW; /* Power */

#if (HSD_USE_DUMMY_DATA == 1)
    uint16_t i = 0;
    float *pfloat = (float *)_this->p_sensor_data_buff;

    for (i = 0; i < 4 ; i++)
    {
      *pfloat++ = dummyDataCounter++;
    }
#endif
  }

  return res;
}

static sys_error_code_t TSC1641TaskSensorRegister(TSC1641Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *pow_met_if = (ISensor_t *) TSC1641TaskGetPowerMeterSensorIF(_this);
  _this->id = SMAddSensor(pow_met_if);

  return res;
}

static sys_error_code_t TSC1641TaskSensorInitTaskParams(TSC1641Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* TEMPERATURE SENSOR STATUS */
  _this->sensor_status.isensor_class = ISENSOR_CLASS_POWERMONITOR;
  _this->sensor_status.is_active = TRUE;
  _this->sensor_status.type.power_meter.adc_conversion_time = 8192;
  _this->sensor_status.type.power_meter.r_shunt = 5000;
  EMD_Init(&_this->data, (uint8_t *)_this->p_sensor_data_buff, E_EM_FLOAT, E_EM_MODE_INTERLEAVED, 2, 1, 4);

  return res;
}

static sys_error_code_t TSC1641TaskSensorSetADCConversionTime(TSC1641Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint32_t adc_conversion_time = (uint32_t) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (adc_conversion_time < 129)
    {
      adc_conversion_time = 128;
    }
    else if (adc_conversion_time < 257)
    {
      adc_conversion_time = 256;
    }
    else if (adc_conversion_time < 513)
    {
      adc_conversion_time = 512;
    }
    else if (adc_conversion_time < 1025)
    {
      adc_conversion_time = 1024;
    }
    else if (adc_conversion_time < 2049)
    {
      adc_conversion_time = 2048;
    }
    else if (adc_conversion_time < 4097)
    {
      adc_conversion_time = 4096;
    }
    else if (adc_conversion_time < 8193)
    {
      adc_conversion_time = 8192;
    }
    else if (adc_conversion_time < 16385)
    {
      adc_conversion_time = 16384;
    }
    else
    {
      adc_conversion_time = 32768;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->sensor_status.type.power_meter.adc_conversion_time = adc_conversion_time;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t TSC1641TaskSensorSetRShunt(TSC1641Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint32_t r_shunt = (uint32_t) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->sensor_status.type.power_meter.r_shunt = r_shunt - (r_shunt % 10);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t TSC1641TaskSensorEnable(TSC1641Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    _this->sensor_status.is_active = TRUE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t TSC1641TaskSensorDisable(TSC1641Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
//  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    _this->sensor_status.is_active = FALSE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t TSC1641TaskSensorIsActive(const TSC1641Task *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.is_active;
}

static sys_error_code_t TSC1641TaskConfigureIrqPin(const TSC1641Task *_this, boolean_t LowPower)
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

static void TSC1641TaskTimerCallbackFunction(ULONG param)
{
  TSC1641Task *p_obj = (TSC1641Task *) param;
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

void TSC1641Task_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((TSC1641Task *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}


uint8_t DataReadyFlag = 0;
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_6)
  {
    DataReadyFlag = 1;
  }
}

