/**
  ******************************************************************************
  * @file    IIS2DULPXTask.c
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
#include "IIS2DULPXTask.h"
#include "IIS2DULPXTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "iis2dulpx_reg.h"
#include <string.h>
#include "services/sysdebug.h"

/* Private includes ----------------------------------------------------------*/

#ifndef IIS2DULPX_TASK_CFG_STACK_DEPTH
#define IIS2DULPX_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*8)
#endif

#ifndef IIS2DULPX_TASK_CFG_PRIORITY
#define IIS2DULPX_TASK_CFG_PRIORITY                 (4)
#endif

#ifndef IIS2DULPX_TASK_CFG_IN_QUEUE_LENGTH
#define IIS2DULPX_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define IIS2DULPX_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef IIS2DULPX_TASK_CFG_TIMER_PERIOD_MS
#define IIS2DULPX_TASK_CFG_TIMER_PERIOD_MS          500
#endif
#ifndef IIS2DULPX_TASK_CFG_MLC_TIMER_PERIOD_MS
#define IIS2DULPX_TASK_CFG_MLC_TIMER_PERIOD_MS      500
#endif

#ifndef IIS2DULPX_TASK_CFG_MAX_INSTANCES_COUNT
#define IIS2DULPX_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_IIS2DULPX, level, message)

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter = 0;
#endif


/**
  * Class object declaration
  */
typedef struct _IIS2DULPXTaskClass
{
  /**
    * IIS2DULPXTask class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * Accelerometer IF virtual table.
    */
  const ISensorMems_vtbl sensor_if_vtbl;

  /**
    * mlc IF virtual table.
    */
  const ISensorMems_vtbl mlc_sensor_if_vtbl;

  /**
    * SensorLL IF virtual table.
    */
  const ISensorLL_vtbl sensor_ll_if_vtbl;

  /**
    * Specifies accelerometer sensor capabilities.
    */
  const SensorDescriptor_t class_descriptor;

  /**
    * Specifies mlc sensor capabilities.
    */
  const SensorDescriptor_t mlc_class_descriptor;

  /**
    * IIS2DULPXTask (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[2 * IIS2DULPX_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} IIS2DULPXTaskClass_t;


/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS2DULPXTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS2DULPXTaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS2DULPXTaskSensorInit(IIS2DULPXTask *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS2DULPXTaskSensorReadData(IIS2DULPXTask *_this);

/**
  * Read the data from the mlc.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS2DULPXTaskSensorReadMLC(IIS2DULPXTask *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t IIS2DULPXTaskSensorRegister(IIS2DULPXTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t IIS2DULPXTaskSensorInitTaskParams(IIS2DULPXTask *_this);

/**
  * Private implementation of sensor interface methods for IIS2DULPX sensor
  */
static sys_error_code_t IIS2DULPXTaskSensorSetODR(IIS2DULPXTask *_this, SMMessage report);
static sys_error_code_t IIS2DULPXTaskSensorSetFS(IIS2DULPXTask *_this, SMMessage report);
static sys_error_code_t IIS2DULPXTaskSensorSetFifoWM(IIS2DULPXTask *_this, SMMessage report);
static sys_error_code_t IIS2DULPXTaskSensorEnable(IIS2DULPXTask *_this, SMMessage report);
static sys_error_code_t IIS2DULPXTaskSensorDisable(IIS2DULPXTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t IIS2DULPXTaskSensorIsActive(const IIS2DULPXTask *_this);

static sys_error_code_t IIS2DULPXTaskEnterLowPowerMode(const IIS2DULPXTask *_this);

static sys_error_code_t IIS2DULPXTaskConfigureIrqPin(const IIS2DULPXTask *_this, boolean_t LowPower);
static sys_error_code_t IIS2DULPXTaskConfigureMLCPin(const IIS2DULPXTask *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void IIS2DULPXTaskTimerCallbackFunction(ULONG param);

/**
  * Callback function called when the mlc software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void IIS2DULPXTaskMLCTimerCallbackFunction(ULONG param);

/**
  * Given a interface pointer it return the instance of the object that implement the interface.
  *
  * @param p_if [IN] specifies a sensor interface implemented by the task object.
  * @return the instance of the task object that implements the given interface.
  */
static inline IIS2DULPXTask *IIS2DULPXTaskGetOwnerFromISensorIF(ISensor_t *p_if);
static inline IIS2DULPXTask *IIS2DULPXTaskGetOwnerFromISensorLLIF(ISensorLL_t *p_if);

/**
  * Read the odr value from the sensor and update the internal model
  */
static sys_error_code_t IIS2DULPX_ODR_Sync(IIS2DULPXTask *_this);

/**
  * Read the odr value from the sensor and update the internal model
  */
static sys_error_code_t IIS2DULPX_FS_Sync(IIS2DULPXTask *_this);

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
static inline sys_error_code_t IIS2DULPXTaskPostReportToFront(IIS2DULPXTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t IIS2DULPXTaskPostReportToBack(IIS2DULPXTask *_this, SMMessage *pReport);


/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static IIS2DULPXTask sTaskObj;

/**
  * The class object.
  */
static IIS2DULPXTaskClass_t sTheClass =
{
  /* class virtual table */
  {
    IIS2DULPXTask_vtblHardwareInit,
    IIS2DULPXTask_vtblOnCreateTask,
    IIS2DULPXTask_vtblDoEnterPowerMode,
    IIS2DULPXTask_vtblHandleError,
    IIS2DULPXTask_vtblOnEnterTaskControlLoop,
    IIS2DULPXTask_vtblForceExecuteStep,
    IIS2DULPXTask_vtblOnEnterPowerMode
  },
  /* class::sensor_if_vtbl virtual table */
  {
    {
      {
        IIS2DULPXTask_vtblAccGetId,
        IIS2DULPXTask_vtblGetEventSourceIF,
        IIS2DULPXTask_vtblAccGetDataInfo
      },
      IIS2DULPXTask_vtblSensorEnable,
      IIS2DULPXTask_vtblSensorDisable,
      IIS2DULPXTask_vtblSensorIsEnabled,
      IIS2DULPXTask_vtblSensorGetDescription,
      IIS2DULPXTask_vtblSensorGetStatus,
      IIS2DULPXTask_vtblSensorGetStatusPointer
    },
    IIS2DULPXTask_vtblAccGetODR,
    IIS2DULPXTask_vtblAccGetFS,
    IIS2DULPXTask_vtblAccGetSensitivity,
    IIS2DULPXTask_vtblSensorSetODR,
    IIS2DULPXTask_vtblSensorSetFS,
    IIS2DULPXTask_vtblSensorSetFifoWM
  },

  /* class::mlc_sensor_if_vtbl virtual table */
  {
    {
      {
        IIS2DULPXTask_vtblMlcGetId,
        IIS2DULPXTask_vtblMlcGetEventSourceIF,
        IIS2DULPXTask_vtblMlcGetDataInfo
      },
      IIS2DULPXTask_vtblSensorEnable,
      IIS2DULPXTask_vtblSensorDisable,
      IIS2DULPXTask_vtblSensorIsEnabled,
      IIS2DULPXTask_vtblMlcGetDescription,
      IIS2DULPXTask_vtblMlcGetStatus,
      IIS2DULPXTask_vtblMlcGetStatusPointer
    },
    IIS2DULPXTask_vtblMlcGetODR,
    IIS2DULPXTask_vtblMlcGetFS,
    IIS2DULPXTask_vtblMlcGetSensitivity,
    IIS2DULPXTask_vtblSensorSetODR,
    IIS2DULPXTask_vtblSensorSetFS,
    IIS2DULPXTask_vtblSensorSetFifoWM
  },
  /* class::sensor_ll_if_vtbl virtual table */
  {
    IIS2DULPXTask_vtblSensorReadReg,
    IIS2DULPXTask_vtblSensorWriteReg,
    IIS2DULPXTask_vtblSensorSyncModel
  },
  /* ACCELEROMETER DESCRIPTOR */
  {
    "iis2dulpx",
    COM_TYPE_ACC
  },

  /* MLC DESCRIPTOR */
  {
    "iis2dulpx",
    COM_TYPE_MLC
  },
  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    IIS2DULPXTaskExecuteStepState1,
    NULL,
    IIS2DULPXTaskExecuteStepDatalog,
  },
  {{0}}, /* task_map_elements */
  {0} /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *IIS2DULPXTaskGetAccSensorIF(IIS2DULPXTask *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

ISourceObservable *IIS2DULPXTaskGetMlcSensorIF(IIS2DULPXTask *_this)
{
  return (ISourceObservable *) & (_this->mlc_sensor_if);
}

ISensorLL_t *IIS2DULPXTaskGetSensorLLIF(IIS2DULPXTask *_this)
{
  return (ISensorLL_t *) & (_this->sensor_ll_if);
}

AManagedTaskEx *IIS2DULPXTaskAlloc(const void *pIRQConfig, const void *pMLCConfig, const void *pCSConfig)
{
  IIS2DULPXTask *p_new_obj = SysAlloc(sizeof(IIS2DULPXTask));

  if (p_new_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_new_obj->super);

    p_new_obj->super.vptr = &sTheClass.vtbl;
    p_new_obj->sensor_if.vptr = &sTheClass.sensor_if_vtbl;
    p_new_obj->mlc_sensor_if.vptr = &sTheClass.mlc_sensor_if_vtbl;
    p_new_obj->sensor_ll_if.vptr = &sTheClass.sensor_ll_if_vtbl;
    p_new_obj->sensor_descriptor = &sTheClass.class_descriptor;
    p_new_obj->mlc_sensor_descriptor = &sTheClass.mlc_class_descriptor;

    p_new_obj->pIRQConfig = (MX_GPIOParams_t *) pIRQConfig;
    p_new_obj->pMLCConfig = (MX_GPIOParams_t *) pMLCConfig;
    p_new_obj->pCSConfig = (MX_GPIOParams_t *) pCSConfig;

    strcpy(p_new_obj->sensor_status.p_name, sTheClass.class_descriptor.p_name);
    strcpy(p_new_obj->mlc_sensor_status.p_name, sTheClass.mlc_class_descriptor.p_name);
  }

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *IIS2DULPXTaskAllocSetName(const void *pIRQConfig, const void *pMLCConfig, const void *pCSConfig,
                                          const char *p_name)
{
  IIS2DULPXTask *p_new_obj = (IIS2DULPXTask *) IIS2DULPXTaskAlloc(pIRQConfig, pMLCConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->sensor_status.p_name, p_name);
  strcpy(p_new_obj->mlc_sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *IIS2DULPXTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pMLCConfig,
                                         const void *pCSConfig)
{
  IIS2DULPXTask *p_obj = (IIS2DULPXTask *)p_mem_block;

  if (p_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_obj->super);

    p_obj->super.vptr = &sTheClass.vtbl;
    p_obj->sensor_if.vptr = &sTheClass.sensor_if_vtbl;
    p_obj->mlc_sensor_if.vptr = &sTheClass.mlc_sensor_if_vtbl;
    p_obj->sensor_ll_if.vptr = &sTheClass.sensor_ll_if_vtbl;
    p_obj->sensor_descriptor = &sTheClass.class_descriptor;
    p_obj->mlc_sensor_descriptor = &sTheClass.mlc_class_descriptor;

    p_obj->pIRQConfig = (MX_GPIOParams_t *) pIRQConfig;
    p_obj->pMLCConfig = (MX_GPIOParams_t *) pMLCConfig;
    p_obj->pCSConfig = (MX_GPIOParams_t *) pCSConfig;
  }

  return (AManagedTaskEx *)p_obj;
}

AManagedTaskEx *IIS2DULPXTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pMLCConfig,
                                                const void *pCSConfig, const char *p_name)
{
  IIS2DULPXTask *p_obj = (IIS2DULPXTask *) IIS2DULPXTaskStaticAlloc(p_mem_block, pIRQConfig, pMLCConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->sensor_status.p_name, p_name);
  strcpy(p_obj->mlc_sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *IIS2DULPXTaskGetSensorIF(IIS2DULPXTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *IIS2DULPXTaskGetEventSrcIF(IIS2DULPXTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_event_src;
}

IEventSrc *IIS2DULPXTaskGetMlcEventSrcIF(IIS2DULPXTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_mlc_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t IIS2DULPXTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2DULPXTask *p_obj = (IIS2DULPXTask *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t IIS2DULPXTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                                VOID **pvStackStart,
                                                ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                                ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2DULPXTask *p_obj = (IIS2DULPXTask *) _this;

  p_obj->sync = true;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) IIS2DULPX_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(IIS2DULPX_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "IIS2DULPX_Q", item_size / 4u, p_queue_items_buff,
                                    IIS2DULPX_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_timer, "IIS2DULPX_T", IIS2DULPXTaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(IIS2DULPX_TASK_CFG_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* create the mlc software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->mlc_timer, "IIS2DULPX_MLC_T", IIS2DULPXTaskMLCTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(IIS2DULPX_TASK_CFG_MLC_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(IIS2DULPX_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(IIS2DULPX_ID, IIS2DULPX_I2C_ADD_H, 0);
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

  p_obj->p_mlc_event_src = DataEventSrcAlloc();
  if (p_obj->p_mlc_event_src == NULL)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }
  IEventSrcInit(p_obj->p_mlc_event_src);

  if (!MTMap_IsInitialized(&sTheClass.task_map))
  {
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, 2 * IIS2DULPX_TASK_CFG_MAX_INSTANCES_COUNT);
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

  /* Add the managed task to the map.*/
  if (p_obj->pMLCConfig != NULL)
  {
    /* Use the PIN as unique key for the map. */
    MTMapElement_t *p_element = NULL;
    uint32_t key = (uint32_t) p_obj->pMLCConfig->pin;
    p_element = MTMap_AddElement(&sTheClass.task_map, key, _this);
    if (p_element == NULL)
    {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      return res;
    }
  }

  memset(p_obj->p_sensor_data_buff, 0, sizeof(p_obj->p_sensor_data_buff));
  memset(p_obj->p_mlc_sensor_data_buff, 0, sizeof(p_obj->p_mlc_sensor_data_buff));
  p_obj->acc_id = 0;
  p_obj->mlc_enable = FALSE;
  p_obj->prev_timestamp = 0.0f;
  p_obj->fifo_level = 0;
  p_obj->samples_per_it = 0;
  p_obj->first_data_ready = 0;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "IIS2DULPX";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = IIS2DULPX_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = IIS2DULPX_TASK_CFG_PRIORITY;
  *pPreemptThreshold = IIS2DULPX_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = IIS2DULPXTaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = IIS2DULPXTaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS2DULPX: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t IIS2DULPXTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                    const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2DULPXTask *p_obj = (IIS2DULPXTask *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (IIS2DULPXTaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS2DULPX: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      if (IIS2DULPXTaskSensorIsActive(p_obj))
      {
        /* Deactivate the sensor */
        iis2dulpx_md_t mode;
        mode.odr = IIS2DULPX_OFF;
        iis2dulpx_mode_set(p_sensor_drv, &mode);

        iis2dulpx_fifo_mode_t fifo_mode;
        fifo_mode.operation = IIS2DULPX_BYPASS_MODE;
        iis2dulpx_fifo_mode_set(p_sensor_drv, fifo_mode);
      }
      p_obj->samples_per_it = 0;
      p_obj->first_data_ready = 0;
      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_timer);
      }
      else
      {
        IIS2DULPXTaskConfigureIrqPin(p_obj, TRUE);
      }
      if (p_obj->pMLCConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->mlc_timer);
      }
      else
      {
        IIS2DULPXTaskConfigureMLCPin(p_obj, TRUE);
      }
      memset(p_obj->p_mlc_sensor_data_buff, 0, sizeof(p_obj->p_mlc_sensor_data_buff));
    }
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS2DULPX: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = IIS2DULPXTaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2DULPX - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->pIRQConfig != NULL)
    {
      IIS2DULPXTaskConfigureIrqPin(p_obj, TRUE);
    }
    if (p_obj->pMLCConfig != NULL)
    {
      IIS2DULPXTaskConfigureMLCPin(p_obj, TRUE);
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
    if (p_obj->pMLCConfig == NULL)
    {
      tx_timer_deactivate(&p_obj->mlc_timer);
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS2DULPX: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t IIS2DULPXTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  IIS2DULPXTask *p_obj = (IIS2DULPXTask*)_this;

  return res;
}

sys_error_code_t IIS2DULPXTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("IIS2DULPX: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (IIS2DULPX_TASK_CFG_TAG)
  IIS2DULPXTask *p_obj = (IIS2DULPXTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = IIS2DULPX_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t IIS2DULPXTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2DULPXTask *p_obj = (IIS2DULPXTask *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = IIS2DULPXTaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t IIS2DULPXTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                    const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  IIS2DULPXTask *p_obj = (IIS2DULPXTask*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t IIS2DULPXTask_vtblAccGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  uint8_t res = p_if_owner->acc_id;

  return res;
}

uint8_t IIS2DULPXTask_vtblMlcGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  uint8_t res = p_if_owner->mlc_id;

  return res;
}

IEventSrc *IIS2DULPXTask_vtblGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  return p_if_owner->p_event_src;
}

IEventSrc *IIS2DULPXTask_vtblMlcGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  return p_if_owner->p_mlc_event_src;
}

sys_error_code_t IIS2DULPXTask_vtblAccGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
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

float_t IIS2DULPXTask_vtblAccGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  float_t res = p_if_owner->sensor_status.type.mems.fs;

  return res;
}

float_t IIS2DULPXTask_vtblAccGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  float_t res = p_if_owner->sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t IIS2DULPXTask_vtblAccGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t IIS2DULPXTask_vtblMlcGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if ((p_measured == NULL) || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = p_if_owner->mlc_sensor_status.type.mems.measured_odr;
    *p_nominal = p_if_owner->mlc_sensor_status.type.mems.odr;
  }

  return res;
}

float_t IIS2DULPXTask_vtblMlcGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);

  /* MLC does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2DULPX: warning - MLC GetFS() not supported.\r\n"));

  return -1.0f;
}

float_t IIS2DULPXTask_vtblMlcGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);

  /* MLC does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2DULPX: warning - MLC GetSensitivity() not supported.\r\n"));

  return -1.0f;
}

EMData_t IIS2DULPXTask_vtblMlcGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  EMData_t res = p_if_owner->data_mlc;

  return res;
}

sys_error_code_t IIS2DULPXTask_vtblSensorSetODR(ISensorMems_t *_this, float_t odr)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    if (odr > 1.0f)
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
      .sensorMessage.fParam = (float_t) odr
    };
    res = IIS2DULPXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t IIS2DULPXTask_vtblSensorSetFS(ISensorMems_t *_this, float_t fs)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.mems.fs = fs;
    p_if_owner->sensor_status.type.mems.sensitivity = 0.0000305f * p_if_owner->sensor_status.type.mems.fs;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FS,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.fParam = (float_t) fs
    };
    res = IIS2DULPXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t IIS2DULPXTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

#if IIS2DULPX_FIFO_ENABLED
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
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
    res = IIS2DULPXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }
#endif

  return res;
}

sys_error_code_t IIS2DULPXTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    if (sensor_id == p_if_owner->acc_id)
    {
      p_if_owner->sensor_status.is_active = TRUE;
    }
    else if (sensor_id == p_if_owner->mlc_id)
    {
      p_if_owner->mlc_sensor_status.is_active = TRUE;
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
    res = IIS2DULPXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t IIS2DULPXTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    if (sensor_id == p_if_owner->acc_id)
    {
      p_if_owner->sensor_status.is_active = FALSE;
    }
    else if (sensor_id == p_if_owner->mlc_id)
    {
      p_if_owner->mlc_sensor_status.is_active = FALSE;
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
    res = IIS2DULPXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t IIS2DULPXTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);

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

SensorDescriptor_t IIS2DULPXTask_vtblSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  return *p_if_owner->sensor_descriptor;
}

SensorDescriptor_t IIS2DULPXTask_vtblMlcGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  return *p_if_owner->mlc_sensor_descriptor;
}

SensorStatus_t IIS2DULPXTask_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);

  return p_if_owner->sensor_status;
}

SensorStatus_t IIS2DULPXTask_vtblMlcGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  return p_if_owner->mlc_sensor_status;
}

SensorStatus_t *IIS2DULPXTask_vtblSensorGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  return &p_if_owner->sensor_status;
}

SensorStatus_t *IIS2DULPXTask_vtblMlcGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
  return &p_if_owner->mlc_sensor_status;
}

sys_error_code_t IIS2DULPXTask_vtblSensorReadReg(ISensorLL_t *_this, uint16_t reg, uint8_t *data, uint16_t len)
{
  assert_param(_this != NULL);
  assert_param(reg <= 0xFFU);
  assert_param(data != NULL);
  assert_param(len != 0U);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorLLIF(_this);
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_if_owner->p_sensor_bus_if->m_xConnector;
  uint8_t reg8 = (uint8_t)(reg & 0x00FF);

  if (iis2dulpx_read_reg(p_sensor_drv, reg8, data, len) != 0)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t IIS2DULPXTask_vtblSensorWriteReg(ISensorLL_t *_this, uint16_t reg, const uint8_t *data, uint16_t len)
{
  assert_param(_this != NULL);
  assert_param(reg <= 0xFFU);
  assert_param(data != NULL);
  assert_param(len != 0U);

  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorLLIF(_this);
  uint8_t reg8 = (uint8_t)(reg & 0x00FF);

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_if_owner->p_sensor_bus_if->m_xConnector;

  /* This generic register write operation could mean that the model is out of sync with the HW */
  p_if_owner->sync = false;

  if (iis2dulpx_write_reg(p_sensor_drv, reg8, (uint8_t *)data, len) != 0)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t IIS2DULPXTask_vtblSensorSyncModel(ISensorLL_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2DULPXTask *p_if_owner = IIS2DULPXTaskGetOwnerFromISensorLLIF(_this);

  if (IIS2DULPX_ODR_Sync(p_if_owner) != SYS_NO_ERROR_CODE)
  {
    res = SYS_BASE_ERROR_CODE;
  }
  if (IIS2DULPX_FS_Sync(p_if_owner) != SYS_NO_ERROR_CODE)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  if (!SYS_IS_ERROR_CODE(res))
  {
    p_if_owner->sync = true;
  }

  return res;
}

/* Private function definition */
// ***************************
static sys_error_code_t IIS2DULPXTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2DULPXTask *p_obj = (IIS2DULPXTask *) _this;
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
            res = IIS2DULPXTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = IIS2DULPXTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = IIS2DULPXTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = IIS2DULPXTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = IIS2DULPXTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2DULPX: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2DULPX: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }

    }
  }

  return res;
}

static sys_error_code_t IIS2DULPXTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2DULPXTask *p_obj = (IIS2DULPXTask *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("IIS2DULPX: new data.\r\n"));
        res = IIS2DULPXTaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          if (p_obj->first_data_ready == 3)
          {
#if IIS2DULPX_FIFO_ENABLED
            if (p_obj->fifo_level != 0)
            {
#endif
              // notify the listeners...
              double_t timestamp = report.sensorDataReadyMessage.fTimestamp;
              double_t delta_timestamp = timestamp - p_obj->prev_timestamp;
              p_obj->prev_timestamp = timestamp;

              /* update measuredODR */
              // Update the sums
              p_obj->delta_timestamp_sum += delta_timestamp;
              p_obj->samples_sum += p_obj->samples_per_it;
              if (p_obj->odr_count < MEAS_ODR_AVG)
              {
                p_obj->odr_count++;
              }
              // Calculate the average
              if (p_obj->odr_count == MEAS_ODR_AVG)
              {
                p_obj->sensor_status.type.mems.measured_odr = (float_t) p_obj->samples_sum / p_obj->delta_timestamp_sum;
                p_obj->delta_timestamp_sum = 0.0f;
                p_obj->samples_sum = 0;
                p_obj->odr_count = 0;
              }
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
              SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("IIS2DULPX: ts = %f\r\n", (float_t)timestamp));
#if IIS2DULPX_FIFO_ENABLED
            }
#endif
          }
          else
          {
            p_obj->first_data_ready++;
          }
        }
        break;
      }

      case SM_MESSAGE_ID_DATA_READY_MLC:
      {
        res = IIS2DULPXTaskSensorReadMLC(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          // notify the listeners...
          double_t timestamp = report.sensorDataReadyMessage.fTimestamp;

          if (p_obj->mlc_enable)
          {
            EMD_Init(&p_obj->data_mlc, p_obj->p_mlc_sensor_data_buff, E_EM_UINT8, E_EM_MODE_INTERLEAVED, 2, 1, 5);

            DataEvent_t evt;

            DataEventInit((IEvent *) &evt, p_obj->p_mlc_event_src, &p_obj->data_mlc, timestamp, p_obj->mlc_id);
            IEventSrcSendEvent(p_obj->p_mlc_event_src, (IEvent *) &evt, NULL);
          }
          else
          {
            res = SYS_INVALID_PARAMETER_ERROR_CODE;
          }
        }
        break;
      }

      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
            res = IIS2DULPXTaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->sensor_status.is_active == true)
              {
                if (p_obj->pIRQConfig == NULL)
                {
                  if (TX_SUCCESS
                      != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->iis2dulpx_task_cfg_timer_period_ms),
                                         AMT_MS_TO_TICKS(p_obj->iis2dulpx_task_cfg_timer_period_ms)))
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
                  IIS2DULPXTaskConfigureIrqPin(p_obj, FALSE);
                }
              }
            }
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->mlc_sensor_status.is_active == true)
              {
                if (p_obj->pMLCConfig == NULL)
                {
                  if (TX_SUCCESS
                      != tx_timer_change(&p_obj->mlc_timer, AMT_MS_TO_TICKS(IIS2DULPX_TASK_CFG_MLC_TIMER_PERIOD_MS),
                                         AMT_MS_TO_TICKS(IIS2DULPX_TASK_CFG_MLC_TIMER_PERIOD_MS)))
                  {
                    res = SYS_UNDEFINED_ERROR_CODE;
                  }
                  if (TX_SUCCESS != tx_timer_activate(&p_obj->mlc_timer))
                  {
                    res = SYS_UNDEFINED_ERROR_CODE;
                  }
                }
                else
                {
                  IIS2DULPXTaskConfigureMLCPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = IIS2DULPXTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = IIS2DULPXTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = IIS2DULPXTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = IIS2DULPXTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = IIS2DULPXTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2DULPX: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2DULPX: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t IIS2DULPXTaskPostReportToFront(IIS2DULPXTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t IIS2DULPXTaskPostReportToBack(IIS2DULPXTask *_this, SMMessage *pReport)
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

static sys_error_code_t IIS2DULPXTaskSensorInit(IIS2DULPXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  int32_t ret_val = 0;
  uint8_t reg0;
  iis2dulpx_md_t mode;
  iis2dulpx_status_t status;
  /* FIFO INT setup */
  iis2dulpx_pin_int_route_t int1_route = {0};
  iis2dulpx_pin_int_route_t int2_route = {0};

  /*add 10ms delay?*/
  iis2dulpx_exit_deep_power_down(p_sensor_drv);
  ret_val = iis2dulpx_device_id_get(p_sensor_drv, &reg0);
  if (ret_val == 0)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, reg0);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS2DULPX: sensor - I am 0x%x.\r\n", reg0));

  /* Restore default configuration */
  iis2dulpx_init_set(p_sensor_drv, IIS2DULPX_RESET);
  do
  {
    iis2dulpx_status_get(p_sensor_drv, &status);
  } while (status.sw_reset);

#if IIS2DULPX_FIFO_ENABLED

  if (_this->samples_per_it == 0)
  {
    uint16_t iis2dulpx_wtm_level = 0;

    /* iis2dulpx_wtm_level of watermark and samples per int*/
    iis2dulpx_wtm_level = ((uint16_t) _this->sensor_status.type.mems.odr * (uint16_t) IIS2DULPX_MAX_DRDY_PERIOD);
    if (iis2dulpx_wtm_level > IIS2DULPX_MAX_WTM_LEVEL)
    {
      iis2dulpx_wtm_level = IIS2DULPX_MAX_WTM_LEVEL;
    }

    _this->samples_per_it = iis2dulpx_wtm_level;
  }

  iis2dulpx_fifo_mode_t fifo_mode;
  fifo_mode.store = IIS2DULPX_FIFO_1X;
  fifo_mode.xl_only = 1;
  fifo_mode.watermark = _this->samples_per_it;
  fifo_mode.operation = IIS2DULPX_STREAM_MODE;
  fifo_mode.batch.dec_ts = IIS2DULPX_DEC_TS_OFF;
  fifo_mode.batch.bdr_xl = IIS2DULPX_BDR_XL_ODR;
  iis2dulpx_fifo_mode_set(p_sensor_drv, fifo_mode);

  /* FIFO_WTM_IA routing on pin INT1 */
  iis2dulpx_pin_int1_route_get(p_sensor_drv, &int1_route);
  *(uint8_t *) &(int1_route) = 0;

  if (_this->pIRQConfig != NULL)
  {
    int1_route.fifo_th = PROPERTY_ENABLE;
  }
  else
  {
    int1_route.fifo_th = PROPERTY_DISABLE;
  }

#else
  _this->samples_per_it = 1;
  if (_this->pIRQConfig != NULL)
  {
    int1_route.drdy = PROPERTY_ENABLE;
  }
  else
  {
    int1_route.drdy = PROPERTY_DISABLE;
  }
#endif /* IIS2DULPX_FIFO_ENABLED */
  iis2dulpx_pin_int1_route_set(p_sensor_drv, &int1_route);

  /* Setup mlc */
  if (_this->mlc_enable)
  {
    iis2dulpx_pin_int1_route_get(p_sensor_drv, &int1_route);
    iis2dulpx_pin_int2_route_get(p_sensor_drv, &int2_route);

    if (/*int1_route.mlc_int1.int1_mlc1 == 1 || */int1_route.emb_function == 1)
    {
      /*int1_route.mlc_int1.int1_mlc1 = 0;*/ /* Missing functions from PID to setup MLC INT */
      int1_route.emb_function = 0;
      iis2dulpx_pin_int1_route_set(p_sensor_drv, &int1_route);
    }

    if (/*int2_route.mlc_int2.int2_mlc1 == 0 || */int2_route.emb_function == 0)
    {
      /*int2_route.mlc_int2.int2_mlc1 = 1;*/ /* Missing functions from PID to setup MLC INT */
      int2_route.emb_function = 1;
      iis2dulpx_pin_int2_route_set(p_sensor_drv, &int2_route);
    }

    SMMessage report;
    report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY_MLC;
    report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

    // if (sTaskObj.in_queue != NULL ) {//TODO: STF.Port - how to check if the queue has been initialized ??
    if (TX_SUCCESS != tx_queue_send(&_this->in_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }

  if (_this->sensor_status.is_active)
  {
#if IIS2DULPX_FIFO_ENABLED
    _this->iis2dulpx_task_cfg_timer_period_ms = (uint16_t)((1000.0f / _this->sensor_status.type.mems.odr) * (((float_t)(_this->samples_per_it)) / 2.0f));
#else
    _this->iis2dulpx_task_cfg_timer_period_ms = (uint16_t)(1000.0f / _this->sensor_status.type.mems.odr);
#endif
  }

  iis2dulpx_mode_get(p_sensor_drv, &mode);
  /* Full scale selection. */
  if (_this->sensor_status.type.mems.fs < 3.0f)
  {
    mode.fs = IIS2DULPX_2g;
  }
  else if (_this->sensor_status.type.mems.fs < 5.0f)
  {
    mode.fs = IIS2DULPX_4g;
  }
  else if (_this->sensor_status.type.mems.fs < 9.0f)
  {
    mode.fs = IIS2DULPX_8g;
  }
  else
  {
    mode.fs = IIS2DULPX_16g;
  }

  if (_this->sensor_status.type.mems.odr < 7.0f)
  {
    mode.odr = IIS2DULPX_6Hz_HP;
  }
  else if (_this->sensor_status.type.mems.odr < 13.0f)
  {
    mode.odr = IIS2DULPX_12Hz5_HP;
  }
  else if (_this->sensor_status.type.mems.odr < 26.0f)
  {
    mode.odr = IIS2DULPX_25Hz_HP;
  }
  else if (_this->sensor_status.type.mems.odr < 51.0f)
  {
    mode.odr = IIS2DULPX_50Hz_HP;
  }
  else if (_this->sensor_status.type.mems.odr < 101.0f)
  {
    mode.odr = IIS2DULPX_100Hz_HP;
  }
  else if (_this->sensor_status.type.mems.odr < 201.0f)
  {
    mode.odr = IIS2DULPX_200Hz_HP;
  }
  else if (_this->sensor_status.type.mems.odr < 401.0f)
  {
    mode.odr = IIS2DULPX_400Hz_HP;
  }
  else
  {
    mode.odr = IIS2DULPX_800Hz_HP;
  }

  if (_this->sensor_status.is_active)
  {
    iis2dulpx_mode_set(p_sensor_drv, &mode);
  }
  else
  {
    mode.odr = IIS2DULPX_OFF;
    iis2dulpx_mode_set(p_sensor_drv, &mode);
    _this->sensor_status.is_active = false;
  }
  _this->odr_count = 0;
  _this->delta_timestamp_sum = 0.0f;
  _this->samples_sum = 0;
  return res;
}

static sys_error_code_t IIS2DULPXTaskSensorReadData(IIS2DULPXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t samples_per_it = _this->samples_per_it;

#if IIS2DULPX_FIFO_ENABLED
  iis2dulpx_fifo_data_level_get(p_sensor_drv, &_this->fifo_level);

  if (_this->fifo_level >= samples_per_it)
  {
    res = iis2dulpx_read_reg(p_sensor_drv, IIS2DULPX_FIFO_DATA_OUT_TAG, (uint8_t *) _this->p_sensor_data_buff,
                             ((uint16_t) samples_per_it * 7u));

    if (!SYS_IS_ERROR_CODE(res))
    {
      int16_t *p16_src = (int16_t *) _this->p_sensor_data_buff;
      int16_t *p16_dest = (int16_t *) _this->p_sensor_data_buff;
      uint16_t i;
      for (i = 0; i < samples_per_it; i++)
      {
        p16_src = (int16_t *) & ((uint8_t *)(p16_src))[1];
        *p16_dest++ = *p16_src++;
        *p16_dest++ = *p16_src++;
        *p16_dest++ = *p16_src++;
      }
    }
  }
  else
  {
    _this->fifo_level = 0;
  }

#else
  res = iis2dulpx_read_reg(p_sensor_drv, IIS2DULPX_OUT_X_L, (uint8_t *) _this->p_sensor_data_buff, samples_per_it * 6);
  _this->fifo_level = 1;
#endif /* IIS2DULPX_FIFO_ENABLED */

  if (!SYS_IS_ERROR_CODE(res))
  {
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
  }

  return res;
}

static sys_error_code_t IIS2DULPXTaskSensorReadMLC(IIS2DULPXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  iis2dulpx_mlc_status_mainpage_t mlc_status;

  if (_this->mlc_enable)
  {
    /* Read MLC status */
    iis2dulpx_mlc_status_get(p_sensor_drv, &mlc_status);
    _this->p_mlc_sensor_data_buff[4] = (mlc_status.is_mlc1) | (mlc_status.is_mlc2 << 1) | (mlc_status.is_mlc3 << 2) | (mlc_status.is_mlc4 << 3);
    /* Read MLC values */
    iis2dulpx_mlc_out_get(p_sensor_drv, (uint8_t *)(&_this->p_mlc_sensor_data_buff[0]));
  }

  return res;
}

static sys_error_code_t IIS2DULPXTaskSensorRegister(IIS2DULPXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

#if !IIS2DULPX_ACC_DISABLED
  ISensor_t *acc_if = (ISensor_t *) IIS2DULPXTaskGetAccSensorIF(_this);
  _this->acc_id = SMAddSensor(acc_if);
#endif
#if !IIS2DULPX_MLC_DISABLED
  ISensor_t *mlc_if = (ISensor_t *) IIS2DULPXTaskGetMlcSensorIF(_this);
  _this->mlc_id = SMAddSensor(mlc_if);
#endif

  return res;
}

static sys_error_code_t IIS2DULPXTaskSensorInitTaskParams(IIS2DULPXTask *_this)
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

  /* MLC STATUS */
  _this->mlc_sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->mlc_sensor_status.is_active = FALSE;
  _this->mlc_sensor_status.type.mems.fs = 1.0f;
  _this->mlc_sensor_status.type.mems.sensitivity = 1.0f;
  _this->mlc_sensor_status.type.mems.odr = 1.0f;
  _this->mlc_sensor_status.type.mems.measured_odr = 0.0f;
  EMD_Init(&_this->data_mlc, _this->p_mlc_sensor_data_buff, E_EM_UINT8, E_EM_MODE_INTERLEAVED, 2, 1, 5);

  return res;
}

static sys_error_code_t IIS2DULPXTaskSensorSetODR(IIS2DULPXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float_t odr = (float_t) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    if (odr < 1.0f)
    {
      iis2dulpx_md_t mode;
      mode.odr = IIS2DULPX_OFF;
      iis2dulpx_mode_set(p_sensor_drv, &mode);
      /* Do not update the model in case of odr = 0 */
      odr = _this->sensor_status.type.mems.odr;
    }
    else
    {
      /* Changing odr must disable MLC sensor: MLC can work properly only when setup from UCF */
      _this->mlc_enable = FALSE;
      _this->mlc_sensor_status.is_active = FALSE;
      if (odr < 7.0f)
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

    }
    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->sensor_status.type.mems.odr = odr;
      _this->sensor_status.type.mems.measured_odr = 0.0f;
    }
  }
  else if (id == _this->mlc_id)
  {
    res = SYS_TASK_INVALID_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2DULPX: warning - MLC SetODR() not supported.\r\n"));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  /* when odr changes the samples_per_it must be recalculated */
  _this->samples_per_it = 0;

  return res;
}

static sys_error_code_t IIS2DULPXTaskSensorSetFS(IIS2DULPXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  float_t fs = (float_t) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  /* Changing fs must disable MLC sensor: MLC can work properly only when setup from UCF */
  _this->mlc_enable = FALSE;
  _this->mlc_sensor_status.is_active = FALSE;

  if (id == _this->acc_id)
  {
    if (fs < 3.0f)
    {
      fs = 2.0f;
    }
    else if (fs < 5.0f)
    {
      fs = 4.0f;
    }
    else if (fs < 9.0f)
    {
      fs = 8.0f;
    }
    else
    {
      fs = 16.0f;
    }

    _this->sensor_status.type.mems.fs = fs;
    _this->sensor_status.type.mems.sensitivity = 0.0000305f * _this->sensor_status.type.mems.fs;
  }
  else if (id == _this->mlc_id)
  {
    res = SYS_TASK_INVALID_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2DULPX: warning - MLC SetFS() not supported.\r\n"));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t IIS2DULPXTaskSensorSetFifoWM(IIS2DULPXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t iis2dulpx_wtm_level = (uint16_t)report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    if (iis2dulpx_wtm_level > IIS2DULPX_MAX_WTM_LEVEL)
    {
      iis2dulpx_wtm_level = IIS2DULPX_MAX_WTM_LEVEL;
    }
    _this->samples_per_it = iis2dulpx_wtm_level;

    iis2dulpx_fifo_mode_t fifo_mode;
    iis2dulpx_fifo_mode_get(p_sensor_drv, &fifo_mode);
    /* Set FIFO watermark */
    fifo_mode.watermark = _this->samples_per_it;
    iis2dulpx_fifo_mode_set(p_sensor_drv, fifo_mode);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t IIS2DULPXTaskSensorEnable(IIS2DULPXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    _this->sensor_status.is_active = TRUE;

    /* Changing sensor configuration must disable MLC sensor: MLC can work properly only when setup from UCF */
    _this->mlc_enable = FALSE;
    _this->mlc_sensor_status.is_active = FALSE;
  }
  else if (id == _this->mlc_id)
  {
    _this->mlc_sensor_status.is_active = TRUE;
    _this->mlc_enable = TRUE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t IIS2DULPXTaskSensorDisable(IIS2DULPXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    _this->sensor_status.is_active = FALSE;
    iis2dulpx_md_t mode;
    mode.odr = IIS2DULPX_OFF;
    iis2dulpx_mode_set(p_sensor_drv, &mode);

    /* Changing sensor configuration must disable MLC sensor: MLC can work properly only when setup from UCF */
    _this->mlc_enable = FALSE;
    _this->mlc_sensor_status.is_active = FALSE;
  }
  else if (id == _this->mlc_id)
  {
    _this->mlc_sensor_status.is_active = FALSE;
    _this->mlc_enable = FALSE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t IIS2DULPXTaskSensorIsActive(const IIS2DULPXTask *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.is_active;
}

static sys_error_code_t IIS2DULPXTaskEnterLowPowerMode(const IIS2DULPXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  iis2dulpx_md_t mode;
  mode.odr = IIS2DULPX_OFF;
  if (iis2dulpx_mode_set(p_sensor_drv, &mode))
  {
    res = SYS_SENSOR_TASK_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_OP_ERROR_CODE);
  }

  return res;
}

static sys_error_code_t IIS2DULPXTaskConfigureIrqPin(const IIS2DULPXTask *_this, boolean_t LowPower)
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

static sys_error_code_t IIS2DULPXTaskConfigureMLCPin(const IIS2DULPXTask *_this, boolean_t LowPower)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (!LowPower)
  {
    /* Configure MLC INT Pin */
    _this->pMLCConfig->p_mx_init_f();
  }
  else
  {
    GPIO_InitTypeDef GPIO_InitStruct =
    {
      0
    };

    // first disable the IRQ to avoid spurious interrupt to wake the MCU up.
    HAL_NVIC_DisableIRQ(_this->pMLCConfig->irq_n);
    HAL_NVIC_ClearPendingIRQ(_this->pMLCConfig->irq_n);
    // then reconfigure the PIN in analog high impedance to reduce the power consumption.
    GPIO_InitStruct.Pin = _this->pMLCConfig->pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(_this->pMLCConfig->port, &GPIO_InitStruct);
  }

  return res;
}

static inline IIS2DULPXTask *IIS2DULPXTaskGetOwnerFromISensorIF(ISensor_t *p_if)
{
  assert_param(p_if != NULL);
  IIS2DULPXTask *p_if_owner = NULL;

  /* check if the virtual function has been called from the mlc sensor IF  */
  p_if_owner = (IIS2DULPXTask *)((uint32_t) p_if - offsetof(IIS2DULPXTask, mlc_sensor_if));
  if (!(p_if_owner->sensor_if.vptr == &sTheClass.sensor_if_vtbl) || !(p_if_owner->super.vptr == &sTheClass.vtbl))
  {
    /* then the virtual function has been called from the acc IF  */
    p_if_owner = (IIS2DULPXTask *)((uint32_t) p_if - offsetof(IIS2DULPXTask, sensor_if));
  }

  return p_if_owner;
}

static inline IIS2DULPXTask *IIS2DULPXTaskGetOwnerFromISensorLLIF(ISensorLL_t *p_if)
{
  assert_param(p_if != NULL);
  IIS2DULPXTask *p_if_owner = NULL;
  p_if_owner = (IIS2DULPXTask *)((uint32_t) p_if - offsetof(IIS2DULPXTask, sensor_ll_if));

  return p_if_owner;
}

static void IIS2DULPXTaskTimerCallbackFunction(ULONG param)
{
  IIS2DULPXTask *p_obj = (IIS2DULPXTask *) param;
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

static void IIS2DULPXTaskMLCTimerCallbackFunction(ULONG param)
{
  IIS2DULPXTask *p_obj = (IIS2DULPXTask *) param;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY_MLC;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  // if (sTaskObj.in_queue != NULL ) {//TODO: STF.Port - how to check if the queue has been initialized ??
  if (TX_SUCCESS != tx_queue_send(&p_obj->in_queue, &report, TX_NO_WAIT))
  {
    /* unable to send the report. Signal the error */
    sys_error_handler();
  }
  //}
}

/* CubeMX integration */

void IIS2DULPXTask_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((IIS2DULPXTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}

void INT2_DULPX_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY_MLC;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((IIS2DULPXTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}

static sys_error_code_t IIS2DULPX_ODR_Sync(IIS2DULPXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  float_t odr = 0.0f;
  iis2dulpx_md_t iis2dulpx_odr_xl;
  if (iis2dulpx_mode_get(p_sensor_drv, &iis2dulpx_odr_xl) == 0)
  {
    _this->sensor_status.is_active = TRUE;

    /* Update only the model */
    switch (iis2dulpx_odr_xl.odr)
    {
      case IIS2DULPX_OFF:
        _this->sensor_status.is_active = FALSE;
        /* Do not update the model in case of odr = 0 */
        odr = _this->sensor_status.type.mems.odr;
        break;
      case IIS2DULPX_6Hz_LP:
      case IIS2DULPX_6Hz_HP:
        odr = 6.0f;
        break;
      case IIS2DULPX_12Hz5_LP:
      case IIS2DULPX_12Hz5_HP:
        odr = 12.5f;
        break;
      case IIS2DULPX_25Hz_LP:
      case IIS2DULPX_25Hz_HP:
        odr = 25.0f;
        break;
      case IIS2DULPX_50Hz_LP:
      case IIS2DULPX_50Hz_HP:
        odr = 50.0f;
        break;
      case IIS2DULPX_100Hz_LP:
      case IIS2DULPX_100Hz_HP:
        odr = 100.0f;
        break;
      case IIS2DULPX_200Hz_LP:
      case IIS2DULPX_200Hz_HP:
        odr = 200.0f;
        break;
      case IIS2DULPX_400Hz_LP:
      case IIS2DULPX_400Hz_HP:
        odr = 400.0f;
        break;
      case IIS2DULPX_800Hz_LP:
      case IIS2DULPX_800Hz_HP:
        odr = 800.0f;
        break;
      default:
        break;
    }
    _this->sensor_status.type.mems.odr = odr;
    _this->sensor_status.type.mems.measured_odr = 0.0f;

  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }
  _this->samples_per_it = 0;
  return res;
}

static sys_error_code_t IIS2DULPX_FS_Sync(IIS2DULPXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  float_t fs = 2.0;
  iis2dulpx_md_t fs_xl;
  if (iis2dulpx_mode_get(p_sensor_drv, &fs_xl) == 0)
  {
    switch (fs_xl.fs)
    {
      case IIS2DULPX_2g:
        fs = 2.0;
        break;
      case IIS2DULPX_4g:
        fs = 4.0;
        break;
      case IIS2DULPX_8g:
        fs = 8.0;
        break;
      case IIS2DULPX_16g:
        fs = 16.0;
        break;
      default:
        break;
    }
    _this->sensor_status.type.mems.fs = fs;
    _this->sensor_status.type.mems.sensitivity = 0.0000305f * _this->sensor_status.type.mems.fs;
  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }
  return res;
}

