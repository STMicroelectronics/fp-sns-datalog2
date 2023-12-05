/**
  ******************************************************************************
  * @file    LIS2DU12Task.c
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
#include "LIS2DU12Task.h"
#include "LIS2DU12Task_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "lis2du12_reg.h"
#include <string.h>
#include "services/sysdebug.h"

/* Private includes ----------------------------------------------------------*/

#ifndef LIS2DU12_TASK_CFG_STACK_DEPTH
#define LIS2DU12_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*8)
#endif

#ifndef LIS2DU12_TASK_CFG_PRIORITY
#define LIS2DU12_TASK_CFG_PRIORITY                 (4)
#endif

#ifndef LIS2DU12_TASK_CFG_IN_QUEUE_LENGTH
#define LIS2DU12_TASK_CFG_IN_QUEUE_LENGTH          50u
#endif

#define LIS2DU12_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef LIS2DU12_TASK_CFG_TIMER_PERIOD_MS
#define LIS2DU12_TASK_CFG_TIMER_PERIOD_MS          500
#endif

#ifndef LIS2DU12_TASK_CFG_MAX_INSTANCES_COUNT
#define LIS2DU12_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)                 SYS_DEBUGF3(SYS_DBG_LIS2DU12, level, message)

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter = 0;
#endif


/**
  * Class object declaration
  */
typedef struct _LIS2DU12TaskClass
{
  /**
    * LIS2DU12Task class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * Accelerometer IF virtual table.
    */
  const ISensorMems_vtbl sensor_if_vtbl;

  /**
    * Specifies accelerometer sensor capabilities.
    */
  const SensorDescriptor_t class_descriptor;

  /**
    * LIS2DU12Task (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[LIS2DU12_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} LIS2DU12TaskClass_t;


/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LIS2DU12TaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LIS2DU12TaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LIS2DU12TaskSensorInit(LIS2DU12Task *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LIS2DU12TaskSensorReadData(LIS2DU12Task *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t LIS2DU12TaskSensorRegister(LIS2DU12Task *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t LIS2DU12TaskSensorInitTaskParams(LIS2DU12Task *_this);

/**
  * Private implementation of sensor interface methods for LIS2DU12 sensor
  */
static sys_error_code_t LIS2DU12TaskSensorSetODR(LIS2DU12Task *_this, SMMessage report);
static sys_error_code_t LIS2DU12TaskSensorSetFS(LIS2DU12Task *_this, SMMessage report);
static sys_error_code_t LIS2DU12TaskSensorSetFifoWM(LIS2DU12Task *_this, SMMessage report);
static sys_error_code_t LIS2DU12TaskSensorEnable(LIS2DU12Task *_this, SMMessage report);
static sys_error_code_t LIS2DU12TaskSensorDisable(LIS2DU12Task *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t LIS2DU12TaskSensorIsActive(const LIS2DU12Task *_this);

static sys_error_code_t LIS2DU12TaskEnterLowPowerMode(const LIS2DU12Task *_this);

static sys_error_code_t LIS2DU12TaskConfigureIrqPin(const LIS2DU12Task *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void LIS2DU12TaskTimerCallbackFunction(ULONG param);


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
static inline sys_error_code_t LIS2DU12TaskPostReportToFront(LIS2DU12Task *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t LIS2DU12TaskPostReportToBack(LIS2DU12Task *_this, SMMessage *pReport);


/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static LIS2DU12Task sTaskObj;

/**
  * The class object.
  */
static LIS2DU12TaskClass_t sTheClass =
{
  /* class virtual table */
  {
    LIS2DU12Task_vtblHardwareInit,
    LIS2DU12Task_vtblOnCreateTask,
    LIS2DU12Task_vtblDoEnterPowerMode,
    LIS2DU12Task_vtblHandleError,
    LIS2DU12Task_vtblOnEnterTaskControlLoop,
    LIS2DU12Task_vtblForceExecuteStep,
    LIS2DU12Task_vtblOnEnterPowerMode
  },
    /* class::sensor_if_vtbl virtual table */
    {
        {
            {
                LIS2DU12Task_vtblAccGetId,
                LIS2DU12Task_vtblGetEventSourceIF,
                LIS2DU12Task_vtblAccGetDataInfo
            },
            LIS2DU12Task_vtblSensorEnable,
            LIS2DU12Task_vtblSensorDisable,
            LIS2DU12Task_vtblSensorIsEnabled,
            LIS2DU12Task_vtblSensorGetDescription,
            LIS2DU12Task_vtblSensorGetStatus
        },
        LIS2DU12Task_vtblAccGetODR,
        LIS2DU12Task_vtblAccGetFS,
        LIS2DU12Task_vtblAccGetSensitivity,
        LIS2DU12Task_vtblSensorSetODR,
        LIS2DU12Task_vtblSensorSetFS,
        LIS2DU12Task_vtblSensorSetFifoWM
    },
  /* ACCELEROMETER DESCRIPTOR */
  {
    "lis2du12",
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
    LIS2DU12TaskExecuteStepState1,
    NULL,
    LIS2DU12TaskExecuteStepDatalog,
  },

  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *LIS2DU12TaskGetAccSensorIF(LIS2DU12Task *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *LIS2DU12TaskAlloc(const void *pIRQConfig, const void *pCSConfig)
{
  LIS2DU12Task *p_new_obj = SysAlloc(sizeof(LIS2DU12Task));

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

AManagedTaskEx *LIS2DU12TaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *p_name)
{
  LIS2DU12Task *p_new_obj = (LIS2DU12Task *)LIS2DU12TaskAlloc(pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *LIS2DU12TaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig)
{
  LIS2DU12Task *p_obj = (LIS2DU12Task *)p_mem_block;

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

AManagedTaskEx *LIS2DU12TaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                               const char *p_name)
{
  LIS2DU12Task *p_obj = (LIS2DU12Task *)LIS2DU12TaskStaticAlloc(p_mem_block, pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *LIS2DU12TaskGetSensorIF(LIS2DU12Task *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *LIS2DU12TaskGetEventSrcIF(LIS2DU12Task *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t LIS2DU12Task_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2DU12Task *p_obj = (LIS2DU12Task *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t LIS2DU12Task_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                               VOID **pvStackStart,
                                               ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                               ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2DU12Task *p_obj = (LIS2DU12Task *) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) LIS2DU12_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(LIS2DU12_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "LIS2DU12_Q", item_size / 4u, p_queue_items_buff,
                                    LIS2DU12_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_timer, "LIS2DU12_T", LIS2DU12TaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(LIS2DU12_TASK_CFG_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(LIS2DU12_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(LIS2DU12_ID, LIS2DU12_I2C_ADD_H, 0);
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
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, LIS2DU12_TASK_CFG_MAX_INSTANCES_COUNT);
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
  *pName = "LIS2DU12";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = LIS2DU12_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = LIS2DU12_TASK_CFG_PRIORITY;
  *pPreemptThreshold = LIS2DU12_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = LIS2DU12TaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = LIS2DU12TaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LIS2DU12: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t LIS2DU12Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2DU12Task *p_obj = (LIS2DU12Task *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (LIS2DU12TaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LIS2DU12: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      /* Deactivate the sensor */
      lis2du12_md_t mode = { LIS2DU12_OFF, LIS2DU12_2g, LIS2DU12_ODR_div_2 };
      mode.odr = LIS2DU12_OFF;
      lis2du12_mode_set(p_sensor_drv, &mode);

      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_timer);
      }
      else
      {
        LIS2DU12TaskConfigureIrqPin(p_obj, TRUE);
      }
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LIS2DU12: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = LIS2DU12TaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LIS2DU12 - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->pIRQConfig != NULL)
    {
      LIS2DU12TaskConfigureIrqPin(p_obj, TRUE);
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LIS2DU12: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t LIS2DU12Task_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  LIS2DU12Task *p_obj = (LIS2DU12Task*)_this;

  return res;
}

sys_error_code_t LIS2DU12Task_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
//  LIS2DU12Task *p_obj = (LIS2DU12Task*) _this;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LIS2DU12: start.\r\n"));

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t LIS2DU12Task_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2DU12Task *p_obj = (LIS2DU12Task *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = LIS2DU12TaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t LIS2DU12Task_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  LIS2DU12Task *p_obj = (LIS2DU12Task*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t LIS2DU12Task_vtblAccGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LIS2DU12Task *p_if_owner = (LIS2DU12Task *)((uint32_t) _this - offsetof(LIS2DU12Task, sensor_if));
  uint8_t res = p_if_owner->acc_id;

  return res;
}

IEventSrc *LIS2DU12Task_vtblGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LIS2DU12Task *p_if_owner = (LIS2DU12Task *)((uint32_t) _this - offsetof(LIS2DU12Task, sensor_if));
  return p_if_owner->p_event_src;
}

sys_error_code_t LIS2DU12Task_vtblAccGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LIS2DU12Task *p_if_owner = (LIS2DU12Task *)((uint32_t) _this - offsetof(LIS2DU12Task, sensor_if));
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

float LIS2DU12Task_vtblAccGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LIS2DU12Task *p_if_owner = (LIS2DU12Task *)((uint32_t) _this - offsetof(LIS2DU12Task, sensor_if));
  float res = p_if_owner->sensor_status.type.mems.fs;

  return res;
}

float LIS2DU12Task_vtblAccGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LIS2DU12Task *p_if_owner = (LIS2DU12Task *)((uint32_t) _this - offsetof(LIS2DU12Task, sensor_if));
  float res = p_if_owner->sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t LIS2DU12Task_vtblAccGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LIS2DU12Task *p_if_owner = (LIS2DU12Task *)((uint32_t) _this - offsetof(LIS2DU12Task, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t LIS2DU12Task_vtblSensorSetODR(ISensorMems_t *_this, float odr)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2DU12Task *p_if_owner = (LIS2DU12Task *)((uint32_t) _this - offsetof(LIS2DU12Task, sensor_if));
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
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_ODR,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.fParam = (float) odr
    };
    res = LIS2DU12TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t LIS2DU12Task_vtblSensorSetFS(ISensorMems_t *_this, float fs)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2DU12Task *p_if_owner = (LIS2DU12Task *)((uint32_t) _this - offsetof(LIS2DU12Task, sensor_if));
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
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FS,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.fParam = (float) fs
    };
    res = LIS2DU12TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t LIS2DU12Task_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

#if LIS2DU12_FIFO_ENABLED
  LIS2DU12Task *p_if_owner = (LIS2DU12Task *)((uint32_t) _this - offsetof(LIS2DU12Task, sensor_if));
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
    res = LIS2DU12TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }
#endif

  return res;
}

sys_error_code_t LIS2DU12Task_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2DU12Task *p_if_owner = (LIS2DU12Task *)((uint32_t) _this - offsetof(LIS2DU12Task, sensor_if));
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
      .sensorMessage.nCmdID = SENSOR_CMD_ID_ENABLE,
      .sensorMessage.nSensorId = sensor_id
    };
    res = LIS2DU12TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t LIS2DU12Task_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2DU12Task *p_if_owner = (LIS2DU12Task *)((uint32_t) _this - offsetof(LIS2DU12Task, sensor_if));
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
      .sensorMessage.nCmdID = SENSOR_CMD_ID_DISABLE,
      .sensorMessage.nSensorId = sensor_id
    };
    res = LIS2DU12TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t LIS2DU12Task_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  LIS2DU12Task *p_if_owner = (LIS2DU12Task *)((uint32_t) _this - offsetof(LIS2DU12Task, sensor_if));

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->acc_id)
  {
    res = p_if_owner->sensor_status.is_active;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

SensorDescriptor_t LIS2DU12Task_vtblSensorGetDescription(ISensor_t *_this)
{

  assert_param(_this != NULL);
  LIS2DU12Task *p_if_owner = (LIS2DU12Task *)((uint32_t) _this - offsetof(LIS2DU12Task, sensor_if));
  return *p_if_owner->sensor_descriptor;
}

SensorStatus_t LIS2DU12Task_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LIS2DU12Task *p_if_owner = (LIS2DU12Task *)((uint32_t) _this - offsetof(LIS2DU12Task, sensor_if));

  return p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t LIS2DU12TaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2DU12Task *p_obj = (LIS2DU12Task *) _this;
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
            res = LIS2DU12TaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = LIS2DU12TaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = LIS2DU12TaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = LIS2DU12TaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = LIS2DU12TaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LIS2DU12: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LIS2DU12: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }

    }
  }

  return res;
}

static sys_error_code_t LIS2DU12TaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LIS2DU12Task *p_obj = (LIS2DU12Task *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("LIS2DU12: new data.\r\n"));
//          if(p_obj->pIRQConfig == NULL)
//          {
//            if(TX_SUCCESS
//                != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->lis2du12_task_cfg_timer_period_ms),
//                                   AMT_MS_TO_TICKS(p_obj->lis2du12_task_cfg_timer_period_ms)))
//            {
//              return SYS_UNDEFINED_ERROR_CODE;
//            }
//          }

        res = LIS2DU12TaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
#if LIS2DU12_FIFO_ENABLED
          if (p_obj->fifo_level != 0)
          {
#endif
            // notify the listeners...
            double timestamp = report.sensorDataReadyMessage.fTimestamp;
            double delta_timestamp = timestamp - p_obj->prev_timestamp;
            p_obj->prev_timestamp = timestamp;

            /* update measuredODR */
            p_obj->sensor_status.type.mems.measured_odr = (float) p_obj->samples_per_it / (float) delta_timestamp;

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
            SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("LIS2DU12: ts = %f\r\n", (float)timestamp));
#if LIS2DU12_FIFO_ENABLED
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
            res = LIS2DU12TaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->sensor_status.is_active == true)
              {
                if (p_obj->pIRQConfig == NULL)
                {
                  if (TX_SUCCESS
                      != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->lis2du12_task_cfg_timer_period_ms),
                                         AMT_MS_TO_TICKS(p_obj->lis2du12_task_cfg_timer_period_ms)))
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
                  LIS2DU12TaskConfigureIrqPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = LIS2DU12TaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = LIS2DU12TaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = LIS2DU12TaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = LIS2DU12TaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = LIS2DU12TaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LIS2DU12: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LIS2DU12: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t LIS2DU12TaskPostReportToFront(LIS2DU12Task *_this, SMMessage *pReport)
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

static inline sys_error_code_t LIS2DU12TaskPostReportToBack(LIS2DU12Task *_this, SMMessage *pReport)
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

static sys_error_code_t LIS2DU12TaskSensorInit(LIS2DU12Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  int32_t ret_val = 0;
//  uint8_t reg0;

  /* FIFO INT setup */
  lis2du12_pin_int2_route_t int2_route =
  {
    0
  };

  /* Select bus interface */
  lis2du12_bus_mode_set(p_sensor_drv, LIS2DU12_I3C_DISABLE);

  /* Set bdu and if_inc recommended for driver usage */
  lis2du12_init_set(p_sensor_drv, LIS2DU12_DRV_RDY);

  /* Check device ID */
  lis2du12_id_t regId;
  ret_val = lis2du12_id_get(p_sensor_drv, &regId);
  if (ret_val == 0)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, regId.whoami);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LIS2DU12: sensor - I am 0x%x.\r\n", regId));

#if LIS2DU12_FIFO_ENABLED

  if (_this->samples_per_it == 0)
  {
    uint16_t lis2du12_wtm_level = 0;

    /* lis2du12_wtm_level of watermark and samples per int*/
    lis2du12_wtm_level = ((uint16_t) _this->sensor_status.type.mems.odr * (uint16_t) LIS2DU12_MAX_DRDY_PERIOD);
    if (lis2du12_wtm_level > LIS2DU12_MAX_WTM_LEVEL)
    {
      lis2du12_wtm_level = LIS2DU12_MAX_WTM_LEVEL;
    }
    else if (lis2du12_wtm_level < LIS2DU12_MIN_WTM_LEVEL)
    {
      lis2du12_wtm_level = LIS2DU12_MIN_WTM_LEVEL;
    }

    _this->samples_per_it = lis2du12_wtm_level;
  }

  /* Calculation of watermark and samples per int*/
  lis2du12_fifo_md_t fifo_md = { LIS2DU12_BYPASS, LIS2DU12_8_BIT };
  fifo_md.operation = LIS2DU12_STREAM;
  fifo_md.store = LIS2DU12_8_BIT;
  fifo_md.watermark = _this->samples_per_it;
  lis2du12_fifo_mode_set(p_sensor_drv, &fifo_md);

  /* In order to read only accelerometer data from the FIFO, rounding_xyx bit must be set to 1 */
  lis2du12_fifo_ctrl_t fifo_ctrl;
  lis2du12_read_reg(p_sensor_drv, LIS2DU12_FIFO_CTRL, (uint8_t *)&fifo_ctrl, 1);
  fifo_ctrl.rounding_xyz = 1;
  lis2du12_write_reg(p_sensor_drv, LIS2DU12_FIFO_CTRL, (uint8_t *)&fifo_ctrl, 1);

  if (_this->pIRQConfig != NULL)
  {
    int2_route.fifo_th = PROPERTY_ENABLE;
  }
  else
  {
    int2_route.fifo_th = PROPERTY_DISABLE;
  }
#else
  _this->samples_per_it = 1;
  if (_this->pIRQConfig != NULL)
  {
    int2_route.drdy_xl = PROPERTY_ENABLE;
  }
  else
  {
    int2_route.drdy_xl = PROPERTY_DISABLE;
  }
#endif /* LIS2DU12_FIFO_ENABLED */
  lis2du12_pin_int2_route_set(p_sensor_drv, &int2_route);

  /* Output data rate selection - power down. */
  lis2du12_md_t mode = { LIS2DU12_OFF, LIS2DU12_2g, LIS2DU12_ODR_div_2 };
  mode.odr = LIS2DU12_OFF;
  lis2du12_mode_set(p_sensor_drv, &mode);

  /* Full scale selection. */
  if (_this->sensor_status.type.mems.fs < 3.0f)
  {
    mode.fs = LIS2DU12_2g;
  }
  else if (_this->sensor_status.type.mems.fs < 5.0f)
  {
    mode.fs = LIS2DU12_4g;
  }
  else if (_this->sensor_status.type.mems.fs < 9.0f)
  {
    mode.fs = LIS2DU12_8g;
  }
  else
  {
    mode.fs = LIS2DU12_16g;
  }

  if (_this->sensor_status.type.mems.odr < 2.0f)
  {
    mode.odr = LIS2DU12_1Hz5_ULP;
  }
  else if (_this->sensor_status.type.mems.odr < 4.0f)
  {
    mode.odr = LIS2DU12_3Hz_ULP;
  }
  else if (_this->sensor_status.type.mems.odr < 6.0f)
  {
    mode.odr = LIS2DU12_6Hz_ULP;
  }
  else if (_this->sensor_status.type.mems.odr < 7.0f)
  {
    mode.odr = LIS2DU12_6Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 13.0f)
  {
    mode.odr = LIS2DU12_12Hz5;
  }
  else if (_this->sensor_status.type.mems.odr < 26.0f)
  {
    mode.odr = LIS2DU12_25Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 51.0f)
  {
    mode.odr = LIS2DU12_50Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 101.0f)
  {
    mode.odr = LIS2DU12_100Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 201.0f)
  {
    mode.odr = LIS2DU12_200Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 401.0f)
  {
    mode.odr = LIS2DU12_400Hz;
  }
  else
  {
    mode.odr = LIS2DU12_800Hz;
  }
  lis2du12_mode_set(p_sensor_drv, &mode);

#if LIS2DU12_FIFO_ENABLED
  lis2du12_fifo_level_get(p_sensor_drv, &fifo_md, (uint8_t *) &_this->fifo_level);
  if (_this->fifo_level >= _this->samples_per_it)
  {
    lis2du12_read_reg(p_sensor_drv, LIS2DU12_OUTX_L, (uint8_t *) _this->p_sensor_data_buff,
                      ((uint16_t)_this->samples_per_it * 6u));
  }

  _this->lis2du12_task_cfg_timer_period_ms = (uint16_t)((1000.0f / _this->sensor_status.type.mems.odr) * (((float)(_this->samples_per_it)) / 2.0f));
#else
  _this->lis2du12_task_cfg_timer_period_ms = (uint16_t)(1000.0f / _this->sensor_status.type.mems.odr);
#endif

  return res;
}

static sys_error_code_t LIS2DU12TaskSensorReadData(LIS2DU12Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t samples_per_it = _this->samples_per_it;

#if LIS2DU12_FIFO_ENABLED
  lis2du12_fifo_md_t fifo_md = { LIS2DU12_BYPASS, LIS2DU12_8_BIT };
  lis2du12_fifo_level_get(p_sensor_drv, &fifo_md, (uint8_t *) &_this->fifo_level);
  if (_this->fifo_level >= samples_per_it)
  {
    lis2du12_read_reg(p_sensor_drv, LIS2DU12_OUTX_L, (uint8_t *) _this->p_sensor_data_buff,
                      ((uint16_t)samples_per_it * 6u));
  }
  else
  {
    _this->fifo_level = 0;
  }
#else
  lis2du12_read_reg(p_sensor_drv, LIS2DU12_OUTX_L, (uint8_t *) _this->p_sensor_data_buff,
                    ((uint16_t) samples_per_it * 6u));
  _this->fifo_level = 1;
#endif /* LIS2DU12_FIFO_ENABLED */

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

static sys_error_code_t LIS2DU12TaskSensorRegister(LIS2DU12Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *acc_if = (ISensor_t *) LIS2DU12TaskGetAccSensorIF(_this);
  _this->acc_id = SMAddSensor(acc_if);

  return res;
}

static sys_error_code_t LIS2DU12TaskSensorInitTaskParams(LIS2DU12Task *_this)
{

  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* ACCELEROMETER SENSOR STATUS */
  _this->sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->sensor_status.is_active = TRUE;
  _this->sensor_status.type.mems.fs = 16.0f;
  _this->sensor_status.type.mems.sensitivity = 0.0000305f * _this->sensor_status.type.mems.fs;
  _this->sensor_status.type.mems.odr = 800.0f;
  _this->sensor_status.type.mems.measured_odr = 0.0f;
  EMD_Init(&_this->data, _this->p_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);

  return res;
}

static sys_error_code_t LIS2DU12TaskSensorSetODR(LIS2DU12Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float odr = (float) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    if (odr < 1.0f)
    {
      lis2du12_md_t mode = { LIS2DU12_OFF, LIS2DU12_2g, LIS2DU12_ODR_div_2 };
      mode.odr = LIS2DU12_OFF;
      lis2du12_mode_set(p_sensor_drv, &mode);
      /* Do not update the model in case of odr = 0 */
      odr = _this->sensor_status.type.mems.odr;
    }
    else if (odr < 2.0f)
    {
      odr = 1.5f;
    }
    else if (odr < 4.0f)
    {
      odr = 3.0f;
    }
    else if (odr < 6.0f)
    {
      odr = 6.0f;
    }
    else if (odr < 7.0f)
    {
      odr = 6.0f;
    }
    else if (odr < 13.0f)
    {
      odr = 12.5f;
    }
    else if (odr < 26.0f)
    {
      odr = 25.0f;
    }
    else if (odr < 51.0f)
    {
      odr = 50.0f;
    }
    else if (odr < 101.0f)
    {
      odr = 100.0f;
    }
    else if (odr < 201.0f)
    {
      odr = 200.0f;
    }
    else if (odr < 401.0f)
    {
      odr = 400.0f;
    }
    else
    {
      odr = 800.0f;
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

static sys_error_code_t LIS2DU12TaskSensorSetFS(LIS2DU12Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float fs = (float) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  lis2du12_md_t mode = { LIS2DU12_OFF, LIS2DU12_2g, LIS2DU12_ODR_div_2 };

  if (id == _this->acc_id)
  {
    /* Full scale selection. */
    if (fs < 3.0f)
    {
      mode.fs = LIS2DU12_2g;
      fs = 2.0f;
    }
    else if (fs < 5.0f)
    {
      mode.fs = LIS2DU12_4g;
      fs = 4.0f;
    }
    else if (fs < 9.0f)
    {
      mode.fs = LIS2DU12_8g;
      fs = 8.0f;
    }
    else
    {
      mode.fs = LIS2DU12_16g;
      fs = 16.0f;
    }
    lis2du12_mode_set(p_sensor_drv, &mode);

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->sensor_status.type.mems.fs = fs;
      _this->sensor_status.type.mems.sensitivity = 0.0000305f * _this->sensor_status.type.mems.fs;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t LIS2DU12TaskSensorSetFifoWM(LIS2DU12Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t lis2du12_wtm_level = (uint16_t)report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    if (lis2du12_wtm_level > LIS2DU12_MAX_WTM_LEVEL)
    {
      lis2du12_wtm_level = LIS2DU12_MAX_WTM_LEVEL;
    }

    _this->samples_per_it = lis2du12_wtm_level;

    lis2du12_fifo_md_t fifo_md = { LIS2DU12_BYPASS, LIS2DU12_8_BIT };
    fifo_md.operation = LIS2DU12_STREAM;
    fifo_md.store = LIS2DU12_8_BIT;
    fifo_md.watermark = lis2du12_wtm_level;
    lis2du12_fifo_mode_set(p_sensor_drv, &fifo_md);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t LIS2DU12TaskSensorEnable(LIS2DU12Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    _this->sensor_status.is_active = TRUE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t LIS2DU12TaskSensorDisable(LIS2DU12Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    _this->sensor_status.is_active = FALSE;
    lis2du12_md_t mode = { LIS2DU12_OFF, LIS2DU12_2g, LIS2DU12_ODR_div_2 };
    mode.odr = LIS2DU12_OFF;
    lis2du12_mode_set(p_sensor_drv, &mode);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t LIS2DU12TaskSensorIsActive(const LIS2DU12Task *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.is_active;
}

static sys_error_code_t LIS2DU12TaskEnterLowPowerMode(const LIS2DU12Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  lis2du12_md_t mode = { LIS2DU12_OFF, LIS2DU12_2g, LIS2DU12_ODR_div_2 };
  mode.odr = LIS2DU12_OFF;
  if (lis2du12_mode_set(p_sensor_drv, &mode))
  {
    res = SYS_SENSOR_TASK_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_OP_ERROR_CODE);
  }

  return res;
}

static sys_error_code_t LIS2DU12TaskConfigureIrqPin(const LIS2DU12Task *_this, boolean_t LowPower)
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

static void LIS2DU12TaskTimerCallbackFunction(ULONG param)
{
  LIS2DU12Task *p_obj = (LIS2DU12Task *) param;
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

void LIS2DU12Task_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((LIS2DU12Task *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}
