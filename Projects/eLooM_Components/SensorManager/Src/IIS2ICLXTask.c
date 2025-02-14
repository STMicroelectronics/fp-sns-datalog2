/**
  ******************************************************************************
  * @file    IIS2ICLXTask.c
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
#include "IIS2ICLXTask.h"
#include "IIS2ICLXTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "iis2iclx_reg.h"
#include <string.h>
#include "services/sysdebug.h"

/* Private includes ----------------------------------------------------------*/

#ifndef IIS2ICLX_TASK_CFG_STACK_DEPTH
#define IIS2ICLX_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*8)
#endif

#ifndef IIS2ICLX_TASK_CFG_PRIORITY
#define IIS2ICLX_TASK_CFG_PRIORITY                 (4)
#endif

#ifndef IIS2ICLX_TASK_CFG_IN_QUEUE_LENGTH
#define IIS2ICLX_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define IIS2ICLX_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef IIS2ICLX_TASK_CFG_TIMER_PERIOD_MS
#define IIS2ICLX_TASK_CFG_TIMER_PERIOD_MS          1000
#endif

#ifndef IIS2ICLX_TASK_CFG_MLC_TIMER_PERIOD_MS
#define IIS2ICLX_TASK_CFG_MLC_TIMER_PERIOD_MS      500
#endif

#ifndef IIS2ICLX_TASK_CFG_MAX_INSTANCES_COUNT
#define IIS2ICLX_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_IIS2ICLX, level, message)

#ifndef IIS2ICLX_TASK_CFG_I2C_ADDRESS
#define IIS2ICLX_TASK_CFG_I2C_ADDRESS             IIS2ICLX_I2C_ADD_H
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
typedef struct _IIS2ICLXTaskClass
{
  /**
    * IIS2ICLXTask class virtual table.
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
    * IIS2ICLXTask (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[IIS2ICLX_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} IIS2ICLXTaskClass_t;


/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS2ICLXTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS2ICLXTaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS2ICLXTaskSensorInit(IIS2ICLXTask *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS2ICLXTaskSensorReadData(IIS2ICLXTask *_this);

/**
  * Read the data from the mlc.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS2ICLXTaskSensorReadMLC(IIS2ICLXTask *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t IIS2ICLXTaskSensorRegister(IIS2ICLXTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t IIS2ICLXTaskSensorInitTaskParams(IIS2ICLXTask *_this);

/**
  * Private implementation of sensor interface methods for IIS2ICLX sensor
  */
static sys_error_code_t IIS2ICLXTaskSensorSetODR(IIS2ICLXTask *_this, SMMessage report);
static sys_error_code_t IIS2ICLXTaskSensorSetFS(IIS2ICLXTask *_this, SMMessage report);
static sys_error_code_t IIS2ICLXTaskSensorSetFifoWM(IIS2ICLXTask *_this, SMMessage report);
static sys_error_code_t IIS2ICLXTaskSensorEnable(IIS2ICLXTask *_this, SMMessage report);
static sys_error_code_t IIS2ICLXTaskSensorDisable(IIS2ICLXTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t IIS2ICLXTaskSensorIsActive(const IIS2ICLXTask *_this);

static sys_error_code_t IIS2ICLXTaskEnterLowPowerMode(const IIS2ICLXTask *_this);

static sys_error_code_t IIS2ICLXTaskConfigureIrqPin(const IIS2ICLXTask *_this, boolean_t LowPower);
static sys_error_code_t IIS2ICLXTaskConfigureMLCPin(const IIS2ICLXTask *_this, boolean_t LowPower);

/**
  * Given a interface pointer it return the instance of the object that implement the interface.
  *
  * @param p_if [IN] specifies a sensor interface implemented by the task object.
  * @return the instance of the task object that implements the given interface.
  */
static inline IIS2ICLXTask *IIS2ICLXTaskGetOwnerFromISensorIF(ISensor_t *p_if);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void IIS2ICLXTaskTimerCallbackFunction(ULONG param);

/**
  * Callback function called when the mlc software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void IIS2ICLXTaskMLCTimerCallbackFunction(ULONG param);

/**
  * Read the odr value from the sensor and update the internal model
  */
static sys_error_code_t IIS2ICLX_ODR_Sync(IIS2ICLXTask *_this);

/**
  * Read the odr value from the sensor and update the internal model
  */
static sys_error_code_t IIS2ICLX_FS_Sync(IIS2ICLXTask *_this);


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
static inline sys_error_code_t IIS2ICLXTaskPostReportToFront(IIS2ICLXTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t IIS2ICLXTaskPostReportToBack(IIS2ICLXTask *_this, SMMessage *pReport);


/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static IIS2ICLXTask sTaskObj;

/**
  * The class object.
  */
static IIS2ICLXTaskClass_t sTheClass =
{
  /* class virtual table */
  {
    IIS2ICLXTask_vtblHardwareInit,
    IIS2ICLXTask_vtblOnCreateTask,
    IIS2ICLXTask_vtblDoEnterPowerMode,
    IIS2ICLXTask_vtblHandleError,
    IIS2ICLXTask_vtblOnEnterTaskControlLoop,
    IIS2ICLXTask_vtblForceExecuteStep,
    IIS2ICLXTask_vtblOnEnterPowerMode
  },

  /* class::sensor_if_vtbl virtual table */
  {
    {
      {
        IIS2ICLXTask_vtblAccGetId,
        IIS2ICLXTask_vtblGetEventSourceIF,
        IIS2ICLXTask_vtblAccGetDataInfo
      },
      IIS2ICLXTask_vtblSensorEnable,
      IIS2ICLXTask_vtblSensorDisable,
      IIS2ICLXTask_vtblSensorIsEnabled,
      IIS2ICLXTask_vtblSensorGetDescription,
      IIS2ICLXTask_vtblSensorGetStatus,
      IIS2ICLXTask_vtblSensorGetStatusPointer
    },
    IIS2ICLXTask_vtblAccGetODR,
    IIS2ICLXTask_vtblAccGetFS,
    IIS2ICLXTask_vtblAccGetSensitivity,
    IIS2ICLXTask_vtblSensorSetODR,
    IIS2ICLXTask_vtblSensorSetFS,
    IIS2ICLXTask_vtblSensorSetFifoWM
  },

  /* class::mlc_sensor_if_vtbl virtual table */
  {
    {
      {
        IIS2ICLXTask_vtblMlcGetId,
        IIS2ICLXTask_vtblMlcGetEventSourceIF,
        IIS2ICLXTask_vtblMlcGetDataInfo
      },
      IIS2ICLXTask_vtblSensorEnable,
      IIS2ICLXTask_vtblSensorDisable,
      IIS2ICLXTask_vtblSensorIsEnabled,
      IIS2ICLXTask_vtblMlcGetDescription,
      IIS2ICLXTask_vtblMlcGetStatus,
      IIS2ICLXTask_vtblMlcGetStatusPointer
    },
    IIS2ICLXTask_vtblMlcGetODR,
    IIS2ICLXTask_vtblMlcGetFS,
    IIS2ICLXTask_vtblMlcGetSensitivity,
    IIS2ICLXTask_vtblSensorSetODR,
    IIS2ICLXTask_vtblSensorSetFS,
    IIS2ICLXTask_vtblSensorSetFifoWM
  },

  /* class::sensor_ll_if_vtbl virtual table */
  {
    IIS2ICLXTask_vtblSensorReadReg,
    IIS2ICLXTask_vtblSensorWriteReg,
    IIS2ICLXTask_vtblSensorSyncModel
  },

  /* ACCELEROMETER DESCRIPTOR */
  {
    "iis2iclx",
    COM_TYPE_ACC
  },

  /* MLC DESCRIPTOR */
  {
    "iis2iclx",
    COM_TYPE_MLC
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    IIS2ICLXTaskExecuteStepState1,
    NULL,
    IIS2ICLXTaskExecuteStepDatalog,
  },

  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *IIS2ICLXTaskGetAccSensorIF(IIS2ICLXTask *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

ISourceObservable *IIS2ICLXTaskGetMlcSensorIF(IIS2ICLXTask *_this)
{
  return (ISourceObservable *) & (_this->mlc_sensor_if);
}

ISensorLL_t *IIS2ICLXTaskGetSensorLLIF(IIS2ICLXTask *_this)
{
  return (ISensorLL_t *) & (_this->sensor_ll_if);
}


AManagedTaskEx *IIS2ICLXTaskAlloc(const void *pIRQConfig, const void *pMLCConfig, const void *pCSConfig)
{
  IIS2ICLXTask *p_new_obj = SysAlloc(sizeof(IIS2ICLXTask));

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

AManagedTaskEx *IIS2ICLXTaskAllocSetName(const void *pIRQConfig, const void *pMLCConfig, const void *pCSConfig,
                                         const char *p_name)
{
  IIS2ICLXTask *p_new_obj = (IIS2ICLXTask *)IIS2ICLXTaskAlloc(pIRQConfig, pMLCConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->sensor_status.p_name, p_name);
  strcpy(p_new_obj->mlc_sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *IIS2ICLXTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pMLCConfig,
                                        const void *pCSConfig)
{
  IIS2ICLXTask *p_obj = (IIS2ICLXTask *)p_mem_block;

  if (p_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_obj->super);
    p_obj->super.vptr = &sTheClass.vtbl;

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

AManagedTaskEx *IIS2ICLXTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pMLCConfig,
                                               const void *pCSConfig,
                                               const char *p_name)
{
  IIS2ICLXTask *p_obj = (IIS2ICLXTask *)IIS2ICLXTaskStaticAlloc(p_mem_block, pIRQConfig, pMLCConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->sensor_status.p_name, p_name);
  strcpy(p_obj->mlc_sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *IIS2ICLXTaskGetSensorIF(IIS2ICLXTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *IIS2ICLXTaskGetEventSrcIF(IIS2ICLXTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_event_src;
}

IEventSrc *IIS2ICLXTaskGetMlcEventSrcIF(IIS2ICLXTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_mlc_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t IIS2ICLXTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2ICLXTask *p_obj = (IIS2ICLXTask *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t IIS2ICLXTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                               VOID **pvStackStart,
                                               ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                               ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2ICLXTask *p_obj = (IIS2ICLXTask *) _this;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) IIS2ICLX_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(IIS2ICLX_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "IIS2ICLX_Q", item_size / 4, p_queue_items_buff,
                                    IIS2ICLX_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_timer, "IIS2ICLX_T", IIS2ICLXTaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(IIS2ICLX_TASK_CFG_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* create the mlc software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->mlc_timer, "IIS2ICLX_MLC_T", IIS2ICLXTaskMLCTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(IIS2ICLX_TASK_CFG_MLC_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(IIS2ICLX_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(IIS2ICLX_ID, IIS2ICLX_TASK_CFG_I2C_ADDRESS, 0);
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
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, IIS2ICLX_TASK_CFG_MAX_INSTANCES_COUNT);
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
  *pName = "IIS2ICLX";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = IIS2ICLX_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = IIS2ICLX_TASK_CFG_PRIORITY;
  *pPreemptThreshold = IIS2ICLX_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = IIS2ICLXTaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = IIS2ICLXTaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS2ICLX: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t IIS2ICLXTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2ICLXTask *p_obj = (IIS2ICLXTask *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (IIS2ICLXTaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS2ICLX: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      if (IIS2ICLXTaskSensorIsActive(p_obj))
      {
        /* Deactivate the sensor */
        iis2iclx_xl_data_rate_set(p_sensor_drv, IIS2ICLX_XL_ODR_OFF);
        iis2iclx_fifo_xl_batch_set(p_sensor_drv, IIS2ICLX_XL_NOT_BATCHED);
        iis2iclx_fifo_mode_set(p_sensor_drv, IIS2ICLX_BYPASS_MODE);
      }
      p_obj->first_data_ready = 0;
      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_timer);
      }
      else
      {
        IIS2ICLXTaskConfigureIrqPin(p_obj, TRUE);
      }
      if (p_obj->pMLCConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->mlc_timer);
      }
      else
      {
        IIS2ICLXTaskConfigureMLCPin(p_obj, TRUE);
      }
      memset(p_obj->p_mlc_sensor_data_buff, 0, sizeof(p_obj->p_mlc_sensor_data_buff));
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS2ICLX: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = IIS2ICLXTaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2ICLX - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->pIRQConfig != NULL)
    {
      IIS2ICLXTaskConfigureIrqPin(p_obj, TRUE);
    }
    if (p_obj->pMLCConfig != NULL)
    {
      IIS2ICLXTaskConfigureMLCPin(p_obj, TRUE);
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS2ICLX: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t IIS2ICLXTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  IIS2ICLXTask *p_obj = (IIS2ICLXTask*)_this;

  return res;
}

sys_error_code_t IIS2ICLXTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("IIS2ICLX: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (IIS2ICLX_TASK_CFG_TAG)
  IIS2ICLXTask *p_obj = (IIS2ICLXTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = IIS2ICLX_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t IIS2ICLXTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2ICLXTask *p_obj = (IIS2ICLXTask *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = IIS2ICLXTaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t IIS2ICLXTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  IIS2ICLXTask *p_obj = (IIS2ICLXTask*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t IIS2ICLXTask_vtblAccGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS2ICLXTask *p_if_owner = (IIS2ICLXTask *)((uint32_t) _this - offsetof(IIS2ICLXTask, sensor_if));
  uint8_t res = p_if_owner->acc_id;

  return res;
}

uint8_t IIS2ICLXTask_vtblMlcGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS2ICLXTask *p_if_owner = (IIS2ICLXTask *)((uint32_t) _this - offsetof(IIS2ICLXTask, mlc_sensor_if));
  uint8_t res = p_if_owner->mlc_id;

  return res;
}

IEventSrc *IIS2ICLXTask_vtblGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS2ICLXTask *p_if_owner = (IIS2ICLXTask *)((uint32_t) _this - offsetof(IIS2ICLXTask, sensor_if));

  return p_if_owner->p_event_src;
}

IEventSrc *IIS2ICLXTask_vtblMlcGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS2ICLXTask *p_if_owner = (IIS2ICLXTask *)((uint32_t) _this - offsetof(IIS2ICLXTask, mlc_sensor_if));
  return p_if_owner->p_mlc_event_src;
}

sys_error_code_t IIS2ICLXTask_vtblAccGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  IIS2ICLXTask *p_if_owner = (IIS2ICLXTask *)((uint32_t) _this - offsetof(IIS2ICLXTask, sensor_if));
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

float IIS2ICLXTask_vtblAccGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  IIS2ICLXTask *p_if_owner = (IIS2ICLXTask *)((uint32_t) _this - offsetof(IIS2ICLXTask, sensor_if));
  float res = p_if_owner->sensor_status.type.mems.fs;

  return res;
}

float IIS2ICLXTask_vtblAccGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  IIS2ICLXTask *p_if_owner = (IIS2ICLXTask *)((uint32_t) _this - offsetof(IIS2ICLXTask, sensor_if));
  float res = p_if_owner->sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t IIS2ICLXTask_vtblAccGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS2ICLXTask *p_if_owner = (IIS2ICLXTask *)((uint32_t) _this - offsetof(IIS2ICLXTask, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t IIS2ICLXTask_vtblMlcGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  IIS2ICLXTask *p_if_owner = (IIS2ICLXTask *)((uint32_t) _this - offsetof(IIS2ICLXTask, mlc_sensor_if));
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

float IIS2ICLXTask_vtblMlcGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);

  /* MLC does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2ICLX: warning - MLC GetFS() not supported.\r\n"));

  return -1.0f;
}

float IIS2ICLXTask_vtblMlcGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);

  /* MLC does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2ICLX: warning - MLC GetSensitivity() not supported.\r\n"));

  return -1.0f;
}

EMData_t IIS2ICLXTask_vtblMlcGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS2ICLXTask *p_if_owner = (IIS2ICLXTask *)((uint32_t) _this - offsetof(IIS2ICLXTask, mlc_sensor_if));
  EMData_t res = p_if_owner->data_mlc;

  return res;
}

sys_error_code_t IIS2ICLXTask_vtblSensorSetODR(ISensorMems_t *_this, float odr)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2ICLXTask *p_if_owner = IIS2ICLXTaskGetOwnerFromISensorIF((ISensor_t *)_this);

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

      if (sensor_id == p_if_owner->acc_id)
      {
        p_if_owner->sensor_status.type.mems.odr = odr;
        p_if_owner->sensor_status.type.mems.measured_odr = 0.0f;
      }
      else if (sensor_id == p_if_owner->mlc_id)
      {
        p_if_owner->mlc_sensor_status.type.mems.odr = odr;
        p_if_owner->mlc_sensor_status.type.mems.measured_odr = 0.0f;
      }
      else
      {
        /**/
      }
    }
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_ODR,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.fParam = (float) odr
    };
    res = IIS2ICLXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t IIS2ICLXTask_vtblSensorSetFS(ISensorMems_t *_this, float fs)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2ICLXTask *p_if_owner = IIS2ICLXTaskGetOwnerFromISensorIF((ISensor_t *)_this);

  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    if (sensor_id == p_if_owner->acc_id)
    {
      p_if_owner->sensor_status.type.mems.fs = fs;
      if (fs < 3.0f)
      {
        p_if_owner->sensor_status.type.mems.sensitivity = 0.0000305f * p_if_owner->sensor_status.type.mems.fs;
      }
      else
      {
        p_if_owner->sensor_status.type.mems.sensitivity = 0.0000406f * p_if_owner->sensor_status.type.mems.fs;
      }
    }
    else if (sensor_id == p_if_owner->mlc_id)
    {
      p_if_owner->mlc_sensor_status.type.mems.fs = fs;
    }
    else
    {
      /**/
    }
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FS,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.fParam = (float) fs
    };
    res = IIS2ICLXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t IIS2ICLXTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

#if IIS2ICLX_FIFO_ENABLED
  IIS2ICLXTask *p_if_owner = IIS2ICLXTaskGetOwnerFromISensorIF((ISensor_t *)_this);
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
    res = IIS2ICLXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }
#endif

  return res;
}

sys_error_code_t IIS2ICLXTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2ICLXTask *p_if_owner = IIS2ICLXTaskGetOwnerFromISensorIF(_this);

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
    res = IIS2ICLXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t IIS2ICLXTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2ICLXTask *p_if_owner = IIS2ICLXTaskGetOwnerFromISensorIF(_this);

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
    res = IIS2ICLXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t IIS2ICLXTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  IIS2ICLXTask *p_if_owner = IIS2ICLXTaskGetOwnerFromISensorIF(_this);

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->acc_id)
  {
    res = p_if_owner->sensor_status.is_active;
  }
  else if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->mlc_id)
  {
    res = p_if_owner->mlc_sensor_status.is_active;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

SensorDescriptor_t IIS2ICLXTask_vtblSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS2ICLXTask *p_if_owner = IIS2ICLXTaskGetOwnerFromISensorIF(_this);

  return *p_if_owner->sensor_descriptor;
}

SensorDescriptor_t IIS2ICLXTask_vtblMlcGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS2ICLXTask *p_if_owner = IIS2ICLXTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->mlc_sensor_descriptor;
}

SensorStatus_t IIS2ICLXTask_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS2ICLXTask *p_if_owner = IIS2ICLXTaskGetOwnerFromISensorIF(_this);

  return p_if_owner->sensor_status;
}

SensorStatus_t IIS2ICLXTask_vtblMlcGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS2ICLXTask *p_if_owner = IIS2ICLXTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->mlc_sensor_status;
}

SensorStatus_t *IIS2ICLXTask_vtblSensorGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS2ICLXTask *p_if_owner = IIS2ICLXTaskGetOwnerFromISensorIF(_this);

  return &p_if_owner->sensor_status;
}

SensorStatus_t *IIS2ICLXTask_vtblMlcGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS2ICLXTask *p_if_owner = IIS2ICLXTaskGetOwnerFromISensorIF(_this);
  return &p_if_owner->mlc_sensor_status;
}

sys_error_code_t IIS2ICLXTask_vtblSensorReadReg(ISensorLL_t *_this, uint16_t reg, uint8_t *data, uint16_t len)
{
  assert_param(_this != NULL);
  assert_param(reg <= 0xFFU);
  assert_param(data != NULL);
  assert_param(len != 0U);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2ICLXTask *p_if_owner = (IIS2ICLXTask *)((uint32_t) _this - offsetof(IIS2ICLXTask, sensor_ll_if));
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_if_owner->p_sensor_bus_if->m_xConnector;
  uint8_t reg8 = (uint8_t)(reg & 0x00FF);

  if (iis2iclx_read_reg(p_sensor_drv, reg8, data, len) != 0)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t IIS2ICLXTask_vtblSensorWriteReg(ISensorLL_t *_this, uint16_t reg, const uint8_t *data, uint16_t len)
{
  assert_param(_this != NULL);
  assert_param(reg <= 0xFFU);
  assert_param(data != NULL);
  assert_param(len != 0U);

  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2ICLXTask *p_if_owner = (IIS2ICLXTask *)((uint32_t) _this - offsetof(IIS2ICLXTask, sensor_ll_if));
  uint8_t reg8 = (uint8_t)(reg & 0x00FF);

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_if_owner->p_sensor_bus_if->m_xConnector;

  /* This generic register write operation could mean that the model is out of sync with the HW */
  p_if_owner->sync = false;

  if (iis2iclx_write_reg(p_sensor_drv, reg8, (uint8_t *)data, len) != 0)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t IIS2ICLXTask_vtblSensorSyncModel(ISensorLL_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2ICLXTask *p_if_owner = (IIS2ICLXTask *)((uint32_t) _this - offsetof(IIS2ICLXTask, sensor_ll_if));

  if (IIS2ICLX_ODR_Sync(p_if_owner) != SYS_NO_ERROR_CODE)
  {
    res = SYS_BASE_ERROR_CODE;
  }
  if (IIS2ICLX_FS_Sync(p_if_owner) != SYS_NO_ERROR_CODE)
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
static sys_error_code_t IIS2ICLXTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2ICLXTask *p_obj = (IIS2ICLXTask *) _this;
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
            res = IIS2ICLXTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = IIS2ICLXTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = IIS2ICLXTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = IIS2ICLXTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = IIS2ICLXTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2ICLX: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2ICLX: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t IIS2ICLXTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS2ICLXTask *p_obj = (IIS2ICLXTask *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("IIS2ICLX: new data.\r\n"));
        res = IIS2ICLXTaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          if (p_obj->first_data_ready == 1)
          {
#if IIS2ICLX_FIFO_ENABLED
            if (p_obj->fifo_level != 0)
            {
#endif
              // update the time stamp
              // notify the listeners...
              double timestamp = report.sensorDataReadyMessage.fTimestamp;
              double delta_timestamp = timestamp - p_obj->prev_timestamp;
              p_obj->prev_timestamp = timestamp;

              /* update measuredODR */
              p_obj->sensor_status.type.mems.measured_odr = (float) p_obj->samples_per_it / (float) delta_timestamp;

              /* Create a bidimensional data interleaved [m x 2], m is the number of samples in the sensor queue (samples_per_it):
               * [X0, Y0]
               * [X1, Y1]
               * ...
               * [Xm-1, Ym-1]
               */
              EMD_Init(&p_obj->data, p_obj->p_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, p_obj->samples_per_it, 2);

              DataEvent_t evt;

              DataEventInit((IEvent *) &evt, p_obj->p_event_src, &p_obj->data, timestamp, p_obj->acc_id);
              IEventSrcSendEvent(p_obj->p_event_src, (IEvent *) &evt, NULL);
              SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("IIS2ICLX: ts = %f\r\n", (float)timestamp));
#if IIS2ICLX_FIFO_ENABLED
            }
#endif
          }
          else
          {
            p_obj->first_data_ready = 1;
          }
        }
        break;
      }
      case SM_MESSAGE_ID_DATA_READY_MLC:
      {
        res = IIS2ICLXTaskSensorReadMLC(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          // notify the listeners...
          double timestamp = report.sensorDataReadyMessage.fTimestamp;

          if (p_obj->mlc_enable)
          {
            EMD_Init(&p_obj->data_mlc, p_obj->p_mlc_sensor_data_buff, E_EM_UINT8, E_EM_MODE_INTERLEAVED, 2, 1, 9);

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
            res = IIS2ICLXTaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->sensor_status.is_active == true)
              {
                if (p_obj->pIRQConfig == NULL)
                {
                  if (TX_SUCCESS
                      != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->iis2iclx_task_cfg_timer_period_ms),
                                         AMT_MS_TO_TICKS(p_obj->iis2iclx_task_cfg_timer_period_ms)))
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
                  IIS2ICLXTaskConfigureIrqPin(p_obj, FALSE);
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
                      != tx_timer_change(&p_obj->mlc_timer, AMT_MS_TO_TICKS(IIS2ICLX_TASK_CFG_MLC_TIMER_PERIOD_MS),
                                         AMT_MS_TO_TICKS(IIS2ICLX_TASK_CFG_MLC_TIMER_PERIOD_MS)))
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
                  IIS2ICLXTaskConfigureMLCPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = IIS2ICLXTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = IIS2ICLXTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = IIS2ICLXTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = IIS2ICLXTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = IIS2ICLXTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2ICLX: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2ICLX: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t IIS2ICLXTaskPostReportToFront(IIS2ICLXTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t IIS2ICLXTaskPostReportToBack(IIS2ICLXTask *_this, SMMessage *pReport)
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

static sys_error_code_t IIS2ICLXTaskSensorInit(IIS2ICLXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t reg0 = 0;
  int32_t ret_val = 0;
  iis2iclx_odr_xl_t iis2iclx_odr_xl = IIS2ICLX_XL_ODR_OFF;
  iis2iclx_bdr_xl_t iis2iclx_bdr_xl = IIS2ICLX_XL_NOT_BATCHED;

  // if this variable need to persist then I move it in the managed task class declaration.
  iis2iclx_pin_int1_route_t int1_route =
  {
    0
  };
  iis2iclx_pin_int2_route_t int2_route =
  {
    0
  };

  // reset the sensor
  ret_val = iis2iclx_reset_set(p_sensor_drv, 1);
  do
  {
    iis2iclx_reset_get(p_sensor_drv, &reg0);
  } while (reg0);

  /*Disable the I2C interface */
  iis2iclx_i2c_interface_set(p_sensor_drv, IIS2ICLX_I2C_DISABLE);

  ret_val = iis2iclx_device_id_get(p_sensor_drv, (uint8_t *) &reg0);
  if (!ret_val)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, reg0);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS2ICLX: sensor - I am 0x%x.\r\n", reg0));

  //TODO: STF - what is this?
  iis2iclx_read_reg(p_sensor_drv, IIS2ICLX_CTRL1_XL, (uint8_t *) &reg0, 1);
  reg0 |= 0xA0;
  iis2iclx_write_reg(p_sensor_drv, IIS2ICLX_CTRL1_XL, (uint8_t *) &reg0, 1);

  /*Set odr and BDR*/

  if (_this->sensor_status.type.mems.odr < 13.0f)
  {
    iis2iclx_odr_xl = IIS2ICLX_XL_ODR_12Hz5;
    iis2iclx_bdr_xl = IIS2ICLX_XL_BATCHED_AT_12Hz5;
  }
  else if (_this->sensor_status.type.mems.odr < 27.0f)
  {
    iis2iclx_odr_xl = IIS2ICLX_XL_ODR_26Hz;
    iis2iclx_bdr_xl = IIS2ICLX_XL_BATCHED_AT_26Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 53.0f)
  {
    iis2iclx_odr_xl = IIS2ICLX_XL_ODR_52Hz;
    iis2iclx_bdr_xl = IIS2ICLX_XL_BATCHED_AT_52Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 105.0f)
  {
    iis2iclx_odr_xl = IIS2ICLX_XL_ODR_104Hz;
    iis2iclx_bdr_xl = IIS2ICLX_XL_BATCHED_AT_104Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 209.0f)
  {
    iis2iclx_odr_xl = IIS2ICLX_XL_ODR_208Hz;
    iis2iclx_bdr_xl = IIS2ICLX_XL_BATCHED_AT_208Hz;
  }
  else if (_this->sensor_status.type.mems.odr < 417.0f)
  {
    iis2iclx_odr_xl = IIS2ICLX_XL_ODR_416Hz;
    iis2iclx_bdr_xl = IIS2ICLX_XL_BATCHED_AT_417Hz;
  }
  else
  {
    iis2iclx_odr_xl = IIS2ICLX_XL_ODR_833Hz;
    iis2iclx_bdr_xl = IIS2ICLX_XL_BATCHED_AT_833Hz;
  }

  /*Set full scale*/
  if (_this->sensor_status.type.mems.fs < 1.0f)
  {
    iis2iclx_xl_full_scale_set(p_sensor_drv, IIS2ICLX_500mg);
  }
  else if (_this->sensor_status.type.mems.fs < 2.0f)
  {
    iis2iclx_xl_full_scale_set(p_sensor_drv, IIS2ICLX_1g);
  }
  else if (_this->sensor_status.type.mems.fs < 3.0f)
  {
    iis2iclx_xl_full_scale_set(p_sensor_drv, IIS2ICLX_2g);
  }
  else
  {
    iis2iclx_xl_full_scale_set(p_sensor_drv, IIS2ICLX_3g);
  }

  if (_this->sensor_status.is_active)
  {
    iis2iclx_xl_data_rate_set(p_sensor_drv, iis2iclx_odr_xl);
    iis2iclx_fifo_xl_batch_set(p_sensor_drv, iis2iclx_bdr_xl);
  }
  else
  {
    iis2iclx_xl_data_rate_set(p_sensor_drv, IIS2ICLX_XL_ODR_OFF);
    iis2iclx_fifo_xl_batch_set(p_sensor_drv, IIS2ICLX_XL_NOT_BATCHED);
    _this->sensor_status.is_active = false;
  }

#if IIS2ICLX_FIFO_ENABLED

  if (_this->samples_per_it == 0)
  {
    uint16_t iis2iclx_wtm_level = 0;

    /* Calculation of watermark and samples per int*/
    iis2iclx_wtm_level = ((uint16_t) _this->sensor_status.type.mems.odr * (uint16_t) IIS2ICLX_MAX_DRDY_PERIOD);
    if (iis2iclx_wtm_level > IIS2ICLX_MAX_WTM_LEVEL)
    {
      iis2iclx_wtm_level = IIS2ICLX_MAX_WTM_LEVEL;
    }
    _this->samples_per_it = iis2iclx_wtm_level;
  }

  /* Set fifo in continuous / stream mode*/
  iis2iclx_fifo_mode_set(p_sensor_drv, IIS2ICLX_STREAM_MODE);

  /* Set FIFO watermark */
  iis2iclx_fifo_watermark_set(p_sensor_drv, _this->samples_per_it);

  if (_this->pIRQConfig != NULL)
  {
    int1_route.int1_ctrl.int1_fifo_th = 1;
  }
  else
  {
    int1_route.int1_ctrl.int1_fifo_th = 0;
  }
  iis2iclx_pin_int1_route_set(p_sensor_drv, &int1_route);

#else
  _this->samples_per_it = 1;
  if (_this->pIRQConfig != NULL)
  {
    int1_route.int1_ctrl.int1_drdy_xl = 1;
  }
  else
  {
    int1_route.int1_ctrl.int1_drdy_xl = 0;
  }
  iis2iclx_pin_int1_route_set(p_sensor_drv, &int1_route);
#endif /* IIS2ICLX_FIFO_ENABLED */

  /* Setup mlc */
  if (_this->mlc_enable)
  {
    iis2iclx_pin_int1_route_get(p_sensor_drv, &int1_route);
    iis2iclx_pin_int2_route_get(p_sensor_drv, &int2_route);

    if (int1_route.mlc_int1.int1_mlc1 == 1 || int1_route.md1_cfg.int1_emb_func == 1)
    {
      int1_route.mlc_int1.int1_mlc1 = 0;
      int1_route.md1_cfg.int1_emb_func = 0;
      iis2iclx_pin_int1_route_set(p_sensor_drv, &int1_route);
    }

    if (int2_route.mlc_int2.int2_mlc1 == 0 || int2_route.md2_cfg.int2_emb_func == 0)
    {
      int2_route.mlc_int2.int2_mlc1 = 1;
      int2_route.md2_cfg.int2_emb_func = 1;
      iis2iclx_pin_int2_route_set(p_sensor_drv, &int2_route);
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
#if IIS2ICLX_FIFO_ENABLED
    _this->iis2iclx_task_cfg_timer_period_ms = (uint16_t)((1000.0f / _this->sensor_status.type.mems.odr) * (((float)(_this->samples_per_it)) / 2.0f));
#else
    _this->iis2iclx_task_cfg_timer_period_ms = (uint16_t)(1000.0f / _this->sensor_status.type.mems.odr);
#endif
  }

  return res;
}

static sys_error_code_t IIS2ICLXTaskSensorReadData(IIS2ICLXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t samples_per_it = _this->samples_per_it;

#if IIS2ICLX_FIFO_ENABLED
  uint8_t reg[2];
  uint16_t i;

  /* Check FIFO_WTM_IA and fifo level. We do not use PID in order to avoid reading one register twice */
  iis2iclx_read_reg(p_sensor_drv, IIS2ICLX_FIFO_STATUS1, reg, 2);

  _this->fifo_level = ((reg[1] & 0x03) << 8) + reg[0];

  if ((reg[1]) & 0x80 && (_this->fifo_level >= samples_per_it))
  {
    res = iis2iclx_read_reg(p_sensor_drv, IIS2ICLX_FIFO_DATA_OUT_TAG, (uint8_t *) _this->p_fifo_data_buff, samples_per_it * 7);

    if (!SYS_IS_ERROR_CODE(res))
    {
#if (HSD_USE_DUMMY_DATA == 1)
      int16_t *p16 = (int16_t *)_this->p_sensor_data_buff;

      for (i = 0; i < samples_per_it; i++)
      {
        *p16++ = dummyDataCounter++;
        *p16++ = dummyDataCounter++;
      }
#else
      /* Arrange Data */
      int16_t *p16_src = (int16_t *) _this->p_fifo_data_buff;
      int16_t *p16_dest = (int16_t *) _this->p_sensor_data_buff;
      for (i = 0; i < samples_per_it; i++)
      {
        p16_src = (int16_t *) & ((uint8_t *)(p16_src))[1];
        *p16_dest++ = *p16_src++;
        *p16_dest++ = *p16_src++;
        p16_src++;
      }
#endif /* HSD_USE_DUMMY_DATA */
    }
  }
  else
  {
    _this->fifo_level = 0;
    res = SYS_BASE_ERROR_CODE;
  }

#else
  res = iis2iclx_read_reg(p_sensor_drv, IIS2ICLX_OUTX_L_A, (uint8_t *) _this->p_sensor_data_buff, samples_per_it * 4);
  _this->fifo_level = 1;

  if (!SYS_IS_ERROR_CODE(res))
  {
#if (HSD_USE_DUMMY_DATA == 1)
    int16_t *p16 = (int16_t *)_this->p_sensor_data_buff;

    for (i = 0; i < samples_per_it; i++)
    {
      *p16++ = dummyDataCounter++;
      *p16++ = dummyDataCounter++;
    }
#endif /* HSD_USE_DUMMY_DATA */
  }
#endif /* IIS2ICLX_FIFO_ENABLED */

  return res;
}

static sys_error_code_t IIS2ICLXTaskSensorReadMLC(IIS2ICLXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  if (_this->mlc_enable)
  {
    iis2iclx_mlc_status_get(p_sensor_drv, (iis2iclx_mlc_status_mainpage_t *) &_this->p_mlc_sensor_data_buff[8]);
    iis2iclx_mem_bank_set(p_sensor_drv, IIS2ICLX_EMBEDDED_FUNC_BANK);
    iis2iclx_read_reg(p_sensor_drv, IIS2ICLX_MLC0_SRC, (uint8_t *)(&_this->p_mlc_sensor_data_buff[0]), 8);
    iis2iclx_mem_bank_set(p_sensor_drv, IIS2ICLX_USER_BANK);
  }

  return res;
}

static sys_error_code_t IIS2ICLXTaskSensorRegister(IIS2ICLXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

#if !IIS2ICLX_ACC_DISABLED
  ISensor_t *acc_if = (ISensor_t *) IIS2ICLXTaskGetAccSensorIF(_this);
  _this->acc_id = SMAddSensor(acc_if);
#endif

#if !IIS2ICLX_MLC_DISABLED
  ISensor_t *mlc_if = (ISensor_t *) IIS2ICLXTaskGetMlcSensorIF(_this);
  _this->mlc_id = SMAddSensor(mlc_if);
#endif

  return res;
}

static sys_error_code_t IIS2ICLXTaskSensorInitTaskParams(IIS2ICLXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* ACCELEROMETER SENSOR STATUS */
  _this->sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->sensor_status.is_active = TRUE;
  _this->sensor_status.type.mems.fs = 3.0f;
  _this->sensor_status.type.mems.sensitivity = 0.0000406f * _this->sensor_status.type.mems.fs;
  _this->sensor_status.type.mems.odr = 833.0f;
  _this->sensor_status.type.mems.measured_odr = 0.0f;
  EMD_Init(&_this->data, _this->p_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 2);

  /* MLC STATUS */
  _this->mlc_sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->mlc_sensor_status.is_active = FALSE;
  _this->mlc_sensor_status.type.mems.fs = 1.0f;
  _this->mlc_sensor_status.type.mems.sensitivity = 1.0f;
  _this->mlc_sensor_status.type.mems.odr = 1.0f;
  _this->mlc_sensor_status.type.mems.measured_odr = 0.0f;
  EMD_Init(&_this->data_mlc, _this->p_mlc_sensor_data_buff, E_EM_UINT8, E_EM_MODE_INTERLEAVED, 2, 1, 9);

  return res;
}

static sys_error_code_t IIS2ICLXTaskSensorSetODR(IIS2ICLXTask *_this, SMMessage report)
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
      iis2iclx_xl_data_rate_set(p_sensor_drv, IIS2ICLX_XL_ODR_OFF);
      /* Do not update the model in case of odr = 0 */
      odr = _this->sensor_status.type.mems.odr;
    }
    else if (odr < 13.0f)
    {
      odr = 12.5f;
    }
    else if (odr < 27.0f)
    {
      odr = 26.0f;
    }
    else if (odr < 53.0f)
    {
      odr = 52.0f;
    }
    else if (odr < 105.0f)
    {
      odr = 104.0f;
    }
    else if (odr < 209.0f)
    {
      odr = 208.0f;
    }
    else if (odr < 417.0f)
    {
      odr = 416.0f;
    }
    else
    {
      odr = 833.0f;
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

    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2ICLX: warning - MLC SetODR() not supported.\r\n"));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t IIS2ICLXTaskSensorSetFS(IIS2ICLXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  float fs = (float) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    if (fs < 1.0f)
    {
      fs = 0.5f;
    }
    else if (fs < 2.0f)
    {
      fs = 1.0f;
    }
    else if (fs < 3.0f)
    {
      fs = 2.0f;
    }
    else
    {
      fs = 3.0f;
    }

    _this->sensor_status.type.mems.fs = fs;
    if (fs < 3.0f)
    {
      _this->sensor_status.type.mems.sensitivity = 0.0000305f * _this->sensor_status.type.mems.fs;
    }
    else
    {
      _this->sensor_status.type.mems.sensitivity = 0.0000406f * _this->sensor_status.type.mems.fs;
    }
  }
  else if (id == _this->mlc_id)
  {
    res = SYS_TASK_INVALID_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS2ICLX: warning - MLC SetFS() not supported.\r\n"));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t IIS2ICLXTaskSensorSetFifoWM(IIS2ICLXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (report.sensorMessage.nSensorId == _this->mlc_id)
  {
    res = SYS_TASK_INVALID_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

    SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("IIS2ICLX: warning - MLC SetFifoWM() not supported.\r\n"));
  }

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t iis2iclx_wtm_level = (uint16_t)report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    if (iis2iclx_wtm_level > IIS2ICLX_MAX_WTM_LEVEL)
    {
      iis2iclx_wtm_level = IIS2ICLX_MAX_WTM_LEVEL;
    }
    _this->samples_per_it = iis2iclx_wtm_level;

    /* Set fifo in continuous / stream mode*/
    iis2iclx_fifo_mode_set(p_sensor_drv, IIS2ICLX_STREAM_MODE);

    /* Setup int for FIFO */
    iis2iclx_fifo_watermark_set(p_sensor_drv, _this->samples_per_it);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t IIS2ICLXTaskSensorEnable(IIS2ICLXTask *_this, SMMessage report)
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

static sys_error_code_t IIS2ICLXTaskSensorDisable(IIS2ICLXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    _this->sensor_status.is_active = FALSE;
    iis2iclx_xl_data_rate_set(p_sensor_drv, IIS2ICLX_XL_ODR_OFF);

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

static boolean_t IIS2ICLXTaskSensorIsActive(const IIS2ICLXTask *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.is_active;
}

static sys_error_code_t IIS2ICLXTaskEnterLowPowerMode(const IIS2ICLXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  if (iis2iclx_xl_data_rate_set(p_sensor_drv, IIS2ICLX_XL_ODR_OFF))
  {
    res = SYS_SENSOR_TASK_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_OP_ERROR_CODE);
  }

  return res;
}

static sys_error_code_t IIS2ICLXTaskConfigureIrqPin(const IIS2ICLXTask *_this, boolean_t LowPower)
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

static sys_error_code_t IIS2ICLXTaskConfigureMLCPin(const IIS2ICLXTask *_this, boolean_t LowPower)
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

static inline IIS2ICLXTask *IIS2ICLXTaskGetOwnerFromISensorIF(ISensor_t *p_if)
{
  assert_param(p_if != NULL);
  IIS2ICLXTask *p_if_owner = NULL;

  /* check if the virtual function has been called from the mlc sensor IF  */
  p_if_owner = (IIS2ICLXTask *)((uint32_t) p_if - offsetof(IIS2ICLXTask, mlc_sensor_if));
  if (!(p_if_owner->sensor_if.vptr == &sTheClass.sensor_if_vtbl) || !(p_if_owner->super.vptr == &sTheClass.vtbl))
  {
    /* then the virtual function has been called from the acc IF  */
    p_if_owner = (IIS2ICLXTask *)((uint32_t) p_if - offsetof(IIS2ICLXTask, sensor_if));
  }

  return p_if_owner;
}

static void IIS2ICLXTaskTimerCallbackFunction(ULONG param)
{
  IIS2ICLXTask *p_obj = (IIS2ICLXTask *) param;
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

static void IIS2ICLXTaskMLCTimerCallbackFunction(ULONG param)
{
  IIS2ICLXTask *p_obj = (IIS2ICLXTask *) param;
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

void IIS2ICLXTask_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((IIS2ICLXTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}

void INT2_ICLX_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY_MLC;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((IIS2ICLXTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}

static sys_error_code_t IIS2ICLX_ODR_Sync(IIS2ICLXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  float odr = 0.0f;
  iis2iclx_odr_xl_t iis2iclx_odr_xl;
  if (iis2iclx_xl_data_rate_get(p_sensor_drv, &iis2iclx_odr_xl) == 0)
  {
    _this->sensor_status.is_active = TRUE;

    /* Update only the model */
    switch (iis2iclx_odr_xl)
    {
      case IIS2ICLX_XL_ODR_OFF:
        _this->sensor_status.is_active = FALSE;
        /* Do not update the model in case of odr = 0 */
        odr = _this->sensor_status.type.mems.odr;
        break;
      case IIS2ICLX_XL_ODR_12Hz5:
        odr = 12.5f;
        break;
      case IIS2ICLX_XL_ODR_26Hz:
        odr = 26.0f;
        break;
      case IIS2ICLX_XL_ODR_52Hz:
        odr = 52.0f;
        break;
      case IIS2ICLX_XL_ODR_104Hz:
        odr = 104.0f;
        break;
      case IIS2ICLX_XL_ODR_208Hz:
        odr = 208.0f;
        break;
      case IIS2ICLX_XL_ODR_416Hz:
        odr = 416.0f;
        break;
      case IIS2ICLX_XL_ODR_833Hz:
        odr = 833.0f;
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

static sys_error_code_t IIS2ICLX_FS_Sync(IIS2ICLXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  float fs = 2.0;
  iis2iclx_fs_xl_t fs_xl;
  if (iis2iclx_xl_full_scale_get(p_sensor_drv, &fs_xl) == 0)
  {
    switch (fs_xl)
    {
      case IIS2ICLX_500mg:
        fs = 0.5;
        break;
      case IIS2ICLX_1g:
        fs = 1.0;
        break;
      case IIS2ICLX_2g:
        fs = 2.0;
        break;
      case IIS2ICLX_3g:
        fs = 3.0;
        break;
      default:
        break;
    }
    _this->sensor_status.type.mems.fs = fs;
    if (fs < 3.0f)
    {
      _this->sensor_status.type.mems.sensitivity = 0.0000305f * _this->sensor_status.type.mems.fs;
    }
    else
    {
      _this->sensor_status.type.mems.sensitivity = 0.0000406f * _this->sensor_status.type.mems.fs;
    }
  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }
  return res;
}
