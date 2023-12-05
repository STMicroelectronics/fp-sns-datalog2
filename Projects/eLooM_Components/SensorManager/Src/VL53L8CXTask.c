/**
  ******************************************************************************
  * @file    VL53L8CXTask.c
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
#include "VL53L8CXTask.h"
#include "VL53L8CXTask_vtbl.h"
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

#ifndef VL53L8CX_TASK_CFG_STACK_DEPTH
#define VL53L8CX_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*14)
#endif

#ifndef VL53L8CX_TASK_CFG_PRIORITY
#define VL53L8CX_TASK_CFG_PRIORITY                 (4)
#endif

#ifndef VL53L8CX_TASK_CFG_IN_QUEUE_LENGTH
#define VL53L8CX_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define VL53L8CX_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef VL53L8CX_TASK_CFG_TIMER_PERIOD_MS
#define VL53L8CX_TASK_CFG_TIMER_PERIOD_MS          500
#endif

#ifndef VL53L8CX_TASK_CFG_MAX_INSTANCES_COUNT
#define VL53L8CX_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_VL53L8CX, level, message)

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter = 0;
#endif


/**
  * Class object declaration
  */
typedef struct _VL53L8CXTaskClass
{
  /**
    * VL53L8CXTask class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * Time-of-flight IF virtual table.
    */
  const ISensorRanging_vtbl sensor_if_vtbl;

  /**
    * Specifies time-of-flight sensor capabilities.
    */
  const SensorDescriptor_t class_descriptor;

  /**
    * VL53L8CXTask (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[VL53L8CX_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} VL53L8CXTaskClass_t;

/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t VL53L8CXTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t VL53L8CXTaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t VL53L8CXTaskSensorInit(VL53L8CXTask *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t VL53L8CXTaskSensorReadData(VL53L8CXTask *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t VL53L8CXTaskSensorRegister(VL53L8CXTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t VL53L8CXTaskSensorInitTaskParams(VL53L8CXTask *_this);

/**
  * Private implementation of sensor interface methods for VL53L8CX sensor
  */
static sys_error_code_t VL53L8CXTaskSensorSetFrequency(VL53L8CXTask *_this, SMMessage report);
static sys_error_code_t VL53L8CXTaskSensorSetResolution(VL53L8CXTask *_this, SMMessage report);
static sys_error_code_t VL53L8CXTaskSensorSetRangingMode(VL53L8CXTask *_this, SMMessage report);
static sys_error_code_t VL53L8CXTaskSensorSetIntegrationTime(VL53L8CXTask *_this, SMMessage report);
static sys_error_code_t VL53L8CXTaskSensorConfigIt(VL53L8CXTask *_this, SMMessage report);
static sys_error_code_t VL53L8CXTaskSensorSetAddress(VL53L8CXTask *_this, SMMessage report);
static sys_error_code_t VL53L8CXTaskSensorSetPowerMode(VL53L8CXTask *_this, SMMessage report);
static sys_error_code_t VL53L8CXTaskSensorEnable(VL53L8CXTask *_this, SMMessage report);
static sys_error_code_t VL53L8CXTaskSensorDisable(VL53L8CXTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t VL53L8CXTaskSensorIsActive(const VL53L8CXTask *_this);

static sys_error_code_t VL53L8CXTaskEnterLowPowerMode(const VL53L8CXTask *_this);

static sys_error_code_t VL53L8CXTaskConfigureIrqPin(const VL53L8CXTask *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void VL53L8CXTaskTimerCallbackFunction(ULONG param);



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
static inline sys_error_code_t VL53L8CXTaskPostReportToFront(VL53L8CXTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t VL53L8CXTaskPostReportToBack(VL53L8CXTask *_this, SMMessage *pReport);

/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
// static VL53L8CXTask sTaskObj;

/**
  * The class object.
  */
static VL53L8CXTaskClass_t sTheClass =
{
  /* Class virtual table */
  {
    VL53L8CXTask_vtblHardwareInit,
    VL53L8CXTask_vtblOnCreateTask,
    VL53L8CXTask_vtblDoEnterPowerMode,
    VL53L8CXTask_vtblHandleError,
    VL53L8CXTask_vtblOnEnterTaskControlLoop,
    VL53L8CXTask_vtblForceExecuteStep,
    VL53L8CXTask_vtblOnEnterPowerMode
  },
  /* class::sensor_if_vtbl virtual table */
  {
    {
      {
        VL53L8CXTask_vtblTofGetId,
        VL53L8CXTask_vtblGetEventSourceIF,
        VL53L8CXTask_vtblTofGetDataInfo
      },

      VL53L8CXTask_vtblSensorEnable,
      VL53L8CXTask_vtblSensorDisable,
      VL53L8CXTask_vtblSensorIsEnabled,
      VL53L8CXTask_vtblSensorGetDescription,
      VL53L8CXTask_vtblSensorGetStatus
    },

    VL53L8CXTask_vtblTofGetProfile,
    VL53L8CXTask_vtblTofGetIT,
    VL53L8CXTask_vtblTofGetAddress,
    VL53L8CXTask_vtblTofGetPowerMode,
    VL53L8CXTask_vtblSensorSetFrequency,
    VL53L8CXTask_vtblSensorSetResolution,
    VL53L8CXTask_vtblSensorSetRangingMode,
    VL53L8CXTask_vtblSensorSetIntegrationTime,
    VL53L8CXTask_vtblSensorConfigIT,
    VL53L8CXTask_vtblSensorSetAddress,
    VL53L8CXTask_vtblSensorSetPowerMode
  },
  /* TIME-OF-FLIGHT DESCRIPTOR */
  {
    "vl53l8cx",
    COM_TYPE_TOF,
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
    VL53L8CXTaskExecuteStepState1,
    NULL,
    VL53L8CXTaskExecuteStepDatalog,
  },
  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *VL53L8CXTaskGetTofSensorIF(VL53L8CXTask *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *VL53L8CXTaskAlloc(const void *pIRQConfig, const void *pCSConfig)
{
  VL53L8CXTask *p_new_obj = SysAlloc(sizeof(VL53L8CXTask));

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

AManagedTaskEx *VL53L8CXTaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *p_name)
{
  VL53L8CXTask *p_new_obj = (VL53L8CXTask *)VL53L8CXTaskAlloc(pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *VL53L8CXTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig)
{
  VL53L8CXTask *p_obj = (VL53L8CXTask *)p_mem_block;

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

AManagedTaskEx *VL53L8CXTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                              const char *p_name)
{
  VL53L8CXTask *p_obj = (VL53L8CXTask *)VL53L8CXTaskStaticAlloc(p_mem_block, pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *VL53L8CXTaskGetSensorIF(VL53L8CXTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *VL53L8CXTaskGetEventSrcIF(VL53L8CXTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t VL53L8CXTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  VL53L8CXTask *p_obj = (VL53L8CXTask *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t VL53L8CXTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                               VOID **pvStackStart,
                                               ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                               ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  VL53L8CXTask *p_obj = (VL53L8CXTask *) _this;

  /* Create task specific sw resources */

  uint32_t item_size = (uint32_t) VL53L8CX_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(VL53L8CX_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "VL53L8CX_Q", item_size / 4u, p_queue_items_buff,
                                         VL53L8CX_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* create the software timer*/
  if (TX_SUCCESS
           != tx_timer_create(&p_obj->read_timer, "VL53L8CX_T", VL53L8CXTaskTimerCallbackFunction, (ULONG)_this,
                              AMT_MS_TO_TICKS(VL53L8CX_TASK_CFG_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(VL53L8CX_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc16(VL53L8CX_ID, VL53L8CX_DEVICE_ADDRESS, 0);
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
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, VL53L8CX_TASK_CFG_MAX_INSTANCES_COUNT);
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
      p_obj->id = 0;
      p_obj->prev_timestamp = 0.0f;
      _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

      *pTaskCode = AMTExRun;
      *pName = "VL53L8CX";
      *pvStackStart = NULL; // allocate the task stack in the system memory pool.
      *pStackDepth = VL53L8CX_TASK_CFG_STACK_DEPTH;
      *pParams = (ULONG) _this;
      *pPriority = VL53L8CX_TASK_CFG_PRIORITY;
      *pPreemptThreshold = VL53L8CX_TASK_CFG_PRIORITY;
      *pTimeSlice = TX_NO_TIME_SLICE;
      *pAutoStart = TX_AUTO_START;

      res = VL53L8CXTaskSensorInitTaskParams(p_obj);
      if (SYS_IS_ERROR_CODE(res))
      {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
      }

        res = VL53L8CXTaskSensorRegister(p_obj);
        if (SYS_IS_ERROR_CODE(res))
        {
          SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("VL53L8CX: unable to register with DB\r\n"));
          sys_error_handler();
        }

  return res;
}

sys_error_code_t VL53L8CXTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  VL53L8CXTask *p_obj = (VL53L8CXTask *) _this;
  VL53L8CX_Platform *p_platform_drv = (VL53L8CX_Platform *) &p_obj->p_sensor_bus_if->m_xConnector;
  p_obj->tof_driver_if.platform = *p_platform_drv;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (VL53L8CXTaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("VL53L8CX: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      /* Deactivate the sensor */
      vl53l8cx_stop_ranging(&p_obj->tof_driver_if);

      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_timer);
      }
      else
      {
        VL53L8CXTaskConfigureIrqPin(p_obj, TRUE);
      }
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("VL53L8CX: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = VL53L8CXTaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("VL53L8CX - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->pIRQConfig != NULL)
    {
      VL53L8CXTaskConfigureIrqPin(p_obj, TRUE);
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("VL53L8CX: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t VL53L8CXTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  VL53L8CXTask *p_obj = (VL53L8CXTask*)_this;

  return res;
}

sys_error_code_t VL53L8CXTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("VL53L8CX: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (VL53L8CX_TASK_CFG_TAG)
  VL53L8CXTask *p_obj = (VL53L8CXTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = VL53L8CX_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t VL53L8CXTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  VL53L8CXTask *p_obj = (VL53L8CXTask *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = VL53L8CXTaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t VL53L8CXTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  VL53L8CXTask *p_obj = (VL53L8CXTask*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t VL53L8CXTask_vtblTofGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));
  uint8_t res = p_if_owner->id;

  return res;
}

IEventSrc *VL53L8CXTask_vtblGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));
  return p_if_owner->p_event_src;
}

sys_error_code_t VL53L8CXTask_vtblTofGetProfile(ISensorRanging_t *_this, ProfileConfig_t *p_config)
{
  assert_param(_this != NULL);
  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* TODO Needed getter from PID? */
  *p_config = p_if_owner->sensor_status.type.ranging.profile_config;

  return res;
}

sys_error_code_t VL53L8CXTask_vtblTofGetIT(ISensorRanging_t *_this, ITConfig_t *p_it_config)
{
  assert_param(_this != NULL);
  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* TODO Needed getter from PID? */
  *p_it_config = p_if_owner->sensor_status.type.ranging.it_config;

  return res;
}

uint32_t VL53L8CXTask_vtblTofGetAddress(ISensorRanging_t *_this)
{
  assert_param(_this != NULL);
  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));
  uint32_t res = p_if_owner->sensor_status.type.ranging.address;

  return res;
}

uint32_t VL53L8CXTask_vtblTofGetPowerMode(ISensorRanging_t *_this)
{
  assert_param(_this != NULL);
  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));

  /* TODO Needed getter from PID? */
  uint32_t res = p_if_owner->sensor_status.type.ranging.power_mode;

  return res;
}

EMData_t VL53L8CXTask_vtblTofGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t VL53L8CXTask_vtblSensorSetFrequency(ISensorRanging_t *_this, uint32_t frequency)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FREQUENCY,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) frequency
    };
    res = VL53L8CXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t VL53L8CXTask_vtblSensorSetResolution(ISensorRanging_t *_this, uint8_t resolution)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_RESOLUTION,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) resolution
    };
    res = VL53L8CXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t VL53L8CXTask_vtblSensorSetRangingMode(ISensorRanging_t *_this, uint8_t mode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_RANGING_MODE,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) mode
    };
    res = VL53L8CXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t VL53L8CXTask_vtblSensorSetIntegrationTime(ISensorRanging_t *_this, uint32_t timing_budget)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_INTEGRATION_TIME,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) timing_budget
    };
    res = VL53L8CXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t VL53L8CXTask_vtblSensorConfigIT(ISensorRanging_t *_this, ITConfig_t *p_it_config)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_CONFIG_IT,
      .sensorMessage.nSensorId = sensor_id
//      .sensorMessage.nParam = (uint32_t) p_it_config
    };
    *(uint32_t*)&report.sensorMessage.nParam = (uint32_t) p_it_config;
    res = VL53L8CXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t VL53L8CXTask_vtblSensorSetAddress(ISensorRanging_t *_this, uint32_t address)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_ADDRESS,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = address
    };
    res = VL53L8CXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t VL53L8CXTask_vtblSensorSetPowerMode(ISensorRanging_t *_this, uint32_t power_mode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_POWERMODE,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = power_mode
    };
    res = VL53L8CXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t VL53L8CXTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));
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
    res = VL53L8CXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t VL53L8CXTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));
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
    res = VL53L8CXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t VL53L8CXTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;

  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));

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

SensorDescriptor_t VL53L8CXTask_vtblSensorGetDescription(ISensor_t *_this)
{

  assert_param(_this != NULL);
  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));
  return *p_if_owner->sensor_descriptor;
}

SensorStatus_t VL53L8CXTask_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  VL53L8CXTask *p_if_owner = (VL53L8CXTask *)((uint32_t) _this - offsetof(VL53L8CXTask, sensor_if));

  return p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t VL53L8CXTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  VL53L8CXTask *p_obj = (VL53L8CXTask *) _this;
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
          case SENSOR_CMD_ID_SET_FREQUENCY:
            res = VL53L8CXTaskSensorSetFrequency(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_INTEGRATION_TIME:
            res = VL53L8CXTaskSensorSetIntegrationTime(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_RESOLUTION:
            res = VL53L8CXTaskSensorSetResolution(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_RANGING_MODE:
            res = VL53L8CXTaskSensorSetRangingMode(p_obj, report);
            break;
          case SENSOR_CMD_ID_CONFIG_IT:
            res = VL53L8CXTaskSensorConfigIt(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_ADDRESS:
            res = VL53L8CXTaskSensorSetAddress(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_POWERMODE:
            res = VL53L8CXTaskSensorSetPowerMode(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = VL53L8CXTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = VL53L8CXTaskSensorDisable(p_obj, report);
            break;
          default:
          {
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("VL53L8CX: unexpected report in Run: %i\r\n", report.messageID));
            break;
          }
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("VL53L8CX: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }

    }
  }

  return res;
}

static sys_error_code_t VL53L8CXTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  VL53L8CXTask *p_obj = (VL53L8CXTask *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("VL53L8CX: new data.\r\n"));
//        if (p_obj->pIRQConfig == NULL)
//        {
//          //if(TX_SUCCESS != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(VL53L8CX_TASK_CFG_TIMER_PERIOD_MS), AMT_MS_TO_TICKS(VL53L8CX_TASK_CFG_TIMER_PERIOD_MS)))
//          if (TX_SUCCESS
//              != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->vl53l8cx_task_cfg_timer_period_ms),
//                                 AMT_MS_TO_TICKS(p_obj->vl53l8cx_task_cfg_timer_period_ms)))
//          {
//            return SYS_UNDEFINED_ERROR_CODE;
//          }
//        }
        res = VL53L8CXTaskSensorReadData(p_obj);

        uint8_t resolution;
        uint32_t profile = p_obj->sensor_status.type.ranging.profile_config.ranging_profile;

        switch (profile)
        {
          case VL53L8CX_PROFILE_4x4_CONTINUOUS:
          case VL53L8CX_PROFILE_4x4_AUTONOMOUS:
            resolution = VL53L8CX_RESOLUTION_4X4;
            break;
          case VL53L8CX_PROFILE_8x8_CONTINUOUS:
          case VL53L8CX_PROFILE_8x8_AUTONOMOUS:
            resolution = VL53L8CX_RESOLUTION_8X8;
            break;
          default:
            resolution = 0; /* silence MISRA rule 1.3 warning */
            res = VL53L8CX_INVALID_PARAM;
            break;
        }

        if (!SYS_IS_ERROR_CODE(res))
        {
            // notify the listeners...
            double timestamp = report.sensorDataReadyMessage.fTimestamp;

            EMD_Init(&p_obj->data, (uint8_t*) &p_obj->p_sensor_data_buff[0], E_EM_UINT32, E_EM_MODE_INTERLEAVED, 3, 1, resolution, 8);
            DataEvent_t evt;

            DataEventInit((IEvent *) &evt, p_obj->p_event_src, &p_obj->data, timestamp, p_obj->id);
            IEventSrcSendEvent(p_obj->p_event_src, (IEvent *) &evt, NULL);
          SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("VL53L8CX: ts = %f\r\n", (float)timestamp));
//          if (p_obj->pIRQConfig == NULL)
//          {
//            if (TX_SUCCESS != tx_timer_activate(&p_obj->read_timer))
//            {
//              res = SYS_UNDEFINED_ERROR_CODE;
//            }
//          }
        }
        break;
      }
      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
            res = VL53L8CXTaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->sensor_status.is_active == true)
              {
                if (p_obj->pIRQConfig == NULL)
                {
                  if (TX_SUCCESS
                      != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->vl53l8cx_task_cfg_timer_period_ms),
                                         AMT_MS_TO_TICKS(p_obj->vl53l8cx_task_cfg_timer_period_ms)))
                  {
                    return SYS_UNDEFINED_ERROR_CODE;
                  }
                  if (TX_SUCCESS != tx_timer_activate(&p_obj->read_timer))
                  {
                    res = SYS_UNDEFINED_ERROR_CODE;
                  }
                }
                else
                {
                  VL53L8CXTaskConfigureIrqPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_FREQUENCY:
            res = VL53L8CXTaskSensorSetFrequency(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_INTEGRATION_TIME:
            res = VL53L8CXTaskSensorSetIntegrationTime(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_RESOLUTION:
            res = VL53L8CXTaskSensorSetResolution(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_RANGING_MODE:
            res = VL53L8CXTaskSensorSetRangingMode(p_obj, report);
            break;
          case SENSOR_CMD_ID_CONFIG_IT:
            res = VL53L8CXTaskSensorConfigIt(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_ADDRESS:
            res = VL53L8CXTaskSensorSetAddress(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_POWERMODE:
            res = VL53L8CXTaskSensorSetPowerMode(p_obj, report);
          case SENSOR_CMD_ID_ENABLE:
            res = VL53L8CXTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = VL53L8CXTaskSensorDisable(p_obj, report);
            break;
          default:
          {
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("VL53L8CX: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
          }
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("VL53L8CX: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static inline sys_error_code_t VL53L8CXTaskPostReportToFront(VL53L8CXTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t VL53L8CXTaskPostReportToBack(VL53L8CXTask *_this, SMMessage *pReport)
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

static sys_error_code_t VL53L8CXTaskSensorInit(VL53L8CXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  VL53L8CX_Platform *p_platform_drv = (VL53L8CX_Platform *) &_this->p_sensor_bus_if->m_xConnector;
  _this->tof_driver_if.platform = *p_platform_drv;

  uint32_t id;
  uint8_t device_id = 0;
  uint8_t revision_id = 0;
  uint8_t status = VL53L8CX_STATUS_OK;

  /* Toggle EVK PWR EN board and Lpn pins */
  HAL_GPIO_WritePin(TOF_LPn_GPIO_Port, TOF_LPn_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TOF_LPn_GPIO_Port, TOF_LPn_Pin, GPIO_PIN_SET);

  status |= WrByte(p_platform_drv, 0x7fff, 0x00);
  status |= RdByte(p_platform_drv, 0, &device_id);
  status |= RdByte(p_platform_drv, 1, &revision_id);
  status |= WrByte(p_platform_drv, 0x7fff, 0x02);

  if (status == 0U)
  {
    id = ((uint32_t) device_id << 8) + revision_id;
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, id);
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("VL53L8CX: sensor - I am 0x%x.\r\n", id));

    /* Initialize as default */
    status = vl53l8cx_init(&_this->tof_driver_if);

    if (status == 0U && _this->sensor_status.is_active)
    {
      /* Setup Power mode*/
      if (vl53l8cx_set_power_mode(&_this->tof_driver_if,
                                  (uint8_t) _this->sensor_status.type.ranging.power_mode) != VL53L8CX_STATUS_OK)
      {
        res = VL53L8CX_ERROR;
      }
      else
      {
        res = VL53L8CX_OK;
      }

      /* Setup Config Profile */
      uint8_t resolution;
      uint8_t ranging_mode;
      uint8_t profile = _this->sensor_status.type.ranging.profile_config.ranging_profile;
      uint8_t ranging_frequency = (uint8_t) _this->sensor_status.type.ranging.profile_config.frequency;
      uint32_t integration_time = _this->sensor_status.type.ranging.profile_config.timing_budget;

      switch (profile)
      {
        case VL53L8CX_PROFILE_4x4_CONTINUOUS:
          resolution = VL53L8CX_RESOLUTION_4X4;
          ranging_mode = VL53L8CX_RANGING_MODE_CONTINUOUS;
          break;
        case VL53L8CX_PROFILE_4x4_AUTONOMOUS:
          resolution = VL53L8CX_RESOLUTION_4X4;
          ranging_mode = VL53L8CX_RANGING_MODE_AUTONOMOUS;
          break;
        case VL53L8CX_PROFILE_8x8_CONTINUOUS:
          resolution = VL53L8CX_RESOLUTION_8X8;
          ranging_mode = VL53L8CX_RANGING_MODE_CONTINUOUS;
          break;
        case VL53L8CX_PROFILE_8x8_AUTONOMOUS:
          resolution = VL53L8CX_RESOLUTION_8X8;
          ranging_mode = VL53L8CX_RANGING_MODE_AUTONOMOUS;
          break;
        default:
          resolution = 0; /* silence MISRA rule 1.3 warning */
          ranging_mode = 0; /* silence MISRA rule 1.3 warning */
          res = VL53L8CX_INVALID_PARAM;
          break;
      }

      if (res != VL53L8CX_OK)
      {
        return res;
      }
      else if (vl53l8cx_set_resolution(&_this->tof_driver_if, resolution) != VL53L8CX_STATUS_OK)
      {
        res = VL53L8CX_ERROR;
      }
      else if (vl53l8cx_set_ranging_mode(&_this->tof_driver_if, ranging_mode) != VL53L8CX_STATUS_OK)
      {
        res = VL53L8CX_ERROR;
      }
      else if (vl53l8cx_set_integration_time_ms(&_this->tof_driver_if, integration_time) != VL53L8CX_STATUS_OK)
      {
        res = VL53L8CX_ERROR;
      }
      else if (vl53l8cx_set_ranging_frequency_hz(&_this->tof_driver_if, ranging_frequency) != VL53L8CX_STATUS_OK)
      {
        res = VL53L8CX_ERROR;
      }
      else
      {
        res = VL53L8CX_OK;
      }

      /* Setup Config IT*/
      uint8_t i;
      uint8_t status = 0U;
      static VL53L8CX_DetectionThresholds thresholds[VL53L8CX_NB_THRESHOLDS];

      if (_this->sensor_status.type.ranging.it_config.criteria == VL53L8CX_IT_DEFAULT)
      {
        (void) vl53l8cx_get_resolution(&_this->tof_driver_if, &resolution);

        /* configure thresholds on each active zone */
        for (i = 0; i < resolution; i++)
        {
          thresholds[i].zone_num = i;
          thresholds[i].measurement = VL53L8CX_DISTANCE_MM;
          thresholds[i].type = (uint8_t) _this->sensor_status.type.ranging.it_config.criteria;
          thresholds[i].mathematic_operation = VL53L8CX_OPERATION_NONE;
          thresholds[i].param_low_thresh = (int32_t) _this->sensor_status.type.ranging.it_config.low_threshold;
          thresholds[i].param_high_thresh = (int32_t) _this->sensor_status.type.ranging.it_config.high_threshold;
        }

        /* the last threshold must be clearly indicated */
        thresholds[i].zone_num |= VL53L8CX_LAST_THRESHOLD;

        /* send array of thresholds to the sensor */
        status |= vl53l8cx_set_detection_thresholds(&_this->tof_driver_if, thresholds);

        /* enable thresholds detection */
        status |= vl53l8cx_set_detection_thresholds_enable(&_this->tof_driver_if, 1U);

        res = (status != 0U) ? VL53L8CX_ERROR : VL53L8CX_OK;
      }

      /* Start the sensor */
      if (vl53l8cx_start_ranging(&_this->tof_driver_if) == VL53L8CX_STATUS_OK)
      {
        switch (_this->sensor_status.type.ranging.profile_config.mode)
        {
          case VL53L8CX_MODE_BLOCKING_CONTINUOUS:
            _this->IsContinuous = 1U;
            _this->IsBlocking = 1U;
            break;

          case VL53L8CX_MODE_BLOCKING_ONESHOT:
            _this->IsContinuous = 0U;
            _this->IsBlocking = 1U;
            break;

          case VL53L8CX_MODE_ASYNC_CONTINUOUS:
            _this->IsContinuous = 1U;
            _this->IsBlocking = 0U;
            break;

          case VL53L8CX_MODE_ASYNC_ONESHOT:
            _this->IsContinuous = 0U;
            _this->IsBlocking = 0U;
            break;

          default:
            _this->sensor_status.is_active = false;
            res = VL53L8CX_INVALID_PARAM;
            break;
        }
      }
      else
      {
        res = VL53L8CX_ERROR;
      }
    }
    else
    {
      vl53l8cx_stop_ranging(&_this->tof_driver_if);
      _this->sensor_status.is_active = false;
    }

    _this->vl53l8cx_task_cfg_timer_period_ms = (uint16_t)(1000.0f / _this->sensor_status.type.ranging.profile_config.frequency);

  }

  return res;
}

static sys_error_code_t VL53L8CXTaskSensorReadData(VL53L8CXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  VL53L8CX_Platform *p_platform_drv = (VL53L8CX_Platform *) &_this->p_sensor_bus_if->m_xConnector;
  _this->tof_driver_if.platform = *p_platform_drv;

  if (_this->sensor_status.is_active)
  {
    uint32_t timeout;
    uint32_t tick_start;
    uint8_t new_data_ready = 0;

    if (_this->IsBlocking == 1U)
    {
      timeout = _this->vl53l8cx_task_cfg_timer_period_ms;
    }
    else
    {
      timeout = 0;
    }

    tick_start = HAL_GetTick();

    do
    {
      (void) vl53l8cx_check_data_ready(&_this->tof_driver_if, &new_data_ready);

      if (new_data_ready == 1U)
      {
        res = VL53L8CX_OK;
        break;
      }
    } while ((HAL_GetTick() - tick_start) < timeout);
  }

  /* a new measure is available if no error is returned by the poll function */
  if (res == VL53L8CX_OK)
  {
    uint8_t i;
    uint8_t resolution;
    VL53L8CX_ResultsData data;

    if (vl53l8cx_get_resolution(&_this->tof_driver_if, &resolution) != VL53L8CX_STATUS_OK)
    {
      res = VL53L8CX_ERROR;
    }
    else if (vl53l8cx_get_ranging_data(&_this->tof_driver_if, &data) != VL53L8CX_STATUS_OK)
    {
      res = VL53L8CX_ERROR;
    }
    else
    {
      for (i = 0; i < resolution; i++)
      {
        _this->p_sensor_data_buff[i][0] = data.nb_target_detected[i];

        /* return Ambient value if ambient rate output is enabled */
        if (_this->sensor_status.type.ranging.profile_config.enable_ambient == true)
        {
          /* apply ambient value to all targets in a given zone */
          _this->p_sensor_data_buff[i][1] = data.ambient_per_spad[i];
        }
        else
        {
          _this->p_sensor_data_buff[i][1] = 0;
        }

        /*** TARGET 1 OUTPUT VALUES ***/
        /* return Signal value if signal rate output is enabled */
        if (_this->sensor_status.type.ranging.profile_config.enable_signal == true)
        {
          _this->p_sensor_data_buff[i][2] = data.signal_per_spad[2*i];
        }
        else
        {
          _this->p_sensor_data_buff[i][2] = 0;
        }

        _this->p_sensor_data_buff[i][3] = data.target_status[2*i];

        _this->p_sensor_data_buff[i][4] = data.distance_mm[2*i];

        /*** TARGET 2 OUTPUT VALUES ***/
        /* return Signal value if signal rate output is enabled */
        if (_this->sensor_status.type.ranging.profile_config.enable_signal == true)
        {
          _this->p_sensor_data_buff[i][5] = data.signal_per_spad[2*i + 1];
        }
        else
        {
          _this->p_sensor_data_buff[i][5] = 0;
        }

        _this->p_sensor_data_buff[i][6] = data.target_status[2*i + 1];

        _this->p_sensor_data_buff[i][7] = data.distance_mm[2*i +1];
      }
      res = VL53L8CX_OK;
    }
  }

//
//#if (HSD_USE_DUMMY_DATA == 1)
//  uint16_t i = 0;
//  int16_t *p16 = (int16_t *)_this->p_sensor_data_buff;
//
//  if(_this->fifo_level >= _this->samples_per_it)
//  {
//  for (i = 0; i < _this->samples_per_it * 3 ; i++)
//  {
//    *p16++ = dummyDataCounter++;
//  }
//  }
//#endif

  return res;
}

static sys_error_code_t VL53L8CXTaskSensorRegister(VL53L8CXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *tof_if = (ISensor_t *) VL53L8CXTaskGetTofSensorIF(_this);
  _this->id = SMAddSensor(tof_if);

  return res;
}

static sys_error_code_t VL53L8CXTaskSensorInitTaskParams(VL53L8CXTask *_this)
{

  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* TIME-OF-FLIGHT SENSOR STATUS */
  _this->sensor_status.isensor_class = ISENSOR_CLASS_RANGING;
  _this->sensor_status.is_active = TRUE;
  _this->sensor_status.type.ranging.profile_config.ranging_profile = VL53L8CX_PROFILE_4x4_CONTINUOUS;
  _this->sensor_status.type.ranging.profile_config.timing_budget = 10;
  _this->sensor_status.type.ranging.profile_config.frequency = 10;
  _this->sensor_status.type.ranging.profile_config.enable_ambient = true;
  _this->sensor_status.type.ranging.profile_config.enable_signal = true;
  _this->sensor_status.type.ranging.profile_config.mode = VL53L8CX_MODE_BLOCKING_CONTINUOUS;
  _this->sensor_status.type.ranging.it_config.criteria = VL53L8CX_IT_DEFAULT;
  _this->sensor_status.type.ranging.address = VL53L8CX_DEVICE_ADDRESS;
  _this->sensor_status.type.ranging.power_mode = VL53L8CX_POWER_MODE_WAKEUP;
  EMD_Init(&_this->data, (uint8_t *) &_this->p_sensor_data_buff[0], E_EM_UINT32, E_EM_MODE_INTERLEAVED, 3, 1, VL53L8CX_RESOLUTION_4X4, 8);

  return res;
}

static sys_error_code_t VL53L8CXTaskSensorSetFrequency(VL53L8CXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint32_t frequency = (uint32_t) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    _this->sensor_status.type.ranging.profile_config.frequency = frequency;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t VL53L8CXTaskSensorSetResolution(VL53L8CXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint32_t resolution = (uint32_t) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (resolution == VL53L8CX_RESOLUTION_4X4)
    {
      if (_this->sensor_status.type.ranging.profile_config.ranging_profile == VL53L8CX_PROFILE_4x4_CONTINUOUS || _this->sensor_status.type.ranging.profile_config.ranging_profile == VL53L8CX_PROFILE_8x8_CONTINUOUS)
      {
        _this->sensor_status.type.ranging.profile_config.ranging_profile = VL53L8CX_PROFILE_4x4_CONTINUOUS;
      }
      else
      {
        _this->sensor_status.type.ranging.profile_config.ranging_profile = VL53L8CX_PROFILE_4x4_AUTONOMOUS;
      }
    }
    else if (resolution == VL53L8CX_RESOLUTION_8X8)
    {
      if (_this->sensor_status.type.ranging.profile_config.ranging_profile == VL53L8CX_PROFILE_4x4_CONTINUOUS || _this->sensor_status.type.ranging.profile_config.ranging_profile == VL53L8CX_PROFILE_8x8_CONTINUOUS)
      {
        _this->sensor_status.type.ranging.profile_config.ranging_profile = VL53L8CX_PROFILE_8x8_CONTINUOUS;
      }
      else
      {
        _this->sensor_status.type.ranging.profile_config.ranging_profile = VL53L8CX_PROFILE_8x8_AUTONOMOUS;
      }
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }
    EMD_Init(&_this->data, (uint8_t *) &_this->p_sensor_data_buff[0], E_EM_UINT32, E_EM_MODE_INTERLEAVED, 3, 1, resolution, 8);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t VL53L8CXTaskSensorSetRangingMode(VL53L8CXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint32_t mode = (uint32_t) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    _this->sensor_status.type.ranging.profile_config.mode = mode;

    if (mode == VL53L8CX_MODE_BLOCKING_ONESHOT || mode == VL53L8CX_MODE_ASYNC_ONESHOT)
    {
      if (_this->sensor_status.type.ranging.profile_config.ranging_profile == VL53L8CX_PROFILE_4x4_CONTINUOUS)
      {
        _this->sensor_status.type.ranging.profile_config.ranging_profile = VL53L8CX_PROFILE_4x4_AUTONOMOUS;
      }
      else if (_this->sensor_status.type.ranging.profile_config.ranging_profile == VL53L8CX_PROFILE_8x8_CONTINUOUS)
      {
        _this->sensor_status.type.ranging.profile_config.ranging_profile = VL53L8CX_PROFILE_8x8_AUTONOMOUS;
      }
    }
    else if (mode == VL53L8CX_MODE_BLOCKING_CONTINUOUS || mode == VL53L8CX_MODE_ASYNC_CONTINUOUS)
    {
      if (_this->sensor_status.type.ranging.profile_config.ranging_profile == VL53L8CX_PROFILE_4x4_AUTONOMOUS)
      {
        _this->sensor_status.type.ranging.profile_config.ranging_profile = VL53L8CX_PROFILE_4x4_CONTINUOUS;
      }
      else if (_this->sensor_status.type.ranging.profile_config.ranging_profile == VL53L8CX_PROFILE_8x8_AUTONOMOUS)
      {
        _this->sensor_status.type.ranging.profile_config.ranging_profile = VL53L8CX_PROFILE_8x8_CONTINUOUS;
      }
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t VL53L8CXTaskSensorSetIntegrationTime(VL53L8CXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint32_t timing_budget = (uint32_t) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    _this->sensor_status.type.ranging.profile_config.timing_budget = timing_budget;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t VL53L8CXTaskSensorConfigIt(VL53L8CXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ITConfig_t *pConfig = (ITConfig_t *)*(uint32_t*)&report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    _this->sensor_status.type.ranging.it_config.criteria = pConfig->criteria;
    if (_this->sensor_status.type.ranging.it_config.criteria != VL53L8CX_IT_DEFAULT)
    {
      _this->sensor_status.type.ranging.it_config.high_threshold = pConfig->high_threshold;
      _this->sensor_status.type.ranging.it_config.low_threshold = pConfig->low_threshold;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t VL53L8CXTaskSensorSetAddress(VL53L8CXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  VL53L8CX_Platform *p_platform_drv = (VL53L8CX_Platform *) &_this->p_sensor_bus_if->m_xConnector;
  _this->tof_driver_if.platform = *p_platform_drv;

  uint32_t address = report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (vl53l8cx_set_i2c_address(&_this->tof_driver_if,
                                 (uint8_t) _this->sensor_status.type.ranging.address) != VL53L8CX_STATUS_OK)
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }
    else
    {
      _this->sensor_status.type.ranging.address = (uint8_t)(address & 0xFFU);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t VL53L8CXTaskSensorSetPowerMode(VL53L8CXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint32_t power_mode = report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    _this->sensor_status.type.ranging.power_mode = power_mode;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t VL53L8CXTaskSensorEnable(VL53L8CXTask *_this, SMMessage report)
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

static sys_error_code_t VL53L8CXTaskSensorDisable(VL53L8CXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
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

static boolean_t VL53L8CXTaskSensorIsActive(const VL53L8CXTask *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.is_active;
}

static sys_error_code_t VL53L8CXTaskEnterLowPowerMode(const VL53L8CXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
//  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t*) &_this->p_sensor_bus_if->m_xConnector;

  return res;
}

static sys_error_code_t VL53L8CXTaskConfigureIrqPin(const VL53L8CXTask *_this, boolean_t LowPower)
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

static void VL53L8CXTaskTimerCallbackFunction(ULONG param)
{
  VL53L8CXTask *p_obj = (VL53L8CXTask *) param;
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

void VL53L8CXTask_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((VL53L8CXTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}
