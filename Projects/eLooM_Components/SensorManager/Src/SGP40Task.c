/**
  ******************************************************************************
  * @file    SGP40Task.c
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
#include "SGP40Task.h"
#include "SGP40Task_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "sgp40_reg.h"
#include <string.h>
#include "services/sysdebug.h"

/* Private includes ----------------------------------------------------------*/
#ifndef SGP40_TASK_CFG_STACK_DEPTH
#define SGP40_TASK_CFG_STACK_DEPTH        (TX_MINIMUM_STACK*8)
#endif

#ifndef SGP40_TASK_CFG_PRIORITY
#define SGP40_TASK_CFG_PRIORITY           4
#endif

#ifndef SGP40_TASK_CFG_IN_QUEUE_LENGTH
#define SGP40_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define SGP40_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef SGP40_TASK_CFG_TIMER_PERIOD_MS
#define SGP40_TASK_CFG_TIMER_PERIOD_MS          250
#endif

#ifndef SGP40_TASK_CFG_MAX_INSTANCES_COUNT
#define SGP40_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_SGP40, level, message)

#ifndef SGP40_TASK_CFG_I2C_ADDRESS
#define SGP40_TASK_CFG_I2C_ADDRESS              SGP40_I2C_ADDRESS
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
typedef struct _SGP40TaskClass
{
  /**
    * SGP40Task class virtual table.
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
    * SGP40Task (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[SGP40_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} SGP40TaskClass_t;

/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t SGP40TaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t SGP40TaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t SGP40TaskSensorInit(SGP40Task *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t SGP40TaskSensorReadData(SGP40Task *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t SGP40TaskSensorRegister(SGP40Task *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t SGP40TaskSensorInitTaskParams(SGP40Task *_this);

/**
  * Private implementation of sensor interface methods for SGP40 sensor
  */
static sys_error_code_t SGP40TaskSensorSetODR(SGP40Task *_this, SMMessage report);
static sys_error_code_t SGP40TaskSensorEnable(SGP40Task *_this, SMMessage report);
static sys_error_code_t SGP40TaskSensorDisable(SGP40Task *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t SGP40TaskSensorIsActive(const SGP40Task *_this);
/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void SGP40TaskTimerCallbackFunction(ULONG param);

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
static inline sys_error_code_t SGP40TaskPostReportToFront(SGP40Task *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t SGP40TaskPostReportToBack(SGP40Task *_this, SMMessage *pReport);

/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static SGP40Task sTaskObj;
/**
  * The class object.
  */
static SGP40TaskClass_t sTheClass =
{
  /* class virtual table */
  {
    SGP40Task_vtblHardwareInit,
    SGP40Task_vtblOnCreateTask,
    SGP40Task_vtblDoEnterPowerMode,
    SGP40Task_vtblHandleError,
    SGP40Task_vtblOnEnterTaskControlLoop,
    SGP40Task_vtblForceExecuteStep,
    SGP40Task_vtblOnEnterPowerMode
  },

  /* class::sensor_if_vtbl virtual table */
  {
    {
      {
        SGP40Task_vtblTempGetId,
        SGP40Task_vtblTempGetEventSourceIF,
        SGP40Task_vtblTempGetDataInfo
      },
      SGP40Task_vtblSensorEnable,
      SGP40Task_vtblSensorDisable,
      SGP40Task_vtblSensorIsEnabled,
      SGP40Task_vtblSensorGetDescription,
      SGP40Task_vtblSensorGetStatus,
      SGP40Task_vtblSensorGetStatusPointer
    },
    SGP40Task_vtblTempGetODR,
    SGP40Task_vtblTempGetFS,
    SGP40Task_vtblTempGetSensitivity,
    SGP40Task_vtblSensorSetODR,
    SGP40Task_vtblSensorSetFS,
    SGP40Task_vtblSensorSetFifoWM
  },

  /* TEMPERATURE DESCRIPTOR */
  {
    "sgp40",
    COM_TYPE_TEMP
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    SGP40TaskExecuteStepState1,
    NULL,
    SGP40TaskExecuteStepDatalog,
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
ISourceObservable *SGP40TaskGetTempSensorIF(SGP40Task *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *SGP40TaskAlloc(const void *pIRQConfig, const void *pCSConfig)
{
  UNUSED(pCSConfig);
  UNUSED(pIRQConfig);
  SGP40Task *p_new_obj = SysAlloc(sizeof(SGP40Task));

  if (p_new_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_new_obj->super);

    p_new_obj->super.vptr = &sTheClass.vtbl;
    p_new_obj->sensor_if.vptr = &sTheClass.sensor_if_vtbl;
    p_new_obj->sensor_descriptor = &sTheClass.class_descriptor;

    strcpy(p_new_obj->sensor_status.p_name, sTheClass.class_descriptor.p_name);
  }
  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *SGP40TaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *p_name)
{
  UNUSED(pCSConfig);
  UNUSED(pIRQConfig);
  SGP40Task *p_new_obj = (SGP40Task *) SGP40TaskAlloc(pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *SGP40TaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig)
{
  UNUSED(pCSConfig);
  UNUSED(pIRQConfig);
  SGP40Task *p_obj = (SGP40Task *) p_mem_block;

  if (p_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_obj->super);
    p_obj->super.vptr = &sTheClass.vtbl;

    p_obj->super.vptr = &sTheClass.vtbl;
    p_obj->sensor_if.vptr = &sTheClass.sensor_if_vtbl;
    p_obj->sensor_descriptor = &sTheClass.class_descriptor;
  }

  return (AManagedTaskEx *) p_obj;
}

AManagedTaskEx *SGP40TaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                            const char *p_name)
{
  UNUSED(pCSConfig);
  UNUSED(pIRQConfig);
  SGP40Task *p_obj = (SGP40Task *) SGP40TaskStaticAlloc(p_mem_block, pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *SGP40TaskGetSensorIF(SGP40Task *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *SGP40TaskGetTempEventSrcIF(SGP40Task *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_temp_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t SGP40Task_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
//  SGP40Task *p_obj = (SGP40Task *) _this;

  return res;
}

sys_error_code_t SGP40Task_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                            VOID **pvStackStart,
                                            ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart, ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SGP40Task *p_obj = (SGP40Task *) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) SGP40_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(SGP40_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "SGP40_Q", item_size / 4u, p_queue_items_buff,
                                    SGP40_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_fifo_timer, "SGP40_T", SGP40TaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(SGP40_TASK_CFG_TIMER_PERIOD_MS), 0,
                         TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Alloc the bus interface (SGP40 supports only I2C) */
  p_obj->p_sensor_bus_if = I2CBusIFAllocTransmitReceive(SGP40_ID, SGP40_TASK_CFG_I2C_ADDRESS, 0);
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

  if (!MTMap_IsInitialized(&sTheClass.task_map))
  {
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, SGP40_TASK_CFG_MAX_INSTANCES_COUNT);
  }

  p_obj->temp_id = 0;
  p_obj->prev_timestamp = 0.0f;
  p_obj->sraw_voc = 0;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "SGP40";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = SGP40_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = SGP40_TASK_CFG_PRIORITY;
  *pPreemptThreshold = SGP40_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = SGP40TaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = SGP40TaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SGP40: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t SGP40Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SGP40Task *p_obj = (SGP40Task *) _this;
//  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (SGP40TaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SGP40: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      tx_timer_deactivate(&p_obj->read_fifo_timer);
    }
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SGP40: -> STATE1\r\n"));
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
    tx_timer_deactivate(&p_obj->read_fifo_timer);

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SGP40: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t SGP40Task_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  // SGP40Task *p_obj = (SGP40Task*)_this;

  return res;
}

sys_error_code_t SGP40Task_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("SGP40: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (SGP40_TASK_CFG_TAG)
  SGP40Task *p_obj = (SGP40Task *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = SGP40_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t SGP40Task_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SGP40Task *p_obj = (SGP40Task *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = SGP40TaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t SGP40Task_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  SGP40Task *p_obj = (SGP40Task*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t SGP40Task_vtblTempGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  SGP40Task *p_if_owner = (SGP40Task *)((uint32_t) _this - offsetof(SGP40Task, sensor_if));
  uint8_t res = p_if_owner->temp_id;

  return res;
}

IEventSrc *SGP40Task_vtblTempGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  SGP40Task *p_if_owner = (SGP40Task *)((uint32_t) _this - offsetof(SGP40Task, sensor_if));
  return p_if_owner->p_temp_event_src;
}

sys_error_code_t SGP40Task_vtblTempGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  SGP40Task *p_if_owner = (SGP40Task *)((uint32_t) _this - offsetof(SGP40Task, sensor_if));
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

float_t SGP40Task_vtblTempGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  SGP40Task *p_if_owner = (SGP40Task *)((uint32_t) _this - offsetof(SGP40Task, sensor_if));
  float_t res = p_if_owner->sensor_status.type.mems.fs;

  return res;
}

float_t SGP40Task_vtblTempGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  SGP40Task *p_if_owner = (SGP40Task *)((uint32_t) _this - offsetof(SGP40Task, sensor_if));
  float_t res = p_if_owner->sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t SGP40Task_vtblTempGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  SGP40Task *p_if_owner = (SGP40Task *)((uint32_t) _this - offsetof(SGP40Task, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t SGP40Task_vtblSensorSetODR(ISensorMems_t *_this, float_t odr)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SGP40Task *p_if_owner = (SGP40Task *)((uint32_t) _this - offsetof(SGP40Task, sensor_if));
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
    res = SGP40TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t SGP40Task_vtblSensorSetFS(ISensorMems_t *_this, float_t fs)
{
  assert_param(_this != NULL);
  /* Does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
  SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("SGP40: warning - SetFS() not supported.\r\n"));
  return SYS_INVALID_FUNC_CALL_ERROR_CODE ;
}

sys_error_code_t SGP40Task_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  /* Does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
  SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("SGP40: warning - SetFifoWM() not supported.\r\n"));
  return SYS_INVALID_FUNC_CALL_ERROR_CODE ;
}

sys_error_code_t SGP40Task_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SGP40Task *p_if_owner = (SGP40Task *)((uint32_t) _this - offsetof(SGP40Task, sensor_if));
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
    res = SGP40TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t SGP40Task_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SGP40Task *p_if_owner = (SGP40Task *)((uint32_t) _this - offsetof(SGP40Task, sensor_if));
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
    res = SGP40TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t SGP40Task_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  SGP40Task *p_if_owner = (SGP40Task *)((uint32_t) _this - offsetof(SGP40Task, sensor_if));

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

SensorDescriptor_t SGP40Task_vtblSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  SGP40Task *p_if_owner = (SGP40Task *)((uint32_t) _this - offsetof(SGP40Task, sensor_if));
  return *p_if_owner->sensor_descriptor;

}

SensorStatus_t SGP40Task_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  SGP40Task *p_if_owner = (SGP40Task *)((uint32_t) _this - offsetof(SGP40Task, sensor_if));

  return p_if_owner->sensor_status;
}

SensorStatus_t *SGP40Task_vtblSensorGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  SGP40Task *p_if_owner = (SGP40Task *)((uint32_t) _this - offsetof(SGP40Task, sensor_if));

  return &p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t SGP40TaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SGP40Task *p_obj = (SGP40Task *) _this;
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
            res = SGP40TaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = SGP40TaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = SGP40TaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SGP40: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SGP40: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t SGP40TaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SGP40Task *p_obj = (SGP40Task *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("SGP40: new data.\r\n"));

        res = SGP40TaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          // notify the listeners...
          double_t timestamp = report.sensorDataReadyMessage.fTimestamp;
          double_t delta_timestamp = timestamp - p_obj->prev_timestamp;
          p_obj->prev_timestamp = timestamp;

          /* update measuredODR */
          p_obj->sensor_status.type.mems.measured_odr = 1.0f / (float_t) delta_timestamp;

          EMD_1dInit(&p_obj->data, (uint8_t *) &p_obj->sraw_voc, E_EM_UINT16, 1);

          DataEvent_t evt;

          DataEventInit((IEvent *) &evt, p_obj->p_temp_event_src, &p_obj->data, timestamp, p_obj->temp_id);
          IEventSrcSendEvent(p_obj->p_temp_event_src, (IEvent *) &evt, NULL);

          SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("SGP40: ts = %f\r\n", (float_t)timestamp));
        }
        break;
      }

      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
            res = SGP40TaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->sensor_status.is_active == true)
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
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = SGP40TaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = SGP40TaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = SGP40TaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SGP40: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SGP40: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t SGP40TaskPostReportToFront(SGP40Task *_this, SMMessage *pReport)
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

static inline sys_error_code_t SGP40TaskPostReportToBack(SGP40Task *_this, SMMessage *pReport)
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

static sys_error_code_t SGP40TaskSensorInit(SGP40Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  if (_this->sensor_status.is_active)
  {
    uint8_t reset_cmd = 0x06U;
    sgp40_write_reg(p_sensor_drv, 0, &reset_cmd, 1);

    _this->task_delay = (uint16_t)(1000.0f / _this->sensor_status.type.mems.odr);
  }

  return res;
}

static sys_error_code_t SGP40TaskSensorReadData(SGP40Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  res = sgp40_data_get(p_sensor_drv, &_this->sraw_voc, NULL);

  if (!SYS_IS_ERROR_CODE(res))
  {
#if (HSD_USE_DUMMY_DATA == 1)
    _this->sraw_voc = dummyDataCounter++;
#endif
  }

  return res;
}

static sys_error_code_t SGP40TaskSensorRegister(SGP40Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *temp_if = (ISensor_t *) SGP40TaskGetTempSensorIF(_this);
  _this->temp_id = SMAddSensor(temp_if);

  return res;
}

static sys_error_code_t SGP40TaskSensorInitTaskParams(SGP40Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* TEMPERATURE SENSOR STATUS */
  _this->sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->sensor_status.is_active = TRUE;
  _this->sensor_status.type.mems.fs = 100.0f;
  _this->sensor_status.type.mems.sensitivity = 1.0f;
  _this->sensor_status.type.mems.odr = 1.0f;
  _this->sensor_status.type.mems.measured_odr = 0.0f;
  EMD_1dInit(&_this->data, (uint8_t *) &_this->sraw_voc, E_EM_UINT16, 1);

  return res;
}

static sys_error_code_t SGP40TaskSensorSetODR(SGP40Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  float_t odr = (float_t) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->temp_id)
  {
    _this->sensor_status.type.mems.odr = odr;
    _this->sensor_status.type.mems.measured_odr = 0.0f;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t SGP40TaskSensorEnable(SGP40Task *_this, SMMessage report)
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

static sys_error_code_t SGP40TaskSensorDisable(SGP40Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->temp_id)
  {
    _this->sensor_status.is_active = FALSE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t SGP40TaskSensorIsActive(const SGP40Task *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.is_active;
}

static void SGP40TaskTimerCallbackFunction(ULONG param)
{
  SGP40Task *p_obj = (SGP40Task *) param;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  if (TX_SUCCESS != tx_queue_send(&p_obj->in_queue, &report, TX_NO_WAIT))
  {
    /* unable to send the report. Signal the error */
    sys_error_handler();
  }
}

