/**
  ******************************************************************************
  * @file    CH1Task.c
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
#include "CH1Task.h"
#include "CH1Task_vtbl.h"
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
#ifndef CH1_TASK_CFG_STACK_DEPTH
#define CH1_TASK_CFG_STACK_DEPTH        (TX_MINIMUM_STACK*10)
#endif

#ifndef CH1_TASK_CFG_PRIORITY
#define CH1_TASK_CFG_PRIORITY           4
#endif

#ifndef CH1_TASK_CFG_IN_QUEUE_LENGTH
#define CH1_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define CH1_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef CH1_TASK_CFG_MAX_INSTANCES_COUNT
#define CH1_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_CH1, level, message)


#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static int16_t dummyDataCounter = 0;
#endif


/**
  * Class object declaration
  */
typedef struct _CH1TaskClass
{
  /**
    * CH1Task class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * IF virtual table.
    */
  const ISensorMems_vtbl sensor_if_vtbl;

  /**
    * Specifies sensor capabilities.
    */
  const SensorDescriptor_t class_descriptor;

  /**
    * CH1Task (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[CH1_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} CH1TaskClass_t;


/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t CH1TaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t CH1TaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t CH1TaskSensorInit(CH1Task *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t CH1TaskSensorRegister(CH1Task *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t CH1TaskSensorInitTaskParams(CH1Task *_this);

/**
  * Private implementation of sensor interface methods for CH1 sensor
  */
static sys_error_code_t CH1TaskSensorSetODR(CH1Task *_this, SMMessage report);
static sys_error_code_t CH1TaskSensorEnable(CH1Task *_this, SMMessage report);
static sys_error_code_t CH1TaskSensorDisable(CH1Task *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t CH1TaskSensorIsActive(const CH1Task *_this);


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
static inline sys_error_code_t CH1TaskPostReportToFront(CH1Task *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t CH1TaskPostReportToBack(CH1Task *_this, SMMessage *pReport);


/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static CH1Task sTaskObj;

/**
  * The class object.
  */
static CH1TaskClass_t sTheClass =
{
  /* class virtual table */
  {
    CH1Task_vtblHardwareInit,
    CH1Task_vtblOnCreateTask,
    CH1Task_vtblDoEnterPowerMode,
    CH1Task_vtblHandleError,
    CH1Task_vtblOnEnterTaskControlLoop,
    CH1Task_vtblForceExecuteStep,
    CH1Task_vtblOnEnterPowerMode
  },

  /* class::sensor_if_vtbl virtual table */
  {
    {
      {
        CH1Task_vtblGetId,
        CH1Task_vtblGetEventSourceIF,
        CH1Task_vtblGetDataInfo
      },
      CH1Task_vtblSensorEnable,
      CH1Task_vtblSensorDisable,
      CH1Task_vtblSensorIsEnabled,
      CH1Task_vtblSensorGetDescription,
      CH1Task_vtblSensorGetStatus,
      CH1Task_vtblSensorGetStatusPointer
    },
    CH1Task_vtblGetODR,
    CH1Task_vtblGetFS,
    CH1Task_vtblGetSensitivity,
    CH1Task_vtblSensorSetODR,
    CH1Task_vtblSensorSetFS,
    CH1Task_vtblSensorSetFifoWM,
  },

  /* DESCRIPTOR */
  {
    "ch1",
    COM_TYPE_POW
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    CH1TaskExecuteStepState1,
    NULL,
    CH1TaskExecuteStepDatalog,
  },
  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *CH1TaskGetPowSensorIF(CH1Task *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *CH1TaskAlloc(const void *pADCConfig)
{
  CH1Task *p_new_obj = SysAlloc(sizeof(CH1Task));

  if (p_new_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_new_obj->super);

    p_new_obj->super.vptr = &sTheClass.vtbl;
    p_new_obj->sensor_if.vptr = &sTheClass.sensor_if_vtbl;
    p_new_obj->sensor_descriptor = &sTheClass.class_descriptor;

    p_new_obj->pADCConfig = (MX_ADCParams_t *) pADCConfig;

    strcpy(p_new_obj->sensor_status.p_name, sTheClass.class_descriptor.p_name);
  }
  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *CH1TaskAllocSetName(const void *pADCConfig, const char *p_name)
{
  CH1Task *p_new_obj = (CH1Task *)CH1TaskAlloc(pADCConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *CH1TaskStaticAlloc(void *p_mem_block, const void *pADCConfig)
{
  CH1Task *p_obj = (CH1Task *)p_mem_block;

  if (p_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_obj->super);
    p_obj->super.vptr = &sTheClass.vtbl;

    p_obj->super.vptr = &sTheClass.vtbl;
    p_obj->sensor_if.vptr = &sTheClass.sensor_if_vtbl;
    p_obj->sensor_descriptor = &sTheClass.class_descriptor;

    p_obj->pADCConfig = (MX_ADCParams_t *) pADCConfig;
  }

  return (AManagedTaskEx *)p_obj;
}

AManagedTaskEx *CH1TaskStaticAllocSetName(void *p_mem_block, const void *pADCConfig, const char *p_name)
{
  CH1Task *p_obj = (CH1Task *)CH1TaskStaticAlloc(p_mem_block, pADCConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *CH1TaskGetSensorIF(CH1Task *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *CH1TaskGetEventSrcIF(CH1Task *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t CH1Task_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  CH1Task *p_obj = (CH1Task *) _this;

  if (p_obj->pADCConfig != NULL)
  {
    MX_ADCParams_t *p_adc_params = (MX_ADCParams_t *)p_obj->pADCConfig;
    p_adc_params->p_mx_dma_init_f();
    p_adc_params->p_mx_init_f();

    /* Start the calibration */
    if (HAL_ADCEx_Calibration_Start(p_adc_params->p_adc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("AnalogMicDriver - ADC init failed.\r\n"));
      res = SYS_BASE_LOW_LEVEL_ERROR_CODE;
    }

    if (!MTMap_IsInitialized(&sTheClass.task_map))
    {
      (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, CH1_TASK_CFG_MAX_INSTANCES_COUNT);
    }

    /* Add the managed task to the map.*/
    /* Use the PIN as unique key for the map. */
    MTMapElement_t *p_element = NULL;
//    uint32_t key = (uint32_t) p_obj->pIRQConfig->pin;
    uint32_t key = (uint32_t)(p_adc_params->p_adc);
    p_element = MTMap_AddElement(&sTheClass.task_map, key, _this);
    if (p_element == NULL)
    {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      return res;
    }
  }
  return res;
}

sys_error_code_t CH1Task_vtblOnCreateTask(
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
  CH1Task *p_obj = (CH1Task *) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t)CH1_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(CH1_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "CH1_Q", item_size / 4u, p_queue_items_buff,
                                    CH1_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
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
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, CH1_TASK_CFG_MAX_INSTANCES_COUNT);
  }

  p_obj->id = 0;
  p_obj->prev_timestamp = 0.0f;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "CH1";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = CH1_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = CH1_TASK_CFG_PRIORITY;
  *pPreemptThreshold = CH1_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = CH1TaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = CH1TaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("CH1: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t CH1Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                              const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  CH1Task *p_obj = (CH1Task *) _this;
  MX_ADCParams_t *p_adc_params = (MX_ADCParams_t *)p_obj->pADCConfig;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (CH1TaskSensorIsActive(p_obj))
    {
      /* Start the ADC and enable the DMA */
      p_adc_params->p_mx_dma_init_f();
      p_adc_params->p_mx_init_f();

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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("CH1: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      /* Anticipated in OnEnter */
      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
    }
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("CH1: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("CH1: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t CH1Task_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  // CH1Task *p_obj = (CH1Task*)_this;

  return res;
}

sys_error_code_t CH1Task_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("CH1: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (CH1_TASK_CFG_TAG)
  CH1Task *p_obj = (CH1Task *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = CH1_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t CH1Task_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  CH1Task *p_obj = (CH1Task *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = CH1TaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t CH1Task_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                              const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  CH1Task *p_obj = (CH1Task *) _this;
  MX_ADCParams_t *p_adc = (MX_ADCParams_t *)p_obj->pADCConfig;

  if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      if (p_obj->sensor_status.is_active)
      {
        HAL_ADC_Stop_DMA(p_adc->p_adc);
        HAL_NVIC_DisableIRQ(DMA1_Stream0_IRQn);
      }
    }
  }

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t CH1Task_vtblGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  CH1Task *p_if_owner = (CH1Task *)((uint32_t) _this - offsetof(CH1Task, sensor_if));
  uint8_t res = p_if_owner->id;

  return res;
}

IEventSrc *CH1Task_vtblGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  CH1Task *p_if_owner = (CH1Task *)((uint32_t) _this - offsetof(CH1Task, sensor_if));
  return p_if_owner->p_event_src;
}

sys_error_code_t CH1Task_vtblGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  CH1Task *p_if_owner = (CH1Task *)((uint32_t) _this - offsetof(CH1Task, sensor_if));
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

float_t CH1Task_vtblGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  CH1Task *p_if_owner = (CH1Task *)((uint32_t) _this - offsetof(CH1Task, sensor_if));
  float_t res = p_if_owner->sensor_status.type.mems.fs;

  return res;
}

float_t CH1Task_vtblGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  CH1Task *p_if_owner = (CH1Task *)((uint32_t) _this - offsetof(CH1Task, sensor_if));
  float_t res = p_if_owner->sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t CH1Task_vtblGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  CH1Task *p_if_owner = (CH1Task *)((uint32_t)_this - offsetof(CH1Task, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t CH1Task_vtblSensorSetODR(ISensorMems_t *_this, float_t odr)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  CH1Task *p_if_owner = (CH1Task *)((uint32_t) _this - offsetof(CH1Task, sensor_if));
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
    res = CH1TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t CH1Task_vtblSensorSetFS(ISensorMems_t *_this, float_t fs)
{
  assert_param(_this != NULL);
  /* Does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
  SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("CH1: warning - SetFS() not supported.\r\n"));
  return SYS_INVALID_FUNC_CALL_ERROR_CODE;
}

sys_error_code_t CH1Task_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  /* Does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
  SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("CH1: warning - SetFifoWM() not supported.\r\n"));
  return SYS_INVALID_FUNC_CALL_ERROR_CODE;
}

sys_error_code_t CH1Task_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  CH1Task *p_if_owner = (CH1Task *)((uint32_t) _this - offsetof(CH1Task, sensor_if));
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
    res = CH1TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t CH1Task_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  CH1Task *p_if_owner = (CH1Task *)((uint32_t) _this - offsetof(CH1Task, sensor_if));
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
    res = CH1TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t CH1Task_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  CH1Task *p_if_owner = (CH1Task *)((uint32_t) _this - offsetof(CH1Task, sensor_if));

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

SensorDescriptor_t CH1Task_vtblSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  CH1Task *p_if_owner = (CH1Task *)((uint32_t) _this - offsetof(CH1Task, sensor_if));
  return *p_if_owner->sensor_descriptor;

}

SensorStatus_t CH1Task_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  CH1Task *p_if_owner = (CH1Task *)((uint32_t) _this - offsetof(CH1Task, sensor_if));

  return p_if_owner->sensor_status;
}

SensorStatus_t *CH1Task_vtblSensorGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  CH1Task *p_if_owner = (CH1Task *)((uint32_t) _this - offsetof(CH1Task, sensor_if));

  return &p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t CH1TaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  CH1Task *p_obj = (CH1Task *) _this;
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
            res = CH1TaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = CH1TaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = CH1TaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("CH1: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("CH1: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t CH1TaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  CH1Task *p_obj = (CH1Task *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("CH1: new data.\r\n"));

        // notify the listeners...
        uint8_t half = report.sensorDataReadyMessage.half;
        double_t timestamp = report.sensorDataReadyMessage.fTimestamp;
        double_t delta_timestamp = timestamp - p_obj->prev_timestamp;
        p_obj->prev_timestamp = timestamp;
        /* update measuredODR */
        p_obj->sensor_status.type.mems.measured_odr = (float_t)(ADC_CONVERTED_DATA_BUFFER_SIZE / 2) / (float_t) delta_timestamp;

        for (uint32_t i = 0; i < ADC_CONVERTED_DATA_BUFFER_SIZE / 2; i++)
        {
#if (HSD_USE_DUMMY_DATA == 1)
          p_obj->sensor_data[(half - 1) * (ADC_CONVERTED_DATA_BUFFER_SIZE / 2) + i] = dummyDataCounter++; // remove the offset
#else
          p_obj->sensor_data[(half - 1) * (ADC_CONVERTED_DATA_BUFFER_SIZE / 2) + i] = (int16_t)(p_obj->aADCxConvertedData[(half - 1) * (ADC_CONVERTED_DATA_BUFFER_SIZE / 2) + i] - 32767); // remove the offset
#endif /* HSD_USE_DUMMY_DATA */
        }
        EMD_1dInit(&p_obj->data, (uint8_t *)&p_obj->sensor_data[(half - 1) * (ADC_CONVERTED_DATA_BUFFER_SIZE / 2)], E_EM_INT16,
                   ADC_CONVERTED_DATA_BUFFER_SIZE / 2);

        DataEvent_t evt;
        DataEventInit((IEvent *)&evt, p_obj->p_event_src, &p_obj->data, timestamp, p_obj->id);
        IEventSrcSendEvent(p_obj->p_event_src, (IEvent *) &evt, NULL);

        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("CH1: ts = %f\r\n", (float_t)timestamp));
        break;
      }

      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
            res = CH1TaskSensorInit(p_obj);
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = CH1TaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = CH1TaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = CH1TaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("CH1: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("CH1: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t CH1TaskPostReportToFront(CH1Task *_this, SMMessage *pReport)
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

static inline sys_error_code_t CH1TaskPostReportToBack(CH1Task *_this, SMMessage *pReport)
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

static sys_error_code_t CH1TaskSensorInit(CH1Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  MX_ADCParams_t *p_adc = (MX_ADCParams_t *)_this->pADCConfig;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /* Setup ADC in base of ODR */
  if (_this->sensor_status.is_active)
  {
    HAL_RCCEx_GetPeriphCLKConfig(&PeriphClkInitStruct);
    switch ((uint32_t)_this->sensor_status.type.mems.odr)
    {
      case 100000:
        PeriphClkInitStruct.PLL2.PLL2N = 200;
        PeriphClkInitStruct.PLL2.PLL2P = 40;
        break;
      case 150000:
        PeriphClkInitStruct.PLL2.PLL2N = 150;
        PeriphClkInitStruct.PLL2.PLL2P = 20;
        break;
      case 200000:
        PeriphClkInitStruct.PLL2.PLL2N = 200;
        PeriphClkInitStruct.PLL2.PLL2P = 20;
        break;
      case 250000:
        PeriphClkInitStruct.PLL2.PLL2N = 250;
        PeriphClkInitStruct.PLL2.PLL2P = 20;
        break;
      case 300000:
        PeriphClkInitStruct.PLL2.PLL2N = 150;
        PeriphClkInitStruct.PLL2.PLL2P = 10;
        break;
      case 350000:
        PeriphClkInitStruct.PLL2.PLL2N = 175;
        PeriphClkInitStruct.PLL2.PLL2P = 10;
        break;
      case 400000:
        PeriphClkInitStruct.PLL2.PLL2N = 200;
        PeriphClkInitStruct.PLL2.PLL2P = 10;
        break;
      case 450000:
        PeriphClkInitStruct.PLL2.PLL2N = 225;
        PeriphClkInitStruct.PLL2.PLL2P = 10;
        break;
      case 500000:
        PeriphClkInitStruct.PLL2.PLL2N = 250;
        PeriphClkInitStruct.PLL2.PLL2P = 10;
        break;
      default:
        PeriphClkInitStruct.PLL2.PLL2N = 250;
        PeriphClkInitStruct.PLL2.PLL2P = 20;
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
        break;
    }
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      res = SYS_BASE_LOW_LEVEL_ERROR_CODE;
    }
  }
  else
  {
    _this->sensor_status.is_active = false;
  }

  if (_this->sensor_status.is_active)
  {
    res = HAL_ADC_Start_DMA(p_adc->p_adc, (uint32_t *)_this->aADCxConvertedData, ADC_CONVERTED_DATA_BUFFER_SIZE);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  }

  return res;
}

static sys_error_code_t CH1TaskSensorRegister(CH1Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *pow_if = (ISensor_t *) CH1TaskGetPowSensorIF(_this);
  _this->id = SMAddSensor(pow_if);

  return res;
}

static sys_error_code_t CH1TaskSensorInitTaskParams(CH1Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* SENSOR STATUS */
  _this->sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->sensor_status.is_active = TRUE;
  _this->sensor_status.type.mems.fs = 1.0f;
  _this->sensor_status.type.mems.sensitivity = 1.0f;
  _this->sensor_status.type.mems.odr = 250000.0f;
  _this->sensor_status.type.mems.measured_odr = 0.0f;
  EMD_1dInit(&_this->data, (uint8_t *)&_this->aADCxConvertedData, E_EM_INT16, ADC_CONVERTED_DATA_BUFFER_SIZE / 2);

  return res;
}

static sys_error_code_t CH1TaskSensorSetODR(CH1Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  float_t odr = (float_t) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (odr < 100001.0f)
    {
      odr = 100000.0f;
    }
    else if (odr < 150001.0f)
    {
      odr = 150000.0f;
    }
    else if (odr < 200001.0f)
    {
      odr = 200000.0f;
    }
    else if (odr < 250001.0f)
    {
      odr = 250000.0f;
    }
    else if (odr < 300001.0f)
    {
      odr = 300000.0f;
    }
    else if (odr < 350001.0f)
    {
      odr = 350000.0f;
    }
    else if (odr < 400001.0f)
    {
      odr = 400000.0f;
    }
    else if (odr < 450001.0f)
    {
      odr = 450000.0f;
    }
    else if (odr < 500001.0f)
    {
      odr = 500000.0f;
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
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

static sys_error_code_t CH1TaskSensorEnable(CH1Task *_this, SMMessage report)
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

static sys_error_code_t CH1TaskSensorDisable(CH1Task *_this, SMMessage report)
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

static boolean_t CH1TaskSensorIsActive(const CH1Task *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.is_active;
}

uint32_t cplt_callback_count = 0;
uint32_t half_cplt_callback_count = 0;
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  cplt_callback_count++;
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.half = 2;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) hadc);
  if (p_val != NULL)
  {
    p_queue = &((CH1Task *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
  half_cplt_callback_count++;
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.half = 1;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) hadc);
  if (p_val != NULL)
  {
    p_queue = &((CH1Task *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}
