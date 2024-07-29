/**
  ******************************************************************************
  * @file    SHT40Task.c
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
#include "SHT40Task.h"
#include "SHT40Task_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "sht40ad1b_reg.h"
#include <string.h>
#include "services/sysdebug.h"

/* Private includes ----------------------------------------------------------*/
#ifndef SHT40_TASK_CFG_STACK_DEPTH
#define SHT40_TASK_CFG_STACK_DEPTH        (TX_MINIMUM_STACK*8)
#endif

#ifndef SHT40_TASK_CFG_PRIORITY
#define SHT40_TASK_CFG_PRIORITY           4
#endif

#ifndef SHT40_TASK_CFG_IN_QUEUE_LENGTH
#define SHT40_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define SHT40_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef SHT40_TASK_CFG_TIMER_PERIOD_MS
#define SHT40_TASK_CFG_TIMER_PERIOD_MS          250
#endif

#ifndef SHT40_TASK_CFG_MAX_INSTANCES_COUNT
#define SHT40_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_SHT40, level, message)

#ifndef SHT40_TASK_CFG_I2C_ADDRESS
#define SHT40_TASK_CFG_I2C_ADDRESS              SHT40AD1B_I2C_ADDRESS
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
typedef struct _SHT40TaskClass
{
  /**
    * SHT40Task class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * Temperature IF virtual table.
    */
  const ISensorMems_vtbl temp_sensor_if_vtbl;

  /**
    * Humidity IF virtual table.
    */
  const ISensorMems_vtbl hum_sensor_if_vtbl;

  /**
    * Specifies temperature sensor capabilities.
    */
  const SensorDescriptor_t temp_class_descriptor;

  /**
    * Specifies humidity sensor capabilities.
    */
  const SensorDescriptor_t hum_class_descriptor;

  /**
    * SHT40Task (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[2 * SHT40_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} SHT40TaskClass_t;

/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t SHT40TaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t SHT40TaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t SHT40TaskSensorInit(SHT40Task *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t SHT40TaskSensorReadData(SHT40Task *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t SHT40TaskSensorRegister(SHT40Task *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t SHT40TaskSensorInitTaskParams(SHT40Task *_this);

/**
  * Private implementation of sensor interface methods for SHT40 sensor
  */
static sys_error_code_t SHT40TaskSensorSetODR(SHT40Task *_this, SMMessage report);
static sys_error_code_t SHT40TaskSensorEnable(SHT40Task *_this, SMMessage report);
static sys_error_code_t SHT40TaskSensorDisable(SHT40Task *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t SHT40TaskSensorIsActive(const SHT40Task *_this);

/**
  * Given a interface pointer it return the instance of the object that implement the interface.
  *
  * @param p_if [IN] specifies a sensor interface implemented by the task object.
  * @return the instance of the task object that implements the given interface.
  */
static inline SHT40Task *SHT40TaskGetOwnerFromISensorIF(ISensor_t *p_if);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void SHT40TaskTimerCallbackFunction(ULONG param);

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
static inline sys_error_code_t SHT40TaskPostReportToFront(SHT40Task *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t SHT40TaskPostReportToBack(SHT40Task *_this, SMMessage *pReport);

/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static SHT40Task sTaskObj;
/**
  * The class object.
  */
static SHT40TaskClass_t sTheClass =
{
  /* class virtual table */
  {
    SHT40Task_vtblHardwareInit,
    SHT40Task_vtblOnCreateTask,
    SHT40Task_vtblDoEnterPowerMode,
    SHT40Task_vtblHandleError,
    SHT40Task_vtblOnEnterTaskControlLoop,
    SHT40Task_vtblForceExecuteStep,
    SHT40Task_vtblOnEnterPowerMode
  },

  /* class::temp_sensor_if_vtbl virtual table */
  {
    {
      {
        SHT40Task_vtblTempGetId,
        SHT40Task_vtblTempGetEventSourceIF,
        SHT40Task_vtblTempGetDataInfo
      },
      SHT40Task_vtblSensorEnable,
      SHT40Task_vtblSensorDisable,
      SHT40Task_vtblSensorIsEnabled,
      SHT40Task_vtblTempSensorGetDescription,
      SHT40Task_vtblTempSensorGetStatus,
      SHT40Task_vtblTempSensorGetStatusPointer
    },
    SHT40Task_vtblTempGetODR,
    SHT40Task_vtblTempGetFS,
    SHT40Task_vtblTempGetSensitivity,
    SHT40Task_vtblSensorSetODR,
    SHT40Task_vtblSensorSetFS,
    SHT40Task_vtblSensorSetFifoWM
  },

  /* class::hum_sensor_if_vtbl virtual table */
  {
    {
      {
        SHT40Task_vtblHumGetId,
        SHT40Task_vtblHumGetEventSourceIF,
        SHT40Task_vtblHumGetDataInfo
      },
      SHT40Task_vtblSensorEnable,
      SHT40Task_vtblSensorDisable,
      SHT40Task_vtblSensorIsEnabled,
      SHT40Task_vtblHumSensorGetDescription,
      SHT40Task_vtblHumSensorGetStatus,
      SHT40Task_vtblHumSensorGetStatusPointer
    },
    SHT40Task_vtblHumGetODR,
    SHT40Task_vtblHumGetFS,
    SHT40Task_vtblHumGetSensitivity,
    SHT40Task_vtblSensorSetODR,
    SHT40Task_vtblSensorSetFS,
    SHT40Task_vtblSensorSetFifoWM
  },

  /* TEMPERATURE DESCRIPTOR */
  {
    "sht40",
    COM_TYPE_TEMP
  },

  /* HUMIDITY DESCRIPTOR */
  {
    "sht40",
    COM_TYPE_HUM
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    SHT40TaskExecuteStepState1,
    NULL,
    SHT40TaskExecuteStepDatalog,
  },
  {
    {
      0
    }
  }, /* task_map_elements */
  {
    0
  } /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *SHT40TaskGetTempSensorIF(SHT40Task *_this)
{
  return (ISourceObservable *) & (_this->temp_sensor_if);
}

ISourceObservable *SHT40TaskGetHumSensorIF(SHT40Task *_this)
{
  return (ISourceObservable *) & (_this->hum_sensor_if);
}

AManagedTaskEx *SHT40TaskAlloc(const void *pIRQConfig, const void *pCSConfig)
{
  UNUSED(pCSConfig);
  UNUSED(pIRQConfig);
  SHT40Task *p_new_obj = SysAlloc(sizeof(SHT40Task));

  if (p_new_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_new_obj->super);

    p_new_obj->super.vptr = &sTheClass.vtbl;
    p_new_obj->temp_sensor_if.vptr = &sTheClass.temp_sensor_if_vtbl;
    p_new_obj->hum_sensor_if.vptr = &sTheClass.hum_sensor_if_vtbl;
    p_new_obj->temp_sensor_descriptor = &sTheClass.temp_class_descriptor;
    p_new_obj->hum_sensor_descriptor = &sTheClass.hum_class_descriptor;

    strcpy(p_new_obj->temp_sensor_status.p_name, sTheClass.temp_class_descriptor.p_name);
    strcpy(p_new_obj->hum_sensor_status.p_name, sTheClass.hum_class_descriptor.p_name);
  }
  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *SHT40TaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *p_name)
{
  UNUSED(pCSConfig);
  UNUSED(pIRQConfig);
  SHT40Task *p_new_obj = (SHT40Task *) SHT40TaskAlloc(pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->temp_sensor_status.p_name, p_name);
  strcpy(p_new_obj->hum_sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *SHT40TaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig)
{
  UNUSED(pCSConfig);
  UNUSED(pIRQConfig);
  SHT40Task *p_obj = (SHT40Task *) p_mem_block;

  if (p_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_obj->super);
    p_obj->super.vptr = &sTheClass.vtbl;

    p_obj->super.vptr = &sTheClass.vtbl;
    p_obj->temp_sensor_if.vptr = &sTheClass.temp_sensor_if_vtbl;
    p_obj->hum_sensor_if.vptr = &sTheClass.hum_sensor_if_vtbl;
    p_obj->temp_sensor_descriptor = &sTheClass.temp_class_descriptor;
    p_obj->hum_sensor_descriptor = &sTheClass.hum_class_descriptor;
  }

  return (AManagedTaskEx *) p_obj;
}

AManagedTaskEx *SHT40TaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                            const char *p_name)
{
  UNUSED(pCSConfig);
  UNUSED(pIRQConfig);
  SHT40Task *p_obj = (SHT40Task *) SHT40TaskStaticAlloc(p_mem_block, pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->temp_sensor_status.p_name, p_name);
  strcpy(p_obj->hum_sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *SHT40TaskGetSensorIF(SHT40Task *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *SHT40TaskGetTempEventSrcIF(SHT40Task *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_temp_event_src;
}

IEventSrc *SHT40TaskGetHumEventSrcIF(SHT40Task *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_hum_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t SHT40Task_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
//  SHT40Task *p_obj = (SHT40Task *) _this;

  return res;
}

sys_error_code_t SHT40Task_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                            VOID **pvStackStart,
                                            ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart, ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SHT40Task *p_obj = (SHT40Task *) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) SHT40_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(SHT40_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "SHT40_Q", item_size / 4u, p_queue_items_buff,
                                    SHT40_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_timer, "SHT40_T", SHT40TaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(SHT40_TASK_CFG_TIMER_PERIOD_MS), 0,
                         TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Alloc the bus interface (SHT40 supports only I2C) */
  p_obj->p_sensor_bus_if = I2CBusIFAllocTransmitReceive(SHT40AD1B_ID, SHT40_TASK_CFG_I2C_ADDRESS, 0);
  if (p_obj->p_sensor_bus_if == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
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

  p_obj->p_hum_event_src = DataEventSrcAlloc();
  if (p_obj->p_hum_event_src == NULL)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }
  IEventSrcInit(p_obj->p_hum_event_src);

  if (!MTMap_IsInitialized(&sTheClass.task_map))
  {
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, SHT40_TASK_CFG_MAX_INSTANCES_COUNT);
  }

  p_obj->temp_id = 0;
  p_obj->hum_id = 0;
  p_obj->prev_timestamp = 0.0f;
  p_obj->temperature = 0.0f;
  p_obj->humidity = 0.0f;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "SHT40";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = SHT40_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = SHT40_TASK_CFG_PRIORITY;
  *pPreemptThreshold = SHT40_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = SHT40TaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = SHT40TaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SHT40: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t SHT40Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SHT40Task *p_obj = (SHT40Task *) _this;
//  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (SHT40TaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SHT40: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      tx_timer_deactivate(&p_obj->read_timer);
    }
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SHT40: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    /* notify the bus */
    if (p_obj->p_sensor_bus_if->m_pfBusCtrl != NULL)
    {
      p_obj->p_sensor_bus_if->m_pfBusCtrl(p_obj->p_sensor_bus_if, E_BUS_CTRL_DEV_NOTIFY_POWER_MODE, 0);
    }
    tx_timer_deactivate(&p_obj->read_timer);
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SHT40: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t SHT40Task_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  // SHT40Task *p_obj = (SHT40Task*)_this;

  return res;
}

sys_error_code_t SHT40Task_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("SHT40: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (SHT40_TASK_CFG_TAG)
  SHT40Task *p_obj = (SHT40Task *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = SHT40_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t SHT40Task_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SHT40Task *p_obj = (SHT40Task *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = SHT40TaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t SHT40Task_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  SHT40Task *p_obj = (SHT40Task*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t SHT40Task_vtblTempGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  SHT40Task *p_if_owner = (SHT40Task *)((uint32_t) _this - offsetof(SHT40Task, temp_sensor_if));
  uint8_t res = p_if_owner->temp_id;

  return res;
}

uint8_t SHT40Task_vtblHumGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  SHT40Task *p_if_owner = (SHT40Task *)((uint32_t) _this - offsetof(SHT40Task, hum_sensor_if));
  uint8_t res = p_if_owner->hum_id;

  return res;
}

IEventSrc *SHT40Task_vtblTempGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  SHT40Task *p_if_owner = (SHT40Task *)((uint32_t) _this - offsetof(SHT40Task, temp_sensor_if));
  return p_if_owner->p_temp_event_src;
}

IEventSrc *SHT40Task_vtblHumGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  SHT40Task *p_if_owner = (SHT40Task *)((uint32_t) _this - offsetof(SHT40Task, hum_sensor_if));
  return p_if_owner->p_hum_event_src;
}

sys_error_code_t SHT40Task_vtblTempGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF((ISensor_t *)_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if ((p_measured == NULL) || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = p_if_owner->temp_sensor_status.type.mems.measured_odr;
    *p_nominal = p_if_owner->temp_sensor_status.type.mems.odr;
  }

  return res;
}

float SHT40Task_vtblTempGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF((ISensor_t *)_this);
  float res = p_if_owner->temp_sensor_status.type.mems.fs;

  return res;
}

float SHT40Task_vtblTempGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF((ISensor_t *)_this);
  float res = p_if_owner->temp_sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t SHT40Task_vtblTempGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  SHT40Task *p_if_owner = (SHT40Task *)((uint32_t) _this - offsetof(SHT40Task, temp_sensor_if));
  EMData_t res = p_if_owner->temp_data;

  return res;
}

sys_error_code_t SHT40Task_vtblHumGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF((ISensor_t *)_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if ((p_measured == NULL) || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = p_if_owner->hum_sensor_status.type.mems.measured_odr;
    *p_nominal = p_if_owner->hum_sensor_status.type.mems.odr;
  }

  return res;
}

float SHT40Task_vtblHumGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF((ISensor_t *)_this);
  float res = p_if_owner->hum_sensor_status.type.mems.fs;

  return res;
}

float SHT40Task_vtblHumGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF((ISensor_t *)_this);
  float res = p_if_owner->hum_sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t SHT40Task_vtblHumGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF((ISensor_t *)_this);
  EMData_t res = p_if_owner->hum_data;

  return res;
}

sys_error_code_t SHT40Task_vtblSensorSetODR(ISensorMems_t *_this, float odr)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF((ISensor_t *)_this);
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    if (odr > 0.0f)
    {
      p_if_owner->hum_sensor_status.type.mems.odr = odr;
      p_if_owner->hum_sensor_status.type.mems.measured_odr = 0.0f;
      p_if_owner->temp_sensor_status.type.mems.odr = odr;
      p_if_owner->temp_sensor_status.type.mems.measured_odr = 0.0f;
    }
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_ODR,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.fParam = (float) odr
    };
    res = SHT40TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t SHT40Task_vtblSensorSetFS(ISensorMems_t *_this, float fs)
{
  assert_param(_this != NULL);
  /* Does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
  SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("SHT40: warning - SetFS() not supported.\r\n"));
  return SYS_INVALID_FUNC_CALL_ERROR_CODE ;
}

sys_error_code_t SHT40Task_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  /* Does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
  SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("SHT40: warning - SetFifoWM() not supported.\r\n"));
  return SYS_INVALID_FUNC_CALL_ERROR_CODE ;
}

sys_error_code_t SHT40Task_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF(_this);
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->hum_sensor_status.is_active = TRUE;
    p_if_owner->temp_sensor_status.is_active = TRUE;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_ENABLE,
      .sensorMessage.nSensorId = sensor_id
    };
    res = SHT40TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t SHT40Task_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF(_this);
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->hum_sensor_status.is_active = FALSE;
    p_if_owner->temp_sensor_status.is_active = FALSE;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_DISABLE,
      .sensorMessage.nSensorId = sensor_id
    };
    res = SHT40TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t SHT40Task_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF(_this);

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->temp_id)
  {
    res = p_if_owner->temp_sensor_status.is_active;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

SensorDescriptor_t SHT40Task_vtblTempSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->temp_sensor_descriptor;
}

SensorStatus_t SHT40Task_vtblTempSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF(_this);
  return p_if_owner->temp_sensor_status;
}

SensorStatus_t *SHT40Task_vtblTempSensorGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF(_this);
  return &p_if_owner->temp_sensor_status;
}

SensorDescriptor_t SHT40Task_vtblHumSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->hum_sensor_descriptor;
}

SensorStatus_t SHT40Task_vtblHumSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF(_this);
  return p_if_owner->hum_sensor_status;
}

SensorStatus_t *SHT40Task_vtblHumSensorGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  SHT40Task *p_if_owner = SHT40TaskGetOwnerFromISensorIF(_this);
  return &p_if_owner->hum_sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t SHT40TaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SHT40Task *p_obj = (SHT40Task *) _this;
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
            res = SHT40TaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = SHT40TaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = SHT40TaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SHT40: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SHT40: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t SHT40TaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SHT40Task *p_obj = (SHT40Task *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("SHT40: new data.\r\n"));

        res = SHT40TaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          // notify the listeners...
          double timestamp = report.sensorDataReadyMessage.fTimestamp;
          double delta_timestamp = timestamp - p_obj->prev_timestamp;
          p_obj->prev_timestamp = timestamp;

          if (p_obj->temp_sensor_status.is_active)
          {
            /* update measuredODR */
            p_obj->temp_sensor_status.type.mems.measured_odr = 1.0f / (float) delta_timestamp;

            EMD_1dInit(&p_obj->temp_data, (uint8_t *) &p_obj->temperature, E_EM_FLOAT, 1);

            DataEvent_t temp_evt;

            DataEventInit((IEvent *) &temp_evt, p_obj->p_temp_event_src, &p_obj->temp_data, timestamp, p_obj->temp_id);
            IEventSrcSendEvent(p_obj->p_temp_event_src, (IEvent *) &temp_evt, NULL);
          }
          if (p_obj->hum_sensor_status.is_active)
          {
            /* update measuredODR */
            p_obj->hum_sensor_status.type.mems.measured_odr = 1.0f / (float) delta_timestamp;

            EMD_1dInit(&p_obj->hum_data, (uint8_t *) &p_obj->humidity, E_EM_FLOAT, 1);

            DataEvent_t hum_evt;

            DataEventInit((IEvent *) &hum_evt, p_obj->p_hum_event_src, &p_obj->hum_data, timestamp, p_obj->hum_id);
            IEventSrcSendEvent(p_obj->p_hum_event_src, (IEvent *) &hum_evt, NULL);
          }

          SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("SHT40: ts = %f\r\n", (float)timestamp));
        }
        break;
      }

      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
            res = SHT40TaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->temp_sensor_status.is_active || p_obj->hum_sensor_status.is_active)
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
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = SHT40TaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = SHT40TaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = SHT40TaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SHT40: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SHT40: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t SHT40TaskPostReportToFront(SHT40Task *_this, SMMessage *pReport)
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

static inline sys_error_code_t SHT40TaskPostReportToBack(SHT40Task *_this, SMMessage *pReport)
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

static sys_error_code_t SHT40TaskSensorInit(SHT40Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  if (_this->temp_sensor_status.is_active || _this->hum_sensor_status.is_active)
  {
    uint8_t reset_cmd = 0x94U;
    sht40ad1b_write_reg(p_sensor_drv, 0, &reset_cmd, 1);

    _this->task_delay = (uint16_t)(1000.0f / _this->temp_sensor_status.type.mems.odr);
  }

  return res;
}

static sys_error_code_t SHT40TaskSensorReadData(SHT40Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float data[2];

  sht40ad1b_data_get(p_sensor_drv, data);
  _this->humidity = data[0];
  _this->temperature = data[1];

#if (HSD_USE_DUMMY_DATA == 1)
  _this->temperature = (float) dummyDataCounter++;
  _this->humidity = (float) dummyDataCounter++;
#endif

  return res;
}

static sys_error_code_t SHT40TaskSensorRegister(SHT40Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *temp_if = (ISensor_t *) SHT40TaskGetTempSensorIF(_this);
  ISensor_t *hum_if = (ISensor_t *) SHT40TaskGetHumSensorIF(_this);
  _this->temp_id = SMAddSensor(temp_if);
  _this->hum_id = SMAddSensor(hum_if);

  return res;
}

static sys_error_code_t SHT40TaskSensorInitTaskParams(SHT40Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* TEMPERATURE SENSOR STATUS */
  _this->temp_sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->temp_sensor_status.is_active = TRUE;
  _this->temp_sensor_status.type.mems.fs = 100.0f;
  _this->temp_sensor_status.type.mems.sensitivity = 1.0f;
  _this->temp_sensor_status.type.mems.odr = 1.0f;
  _this->temp_sensor_status.type.mems.measured_odr = 0.0f;
  EMD_1dInit(&_this->temp_data, (uint8_t *) &_this->temperature, E_EM_FLOAT, 1);

  /* HUMIDITY SENSOR STATUS */
  _this->hum_sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->hum_sensor_status.is_active = TRUE;
  _this->hum_sensor_status.type.mems.fs = 100.0f;
  _this->hum_sensor_status.type.mems.sensitivity = 1.0f;
  _this->hum_sensor_status.type.mems.odr = 1.0f;
  _this->hum_sensor_status.type.mems.measured_odr = 0.0f;
  EMD_1dInit(&_this->hum_data, (uint8_t *) &_this->humidity, E_EM_FLOAT, 1);

  return res;
}

static sys_error_code_t SHT40TaskSensorSetODR(SHT40Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  float odr = (float) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->temp_id || id == _this->hum_id)
  {
    _this->temp_sensor_status.type.mems.odr = odr;
    _this->temp_sensor_status.type.mems.measured_odr = 0.0f;
    _this->hum_sensor_status.type.mems.odr = odr;
    _this->hum_sensor_status.type.mems.measured_odr = 0.0f;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t SHT40TaskSensorEnable(SHT40Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->temp_id)
  {
    _this->temp_sensor_status.is_active = TRUE;
  }
  else if (id == _this->hum_id)
  {
    _this->hum_sensor_status.is_active = TRUE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t SHT40TaskSensorDisable(SHT40Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->temp_id)
  {
    _this->temp_sensor_status.is_active = FALSE;
  }
  else if (id == _this->hum_id)
  {
    _this->hum_sensor_status.is_active = FALSE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t SHT40TaskSensorIsActive(const SHT40Task *_this)
{
  assert_param(_this != NULL);
  return _this->temp_sensor_status.is_active;
}

static inline SHT40Task *SHT40TaskGetOwnerFromISensorIF(ISensor_t *p_if)
{
  assert_param(p_if != NULL);
  SHT40Task *p_if_owner = NULL;

  /* check if the virtual function has been called from the temp sensor IF  */
  p_if_owner = (SHT40Task *)((uint32_t) p_if - offsetof(SHT40Task, hum_sensor_if));
  if (!(p_if_owner->temp_sensor_if.vptr == &sTheClass.temp_sensor_if_vtbl)
      || !(p_if_owner->super.vptr == &sTheClass.vtbl))
  {
    /* then the virtual function has been called from the hum IF  */
    p_if_owner = (SHT40Task *)((uint32_t) p_if - offsetof(SHT40Task, temp_sensor_if));
  }

  return p_if_owner;
}

static void SHT40TaskTimerCallbackFunction(ULONG param)
{
  SHT40Task *p_obj = (SHT40Task *) param;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  if (TX_SUCCESS != tx_queue_send(&p_obj->in_queue, &report, TX_NO_WAIT))
  {
    /* unable to send the report. Signal the error */
    sys_error_handler();
  }
}

