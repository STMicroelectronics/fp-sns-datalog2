/**
  ******************************************************************************
  * @file    H3LIS331DLTask.c
  * @author  AME/AMS/S2CSupport
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "H3LIS331DLTask.h"
#include "H3LIS331DLTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "h3lis331dl_reg.h"
#include <string.h>
#include "services/sysdebug.h"
#include "mx.h"

/* Private includes ----------------------------------------------------------*/

#ifndef H3LIS331DL_TASK_CFG_STACK_DEPTH
#define H3LIS331DL_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*8)
#endif

#ifndef H3LIS331DL_TASK_CFG_PRIORITY
#define H3LIS331DL_TASK_CFG_PRIORITY                 (4)
#endif

#ifndef H3LIS331DL_TASK_CFG_IN_QUEUE_LENGTH
#define H3LIS331DL_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define H3LIS331DL_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef H3LIS331DL_TASK_CFG_TIMER_PERIOD_MS
#define H3LIS331DL_TASK_CFG_TIMER_PERIOD_MS          500
#endif

#ifndef H3LIS331DL_TASK_CFG_MAX_INSTANCES_COUNT
#define H3LIS331DL_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)                   SYS_DEBUGF3(SYS_DBG_H3LIS331DL, level, message)

#ifndef H3LIS331DL_TASK_CFG_I2C_ADDRESS
#define H3LIS331DL_TASK_CFG_I2C_ADDRESS              H3LIS331DL_I2C_ADD_H
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
typedef struct _H3LIS331DLTaskClass
{
  /**
    * H3LIS331DLTask class virtual table.
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
    * H3LIS331DLTask (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
      * Memory buffer used to allocate the map (key, value).
     */
  MTMapElement_t task_map_elements[H3LIS331DL_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} H3LIS331DLTaskClass_t;


/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t H3LIS331DLTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t H3LIS331DLTaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t H3LIS331DLTaskSensorInit(H3LIS331DLTask *_this);

/**
  * Read the Int1 Src from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t H3LIS331DLTaskSensorReadInt1Src(H3LIS331DLTask *_this, h3lis331dl_int1_src_t *int1_src);

/**
  * Read the Status from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t H3LIS331DLTaskSensorReadStatus(H3LIS331DLTask *_this, uint8_t *status);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t H3LIS331DLTaskSensorReadData(H3LIS331DLTask *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t H3LIS331DLTaskSensorRegister(H3LIS331DLTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t H3LIS331DLTaskSensorInitTaskParams(H3LIS331DLTask *_this);

/**
  * Private implementation of sensor interface methods for H3LIS331DL sensor
  */
static sys_error_code_t H3LIS331DLTaskSensorSetODR(H3LIS331DLTask *_this, SMMessage report);
static sys_error_code_t H3LIS331DLTaskSensorSetFS(H3LIS331DLTask *_this, SMMessage report);
static sys_error_code_t H3LIS331DLTaskSensorEnable(H3LIS331DLTask *_this, SMMessage report);
static sys_error_code_t H3LIS331DLTaskSensorDisable(H3LIS331DLTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t H3LIS331DLTaskSensorIsActive(const H3LIS331DLTask *_this);

static sys_error_code_t H3LIS331DLTaskEnterLowPowerMode(const H3LIS331DLTask *_this);

static sys_error_code_t H3LIS331DLTaskConfigureIrqPin(const H3LIS331DLTask *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param timer [IN] specifies the handle of the expired timer.
  */
static void H3LIS331DLTaskTimerCallbackFunction(ULONG param);


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
static inline sys_error_code_t H3LIS331DLTaskPostReportToFront(H3LIS331DLTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t H3LIS331DLTaskPostReportToBack(H3LIS331DLTask *_this, SMMessage *pReport);


/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static H3LIS331DLTask sTaskObj;

/**
  * The class object.
  */
static H3LIS331DLTaskClass_t sTheClass =
{
  /* Class virtual table */
  {
    H3LIS331DLTask_vtblHardwareInit,
    H3LIS331DLTask_vtblOnCreateTask,
    H3LIS331DLTask_vtblDoEnterPowerMode,
    H3LIS331DLTask_vtblHandleError,
    H3LIS331DLTask_vtblOnEnterTaskControlLoop,
    H3LIS331DLTask_vtblForceExecuteStep,
    H3LIS331DLTask_vtblOnEnterPowerMode
  },
  /* class::sensor_if_vtbl virtual table */
  {
    {
      {
        H3LIS331DLTask_vtblAccGetId,
        H3LIS331DLTask_vtblGetEventSourceIF,
        H3LIS331DLTask_vtblAccGetDataInfo
      },
      H3LIS331DLTask_vtblSensorEnable,
      H3LIS331DLTask_vtblSensorDisable,
      H3LIS331DLTask_vtblSensorIsEnabled,
      H3LIS331DLTask_vtblSensorGetDescription,
      H3LIS331DLTask_vtblSensorGetStatus,
      H3LIS331DLTask_vtblSensorGetStatusPointer
    },
    H3LIS331DLTask_vtblAccGetODR,
    H3LIS331DLTask_vtblAccGetFS,
    H3LIS331DLTask_vtblAccGetSensitivity,
    H3LIS331DLTask_vtblSensorSetODR,
    H3LIS331DLTask_vtblSensorSetFS,
    H3LIS331DLTask_vtblSensorSetFifoWM
  },

  /* ACCELEROMETER DESCRIPTOR */
  {
    "h3lis331dl",
    COM_TYPE_ACC
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    H3LIS331DLTaskExecuteStepState1,
    NULL,
    H3LIS331DLTaskExecuteStepDatalog,
  },

  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *H3LIS331DLTaskGetAccSensorIF(H3LIS331DLTask *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *H3LIS331DLTaskAlloc(const void *pIRQConfig, const void *pCSConfig)
{
  H3LIS331DLTask *p_new_obj = SysAlloc(sizeof(H3LIS331DLTask));

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

AManagedTaskEx *H3LIS331DLTaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *p_name)
{
  H3LIS331DLTask *p_new_obj = (H3LIS331DLTask *)H3LIS331DLTaskAlloc(pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *H3LIS331DLTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig)
{
  H3LIS331DLTask *p_obj = (H3LIS331DLTask *)p_mem_block;

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

AManagedTaskEx *H3LIS331DLTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                                 const char *p_name)
{
  H3LIS331DLTask *p_obj = (H3LIS331DLTask *)H3LIS331DLTaskStaticAlloc(p_mem_block, pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *H3LIS331DLTaskGetSensorIF(H3LIS331DLTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *H3LIS331DLTaskGetEventSrcIF(H3LIS331DLTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t H3LIS331DLTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  H3LIS331DLTask *p_obj = (H3LIS331DLTask *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t H3LIS331DLTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                                 VOID **pvStackStart,
                                                 ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                                 ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  H3LIS331DLTask *p_obj = (H3LIS331DLTask *) _this;

  /* Create task specific sw resources */
  uint32_t item_size = (uint32_t) H3LIS331DL_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(H3LIS331DL_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  else if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "H3LIS331DL_Q", item_size / 4u, p_queue_items_buff,
                                         H3LIS331DL_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_timer, "H3LIS331DL_T", H3LIS331DLTaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(H3LIS331DL_TASK_CFG_TIMER_PERIOD_MS),
                         0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(H3LIS331DL_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0x40);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(H3LIS331DL_ID, H3LIS331DL_TASK_CFG_I2C_ADDRESS, 0);
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
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, H3LIS331DL_TASK_CFG_MAX_INSTANCES_COUNT);
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
  *pName = "H3LIS331DL";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = H3LIS331DL_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = H3LIS331DL_TASK_CFG_PRIORITY;
  *pPreemptThreshold = H3LIS331DL_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = H3LIS331DLTaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = H3LIS331DLTaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("H3LIS331DL: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t H3LIS331DLTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  H3LIS331DLTask *p_obj = (H3LIS331DLTask *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;


  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (H3LIS331DLTaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("H3LIS331DL: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      /* Deactivate the sensor */
      h3lis331dl_data_rate_set(p_sensor_drv, H3LIS331DL_ODR_OFF);

      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_timer);
      }
      else
      {
        H3LIS331DLTaskConfigureIrqPin(p_obj, TRUE);
      }
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("H3LIS331DL: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = H3LIS331DLTaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->pIRQConfig != NULL)
    {
      H3LIS331DLTaskConfigureIrqPin(p_obj, TRUE);
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("H3LIS331DL: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t H3LIS331DLTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  H3LIS331DLTask *p_obj = (H3LIS331DLTask*)_this;

  return res;
}

sys_error_code_t H3LIS331DLTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("H3LIS331DL: start.\r\n"));
#if defined(ENABLE_THREADX_DBG_PIN) && defined (H3LIS331DL_TASK_CFG_TAG)
  H3LIS331DLTask *p_obj = (H3LIS331DLTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = H3LIS331DL_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t H3LIS331DLTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  H3LIS331DLTask *p_obj = (H3LIS331DLTask *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = H3LIS331DLTaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t H3LIS331DLTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t H3LIS331DLTask_vtblAccGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  H3LIS331DLTask *p_if_owner = (H3LIS331DLTask *)((uint32_t) _this - offsetof(H3LIS331DLTask, sensor_if));
  uint8_t res = p_if_owner->acc_id;
  return res;
}

IEventSrc *H3LIS331DLTask_vtblGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  H3LIS331DLTask *p_if_owner = (H3LIS331DLTask *)((uint32_t) _this - offsetof(H3LIS331DLTask, sensor_if));
  return p_if_owner->p_event_src;

}

sys_error_code_t H3LIS331DLTask_vtblAccGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  H3LIS331DLTask *p_if_owner = (H3LIS331DLTask *)((uint32_t) _this - offsetof(H3LIS331DLTask, sensor_if));
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

float H3LIS331DLTask_vtblAccGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  H3LIS331DLTask *p_if_owner = (H3LIS331DLTask *)((uint32_t) _this - offsetof(H3LIS331DLTask, sensor_if));
  float res = p_if_owner->sensor_status.type.mems.fs;

  return res;
}

float H3LIS331DLTask_vtblAccGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  H3LIS331DLTask *p_if_owner = (H3LIS331DLTask *)((uint32_t) _this - offsetof(H3LIS331DLTask, sensor_if));
  float res = p_if_owner->sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t H3LIS331DLTask_vtblAccGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  H3LIS331DLTask *p_if_owner = (H3LIS331DLTask *)((uint32_t) _this - offsetof(H3LIS331DLTask, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t H3LIS331DLTask_vtblSensorSetODR(ISensorMems_t *_this, float odr)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  H3LIS331DLTask *p_if_owner = (H3LIS331DLTask *)((uint32_t) _this - offsetof(H3LIS331DLTask, sensor_if));
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
      /* ODR = 0 sends only message to switch off the sensor.
       * Do not update the model in case of odr = 0 */

      p_if_owner->sensor_status.type.mems.odr = odr;
      p_if_owner->sensor_status.type.mems.measured_odr = 0.0f;
    }
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_ODR,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) odr // EG: todo - what about float values?
    };
    res = H3LIS331DLTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t H3LIS331DLTask_vtblSensorSetFS(ISensorMems_t *_this, float fs)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  H3LIS331DLTask *p_if_owner = (H3LIS331DLTask *)((uint32_t) _this - offsetof(H3LIS331DLTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.mems.fs = fs;
    p_if_owner->sensor_status.type.mems.sensitivity = 0.000030625f * p_if_owner->sensor_status.type.mems.fs;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FS,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) fs
    };
    res = H3LIS331DLTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t H3LIS331DLTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  /* H3LIS331DL does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);
  SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("H3LIS331DL: warning - SetFifoWM() not supported.\r\n"));
  return SYS_INVALID_FUNC_CALL_ERROR_CODE;
}

sys_error_code_t H3LIS331DLTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  H3LIS331DLTask *p_if_owner = (H3LIS331DLTask *)((uint32_t) _this - offsetof(H3LIS331DLTask, sensor_if));
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
    res = H3LIS331DLTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t H3LIS331DLTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  H3LIS331DLTask *p_if_owner = (H3LIS331DLTask *)((uint32_t) _this - offsetof(H3LIS331DLTask, sensor_if));
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
    res = H3LIS331DLTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t H3LIS331DLTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;

  H3LIS331DLTask *p_if_owner = (H3LIS331DLTask *)((uint32_t) _this - offsetof(H3LIS331DLTask, sensor_if));

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

SensorDescriptor_t H3LIS331DLTask_vtblSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  H3LIS331DLTask *p_if_owner = (H3LIS331DLTask *)((uint32_t) _this - offsetof(H3LIS331DLTask, sensor_if));
  return *p_if_owner->sensor_descriptor;
}

SensorStatus_t H3LIS331DLTask_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  H3LIS331DLTask *p_if_owner = (H3LIS331DLTask *)((uint32_t) _this - offsetof(H3LIS331DLTask, sensor_if));

  return p_if_owner->sensor_status;
}

SensorStatus_t *H3LIS331DLTask_vtblSensorGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  H3LIS331DLTask *p_if_owner = (H3LIS331DLTask *)((uint32_t) _this - offsetof(H3LIS331DLTask, sensor_if));

  return &p_if_owner->sensor_status;
}


/* Private function definition */
// ***************************
static sys_error_code_t H3LIS331DLTaskExecuteStepState1(AManagedTask *_this)
{
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskExecuteStepState1 called\r\n"));
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  H3LIS331DLTask *p_obj = (H3LIS331DLTask *) _this;
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
            res = H3LIS331DLTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = H3LIS331DLTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = H3LIS331DLTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = H3LIS331DLTaskSensorDisable(p_obj, report);
            break;
          default:
          {
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL: unexpected report in Run: %i\r\n", report.messageID));
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
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }

    }
  }

  return res;
}

static sys_error_code_t H3LIS331DLTaskExecuteStepDatalog(AManagedTask *_this)
{
  // SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskExecuteStepDatalog called\r\n"));
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint8_t Status = 0x00;
  H3LIS331DLTask *p_obj = (H3LIS331DLTask *) _this;
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
        // read status
        res = H3LIS331DLTaskSensorReadStatus(p_obj, &Status);

        // read data
        res = H3LIS331DLTaskSensorReadData(p_obj);

        // clear interrupt
        h3lis331dl_int1_src_t Int1Src;
        H3LIS331DLTaskSensorReadInt1Src(p_obj, &Int1Src);

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
        }
        while (0x00 != Status)
        {
          // cleanup data overruns
          H3LIS331DLTaskSensorReadInt1Src(p_obj, &Int1Src);
          H3LIS331DLTaskSensorReadData(p_obj);
          H3LIS331DLTaskSensorReadStatus(p_obj, &Status);
        }
        break;
      }
      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
            res = H3LIS331DLTaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->sensor_status.is_active == true)
              {
                if (p_obj->pIRQConfig == NULL)
                {
                  if (TX_SUCCESS
                      != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->h3lis331dl_task_cfg_timer_period_ms),
                                         AMT_MS_TO_TICKS(p_obj->h3lis331dl_task_cfg_timer_period_ms)))
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

                  H3LIS331DLTaskConfigureIrqPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = H3LIS331DLTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = H3LIS331DLTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = H3LIS331DLTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = H3LIS331DLTaskSensorDisable(p_obj, report);
            break;
          default:
          {
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL: unexpected report in Datalog: %i\r\n", report.messageID));
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

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static inline sys_error_code_t H3LIS331DLTaskPostReportToFront(H3LIS331DLTask *_this, SMMessage *pReport)
{
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskPostReportToFront called\r\n"));
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

static inline sys_error_code_t H3LIS331DLTaskPostReportToBack(H3LIS331DLTask *_this, SMMessage *pReport)
{
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskPostReportToBack called\r\n"));
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

static sys_error_code_t H3LIS331DLTaskSensorInit(H3LIS331DLTask *_this)
{
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskSensorInit called\r\n"));
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  int32_t ret_val = 0;
  uint8_t reg0;
  h3lis331dl_dr_t H3LIS331DL_odr = H3LIS331DL_ODR_OFF;
  uint8_t status_reg = 0;

  /* INT setup */
  h3lis331dl_i1_cfg_t int1_route = H3LIS331DL_PAD1_INT1_SRC;

  // check device ID
  ret_val = h3lis331dl_device_id_get(p_sensor_drv, &reg0);
  if (ret_val == 0)
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("H3LIS331DL: sensor - I am 0x%x.\r\n", reg0));
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, reg0);
    if (H3LIS331DL_ID != reg0)
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("H3LIS331DL not found, stopping.\r\n"));
      while (1);
    }
    else
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("H3LIS331DL found.\r\n"));
    }
  }

  // set SPI 4 wire interface
  h3lis331dl_spi_mode_set(p_sensor_drv, H3LIS331DL_SPI_4_WIRE);

  /* Enable BDU */
  h3lis331dl_block_data_update_set(p_sensor_drv, PROPERTY_ENABLE);

  /* Enable all sensors */
  {

  }

  /* Configure filtering chain */
  h3lis331dl_hp_path_set(p_sensor_drv, H3LIS331DL_HP_DISABLE);

  /* Full scale selection. */
  if (_this->sensor_status.type.mems.fs < 101.0f)
  {
    h3lis331dl_full_scale_set(p_sensor_drv, H3LIS331DL_100g);
  }
  else if (_this->sensor_status.type.mems.fs < 201.0f)
  {
    h3lis331dl_full_scale_set(p_sensor_drv, H3LIS331DL_200g);
  }
  else
  {
    h3lis331dl_full_scale_set(p_sensor_drv, H3LIS331DL_400g);
  }

  // setup irq - int 1 data ready, latched
  _this->samples_per_it = 1;
  if (_this->pIRQConfig != NULL)
  {
    int1_route = H3LIS331DL_PAD1_DRDY;
  }
  else
  {
    int1_route = H3LIS331DL_PAD1_INT1_SRC;
  }

  h3lis331dl_pin_int1_route_set(p_sensor_drv, int1_route);
  h3lis331dl_int1_notification_set(p_sensor_drv, H3LIS331DL_INT1_LATCHED);

  // clear any stale data/status
  h3lis331dl_flag_data_ready_get(p_sensor_drv, &status_reg);
  while (0 != status_reg)
  {
    uint8_t data[6] = {0};
    h3lis331dl_read_reg(p_sensor_drv, H3LIS331DL_OUT_X_L, data, 6);
    h3lis331dl_flag_data_ready_get(p_sensor_drv, &status_reg);
  }


  /* Output data rate selection - also on/off/lowpower. */
  if (_this->sensor_status.type.mems.odr < 0.6f)
  {
    H3LIS331DL_odr = H3LIS331DL_ODR_Hz5;
  }
  else if (_this->sensor_status.type.mems.odr < 1.1f)
  {
    H3LIS331DL_odr = H3LIS331DL_ODR_1Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 2.1f)
  {
    H3LIS331DL_odr = H3LIS331DL_ODR_2Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 5.1f)
  {
    H3LIS331DL_odr = H3LIS331DL_ODR_5Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 11.0f)
  {
    H3LIS331DL_odr = H3LIS331DL_ODR_10Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 51.0f)
  {
    H3LIS331DL_odr = H3LIS331DL_ODR_50Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 101.0f)
  {
    H3LIS331DL_odr = H3LIS331DL_ODR_100Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 401.0f)
  {
    H3LIS331DL_odr = H3LIS331DL_ODR_400Hz;
  }
  else
  {
    H3LIS331DL_odr = H3LIS331DL_ODR_1kHz;
  }

  if (_this->sensor_status.is_active)
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("H3LIS331DL Is Active with ODR: 0x%01x.\r\n", H3LIS331DL_odr));
    h3lis331dl_data_rate_set(p_sensor_drv, H3LIS331DL_odr);
  }
  else
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("H3LIS331DL Is Inactive with ODR: 0x%01x.\r\n", H3LIS331DL_ODR_OFF));
    h3lis331dl_data_rate_set(p_sensor_drv, H3LIS331DL_ODR_OFF);
  }

  _this->h3lis331dl_task_cfg_timer_period_ms = (uint16_t)(1000.0f / _this->sensor_status.type.mems.odr);

  return res;
}

static sys_error_code_t H3LIS331DLTaskSensorReadInt1Src(H3LIS331DLTask *_this, h3lis331dl_int1_src_t *int1_src)
{
  //SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskSensorReadInt1Src called\r\n"));
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  res = h3lis331dl_read_reg(p_sensor_drv, H3LIS331DL_INT1_SRC, (uint8_t *) int1_src, 1);

  return res;
}

static sys_error_code_t H3LIS331DLTaskSensorReadStatus(H3LIS331DLTask *_this, uint8_t *status)
{
  //SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskSensorReadStatus called\r\n"));
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  res = h3lis331dl_read_reg(p_sensor_drv, H3LIS331DL_STATUS_REG, (uint8_t *) status, 1);

  return res;
}

static sys_error_code_t H3LIS331DLTaskSensorReadData(H3LIS331DLTask *_this)
{
  // SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskSensorReadData called\r\n"));
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t samples_per_it = _this->samples_per_it;

  res = h3lis331dl_read_reg(p_sensor_drv, H3LIS331DL_OUT_X_L, (uint8_t *) _this->p_sensor_data_buff, samples_per_it * 6);

  if (!SYS_IS_ERROR_CODE(res))
  {
#if (HSD_USE_DUMMY_DATA == 1)
    uint16_t i = 0;
    int16_t *p16 = (int16_t *)_this->p_sensor_data_buff;
    for (i = 0; i < samples_per_it * 3 ; i++)
    {
      *p16++ = dummyDataCounter++;
    }
#endif
  }

  return res;
}

static sys_error_code_t H3LIS331DLTaskSensorRegister(H3LIS331DLTask *_this)
{
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskSensorRegister called\r\n"));
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *acc_if = (ISensor_t *) H3LIS331DLTaskGetAccSensorIF(_this);
  _this->acc_id = SMAddSensor(acc_if);

  return res;
}

static sys_error_code_t H3LIS331DLTaskSensorInitTaskParams(H3LIS331DLTask *_this)
{
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskSensorInitTaskParams called\r\n"));
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* ACCELEROMETER SENSOR STATUS */
  _this->sensor_status.is_active = TRUE;
  _this->sensor_status.type.mems.fs = 100.0f;
  _this->sensor_status.type.mems.sensitivity = 0.000030625f * _this->sensor_status.type.mems.fs;
  _this->sensor_status.type.mems.odr = 1000.0f;
  _this->sensor_status.type.mems.measured_odr = 0.0f;
  EMD_Init(&_this->data, _this->p_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);

  return res;
}

static sys_error_code_t H3LIS331DLTaskSensorSetODR(H3LIS331DLTask *_this, SMMessage report)
{
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskSensorSetODR called\r\n"));
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float odr = (float) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    if (odr < 0.1f)
    {
      h3lis331dl_data_rate_set(p_sensor_drv, H3LIS331DL_ODR_OFF);
      /* Do not update the model in case of ODR = 0 */
      odr = _this->sensor_status.type.mems.odr;
    }
    else if (odr < 0.6f)
    {
      odr = 0.5f;
    }
    else if (odr < 1.1f)
    {
      odr = 1.0f;
    }
    else if (odr < 2.1f)
    {
      odr = 2.0f;
    }
    else if (odr < 5.1f)
    {
      odr = 5.0f;
    }
    else if (odr < 11.0f)
    {
      odr = 10.0f;
    }
    else if (odr < 51.0f)
    {
      odr = 50.0f;
    }
    else if (odr < 101.0f)
    {
      odr = 100.0f;
    }
    else if (odr < 401.0f)
    {
      odr = 400.0f;
    }
    else
    {
      odr = 1000.0f;
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

static sys_error_code_t H3LIS331DLTaskSensorSetFS(H3LIS331DLTask *_this, SMMessage report)
{
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskSensorSetFS called\r\n"));
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  float fs = (float) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    if (fs < 101.0f)
    {
      fs = 100.0f;
    }
    else if (fs < 201.0f)
    {
      fs = 200.0f;
    }
    else
    {
      fs = 400.0f;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->sensor_status.type.mems.fs = fs;
      _this->sensor_status.type.mems.sensitivity = 0.000030625f * _this->sensor_status.type.mems.fs;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t H3LIS331DLTaskSensorEnable(H3LIS331DLTask *_this, SMMessage report)
{
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskSensorEnable called\r\n"));
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

static sys_error_code_t H3LIS331DLTaskSensorDisable(H3LIS331DLTask *_this, SMMessage report)
{
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskSensorDisable called\r\n"));
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    _this->sensor_status.is_active = FALSE;
    h3lis331dl_data_rate_set(p_sensor_drv, H3LIS331DL_ODR_OFF);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t H3LIS331DLTaskSensorIsActive(const H3LIS331DLTask *_this)
{
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskSensorIsActive called\r\n"));
  assert_param(_this != NULL);
  return _this->sensor_status.is_active;
}

static sys_error_code_t H3LIS331DLTaskEnterLowPowerMode(const H3LIS331DLTask *_this)
{
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskEnterLowPowerMode called\r\n"));
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  if (h3lis331dl_data_rate_set(p_sensor_drv, H3LIS331DL_ODR_OFF))
  {
    res = SYS_SENSOR_TASK_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_OP_ERROR_CODE);
  }

  return res;
}

static sys_error_code_t H3LIS331DLTaskConfigureIrqPin(const H3LIS331DLTask *_this, boolean_t LowPower)
{
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskConfigureIrqPin called\r\n"));
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

static void H3LIS331DLTaskTimerCallbackFunction(ULONG param)
{
  H3LIS331DLTask *p_obj = (H3LIS331DLTask *) param;
  SMMessage report;

  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTaskTimerCallbackFunction called\r\n"));

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

void H3LIS331DLTask_EXTI_Callback(uint16_t nPin)
{
  //SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("H3LIS331DL H3LIS331DLTask_EXTI_Callback called\r\n"));
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((H3LIS331DLTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}
