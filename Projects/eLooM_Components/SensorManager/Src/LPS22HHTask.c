/**
  ******************************************************************************
  * @file    LPS22HHTask.c
  * @author  SRA - MCD
  * @brief   This file provides a set of functions to handle lps22hh sensor
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
#include "LPS22HHTask.h"
#include "LPS22HHTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "lps22hh_reg.h"
#include <string.h>
#include <stdlib.h>
#include "services/sysdebug.h"

/* Private includes ----------------------------------------------------------*/

#ifndef LPS22HH_TASK_CFG_STACK_DEPTH
#define LPS22HH_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*10)
#endif

#ifndef LPS22HH_TASK_CFG_PRIORITY
#define LPS22HH_TASK_CFG_PRIORITY                 4
#endif

#ifndef LPS22HH_TASK_CFG_IN_QUEUE_LENGTH
#define LPS22HH_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define LPS22HH_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef LPS22HH_TASK_CFG_TIMER_PERIOD_MS
#define LPS22HH_TASK_CFG_TIMER_PERIOD_MS          1000
#endif

#ifndef LPS22HH_TASK_CFG_MAX_INSTANCES_COUNT
#define LPS22HH_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, report)                SYS_DEBUGF3(SYS_DBG_LPS22HH, level, report)

#ifndef LPS22HH_TASK_CFG_I2C_ADDRESS
#define LPS22HH_TASK_CFG_I2C_ADDRESS              LPS22HH_I2C_ADD_H
#endif

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter_press = 0;
static uint16_t dummyDataCounter_temp = 0;
#endif

#ifndef FIFO_WATERMARK
#define FIFO_WATERMARK 5
#endif

/**
  * Class object declaration
  */
typedef struct _LPS22HHTaskClass
{
  /**
    * LPS22HHTask class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * Temperature IF virtual table.
    */
  const ISensorMems_vtbl temp_sensor_if_vtbl;

  /**
    * Pressure IF virtual table.
    */
  const ISensorMems_vtbl press_sensor_if_vtbl;

  /**
    * Specifies temperature sensor capabilities.
    */
  const SensorDescriptor_t temp_class_descriptor;

  /**
    * Specifies pressure sensor capabilities.
    */
  const SensorDescriptor_t press_class_descriptor;

  /**
    * LPS22HHTask (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[LPS22HH_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} LPS22HHTaskClass_t;

/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LPS22HHTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LPS22HHTaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LPS22HHTaskSensorInit(LPS22HHTask *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LPS22HHTaskSensorReadData(LPS22HHTask *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t LPS22HHTaskSensorRegister(LPS22HHTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t LPS22HHTaskSensorInitTaskParams(LPS22HHTask *_this);

/**
  * Private implementation of sensor interface methods for LPS22HH sensor
  */
static sys_error_code_t LPS22HHTaskSensorSetODR(LPS22HHTask *_this, SMMessage report);
static sys_error_code_t LPS22HHTaskSensorSetFS(LPS22HHTask *_this, SMMessage report);
static sys_error_code_t LPS22HHTaskSensorSetFifoWM(LPS22HHTask *_this, SMMessage report);
static sys_error_code_t LPS22HHTaskSensorEnable(LPS22HHTask *_this, SMMessage report);
static sys_error_code_t LPS22HHTaskSensorDisable(LPS22HHTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t LPS22HHTaskSensorIsActive(const LPS22HHTask *_this);

static sys_error_code_t LPS22HHTaskEnterLowPowerMode(const LPS22HHTask *_this);

static sys_error_code_t LPS22HHTaskConfigureIrqPin(const LPS22HHTask *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void LPS22HHTaskTimerCallbackFunction(ULONG param);

/**
  * Given a interface pointer it return the instance of the object that implement the interface.
  *
  * @param p_if [IN] specifies a sensor interface implemented by the task object.
  * @return the instance of the task object that implements the given interface.
  */
static inline LPS22HHTask *LPS22HHTaskGetOwnerFromISensorIF(ISensor_t *p_if);


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
static inline sys_error_code_t LPS22HHTaskPostReportToFront(LPS22HHTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t LPS22HHTaskPostReportToBack(LPS22HHTask *_this, SMMessage *pReport);

/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static LPS22HHTask *sTaskObj = NULL;

/**
  * The class object.
  */
static LPS22HHTaskClass_t sTheClass =
{
  /* class virtual table */
  {
    LPS22HHTask_vtblHardwareInit,
    LPS22HHTask_vtblOnCreateTask,
    LPS22HHTask_vtblDoEnterPowerMode,
    LPS22HHTask_vtblHandleError,
    LPS22HHTask_vtblOnEnterTaskControlLoop,
    LPS22HHTask_vtblForceExecuteStep,
    LPS22HHTask_vtblOnEnterPowerMode
  },

  /* class::temp_sensor_if_vtbl virtual table */
  {
    {
      {
        LPS22HHTask_vtblTempGetId,
        LPS22HHTask_vtblTempGetEventSourceIF,
        LPS22HHTask_vtblTempGetDataInfo
      },
      LPS22HHTask_vtblSensorEnable,
      LPS22HHTask_vtblSensorDisable,
      LPS22HHTask_vtblSensorIsEnabled,
      LPS22HHTask_vtblTempGetDescription,
      LPS22HHTask_vtblTempGetStatus,
      LPS22HHTask_vtblTempGetStatusPointer
    },
    LPS22HHTask_vtblTempGetODR,
    LPS22HHTask_vtblTempGetFS,
    LPS22HHTask_vtblTempGetSensitivity,
    LPS22HHTask_vtblSensorSetODR,
    LPS22HHTask_vtblSensorSetFS,
    LPS22HHTask_vtblSensorSetFifoWM,
  },

  /* class::press_sensor_if_vtbl virtual table */
  {
    {
      {
        LPS22HHTask_vtblPressGetId,
        LPS22HHTask_vtblPressGetEventSourceIF,
        LPS22HHTask_vtblPressGetDataInfo
      },
      LPS22HHTask_vtblSensorEnable,
      LPS22HHTask_vtblSensorDisable,
      LPS22HHTask_vtblSensorIsEnabled,
      LPS22HHTask_vtblPressGetDescription,
      LPS22HHTask_vtblPressGetStatus,
      LPS22HHTask_vtblPressGetStatusPointer
    },
    LPS22HHTask_vtblPressGetODR,
    LPS22HHTask_vtblPressGetFS,
    LPS22HHTask_vtblPressGetSensitivity,
    LPS22HHTask_vtblSensorSetODR,
    LPS22HHTask_vtblSensorSetFS,
    LPS22HHTask_vtblSensorSetFifoWM,
  },

  /* TEMPERATURE DESCRIPTOR */
  {
    "lps22hh",
    COM_TYPE_TEMP
  },
  /* PRESSURE DESCRIPTOR */
  {
    "lps22hh",
    COM_TYPE_PRESS
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    LPS22HHTaskExecuteStepState1,
    NULL,
    LPS22HHTaskExecuteStepDatalog,
  },

  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *LPS22HHTaskGetTempSensorIF(LPS22HHTask *_this)
{
  return (ISourceObservable *) & (_this->temp_sensor_if);
}

ISourceObservable *LPS22HHTaskGetPressSensorIF(LPS22HHTask *_this)
{
  return (ISourceObservable *) & (_this->press_sensor_if);
}

AManagedTaskEx *LPS22HHTaskAlloc(const void *pIRQConfig, const void *pCSConfig)
{
  LPS22HHTask *p_new_obj = SysAlloc(sizeof(LPS22HHTask));

  if (p_new_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_new_obj->super);

    p_new_obj->super.vptr = &sTheClass.vtbl;
    p_new_obj->temp_sensor_if.vptr = &sTheClass.temp_sensor_if_vtbl;
    p_new_obj->press_sensor_if.vptr = &sTheClass.press_sensor_if_vtbl;
    p_new_obj->temp_sensor_descriptor = &sTheClass.temp_class_descriptor;
    p_new_obj->press_sensor_descriptor = &sTheClass.press_class_descriptor;

    p_new_obj->pIRQConfig = (MX_GPIOParams_t *) pIRQConfig;
    p_new_obj->pCSConfig = (MX_GPIOParams_t *) pCSConfig;

    strcpy(p_new_obj->temp_sensor_status.p_name, sTheClass.temp_class_descriptor.p_name);
    strcpy(p_new_obj->press_sensor_status.p_name, sTheClass.press_class_descriptor.p_name);
  }

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *LPS22HHTaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *p_name)
{
  LPS22HHTask *p_new_obj = (LPS22HHTask *)LPS22HHTaskAlloc(pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->temp_sensor_status.p_name, p_name);
  strcpy(p_new_obj->press_sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *LPS22HHTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig)
{
  LPS22HHTask *p_obj = (LPS22HHTask *) p_mem_block;

  if (p_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_obj->super);

    p_obj->super.vptr = &sTheClass.vtbl;
    p_obj->temp_sensor_if.vptr = &sTheClass.temp_sensor_if_vtbl;
    p_obj->press_sensor_if.vptr = &sTheClass.press_sensor_if_vtbl;
    p_obj->temp_sensor_descriptor = &sTheClass.temp_class_descriptor;
    p_obj->press_sensor_descriptor = &sTheClass.press_class_descriptor;

    p_obj->pIRQConfig = (MX_GPIOParams_t *) pIRQConfig;
    p_obj->pCSConfig = (MX_GPIOParams_t *) pCSConfig;
  }

  return (AManagedTaskEx *) p_obj;
}

AManagedTaskEx *LPS22HHTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                              const char *p_name)
{
  LPS22HHTask *p_obj = (LPS22HHTask *) LPS22HHTaskStaticAlloc(p_mem_block, pIRQConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->temp_sensor_status.p_name, p_name);
  strcpy(p_obj->press_sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *LPS22HHTaskGetSensorIF(LPS22HHTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *LPS22HHTaskGetTempEventSrcIF(LPS22HHTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *)_this->p_temp_event_src;
}

IEventSrc *LPS22HHTaskGetPressEventSrcIF(LPS22HHTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *)_this->p_press_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t LPS22HHTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_obj = (LPS22HHTask *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t LPS22HHTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                              VOID **pvStackStart,
                                              ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                              ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_obj = (LPS22HHTask *) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) LPS22HH_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(LPS22HH_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "LPS22HH_Q", item_size / 4u, p_queue_items_buff,
                                    LPS22HH_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_fifo_timer, "LPS22HH_T", LPS22HHTaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(LPS22HH_TASK_CFG_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(LPS22HH_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(LPS22HH_ID, LPS22HH_TASK_CFG_I2C_ADDRESS, 0);
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
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, LPS22HH_TASK_CFG_MAX_INSTANCES_COUNT);
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
  memset(p_obj->p_temp_data_buff, 0, sizeof(p_obj->p_temp_data_buff));
  memset(p_obj->p_press_data_buff, 0, sizeof(p_obj->p_press_data_buff));
  p_obj->press_id = 0;
  p_obj->temp_id = 1;
  p_obj->prev_timestamp = 0.0f;
  p_obj->fifo_level = 0;
  p_obj->samples_per_it = 0;
  p_obj->task_delay = 0;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "LPS22HH";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = LPS22HH_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = LPS22HH_TASK_CFG_PRIORITY;
  *pPreemptThreshold = LPS22HH_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = LPS22HHTaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = LPS22HHTaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22HH: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t LPS22HHTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_obj = (LPS22HHTask *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (LPS22HHTaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22HH: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      if (LPS22HHTaskSensorIsActive(p_obj))
      {
        /* Deactivate the sensor */
        lps22hh_data_rate_set(p_sensor_drv, (lps22hh_odr_t)(LPS22HH_POWER_DOWN | 0x10));
      }
      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_fifo_timer);
      }
      else
      {
        LPS22HHTaskConfigureIrqPin(p_obj, TRUE);
      }
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22HH: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = LPS22HHTaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22HH - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->pIRQConfig != NULL)
    {
      LPS22HHTaskConfigureIrqPin(p_obj, TRUE);
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22HH: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t LPS22HHTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  LPS22HHTask *p_obj = (LPS22HHTask*)_this;

  return res;
}

sys_error_code_t LPS22HHTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("LPS22HH: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (LPS22HH_TASK_CFG_TAG)
  LPS22HHTask *p_obj = (LPS22HHTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = LPS22HH_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t LPS22HHTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_obj = (LPS22HHTask *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = LPS22HHTaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t LPS22HHTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  LPS22HHTask *p_obj = (LPS22HHTask*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t LPS22HHTask_vtblTempGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask *)((uint32_t) _this - offsetof(LPS22HHTask, temp_sensor_if));
  uint8_t res = p_if_owner->temp_id;

  return res;
}

uint8_t LPS22HHTask_vtblPressGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask *)((uint32_t) _this - offsetof(LPS22HHTask, press_sensor_if));
  uint8_t res = p_if_owner->press_id;

  return res;
}

IEventSrc *LPS22HHTask_vtblTempGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask *)((uint32_t) _this - offsetof(LPS22HHTask, temp_sensor_if));

  return p_if_owner->p_temp_event_src;
}

IEventSrc *LPS22HHTask_vtblPressGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask *)((uint32_t) _this - offsetof(LPS22HHTask, press_sensor_if));

  return p_if_owner->p_press_event_src;
}

sys_error_code_t LPS22HHTask_vtblPressGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LPS22HHTask *p_if_owner = (LPS22HHTask *)((uint32_t) _this - offsetof(LPS22HHTask, press_sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if ((p_measured == NULL) || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = p_if_owner->press_sensor_status.type.mems.measured_odr;
    *p_nominal = p_if_owner->press_sensor_status.type.mems.odr;
  }

  return res;
}

float LPS22HHTask_vtblPressGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask *)((uint32_t) _this - offsetof(LPS22HHTask, press_sensor_if));
  float res = p_if_owner->press_sensor_status.type.mems.fs;

  return res;
}

float LPS22HHTask_vtblPressGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask *)((uint32_t) _this - offsetof(LPS22HHTask, press_sensor_if));
  float res = p_if_owner->press_sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t LPS22HHTask_vtblPressGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask *)((uint32_t) _this - offsetof(LPS22HHTask, press_sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t LPS22HHTask_vtblTempGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LPS22HHTask *p_if_owner = (LPS22HHTask *)((uint32_t) _this - offsetof(LPS22HHTask, temp_sensor_if));
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

float LPS22HHTask_vtblTempGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask *)((uint32_t) _this - offsetof(LPS22HHTask, temp_sensor_if));
  float res = p_if_owner->temp_sensor_status.type.mems.fs;

  return res;
}

float LPS22HHTask_vtblTempGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask *)((uint32_t) _this - offsetof(LPS22HHTask, temp_sensor_if));
  float res = p_if_owner->temp_sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t LPS22HHTask_vtblTempGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = (LPS22HHTask *)((uint32_t) _this - offsetof(LPS22HHTask, temp_sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t LPS22HHTask_vtblSensorSetODR(ISensorMems_t *_this, float odr)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF((ISensor_t *)_this);

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->press_sensor_status.type.mems.odr = odr;
    p_if_owner->press_sensor_status.type.mems.measured_odr = 0.0f;
    p_if_owner->temp_sensor_status.type.mems.odr = odr;
    p_if_owner->temp_sensor_status.type.mems.measured_odr = 0.0f;

    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_ODR,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.fParam = (float) odr
    };
    res = LPS22HHTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t LPS22HHTask_vtblSensorSetFS(ISensorMems_t *_this, float fs)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF((ISensor_t *)_this);

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->press_sensor_status.type.mems.fs = fs;
    p_if_owner->temp_sensor_status.type.mems.fs = fs;

    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FS,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.fParam = (float) fs
    };
    res = LPS22HHTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t LPS22HHTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF((ISensor_t *)_this);

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
    res = LPS22HHTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t LPS22HHTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    if (sensor_id == p_if_owner->press_id)
    {
      p_if_owner->press_sensor_status.is_active = TRUE;
    }
    else if (sensor_id == p_if_owner->temp_id)
    {
      p_if_owner->temp_sensor_status.is_active = TRUE;
    }
    else
    {
      /**/
    }
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_ENABLE,
      .sensorMessage.nSensorId = sensor_id
    };
    res = LPS22HHTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t LPS22HHTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    if (sensor_id == p_if_owner->press_id)
    {
      p_if_owner->press_sensor_status.is_active = FALSE;
    }
    else if (sensor_id == p_if_owner->temp_id)
    {
      p_if_owner->temp_sensor_status.is_active = FALSE;
    }
    else
    {
      /**/
    }
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_DISABLE,
      .sensorMessage.nSensorId = sensor_id
    };
    res = LPS22HHTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t LPS22HHTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->press_id)
  {
    res = p_if_owner->press_sensor_status.is_active;
  }
  else if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->temp_id)
  {
    res = p_if_owner->temp_sensor_status.is_active;
  }
  return res;
}

SensorDescriptor_t LPS22HHTask_vtblTempGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->temp_sensor_descriptor;
}

SensorDescriptor_t LPS22HHTask_vtblPressGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->press_sensor_descriptor;
}

SensorStatus_t LPS22HHTask_vtblTempGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->temp_sensor_status;
}

SensorStatus_t LPS22HHTask_vtblPressGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->press_sensor_status;
}

SensorStatus_t *LPS22HHTask_vtblTempGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);
  return &p_if_owner->temp_sensor_status;
}

SensorStatus_t *LPS22HHTask_vtblPressGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LPS22HHTask *p_if_owner = LPS22HHTaskGetOwnerFromISensorIF(_this);
  return &p_if_owner->press_sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t LPS22HHTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_obj = (LPS22HHTask *) _this;
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
            res = LPS22HHTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = LPS22HHTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = LPS22HHTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = LPS22HHTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = LPS22HHTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22HH: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22HH: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t LPS22HHTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LPS22HHTask *p_obj = (LPS22HHTask *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("LPS22HH: new data.\r\n"));
//          if(p_obj->pIRQConfig == NULL)
//          {
//            if(TX_SUCCESS != tx_timer_change(&p_obj->read_fifo_timer, AMT_MS_TO_TICKS(p_obj->task_delay), AMT_MS_TO_TICKS(p_obj->task_delay)))
//            {
//              return SYS_UNDEFINED_ERROR_CODE;
//            }
//          }

        res = LPS22HHTaskSensorReadData(p_obj);

        if (!SYS_IS_ERROR_CODE(res))
        {
#if LPS22HH_FIFO_ENABLED
          if (p_obj->fifo_level != 0)
          {
#endif
            // notify the listeners...
            double timestamp = report.sensorDataReadyMessage.fTimestamp;
            double delta_timestamp = timestamp - p_obj->prev_timestamp;
            p_obj->prev_timestamp = timestamp;

            DataEvent_t evt;

            if (p_obj->press_sensor_status.is_active)
            {
              /* update measuredODR */
              p_obj->press_sensor_status.type.mems.measured_odr = (float) p_obj->samples_per_it / (float) delta_timestamp;

              EMD_1dInit(&p_obj->data, (uint8_t *) &p_obj->p_press_data_buff[0], E_EM_FLOAT, p_obj->samples_per_it);
              DataEventInit((IEvent *) &evt, p_obj->p_press_event_src, &p_obj->data, timestamp, p_obj->press_id);
              IEventSrcSendEvent(p_obj->p_press_event_src, (IEvent *) &evt, NULL);
            }
            if (p_obj->temp_sensor_status.is_active)
            {
              /* update measuredODR */
              p_obj->temp_sensor_status.type.mems.measured_odr = (float) p_obj->samples_per_it / (float) delta_timestamp;

              EMD_1dInit(&p_obj->data, (uint8_t *) &p_obj->p_temp_data_buff[0], E_EM_FLOAT, p_obj->samples_per_it);
              DataEventInit((IEvent *) &evt, p_obj->p_temp_event_src, &p_obj->data, timestamp, p_obj->temp_id);
              IEventSrcSendEvent(p_obj->p_temp_event_src, (IEvent *) &evt, NULL);
            }
#if LPS22HH_FIFO_ENABLED
          }
#endif

        }
//            if(p_obj->pIRQConfig == NULL)
//            {
//              if(TX_SUCCESS != tx_timer_activate(&p_obj->read_fifo_timer))
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
            res = LPS22HHTaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->press_sensor_status.is_active == true || p_obj->temp_sensor_status.is_active == true)
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
                  LPS22HHTaskConfigureIrqPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = LPS22HHTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = LPS22HHTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = LPS22HHTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = LPS22HHTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = LPS22HHTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22HH: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LPS22HH: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t LPS22HHTaskPostReportToFront(LPS22HHTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t LPS22HHTaskPostReportToBack(LPS22HHTask *_this, SMMessage *pReport)
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

static sys_error_code_t LPS22HHTaskSensorInit(LPS22HHTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t reg0 = 0;
  int32_t ret_val = 0;
  float lps22hh_odr = 0.0f;
  uint8_t rst;

  /* Check device ID */
  ret_val = lps22hh_device_id_get(p_sensor_drv, (uint8_t *) &reg0);
  if (!ret_val)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, reg0);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LPS22HH: sensor - I am 0x%x.\r\n", reg0));

  /* Disable MIPI I3C(SM) interface */
  lps22hh_i3c_interface_set(p_sensor_drv, LPS22HH_I3C_DISABLE);

  /* Power down the device, set Low Noise Enable (bit 5), clear One Shot (bit 4) */
  lps22hh_data_rate_set(p_sensor_drv, (lps22hh_odr_t)(LPS22HH_POWER_DOWN | 0x10));

  /* Disable low-pass filter on LPS22HH pressure data */
  lps22hh_lp_bandwidth_set(p_sensor_drv, LPS22HH_LPF_ODR_DIV_2);

  /* Set block data update mode */
  lps22hh_block_data_update_set(p_sensor_drv, PROPERTY_ENABLE);

  /* Set autoincrement for multi-byte read/write */
  lps22hh_auto_increment_set(p_sensor_drv, PROPERTY_ENABLE);

  lps22hh_reset_set(p_sensor_drv, 1);

  do
  {
    lps22hh_reset_get(p_sensor_drv, &rst);
  } while (rst);

#if LPS22HH_FIFO_ENABLED

  /* Set fifo mode */
  uint16_t lps22hh_wtm_level = 0;
  uint16_t lps22hh_wtm_level_press;
  uint16_t lps22hh_wtm_level_temp;

  if (_this->samples_per_it == 0)
  {
    /* Calculation of watermark and samples per int*/
    lps22hh_wtm_level_press = ((uint16_t) _this->press_sensor_status.type.mems.odr * (uint16_t) LPS22HH_MAX_DRDY_PERIOD);
    lps22hh_wtm_level_temp = ((uint16_t) _this->temp_sensor_status.type.mems.odr * (uint16_t) LPS22HH_MAX_DRDY_PERIOD);

    if (_this->press_sensor_status.is_active && _this->temp_sensor_status.is_active) /* Both subSensor is active */
    {
      if (lps22hh_wtm_level_press > lps22hh_wtm_level_temp)
      {
        lps22hh_wtm_level = lps22hh_wtm_level_press;
      }
      else
      {
        lps22hh_wtm_level = lps22hh_wtm_level_temp;
      }
    }
    else /* Only one subSensor is active */
    {
      if (_this->press_sensor_status.is_active)
      {
        lps22hh_wtm_level = lps22hh_wtm_level_press;
      }
      else
      {
        lps22hh_wtm_level = lps22hh_wtm_level_temp;
      }
    }

    if (lps22hh_wtm_level > LPS22HH_MAX_WTM_LEVEL)
    {
      lps22hh_wtm_level = LPS22HH_MAX_WTM_LEVEL;
    }
    else if (lps22hh_wtm_level < LPS22HH_MIN_WTM_LEVEL)
    {
      lps22hh_wtm_level = LPS22HH_MIN_WTM_LEVEL;
    }
    _this->samples_per_it = lps22hh_wtm_level;
  }

  /* Set fifo mode */
  lps22hh_fifo_mode_set(p_sensor_drv, LPS22HH_STREAM_MODE);
  /* Set FIFO wm */
  lps22hh_fifo_watermark_set(p_sensor_drv, _this->samples_per_it);
  /* Enable FIFO stop on watermark */
  lps22hh_fifo_stop_on_wtm_set(p_sensor_drv, PROPERTY_ENABLE);

  if (_this->pIRQConfig != NULL)
  {
    lps22hh_ctrl_reg3_t int_route;

    lps22hh_int_notification_set(p_sensor_drv, LPS22HH_INT_LATCHED);

    lps22hh_pin_int_route_get(p_sensor_drv, &int_route);
    int_route.int_f_wtm = PROPERTY_ENABLE;
    lps22hh_pin_int_route_set(p_sensor_drv, &int_route);
  }

#else
  if (_this->pIRQConfig != NULL)
  {
    lps22hh_ctrl_reg3_t int_route;

    lps22hh_int_notification_set(p_sensor_drv, LPS22HH_INT_LATCHED);

    lps22hh_pin_int_route_get(p_sensor_drv, &int_route);
    int_route.drdy = PROPERTY_ENABLE;
    lps22hh_pin_int_route_set(p_sensor_drv, &int_route);
  }

  _this->samples_per_it = 1;
#endif

  /* Set odr */

  if (_this->temp_sensor_status.is_active == TRUE)
  {
    lps22hh_odr = _this->temp_sensor_status.type.mems.odr;
    _this->press_sensor_status.type.mems.odr = _this->temp_sensor_status.type.mems.odr;
  }
  else
  {
    lps22hh_odr = _this->press_sensor_status.type.mems.odr;
    _this->temp_sensor_status.type.mems.odr = _this->press_sensor_status.type.mems.odr;
  }

  if (lps22hh_odr < 2.0f)
  {
    lps22hh_data_rate_set(p_sensor_drv, LPS22HH_1_Hz);
  }
  else if (lps22hh_odr < 11.0f)
  {
    lps22hh_data_rate_set(p_sensor_drv, LPS22HH_10_Hz);
  }
  else if (lps22hh_odr < 26.0f)
  {
    lps22hh_data_rate_set(p_sensor_drv, LPS22HH_25_Hz);
  }
  else if (lps22hh_odr < 51.0f)
  {
    lps22hh_data_rate_set(p_sensor_drv, LPS22HH_50_Hz);
  }
  else if (lps22hh_odr < 76.0f)
  {
    lps22hh_data_rate_set(p_sensor_drv, LPS22HH_75_Hz);
  }
  else if (lps22hh_odr < 101.0f)
  {
    lps22hh_data_rate_set(p_sensor_drv, LPS22HH_100_Hz);
  }
  else
  {
    lps22hh_data_rate_set(p_sensor_drv, LPS22HH_200_Hz);
  }

  if ((_this->press_sensor_status.is_active) && (_this->temp_sensor_status.is_active))
  {
    _this->task_delay = (uint16_t)(
                          _this->press_sensor_status.type.mems.odr < _this->temp_sensor_status.type.mems.odr ?
                          _this->press_sensor_status.type.mems.odr :
                          _this->temp_sensor_status.type.mems.odr);
  }
  else if (_this->press_sensor_status.is_active)
  {
    _this->task_delay = (uint16_t)(_this->press_sensor_status.type.mems.odr);
  }
  else if (_this->temp_sensor_status.is_active)
  {
    _this->task_delay = (uint16_t)(_this->temp_sensor_status.type.mems.odr);
  }
  else
  {
  }

#if LPS22HH_FIFO_ENABLED
  _this->task_delay = (uint16_t)((1000.0f / _this->task_delay) * (((float)(_this->samples_per_it)) / 2.0f));
#else
  _this->task_delay = (uint16_t)(1000.0f / _this->task_delay);
#endif

  return res;
}

static sys_error_code_t LPS22HHTaskSensorReadData(LPS22HHTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t samples_per_it = _this->samples_per_it;

#if LPS22HH_FIFO_ENABLED
  lps22hh_fifo_data_level_get(p_sensor_drv, (uint8_t *)&_this->fifo_level);

  if (_this->fifo_level >= samples_per_it)
  {
    lps22hh_read_reg(p_sensor_drv, LPS22HH_FIFO_DATA_OUT_PRESS_XL, (uint8_t *) _this->p_sensor_data_buff,
                     5 * _this->fifo_level);

    uint16_t i = 0;

    for (i = 0; i < samples_per_it; i++)
    {
      uint32_t press = (((uint32_t) _this->p_sensor_data_buff[5 * i + 0])) | (((uint32_t) _this->p_sensor_data_buff[5 * i + 1]) << (8 * 1))
                       | (((uint32_t) _this->p_sensor_data_buff[5 * i + 2]) << (8 * 2));

      /* convert the 2's complement 24 bit to 2's complement 32 bit */
      if (press & 0x00800000)
      {
        press |= 0xFF000000;
      }

      uint16_t temp = *((uint16_t *)(&_this->p_sensor_data_buff[5 * i + 3]));

      if (_this->press_sensor_status.is_active && !_this->temp_sensor_status.is_active) /* Only Pressure */
      {
        _this->p_press_data_buff[i] = (float) press / 4096.0f; /* Pressure */
      }
      else if (!_this->press_sensor_status.is_active && _this->temp_sensor_status.is_active) /* Only Temperature */
      {
        _this->p_temp_data_buff[i] = (float) temp / 100.0f; /* Temperature */
      }
      else if (_this->press_sensor_status.is_active && _this->temp_sensor_status.is_active) /* Both Sub Sensors */
      {
        _this->p_press_data_buff[i] = (float) press / 4096.0f; /* Pressure */
        _this->p_temp_data_buff[i] = (float) temp / 100.0f; /* Temperature */
      }
    }
  }
  else
  {
    _this->fifo_level = 0;
    res = SYS_BASE_ERROR_CODE;
  }
#else
  lps22hh_reg_t reg;
  uint32_t data_raw_pressure;
  int16_t data_raw_temperature;

  /* Read output only if new value is available */
  lps22hh_read_reg(p_sensor_drv, LPS22HH_STATUS, (uint8_t *) &reg, 1);

  if (reg.status.p_da)
  {
    memset(&data_raw_pressure, 0x00, sizeof(uint32_t));
    lps22hh_pressure_raw_get(p_sensor_drv, &data_raw_pressure);
    _this->p_press_data_buff[0] = lps22hh_from_lsb_to_hpa(data_raw_pressure);
  }
  if (reg.status.t_da)
  {
    memset(&data_raw_temperature, 0x00, sizeof(int16_t));
    lps22hh_temperature_raw_get(p_sensor_drv, &data_raw_temperature);
    _this->p_temp_data_buff[0] = lps22hh_from_lsb_to_celsius(data_raw_temperature);
  }

#endif

#if (HSD_USE_DUMMY_DATA == 1)
  uint16_t i = 0;
  for (i = 0; i < _this->samples_per_it ; i++)
  {
    _this->p_press_data_buff[i]  = (float)(dummyDataCounter_press++);
    _this->p_temp_data_buff[i]  = (float)(dummyDataCounter_temp++);
  }

#endif

  return res;
}

static sys_error_code_t LPS22HHTaskSensorRegister(LPS22HHTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *press_if = (ISensor_t *) LPS22HHTaskGetPressSensorIF(_this);
  ISensor_t *temp_if = (ISensor_t *) LPS22HHTaskGetTempSensorIF(_this);

  _this->press_id = SMAddSensor(press_if);
  _this->temp_id = SMAddSensor(temp_if);

  return res;
}

static sys_error_code_t LPS22HHTaskSensorInitTaskParams(LPS22HHTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* PRESSURE STATUS */
  _this->press_sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->press_sensor_status.is_active = TRUE;
  _this->press_sensor_status.type.mems.fs = 1260.0f;
  _this->press_sensor_status.type.mems.sensitivity = 1.0f;
  _this->press_sensor_status.type.mems.odr = 200.0f;
  _this->press_sensor_status.type.mems.measured_odr = 0.0f;

  _this->temp_sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->temp_sensor_status.is_active = TRUE;
  _this->temp_sensor_status.type.mems.fs = 85.0f;
  _this->temp_sensor_status.type.mems.sensitivity = 1.0f;
  _this->temp_sensor_status.type.mems.odr = 200.0f;
  _this->temp_sensor_status.type.mems.measured_odr = 0.0f;

  EMD_1dInit(&_this->data, (uint8_t *) &_this->p_press_data_buff[0], E_EM_FLOAT, 1);

  return res;
}

static sys_error_code_t LPS22HHTaskSensorSetODR(LPS22HHTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float odr = (float) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->temp_id || id == _this->press_id)
  {
    if (odr < 1.0f)
    {
      /* Power down the device, set Low Noise Enable (bit 5), clear One Shot (bit 4) */
      lps22hh_data_rate_set(p_sensor_drv, (lps22hh_odr_t)(LPS22HH_POWER_DOWN | 0x10));
      /* Do not update the model in case of odr = 0 */
      odr = _this->temp_sensor_status.type.mems.odr;
      odr = _this->press_sensor_status.type.mems.odr;
    }
    else if (odr < 2.0f)
    {
      odr = 1.0f;
    }
    else if (odr < 11.0f)
    {
      odr = 10.0f;
    }
    else if (odr < 26.0f)
    {
      odr = 25.0f;
    }
    else if (odr < 51.0f)
    {
      odr = 50.0f;
    }
    else if (odr < 76.0f)
    {
      odr = 75.0f;
    }
    else if (odr < 101.0f)
    {
      odr = 100.0f;
    }
    else
    {
      odr = 200.0f;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->press_sensor_status.type.mems.odr = odr;
      _this->press_sensor_status.type.mems.measured_odr = 0.0f;
      _this->temp_sensor_status.type.mems.odr = odr;
      _this->temp_sensor_status.type.mems.measured_odr = 0.0f;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }
  _this->samples_per_it = 0;

  return res;
}

static sys_error_code_t LPS22HHTaskSensorSetFS(LPS22HHTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NOT_IMPLEMENTED_ERROR_CODE;

  return res;
}

static sys_error_code_t LPS22HHTaskSensorSetFifoWM(LPS22HHTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t lps22hh_wtm_level = (uint16_t)report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if ((id == _this->press_id) || (id == _this->temp_id))
  {
    if (lps22hh_wtm_level > LPS22HH_MAX_WTM_LEVEL)
    {
      lps22hh_wtm_level = LPS22HH_MAX_WTM_LEVEL;
    }
    _this->samples_per_it = lps22hh_wtm_level;

    /* Set fifo in continuous / stream mode*/
    lps22hh_fifo_mode_set(p_sensor_drv, LPS22HH_STREAM_MODE);

    /* Setup int for FIFO */
    lps22hh_fifo_watermark_set(p_sensor_drv, _this->samples_per_it);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }
  return res;
}

static sys_error_code_t LPS22HHTaskSensorEnable(LPS22HHTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->temp_id)
  {
    _this->temp_sensor_status.is_active = TRUE;
  }
  else if (id == _this->press_id)
  {
    _this->press_sensor_status.is_active = TRUE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t LPS22HHTaskSensorDisable(LPS22HHTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->temp_id)
  {
    _this->temp_sensor_status.is_active = FALSE;
  }
  else if (id == _this->press_id)
  {
    _this->press_sensor_status.is_active = FALSE;
  }
  else
    res = SYS_INVALID_PARAMETER_ERROR_CODE;

  return res;
}

static boolean_t LPS22HHTaskSensorIsActive(const LPS22HHTask *_this)
{
  assert_param(_this != NULL);
  return (_this->temp_sensor_status.is_active || _this->press_sensor_status.is_active);
}

static sys_error_code_t LPS22HHTaskEnterLowPowerMode(const LPS22HHTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  if (lps22hh_data_rate_set(p_sensor_drv, (lps22hh_odr_t)(LPS22HH_POWER_DOWN | 0x10)))
  {
    res = SYS_SENSOR_TASK_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_OP_ERROR_CODE);
  }

  return res;
}

static sys_error_code_t LPS22HHTaskConfigureIrqPin(const LPS22HHTask *_this, boolean_t LowPower)
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

static void LPS22HHTaskTimerCallbackFunction(ULONG param)
{

  LPS22HHTask *p_obj = (LPS22HHTask *) param;
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

void LPS22HHTask_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((LPS22HHTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}

static inline LPS22HHTask *LPS22HHTaskGetOwnerFromISensorIF(ISensor_t *p_if)
{
  assert_param(p_if != NULL);
  LPS22HHTask *p_if_owner = NULL;

  /* check if the virtual function has been called from the pressure IF */
  p_if_owner = (LPS22HHTask *)((uint32_t) p_if - offsetof(LPS22HHTask, press_sensor_if));
  if (!(p_if_owner->temp_sensor_if.vptr == &sTheClass.temp_sensor_if_vtbl)
      || !(p_if_owner->super.vptr == &sTheClass.vtbl))
  {
    /* then the virtual function has been called from the temperature IF  */
    p_if_owner = (LPS22HHTask *)((uint32_t) p_if - offsetof(LPS22HHTask, temp_sensor_if));
  }

  return p_if_owner;
}
