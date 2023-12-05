/**
  ******************************************************************************
  * @file    IIS3DWBTask.c
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
#include "IIS3DWBTask.h"
#include "IIS3DWBTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "iis3dwb_reg.h"
#include <string.h>
#include "services/sysdebug.h"

/* Private includes ----------------------------------------------------------*/

#ifndef IIS3DWB_TASK_CFG_STACK_DEPTH
#define IIS3DWB_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*8)
#endif

#ifndef IIS3DWB_TASK_CFG_PRIORITY
#define IIS3DWB_TASK_CFG_PRIORITY                 (4)
#endif

#ifndef IIS3DWB_TASK_CFG_IN_QUEUE_LENGTH
#define IIS3DWB_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define IIS3DWB_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef IIS3DWB_TASK_CFG_TIMER_PERIOD_MS
#define IIS3DWB_TASK_CFG_TIMER_PERIOD_MS          500
#endif

#ifndef IIS3DWB_TASK_CFG_MAX_INSTANCES_COUNT
#define IIS3DWB_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_IIS3DWB, level, message)

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter = 0;
#endif


/**
  * Class object declaration
  */
typedef struct _IIS3DWBTaskClass
{
  /**
    * IIS3DWBTask class virtual table.
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
    * IIS3DWBTask (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
   */
  MTMapElement_t task_map_elements[IIS3DWB_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} IIS3DWBTaskClass_t;


/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS3DWBTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS3DWBTaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS3DWBTaskSensorInit(IIS3DWBTask *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS3DWBTaskSensorReadData(IIS3DWBTask *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t IIS3DWBTaskSensorRegister(IIS3DWBTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t IIS3DWBTaskSensorInitTaskParams(IIS3DWBTask *_this);

/**
  * Private implementation of sensor interface methods for IIS3DWB sensor
  */
static sys_error_code_t IIS3DWBTaskSensorSetODR(IIS3DWBTask *_this, SMMessage report);
static sys_error_code_t IIS3DWBTaskSensorSetFS(IIS3DWBTask *_this, SMMessage report);
static sys_error_code_t IIS3DWBTaskSensorSetFifoWM(IIS3DWBTask *_this, SMMessage report);
static sys_error_code_t IIS3DWBTaskSensorEnable(IIS3DWBTask *_this, SMMessage report);
static sys_error_code_t IIS3DWBTaskSensorDisable(IIS3DWBTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t IIS3DWBTaskSensorIsActive(const IIS3DWBTask *_this);

static sys_error_code_t IIS3DWBTaskEnterLowPowerMode(const IIS3DWBTask *_this);

static sys_error_code_t IIS3DWBTaskConfigureIrqPin(const IIS3DWBTask *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void IIS3DWBTaskTimerCallbackFunction(ULONG param);



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
static inline sys_error_code_t IIS3DWBTaskPostReportToFront(IIS3DWBTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t IIS3DWBTaskPostReportToBack(IIS3DWBTask *_this, SMMessage *pReport);


/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static IIS3DWBTask sTaskObj;

/**
  * The class object.
  */
static IIS3DWBTaskClass_t sTheClass =
{
  /* class virtual table */
  {
    IIS3DWBTask_vtblHardwareInit,
    IIS3DWBTask_vtblOnCreateTask,
    IIS3DWBTask_vtblDoEnterPowerMode,
    IIS3DWBTask_vtblHandleError,
    IIS3DWBTask_vtblOnEnterTaskControlLoop,
    IIS3DWBTask_vtblForceExecuteStep,
    IIS3DWBTask_vtblOnEnterPowerMode
  },

  /* class::sensor_if_vtbl virtual table */
  {
    {
      {
        IIS3DWBTask_vtblAccGetId,
        IIS3DWBTask_vtblGetEventSourceIF,
        IIS3DWBTask_vtblAccGetDataInfo
      },
      IIS3DWBTask_vtblSensorEnable,
      IIS3DWBTask_vtblSensorDisable,
      IIS3DWBTask_vtblSensorIsEnabled,
      IIS3DWBTask_vtblSensorGetDescription,
      IIS3DWBTask_vtblSensorGetStatus
    },
    IIS3DWBTask_vtblAccGetODR,
    IIS3DWBTask_vtblAccGetFS,
    IIS3DWBTask_vtblAccGetSensitivity,
    IIS3DWBTask_vtblSensorSetODR,
    IIS3DWBTask_vtblSensorSetFS,
    IIS3DWBTask_vtblSensorSetFifoWM
  },

  /* ACCELEROMETER DESCRIPTOR */
  {
    "iis3dwb",
    COM_TYPE_ACC,
    {
      26667.0,
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
    IIS3DWBTaskExecuteStepState1,
    NULL,
    IIS3DWBTaskExecuteStepDatalog,
  },

  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *IIS3DWBTaskGetAccSensorIF(IIS3DWBTask *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *IIS3DWBTaskAlloc(const void *pIRQConfig, const void *pCSConfig)
{
  IIS3DWBTask *p_new_obj = SysAlloc(sizeof(IIS3DWBTask));

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

AManagedTaskEx *IIS3DWBTaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *p_name)
{
  IIS3DWBTask *p_new_obj = (IIS3DWBTask *)IIS3DWBTaskAlloc(pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *IIS3DWBTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig)
{
  IIS3DWBTask *p_obj = (IIS3DWBTask *)p_mem_block;

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

AManagedTaskEx *IIS3DWBTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                              const char *p_name)
{
  IIS3DWBTask *p_obj = (IIS3DWBTask *)IIS3DWBTaskStaticAlloc(p_mem_block, pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *IIS3DWBTaskGetSensorIF(IIS3DWBTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *IIS3DWBTaskGetEventSrcIF(IIS3DWBTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t IIS3DWBTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWBTask *p_obj = (IIS3DWBTask *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t IIS3DWBTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                              VOID **pvStackStart,
                                              ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                              ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWBTask *p_obj = (IIS3DWBTask *) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) IIS3DWB_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(IIS3DWB_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "IIS3DWB_Q", item_size / 4u, p_queue_items_buff,
                                    IIS3DWB_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_timer, "IIS3DWB_T", IIS3DWBTaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(IIS3DWB_TASK_CFG_TIMER_PERIOD_MS),
                         0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(IIS3DWB_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(IIS3DWB_ID, IIS3DWB_I2C_ADD_H, 0);
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
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, IIS3DWB_TASK_CFG_MAX_INSTANCES_COUNT);
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
  p_obj->samples_per_it = 0;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "IIS3DWB";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = IIS3DWB_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = IIS3DWB_TASK_CFG_PRIORITY;
  *pPreemptThreshold = IIS3DWB_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = IIS3DWBTaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = IIS3DWBTaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t IIS3DWBTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWBTask *p_obj = (IIS3DWBTask *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (IIS3DWBTaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      /* Deactivate the sensor */
      iis3dwb_xl_data_rate_set(p_sensor_drv, IIS3DWB_XL_ODR_OFF);
      iis3dwb_fifo_xl_batch_set(p_sensor_drv, IIS3DWB_XL_NOT_BATCHED);
      iis3dwb_fifo_mode_set(p_sensor_drv, IIS3DWB_BYPASS_MODE);

      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_timer);
      }
      else
      {
        IIS3DWBTaskConfigureIrqPin(p_obj, TRUE);
      }
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = IIS3DWBTaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->pIRQConfig != NULL)
    {
      IIS3DWBTaskConfigureIrqPin(p_obj, TRUE);
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t IIS3DWBTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  IIS3DWBTask *p_obj = (IIS3DWBTask*)_this;

  return res;
}

sys_error_code_t IIS3DWBTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (IIS3DWB_TASK_CFG_TAG)
  IIS3DWBTask *p_obj = (IIS3DWBTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = IIS3DWB_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t IIS3DWBTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWBTask *p_obj = (IIS3DWBTask *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = IIS3DWBTaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t IIS3DWBTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  IIS3DWBTask *p_obj = (IIS3DWBTask*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t IIS3DWBTask_vtblAccGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS3DWBTask *p_if_owner = (IIS3DWBTask *)((uint32_t) _this - offsetof(IIS3DWBTask, sensor_if));
  uint8_t res = p_if_owner->acc_id;

  return res;
}

IEventSrc *IIS3DWBTask_vtblGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS3DWBTask *p_if_owner = (IIS3DWBTask *)((uint32_t) _this - offsetof(IIS3DWBTask, sensor_if));
  return p_if_owner->p_event_src;
}

sys_error_code_t IIS3DWBTask_vtblAccGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  IIS3DWBTask *p_if_owner = (IIS3DWBTask *)((uint32_t) _this - offsetof(IIS3DWBTask, sensor_if));
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

float IIS3DWBTask_vtblAccGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  IIS3DWBTask *p_if_owner = (IIS3DWBTask *)((uint32_t) _this - offsetof(IIS3DWBTask, sensor_if));
  float res = p_if_owner->sensor_status.type.mems.fs;

  return res;
}

float IIS3DWBTask_vtblAccGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  IIS3DWBTask *p_if_owner = (IIS3DWBTask *)((uint32_t) _this - offsetof(IIS3DWBTask, sensor_if));
  float res = p_if_owner->sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t IIS3DWBTask_vtblAccGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS3DWBTask *p_if_owner = (IIS3DWBTask *)((uint32_t) _this - offsetof(IIS3DWBTask, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t IIS3DWBTask_vtblSensorSetODR(ISensorMems_t *_this, float odr)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWBTask *p_if_owner = (IIS3DWBTask *)((uint32_t) _this - offsetof(IIS3DWBTask, sensor_if));
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
    res = IIS3DWBTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t IIS3DWBTask_vtblSensorSetFS(ISensorMems_t *_this, float fs)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWBTask *p_if_owner = (IIS3DWBTask *)((uint32_t) _this - offsetof(IIS3DWBTask, sensor_if));
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
    res = IIS3DWBTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t IIS3DWBTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

#if IIS3DWB_FIFO_ENABLED
  IIS3DWBTask *p_if_owner = (IIS3DWBTask *)((uint32_t) _this - offsetof(IIS3DWBTask, sensor_if));
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
    res = IIS3DWBTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }
#endif

  return res;
}

sys_error_code_t IIS3DWBTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWBTask *p_if_owner = (IIS3DWBTask *)((uint32_t) _this - offsetof(IIS3DWBTask, sensor_if));
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
    res = IIS3DWBTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t IIS3DWBTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWBTask *p_if_owner = (IIS3DWBTask *)((uint32_t) _this - offsetof(IIS3DWBTask, sensor_if));
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
    res = IIS3DWBTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t IIS3DWBTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  IIS3DWBTask *p_if_owner = (IIS3DWBTask *)((uint32_t) _this - offsetof(IIS3DWBTask, sensor_if));

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

SensorDescriptor_t IIS3DWBTask_vtblSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS3DWBTask *p_if_owner = (IIS3DWBTask *)((uint32_t) _this - offsetof(IIS3DWBTask, sensor_if));
  return *p_if_owner->sensor_descriptor;
}

SensorStatus_t IIS3DWBTask_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS3DWBTask *p_if_owner = (IIS3DWBTask *)((uint32_t) _this - offsetof(IIS3DWBTask, sensor_if));

  return p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t IIS3DWBTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWBTask *p_obj = (IIS3DWBTask *) _this;
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
            res = IIS3DWBTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = IIS3DWBTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = IIS3DWBTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = IIS3DWBTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = IIS3DWBTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t IIS3DWBTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWBTask *p_obj = (IIS3DWBTask *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("IIS3DWB: new data.\r\n"));
//          if(p_obj->pIRQConfig == NULL)
//          {
//            if(TX_SUCCESS
//                != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(IIS3DWB_TASK_CFG_TIMER_PERIOD_MS), AMT_MS_TO_TICKS(IIS3DWB_TASK_CFG_TIMER_PERIOD_MS)))
//            {
//              return SYS_UNDEFINED_ERROR_CODE;
//            }
//          }

        res = IIS3DWBTaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
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

          SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("IIS3DWB: ts = %f\r\n", (float)timestamp));
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
            res = IIS3DWBTaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->sensor_status.is_active == true)
              {
                if (p_obj->pIRQConfig == NULL)
                {
                  if (TX_SUCCESS
                      != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(IIS3DWB_TASK_CFG_TIMER_PERIOD_MS),
                                         AMT_MS_TO_TICKS(IIS3DWB_TASK_CFG_TIMER_PERIOD_MS)))
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
                  IIS3DWBTaskConfigureIrqPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = IIS3DWBTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = IIS3DWBTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = IIS3DWBTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = IIS3DWBTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = IIS3DWBTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t IIS3DWBTaskPostReportToFront(IIS3DWBTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t IIS3DWBTaskPostReportToBack(IIS3DWBTask *_this, SMMessage *pReport)
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

static sys_error_code_t IIS3DWBTaskSensorInit(IIS3DWBTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t reg0 = 0;
  int32_t ret_val = 0;

  iis3dwb_pin_int1_route_t pin_int1_route =
  {
    0
  };

  // reset the sensor
  ret_val = iis3dwb_reset_set(p_sensor_drv, 1);
  do
  {
    iis3dwb_reset_get(p_sensor_drv, &reg0);
  } while (reg0);

  /*Disable the I2C interface*/
  iis3dwb_i2c_interface_set(p_sensor_drv, IIS3DWB_I2C_DISABLE);

  ret_val = iis3dwb_device_id_get(p_sensor_drv, (uint8_t *) &reg0);
  if (!ret_val)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, reg0);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB: sensor - I am 0x%x.\r\n", reg0));

  //TODO: STF - what is this?
  iis3dwb_read_reg(p_sensor_drv, IIS3DWB_CTRL1_XL, (uint8_t *) &reg0, 1);
  reg0 |= 0xA0;
  iis3dwb_write_reg(p_sensor_drv, IIS3DWB_CTRL1_XL, (uint8_t *) &reg0, 1);

  /*Set full scale*/
  if (_this->sensor_status.type.mems.fs < 3.0f)
  {
    iis3dwb_xl_full_scale_set(p_sensor_drv, IIS3DWB_2g);
  }
  else if (_this->sensor_status.type.mems.fs < 5.0f)
  {
    iis3dwb_xl_full_scale_set(p_sensor_drv, IIS3DWB_4g);
  }
  else if (_this->sensor_status.type.mems.fs < 9.0f)
  {
    iis3dwb_xl_full_scale_set(p_sensor_drv, IIS3DWB_8g);
  }
  else
  {
    iis3dwb_xl_full_scale_set(p_sensor_drv, IIS3DWB_16g);
  }

  /* Set odr */
  if (_this->sensor_status.is_active)
  {
    iis3dwb_xl_data_rate_set(p_sensor_drv, IIS3DWB_XL_ODR_26k7Hz);
    /*Set 2nd stage filter*/
    iis3dwb_xl_filt_path_on_out_set(p_sensor_drv, IIS3DWB_LP_6k3Hz);
  }
  else
  {
    iis3dwb_xl_data_rate_set(p_sensor_drv, IIS3DWB_XL_ODR_OFF);
    _this->sensor_status.is_active = false;
  }

#if IIS3DWB_FIFO_ENABLED

  if (_this->samples_per_it == 0)
  {
    uint16_t iis3dwb_wtm_level = 0;
    /* Calculation of watermark and samples per int*/
    iis3dwb_wtm_level = ((uint16_t) _this->sensor_status.type.mems.odr * (uint16_t) IIS3DWB_MAX_DRDY_PERIOD);
    if (iis3dwb_wtm_level > IIS3DWB_MAX_WTM_LEVEL)
    {
      iis3dwb_wtm_level = IIS3DWB_MAX_WTM_LEVEL;
    }
    else if (iis3dwb_wtm_level < IIS3DWB_MIN_WTM_LEVEL)
    {
      iis3dwb_wtm_level = IIS3DWB_MIN_WTM_LEVEL;
    }

    _this->samples_per_it = iis3dwb_wtm_level;
  }

  /*Set fifo in continuous / stream mode*/
  iis3dwb_fifo_mode_set(p_sensor_drv, IIS3DWB_STREAM_MODE);

  /*Set watermark*/
  iis3dwb_fifo_watermark_set(p_sensor_drv, _this->samples_per_it);
  /*Data Ready pulse mode*/
  iis3dwb_data_ready_mode_set(p_sensor_drv, IIS3DWB_DRDY_PULSED);

  /* FIFO_WTM_IA routing on pin INT1 */
  iis3dwb_pin_int1_route_get(p_sensor_drv, &pin_int1_route);
  *(uint8_t *) &(pin_int1_route) = 0;
  if (_this->pIRQConfig != NULL)
  {
    pin_int1_route.fifo_th = PROPERTY_ENABLE;
  }
  else
  {
    pin_int1_route.fifo_th = PROPERTY_DISABLE;
  }
  iis3dwb_pin_int1_route_set(p_sensor_drv, &pin_int1_route);

  /*Enable writing to FIFO*/
  iis3dwb_fifo_xl_batch_set(p_sensor_drv, IIS3DWB_XL_BATCHED_AT_26k7Hz);
#else
  _this->samples_per_it = 1;
  *(uint8_t *) &(pin_int1_route) = 0;
  if (_this->pIRQConfig != NULL)
  {
    pin_int1_route.drdy_xl = PROPERTY_ENABLE;
  }
  else
  {
    pin_int1_route.drdy_xl = PROPERTY_DISABLE;
  }
  iis3dwb_pin_int1_route_set(p_sensor_drv, &pin_int1_route);
#endif /* IIS3DWB_FIFO_ENABLED */

  return res;
}

static sys_error_code_t IIS3DWBTaskSensorReadData(IIS3DWBTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t samples_per_it = _this->samples_per_it;

#if IIS3DWB_FIFO_ENABLED
  uint8_t reg[2];
  uint16_t fifo_level = 0;
  uint16_t i;

  /* Check FIFO_WTM_IA and fifo level. We do not use PID in order to avoid reading one register twice */
  iis3dwb_read_reg(p_sensor_drv, IIS3DWB_FIFO_STATUS1, reg, 2);

  fifo_level = ((reg[1] & 0x03) << 8) + reg[0];

  if (((reg[1]) & 0x80) && (fifo_level >= samples_per_it))
  {
    iis3dwb_read_reg(p_sensor_drv, IIS3DWB_FIFO_DATA_OUT_TAG, (uint8_t *) _this->p_sensor_data_buff, samples_per_it * 7);

#if (HSD_USE_DUMMY_DATA == 1)
    int16_t *p16 = (int16_t *)_this->p_sensor_data_buff;

    for (i = 0; i < samples_per_it; i++)
    {
      *p16++ = dummyDataCounter++;
      *p16++ = dummyDataCounter++;
      *p16++ = dummyDataCounter++;
    }
#else
    /* Arrange Data */
    int16_t *p16_src = (int16_t *) _this->p_sensor_data_buff;
    int16_t *p16_dest = (int16_t *) _this->p_sensor_data_buff;
    for (i = 0; i < samples_per_it; i++)
    {
      p16_src = (int16_t *) & ((uint8_t *)(p16_src))[1];
      *p16_dest++ = *p16_src++;
      *p16_dest++ = *p16_src++;
      *p16_dest++ = *p16_src++;
    }
#endif /* HSD_USE_DUMMY_DATA */
  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }
#else
  iis3dwb_read_reg(p_sensor_drv, IIS3DWB_OUTX_L_A, (uint8_t *) _this->p_sensor_data_buff, samples_per_it * 6);
#if (HSD_USE_DUMMY_DATA == 1)
  int16_t *p16 = (int16_t *)_this->p_sensor_data_buff;
  for (i = 0; i < samples_per_it ; i++)
  {
    *p16++ = dummyDataCounter++;
    *p16++ = dummyDataCounter++;
    *p16++ = dummyDataCounter++;
  }
#endif /* HSD_USE_DUMMY_DATA */
#endif /* IIS3DWB_FIFO_ENABLED */

  return res;
}

static sys_error_code_t IIS3DWBTaskSensorRegister(IIS3DWBTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *acc_if = (ISensor_t *) IIS3DWBTaskGetAccSensorIF(_this);
  _this->acc_id = SMAddSensor(acc_if);

  return res;
}

static sys_error_code_t IIS3DWBTaskSensorInitTaskParams(IIS3DWBTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* ACCELEROMETER SENSOR STATUS */
  _this->sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->sensor_status.is_active = TRUE;
  _this->sensor_status.type.mems.fs = 16.0f;
  _this->sensor_status.type.mems.sensitivity = 0.0000305f * _this->sensor_status.type.mems.fs;
  _this->sensor_status.type.mems.odr = 26667.0f;
  _this->sensor_status.type.mems.measured_odr = 0.0f;
  EMD_Init(&_this->data, _this->p_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);

  return res;
}

static sys_error_code_t IIS3DWBTaskSensorSetODR(IIS3DWBTask *_this, SMMessage report)
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
      iis3dwb_xl_data_rate_set(p_sensor_drv, IIS3DWB_XL_ODR_OFF);
      /* Do not update the model in case of odr = 0 */
      odr = _this->sensor_status.type.mems.odr;
    }
    else
    {
      odr = 26667.0f;
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

static sys_error_code_t IIS3DWBTaskSensorSetFS(IIS3DWBTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float fs = (float) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    if (fs < 3.0f)
    {
      iis3dwb_xl_full_scale_set(p_sensor_drv, IIS3DWB_2g);
      fs = 2.0f;
    }
    else if (fs < 5.0f)
    {
      iis3dwb_xl_full_scale_set(p_sensor_drv, IIS3DWB_4g);
      fs = 4.0f;
    }
    else if (fs < 9.0f)
    {
      iis3dwb_xl_full_scale_set(p_sensor_drv, IIS3DWB_8g);
      fs = 8.0f;
    }
    else
    {
      iis3dwb_xl_full_scale_set(p_sensor_drv, IIS3DWB_16g);
      fs = 16.0f;
    }

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

static sys_error_code_t IIS3DWBTaskSensorSetFifoWM(IIS3DWBTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t iis3dwb_wtm_level = (uint16_t)report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    if (iis3dwb_wtm_level > IIS3DWB_MAX_WTM_LEVEL)
    {
      iis3dwb_wtm_level = IIS3DWB_MAX_WTM_LEVEL;
    }
    _this->samples_per_it = iis3dwb_wtm_level;

    /*Set fifo in continuous / stream mode*/
    iis3dwb_fifo_mode_set(p_sensor_drv, IIS3DWB_STREAM_MODE);

    /*Set watermark*/
    iis3dwb_fifo_watermark_set(p_sensor_drv, _this->samples_per_it);
    /*Data Ready pulse mode*/
    iis3dwb_data_ready_mode_set(p_sensor_drv, IIS3DWB_DRDY_PULSED);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t IIS3DWBTaskSensorEnable(IIS3DWBTask *_this, SMMessage report)
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

static sys_error_code_t IIS3DWBTaskSensorDisable(IIS3DWBTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    _this->sensor_status.is_active = FALSE;
    iis3dwb_xl_data_rate_set(p_sensor_drv, IIS3DWB_XL_ODR_OFF);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t IIS3DWBTaskSensorIsActive(const IIS3DWBTask *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.is_active;
}

static sys_error_code_t IIS3DWBTaskEnterLowPowerMode(const IIS3DWBTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  if (iis3dwb_xl_data_rate_set(p_sensor_drv, IIS3DWB_XL_ODR_OFF))
  {
    res = SYS_SENSOR_TASK_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_OP_ERROR_CODE);
  }

  return res;
}

static sys_error_code_t IIS3DWBTaskConfigureIrqPin(const IIS3DWBTask *_this, boolean_t LowPower)
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

static void IIS3DWBTaskTimerCallbackFunction(ULONG param)
{
  IIS3DWBTask *p_obj = (IIS3DWBTask *) param;
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

void IIS3DWBTask_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((IIS3DWBTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}
