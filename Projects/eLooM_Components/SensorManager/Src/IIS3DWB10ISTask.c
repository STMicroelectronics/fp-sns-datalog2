/**
  ******************************************************************************
  * @file    IIS3DWB10ISTask.c
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
#include "IIS3DWB10ISTask.h"
#include "IIS3DWB10ISTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "iis3dwb10is_reg.h"
#include <string.h>
#include <stdlib.h>
#include "services/sysdebug.h"

/* Private includes ----------------------------------------------------------*/

#ifndef IIS3DWB10IS_TASK_CFG_STACK_DEPTH
#define IIS3DWB10IS_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*8)
#endif

#ifndef IIS3DWB10IS_TASK_CFG_PRIORITY
#define IIS3DWB10IS_TASK_CFG_PRIORITY                 (TX_MAX_PRIORITIES - 1)
#endif

#ifndef IIS3DWB10IS_TASK_CFG_IN_QUEUE_LENGTH
#define IIS3DWB10IS_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define IIS3DWB10IS_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef IIS3DWB10IS_TASK_CFG_TIMER_PERIOD_MS
#define IIS3DWB10IS_TASK_CFG_TIMER_PERIOD_MS          1000
#endif
#ifndef IIS3DWB10IS_TASK_CFG_ISPU_TIMER_PERIOD_MS
#define IIS3DWB10IS_TASK_CFG_ISPU_TIMER_PERIOD_MS      500
#endif

#define IIS3DWB10IS_TAG_ACC                           (0x10)

#ifndef IIS3DWB10IS_TASK_CFG_MAX_INSTANCES_COUNT
#define IIS3DWB10IS_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_IIS3DWB10IS, level, message)

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static int16_t dummyDataCounter_acc = 0;
#endif

/**
  * Class object declaration
  */
typedef struct _IIS3DWB10ISTaskClass
{
  /**
    * IIS3DWB10ISTask class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * Accelerometer IF virtual table.
    */
  const ISensorMems_vtbl acc_sensor_if_vtbl;

  /**
    * Ispu Sensor IF virtual table.
    */
  const ISensorMems_vtbl ispu_sensor_if_vtbl;

  /**
    * Ispu SensorIspu IF virtual table.
    */
  const ISensorLL_vtbl sensor_ll_if_vtbl;

  /**
    * Specifies accelerometer sensor capabilities.
    */
  const SensorDescriptor_t acc_class_descriptor;

  /**
    * Specifies ispu sensor capabilities.
    */
  const SensorDescriptor_t ispu_class_descriptor;

  /**
    * IIS3DWB10ISTask (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
   */
  MTMapElement_t task_map_elements[2 * IIS3DWB10IS_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} IIS3DWB10ISTaskClass_t;

/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS3DWB10ISTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS3DWB10ISTaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS3DWB10ISTaskSensorInit(IIS3DWB10ISTask *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS3DWB10ISTaskSensorReadData(IIS3DWB10ISTask *_this);

/**
  * Read the data from the ispu.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t IIS3DWB10ISTaskSensorReadISPU(IIS3DWB10ISTask *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t IIS3DWB10ISTaskSensorRegister(IIS3DWB10ISTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t IIS3DWB10ISTaskSensorInitTaskParams(IIS3DWB10ISTask *_this);

/**
  * Private implementation of sensor interface methods for IIS3DWB10IS sensor
  */
static sys_error_code_t IIS3DWB10ISTaskSensorSetODR(IIS3DWB10ISTask *_this, SMMessage report);
static sys_error_code_t IIS3DWB10ISTaskSensorSetFS(IIS3DWB10ISTask *_this, SMMessage report);
static sys_error_code_t IIS3DWB10ISTaskSensorSetFifoWM(IIS3DWB10ISTask *_this, SMMessage report);
static sys_error_code_t IIS3DWB10ISTaskSensorEnable(IIS3DWB10ISTask *_this, SMMessage report);
static sys_error_code_t IIS3DWB10ISTaskSensorDisable(IIS3DWB10ISTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t IIS3DWB10ISTaskSensorIsActive(const IIS3DWB10ISTask *_this);

static sys_error_code_t IIS3DWB10ISTaskEnterLowPowerMode(const IIS3DWB10ISTask *_this);

static sys_error_code_t IIS3DWB10ISTaskConfigureIrqPin(const IIS3DWB10ISTask *_this, boolean_t LowPower);
static sys_error_code_t IIS3DWB10ISTaskConfigureISPUPin(const IIS3DWB10ISTask *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void IIS3DWB10ISTaskTimerCallbackFunction(ULONG param);

/**
  * Callback function called when the ispu software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void IIS3DWB10ISTaskISPUTimerCallbackFunction(ULONG param);

/**
  * Given a interface pointer it return the instance of the object that implement the interface.
  *
  * @param p_if [IN] specifies a sensor interface implemented by the task object.
  * @return the instance of the task object that implements the given interface.
  */
static inline IIS3DWB10ISTask *IIS3DWB10ISTaskGetOwnerFromISensorIF(ISensor_t *p_if);

/**
  * Given a interface pointer it return the instance of the object that implement the interface.
  *
  * @param p_if [IN] specifies a ISensorLL interface implemented by the task object.
  * @return the instance of the task object that implements the given interface.
  */
static inline IIS3DWB10ISTask *IIS3DWB10ISTaskGetOwnerFromISensorLLIF(ISensorLL_t *p_if);

/**
  * Read the odr value from the sensor and update the internal model
  */
static sys_error_code_t IIS3DWB10IS_ODR_Sync(IIS3DWB10ISTask *_this);

/**
  * Read the odr value from the sensor and update the internal model
  */
static sys_error_code_t IIS3DWB10IS_FS_Sync(IIS3DWB10ISTask *_this);

static int32_t save_20bit_to_24bit(int32_t x_raw);

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
static inline sys_error_code_t IIS3DWB10ISTaskPostReportToFront(IIS3DWB10ISTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t IIS3DWB10ISTaskPostReportToBack(IIS3DWB10ISTask *_this, SMMessage *pReport);

/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static IIS3DWB10ISTask sTaskObj;
/**
  * The class object.
  */
static IIS3DWB10ISTaskClass_t sTheClass =
{
  /* class virtual table */
  {
    IIS3DWB10ISTask_vtblHardwareInit,
    IIS3DWB10ISTask_vtblOnCreateTask,
    IIS3DWB10ISTask_vtblDoEnterPowerMode,
    IIS3DWB10ISTask_vtblHandleError,
    IIS3DWB10ISTask_vtblOnEnterTaskControlLoop,
    IIS3DWB10ISTask_vtblForceExecuteStep,
    IIS3DWB10ISTask_vtblOnEnterPowerMode
  },

  /* class::acc_sensor_if_vtbl virtual table */
  { { {
        IIS3DWB10ISTask_vtblAccGetId,
        IIS3DWB10ISTask_vtblAccGetEventSourceIF,
        IIS3DWB10ISTask_vtblAccGetDataInfo
      },
      IIS3DWB10ISTask_vtblSensorEnable,
      IIS3DWB10ISTask_vtblSensorDisable,
      IIS3DWB10ISTask_vtblSensorIsEnabled,
      IIS3DWB10ISTask_vtblAccGetDescription,
      IIS3DWB10ISTask_vtblAccGetStatus,
      IIS3DWB10ISTask_vtblAccGetStatusPointer
    },
    IIS3DWB10ISTask_vtblAccGetODR,
    IIS3DWB10ISTask_vtblAccGetFS,
    IIS3DWB10ISTask_vtblAccGetSensitivity,
    IIS3DWB10ISTask_vtblSensorSetODR,
    IIS3DWB10ISTask_vtblSensorSetFS,
    IIS3DWB10ISTask_vtblSensorSetFifoWM
  },

  /* class::ispu_sensor_if_vtbl virtual table */
  { { {
        IIS3DWB10ISTask_vtblIspuGetId,
        IIS3DWB10ISTask_vtblIspuGetEventSourceIF,
        IIS3DWB10ISTask_vtblIspuGetDataInfo
      },
      IIS3DWB10ISTask_vtblSensorEnable,
      IIS3DWB10ISTask_vtblSensorDisable,
      IIS3DWB10ISTask_vtblSensorIsEnabled,
      IIS3DWB10ISTask_vtblIspuGetDescription,
      IIS3DWB10ISTask_vtblIspuGetStatus,
      IIS3DWB10ISTask_vtblIspuGetStatusPointer
    },
    IIS3DWB10ISTask_vtblIspuGetODR,
    IIS3DWB10ISTask_vtblIspuGetFS,
    IIS3DWB10ISTask_vtblIspuGetSensitivity,
    IIS3DWB10ISTask_vtblSensorSetODR,
    IIS3DWB10ISTask_vtblSensorSetFS,
    IIS3DWB10ISTask_vtblSensorSetFifoWM
  },

  /* class::ispu_sensorIspu_if_vtbl virtual table */
  {
    IIS3DWB10ISTask_vtblSensorReadReg,
    IIS3DWB10ISTask_vtblSensorWriteReg,
    IIS3DWB10ISTask_vtblSensorSyncModel
  },

  /* ACCELEROMETER DESCRIPTOR */
  {
    "iis3dwb10is",
    COM_TYPE_ACC
  },

  /* ISPU DESCRIPTOR */
  {
    "iis3dwb10is",
    COM_TYPE_ISPU
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    IIS3DWB10ISTaskExecuteStepState1,
    NULL,
    IIS3DWB10ISTaskExecuteStepDatalog,
  },

  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *IIS3DWB10ISTaskGetAccSensorIF(IIS3DWB10ISTask *_this)
{
  return (ISourceObservable *) & (_this->acc_sensor_if);
}

ISourceObservable *IIS3DWB10ISTaskGetIspuSensorIF(IIS3DWB10ISTask *_this)
{
  return (ISourceObservable *) & (_this->ispu_sensor_if);
}

ISensorLL_t *IIS3DWB10ISTaskGetSensorLLIF(IIS3DWB10ISTask *_this)
{
  return (ISensorLL_t *) & (_this->sensor_ll_if);
}

AManagedTaskEx *IIS3DWB10ISTaskAlloc(const void *pIRQConfig, const void *pISPUConfig, const void *pCSConfig)
{
  IIS3DWB10ISTask *p_new_obj = SysAlloc(sizeof(IIS3DWB10ISTask));

  if (p_new_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_new_obj->super);

    p_new_obj->super.vptr = &sTheClass.vtbl;
    p_new_obj->acc_sensor_if.vptr = &sTheClass.acc_sensor_if_vtbl;
    p_new_obj->ispu_sensor_if.vptr = &sTheClass.ispu_sensor_if_vtbl;
    p_new_obj->sensor_ll_if.vptr = &sTheClass.sensor_ll_if_vtbl;
    p_new_obj->acc_sensor_descriptor = &sTheClass.acc_class_descriptor;
    p_new_obj->ispu_sensor_descriptor = &sTheClass.ispu_class_descriptor;

    p_new_obj->p_irq_config = (MX_GPIOParams_t *) pIRQConfig;
    p_new_obj->p_ispu_config = (MX_GPIOParams_t *) pISPUConfig;
    p_new_obj->p_cs_config = (MX_GPIOParams_t *) pCSConfig;

    strcpy(p_new_obj->acc_sensor_status.p_name, sTheClass.acc_class_descriptor.p_name);
    strcpy(p_new_obj->ispu_sensor_status.p_name, sTheClass.ispu_class_descriptor.p_name);
  }

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *IIS3DWB10ISTaskAllocSetName(const void *pIRQConfig, const void *pISPUConfig, const void *pCSConfig,
                                            const char *p_name)
{
  IIS3DWB10ISTask *p_new_obj = (IIS3DWB10ISTask *) IIS3DWB10ISTaskAlloc(pIRQConfig, pISPUConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->acc_sensor_status.p_name, p_name);
  strcpy(p_new_obj->ispu_sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *IIS3DWB10ISTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pISPUConfig,
                                           const void *pCSConfig)
{
  IIS3DWB10ISTask *p_obj = (IIS3DWB10ISTask *) p_mem_block;

  if (p_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_obj->super);

    p_obj->super.vptr = &sTheClass.vtbl;
    p_obj->acc_sensor_if.vptr = &sTheClass.acc_sensor_if_vtbl;
    p_obj->ispu_sensor_if.vptr = &sTheClass.ispu_sensor_if_vtbl;
    p_obj->sensor_ll_if.vptr = &sTheClass.sensor_ll_if_vtbl;
    p_obj->acc_sensor_descriptor = &sTheClass.acc_class_descriptor;
    p_obj->ispu_sensor_descriptor = &sTheClass.ispu_class_descriptor;

    p_obj->p_irq_config = (MX_GPIOParams_t *) pIRQConfig;
    p_obj->p_ispu_config = (MX_GPIOParams_t *) pISPUConfig;
    p_obj->p_cs_config = (MX_GPIOParams_t *) pCSConfig;
  }

  return (AManagedTaskEx *)p_obj;
}

AManagedTaskEx *IIS3DWB10ISTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pISPUConfig,
                                                  const void *pCSConfig, const char *p_name)
{
  IIS3DWB10ISTask *p_obj = (IIS3DWB10ISTask *) IIS3DWB10ISTaskStaticAlloc(p_mem_block, pIRQConfig, pISPUConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->acc_sensor_status.p_name, p_name);
  strcpy(p_obj->ispu_sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *IIS3DWB10ISTaskGetSensorIF(IIS3DWB10ISTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *IIS3DWB10ISTaskGetAccEventSrcIF(IIS3DWB10ISTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_acc_event_src;
}

IEventSrc *IIS3DWB10ISTaskGetMlcEventSrcIF(IIS3DWB10ISTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *)_this->p_ispu_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t IIS3DWB10ISTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWB10ISTask *p_obj = (IIS3DWB10ISTask *) _this;

  /* Configure CS Pin */
  if (p_obj->p_cs_config != NULL)
  {
    p_obj->p_cs_config->p_mx_init_f();
  }

  return res;
}

sys_error_code_t IIS3DWB10ISTask_vtblOnCreateTask(
  AManagedTask *_this,
  tx_entry_function_t *pTaskCode,
  CHAR **pName,
  VOID **pvStackStart, ULONG *pStackDepth,
  UINT *pPriority,
  UINT *pPreemptThreshold,
  ULONG *pTimeSlice,
  ULONG *pAutoStart,
  ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWB10ISTask *p_obj = (IIS3DWB10ISTask *) _this;

  p_obj->sync = true;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) IIS3DWB10IS_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(IIS3DWB10IS_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "IIS3DWB10IS_Q", item_size / 4u, p_queue_items_buff,
                                    IIS3DWB10IS_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_timer, "IIS3DWB10IS_T", IIS3DWB10ISTaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(IIS3DWB10IS_TASK_CFG_TIMER_PERIOD_MS),
                         0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* create the ispu software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->ispu_timer, "IIS3DWB10IS_ISPU_T", IIS3DWB10ISTaskISPUTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(IIS3DWB10IS_TASK_CFG_ISPU_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->p_cs_config != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(IIS3DWB10IS_ID, p_obj->p_cs_config->port, (uint16_t) p_obj->p_cs_config->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    res = SYS_TASK_INVALID_PARAM_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
  }

  if (SYS_IS_ERROR_CODE(res))
  {
    return res;
  }
  /* Initialize the EventSrc interface */
  p_obj->p_acc_event_src = DataEventSrcAlloc();
  if (p_obj->p_acc_event_src == NULL)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }
  IEventSrcInit(p_obj->p_acc_event_src);

  p_obj->p_ispu_event_src = DataEventSrcAlloc();
  if (p_obj->p_ispu_event_src == NULL)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }
  IEventSrcInit(p_obj->p_ispu_event_src);

  if (!MTMap_IsInitialized(&sTheClass.task_map))
  {
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, 2 * IIS3DWB10IS_TASK_CFG_MAX_INSTANCES_COUNT);
  }

  /* Add the managed task to the map.*/
  if (p_obj->p_irq_config != NULL)
  {
    /* Use the PIN as unique key for the map. */
    MTMapElement_t *p_element = NULL;
    uint32_t key = (uint32_t) p_obj->p_irq_config->pin;
    p_element = MTMap_AddElement(&sTheClass.task_map, key, _this);
    if (p_element == NULL)
    {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      return res;
    }
  }

  /* Add the managed task to the map.*/
  if (p_obj->p_ispu_config != NULL)
  {
    /* Use the PIN as unique key for the map. */
    MTMapElement_t *p_element = NULL;
    uint32_t key = (uint32_t) p_obj->p_ispu_config->pin;
    p_element = MTMap_AddElement(&sTheClass.task_map, key, _this);
    if (p_element == NULL)
    {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      return res;
    }
  }

#if IIS3DWB10IS_FIFO_ENABLED
  memset(p_obj->p_sensor_data_buff, 0, sizeof(p_obj->p_sensor_data_buff));
  memset(p_obj->p_acc_sensor_data_buff, 0, sizeof(p_obj->p_acc_sensor_data_buff));
#else
  memset(p_obj->p_acc_sample, 0, sizeof(p_obj->p_acc_sample));
  p_obj->acc_drdy = 0;
#endif

  memset(p_obj->p_ispu_output_buff, 0, sizeof(p_obj->p_ispu_output_buff));
  p_obj->acc_id = 0;
  p_obj->ispu_enable = FALSE;
  p_obj->acc_prev_timestamp = 0.0f;
  p_obj->acc_samples_count = 0;
  p_obj->fifo_level = 0;
  p_obj->samples_per_it = 0;
  p_obj->first_data_ready = 0;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "IIS3DWB10IS";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = IIS3DWB10IS_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = IIS3DWB10IS_TASK_CFG_PRIORITY;
  *pPreemptThreshold = IIS3DWB10IS_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = IIS3DWB10ISTaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = IIS3DWB10ISTaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB10IS: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t IIS3DWB10ISTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                      const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWB10ISTask *p_obj = (IIS3DWB10ISTask *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (IIS3DWB10ISTaskSensorIsActive(p_obj))
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
      p_obj->acc_prev_timestamp = 0.0f;
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB10IS: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      if (IIS3DWB10ISTaskSensorIsActive(p_obj))
      {
        /* Deactivate the sensor */
        iis3dwb10is_data_rate_t iis3dwb10is_xl_data_rate;
        iis3dwb10is_xl_data_rate.odr = IIS3DWB10IS_ODR_IDLE;

        iis3dwb10is_xl_data_rate_set(p_sensor_drv, iis3dwb10is_xl_data_rate);
        iis3dwb10is_fifo_mode_set(p_sensor_drv, IIS3DWB10IS_BYPASS_MODE);
      }

      p_obj->first_data_ready = 0;
      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
    }
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB10IS: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = IIS3DWB10ISTaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB10IS - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->p_irq_config != NULL)
    {
      IIS3DWB10ISTaskConfigureIrqPin(p_obj, TRUE);
    }
    if (p_obj->p_ispu_config != NULL)
    {
      IIS3DWB10ISTaskConfigureISPUPin(p_obj, TRUE);
    }
    /* notify the bus */
    if (p_obj->p_sensor_bus_if->m_pfBusCtrl != NULL)
    {
      p_obj->p_sensor_bus_if->m_pfBusCtrl(p_obj->p_sensor_bus_if, E_BUS_CTRL_DEV_NOTIFY_POWER_MODE, 0);
    }
    if (p_obj->p_irq_config == NULL)
    {
      tx_timer_deactivate(&p_obj->read_timer);
    }
    if (p_obj->p_ispu_config == NULL)
    {
      tx_timer_deactivate(&p_obj->ispu_timer);
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB10IS: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t IIS3DWB10ISTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  IIS3DWB10ISTask *p_obj = (IIS3DWB10ISTask*)_this;

  return res;
}

sys_error_code_t IIS3DWB10ISTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("IIS3DWB10IS: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (IIS3DWB10IS_TASK_CFG_TAG)
  IIS3DWB10ISTask *p_obj = (IIS3DWB10ISTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = IIS3DWB10IS_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t IIS3DWB10ISTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWB10ISTask *p_obj = (IIS3DWB10ISTask *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = IIS3DWB10ISTaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t IIS3DWB10ISTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                      const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWB10ISTask *p_obj = (IIS3DWB10ISTask *) _this;

  if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      if (p_obj->p_irq_config == NULL)
      {
        tx_timer_deactivate(&p_obj->read_timer);
      }
      else
      {
        IIS3DWB10ISTaskConfigureIrqPin(p_obj, TRUE);
      }
      if (p_obj->p_ispu_config == NULL)
      {
        tx_timer_deactivate(&p_obj->ispu_timer);
      }
      else
      {
        IIS3DWB10ISTaskConfigureISPUPin(p_obj, TRUE);
      }
    }
  }
  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t IIS3DWB10ISTask_vtblAccGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS3DWB10ISTask *p_if_owner = (IIS3DWB10ISTask *)((uint32_t) _this - offsetof(IIS3DWB10ISTask, acc_sensor_if));
  uint8_t res = p_if_owner->acc_id;

  return res;
}

uint8_t IIS3DWB10ISTask_vtblIspuGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS3DWB10ISTask *p_if_owner = (IIS3DWB10ISTask *)((uint32_t) _this - offsetof(IIS3DWB10ISTask, ispu_sensor_if));
  uint8_t res = p_if_owner->ispu_id;

  return res;
}

IEventSrc *IIS3DWB10ISTask_vtblAccGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS3DWB10ISTask *p_if_owner = (IIS3DWB10ISTask *)((uint32_t) _this - offsetof(IIS3DWB10ISTask, acc_sensor_if));

  return p_if_owner->p_acc_event_src;
}

IEventSrc *IIS3DWB10ISTask_vtblIspuGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS3DWB10ISTask *p_if_owner = (IIS3DWB10ISTask *)((uint32_t) _this - offsetof(IIS3DWB10ISTask, ispu_sensor_if));
  return p_if_owner->p_ispu_event_src;
}

sys_error_code_t IIS3DWB10ISTask_vtblAccGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  IIS3DWB10ISTask *p_if_owner = (IIS3DWB10ISTask *)((uint32_t) _this - offsetof(IIS3DWB10ISTask, acc_sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if ((p_measured == NULL) || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = p_if_owner->acc_sensor_status.type.mems.measured_odr;
    *p_nominal = p_if_owner->acc_sensor_status.type.mems.odr;
  }

  return res;
}

float_t IIS3DWB10ISTask_vtblAccGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  IIS3DWB10ISTask *p_if_owner = (IIS3DWB10ISTask *)((uint32_t) _this - offsetof(IIS3DWB10ISTask, acc_sensor_if));
  float_t res = p_if_owner->acc_sensor_status.type.mems.fs;

  return res;
}

float_t IIS3DWB10ISTask_vtblAccGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  IIS3DWB10ISTask *p_if_owner = (IIS3DWB10ISTask *)((uint32_t) _this - offsetof(IIS3DWB10ISTask, acc_sensor_if));
  float_t res = p_if_owner->acc_sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t IIS3DWB10ISTask_vtblAccGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS3DWB10ISTask *p_if_owner = (IIS3DWB10ISTask *)((uint32_t)_this - offsetof(IIS3DWB10ISTask, acc_sensor_if));
  EMData_t res = p_if_owner->data_acc;

  return res;
}

sys_error_code_t IIS3DWB10ISTask_vtblIspuGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  IIS3DWB10ISTask *p_if_owner = (IIS3DWB10ISTask *)((uint32_t)_this - offsetof(IIS3DWB10ISTask, ispu_sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if ((p_measured == NULL) || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = p_if_owner->ispu_sensor_status.type.mems.measured_odr;
    *p_nominal = p_if_owner->ispu_sensor_status.type.mems.odr;
  }

  return res;
}

float_t IIS3DWB10ISTask_vtblIspuGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  /* ISPU does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB10IS: warning - ISPU GetFS() not supported.\r\n"));
  return SYS_INVALID_FUNC_CALL_ERROR_CODE;
}

float_t IIS3DWB10ISTask_vtblIspuGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);

  /* ISPU does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB10ISPU: warning - ISPU GetSensitivity() not supported.\r\n"));

  return -1.0f;
}

EMData_t IIS3DWB10ISTask_vtblIspuGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  IIS3DWB10ISTask *p_if_owner = (IIS3DWB10ISTask *)((uint32_t)_this - offsetof(IIS3DWB10ISTask, ispu_sensor_if));
  EMData_t res = p_if_owner->data_ispu;

  return res;
}

sys_error_code_t IIS3DWB10ISTask_vtblSensorSetODR(ISensorMems_t *_this, float_t odr)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWB10ISTask *p_if_owner = IIS3DWB10ISTaskGetOwnerFromISensorIF((ISensor_t *)_this);

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
        p_if_owner->acc_sensor_status.type.mems.odr = odr;
        p_if_owner->acc_sensor_status.type.mems.measured_odr = 0.0f;
      }
      else if (sensor_id == p_if_owner->ispu_id)
      {
        p_if_owner->ispu_sensor_status.type.mems.odr = odr;
        p_if_owner->ispu_sensor_status.type.mems.measured_odr = 0.0f;
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
      .sensorMessage.fParam = (float_t) odr
    };
    res = IIS3DWB10ISTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t IIS3DWB10ISTask_vtblSensorSetFS(ISensorMems_t *_this, float_t fs)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWB10ISTask *p_if_owner = IIS3DWB10ISTaskGetOwnerFromISensorIF((ISensor_t *)_this);

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
      p_if_owner->acc_sensor_status.type.mems.fs = fs;
      p_if_owner->acc_sensor_status.type.mems.sensitivity = 0.0000019075f * p_if_owner->acc_sensor_status.type.mems.fs;
    }
    else if (sensor_id == p_if_owner->ispu_id)
    {
      p_if_owner->ispu_sensor_status.type.mems.fs = fs;
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
      .sensorMessage.fParam = (float_t) fs
    };
    res = IIS3DWB10ISTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t IIS3DWB10ISTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWB10ISTask *p_if_owner = IIS3DWB10ISTaskGetOwnerFromISensorIF((ISensor_t *)_this);

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
    res = IIS3DWB10ISTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t IIS3DWB10ISTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWB10ISTask *p_if_owner = IIS3DWB10ISTaskGetOwnerFromISensorIF(_this);

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
      p_if_owner->acc_sensor_status.is_active = TRUE;
    }
    else if (sensor_id == p_if_owner->ispu_id)
    {
      p_if_owner->ispu_sensor_status.is_active = TRUE;
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
    res = IIS3DWB10ISTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t IIS3DWB10ISTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWB10ISTask *p_if_owner = IIS3DWB10ISTaskGetOwnerFromISensorIF(_this);

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
      p_if_owner->acc_sensor_status.is_active = FALSE;
    }
    else if (sensor_id == p_if_owner->ispu_id)
    {
      p_if_owner->ispu_sensor_status.is_active = FALSE;
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
    res = IIS3DWB10ISTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t IIS3DWB10ISTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  IIS3DWB10ISTask *p_if_owner = IIS3DWB10ISTaskGetOwnerFromISensorIF(_this);

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->acc_id)
  {
    res = p_if_owner->acc_sensor_status.is_active;
  }

  else if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->ispu_id)
  {
    res = p_if_owner->ispu_sensor_status.is_active;
  }

  return res;
}

SensorDescriptor_t IIS3DWB10ISTask_vtblAccGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS3DWB10ISTask *p_if_owner = IIS3DWB10ISTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->acc_sensor_descriptor;
}

SensorDescriptor_t IIS3DWB10ISTask_vtblIspuGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS3DWB10ISTask *p_if_owner = IIS3DWB10ISTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->ispu_sensor_descriptor;
}

SensorStatus_t IIS3DWB10ISTask_vtblAccGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS3DWB10ISTask *p_if_owner = IIS3DWB10ISTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->acc_sensor_status;
}

SensorStatus_t IIS3DWB10ISTask_vtblIspuGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS3DWB10ISTask *p_if_owner = IIS3DWB10ISTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->ispu_sensor_status;
}

SensorStatus_t *IIS3DWB10ISTask_vtblAccGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS3DWB10ISTask *p_if_owner = IIS3DWB10ISTaskGetOwnerFromISensorIF(_this);
  return &p_if_owner->acc_sensor_status;
}

SensorStatus_t *IIS3DWB10ISTask_vtblIspuGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  IIS3DWB10ISTask *p_if_owner = IIS3DWB10ISTaskGetOwnerFromISensorIF(_this);
  return &p_if_owner->ispu_sensor_status;
}

sys_error_code_t IIS3DWB10ISTask_vtblSensorReadReg(ISensorLL_t *_this, uint16_t reg, uint8_t *data, uint16_t len)
{
  assert_param(_this != NULL);
  assert_param(reg <= 0xFFU);
  assert_param(data != NULL);
  assert_param(len != 0U);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWB10ISTask *p_if_owner = IIS3DWB10ISTaskGetOwnerFromISensorLLIF(_this);
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_if_owner->p_sensor_bus_if->m_xConnector;
  uint8_t reg8 = (uint8_t)(reg & 0x00FF);

  if (iis3dwb10is_read_reg(p_sensor_drv, reg8, data, len) != 0)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t IIS3DWB10ISTask_vtblSensorWriteReg(ISensorLL_t *_this, uint16_t reg, const uint8_t *data, uint16_t len)
{
  assert_param(_this != NULL);
  assert_param(reg <= 0xFFU);
  assert_param(data != NULL);
  assert_param(len != 0U);

  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWB10ISTask *p_if_owner = IIS3DWB10ISTaskGetOwnerFromISensorLLIF(_this);
  uint8_t reg8 = (uint8_t)(reg & 0x00FF);

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_if_owner->p_sensor_bus_if->m_xConnector;

  /* This generic register write operation could mean that the model is out of sync with the HW */
  p_if_owner->sync = false;

  if (iis3dwb10is_write_reg(p_sensor_drv, reg8, (uint8_t *) data, len) != 0)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t IIS3DWB10ISTask_vtblSensorSyncModel(ISensorLL_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWB10ISTask *p_if_owner = IIS3DWB10ISTaskGetOwnerFromISensorLLIF(_this);

  if (IIS3DWB10IS_ODR_Sync(p_if_owner) != SYS_NO_ERROR_CODE)
  {
    res = SYS_BASE_ERROR_CODE;
  }
  if (IIS3DWB10IS_FS_Sync(p_if_owner) != SYS_NO_ERROR_CODE)
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
static sys_error_code_t IIS3DWB10ISTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWB10ISTask *p_obj = (IIS3DWB10ISTask *) _this;
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
            res = IIS3DWB10ISTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = IIS3DWB10ISTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = IIS3DWB10ISTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = IIS3DWB10ISTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = IIS3DWB10ISTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB10IS: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB10IS: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t IIS3DWB10ISTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  IIS3DWB10ISTask *p_obj = (IIS3DWB10ISTask *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("IIS3DWB10IS: new data.\r\n"));

        res = IIS3DWB10ISTaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          if (p_obj->first_data_ready == 1)
          {
#if IIS3DWB10IS_FIFO_ENABLED
            if (p_obj->fifo_level != 0)
            {
#endif
              // notify the listeners...
              double_t timestamp = report.sensorDataReadyMessage.fTimestamp;

              /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue:
               * [X0, Y0, Z0]
               * [X1, Y1, Z1]
               * ...
               * [Xm-1, Ym-1, Zm-1]
               */
              DataEvent_t evt_acc;
              double_t delta_timestamp = timestamp - p_obj->acc_prev_timestamp;
              p_obj->acc_prev_timestamp = timestamp;

#if IIS3DWB10IS_FIFO_ENABLED

              if (p_obj->acc_sensor_status.is_active)
              {
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
                  p_obj->acc_sensor_status.type.mems.measured_odr = (float_t) p_obj->samples_sum / p_obj->delta_timestamp_sum;
                  p_obj->delta_timestamp_sum = 0.0f;
                  p_obj->samples_sum = 0;
                  p_obj->odr_count = 0;
                }

                EMD_Init(&p_obj->data_acc, p_obj->p_acc_sensor_data_buff, E_EM_INT24, E_EM_MODE_INTERLEAVED, 2,
                         p_obj->acc_samples_count, 3);
                DataEventInit((IEvent *) &evt_acc, p_obj->p_acc_event_src, &p_obj->data_acc, timestamp, p_obj->acc_id);

                IEventSrcSendEvent(p_obj->p_acc_event_src, (IEvent *) &evt_acc, NULL);
              }

#else
              if (p_obj->acc_sensor_status.is_active && p_obj->acc_drdy)
              {
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
                  p_obj->acc_sensor_status.type.mems.measured_odr = (float_t) p_obj->samples_sum / p_obj->delta_timestamp_sum;
                  p_obj->delta_timestamp_sum = 0.0f;
                  p_obj->samples_sum = 0;
                  p_obj->odr_count = 0;
                }

                EMD_Init(&p_obj->data_acc, p_obj->p_acc_sample, E_EM_INT32, E_EM_MODE_INTERLEAVED, 2, p_obj->acc_samples_count, 3);
                DataEventInit((IEvent *) &evt_acc, p_obj->p_acc_event_src, &p_obj->data_acc, timestamp, p_obj->acc_id);

                IEventSrcSendEvent(p_obj->p_acc_event_src, (IEvent *) &evt_acc, NULL);
                p_obj->acc_drdy = 0;
              }
#endif
              SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("IIS3DWB10IS: ts = %f\r\n", (float_t)timestamp));
#if IIS3DWB10IS_FIFO_ENABLED
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

      case SM_MESSAGE_ID_DATA_READY_ISPU:
      {
        res = IIS3DWB10ISTaskSensorReadISPU(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          // notify the listeners...
          double_t timestamp = report.sensorDataReadyMessage.fTimestamp;

          if (p_obj->ispu_enable)
          {
            EMD_Init(&p_obj->data_ispu, (uint8_t *) p_obj->p_ispu_output_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 32);

            DataEvent_t evt;

            DataEventInit((IEvent *)&evt, p_obj->p_ispu_event_src, &p_obj->data_ispu, timestamp, p_obj->ispu_id);
            IEventSrcSendEvent(p_obj->p_ispu_event_src, (IEvent *) &evt, NULL);
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
            res = IIS3DWB10ISTaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->acc_sensor_status.is_active == true)
              {
                if (p_obj->p_irq_config == NULL)
                {
                  if (TX_SUCCESS
                      != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->iis3dwb10is_task_cfg_timer_period_ms),
                                         AMT_MS_TO_TICKS(p_obj->iis3dwb10is_task_cfg_timer_period_ms)))
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
                  IIS3DWB10ISTaskConfigureIrqPin(p_obj, FALSE);
                }
              }
            }
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->ispu_sensor_status.is_active == true)
              {
                if (p_obj->p_ispu_config == NULL)
                {
                  if (TX_SUCCESS != tx_timer_change(&p_obj->ispu_timer, AMT_MS_TO_TICKS(IIS3DWB10IS_TASK_CFG_ISPU_TIMER_PERIOD_MS),
                                                    AMT_MS_TO_TICKS(IIS3DWB10IS_TASK_CFG_ISPU_TIMER_PERIOD_MS)))
                  {
                    res = SYS_UNDEFINED_ERROR_CODE;
                  }

                  if (TX_SUCCESS != tx_timer_activate(&p_obj->ispu_timer))
                  {
                    res = SYS_UNDEFINED_ERROR_CODE;
                  }
                }
                else
                {
                  IIS3DWB10ISTaskConfigureISPUPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = IIS3DWB10ISTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = IIS3DWB10ISTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = IIS3DWB10ISTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = IIS3DWB10ISTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = IIS3DWB10ISTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB10IS: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }

      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB10IS: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t IIS3DWB10ISTaskPostReportToFront(IIS3DWB10ISTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t IIS3DWB10ISTaskPostReportToBack(IIS3DWB10ISTask *_this, SMMessage *pReport)
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

static sys_error_code_t IIS3DWB10ISTaskSensorInit(IIS3DWB10ISTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  int32_t ret_val = 0;
  uint8_t reg0 = 0;
  iis3dwb10is_reset_t rst;
  iis3dwb10is_xl_data_cfg_t xl_cfg;
  iis3dwb10is_data_rate_t iis3dwb10is_xl_data_rate;
  iis3dwb10is_xl_data_rate.odr = IIS3DWB10IS_ODR_IDLE;

  /* Check device ID */
  ret_val = iis3dwb10is_device_id_get(p_sensor_drv, (uint8_t *) &reg0);
  if (!ret_val)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, reg0);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB10IS: sensor - I am 0x%x.\r\n", reg0));

  /* Restore default configuration */
  rst.boot = 1;
  rst.sw_rst = 1;
  ret_val = iis3dwb10is_reset_set(p_sensor_drv, rst);
  do
  {
    iis3dwb10is_reset_get(p_sensor_drv, &rst);
  } while (rst.sw_rst);

  /* Enable Block Data Update */
  iis3dwb10is_block_data_update_set(p_sensor_drv, PROPERTY_ENABLE);

  iis3dwb10is_xl_data_config_get(p_sensor_drv, &xl_cfg);
  xl_cfg.rounding = IIS3DWB10IS_WRAPAROUND_DISABLED;
  iis3dwb10is_xl_data_config_set(p_sensor_drv, xl_cfg);

  /* Set full scale */
  if (_this->acc_sensor_status.type.mems.fs < 51.0f)
  {
    iis3dwb10is_xl_full_scale_set(p_sensor_drv, IIS3DWB_50g);
  }
  else if (_this->acc_sensor_status.type.mems.fs < 101.0f)
  {
    iis3dwb10is_xl_full_scale_set(p_sensor_drv, IIS3DWB_100g);
  }
  else
  {
    iis3dwb10is_xl_full_scale_set(p_sensor_drv, IIS3DWB_200g);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB10IS: sensor FS - %.1f \r\n", _this->acc_sensor_status.type.mems.fs));


  /* Set Output Data Rate */
  iis3dwb10is_xl_data_rate.burst = IIS3DWB10IS_CONTINUOS_MODE;
  if (_this->acc_sensor_status.type.mems.odr < 2600.0f)
  {
    iis3dwb10is_xl_data_rate.odr = IIS3DWB10IS_ODR_2KHz5;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 5100.0f)
  {
    iis3dwb10is_xl_data_rate.odr = IIS3DWB10IS_ODR_5KHz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 10100.0f)
  {
    iis3dwb10is_xl_data_rate.odr = IIS3DWB10IS_ODR_10KHz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 20100.0f)
  {
    iis3dwb10is_xl_data_rate.odr = IIS3DWB10IS_ODR_20KHz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 40100.0f)
  {
    iis3dwb10is_xl_data_rate.odr = IIS3DWB10IS_ODR_40KHz;
  }
  else
  {
    iis3dwb10is_xl_data_rate.odr = IIS3DWB10IS_ODR_80KHz;
  }

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB10IS: sensor ODR - %.1f \r\n", _this->acc_sensor_status.type.mems.odr));

  iis3dwb10is_pin_int_route_t int_route = {0};

#if IIS3DWB10IS_FIFO_ENABLED
  uint16_t iis3dwb10is_wtm_level = 0;
  iis3dwb10is_fifo_sensor_batch_t fifo_batch;

  if (_this->samples_per_it == 0)
  {
    /* Calculation of watermark and samples per int*/
    iis3dwb10is_wtm_level = ((uint16_t) _this->acc_sensor_status.type.mems.odr * (uint16_t) IIS3DWB10IS_MAX_DRDY_PERIOD);
    if (iis3dwb10is_wtm_level > IIS3DWB10IS_MAX_WTM_LEVEL)
    {
      iis3dwb10is_wtm_level = IIS3DWB10IS_MAX_WTM_LEVEL;
    }
    _this->samples_per_it = iis3dwb10is_wtm_level;
  }

  /*
   * Set FIFO watermark (number of unread sensor data TAG + 9 bytes
   * stored in FIFO) to FIFO_WATERMARK samples
   */
  iis3dwb10is_fifo_watermark_set(p_sensor_drv, _this->samples_per_it);
  iis3dwb10is_fifo_stop_on_wtm_set(p_sensor_drv, 0);

  iis3dwb10is_fifo_batch_get(p_sensor_drv, &fifo_batch);
  fifo_batch.batch_xl = 1;
  fifo_batch.batch_temp = 0;
  fifo_batch.batch_ts = IIS3DWB10IS_TMSTMP_NOT_BATCHED;
  fifo_batch.batch_qvar = 0;
  fifo_batch.batch_ispu = 0;
  iis3dwb10is_fifo_batch_set(p_sensor_drv, fifo_batch);

  /* Set FIFO mode to Stream mode (aka Continuous Mode) */
  iis3dwb10is_fifo_mode_set(p_sensor_drv, IIS3DWB10IS_STREAM_MODE);

  /* Enable Block Data Update */
  iis3dwb10is_block_data_update_set(p_sensor_drv, PROPERTY_ENABLE);

  if (_this->p_irq_config != NULL)
  {
    int_route.int1_fifo_th = PROPERTY_ENABLE;
  }
  else
  {
    int_route.int1_fifo_th = PROPERTY_DISABLE;
  }

  iis3dwb10is_pin_int_route_set(p_sensor_drv, int_route);

#else

  uint8_t buff[12];
  iis3dwb10is_read_reg(p_sensor_drv, IIS3DWB10IS_OUTX_L_A, &buff[0], 12);

  _this->samples_per_it = 1;
  if (_this->p_irq_config != NULL)
  {
    if (_this->acc_sensor_status.is_active)
    {
      int_route.int1_drdy_xl = PROPERTY_ENABLE;
    }
  }
  else
  {
    int_route.int1_drdy_xl = PROPERTY_DISABLE;
  }
  iis3dwb10is_pin_int_route_set(p_sensor_drv, int_route);

#endif /* IIS3DWB10IS_FIFO_ENABLED */

  if (_this->acc_sensor_status.is_active == false)
  {
    iis3dwb10is_xl_data_rate.odr = IIS3DWB10IS_ODR_IDLE;
    _this->acc_sensor_status.is_active = false;
  }
  iis3dwb10is_xl_data_rate_set(p_sensor_drv, iis3dwb10is_xl_data_rate);

#if IIS3DWB10IS_FIFO_ENABLED
  iis3dwb10is_fifo_status_t fifo_status;

  /* Read watermark flag */
  iis3dwb10is_fifo_status_get(p_sensor_drv, &fifo_status);
  _this->fifo_level =  fifo_status.fifo_level;

  if ((fifo_status.fifo_th) && (_this->fifo_level >= _this->samples_per_it))
  {
    iis3dwb10is_fifo_out_raw_get(p_sensor_drv, _this->p_sensor_data_buff, _this->samples_per_it);
  }

#endif

#if IIS3DWB10IS_FIFO_ENABLED
  _this->iis3dwb10is_task_cfg_timer_period_ms = (uint16_t)((1000.0f / _this->acc_sensor_status.type.mems.odr) * (((float_t)(_this->samples_per_it)) / 2.0f));
#else
  _this->iis3dwb10is_task_cfg_timer_period_ms = (uint16_t)(1000.0f / _this->acc_sensor_status.type.mems.odr);
#endif

  _this->odr_count = 0;
  _this->delta_timestamp_sum = 0.0f;
  _this->samples_sum = 0;
  return res;
}

int32_t flag = 0;
static sys_error_code_t IIS3DWB10ISTaskSensorReadData(IIS3DWB10ISTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t samples_per_it = _this->samples_per_it;

#if IIS3DWB10IS_FIFO_ENABLED
  uint16_t i;
  iis3dwb10is_fifo_status_t fifo_status;

  /* Read watermark flag */
  iis3dwb10is_fifo_status_get(p_sensor_drv, &fifo_status);
  _this->fifo_level =  fifo_status.fifo_level;

  if ((fifo_status.fifo_th) && (_this->fifo_level >= samples_per_it))
  {
    uint8_t *p_acc = _this->p_acc_sensor_data_buff;
    _this->acc_samples_count = 0;
    iis3dwb10is_fifo_out_raw_t val;

    iis3dwb10is_fifo_out_raw_get(p_sensor_drv, _this->p_sensor_data_buff, samples_per_it);
    iis3dwb10is_fifo_process(_this->p_sensor_data_buff, &val);

    for (i = 0; i < samples_per_it; i++)
    {
      uint8_t *data_ptr = &_this->p_sensor_data_buff[i * FIFO_ROW_LEN];
      if (data_ptr[0] == IIS3DWB10IS_TAG_XL)
      {
#if (HSD_USE_DUMMY_DATA == 1)
        for (int axis = 0; axis < 3; axis++)
        {
          // Create a 24-bit dummy counter
          int32_t dummy_value = (int32_t)dummyDataCounter_acc++;
          dummy_value = save_20bit_to_24bit(dummy_value); // Convert to 24-bit
          memcpy(p_acc, &dummy_value, 3); // Copy 3 bytes (24 bits)
          p_acc += 3;
        }
#else
        // Extract the 20-bit x_raw, y_raw, and z_raw values into temporary variables
        int32_t x_raw = (val.xl.x_raw); // x_raw is a 20-bit signed integer
        int32_t y_raw = (val.xl.y_raw); // y_raw is a 20-bit signed integer
        int32_t z_raw = (val.xl.z_raw); // z_raw is a 20-bit signed integer

        x_raw = save_20bit_to_24bit(x_raw); // Convert to 24-bit
        y_raw = save_20bit_to_24bit(y_raw);
        z_raw = save_20bit_to_24bit(z_raw);

        // Copy into the 24-bit destination buffer
        memcpy(p_acc, &x_raw, 3); // Copy 3 bytes (24 bits) for x_raw
        p_acc += 3;
        memcpy(p_acc, &y_raw, 3); // Copy 3 bytes (24 bits) for y_raw
        p_acc += 3;
        memcpy(p_acc, &z_raw, 3); // Copy 3 bytes (24 bits) for z_raw
        p_acc += 3;
#endif
        _this->acc_samples_count++;
      }
    }
  }

  else
  {
    _this->fifo_level = 0;
    res = SYS_BASE_ERROR_CODE;
  }
#else
  if (_this->acc_sensor_status.is_active)
  {
    iis3dwb10is_read_reg(p_sensor_drv, IIS3DWB10IS_OUTX_L_A, _this->p_acc_sample, samples_per_it * 12);
#if (HSD_USE_DUMMY_DATA == 1)
    uint16_t i = 0;
    int16_t *p16 = (int16_t *)(_this->p_acc_sample);
    for (i = 0; i < samples_per_it * 3; i++)
    {
      *p16++ = dummyDataCounter_acc++;
    }
#endif
    _this->acc_samples_count = 1U;
    _this->acc_drdy = 1;
  }
  _this->fifo_level = 1;
#endif /* IIS3DWB10IS_FIFO_ENABLED */

  return res;
}

static int32_t save_20bit_to_24bit(int32_t x_raw)
{
  // Ensure x_raw is treated as a signed 20-bit value
  if (x_raw & 0x80000)
  {
    // Check the 20th bit (sign bit)
    x_raw |= 0xFFF00000; // Sign-extend to 32 bits
  }
  else
  {
    x_raw &= 0x000FFFFF; // Mask to ensure only 20 bits are used
  }

  // Return the 24-bit value (stored in a 32-bit container)
  return x_raw & 0xFFFFFF; // Mask to 24 bits
}

static sys_error_code_t IIS3DWB10ISTaskSensorReadISPU(IIS3DWB10ISTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
//  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
//
//  if (_this->ispu_enable)
//  {
//    iis3dwb10is_mem_bank_set(p_sensor_drv, IIS3DWB10IS_ISPU_MEM_BANK);
//    iis3dwb10is_read_reg(p_sensor_drv, IIS3DWB10IS_ISPU_DOUT_00_L, (uint8_t *)(&_this->p_ispu_output_buff), 32 * 2);
//    iis3dwb10is_mem_bank_set(p_sensor_drv, IIS3DWB10IS_MAIN_MEM_BANK);
//  }

  return res;
}

static sys_error_code_t IIS3DWB10ISTaskSensorRegister(IIS3DWB10ISTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

#if !IIS3DWB10IS_ACC_DISABLED
  ISensor_t *acc_if = (ISensor_t *) IIS3DWB10ISTaskGetAccSensorIF(_this);
  _this->acc_id = SMAddSensor(acc_if);
#endif
#if !IIS3DWB10IS_ISPU_DISABLED
  ISensor_t *ispu_if = (ISensor_t *) IIS3DWB10ISTaskGetIspuSensorIF(_this);
  _this->ispu_id = SMAddSensor(ispu_if);
#endif

  return res;
}

static sys_error_code_t IIS3DWB10ISTaskSensorInitTaskParams(IIS3DWB10ISTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* ACCELEROMETER STATUS */
  _this->acc_sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->acc_sensor_status.is_active = TRUE;
  _this->acc_sensor_status.type.mems.fs = 50.0f;
  _this->acc_sensor_status.type.mems.sensitivity = 0.0000019075f * _this->acc_sensor_status.type.mems.fs;
  _this->acc_sensor_status.type.mems.odr = 80000.0f;
  _this->acc_sensor_status.type.mems.measured_odr = 0.0f;
#if IIS3DWB10IS_FIFO_ENABLED
  EMD_Init(&_this->data_acc,  _this->p_acc_sensor_data_buff, E_EM_INT24, E_EM_MODE_INTERLEAVED, 2, 1, 3);
#else
  EMD_Init(&_this->data_acc,  _this->p_acc_sample, E_EM_INT32, E_EM_MODE_INTERLEAVED, 2, 1, 3);
#endif

  /* ISPU STATUS */
  _this->ispu_sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->ispu_sensor_status.is_active = FALSE;
  _this->ispu_sensor_status.type.mems.fs = 1.0f;
  _this->ispu_sensor_status.type.mems.sensitivity = 1.0f;
  _this->ispu_sensor_status.type.mems.odr = 1.0f;
  _this->ispu_sensor_status.type.mems.measured_odr = 0.0f;
  EMD_Init(&_this->data_ispu, (uint8_t *)_this->p_ispu_output_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 32);

  return res;
}

static sys_error_code_t IIS3DWB10ISTaskSensorSetODR(IIS3DWB10ISTask *_this, SMMessage report)
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
      iis3dwb10is_data_rate_t iis3dwb10is_xl_data_rate;
      iis3dwb10is_xl_data_rate.odr = IIS3DWB10IS_ODR_IDLE;

      iis3dwb10is_xl_data_rate_set(p_sensor_drv, iis3dwb10is_xl_data_rate);
      /* Do not update the model in case of odr = 0 */
      odr = _this->acc_sensor_status.type.mems.odr;
    }
    else
    {

      if (odr < 2600.0f)
      {
        odr = 2500.0f;
      }
      else if (odr < 5100.0f)
      {
        odr = 5000.0f;
      }
      else if (odr < 10100.0f)
      {
        odr = 10000.0f;
      }
      else if (odr < 20100.0f)
      {
        odr = 20000.0f;
      }
      else if (odr < 40100.0f)
      {
        odr = 40000.0f;
      }
      else
      {
        odr = 80000.0f;
      }
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->acc_sensor_status.type.mems.odr = odr;
      _this->acc_sensor_status.type.mems.measured_odr = 0.0f;
    }
  }
  else if (id == _this->ispu_id)
  {
    res = SYS_TASK_INVALID_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB10IS: warning - ISPU SetODR() not supported.\r\n"));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t IIS3DWB10ISTaskSensorSetFS(IIS3DWB10ISTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  float_t fs = (float_t) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  /* Changing fs must disable ISPU sensor: ISPU can work properly only when setup from UCF */
  _this->ispu_enable = FALSE;
  _this->ispu_sensor_status.is_active = FALSE;

  if (id == _this->acc_id)
  {
    if (fs < 51.0f)
    {
      fs = 50.0f;
    }
    else if (fs < 101.0f)
    {
      fs = 100.0f;
    }
    else
    {
      fs = 200.0f;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->acc_sensor_status.type.mems.fs = fs;
      _this->acc_sensor_status.type.mems.sensitivity = 0.0000019075f * _this->acc_sensor_status.type.mems.fs;
    }
  }
  else if (id == _this->ispu_id)
  {
    res = SYS_TASK_INVALID_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB10IS: warning - ISPU SetFS() not supported.\r\n"));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t IIS3DWB10ISTaskSensorSetFifoWM(IIS3DWB10ISTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t iis3dwb10is_wtm_level = (uint16_t)report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    if (iis3dwb10is_wtm_level > IIS3DWB10IS_MAX_WTM_LEVEL)
    {
      iis3dwb10is_wtm_level = IIS3DWB10IS_MAX_WTM_LEVEL;
    }
    _this->samples_per_it = iis3dwb10is_wtm_level;

//    /*Set fifo in continuous / stream mode*/
//    iis3dwb10is_fifo_mode_set(p_sensor_drv, IIS3DWB10IS_STREAM_MODE);

    /*Set watermark*/
    iis3dwb10is_fifo_watermark_set(p_sensor_drv, _this->samples_per_it);
//    /*Data Ready pulse mode*/
//    iis3dwb10is_data_ready_mode_set(p_sensor_drv, IIS3DWB10IS_DRDY_PULSED);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t IIS3DWB10ISTaskSensorEnable(IIS3DWB10ISTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    _this->acc_sensor_status.is_active = TRUE;

    /* Changing sensor configuration must disable ISPU sensor: ISPU can work properly only when setup from UCF */
    _this->ispu_enable = FALSE;
    _this->ispu_sensor_status.is_active = FALSE;
  }
  else if (id == _this->ispu_id)
  {
    _this->ispu_sensor_status.is_active = TRUE;
    _this->ispu_enable = TRUE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t IIS3DWB10ISTaskSensorDisable(IIS3DWB10ISTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    _this->acc_sensor_status.is_active = FALSE;

    iis3dwb10is_data_rate_t iis3dwb10is_xl_data_rate;
    iis3dwb10is_xl_data_rate.odr = IIS3DWB10IS_ODR_IDLE;

    iis3dwb10is_xl_data_rate_set(p_sensor_drv, iis3dwb10is_xl_data_rate);
    iis3dwb10is_fifo_mode_set(p_sensor_drv, IIS3DWB10IS_BYPASS_MODE);

    /* Changing sensor configuration must disable ISPU sensor: ISPU can work properly only when setup from UCF */
    _this->ispu_enable = FALSE;
    _this->ispu_sensor_status.is_active = FALSE;
  }
  else if (id == _this->ispu_id)
  {
    _this->ispu_sensor_status.is_active = FALSE;
    _this->ispu_enable = FALSE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t IIS3DWB10ISTaskSensorIsActive(const IIS3DWB10ISTask *_this)
{
  assert_param(_this != NULL);
  return (_this->acc_sensor_status.is_active);
}

static sys_error_code_t IIS3DWB10ISTaskEnterLowPowerMode(const IIS3DWB10ISTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  iis3dwb10is_data_rate_t iis3dwb10is_xl_data_rate;
  iis3dwb10is_xl_data_rate.odr = IIS3DWB10IS_ODR_IDLE;

  iis3dwb10is_xl_data_rate_set(p_sensor_drv, iis3dwb10is_xl_data_rate);

  return res;
}

static sys_error_code_t IIS3DWB10ISTaskConfigureIrqPin(const IIS3DWB10ISTask *_this, boolean_t LowPower)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (!LowPower)
  {
    /* Configure INT Pin */
    _this->p_irq_config->p_mx_init_f();
  }
  else
  {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // first disable the IRQ to avoid spurious interrupt to wake the MCU up.
    HAL_NVIC_DisableIRQ(_this->p_irq_config->irq_n);
    HAL_NVIC_ClearPendingIRQ(_this->p_irq_config->irq_n);
    // then reconfigure the PIN in analog high impedance to reduce the power consumption.
    GPIO_InitStruct.Pin =  _this->p_irq_config->pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(_this->p_irq_config->port, &GPIO_InitStruct);
  }

  return res;
}

static sys_error_code_t IIS3DWB10ISTaskConfigureISPUPin(const IIS3DWB10ISTask *_this, boolean_t LowPower)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (!LowPower)
  {
    /* Configure ISPU INT Pin */
    _this->p_ispu_config->p_mx_init_f();
  }
  else
  {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // first disable the IRQ to avoid spurious interrupt to wake the MCU up.
    HAL_NVIC_DisableIRQ(_this->p_ispu_config->irq_n);
    HAL_NVIC_ClearPendingIRQ(_this->p_ispu_config->irq_n);
    // then reconfigure the PIN in analog high impedance to reduce the power consumption.
    GPIO_InitStruct.Pin =  _this->p_ispu_config->pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(_this->p_ispu_config->port, &GPIO_InitStruct);
  }

  return res;
}

static inline IIS3DWB10ISTask *IIS3DWB10ISTaskGetOwnerFromISensorIF(ISensor_t *p_if)
{
  assert_param(p_if != NULL);
  IIS3DWB10ISTask *p_if_owner = NULL;

  /* check if the virtual function has been called from the ispu sensor IF  */
  p_if_owner = (IIS3DWB10ISTask *)((uint32_t) p_if - offsetof(IIS3DWB10ISTask, ispu_sensor_if));
  if (!(p_if_owner->acc_sensor_if.vptr == &sTheClass.acc_sensor_if_vtbl) || !(p_if_owner->super.vptr == &sTheClass.vtbl))
  {
    /* then the virtual function has been called from the acc IF  */
    p_if_owner = (IIS3DWB10ISTask *)((uint32_t) p_if - offsetof(IIS3DWB10ISTask, acc_sensor_if));
  }

  return p_if_owner;
}

static inline IIS3DWB10ISTask *IIS3DWB10ISTaskGetOwnerFromISensorLLIF(ISensorLL_t *p_if)
{
  assert_param(p_if != NULL);
  IIS3DWB10ISTask *p_if_owner = NULL;
  p_if_owner = (IIS3DWB10ISTask *)((uint32_t) p_if - offsetof(IIS3DWB10ISTask, sensor_ll_if));

  return p_if_owner;
}

static void IIS3DWB10ISTaskTimerCallbackFunction(ULONG param)
{
  IIS3DWB10ISTask *p_obj = (IIS3DWB10ISTask *) param;
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

static void IIS3DWB10ISTaskISPUTimerCallbackFunction(ULONG param)
{
  IIS3DWB10ISTask *p_obj = (IIS3DWB10ISTask *) param;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY_ISPU;
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

void IIS3DWB10ISTask_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((IIS3DWB10ISTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}

void INT2_IIS3DWB10IS_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY_ISPU;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((IIS3DWB10ISTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}


static sys_error_code_t IIS3DWB10IS_ODR_Sync(IIS3DWB10ISTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  float_t odr = 0.0f;
  iis3dwb10is_data_rate_t iis3dwb10is_odr_xl;
  if (iis3dwb10is_xl_data_rate_get(p_sensor_drv, &iis3dwb10is_odr_xl) == 0)
  {
    _this->acc_sensor_status.is_active = TRUE;

    /* Update only the model */
    switch (iis3dwb10is_odr_xl.odr)
    {
      case IIS3DWB10IS_ODR_IDLE:
        _this->acc_sensor_status.is_active = FALSE;
        /* Do not update the model in case of odr = 0 */
        odr = _this->acc_sensor_status.type.mems.odr;
        break;
      case IIS3DWB10IS_ODR_2KHz5:
        odr = 2500.0f;
        break;
      case IIS3DWB10IS_ODR_5KHz:
        odr = 5000.0f;
        break;
      case IIS3DWB10IS_ODR_10KHz:
        odr = 10000.0f;
        break;
      case IIS3DWB10IS_ODR_20KHz:
        odr = 20000.0f;
        break;
      case IIS3DWB10IS_ODR_40KHz:
        odr = 40000.0f;
        break;
      case IIS3DWB10IS_ODR_80KHz:
        odr = 80000.0f;
        break;
    }
    _this->acc_sensor_status.type.mems.odr = odr;
    _this->acc_sensor_status.type.mems.measured_odr = 0.0f;

  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }

  _this->samples_per_it = 0;
  return res;
}

static sys_error_code_t IIS3DWB10IS_FS_Sync(IIS3DWB10ISTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  float_t fs = 50.0f;
  iis3dwb10is_fs_xl_t fs_xl;
  if (iis3dwb10is_xl_full_scale_get(p_sensor_drv, &fs_xl) == 0)
  {
    switch (fs_xl)
    {
      case IIS3DWB_50g:
        fs = 50.0f;
        break;
      case IIS3DWB_100g:
        fs = 100.0f;
        break;
      case IIS3DWB_200g:
        fs = 200.0f;
        break;
      default:
        break;
    }
    _this->acc_sensor_status.type.mems.fs = fs;
    _this->acc_sensor_status.type.mems.sensitivity = 0.0000019075f * _this->acc_sensor_status.type.mems.fs;
  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}
