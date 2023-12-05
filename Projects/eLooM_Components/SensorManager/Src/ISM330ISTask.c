/**
  ******************************************************************************
  * @file    ISM330ISTask.c
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
#include "ISM330ISTask.h"
#include "ISM330ISTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "ism330is_reg.h"
#include <string.h>
#include <stdlib.h>
#include "services/sysdebug.h"

/* Private includes ----------------------------------------------------------*/

#ifndef ISM330IS_TASK_CFG_STACK_DEPTH
#define ISM330IS_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*5)
#endif

#ifndef ISM330IS_TASK_CFG_PRIORITY
#define ISM330IS_TASK_CFG_PRIORITY                 (TX_MAX_PRIORITIES - 1)
#endif

#ifndef ISM330IS_TASK_CFG_IN_QUEUE_LENGTH
#define ISM330IS_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define ISM330IS_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef ISM330IS_TASK_CFG_TIMER_PERIOD_MS
#define ISM330IS_TASK_CFG_TIMER_PERIOD_MS          1000
#endif
#ifndef ISM330IS_TASK_CFG_ISPU_IMER_PERIOD_MS
#define ISM330IS_TASK_CFG_ISPU_TIMER_PERIOD_MS     500
#endif

#define ISM330IS_TAG_ACC                           (0x02)

#ifndef ISM330IS_TASK_CFG_MAX_INSTANCES_COUNT
#define ISM330IS_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)                 SYS_DEBUGF3(SYS_DBG_ISM330IS, level, message)

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static int16_t dummyDataCounter_acc = 0;
static int16_t dummyDataCounter_gyro = 0;
#endif

/**
  * Class object declaration
  */
typedef struct _ISM330ISTaskClass
{
  /**
    * ISM330ISTask class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * Accelerometer IF virtual table.
    */
  const ISensorMems_vtbl acc_sensor_if_vtbl;

  /**
    * Gyro IF virtual table.
    */
  const ISensorMems_vtbl gyro_sensor_if_vtbl;

  /**
    * Ispu Sensor IF virtual table.
    */
  const ISensorMems_vtbl ispu_sensor_if_vtbl;

  /**
    * Ispu SensorMlc IF virtual table.
    */
  const ISensorLL_vtbl sensor_ll_if_vtbl;

  /**
    * Specifies accelerometer sensor capabilities.
    */
  const SensorDescriptor_t acc_class_descriptor;

  /**
    * Specifies gyroscope sensor capabilities.
    */
  const SensorDescriptor_t gyro_class_descriptor;

  /**
    * Specifies ispu sensor capabilities.
    */
  const SensorDescriptor_t ispu_class_descriptor;

  /**
    * ISM330ISTask (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[2 * ISM330IS_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} ISM330ISTaskClass_t;

/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t ISM330ISTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t ISM330ISTaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t ISM330ISTaskSensorInit(ISM330ISTask *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t ISM330ISTaskSensorReadData(ISM330ISTask *_this);

/**
  * Read the data from the ispu.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t ISM330ISTaskSensorReadISPU(ISM330ISTask *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t ISM330ISTaskSensorRegister(ISM330ISTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t ISM330ISTaskSensorInitTaskParams(ISM330ISTask *_this);

/**
  * Private implementation of sensor interface methods for ISM330IS sensor
  */
static sys_error_code_t ISM330ISTaskSensorSetODR(ISM330ISTask *_this, SMMessage report);
static sys_error_code_t ISM330ISTaskSensorSetFS(ISM330ISTask *_this, SMMessage report);
static sys_error_code_t ISM330ISTaskSensorEnable(ISM330ISTask *_this, SMMessage report);
static sys_error_code_t ISM330ISTaskSensorDisable(ISM330ISTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t ISM330ISTaskSensorIsActive(const ISM330ISTask *_this);

static sys_error_code_t ISM330ISTaskEnterLowPowerMode(const ISM330ISTask *_this);

static sys_error_code_t ISM330ISTaskConfigureIrqPin(const ISM330ISTask *_this, boolean_t LowPower);
static sys_error_code_t ISM330ISTaskConfigureISPUPin(const ISM330ISTask *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void ISM330ISTaskTimerCallbackFunction(ULONG param);

/**
  * Callback function called when the ispu software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void ISM330ISTaskISPUTimerCallbackFunction(ULONG param);

/**
  * Given a interface pointer it return the instance of the object that implement the interface.
  *
  * @param p_if [IN] specifies a sensor interface implemented by the task object.
  * @return the instance of the task object that implements the given interface.
  */
static inline ISM330ISTask *ISM330ISTaskGetOwnerFromISensorIF(ISensor_t *p_if);

/**
  * Given a interface pointer it return the instance of the object that implement the interface.
  *
  * @param p_if [IN] specifies a ISensorLL interface implemented by the task object.
  * @return the instance of the task object that implements the given interface.
  */
static inline ISM330ISTask *ISM330ISTaskGetOwnerFromISensorLLIF(ISensorLL_t *p_if);

/**
  * Read the odr value from the sensor and update the internal model
  */
static sys_error_code_t ISM330IS_ODR_Sync(ISM330ISTask *_this);

/**
  * Read the odr value from the sensor and update the internal model
  */
static sys_error_code_t ISM330IS_FS_Sync(ISM330ISTask *_this);

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
static inline sys_error_code_t ISM330ISTaskPostReportToFront(ISM330ISTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t ISM330ISTaskPostReportToBack(ISM330ISTask *_this, SMMessage *pReport);

/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static ISM330ISTask sTaskObj;
/**
  * The class object.
  */
static ISM330ISTaskClass_t sTheClass =
{
  /* class virtual table */
  {
    ISM330ISTask_vtblHardwareInit,
    ISM330ISTask_vtblOnCreateTask,
    ISM330ISTask_vtblDoEnterPowerMode,
    ISM330ISTask_vtblHandleError,
    ISM330ISTask_vtblOnEnterTaskControlLoop,
    ISM330ISTask_vtblForceExecuteStep,
    ISM330ISTask_vtblOnEnterPowerMode
  },

  /* class::acc_sensor_if_vtbl virtual table */
  { { {
        ISM330ISTask_vtblAccGetId,
        ISM330ISTask_vtblAccGetEventSourceIF,
        ISM330ISTask_vtblAccGetDataInfo
      },
      ISM330ISTask_vtblSensorEnable,
      ISM330ISTask_vtblSensorDisable,
      ISM330ISTask_vtblSensorIsEnabled,
      ISM330ISTask_vtblAccGetDescription,
      ISM330ISTask_vtblAccGetStatus
    },
    ISM330ISTask_vtblAccGetODR,
    ISM330ISTask_vtblAccGetFS,
    ISM330ISTask_vtblAccGetSensitivity,
    ISM330ISTask_vtblSensorSetODR,
    ISM330ISTask_vtblSensorSetFS,
    ISM330ISTask_vtblSensorSetFifoWM
  },

  /* class::gyro_sensor_if_vtbl virtual table */
  { { {
        ISM330ISTask_vtblGyroGetId,
        ISM330ISTask_vtblGyroGetEventSourceIF,
        ISM330ISTask_vtblGyroGetDataInfo
      },
      ISM330ISTask_vtblSensorEnable,
      ISM330ISTask_vtblSensorDisable,
      ISM330ISTask_vtblSensorIsEnabled,
      ISM330ISTask_vtblGyroGetDescription,
      ISM330ISTask_vtblGyroGetStatus
    },
    ISM330ISTask_vtblGyroGetODR,
    ISM330ISTask_vtblGyroGetFS,
    ISM330ISTask_vtblGyroGetSensitivity,
    ISM330ISTask_vtblSensorSetODR,
    ISM330ISTask_vtblSensorSetFS,
    ISM330ISTask_vtblSensorSetFifoWM
  },

  /* class::ispu_sensor_if_vtbl virtual table */
  { { {
        ISM330ISTask_vtblIspuGetId,
        ISM330ISTask_vtblIspuGetEventSourceIF,
        ISM330ISTask_vtblIspuGetDataInfo
      },
      ISM330ISTask_vtblSensorEnable,
      ISM330ISTask_vtblSensorDisable,
      ISM330ISTask_vtblSensorIsEnabled,
      ISM330ISTask_vtblIspuGetDescription,
      ISM330ISTask_vtblIspuGetStatus
    },
    ISM330ISTask_vtblIspuGetODR,
    ISM330ISTask_vtblIspuGetFS,
    ISM330ISTask_vtblIspuGetSensitivity,
    ISM330ISTask_vtblSensorSetODR,
    ISM330ISTask_vtblSensorSetFS,
    ISM330ISTask_vtblSensorSetFifoWM
  },

  /* class::ispu_sensorMlc_if_vtbl virtual table */
  {
    ISM330ISTask_vtblSensorReadReg,
    ISM330ISTask_vtblSensorWriteReg,
    ISM330ISTask_vtblSensorSyncModel
  },

  /* ACCELEROMETER DESCRIPTOR */
  {
    "ism330is",
    COM_TYPE_ACC,
    {
      12.5,
      26,
      52,
      104,
      208,
      416,
      833,
      1667,
      3333,
      6667,
      COM_END_OF_LIST_FLOAT,
    },
    {
      2,
      4,
      8,
      16,
      COM_END_OF_LIST_FLOAT,
    },
    {
      "acc",
    },
    "g",
    {
      0,
      1000,
    }
  },

  /* GYROSCOPE DESCRIPTOR */
  {
    "ism330is",
    COM_TYPE_GYRO,
    {
      12.5,
      26,
      52,
      104,
      208,
      416,
      833,
      1667,
      3333,
      6667,
      COM_END_OF_LIST_FLOAT,
    },
    {
      125,
      250,
      500,
      1000,
      2000,
      COM_END_OF_LIST_FLOAT,
    },
    {
      "gyro",
    },
    "mdps",
    {
      0,
      1000,
    }
  },

  /* ISPU DESCRIPTOR */
  {
    "ism330is",
    COM_TYPE_ISPU,
    {
      1,
      COM_END_OF_LIST_FLOAT,
    },
    {
      1,
      COM_END_OF_LIST_FLOAT,
    },
    {
      "ispu",
    },
    "out",
    {
      0,
      1,
    }
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    ISM330ISTaskExecuteStepState1,
    NULL,
    ISM330ISTaskExecuteStepDatalog,
  },

  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *ISM330ISTaskGetAccSensorIF(ISM330ISTask *_this)
{
  return (ISourceObservable *) & (_this->acc_sensor_if);
}

ISourceObservable *ISM330ISTaskGetGyroSensorIF(ISM330ISTask *_this)
{
  return (ISourceObservable *) & (_this->gyro_sensor_if);
}

ISourceObservable *ISM330ISTaskGetIspuSensorIF(ISM330ISTask *_this)
{
  return (ISourceObservable *) & (_this->ispu_sensor_if);
}

ISensorLL_t *ISM330ISTaskGetSensorLLIF(ISM330ISTask *_this)
{
  return (ISensorLL_t *) & (_this->sensor_ll_if);
}

AManagedTaskEx *ISM330ISTaskAlloc(const void *pIRQConfig, const void *pISPUConfig, const void *pCSConfig)
{
  ISM330ISTask *p_new_obj = SysAlloc(sizeof(ISM330ISTask));

  if (p_new_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_new_obj->super);

    p_new_obj->super.vptr = &sTheClass.vtbl;
    p_new_obj->acc_sensor_if.vptr = &sTheClass.acc_sensor_if_vtbl;
    p_new_obj->gyro_sensor_if.vptr = &sTheClass.gyro_sensor_if_vtbl;
    p_new_obj->ispu_sensor_if.vptr = &sTheClass.ispu_sensor_if_vtbl;
    p_new_obj->sensor_ll_if.vptr = &sTheClass.sensor_ll_if_vtbl;
    p_new_obj->acc_sensor_descriptor = &sTheClass.acc_class_descriptor;
    p_new_obj->gyro_sensor_descriptor = &sTheClass.gyro_class_descriptor;
    p_new_obj->ispu_sensor_descriptor = &sTheClass.ispu_class_descriptor;

    p_new_obj->p_irq_config = (MX_GPIOParams_t *) pIRQConfig;
    p_new_obj->p_ispu_config = (MX_GPIOParams_t *) pISPUConfig;
    p_new_obj->p_cs_config = (MX_GPIOParams_t *) pCSConfig;

    strcpy(p_new_obj->acc_sensor_status.p_name, sTheClass.acc_class_descriptor.p_name);
    strcpy(p_new_obj->gyro_sensor_status.p_name, sTheClass.gyro_class_descriptor.p_name);
    strcpy(p_new_obj->ispu_sensor_status.p_name, sTheClass.ispu_class_descriptor.p_name);
  }

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *ISM330ISTaskAllocSetName(const void *pIRQConfig, const void *pISPUConfig, const void *pCSConfig,
                                         const char *p_name)
{
  ISM330ISTask *p_new_obj = (ISM330ISTask *) ISM330ISTaskAlloc(pIRQConfig, pISPUConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->acc_sensor_status.p_name, p_name);
  strcpy(p_new_obj->gyro_sensor_status.p_name, p_name);
  strcpy(p_new_obj->ispu_sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *ISM330ISTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pISPUConfig,
                                        const void *pCSConfig)
{
  ISM330ISTask *p_obj = (ISM330ISTask *) p_mem_block;

  if (p_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_obj->super);

    p_obj->super.vptr = &sTheClass.vtbl;
    p_obj->acc_sensor_if.vptr = &sTheClass.acc_sensor_if_vtbl;
    p_obj->gyro_sensor_if.vptr = &sTheClass.gyro_sensor_if_vtbl;
    p_obj->ispu_sensor_if.vptr = &sTheClass.ispu_sensor_if_vtbl;
    p_obj->sensor_ll_if.vptr = &sTheClass.sensor_ll_if_vtbl;
    p_obj->acc_sensor_descriptor = &sTheClass.acc_class_descriptor;
    p_obj->gyro_sensor_descriptor = &sTheClass.gyro_class_descriptor;
    p_obj->ispu_sensor_descriptor = &sTheClass.ispu_class_descriptor;

    p_obj->p_irq_config = (MX_GPIOParams_t *) pIRQConfig;
    p_obj->p_ispu_config = (MX_GPIOParams_t *) pISPUConfig;
    p_obj->p_cs_config = (MX_GPIOParams_t *) pCSConfig;
  }

  return (AManagedTaskEx *) p_obj;
}

AManagedTaskEx *ISM330ISTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pISPUConfig,
                                               const void *pCSConfig, const char *p_name)
{
  ISM330ISTask *p_obj = (ISM330ISTask *) ISM330ISTaskStaticAlloc(p_mem_block, pIRQConfig, pISPUConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->acc_sensor_status.p_name, p_name);
  strcpy(p_obj->gyro_sensor_status.p_name, p_name);
  strcpy(p_obj->ispu_sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *ISM330ISTaskGetSensorIF(ISM330ISTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *ISM330ISTaskGetAccEventSrcIF(ISM330ISTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_acc_event_src;
}

IEventSrc *ISM330ISTaskGetGyroEventSrcIF(ISM330ISTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_gyro_event_src;
}

IEventSrc *ISM330ISTaskGetMlcEventSrcIF(ISM330ISTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *)_this->p_ispu_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t ISM330ISTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ISM330ISTask *p_obj = (ISM330ISTask *) _this;

  /* Configure CS Pin */
  if (p_obj->p_cs_config != NULL)
  {
    p_obj->p_cs_config->p_mx_init_f();
  }

  return res;
}

sys_error_code_t ISM330ISTask_vtblOnCreateTask(
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
  ISM330ISTask *p_obj = (ISM330ISTask *) _this;

  p_obj->sync = true;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) ISM330IS_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(ISM330IS_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "ISM330IS_Q", item_size / 4u, p_queue_items_buff,
                                    ISM330IS_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_timer, "ISM330IS_T", ISM330ISTaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(ISM330IS_TASK_CFG_TIMER_PERIOD_MS),
                         0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* create the ispu software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->ispu_timer, "ISM330IS_ISPU_T", ISM330ISTaskISPUTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(ISM330IS_TASK_CFG_ISPU_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->p_cs_config != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(ISM330IS_ID, p_obj->p_cs_config->port, (uint16_t) p_obj->p_cs_config->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(ISM330IS_ID, ISM330IS_I2C_ADD_H, 0);
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
  p_obj->p_acc_event_src = DataEventSrcAlloc();
  if (p_obj->p_acc_event_src == NULL)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }
  IEventSrcInit(p_obj->p_acc_event_src);

  p_obj->p_gyro_event_src = DataEventSrcAlloc();
  if (p_obj->p_gyro_event_src == NULL)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }
  IEventSrcInit(p_obj->p_gyro_event_src);

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
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, 2 * ISM330IS_TASK_CFG_MAX_INSTANCES_COUNT);
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
  memset(p_obj->p_acc_sample, 0, sizeof(p_obj->p_acc_sample));
  memset(p_obj->p_gyro_sample, 0, sizeof(p_obj->p_gyro_sample));
  memset(p_obj->p_ispu_output_buff, 0, sizeof(p_obj->p_ispu_output_buff));
  p_obj->acc_id = 0;
  p_obj->gyro_id = 1;
  p_obj->ispu_enable = FALSE;
  p_obj->prev_timestamp = 0.0f;
  p_obj->acc_samples_count = 0;
  p_obj->gyro_samples_count = 0;
  p_obj->samples_per_it = 0;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "ISM330IS";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = ISM330IS_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = ISM330IS_TASK_CFG_PRIORITY;
  *pPreemptThreshold = ISM330IS_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = ISM330ISTaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = ISM330ISTaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ISM330IS: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t ISM330ISTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ISM330ISTask *p_obj = (ISM330ISTask *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (ISM330ISTaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ISM330IS: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      /* Deactivate the sensor */
      ism330is_xl_data_rate_set(p_sensor_drv, ISM330IS_XL_ODR_OFF);
      ism330is_gy_data_rate_set(p_sensor_drv, ISM330IS_GY_ODR_OFF);

      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->p_irq_config == NULL)
      {
        tx_timer_deactivate(&p_obj->read_timer);
      }
      else
      {
        ISM330ISTaskConfigureIrqPin(p_obj, TRUE);
      }
      if (p_obj->p_ispu_config == NULL)
      {
        tx_timer_deactivate(&p_obj->ispu_timer);
      }
      else
      {
        ISM330ISTaskConfigureISPUPin(p_obj, TRUE);
      }
    }
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ISM330IS: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = ISM330ISTaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("ISM330IS - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->p_irq_config != NULL)
    {
      ISM330ISTaskConfigureIrqPin(p_obj, TRUE);
    }
    if (p_obj->p_ispu_config != NULL)
    {
      ISM330ISTaskConfigureISPUPin(p_obj, TRUE);
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ISM330IS: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t ISM330ISTask_vtblHandleError(AManagedTask *_this, SysEvent Error)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t ISM330ISTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ISM330IS: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (ISM330IS_TASK_CFG_TAG)
  ISM330ISTask *p_obj = (ISM330ISTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = ISM330IS_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t ISM330ISTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ISM330ISTask *p_obj = (ISM330ISTask *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = ISM330ISTaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t ISM330ISTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  ISM330ISTask *p_obj = (ISM330ISTask*)_this;



  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t ISM330ISTask_vtblAccGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = (ISM330ISTask *)((uint32_t) _this - offsetof(ISM330ISTask, acc_sensor_if));
  uint8_t res = p_if_owner->acc_id;

  return res;
}

uint8_t ISM330ISTask_vtblGyroGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = (ISM330ISTask *)((uint32_t) _this - offsetof(ISM330ISTask, gyro_sensor_if));
  uint8_t res = p_if_owner->gyro_id;

  return res;
}

uint8_t ISM330ISTask_vtblIspuGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = (ISM330ISTask *)((uint32_t) _this - offsetof(ISM330ISTask, ispu_sensor_if));
  uint8_t res = p_if_owner->ispu_id;

  return res;
}

IEventSrc *ISM330ISTask_vtblAccGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = (ISM330ISTask *)((uint32_t) _this - offsetof(ISM330ISTask, acc_sensor_if));

  return p_if_owner->p_acc_event_src;
}

IEventSrc *ISM330ISTask_vtblGyroGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = (ISM330ISTask *)((uint32_t) _this - offsetof(ISM330ISTask, gyro_sensor_if));
  return p_if_owner->p_gyro_event_src;
}

IEventSrc *ISM330ISTask_vtblIspuGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = (ISM330ISTask *)((uint32_t) _this - offsetof(ISM330ISTask, ispu_sensor_if));
  return p_if_owner->p_ispu_event_src;
}

sys_error_code_t ISM330ISTask_vtblAccGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  ISM330ISTask *p_if_owner = (ISM330ISTask *)((uint32_t) _this - offsetof(ISM330ISTask, acc_sensor_if));
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

float ISM330ISTask_vtblAccGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = (ISM330ISTask *)((uint32_t) _this - offsetof(ISM330ISTask, acc_sensor_if));
  float res = p_if_owner->acc_sensor_status.type.mems.fs;

  return res;
}

float ISM330ISTask_vtblAccGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = (ISM330ISTask *)((uint32_t) _this - offsetof(ISM330ISTask, acc_sensor_if));
  float res = p_if_owner->acc_sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t ISM330ISTask_vtblAccGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = (ISM330ISTask *)((uint32_t)_this - offsetof(ISM330ISTask, acc_sensor_if));
  EMData_t res = p_if_owner->data_acc;

  return res;
}

sys_error_code_t ISM330ISTask_vtblGyroGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  ISM330ISTask *p_if_owner = (ISM330ISTask *)((uint32_t) _this - offsetof(ISM330ISTask, gyro_sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if ((p_measured == NULL) || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = p_if_owner->gyro_sensor_status.type.mems.measured_odr;
    *p_nominal = p_if_owner->gyro_sensor_status.type.mems.odr;
  }

  return res;
}

float ISM330ISTask_vtblGyroGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = (ISM330ISTask *)((uint32_t) _this - offsetof(ISM330ISTask, gyro_sensor_if));
  float res = p_if_owner->gyro_sensor_status.type.mems.fs;

  return res;
}

float ISM330ISTask_vtblGyroGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = (ISM330ISTask *)((uint32_t) _this - offsetof(ISM330ISTask, gyro_sensor_if));
  float res = p_if_owner->gyro_sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t ISM330ISTask_vtblGyroGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = (ISM330ISTask *)((uint32_t)_this - offsetof(ISM330ISTask, gyro_sensor_if));
  EMData_t res = p_if_owner->data_gyro;

  return res;
}

sys_error_code_t ISM330ISTask_vtblIspuGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  ISM330ISTask *p_if_owner = (ISM330ISTask *)((uint32_t)_this - offsetof(ISM330ISTask, ispu_sensor_if));
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

float ISM330ISTask_vtblIspuGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  /* ISPU does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);
  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("ISM330IS: warning - ISPU GetFS() not supported.\r\n"));
  return SYS_INVALID_FUNC_CALL_ERROR_CODE;
}

float ISM330ISTask_vtblIspuGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);

  /* ISPU does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("ISM330ISPU: warning - ISPU GetSensitivity() not supported.\r\n"));

  return -1.0f;
}

EMData_t ISM330ISTask_vtblIspuGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = (ISM330ISTask *)((uint32_t)_this - offsetof(ISM330ISTask, ispu_sensor_if));
  EMData_t res = p_if_owner->data_ispu;

  return res;
}

sys_error_code_t ISM330ISTask_vtblSensorSetODR(ISensorMems_t *_this, float odr)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ISM330ISTask *p_if_owner = ISM330ISTaskGetOwnerFromISensorIF((ISensor_t *)_this);

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
    res = ISM330ISTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t ISM330ISTask_vtblSensorSetFS(ISensorMems_t *_this, float fs)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ISM330ISTask *p_if_owner = ISM330ISTaskGetOwnerFromISensorIF((ISensor_t *)_this);

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
    res = ISM330ISTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t ISM330ISTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  /* ISPU does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);
  SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("ISM330ISPU: warning - SetFifoWM() not supported.\r\n"));
  return SYS_INVALID_FUNC_CALL_ERROR_CODE;
}

sys_error_code_t ISM330ISTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ISM330ISTask *p_if_owner = ISM330ISTaskGetOwnerFromISensorIF(_this);

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
    res = ISM330ISTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t ISM330ISTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ISM330ISTask *p_if_owner = ISM330ISTaskGetOwnerFromISensorIF(_this);

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
    res = ISM330ISTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t ISM330ISTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  ISM330ISTask *p_if_owner = ISM330ISTaskGetOwnerFromISensorIF(_this);

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->acc_id)
  {
    res = p_if_owner->acc_sensor_status.is_active;
  }
  else if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->gyro_id)
  {
    res = p_if_owner->gyro_sensor_status.is_active;
  }
  else if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->ispu_id)
  {
    res = p_if_owner->ispu_sensor_status.is_active;
  }

  return res;
}

SensorDescriptor_t ISM330ISTask_vtblAccGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = ISM330ISTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->acc_sensor_descriptor;
}

SensorDescriptor_t ISM330ISTask_vtblGyroGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = ISM330ISTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->gyro_sensor_descriptor;
}

SensorDescriptor_t ISM330ISTask_vtblIspuGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = ISM330ISTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->ispu_sensor_descriptor;
}

SensorStatus_t ISM330ISTask_vtblAccGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = ISM330ISTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->acc_sensor_status;
}

SensorStatus_t ISM330ISTask_vtblGyroGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = ISM330ISTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->gyro_sensor_status;
}

SensorStatus_t ISM330ISTask_vtblIspuGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  ISM330ISTask *p_if_owner = ISM330ISTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->ispu_sensor_status;
}

sys_error_code_t ISM330ISTask_vtblSensorReadReg(ISensorLL_t *_this, uint16_t reg, uint8_t *data, uint16_t len)
{
  assert_param(_this != NULL);
  assert_param(reg <= 0xFFU);
  assert_param(data != NULL);
  assert_param(len != 0U);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ISM330ISTask *p_if_owner = ISM330ISTaskGetOwnerFromISensorLLIF(_this);
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_if_owner->p_sensor_bus_if->m_xConnector;
  uint8_t reg8 = (uint8_t)(reg & 0x00FF);

  if (ism330is_read_reg(p_sensor_drv, reg8, data, len) != 0)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t ISM330ISTask_vtblSensorWriteReg(ISensorLL_t *_this, uint16_t reg, const uint8_t *data, uint16_t len)
{
  assert_param(_this != NULL);
  assert_param(reg <= 0xFFU);
  assert_param(data != NULL);
  assert_param(len != 0U);

  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ISM330ISTask *p_if_owner = ISM330ISTaskGetOwnerFromISensorLLIF(_this);
  uint8_t reg8 = (uint8_t)(reg & 0x00FF);

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_if_owner->p_sensor_bus_if->m_xConnector;

  /* This generic register write operation could mean that the model is out of sync with the HW */
  p_if_owner->sync = false;

  if (ism330is_write_reg(p_sensor_drv, reg8, (uint8_t *)data, len) != 0)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t ISM330ISTask_vtblSensorSyncModel(ISensorLL_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ISM330ISTask *p_if_owner = ISM330ISTaskGetOwnerFromISensorLLIF(_this);

  if (ISM330IS_ODR_Sync(p_if_owner) != SYS_NO_ERROR_CODE)
  {
    res = SYS_BASE_ERROR_CODE;
  }
  if (ISM330IS_FS_Sync(p_if_owner) != SYS_NO_ERROR_CODE)
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

static sys_error_code_t ISM330ISTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ISM330ISTask *p_obj = (ISM330ISTask *) _this;
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
            res = ISM330ISTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = ISM330ISTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = ISM330ISTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = ISM330ISTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("ISM330IS: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("ISM330IS: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t ISM330ISTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ISM330ISTask *p_obj = (ISM330ISTask *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("ISM330IS: new data.\r\n"));
//          if(p_obj->p_irq_config == NULL)
//          {
//            if(TX_SUCCESS != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->ism330is_task_cfg_timer_period_ms), AMT_MS_TO_TICKS(p_obj->ism330is_task_cfg_timer_period_ms)))
//            {
//              return SYS_UNDEFINED_ERROR_CODE;
//            }
//          }

        res = ISM330ISTaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          // notify the listeners...
          double timestamp = report.sensorDataReadyMessage.fTimestamp;
          double delta_timestamp = timestamp - p_obj->prev_timestamp;
          p_obj->prev_timestamp = timestamp;

          /* Create a bidimensional data interleaved [m x 3], m is the number of samples (1 in this case)
           * [X0, Y0, Z0]
           */
          if (p_obj->acc_sensor_status.is_active)
          {
            DataEvent_t evt_acc;

            /* update measuredODR */
            p_obj->acc_sensor_status.type.mems.measured_odr = (float)p_obj->acc_samples_count / (float)delta_timestamp;

            EMD_Init(&p_obj->data_acc, p_obj->p_acc_sample, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, p_obj->acc_samples_count, 3);
            DataEventInit((IEvent *) &evt_acc, p_obj->p_acc_event_src, &p_obj->data_acc, timestamp, p_obj->acc_id);

            IEventSrcSendEvent(p_obj->p_acc_event_src, (IEvent *) &evt_acc, NULL);
          }
          if (p_obj->gyro_sensor_status.is_active)
          {
            DataEvent_t evt_gyro;

            /* update measuredODR */
            p_obj->gyro_sensor_status.type.mems.measured_odr = (float)p_obj->gyro_samples_count / (float)delta_timestamp;

            EMD_Init(&p_obj->data_gyro, p_obj->p_gyro_sample, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, p_obj->gyro_samples_count, 3);
            DataEventInit((IEvent *) &evt_gyro, p_obj->p_gyro_event_src, &p_obj->data_gyro, timestamp, p_obj->gyro_id);

            IEventSrcSendEvent(p_obj->p_gyro_event_src, (IEvent *) &evt_gyro, NULL);
          }

          SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("ISM330IS: ts = %f\r\n", (float)timestamp));
        }
//            if(p_obj->p_irq_config == NULL)
//            {
//              if (TX_SUCCESS != tx_timer_activate(&p_obj->read_timer))
//              {
//                res = SYS_UNDEFINED_ERROR_CODE;
//              }
//            }

        break;
      }

      case SM_MESSAGE_ID_DATA_READY_ISPU:
      {
//          if(p_obj->p_ispu_config == NULL)
//          {
//          if (TX_SUCCESS != tx_timer_change(&p_obj->ispu_timer, AMT_MS_TO_TICKS(ISM330IS_TASK_CFG_ISPU_TIMER_PERIOD_MS),
//                                            AMT_MS_TO_TICKS(ISM330IS_TASK_CFG_ISPU_TIMER_PERIOD_MS)))
//            {
//              return SYS_UNDEFINED_ERROR_CODE;
//            }
//          }
        res = ISM330ISTaskSensorReadISPU(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          // notify the listeners...
          double timestamp = report.sensorDataReadyMessage.fTimestamp;

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

//            if(p_obj->p_ispu_config == NULL)
//            {
//              if (TX_SUCCESS != tx_timer_activate(&p_obj->ispu_timer))
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
            ISM330ISTaskConfigureIrqPin(p_obj, FALSE);
            res = ISM330ISTaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->acc_sensor_status.is_active == true || p_obj->gyro_sensor_status.is_active == true)
              {
                if (p_obj->p_irq_config == NULL)
                {
                  if (TX_SUCCESS != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->ism330is_task_cfg_timer_period_ms),
                                                    AMT_MS_TO_TICKS(p_obj->ism330is_task_cfg_timer_period_ms)))
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
                }
              }
            }
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->ispu_sensor_status.is_active == true)
              {
                if (p_obj->p_ispu_config == NULL)
                {
                  if (TX_SUCCESS != tx_timer_change(&p_obj->ispu_timer, AMT_MS_TO_TICKS(ISM330IS_TASK_CFG_ISPU_TIMER_PERIOD_MS),
                                                    AMT_MS_TO_TICKS(ISM330IS_TASK_CFG_ISPU_TIMER_PERIOD_MS)))
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
                  ISM330ISTaskConfigureISPUPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = ISM330ISTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = ISM330ISTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = ISM330ISTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = ISM330ISTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("ISM330IS: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }

      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("ISM330IS: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t ISM330ISTaskPostReportToFront(ISM330ISTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t ISM330ISTaskPostReportToBack(ISM330ISTask *_this, SMMessage *pReport)
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

static sys_error_code_t ISM330ISTaskSensorInit(ISM330ISTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  /* Setup acc and gyro */
  uint8_t reg0 = 0;
  ism330is_xl_data_rate_t ism330is_odr_xl = ISM330IS_XL_ODR_OFF;
  ism330is_gy_data_rate_t ism330is_odr_g = ISM330IS_GY_ODR_OFF;
  int32_t ret_val = 0;
  ism330is_pin_int2_route_t int2_route =
  {
    0
  };

//  if(_this->ispu_enable == false)
//  {
//  ret_val = ism330is_reset_set(p_sensor_drv, 1);
  do
  {
    ism330is_reset_get(p_sensor_drv, &reg0);
  } while (reg0);

  ret_val = ism330is_ui_i2c_mode_set(p_sensor_drv, ISM330IS_I2C_DISABLE);

  ret_val = ism330is_device_id_get(p_sensor_drv, (uint8_t *) &reg0);
  if (!ret_val)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, reg0);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ISM330IS: sensor - I am 0x%x.\r\n", reg0));

  /* AXL fs */
  if (_this->acc_sensor_status.type.mems.fs < 3.0f)
  {
    ism330is_xl_full_scale_set(p_sensor_drv, ISM330IS_2g);
  }
  else if (_this->acc_sensor_status.type.mems.fs < 5.0f)
  {
    ism330is_xl_full_scale_set(p_sensor_drv, ISM330IS_4g);
  }
  else if (_this->acc_sensor_status.type.mems.fs < 9.0f)
  {
    ism330is_xl_full_scale_set(p_sensor_drv, ISM330IS_8g);
  }
  else
    ism330is_xl_full_scale_set(p_sensor_drv, ISM330IS_16g);

  /* GYRO fs */
  if (_this->gyro_sensor_status.type.mems.fs < 126.0f)
  {
    ism330is_gy_full_scale_set(p_sensor_drv, ISM330IS_125dps);
  }
  else if (_this->gyro_sensor_status.type.mems.fs < 251.0f)
  {
    ism330is_gy_full_scale_set(p_sensor_drv, ISM330IS_250dps);
  }
  else if (_this->gyro_sensor_status.type.mems.fs < 501.0f)
  {
    ism330is_gy_full_scale_set(p_sensor_drv, ISM330IS_500dps);
  }
  else if (_this->gyro_sensor_status.type.mems.fs < 1001.0f)
  {
    ism330is_gy_full_scale_set(p_sensor_drv, ISM330IS_1000dps);
  }
  else
  {
    ism330is_gy_full_scale_set(p_sensor_drv, ISM330IS_2000dps);
  }

  if (_this->acc_sensor_status.type.mems.odr < 13.0f)
  {
    ism330is_odr_xl = ISM330IS_XL_ODR_AT_12Hz5_HP;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 27.0f)
  {
    ism330is_odr_xl = ISM330IS_XL_ODR_AT_26H_HP;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 53.0f)
  {
    ism330is_odr_xl = ISM330IS_XL_ODR_AT_52Hz_HP;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 105.0f)
  {
    ism330is_odr_xl = ISM330IS_XL_ODR_AT_104Hz_HP;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 209.0f)
  {
    ism330is_odr_xl = ISM330IS_XL_ODR_AT_208Hz_HP;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 417.0f)
  {
    ism330is_odr_xl = ISM330IS_XL_ODR_AT_416Hz_HP;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 834.0f)
  {
    ism330is_odr_xl = ISM330IS_XL_ODR_AT_833Hz_HP;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 1668.0f)
  {
    ism330is_odr_xl = ISM330IS_XL_ODR_AT_1667Hz_HP;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 3334.0f)
  {
    ism330is_odr_xl = ISM330IS_XL_ODR_AT_3333Hz_HP;
  }
  else
  {
    ism330is_odr_xl = ISM330IS_XL_ODR_AT_6667Hz_HP;
  }

  if (_this->gyro_sensor_status.type.mems.odr < 13.0f)
  {
    ism330is_odr_g = ISM330IS_GY_ODR_AT_12Hz5_HP;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 27.0f)
  {
    ism330is_odr_g = ISM330IS_GY_ODR_AT_26H_HP;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 53.0f)
  {
    ism330is_odr_g = ISM330IS_GY_ODR_AT_52Hz_HP;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 105.0f)
  {
    ism330is_odr_g = ISM330IS_GY_ODR_AT_104Hz_HP;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 209.0f)
  {
    ism330is_odr_g = ISM330IS_GY_ODR_AT_208Hz_HP;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 417.0f)
  {
    ism330is_odr_g = ISM330IS_GY_ODR_AT_416Hz_HP;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 834.0f)
  {
    ism330is_odr_g = ISM330IS_GY_ODR_AT_833Hz_HP;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 1668.0f)
  {
    ism330is_odr_g = ISM330IS_GY_ODR_AT_1667Hz_HP;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 3334.0f)
  {
    ism330is_odr_g = ISM330IS_GY_ODR_AT_3333Hz_HP;
  }
  else
  {
    ism330is_odr_g = ISM330IS_GY_ODR_AT_6667Hz_HP;
  }

  if (_this->acc_sensor_status.is_active)
  {
    ism330is_xl_data_rate_set(p_sensor_drv, ism330is_odr_xl);
  }
  else
  {
    ism330is_xl_data_rate_set(p_sensor_drv, ISM330IS_XL_ODR_OFF);
  }

  if (_this->gyro_sensor_status.is_active)
  {
    ism330is_gy_data_rate_set(p_sensor_drv, ism330is_odr_g);
  }
  else
  {
    ism330is_gy_data_rate_set(p_sensor_drv, ISM330IS_GY_ODR_OFF);
  }

  if (_this->p_irq_config != NULL)
  {
    if (_this->acc_sensor_status.is_active && _this->gyro_sensor_status.is_active) /* Both subSensor is active */
    {
      int2_route.drdy_xl = 1;
      int2_route.drdy_gy = 1;
    }
    else if (_this->acc_sensor_status.is_active)
    {
      int2_route.drdy_xl = 1;
    }
    else
    {
      int2_route.drdy_gy = 1;
    }
  }
  else
  {
    int2_route.drdy_xl = 0;
    int2_route.drdy_gy = 0;
  }

  if (_this->ispu_enable == false)
  {
    ism330is_pin_int2_route_set(p_sensor_drv, int2_route);
  }

  _this->ism330is_task_cfg_timer_period_ms = (uint16_t)(
                                               _this->acc_sensor_status.type.mems.odr < _this->gyro_sensor_status.type.mems.odr ?
                                               _this->acc_sensor_status.type.mems.odr :
                                               _this->gyro_sensor_status.type.mems.odr);
  _this->ism330is_task_cfg_timer_period_ms = (uint16_t)(1000.0f / _this->ism330is_task_cfg_timer_period_ms);

  _this->samples_per_it = 1;
  return res;
}

static sys_error_code_t ISM330ISTaskSensorReadData(ISM330ISTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  if ((_this->acc_sensor_status.is_active) && (_this->gyro_sensor_status.is_active))
  {
    uint32_t odr_acc = (uint32_t) _this->acc_sensor_status.type.mems.odr;
    uint32_t odr_gyro = (uint32_t) _this->gyro_sensor_status.type.mems.odr;

    if (odr_acc != odr_gyro)
    {
      ism330is_status_reg_t val;
      /* Need to read which sensor generated the INT in case of different odr; */
      ism330is_status_reg_get(p_sensor_drv, &val);

      if (val.xlda == 1U)
      {
        ism330is_read_reg(p_sensor_drv, ISM330IS_OUTX_L_A, _this->p_acc_sample, 6);
#if (HSD_USE_DUMMY_DATA == 1)
        int16_t *p16 = (int16_t *)(_this->p_acc_sample);
        *p16++ = dummyDataCounter_acc++;
        *p16++ = dummyDataCounter_acc++;
        *p16++ = dummyDataCounter_acc++;
#endif
        _this->acc_samples_count = 1U;

        /* Read newly INT status: while reading acc a new gyro data could be available */
        ism330is_status_reg_get(p_sensor_drv, &val);
      }
      if (val.gda == 1U)
      {
        ism330is_read_reg(p_sensor_drv, ISM330IS_OUTX_L_G, _this->p_gyro_sample, 6);
#if (HSD_USE_DUMMY_DATA == 1)
        int16_t *p16 = (int16_t *)(_this->p_gyro_sample);
        *p16++ = dummyDataCounter_gyro++;
        *p16++ = dummyDataCounter_gyro++;
        *p16++ = dummyDataCounter_gyro++;
#endif
        _this->gyro_samples_count = 1U;
      }
    }
    else
    {
      uint8_t p_samples[12];

      ism330is_read_reg(p_sensor_drv, ISM330IS_OUTX_L_G, p_samples, 12);
#if (HSD_USE_DUMMY_DATA == 1)
      int16_t *p16 = (int16_t *)(_this->p_acc_sample);
      *p16++ = dummyDataCounter_acc++;
      *p16++ = dummyDataCounter_acc++;
      *p16++ = dummyDataCounter_acc++;

      p16 = (int16_t *)(_this->p_gyro_sample);
      *p16++ = dummyDataCounter_gyro++;
      *p16++ = dummyDataCounter_gyro++;
      *p16++ = dummyDataCounter_gyro++;
#else
      memcpy(_this->p_gyro_sample, p_samples, 6);
      memcpy(_this->p_acc_sample, &p_samples[6], 6);
#endif
      _this->acc_samples_count = 1U;
      _this->gyro_samples_count = 1U;
    }
  }
  else if (_this->acc_sensor_status.is_active)
  {
    ism330is_read_reg(p_sensor_drv, ISM330IS_OUTX_L_A, _this->p_acc_sample, 6);
#if (HSD_USE_DUMMY_DATA == 1)
    int16_t *p16 = (int16_t *)(_this->p_acc_sample);
    *p16++ = dummyDataCounter_acc++;
    *p16++ = dummyDataCounter_acc++;
    *p16++ = dummyDataCounter_acc++;
#endif
    _this->acc_samples_count = 1U;
  }
  else if (_this->gyro_sensor_status.is_active)
  {
    ism330is_read_reg(p_sensor_drv, ISM330IS_OUTX_L_G, _this->p_gyro_sample, 6);
#if (HSD_USE_DUMMY_DATA == 1)
    int16_t *p16 = (int16_t *)(_this->p_gyro_sample);
    *p16++ = dummyDataCounter_gyro++;
    *p16++ = dummyDataCounter_gyro++;
    *p16++ = dummyDataCounter_gyro++;
#endif
    _this->gyro_samples_count = 1U;
  }
  else
  {
  }

  return res;
}

static sys_error_code_t ISM330ISTaskSensorReadISPU(ISM330ISTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  if (_this->ispu_enable)
  {
    ism330is_mem_bank_set(p_sensor_drv, ISM330IS_ISPU_MEM_BANK);
    ism330is_read_reg(p_sensor_drv, ISM330IS_ISPU_DOUT_00_L, (uint8_t *)(&_this->p_ispu_output_buff), 32 * 2);
    ism330is_mem_bank_set(p_sensor_drv, ISM330IS_MAIN_MEM_BANK);
  }

  return res;
}

static sys_error_code_t ISM330ISTaskSensorRegister(ISM330ISTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *acc_if = (ISensor_t *) ISM330ISTaskGetAccSensorIF(_this);
  ISensor_t *gyro_if = (ISensor_t *) ISM330ISTaskGetGyroSensorIF(_this);
  ISensor_t *ispu_if = (ISensor_t *) ISM330ISTaskGetIspuSensorIF(_this);

  _this->ispu_id = SMAddSensor(ispu_if);
  _this->acc_id = SMAddSensor(acc_if);
  _this->gyro_id = SMAddSensor(gyro_if);

  return res;
}

static sys_error_code_t ISM330ISTaskSensorInitTaskParams(ISM330ISTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* ACCELEROMETER STATUS */
  _this->acc_sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->acc_sensor_status.is_active = TRUE;
  _this->acc_sensor_status.type.mems.fs = 16.0f;
  _this->acc_sensor_status.type.mems.sensitivity = 0.0000305f * _this->acc_sensor_status.type.mems.fs;
  _this->acc_sensor_status.type.mems.odr = 1667.0f;
  _this->acc_sensor_status.type.mems.measured_odr = 0.0f;
  EMD_Init(&_this->data_acc, _this->p_acc_sample, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);

  /* GYROSCOPE STATUS */
  _this->gyro_sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->gyro_sensor_status.is_active = TRUE;
  _this->gyro_sensor_status.type.mems.fs = 2000.0f;
  _this->gyro_sensor_status.type.mems.sensitivity = 0.035f * _this->gyro_sensor_status.type.mems.fs;
  _this->gyro_sensor_status.type.mems.odr = 1667.0f;
  _this->gyro_sensor_status.type.mems.measured_odr = 0.0f;
  EMD_Init(&_this->data_gyro, _this->p_gyro_sample, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);

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

static sys_error_code_t ISM330ISTaskSensorSetODR(ISM330ISTask *_this, SMMessage report)
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
      ism330is_xl_data_rate_set(p_sensor_drv, ISM330IS_XL_ODR_OFF);
      /* Do not update the model in case of odr = 0 */
      odr = _this->acc_sensor_status.type.mems.odr;
    }
    else
    {
      /* Changing odr must disable ISPU sensor: ISPU can work properly only when setup from UCF */
      _this->ispu_enable = FALSE;
      _this->ispu_sensor_status.is_active = FALSE;

      if (odr < 13.0f)
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
      else if (odr < 834.0f)
      {
        odr = 833.0f;
      }
      else if (odr < 1668.0f)
      {
        odr = 1667.0f;
      }
      else if (odr < 3334.0f)
      {
        odr = 3333.0f;
      }
      else
      {
        odr = 6667;
      }
    }
    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->acc_sensor_status.type.mems.odr = odr;
      _this->acc_sensor_status.type.mems.measured_odr = 0.0f;
    }
  }
  else if (id == _this->gyro_id)
  {
    if (odr < 1.0f)
    {
      ism330is_gy_data_rate_set(p_sensor_drv, ISM330IS_GY_ODR_OFF);
      /* Do not update the model in case of odr = 0 */
      odr = _this->gyro_sensor_status.type.mems.odr;
    }
    else
    {
      /* Changing odr must disable ISPU sensor: ISPU can work properly only when setup from UCF */
      _this->ispu_enable = FALSE;
      _this->ispu_sensor_status.is_active = FALSE;

      if (odr < 13.0f)
      {
        odr = 12.5f;
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
      else if (odr < 834.0f)
      {
        odr = 833.0f;
      }
      else if (odr < 1668.0f)
      {
        odr = 1667.0f;
      }
      else if (odr < 3334.0f)
      {
        odr = 3333.0f;
      }
      else
      {
        odr = 6667.0f;
      }
    }
    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->gyro_sensor_status.type.mems.odr = odr;
      _this->gyro_sensor_status.type.mems.measured_odr = 0.0f;
    }
  }
  else if (id == _this->ispu_id)
  {
    res = SYS_TASK_INVALID_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("ISM330IS: warning - ISPU SetODR() not supported.\r\n"));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t ISM330ISTaskSensorSetFS(ISM330ISTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float fs = (float) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  /* Changing fs must disable ISPU sensor: ISPU can work properly only when setup from UCF */
  _this->ispu_enable = FALSE;
  _this->ispu_sensor_status.is_active = FALSE;

  if (id == _this->acc_id)
  {
    if (fs < 3.0f)
    {
      ism330is_xl_full_scale_set(p_sensor_drv, ISM330IS_2g);
      fs = 2.0f;
    }
    else if (fs < 5.0f)
    {
      ism330is_xl_full_scale_set(p_sensor_drv, ISM330IS_4g);
      fs = 4.0f;
    }
    else if (fs < 9.0f)
    {
      ism330is_xl_full_scale_set(p_sensor_drv, ISM330IS_8g);
      fs = 8.0f;
    }
    else
    {
      ism330is_xl_full_scale_set(p_sensor_drv, ISM330IS_16g);
      fs = 16.0f;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->acc_sensor_status.type.mems.fs = fs;
      _this->acc_sensor_status.type.mems.sensitivity = 0.0000305f * _this->acc_sensor_status.type.mems.fs;
    }
  }
  else if (id == _this->gyro_id)
  {
    if (fs < 126.0f)
    {
      ism330is_gy_full_scale_set(p_sensor_drv, ISM330IS_125dps);
      fs = 125.0f;
    }
    else if (fs < 251.0f)
    {
      ism330is_gy_full_scale_set(p_sensor_drv, ISM330IS_250dps);
      fs = 250.0f;
    }
    else if (fs < 501.0f)
    {
      ism330is_gy_full_scale_set(p_sensor_drv, ISM330IS_500dps);
      fs = 500.0f;
    }
    else if (fs < 1001.0f)
    {
      ism330is_gy_full_scale_set(p_sensor_drv, ISM330IS_1000dps);
      fs = 1000.0f;
    }
    else
    {
      ism330is_gy_full_scale_set(p_sensor_drv, ISM330IS_2000dps);
      fs = 2000.0f;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->gyro_sensor_status.type.mems.fs = fs;
      _this->gyro_sensor_status.type.mems.sensitivity = 0.035f * _this->gyro_sensor_status.type.mems.fs;
    }
  }
  else if (id == _this->ispu_id)
  {
    res = SYS_TASK_INVALID_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("ISM330IS: warning - ISPU SetFS() not supported.\r\n"));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t ISM330ISTaskSensorEnable(ISM330ISTask *_this, SMMessage report)
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
  else if (id == _this->gyro_id)
  {
    _this->gyro_sensor_status.is_active = TRUE;

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

static sys_error_code_t ISM330ISTaskSensorDisable(ISM330ISTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    _this->acc_sensor_status.is_active = FALSE;
    ism330is_xl_data_rate_set(p_sensor_drv, ISM330IS_XL_ODR_OFF);

    /* Changing sensor configuration must disable ISPU sensor: ISPU can work properly only when setup from UCF */
    _this->ispu_enable = FALSE;
    _this->ispu_sensor_status.is_active = FALSE;
  }
  else if (id == _this->gyro_id)
  {
    _this->gyro_sensor_status.is_active = FALSE;
    ism330is_gy_data_rate_set(p_sensor_drv, ISM330IS_GY_ODR_OFF);

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

static boolean_t ISM330ISTaskSensorIsActive(const ISM330ISTask *_this)
{
  assert_param(_this != NULL);
  return (_this->acc_sensor_status.is_active || _this->gyro_sensor_status.is_active);
}

static sys_error_code_t ISM330ISTaskEnterLowPowerMode(const ISM330ISTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  ism330is_xl_data_rate_t ism330is_odr_xl = ISM330IS_XL_ODR_OFF;
  ism330is_gy_data_rate_t ism330is_odr_g = ISM330IS_GY_ODR_OFF;

  ism330is_xl_data_rate_set(p_sensor_drv, ism330is_odr_xl);
  ism330is_gy_data_rate_set(p_sensor_drv, ism330is_odr_g);

  return res;
}

static sys_error_code_t ISM330ISTaskConfigureIrqPin(const ISM330ISTask *_this, boolean_t LowPower)
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

static sys_error_code_t ISM330ISTaskConfigureISPUPin(const ISM330ISTask *_this, boolean_t LowPower)
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

static inline ISM330ISTask *ISM330ISTaskGetOwnerFromISensorIF(ISensor_t *p_if)
{
  assert_param(p_if != NULL);
  ISM330ISTask *p_if_owner = NULL;

  /* check if the virtual function has been called from the ispu sensor IF  */
  p_if_owner = (ISM330ISTask *)((uint32_t) p_if - offsetof(ISM330ISTask, ispu_sensor_if));
  if (!(p_if_owner->gyro_sensor_if.vptr == &sTheClass.gyro_sensor_if_vtbl)
      || !(p_if_owner->super.vptr == &sTheClass.vtbl))
  {
    /* then the virtual function has been called from the gyro IF  */
    p_if_owner = (ISM330ISTask *)((uint32_t) p_if - offsetof(ISM330ISTask, gyro_sensor_if));
  }
  if (!(p_if_owner->acc_sensor_if.vptr == &sTheClass.acc_sensor_if_vtbl) || !(p_if_owner->super.vptr == &sTheClass.vtbl))
  {
    /* then the virtual function has been called from the acc IF  */
    p_if_owner = (ISM330ISTask *)((uint32_t) p_if - offsetof(ISM330ISTask, acc_sensor_if));
  }

  return p_if_owner;
}

static inline ISM330ISTask *ISM330ISTaskGetOwnerFromISensorLLIF(ISensorLL_t *p_if)
{
  assert_param(p_if != NULL);
  ISM330ISTask *p_if_owner = NULL;
  p_if_owner = (ISM330ISTask *)((uint32_t) p_if - offsetof(ISM330ISTask, sensor_ll_if));

  return p_if_owner;
}

static void ISM330ISTaskTimerCallbackFunction(ULONG param)
{
  ISM330ISTask *p_obj = (ISM330ISTask *) param;
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

static void ISM330ISTaskISPUTimerCallbackFunction(ULONG param)
{
  ISM330ISTask *p_obj = (ISM330ISTask *) param;
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

void ISM330ISTask_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY_ISPU;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((ISM330ISTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}

void INT2_ISM330IS_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((ISM330ISTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}

static sys_error_code_t ISM330IS_ODR_Sync(ISM330ISTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  float odr = 0.0f;
  ism330is_xl_data_rate_t ism330is_odr_xl;
  if (ism330is_xl_data_rate_get(p_sensor_drv, &ism330is_odr_xl) == 0)
  {
    _this->acc_sensor_status.is_active = TRUE;

    /* Update only the model */
    switch (ism330is_odr_xl)
    {
      case ISM330IS_XL_ODR_OFF:
        _this->acc_sensor_status.is_active = FALSE;
        /* Do not update the model in case of odr = 0 */
        odr = _this->acc_sensor_status.type.mems.odr;
        break;
      case ISM330IS_XL_ODR_AT_12Hz5_HP:
        odr = 12.5f;
        break;
      case ISM330IS_XL_ODR_AT_26H_HP:
        odr = 26.0f;
        break;
      case ISM330IS_XL_ODR_AT_52Hz_HP:
        odr = 52.0f;
        break;
      case ISM330IS_XL_ODR_AT_104Hz_HP:
        odr = 104.0f;
        break;
      case ISM330IS_XL_ODR_AT_208Hz_HP:
        odr = 208.0f;
        break;
      case ISM330IS_XL_ODR_AT_416Hz_HP:
        odr = 416.0f;
        break;
      case ISM330IS_XL_ODR_AT_833Hz_HP:
        odr = 833.0f;
        break;
      case ISM330IS_XL_ODR_AT_1667Hz_HP:
        odr = 1667.0f;
        break;
      case ISM330IS_XL_ODR_AT_3333Hz_HP:
        odr = 3333.0f;
        break;
      case ISM330IS_XL_ODR_AT_6667Hz_HP:
        odr = 6667.0f;
        break;
      default:
        break;
    }
    _this->acc_sensor_status.type.mems.odr = odr;
    _this->acc_sensor_status.type.mems.measured_odr = 0.0f;
  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }

  odr = 0;
  ism330is_gy_data_rate_t ism330is_odr_g;
  if (ism330is_gy_data_rate_get(p_sensor_drv, &ism330is_odr_g) == 0)
  {
    _this->gyro_sensor_status.is_active = TRUE;

    /* Update only the model */
    switch (ism330is_odr_g)
    {
      case ISM330IS_GY_ODR_OFF:
        _this->gyro_sensor_status.is_active = FALSE;
        /* Do not update the model in case of odr = 0 */
        odr = _this->gyro_sensor_status.type.mems.odr;
        break;
      case ISM330IS_GY_ODR_AT_12Hz5_HP:
        odr = 12.5f;
        break;
      case ISM330IS_GY_ODR_AT_26H_HP:
        odr = 26.0f;
        break;
      case ISM330IS_GY_ODR_AT_52Hz_HP:
        odr = 52.0f;
        break;
      case ISM330IS_GY_ODR_AT_104Hz_HP:
        odr = 104.0f;
        break;
      case ISM330IS_GY_ODR_AT_208Hz_HP:
        odr = 208.0f;
        break;
      case ISM330IS_GY_ODR_AT_416Hz_HP:
        odr = 416.0f;
        break;
      case ISM330IS_GY_ODR_AT_833Hz_HP:
        odr = 833.0f;
        break;
      case ISM330IS_GY_ODR_AT_1667Hz_HP:
        odr = 1667.0f;
        break;
      case ISM330IS_GY_ODR_AT_3333Hz_HP:
        odr = 3333.0f;
        break;
      case ISM330IS_GY_ODR_AT_6667Hz_HP:
        odr = 6667.0f;
        break;
      default:
        break;
    }
    _this->gyro_sensor_status.type.mems.odr = odr;
    _this->gyro_sensor_status.type.mems.measured_odr = 0.0f;
  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t ISM330IS_FS_Sync(ISM330ISTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  float full_scale = 2.0;
  ism330is_xl_full_scale_t fs_xl;
  if (ism330is_xl_full_scale_get(p_sensor_drv, &fs_xl) == 0)
  {
    switch (fs_xl)
    {
      case ISM330IS_2g:
        full_scale = 2.0;
        break;
      case ISM330IS_4g:
        full_scale = 4.0;
        break;
      case ISM330IS_8g:
        full_scale = 8.0;
        break;
      case ISM330IS_16g:
        full_scale = 16.0;
        break;
      default:
        break;
    }
    _this->acc_sensor_status.type.mems.fs = full_scale;
    _this->acc_sensor_status.type.mems.sensitivity = 0.0000305f * _this->acc_sensor_status.type.mems.fs;
  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }

  full_scale = 125;
  ism330is_gy_full_scale_t fs_g;
  if (ism330is_gy_full_scale_get(p_sensor_drv, &fs_g) == 0)
  {
    switch (fs_g)
    {
      case ISM330IS_125dps:
        full_scale = 125;
        break;
      case ISM330IS_250dps:
        full_scale = 250;
        break;
      case ISM330IS_500dps:
        full_scale = 500;
        break;
      case ISM330IS_1000dps:
        full_scale = 1000;
        break;
      case ISM330IS_2000dps:
        full_scale = 2000;
        break;
      default:
        break;
    }
    _this->gyro_sensor_status.type.mems.fs = full_scale;
    _this->gyro_sensor_status.type.mems.sensitivity = 0.035f * _this->gyro_sensor_status.type.mems.fs;
  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}
