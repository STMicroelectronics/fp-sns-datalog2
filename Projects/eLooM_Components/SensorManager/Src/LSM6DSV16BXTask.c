/**
  ******************************************************************************
  * @file    LSM6DSV16BXTask.c
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
#include "LSM6DSV16BXTask.h"
#include "LSM6DSV16BXTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "lsm6dsv16bx_reg.h"
#include <string.h>
#include <stdlib.h>
#include "services/sysdebug.h"

/* Private includes ----------------------------------------------------------*/

#ifndef LSM6DSV16BX_TASK_CFG_STACK_DEPTH
#define LSM6DSV16BX_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*8)
#endif

#ifndef LSM6DSV16BX_TASK_CFG_PRIORITY
#define LSM6DSV16BX_TASK_CFG_PRIORITY                 (TX_MAX_PRIORITIES - 1)
#endif

#ifndef LSM6DSV16BX_TASK_CFG_IN_QUEUE_LENGTH
#define LSM6DSV16BX_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define LSM6DSV16BX_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef LSM6DSV16BX_TASK_CFG_TIMER_PERIOD_MS
#define LSM6DSV16BX_TASK_CFG_TIMER_PERIOD_MS          1000
#endif
#ifndef LSM6DSV16BX_TASK_CFG_MLC_TIMER_PERIOD_MS
#define LSM6DSV16BX_TASK_CFG_MLC_TIMER_PERIOD_MS      200
#endif

#define LSM6DSV16BX_TAG_ACC                           (0x02)

#ifndef LSM6DSV16BX_TASK_CFG_MAX_INSTANCES_COUNT
#define LSM6DSV16BX_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)                   SYS_DEBUGF3(SYS_DBG_LSM6DSV16BX, level, message)

#ifndef LSM6DSV16BX_TASK_CFG_I2C_ADDRESS
#define LSM6DSV16BX_TASK_CFG_I2C_ADDRESS             LSM6DSV16BX_I2C_ADD_H
#endif

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
typedef struct _LSM6DSV16BXTaskClass
{
  /**
    * LSM6DSV16BXTask class virtual table.
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
  const SensorDescriptor_t acc_class_descriptor;

  /**
    * Specifies gyroscope sensor capabilities.
    */
  const SensorDescriptor_t gyro_class_descriptor;

  /**
    * Specifies mlc sensor capabilities.
    */
  const SensorDescriptor_t mlc_class_descriptor;

  /**
    * LSM6DSV16BXTask (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[2 * LSM6DSV16BX_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} LSM6DSV16BXTaskClass_t;

/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LSM6DSV16BXTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LSM6DSV16BXTaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LSM6DSV16BXTaskSensorInit(LSM6DSV16BXTask *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LSM6DSV16BXTaskSensorReadData(LSM6DSV16BXTask *_this);

/**
  * Read the data from the mlc.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LSM6DSV16BXTaskSensorReadMLC(LSM6DSV16BXTask *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t LSM6DSV16BXTaskSensorRegister(LSM6DSV16BXTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t LSM6DSV16BXTaskSensorInitTaskParams(LSM6DSV16BXTask *_this);

/**
  * Private implementation of sensor interface methods for LSM6DSV16BX sensor
  */
static sys_error_code_t LSM6DSV16BXTaskSensorSetODR(LSM6DSV16BXTask *_this, SMMessage report);
static sys_error_code_t LSM6DSV16BXTaskSensorSetFS(LSM6DSV16BXTask *_this, SMMessage report);
static sys_error_code_t LSM6DSV16BXTaskSensorSetFifoWM(LSM6DSV16BXTask *_this, SMMessage report);
static sys_error_code_t LSM6DSV16BXTaskSensorEnable(LSM6DSV16BXTask *_this, SMMessage report);
static sys_error_code_t LSM6DSV16BXTaskSensorDisable(LSM6DSV16BXTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t LSM6DSV16BXTaskSensorIsActive(const LSM6DSV16BXTask *_this);

static sys_error_code_t LSM6DSV16BXTaskEnterLowPowerMode(const LSM6DSV16BXTask *_this);

static sys_error_code_t LSM6DSV16BXTaskConfigureIrqPin(const LSM6DSV16BXTask *_this, boolean_t LowPower);
static sys_error_code_t LSM6DSV16BXTaskConfigureMLCPin(const LSM6DSV16BXTask *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void LSM6DSV16BXTaskTimerCallbackFunction(ULONG param);

/**
  * Callback function called when the mlc software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void LSM6DSV16BXTaskMLCTimerCallbackFunction(ULONG param);

/**
  * Given a interface pointer it return the instance of the object that implement the interface.
  *
  * @param p_if [IN] specifies a sensor interface implemented by the task object.
  * @return the instance of the task object that implements the given interface.
  */
static inline LSM6DSV16BXTask *LSM6DSV16BXTaskGetOwnerFromISensorIF(ISensor_t *p_if);

/**
  * Given a interface pointer it return the instance of the object that implement the interface.
  *
  * @param p_if [IN] specifies a ISensorLL interface implemented by the task object.
  * @return the instance of the task object that implements the given interface.
  */
static inline LSM6DSV16BXTask *LSM6DSV16BXTaskGetOwnerFromISensorLLIF(ISensorLL_t *p_if);

/**
  * Read the odr value from the sensor and update the internal model
  */
static sys_error_code_t LSM6DSV16BX_ODR_Sync(LSM6DSV16BXTask *_this);

/**
  * Read the odr value from the sensor and update the internal model
  */
static sys_error_code_t LSM6DSV16BX_FS_Sync(LSM6DSV16BXTask *_this);

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
static inline sys_error_code_t LSM6DSV16BXTaskPostReportToFront(LSM6DSV16BXTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t LSM6DSV16BXTaskPostReportToBack(LSM6DSV16BXTask *_this, SMMessage *pReport);

/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static LSM6DSV16BXTask sTaskObj;
/**
  * The class object.
  */
static LSM6DSV16BXTaskClass_t sTheClass =
{
  /* class virtual table */
  {
    LSM6DSV16BXTask_vtblHardwareInit,
    LSM6DSV16BXTask_vtblOnCreateTask,
    LSM6DSV16BXTask_vtblDoEnterPowerMode,
    LSM6DSV16BXTask_vtblHandleError,
    LSM6DSV16BXTask_vtblOnEnterTaskControlLoop,
    LSM6DSV16BXTask_vtblForceExecuteStep,
    LSM6DSV16BXTask_vtblOnEnterPowerMode
  },

  /* class::acc_sensor_if_vtbl virtual table */
  {
    {
      {
        LSM6DSV16BXTask_vtblAccGetId,
        LSM6DSV16BXTask_vtblAccGetEventSourceIF,
        LSM6DSV16BXTask_vtblAccGetDataInfo
      },
      LSM6DSV16BXTask_vtblSensorEnable,
      LSM6DSV16BXTask_vtblSensorDisable,
      LSM6DSV16BXTask_vtblSensorIsEnabled,
      LSM6DSV16BXTask_vtblAccGetDescription,
      LSM6DSV16BXTask_vtblAccGetStatus,
      LSM6DSV16BXTask_vtblAccGetStatusPointer
    },
    LSM6DSV16BXTask_vtblAccGetODR,
    LSM6DSV16BXTask_vtblAccGetFS,
    LSM6DSV16BXTask_vtblAccGetSensitivity,
    LSM6DSV16BXTask_vtblSensorSetODR,
    LSM6DSV16BXTask_vtblSensorSetFS,
    LSM6DSV16BXTask_vtblSensorSetFifoWM
  },

  /* class::gyro_sensor_if_vtbl virtual table */
  {
    {
      {
        LSM6DSV16BXTask_vtblGyroGetId,
        LSM6DSV16BXTask_vtblGyroGetEventSourceIF,
        LSM6DSV16BXTask_vtblGyroGetDataInfo
      },
      LSM6DSV16BXTask_vtblSensorEnable,
      LSM6DSV16BXTask_vtblSensorDisable,
      LSM6DSV16BXTask_vtblSensorIsEnabled,
      LSM6DSV16BXTask_vtblGyroGetDescription,
      LSM6DSV16BXTask_vtblGyroGetStatus,
      LSM6DSV16BXTask_vtblGyroGetStatusPointer
    },
    LSM6DSV16BXTask_vtblGyroGetODR,
    LSM6DSV16BXTask_vtblGyroGetFS,
    LSM6DSV16BXTask_vtblGyroGetSensitivity,
    LSM6DSV16BXTask_vtblSensorSetODR,
    LSM6DSV16BXTask_vtblSensorSetFS,
    LSM6DSV16BXTask_vtblSensorSetFifoWM
  },

  /* class::mlc_sensor_if_vtbl virtual table */
  {
    {
      {
        LSM6DSV16BXTask_vtblMlcGetId,
        LSM6DSV16BXTask_vtblMlcGetEventSourceIF,
        LSM6DSV16BXTask_vtblMlcGetDataInfo
      },
      LSM6DSV16BXTask_vtblSensorEnable,
      LSM6DSV16BXTask_vtblSensorDisable,
      LSM6DSV16BXTask_vtblSensorIsEnabled,
      LSM6DSV16BXTask_vtblMlcGetDescription,
      LSM6DSV16BXTask_vtblMlcGetStatus,
      LSM6DSV16BXTask_vtblMlcGetStatusPointer
    },
    LSM6DSV16BXTask_vtblMlcGetODR,
    LSM6DSV16BXTask_vtblMlcGetFS,
    LSM6DSV16BXTask_vtblMlcGetSensitivity,
    LSM6DSV16BXTask_vtblSensorSetODR,
    LSM6DSV16BXTask_vtblSensorSetFS,
    LSM6DSV16BXTask_vtblSensorSetFifoWM
  },

  /* class::sensor_ll_if_vtbl virtual table */
  {
    LSM6DSV16BXTask_vtblSensorReadReg,
    LSM6DSV16BXTask_vtblSensorWriteReg,
    LSM6DSV16BXTask_vtblSensorSyncModel
  },

  /* ACCELEROMETER DESCRIPTOR */
  {
    "lsm6dsv16bx",
    COM_TYPE_ACC
  },

  /* GYROSCOPE DESCRIPTOR */
  {
    "lsm6dsv16bx",
    COM_TYPE_GYRO
  },

  /* MLC DESCRIPTOR */
  {
    "lsm6dsv16bx",
    COM_TYPE_MLC
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    LSM6DSV16BXTaskExecuteStepState1,
    NULL,
    LSM6DSV16BXTaskExecuteStepDatalog,
  },

  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *LSM6DSV16BXTaskGetAccSensorIF(LSM6DSV16BXTask *_this)
{
  return (ISourceObservable *) & (_this->acc_sensor_if);
}

ISourceObservable *LSM6DSV16BXTaskGetGyroSensorIF(LSM6DSV16BXTask *_this)
{
  return (ISourceObservable *) & (_this->gyro_sensor_if);
}

ISourceObservable *LSM6DSV16BXTaskGetMlcSensorIF(LSM6DSV16BXTask *_this)
{
  return (ISourceObservable *) & (_this->mlc_sensor_if);
}

ISensorLL_t *LSM6DSV16BXTaskGetSensorLLIF(LSM6DSV16BXTask *_this)
{
  return (ISensorLL_t *) & (_this->sensor_ll_if);
}

AManagedTaskEx *LSM6DSV16BXTaskAlloc(const void *pIRQConfig, const void *pMLCConfig, const void *pCSConfig)
{
  LSM6DSV16BXTask *p_new_obj = SysAlloc(sizeof(LSM6DSV16BXTask));

  if (p_new_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_new_obj->super);

    p_new_obj->super.vptr = &sTheClass.vtbl;
    p_new_obj->acc_sensor_if.vptr = &sTheClass.acc_sensor_if_vtbl;
    p_new_obj->gyro_sensor_if.vptr = &sTheClass.gyro_sensor_if_vtbl;
    p_new_obj->mlc_sensor_if.vptr = &sTheClass.mlc_sensor_if_vtbl;
    p_new_obj->sensor_ll_if.vptr = &sTheClass.sensor_ll_if_vtbl;
    p_new_obj->acc_sensor_descriptor = &sTheClass.acc_class_descriptor;
    p_new_obj->gyro_sensor_descriptor = &sTheClass.gyro_class_descriptor;
    p_new_obj->mlc_sensor_descriptor = &sTheClass.mlc_class_descriptor;

    p_new_obj->pIRQConfig = (MX_GPIOParams_t *) pIRQConfig;
    p_new_obj->pMLCConfig = (MX_GPIOParams_t *) pMLCConfig;
    p_new_obj->pCSConfig = (MX_GPIOParams_t *) pCSConfig;

    strcpy(p_new_obj->acc_sensor_status.p_name, sTheClass.acc_class_descriptor.p_name);
    strcpy(p_new_obj->gyro_sensor_status.p_name, sTheClass.gyro_class_descriptor.p_name);
    strcpy(p_new_obj->mlc_sensor_status.p_name, sTheClass.mlc_class_descriptor.p_name);
  }

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *LSM6DSV16BXTaskAllocSetName(const void *pIRQConfig, const void *pMLCConfig, const void *pCSConfig,
                                            const char *p_name)
{
  LSM6DSV16BXTask *p_new_obj = (LSM6DSV16BXTask *) LSM6DSV16BXTaskAlloc(pIRQConfig, pMLCConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->acc_sensor_status.p_name, p_name);
  strcpy(p_new_obj->gyro_sensor_status.p_name, p_name);
  strcpy(p_new_obj->mlc_sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *LSM6DSV16BXTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pMLCConfig,
                                           const void *pCSConfig)
{
  LSM6DSV16BXTask *p_obj = (LSM6DSV16BXTask *) p_mem_block;

  if (p_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_obj->super);

    p_obj->super.vptr = &sTheClass.vtbl;
    p_obj->acc_sensor_if.vptr = &sTheClass.acc_sensor_if_vtbl;
    p_obj->gyro_sensor_if.vptr = &sTheClass.gyro_sensor_if_vtbl;
    p_obj->mlc_sensor_if.vptr = &sTheClass.mlc_sensor_if_vtbl;
    p_obj->sensor_ll_if.vptr = &sTheClass.sensor_ll_if_vtbl;
    p_obj->acc_sensor_descriptor = &sTheClass.acc_class_descriptor;
    p_obj->gyro_sensor_descriptor = &sTheClass.gyro_class_descriptor;
    p_obj->mlc_sensor_descriptor = &sTheClass.mlc_class_descriptor;

    p_obj->pIRQConfig = (MX_GPIOParams_t *) pIRQConfig;
    p_obj->pMLCConfig = (MX_GPIOParams_t *) pMLCConfig;
    p_obj->pCSConfig = (MX_GPIOParams_t *) pCSConfig;
  }

  return (AManagedTaskEx *) p_obj;
}

AManagedTaskEx *LSM6DSV16BXTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pMLCConfig,
                                                  const void *pCSConfig, const char *p_name)
{
  LSM6DSV16BXTask *p_obj = (LSM6DSV16BXTask *) LSM6DSV16BXTaskStaticAlloc(p_mem_block, pIRQConfig, pMLCConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->acc_sensor_status.p_name, p_name);
  strcpy(p_obj->gyro_sensor_status.p_name, p_name);
  strcpy(p_obj->mlc_sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *LSM6DSV16BXTaskGetSensorIF(LSM6DSV16BXTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *LSM6DSV16BXTaskGetAccEventSrcIF(LSM6DSV16BXTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_acc_event_src;
}

IEventSrc *LSM6DSV16BXTaskGetGyroEventSrcIF(LSM6DSV16BXTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_gyro_event_src;
}

IEventSrc *LSM6DSV16BXTaskGetMlcEventSrcIF(LSM6DSV16BXTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_mlc_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t LSM6DSV16BXTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16BXTask *p_obj = (LSM6DSV16BXTask *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t LSM6DSV16BXTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                                  VOID **pvStackStart,
                                                  ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                                  ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16BXTask *p_obj = (LSM6DSV16BXTask *) _this;
  p_obj->sync = true;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) LSM6DSV16BX_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(LSM6DSV16BX_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "LSM6DSV16BX_Q", item_size / 4u, p_queue_items_buff,
                                    LSM6DSV16BX_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_timer, "LSM6DSV16BX_T", LSM6DSV16BXTaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(LSM6DSV16BX_TASK_CFG_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* create the mlc software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->mlc_timer, "LSM6DSV16BX_MLC_T", LSM6DSV16BXTaskMLCTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(LSM6DSV16BX_TASK_CFG_MLC_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(LSM6DSV16BX_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(LSM6DSV16BX_ID, LSM6DSV16BX_TASK_CFG_I2C_ADDRESS, 0);
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
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, 2 * LSM6DSV16BX_TASK_CFG_MAX_INSTANCES_COUNT);
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

#if LSM6DSV16BX_FIFO_ENABLED
  memset(p_obj->p_fast_sensor_data_buff, 0, sizeof(p_obj->p_fast_sensor_data_buff));
  memset(p_obj->p_slow_sensor_data_buff, 0, sizeof(p_obj->p_slow_sensor_data_buff));
#else
  memset(p_obj->p_acc_sample, 0, sizeof(p_obj->p_acc_sample));
  memset(p_obj->p_gyro_sample, 0, sizeof(p_obj->p_gyro_sample));
  p_obj->acc_drdy = 0;
  p_obj->gyro_drdy = 0;
#endif
  memset(p_obj->p_mlc_sensor_data_buff, 0, sizeof(p_obj->p_mlc_sensor_data_buff));
  p_obj->acc_id = 0;
  p_obj->gyro_id = 1;
  p_obj->mlc_enable = FALSE;
  p_obj->prev_timestamp = 0.0f;
  p_obj->acc_samples_count = 0;
  p_obj->gyro_samples_count = 0;
  p_obj->fifo_level = 0;
  p_obj->samples_per_it = 0;
  p_obj->first_data_ready = 0;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "LSM6DSV16BX";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = LSM6DSV16BX_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = LSM6DSV16BX_TASK_CFG_PRIORITY;
  *pPreemptThreshold = LSM6DSV16BX_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = LSM6DSV16BXTaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = LSM6DSV16BXTaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV16BX: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t LSM6DSV16BXTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                      const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16BXTask *p_obj = (LSM6DSV16BXTask *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (LSM6DSV16BXTaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV16BX: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      if (LSM6DSV16BXTaskSensorIsActive(p_obj))
      {
        /* Deactivate the sensor */
        lsm6dsv16bx_xl_data_rate_set(p_sensor_drv, LSM6DSV16BX_XL_ODR_OFF);
        lsm6dsv16bx_gy_data_rate_set(p_sensor_drv, LSM6DSV16BX_GY_ODR_OFF);
        lsm6dsv16bx_fifo_gy_batch_set(p_sensor_drv, LSM6DSV16BX_GY_NOT_BATCHED);
        lsm6dsv16bx_fifo_xl_batch_set(p_sensor_drv, LSM6DSV16BX_XL_NOT_BATCHED);
        lsm6dsv16bx_fifo_mode_set(p_sensor_drv, LSM6DSV16BX_BYPASS_MODE);
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
        LSM6DSV16BXTaskConfigureIrqPin(p_obj, TRUE);
      }
      if (p_obj->pMLCConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->mlc_timer);
      }
      else
      {
        LSM6DSV16BXTaskConfigureMLCPin(p_obj, TRUE);
      }
      memset(p_obj->p_mlc_sensor_data_buff, 0, sizeof(p_obj->p_mlc_sensor_data_buff));
    }
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV16BX: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = LSM6DSV16BXTaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV16BX - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->pIRQConfig != NULL)
    {
      LSM6DSV16BXTaskConfigureIrqPin(p_obj, TRUE);
    }
    if (p_obj->pMLCConfig != NULL)
    {
      LSM6DSV16BXTaskConfigureMLCPin(p_obj, TRUE);
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV16BX: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t LSM6DSV16BXTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  LSM6DSV16BXTask *p_obj = (LSM6DSV16BXTask*)_this;

  return res;
}

sys_error_code_t LSM6DSV16BXTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("LSM6DSV16BX: start.\r\n"));

#if defined (ENABLE_THREADX_DBG_PIN) && defined (LSM6DSV16BX_TASK_CFG_TAG)
  LSM6DSV16BXTask *p_obj = (LSM6DSV16BXTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = LSM6DSV16BX_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t LSM6DSV16BXTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16BXTask *p_obj = (LSM6DSV16BXTask *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = LSM6DSV16BXTaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t LSM6DSV16BXTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                      const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  LSM6DSV16BXTask *p_obj = (LSM6DSV16BXTask*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t LSM6DSV16BXTask_vtblAccGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = (LSM6DSV16BXTask *)((uint32_t) _this - offsetof(LSM6DSV16BXTask, acc_sensor_if));
  uint8_t res = p_if_owner->acc_id;

  return res;
}

uint8_t LSM6DSV16BXTask_vtblGyroGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = (LSM6DSV16BXTask *)((uint32_t) _this - offsetof(LSM6DSV16BXTask, gyro_sensor_if));
  uint8_t res = p_if_owner->gyro_id;

  return res;
}

uint8_t LSM6DSV16BXTask_vtblMlcGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = (LSM6DSV16BXTask *)((uint32_t) _this - offsetof(LSM6DSV16BXTask, mlc_sensor_if));
  uint8_t res = p_if_owner->mlc_id;

  return res;
}

IEventSrc *LSM6DSV16BXTask_vtblAccGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = (LSM6DSV16BXTask *)((uint32_t) _this - offsetof(LSM6DSV16BXTask, acc_sensor_if));

  return p_if_owner->p_acc_event_src;
}

IEventSrc *LSM6DSV16BXTask_vtblGyroGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = (LSM6DSV16BXTask *)((uint32_t) _this - offsetof(LSM6DSV16BXTask, gyro_sensor_if));
  return p_if_owner->p_gyro_event_src;
}

IEventSrc *LSM6DSV16BXTask_vtblMlcGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = (LSM6DSV16BXTask *)((uint32_t) _this - offsetof(LSM6DSV16BXTask, mlc_sensor_if));
  return p_if_owner->p_mlc_event_src;
}

sys_error_code_t LSM6DSV16BXTask_vtblAccGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LSM6DSV16BXTask *p_if_owner = (LSM6DSV16BXTask *)((uint32_t) _this - offsetof(LSM6DSV16BXTask, acc_sensor_if));
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

float LSM6DSV16BXTask_vtblAccGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = (LSM6DSV16BXTask *)((uint32_t) _this - offsetof(LSM6DSV16BXTask, acc_sensor_if));
  float res = p_if_owner->acc_sensor_status.type.mems.fs;

  return res;
}

float LSM6DSV16BXTask_vtblAccGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = (LSM6DSV16BXTask *)((uint32_t) _this - offsetof(LSM6DSV16BXTask, acc_sensor_if));
  float res = p_if_owner->acc_sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t LSM6DSV16BXTask_vtblAccGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = (LSM6DSV16BXTask *)((uint32_t) _this - offsetof(LSM6DSV16BXTask, acc_sensor_if));
  EMData_t res = p_if_owner->data_acc;

  return res;
}

sys_error_code_t LSM6DSV16BXTask_vtblGyroGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LSM6DSV16BXTask *p_if_owner = (LSM6DSV16BXTask *)((uint32_t) _this - offsetof(LSM6DSV16BXTask, gyro_sensor_if));
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

float LSM6DSV16BXTask_vtblGyroGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = (LSM6DSV16BXTask *)((uint32_t) _this - offsetof(LSM6DSV16BXTask, gyro_sensor_if));
  float res = p_if_owner->gyro_sensor_status.type.mems.fs;

  return res;
}

float LSM6DSV16BXTask_vtblGyroGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = (LSM6DSV16BXTask *)((uint32_t) _this - offsetof(LSM6DSV16BXTask, gyro_sensor_if));
  float res = p_if_owner->gyro_sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t LSM6DSV16BXTask_vtblGyroGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = (LSM6DSV16BXTask *)((uint32_t) _this - offsetof(LSM6DSV16BXTask, gyro_sensor_if));
  EMData_t res = p_if_owner->data_gyro;

  return res;
}

sys_error_code_t LSM6DSV16BXTask_vtblMlcGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LSM6DSV16BXTask *p_if_owner = (LSM6DSV16BXTask *)((uint32_t) _this - offsetof(LSM6DSV16BXTask, mlc_sensor_if));
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

float LSM6DSV16BXTask_vtblMlcGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);

  /* MLC does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV16BX: warning - MLC GetFS() not supported.\r\n"));

  return -1.0f;
}

float LSM6DSV16BXTask_vtblMlcGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);

  /* MLC does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV16BX: warning - MLC GetSensitivity() not supported.\r\n"));

  return -1.0f;
}

EMData_t LSM6DSV16BXTask_vtblMlcGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = (LSM6DSV16BXTask *)((uint32_t) _this - offsetof(LSM6DSV16BXTask, mlc_sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t LSM6DSV16BXTask_vtblSensorSetODR(ISensorMems_t *_this, float odr)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorIF((ISensor_t *)_this);

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
      else if (sensor_id == p_if_owner->gyro_id)
      {
        p_if_owner->gyro_sensor_status.type.mems.odr = odr;
        p_if_owner->gyro_sensor_status.type.mems.measured_odr = 0.0f;
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
    res = LSM6DSV16BXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t LSM6DSV16BXTask_vtblSensorSetFS(ISensorMems_t *_this, float fs)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorIF((ISensor_t *)_this);

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
      p_if_owner->acc_sensor_status.type.mems.sensitivity = 0.0000305f * p_if_owner->acc_sensor_status.type.mems.fs;
    }
    else if (sensor_id == p_if_owner->gyro_id)
    {
      p_if_owner->gyro_sensor_status.type.mems.fs = fs;
      p_if_owner->gyro_sensor_status.type.mems.sensitivity = 0.000035f * p_if_owner->gyro_sensor_status.type.mems.fs;
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
    res = LSM6DSV16BXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t LSM6DSV16BXTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorIF((ISensor_t *)_this);

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
    res = LSM6DSV16BXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t LSM6DSV16BXTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorIF(_this);

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
      p_if_owner->acc_sensor_status.is_active = TRUE;
    }
    else if (sensor_id == p_if_owner->gyro_id)
    {
      p_if_owner->gyro_sensor_status.is_active = TRUE;
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
    res = LSM6DSV16BXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t LSM6DSV16BXTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorIF(_this);

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
      p_if_owner->acc_sensor_status.is_active = FALSE;
    }
    else if (sensor_id == p_if_owner->gyro_id)
    {
      p_if_owner->gyro_sensor_status.is_active = FALSE;
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
    res = LSM6DSV16BXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t LSM6DSV16BXTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorIF(_this);

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->acc_id)
  {
    res = p_if_owner->acc_sensor_status.is_active;
  }
  else if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->gyro_id)
  {
    res = p_if_owner->gyro_sensor_status.is_active;
  }
  else if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->mlc_id)
  {
    res = p_if_owner->mlc_sensor_status.is_active;
  }

  return res;
}

SensorDescriptor_t LSM6DSV16BXTask_vtblAccGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->acc_sensor_descriptor;
}

SensorDescriptor_t LSM6DSV16BXTask_vtblGyroGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->gyro_sensor_descriptor;
}

SensorDescriptor_t LSM6DSV16BXTask_vtblMlcGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->mlc_sensor_descriptor;
}

SensorStatus_t LSM6DSV16BXTask_vtblAccGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->acc_sensor_status;
}

SensorStatus_t LSM6DSV16BXTask_vtblGyroGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->gyro_sensor_status;
}

SensorStatus_t LSM6DSV16BXTask_vtblMlcGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->mlc_sensor_status;
}

SensorStatus_t *LSM6DSV16BXTask_vtblAccGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorIF(_this);
  return &p_if_owner->acc_sensor_status;
}

SensorStatus_t *LSM6DSV16BXTask_vtblGyroGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorIF(_this);
  return &p_if_owner->gyro_sensor_status;
}

SensorStatus_t *LSM6DSV16BXTask_vtblMlcGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorIF(_this);
  return &p_if_owner->mlc_sensor_status;
}

sys_error_code_t LSM6DSV16BXTask_vtblSensorReadReg(ISensorLL_t *_this, uint16_t reg, uint8_t *data, uint16_t len)
{
  assert_param(_this != NULL);
  assert_param(reg <= 0xFFU);
  assert_param(data != NULL);
  assert_param(len != 0U);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorLLIF(_this);
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_if_owner->p_sensor_bus_if->m_xConnector;
  uint8_t reg8 = (uint8_t)(reg & 0x00FF);

  if (lsm6dsv16bx_read_reg(p_sensor_drv, reg8, data, len) != 0)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t LSM6DSV16BXTask_vtblSensorWriteReg(ISensorLL_t *_this, uint16_t reg, const uint8_t *data, uint16_t len)
{
  assert_param(_this != NULL);
  assert_param(reg <= 0xFFU);
  assert_param(data != NULL);
  assert_param(len != 0U);

  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorLLIF(_this);
  uint8_t reg8 = (uint8_t)(reg & 0x00FF);

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_if_owner->p_sensor_bus_if->m_xConnector;

  /* This generic register write operation could mean that the model is out of sync with the HW */
  p_if_owner->sync = false;

  if (lsm6dsv16bx_write_reg(p_sensor_drv, reg8, (uint8_t *) data, len) != 0)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t LSM6DSV16BXTask_vtblSensorSyncModel(ISensorLL_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16BXTask *p_if_owner = LSM6DSV16BXTaskGetOwnerFromISensorLLIF(_this);

  if (LSM6DSV16BX_ODR_Sync(p_if_owner) != SYS_NO_ERROR_CODE)
  {
    res = SYS_BASE_ERROR_CODE;
  }
  if (LSM6DSV16BX_FS_Sync(p_if_owner) != SYS_NO_ERROR_CODE)
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
static sys_error_code_t LSM6DSV16BXTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16BXTask *p_obj = (LSM6DSV16BXTask *) _this;
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
            res = LSM6DSV16BXTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = LSM6DSV16BXTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = LSM6DSV16BXTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = LSM6DSV16BXTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = LSM6DSV16BXTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV16BX: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV16BX: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t LSM6DSV16BXTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16BXTask *p_obj = (LSM6DSV16BXTask *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("LSM6DSV16BX: new data.\r\n"));
        res = LSM6DSV16BXTaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          if (p_obj->first_data_ready == 4)
          {
#if LSM6DSV16BX_FIFO_ENABLED
            if (p_obj->fifo_level != 0)
            {
#endif
              // notify the listeners...
              double timestamp = report.sensorDataReadyMessage.fTimestamp;
              double delta_timestamp = timestamp - p_obj->prev_timestamp;
              p_obj->prev_timestamp = timestamp;

              DataEvent_t evt_acc, evt_gyro;

#if LSM6DSV16BX_FIFO_ENABLED
              if ((p_obj->acc_sensor_status.is_active) && (p_obj->gyro_sensor_status.is_active)) /* Read both ACC and GYRO */
              {
                if (p_obj->acc_sensor_status.type.mems.odr > p_obj->gyro_sensor_status.type.mems.odr) /* Acc is faster than Gyro */
                {
                  /* update measuredODR */
                  p_obj->acc_sensor_status.type.mems.measured_odr = (float) p_obj->acc_samples_count / (float) delta_timestamp;
                  p_obj->gyro_sensor_status.type.mems.measured_odr = (p_obj->gyro_sensor_status.type.mems.odr / p_obj->acc_sensor_status.type.mems.odr) * p_obj->acc_sensor_status.type.mems.measured_odr;

                  /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue:
                   * [X0, Y0, Z0]
                   * [X1, Y1, Z1]
                   * ...
                   * [Xm-1, Ym-1, Zm-1]
                   */
                  if (p_obj->acc_samples_count != 0)
                  {
                    EMD_Init(&p_obj->data_acc, p_obj->p_fast_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                             p_obj->acc_samples_count, 3);
                    DataEventInit((IEvent *) &evt_acc, p_obj->p_acc_event_src, &p_obj->data_acc, timestamp, p_obj->acc_id);
                    IEventSrcSendEvent(p_obj->p_acc_event_src, (IEvent *) &evt_acc, NULL);
                  }
                  if (p_obj->gyro_samples_count != 0)
                  {
                    EMD_Init(&p_obj->data_gyro, p_obj->p_slow_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                             p_obj->gyro_samples_count, 3);
                    DataEventInit((IEvent *) &evt_gyro, p_obj->p_gyro_event_src, &p_obj->data_gyro, timestamp, p_obj->gyro_id);
                    IEventSrcSendEvent(p_obj->p_gyro_event_src, (IEvent *) &evt_gyro, NULL);
                  }
                }
                else
                {
                  /* update measuredODR */
                  p_obj->gyro_sensor_status.type.mems.measured_odr = (float) p_obj->gyro_samples_count / (float) delta_timestamp;
                  p_obj->acc_sensor_status.type.mems.measured_odr = (p_obj->acc_sensor_status.type.mems.odr / p_obj->gyro_sensor_status.type.mems.odr) * p_obj->gyro_sensor_status.type.mems.measured_odr;

                  /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue:
                   * [X0, Y0, Z0]
                   * [X1, Y1, Z1]
                   * ...
                   * [Xm-1, Ym-1, Zm-1]
                   */
                  if (p_obj->acc_samples_count != 0)
                  {
                    EMD_Init(&p_obj->data_acc, p_obj->p_slow_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                             p_obj->acc_samples_count, 3);
                    DataEventInit((IEvent *) &evt_acc, p_obj->p_acc_event_src, &p_obj->data_acc, timestamp, p_obj->acc_id);
                    IEventSrcSendEvent(p_obj->p_acc_event_src, (IEvent *) &evt_acc, NULL);
                  }
                  if (p_obj->gyro_samples_count != 0)
                  {
                    EMD_Init(&p_obj->data_gyro, p_obj->p_fast_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                             p_obj->gyro_samples_count, 3);
                    DataEventInit((IEvent *) &evt_gyro, p_obj->p_gyro_event_src, &p_obj->data_gyro, timestamp, p_obj->gyro_id);
                    IEventSrcSendEvent(p_obj->p_gyro_event_src, (IEvent *) &evt_gyro, NULL);
                  }
                }

              }
              else /* Only 1 out of 2 is active */
              {
                if (p_obj->acc_sensor_status.is_active)
                {
                  /* update measuredODR */
                  p_obj->acc_sensor_status.type.mems.measured_odr = (float) p_obj->acc_samples_count / (float) delta_timestamp;

                  /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue:
                   * [X0, Y0, Z0]
                   * [X1, Y1, Z1]
                   * ...
                   * [Xm-1, Ym-1, Zm-1]
                   */
                  EMD_Init(&p_obj->data_acc, p_obj->p_fast_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                           p_obj->acc_samples_count, 3);
                  DataEventInit((IEvent *) &evt_acc, p_obj->p_acc_event_src, &p_obj->data_acc, timestamp, p_obj->acc_id);

                  IEventSrcSendEvent(p_obj->p_acc_event_src, (IEvent *) &evt_acc, NULL);
                }
                else if (p_obj->gyro_sensor_status.is_active)
                {
                  /* update measuredODR */
                  p_obj->gyro_sensor_status.type.mems.measured_odr = (float) p_obj->gyro_samples_count / (float) delta_timestamp;

                  /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue:
                   * [X0, Y0, Z0]
                   * [X1, Y1, Z1]
                   * ...
                   * [Xm-1, Ym-1, Zm-1]
                   */
                  EMD_Init(&p_obj->data_gyro, p_obj->p_fast_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                           p_obj->gyro_samples_count, 3);
                  DataEventInit((IEvent *) &evt_gyro, p_obj->p_gyro_event_src, &p_obj->data_gyro, timestamp, p_obj->gyro_id);

                  IEventSrcSendEvent(p_obj->p_gyro_event_src, (IEvent *) &evt_gyro, NULL);
                }
                else
                {
                  res = SYS_INVALID_PARAMETER_ERROR_CODE;
                }
              }
#else
              if (p_obj->acc_sensor_status.is_active && p_obj->acc_drdy)
              {
                /* update measuredODR */
                p_obj->acc_sensor_status.type.mems.measured_odr = (float)p_obj->acc_samples_count / (float)delta_timestamp;

                EMD_Init(&p_obj->data_acc, p_obj->p_acc_sample, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, p_obj->acc_samples_count, 3);
                DataEventInit((IEvent *) &evt_acc, p_obj->p_acc_event_src, &p_obj->data_acc, timestamp, p_obj->acc_id);

                IEventSrcSendEvent(p_obj->p_acc_event_src, (IEvent *) &evt_acc, NULL);
                p_obj->acc_drdy = 0;
              }
              if (p_obj->gyro_sensor_status.is_active && p_obj->gyro_drdy)
              {
                /* update measuredODR */
                p_obj->gyro_sensor_status.type.mems.measured_odr = (float)p_obj->gyro_samples_count / (float)delta_timestamp;

                EMD_Init(&p_obj->data_gyro, p_obj->p_gyro_sample, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, p_obj->gyro_samples_count, 3);
                DataEventInit((IEvent *) &evt_gyro, p_obj->p_gyro_event_src, &p_obj->data_gyro, timestamp, p_obj->gyro_id);

                IEventSrcSendEvent(p_obj->p_gyro_event_src, (IEvent *) &evt_gyro, NULL);
                p_obj->gyro_drdy = 0;
              }
#endif
              SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("LSM6DSV16BX: ts = %f\r\n", (float)timestamp));
#if LSM6DSV16BX_FIFO_ENABLED
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
        res = LSM6DSV16BXTaskSensorReadMLC(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          // notify the listeners...
          double timestamp = report.sensorDataReadyMessage.fTimestamp;

          if (p_obj->mlc_enable)
          {
            EMD_Init(&p_obj->data, p_obj->p_mlc_sensor_data_buff, E_EM_UINT8, E_EM_MODE_INTERLEAVED, 2, 1, 5);

            DataEvent_t evt;

            DataEventInit((IEvent *) &evt, p_obj->p_mlc_event_src, &p_obj->data, timestamp, p_obj->mlc_id);
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
            res = LSM6DSV16BXTaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->acc_sensor_status.is_active == true || p_obj->gyro_sensor_status.is_active == true)
              {
                if (p_obj->pIRQConfig == NULL)
                {
                  if (TX_SUCCESS
                      != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->lsm6dsv16bx_task_cfg_timer_period_ms),
                                         AMT_MS_TO_TICKS(p_obj->lsm6dsv16bx_task_cfg_timer_period_ms)))
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
                  LSM6DSV16BXTaskConfigureIrqPin(p_obj, FALSE);
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
                      != tx_timer_change(&p_obj->mlc_timer, AMT_MS_TO_TICKS(LSM6DSV16BX_TASK_CFG_MLC_TIMER_PERIOD_MS),
                                         AMT_MS_TO_TICKS(LSM6DSV16BX_TASK_CFG_MLC_TIMER_PERIOD_MS)))
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
                  LSM6DSV16BXTaskConfigureMLCPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = LSM6DSV16BXTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = LSM6DSV16BXTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = LSM6DSV16BXTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = LSM6DSV16BXTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = LSM6DSV16BXTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV16BX: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }

      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV16BX: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}

static inline sys_error_code_t LSM6DSV16BXTaskPostReportToFront(LSM6DSV16BXTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t LSM6DSV16BXTaskPostReportToBack(LSM6DSV16BXTask *_this, SMMessage *pReport)
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

static sys_error_code_t LSM6DSV16BXTaskSensorInit(LSM6DSV16BXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  /* Setup acc and gyro */
  uint8_t reg0 = 0;
  lsm6dsv16bx_xl_data_rate_t lsm6dsv16bx_xl_data_rate = LSM6DSV16BX_XL_ODR_OFF;
  lsm6dsv16bx_fifo_xl_batch_t lsm6dsv16bx_fifo_xl_batch = LSM6DSV16BX_XL_NOT_BATCHED;
  lsm6dsv16bx_gy_data_rate_t lsm6dsv16bx_gy_data_rate = LSM6DSV16BX_GY_ODR_OFF;
  lsm6dsv16bx_fifo_gy_batch_t lsm6dsv16bx_fifo_gy_batch = LSM6DSV16BX_GY_NOT_BATCHED;
  lsm6dsv16bx_filt_settling_mask_t filt_settling_mask = {0};
  int32_t ret_val = 0;

  uint8_t mlc_int1;
  uint8_t mlc_int2;
  lsm6dsv16bx_pin_int_route_t int1_route = {0};

  lsm6dsv16bx_reset_t rst;

  ret_val = lsm6dsv16bx_device_id_get(p_sensor_drv, (uint8_t *) &reg0);
  if (!ret_val)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, reg0);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV16BX: sensor - I am 0x%x.\r\n", reg0));

  /* Restore default configuration */
  ret_val = lsm6dsv16bx_reset_set(p_sensor_drv, LSM6DSV16BX_RESTORE_CTRL_REGS);
  do
  {
    lsm6dsv16bx_reset_get(p_sensor_drv, &rst);
  } while (rst != LSM6DSV16BX_READY);

  /* Enable Block Data Update */
  ret_val = lsm6dsv16bx_block_data_update_set(p_sensor_drv, PROPERTY_ENABLE);

  /* AXL fs */
  if (_this->acc_sensor_status.type.mems.fs < 3.0f)
  {
    lsm6dsv16bx_xl_full_scale_set(p_sensor_drv, LSM6DSV16BX_2g);
  }
  else if (_this->acc_sensor_status.type.mems.fs < 5.0f)
  {
    lsm6dsv16bx_xl_full_scale_set(p_sensor_drv, LSM6DSV16BX_4g);
  }
  else if (_this->acc_sensor_status.type.mems.fs < 9.0f)
  {
    lsm6dsv16bx_xl_full_scale_set(p_sensor_drv, LSM6DSV16BX_8g);
  }
  else
  {
    lsm6dsv16bx_xl_full_scale_set(p_sensor_drv, LSM6DSV16BX_16g);
  }

  /* GYRO fs */
  if (_this->gyro_sensor_status.type.mems.fs < 126.0f)
  {
    lsm6dsv16bx_gy_full_scale_set(p_sensor_drv, LSM6DSV16BX_125dps);
  }
  else if (_this->gyro_sensor_status.type.mems.fs < 251.0f)
  {
    lsm6dsv16bx_gy_full_scale_set(p_sensor_drv, LSM6DSV16BX_250dps);
  }
  else if (_this->gyro_sensor_status.type.mems.fs < 501.0f)
  {
    lsm6dsv16bx_gy_full_scale_set(p_sensor_drv, LSM6DSV16BX_500dps);
  }
  else if (_this->gyro_sensor_status.type.mems.fs < 1001.0f)
  {
    lsm6dsv16bx_gy_full_scale_set(p_sensor_drv, LSM6DSV16BX_1000dps);
  }
  else if (_this->gyro_sensor_status.type.mems.fs < 2001.0f)
  {
    lsm6dsv16bx_gy_full_scale_set(p_sensor_drv, LSM6DSV16BX_2000dps);
  }
  else
  {
    lsm6dsv16bx_gy_full_scale_set(p_sensor_drv, LSM6DSV16BX_4000dps);
  }

  /* Configure filtering chain */
  filt_settling_mask.drdy = PROPERTY_ENABLE;
  filt_settling_mask.irq_xl = PROPERTY_ENABLE;
  filt_settling_mask.irq_g = PROPERTY_ENABLE;
  lsm6dsv16bx_filt_settling_mask_set(p_sensor_drv, filt_settling_mask);

#if LSM6DSV16BX_FIFO_ENABLED
  uint16_t lsm6dsv16bx_wtm_level = 0;
  uint16_t lsm6dsv16bx_wtm_level_acc;
  uint16_t lsm6dsv16bx_wtm_level_gyro;

  if (_this->samples_per_it == 0)
  {
    /* Calculation of watermark and samples per int*/
    lsm6dsv16bx_wtm_level_acc = ((uint16_t) _this->acc_sensor_status.type.mems.odr * (uint16_t) LSM6DSV16BX_MAX_DRDY_PERIOD);
    lsm6dsv16bx_wtm_level_gyro = ((uint16_t) _this->gyro_sensor_status.type.mems.odr * (uint16_t) LSM6DSV16BX_MAX_DRDY_PERIOD);

    if (_this->acc_sensor_status.is_active && _this->gyro_sensor_status.is_active) /* Both subSensor is active */
    {
      if (lsm6dsv16bx_wtm_level_acc > lsm6dsv16bx_wtm_level_gyro)
      {
        lsm6dsv16bx_wtm_level = lsm6dsv16bx_wtm_level_acc;
      }
      else
      {
        lsm6dsv16bx_wtm_level = lsm6dsv16bx_wtm_level_gyro;
      }
    }
    else /* Only one subSensor is active */
    {
      if (_this->acc_sensor_status.is_active)
      {
        lsm6dsv16bx_wtm_level = lsm6dsv16bx_wtm_level_acc;
      }
      else
      {
        lsm6dsv16bx_wtm_level = lsm6dsv16bx_wtm_level_gyro;
      }
    }

    if (lsm6dsv16bx_wtm_level > LSM6DSV16BX_MAX_WTM_LEVEL)
    {
      lsm6dsv16bx_wtm_level = LSM6DSV16BX_MAX_WTM_LEVEL;
    }
    _this->samples_per_it = lsm6dsv16bx_wtm_level;
  }

  /* Setup wtm for FIFO */
  lsm6dsv16bx_fifo_watermark_set(p_sensor_drv, _this->samples_per_it);
  lsm6dsv16bx_fifo_stop_on_wtm_set(p_sensor_drv, PROPERTY_ENABLE);

  if (_this->pIRQConfig != NULL)
  {
    int1_route.fifo_th = PROPERTY_ENABLE;
  }
  else
  {
    int1_route.fifo_th = PROPERTY_DISABLE;
  }

  lsm6dsv16bx_pin_int1_route_set(p_sensor_drv, int1_route);

  lsm6dsv16bx_fifo_mode_set(p_sensor_drv, LSM6DSV16BX_STREAM_MODE);

#else

  int16_t buff[3];
  lsm6dsv16bx_acceleration_raw_get(p_sensor_drv, &buff[0]);
  lsm6dsv16bx_angular_rate_raw_get(p_sensor_drv, &buff[0]);

  _this->samples_per_it = 1;
  if (_this->pIRQConfig != NULL)
  {
    if (_this->acc_sensor_status.is_active && _this->gyro_sensor_status.is_active) /* Both subSensor is active */
    {
      int1_route.drdy_xl = PROPERTY_ENABLE;
      int1_route.drdy_gy = PROPERTY_ENABLE;
    }
    else if (_this->acc_sensor_status.is_active)
    {
      int1_route.drdy_xl = PROPERTY_ENABLE;
    }
    else
    {
      int1_route.drdy_gy = PROPERTY_ENABLE;
    }
  }
  else
  {
    int1_route.drdy_xl = PROPERTY_DISABLE;
    int1_route.drdy_gy = PROPERTY_DISABLE;
  }
  lsm6dsv16bx_pin_int1_route_set(p_sensor_drv, int1_route);

#endif /* LSM6DSV16BX_FIFO_ENABLED */

  /* Set Output Data Rate.
   * Selected data rate have to be equal or greater with respect
   * with MLC data rate.
   */
  if (_this->acc_sensor_status.type.mems.odr < 8.0f)
  {
    lsm6dsv16bx_xl_data_rate = LSM6DSV16BX_XL_ODR_AT_7Hz5;
    lsm6dsv16bx_fifo_xl_batch = LSM6DSV16BX_XL_BATCHED_AT_7Hz5;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 16.0f)
  {
    lsm6dsv16bx_xl_data_rate = LSM6DSV16BX_XL_ODR_AT_15Hz;
    lsm6dsv16bx_fifo_xl_batch = LSM6DSV16BX_XL_BATCHED_AT_15Hz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 31.0f)
  {
    lsm6dsv16bx_xl_data_rate = LSM6DSV16BX_XL_ODR_AT_30Hz;
    lsm6dsv16bx_fifo_xl_batch = LSM6DSV16BX_XL_BATCHED_AT_30Hz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 61.0f)
  {
    lsm6dsv16bx_xl_data_rate = LSM6DSV16BX_XL_ODR_AT_60Hz;
    lsm6dsv16bx_fifo_xl_batch = LSM6DSV16BX_XL_BATCHED_AT_60Hz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 121.0f)
  {
    lsm6dsv16bx_xl_data_rate = LSM6DSV16BX_XL_ODR_AT_120Hz;
    lsm6dsv16bx_fifo_xl_batch = LSM6DSV16BX_XL_BATCHED_AT_120Hz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 241.0f)
  {
    lsm6dsv16bx_xl_data_rate = LSM6DSV16BX_XL_ODR_AT_240Hz;
    lsm6dsv16bx_fifo_xl_batch = LSM6DSV16BX_XL_BATCHED_AT_240Hz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 481.0f)
  {
    lsm6dsv16bx_xl_data_rate = LSM6DSV16BX_XL_ODR_AT_480Hz;
    lsm6dsv16bx_fifo_xl_batch = LSM6DSV16BX_XL_BATCHED_AT_480Hz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 961.0f)
  {
    lsm6dsv16bx_xl_data_rate = LSM6DSV16BX_XL_ODR_AT_960Hz;
    lsm6dsv16bx_fifo_xl_batch = LSM6DSV16BX_XL_BATCHED_AT_960Hz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 1921.0f)
  {
    lsm6dsv16bx_xl_data_rate = LSM6DSV16BX_XL_ODR_AT_1920Hz;
    lsm6dsv16bx_fifo_xl_batch = LSM6DSV16BX_XL_BATCHED_AT_1920Hz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 3841.0f)
  {
    lsm6dsv16bx_xl_data_rate = LSM6DSV16BX_XL_ODR_AT_3840Hz;
    lsm6dsv16bx_fifo_xl_batch = LSM6DSV16BX_XL_BATCHED_AT_3840Hz;
  }
  else
  {
    lsm6dsv16bx_xl_data_rate = LSM6DSV16BX_XL_ODR_AT_7680Hz;
    lsm6dsv16bx_fifo_xl_batch = LSM6DSV16BX_XL_BATCHED_AT_7680Hz;
  }

  if (_this->gyro_sensor_status.type.mems.odr < 8.0f)
  {
    lsm6dsv16bx_gy_data_rate = LSM6DSV16BX_GY_ODR_AT_7Hz5;
    lsm6dsv16bx_fifo_gy_batch = LSM6DSV16BX_GY_BATCHED_AT_7Hz5;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 16.0f)
  {
    lsm6dsv16bx_gy_data_rate = LSM6DSV16BX_GY_ODR_AT_15Hz;
    lsm6dsv16bx_fifo_gy_batch = LSM6DSV16BX_GY_BATCHED_AT_15Hz;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 31.0f)
  {
    lsm6dsv16bx_gy_data_rate = LSM6DSV16BX_GY_ODR_AT_30Hz;
    lsm6dsv16bx_fifo_gy_batch = LSM6DSV16BX_GY_BATCHED_AT_30Hz;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 61.0f)
  {
    lsm6dsv16bx_gy_data_rate = LSM6DSV16BX_GY_ODR_AT_60Hz;
    lsm6dsv16bx_fifo_gy_batch = LSM6DSV16BX_GY_BATCHED_AT_60Hz;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 121.0f)
  {
    lsm6dsv16bx_gy_data_rate = LSM6DSV16BX_GY_ODR_AT_120Hz;
    lsm6dsv16bx_fifo_gy_batch = LSM6DSV16BX_GY_BATCHED_AT_120Hz;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 241.0f)
  {
    lsm6dsv16bx_gy_data_rate = LSM6DSV16BX_GY_ODR_AT_240Hz;
    lsm6dsv16bx_fifo_gy_batch = LSM6DSV16BX_GY_BATCHED_AT_240Hz;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 481.0f)
  {
    lsm6dsv16bx_gy_data_rate = LSM6DSV16BX_GY_ODR_AT_480Hz;
    lsm6dsv16bx_fifo_gy_batch = LSM6DSV16BX_GY_BATCHED_AT_480Hz;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 961.0f)
  {
    lsm6dsv16bx_gy_data_rate = LSM6DSV16BX_GY_ODR_AT_960Hz;
    lsm6dsv16bx_fifo_gy_batch = LSM6DSV16BX_GY_BATCHED_AT_960Hz;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 1921.0f)
  {
    lsm6dsv16bx_gy_data_rate = LSM6DSV16BX_GY_ODR_AT_1920Hz;
    lsm6dsv16bx_fifo_gy_batch = LSM6DSV16BX_GY_BATCHED_AT_1920Hz;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 3841.0f)
  {
    lsm6dsv16bx_gy_data_rate = LSM6DSV16BX_GY_ODR_AT_3840Hz;
    lsm6dsv16bx_fifo_gy_batch = LSM6DSV16BX_GY_BATCHED_AT_3840Hz;
  }
  else
  {
    lsm6dsv16bx_gy_data_rate = LSM6DSV16BX_GY_ODR_AT_7680Hz;
    lsm6dsv16bx_fifo_gy_batch = LSM6DSV16BX_GY_BATCHED_AT_7680Hz;
  }

  if (_this->acc_sensor_status.is_active)
  {
    lsm6dsv16bx_xl_data_rate_set(p_sensor_drv, lsm6dsv16bx_xl_data_rate);
    lsm6dsv16bx_fifo_xl_batch_set(p_sensor_drv, lsm6dsv16bx_fifo_xl_batch);
  }
  else
  {
    lsm6dsv16bx_xl_data_rate_set(p_sensor_drv, LSM6DSV16BX_XL_ODR_OFF);
    lsm6dsv16bx_fifo_xl_batch_set(p_sensor_drv, LSM6DSV16BX_XL_NOT_BATCHED);
    _this->acc_sensor_status.is_active = false;
  }

  if (_this->gyro_sensor_status.is_active)
  {
    lsm6dsv16bx_gy_data_rate_set(p_sensor_drv, lsm6dsv16bx_gy_data_rate);
    lsm6dsv16bx_fifo_gy_batch_set(p_sensor_drv, lsm6dsv16bx_fifo_gy_batch);
  }
  else
  {
    lsm6dsv16bx_gy_data_rate_set(p_sensor_drv, LSM6DSV16BX_GY_ODR_OFF);
    lsm6dsv16bx_fifo_gy_batch_set(p_sensor_drv, LSM6DSV16BX_GY_NOT_BATCHED);
    _this->gyro_sensor_status.is_active = false;
  }

  /* Check configuration from UCF */
  lsm6dsv16bx_mem_bank_set(p_sensor_drv, LSM6DSV16BX_EMBED_FUNC_MEM_BANK);
  lsm6dsv16bx_read_reg(p_sensor_drv, LSM6DSV16BX_MLC_INT1, &mlc_int1, 1);
  lsm6dsv16bx_read_reg(p_sensor_drv, LSM6DSV16BX_MLC_INT2, &mlc_int2, 1);

  /* MLC must be handled in polling due to HW limitations: disable MLC INT */
  mlc_int1 = 0;
  mlc_int2 = 0;
  lsm6dsv16bx_write_reg(p_sensor_drv, LSM6DSV16BX_MLC_INT1, &mlc_int1, 1);
  lsm6dsv16bx_write_reg(p_sensor_drv, LSM6DSV16BX_MLC_INT2, &mlc_int2, 1);
  lsm6dsv16bx_mem_bank_set(p_sensor_drv, LSM6DSV16BX_MAIN_MEM_BANK);

  if (_this->mlc_enable == false)
  {
    lsm6dsv16bx_mlc_set(p_sensor_drv, LSM6DSV16BX_MLC_OFF);
  }
  else
  {
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

#if LSM6DSV16BX_FIFO_ENABLED
  uint8_t reg[2];
  /* Check FIFO_WTM_IA and fifo level. We do not use PID in order to avoid reading one register twice */
  lsm6dsv16bx_read_reg(p_sensor_drv, LSM6DSV16BX_FIFO_STATUS1, reg, 2);

  _this->fifo_level = ((reg[1] & 0x01) << 8) + reg[0];

  if (_this->fifo_level >= _this->samples_per_it)
  {
    lsm6dsv16bx_read_reg(p_sensor_drv, LSM6DSV16BX_FIFO_DATA_OUT_TAG, _this->p_fast_sensor_data_buff,
                         _this->samples_per_it * 7);
  }
#endif

  if ((_this->acc_sensor_status.is_active) && (_this->gyro_sensor_status.is_active))
  {
    _this->lsm6dsv16bx_task_cfg_timer_period_ms = (uint16_t)(
                                                    _this->acc_sensor_status.type.mems.odr > _this->gyro_sensor_status.type.mems.odr ?
                                                    _this->acc_sensor_status.type.mems.odr :
                                                    _this->gyro_sensor_status.type.mems.odr);
  }
  else if (_this->acc_sensor_status.is_active)
  {
    _this->lsm6dsv16bx_task_cfg_timer_period_ms = (uint16_t)(_this->acc_sensor_status.type.mems.odr);
  }
  else if (_this->gyro_sensor_status.is_active)
  {
    _this->lsm6dsv16bx_task_cfg_timer_period_ms = (uint16_t)(_this->acc_sensor_status.type.mems.odr);
  }
  else
  {
  }

#if LSM6DSV16BX_FIFO_ENABLED
  _this->lsm6dsv16bx_task_cfg_timer_period_ms = (uint16_t)((1000.0f / _this->lsm6dsv16bx_task_cfg_timer_period_ms) * (((float)(_this->samples_per_it)) / 2.0f));
#else
  _this->lsm6dsv16bx_task_cfg_timer_period_ms = (uint16_t)(1000.0f / _this->lsm6dsv16bx_task_cfg_timer_period_ms);
#endif

  return res;
}

static sys_error_code_t LSM6DSV16BXTaskSensorReadData(LSM6DSV16BXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

#if LSM6DSV16BX_FIFO_ENABLED
  uint16_t samples_per_it = _this->samples_per_it;
  uint8_t reg[2];
  uint16_t i;

  /* Check FIFO_WTM_IA and fifo level. We do not use PID in order to avoid reading one register twice */
  lsm6dsv16bx_read_reg(p_sensor_drv, LSM6DSV16BX_FIFO_STATUS1, reg, 2);

  _this->fifo_level = ((reg[1] & 0x01) << 8) + reg[0];

  if (((reg[1]) & 0x80) && (_this->fifo_level >= samples_per_it))
  {
    res = lsm6dsv16bx_read_reg(p_sensor_drv, LSM6DSV16BX_FIFO_DATA_OUT_TAG, _this->p_fast_sensor_data_buff, samples_per_it * 7);

    if (!SYS_IS_ERROR_CODE(res))
    {
#if (HSD_USE_DUMMY_DATA == 1)
      int16_t *p16 = (int16_t *)(_this->p_fast_sensor_data_buff);

      for (i = 0; i < samples_per_it; i++)
      {
        p16 = (int16_t *)(&_this->p_fast_sensor_data_buff[i * 7] + 1);
        if ((_this->p_fast_sensor_data_buff[i * 7] >> 3) == LSM6DSV16BX_TAG_ACC)
        {
          *p16++ = dummyDataCounter_acc++;
          *p16++ = dummyDataCounter_acc++;
          *p16++ = dummyDataCounter_acc++;
        }
        else
        {
          *p16++ = dummyDataCounter_gyro++;
          *p16++ = dummyDataCounter_gyro++;
          *p16++ = dummyDataCounter_gyro++;
        }
      }
#endif
      if ((_this->acc_sensor_status.is_active) && (_this->gyro_sensor_status.is_active))
      {
        /* Read both ACC and GYRO */

        uint32_t odr_acc = (uint32_t) _this->acc_sensor_status.type.mems.odr;
        uint32_t odr_gyro = (uint32_t) _this->gyro_sensor_status.type.mems.odr;

        int16_t *p16_src = (int16_t *) _this->p_fast_sensor_data_buff;
        int16_t *p_acc, *p_gyro;

        _this->acc_samples_count = 0;
        _this->gyro_samples_count = 0;

        if (odr_acc > odr_gyro) /* Acc is faster than Gyro */
        {
          p_acc = (int16_t *) _this->p_fast_sensor_data_buff;
          p_gyro = (int16_t *) _this->p_slow_sensor_data_buff;
        }
        else
        {
          p_acc = (int16_t *) _this->p_slow_sensor_data_buff;
          p_gyro = (int16_t *) _this->p_fast_sensor_data_buff;
        }

        uint8_t *p_tag = (uint8_t *) p16_src;

        for (i = 0; i < samples_per_it; i++)
        {
          if (((*p_tag) >> 3) == LSM6DSV16BX_TAG_ACC)
          {
            p16_src = (int16_t *)(p_tag + 1);
            p_acc[2] = *p16_src++;
            p_acc[1] = *p16_src++;
            p_acc[0] = *p16_src++;
            p_acc = p_acc + 3;
            _this->acc_samples_count++;
          }
          else
          {
            p16_src = (int16_t *)(p_tag + 1);
            *p_gyro++ = *p16_src++;
            *p_gyro++ = *p16_src++;
            *p_gyro++ = *p16_src++;
            _this->gyro_samples_count++;
          }
          p_tag += 7;
        }
      }
      else /* 1 subsensor active only --> simply drop TAGS */
      {
        int16_t *p16_src = (int16_t *) _this->p_fast_sensor_data_buff;
        int16_t *p16_dest = (int16_t *) _this->p_fast_sensor_data_buff;
        int16_t p_acc[3];

        uint8_t *p_tag = (uint8_t *) p16_src;

        for (i = 0; i < samples_per_it; i++)
        {
          if (((*p_tag) >> 3) == LSM6DSV16BX_TAG_ACC)
          {
            p16_src = (int16_t *)(p_tag + 1);
            p_acc[2] = *p16_src++;
            p_acc[1] = *p16_src++;
            p_acc[0] = *p16_src++;
            memcpy(p16_dest, &p_acc, 6);
            p16_dest = p16_dest + 3;
          }
          else
          {
            p16_src = (int16_t *)(p_tag + 1);
            *p16_dest++ = *p16_src++;
            *p16_dest++ = *p16_src++;
            *p16_dest++ = *p16_src++;
          }
          p_tag += 7;
        }
        if (_this->acc_sensor_status.is_active)
        {
          _this->acc_samples_count = samples_per_it;
        }
        else
        {
          _this->gyro_samples_count = samples_per_it;
        }
      }
    }
  }
  else
  {
    _this->fifo_level = 0;
    res = SYS_BASE_ERROR_CODE;
  }
#else
  if ((_this->acc_sensor_status.is_active) && (_this->gyro_sensor_status.is_active))
  {
    lsm6dsv16bx_status_reg_t val;
    uint32_t odr_acc = (uint32_t) _this->acc_sensor_status.type.mems.odr;
    uint32_t odr_gyro = (uint32_t) _this->gyro_sensor_status.type.mems.odr;

    if (odr_acc != odr_gyro)
    {
      /* Need to read which sensor generated the INT in case of different odr; */
      lsm6dsv16bx_read_reg(p_sensor_drv, LSM6DSV16BX_STATUS_REG, (uint8_t *) &val, 1);
    }
    else
    {
      /* Manually set the variable to read both sensors (avoid to loose time with a read) */
      val.xlda = 1U;
      val.gda = 1U;
    }

    if (val.xlda == 1U)
    {
      int16_t *p16 = (int16_t *)(_this->p_acc_sample);
      res = lsm6dsv16bx_acceleration_raw_get(p_sensor_drv, p16);

      if (!SYS_IS_ERROR_CODE(res))
      {
#if (HSD_USE_DUMMY_DATA == 1)
        *p16++ = dummyDataCounter_acc++;
        *p16++ = dummyDataCounter_acc++;
        *p16++ = dummyDataCounter_acc++;
#endif
        _this->acc_samples_count = 1U;
        _this->acc_drdy = 1;
      }
    }
    if (val.gda == 1U)
    {
      int16_t *p16 = (int16_t *)(_this->p_gyro_sample);
      res = lsm6dsv16bx_angular_rate_raw_get(p_sensor_drv, p16);

      if (!SYS_IS_ERROR_CODE(res))
      {
#if (HSD_USE_DUMMY_DATA == 1)
        *p16++ = dummyDataCounter_gyro++;
        *p16++ = dummyDataCounter_gyro++;
        *p16++ = dummyDataCounter_gyro++;
#endif
        _this->gyro_samples_count = 1U;
        _this->gyro_drdy = 1;
      }
    }
  }
  else if (_this->acc_sensor_status.is_active)
  {
    int16_t *p16 = (int16_t *)(_this->p_acc_sample);
    res = lsm6dsv16bx_acceleration_raw_get(p_sensor_drv, p16);

    if (!SYS_IS_ERROR_CODE(res))
    {
#if (HSD_USE_DUMMY_DATA == 1)
      uint16_t i = 0;
      for (i = 0; i < samples_per_it * 3; i++)
      {
        *p16++ = dummyDataCounter_acc++;
      }
#endif
      _this->acc_samples_count = 1U;
      _this->acc_drdy = 1;
    }
  }
  else if (_this->gyro_sensor_status.is_active)
  {
    int16_t *p16 = (int16_t *)(_this->p_gyro_sample);
    res = lsm6dsv16bx_angular_rate_raw_get(p_sensor_drv, p16);

    if (!SYS_IS_ERROR_CODE(res))
    {
#if (HSD_USE_DUMMY_DATA == 1)
      uint16_t i = 0;
      for (i = 0; i < samples_per_it * 3; i++)
      {
        *p16++ = dummyDataCounter_gyro++;
      }
#endif
      _this->gyro_samples_count = 1U;
      _this->gyro_drdy = 1;
    }
  }
  else
  {
  }
  _this->fifo_level = 1;
#endif /* LSM6DSV16BX_FIFO_ENABLED */

  return res;
}

uint8_t lsm6dsv16bx_mlc_output[4];
static sys_error_code_t LSM6DSV16BXTaskSensorReadMLC(LSM6DSV16BXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_BASE_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  lsm6dsv16bx_mlc_status_t mlc_status;

  if (_this->mlc_enable)
  {
    lsm6dsv16bx_mem_bank_set(p_sensor_drv, LSM6DSV16BX_EMBED_FUNC_MEM_BANK);
    lsm6dsv16bx_read_reg(p_sensor_drv, LSM6DSV16BX_MLC_STATUS, (uint8_t *)(&mlc_status), 1);
    _this->p_mlc_sensor_data_buff[4] = (mlc_status.is_mlc1) | (mlc_status.is_mlc2 << 1) | (mlc_status.is_mlc3 << 2) | (mlc_status.is_mlc4 << 3);

    lsm6dsv16bx_read_reg(p_sensor_drv, LSM6DSV16BX_MLC1_SRC, (uint8_t *)(&lsm6dsv16bx_mlc_output[0]), 4);
    if (memcmp(&_this->p_mlc_sensor_data_buff[0], &lsm6dsv16bx_mlc_output[0], 4) != 0)
    {
      memcpy(&_this->p_mlc_sensor_data_buff[0], &lsm6dsv16bx_mlc_output[0], 4);
      res = SYS_NO_ERROR_CODE;
    }
    else
    {
      res = SYS_SENSOR_TASK_NO_DRDY_ERROR_CODE;
    }
    lsm6dsv16bx_mem_bank_set(p_sensor_drv, LSM6DSV16BX_MAIN_MEM_BANK);
  }

  return res;
}

static sys_error_code_t LSM6DSV16BXTaskSensorRegister(LSM6DSV16BXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

#if !LSM6DSV16BX_ACC_DISABLED
  ISensor_t *acc_if = (ISensor_t *) LSM6DSV16BXTaskGetAccSensorIF(_this);
  _this->acc_id = SMAddSensor(acc_if);
#endif
#if !LSM6DSV16BX_GYRO_DISABLED
  ISensor_t *gyro_if = (ISensor_t *) LSM6DSV16BXTaskGetGyroSensorIF(_this);
  _this->gyro_id = SMAddSensor(gyro_if);
#endif
#if !LSM5DSV16BX_MLC_DISABLED
  ISensor_t *mlc_if = (ISensor_t *) LSM6DSV16BXTaskGetMlcSensorIF(_this);
  _this->mlc_id = SMAddSensor(mlc_if);
#endif

  return res;
}

static sys_error_code_t LSM6DSV16BXTaskSensorInitTaskParams(LSM6DSV16BXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* ACCELEROMETER STATUS */
  _this->acc_sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->acc_sensor_status.is_active = TRUE;
  _this->acc_sensor_status.type.mems.fs = 16.0f;
  _this->acc_sensor_status.type.mems.sensitivity = 0.0000305f * _this->acc_sensor_status.type.mems.fs;
  _this->acc_sensor_status.type.mems.odr = 7680.0f;
  _this->acc_sensor_status.type.mems.measured_odr = 0.0f;
#if LSM6DSV16BX_FIFO_ENABLED
  EMD_Init(&_this->data_acc, _this->p_fast_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);
#else
  EMD_Init(&_this->data_acc, _this->p_acc_sample, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);
#endif

  /* GYROSCOPE STATUS */
  _this->gyro_sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->gyro_sensor_status.is_active = TRUE;
  _this->gyro_sensor_status.type.mems.fs = 4000.0f;
  _this->gyro_sensor_status.type.mems.odr = 7680.0f;
  _this->gyro_sensor_status.type.mems.sensitivity = 0.000035f * _this->gyro_sensor_status.type.mems.fs;
  _this->gyro_sensor_status.type.mems.measured_odr = 0.0f;
#if LSM6DSV16BX_FIFO_ENABLED
  EMD_Init(&_this->data_gyro, _this->p_slow_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);
#else
  EMD_Init(&_this->data_gyro, _this->p_gyro_sample, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);
#endif

  /* MLC STATUS */
  _this->mlc_sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->mlc_sensor_status.is_active = FALSE;
  _this->mlc_sensor_status.type.mems.fs = 1.0f;
  _this->mlc_sensor_status.type.mems.sensitivity = 1.0f;
  _this->mlc_sensor_status.type.mems.odr = 1.0f;
  _this->mlc_sensor_status.type.mems.measured_odr = 0.0f;
  EMD_Init(&_this->data, _this->p_mlc_sensor_data_buff, E_EM_UINT8, E_EM_MODE_INTERLEAVED, 2, 1, 5);

  return res;
}

static sys_error_code_t LSM6DSV16BXTaskSensorSetODR(LSM6DSV16BXTask *_this, SMMessage report)
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
      lsm6dsv16bx_xl_data_rate_set(p_sensor_drv, LSM6DSV16BX_XL_ODR_OFF);
      /* Do not update the model in case of odr = 0 */
      odr = _this->acc_sensor_status.type.mems.odr;
    }
    else
    {
      /* Changing odr must disable MLC sensor: MLC can work properly only when setup from UCF */
      _this->mlc_enable = FALSE;
      _this->mlc_sensor_status.is_active = FALSE;

      if (odr < 8.0f)
      {
        odr = 7.5f;
      }
      else if (odr < 16.0f)
      {
        odr = 15.0f;
      }
      else if (odr < 31.0f)
      {
        odr = 30.0f;
      }
      else if (odr < 61.0f)
      {
        odr = 60.0f;
      }
      else if (odr < 121.0f)
      {
        odr = 120.0f;
      }
      else if (odr < 241.0f)
      {
        odr = 240.0f;
      }
      else if (odr < 481.0f)
      {
        odr = 480.0f;
      }
      else if (odr < 961.0f)
      {
        odr = 960.0f;
      }
      else if (odr < 1921.0f)
      {
        odr = 1920.0f;
      }
      else if (odr < 3841.0f)
      {
        odr = 3840.0f;
      }
      else
      {
        odr = 7680;
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
      lsm6dsv16bx_gy_data_rate_set(p_sensor_drv, LSM6DSV16BX_GY_ODR_OFF);
      /* Do not update the model in case of odr = 0 */
      odr = _this->gyro_sensor_status.type.mems.odr;
    }
    else
    {
      /* Changing odr must disable MLC sensor: MLC can work properly only when setup from UCF */
      _this->mlc_enable = FALSE;
      _this->mlc_sensor_status.is_active = FALSE;

      if (odr < 8.0f)
      {
        odr = 7.5f;
      }
      else if (odr < 16.0f)
      {
        odr = 15.0f;
      }
      else if (odr < 31.0f)
      {
        odr = 30.0f;
      }
      else if (odr < 61.0f)
      {
        odr = 60.0f;
      }
      else if (odr < 121.0f)
      {
        odr = 120.0f;
      }
      else if (odr < 241.0f)
      {
        odr = 240.0f;
      }
      else if (odr < 481.0f)
      {
        odr = 480.0f;
      }
      else if (odr < 961.0f)
      {
        odr = 960.0f;
      }
      else if (odr < 1921.0f)
      {
        odr = 1920.0f;
      }
      else if (odr < 3841.0f)
      {
        odr = 3840.0f;
      }
      else
      {
        odr = 7680;
      }
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->gyro_sensor_status.type.mems.odr = odr;
      _this->gyro_sensor_status.type.mems.measured_odr = 0.0f;
    }
  }
  else if (id == _this->mlc_id)
  {
    res = SYS_TASK_INVALID_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV16BX: warning - MLC SetODR() not supported.\r\n"));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  /* when odr changes the samples_per_it must be recalculated */
  _this->samples_per_it = 0;

  return res;
}

static sys_error_code_t LSM6DSV16BXTaskSensorSetFS(LSM6DSV16BXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  float fs = (float) report.sensorMessage.fParam;
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

    _this->acc_sensor_status.type.mems.fs = fs;
    _this->acc_sensor_status.type.mems.sensitivity = 0.0000305f * _this->acc_sensor_status.type.mems.fs;
  }
  else if (id == _this->gyro_id)
  {
    if (fs < 126.0f)
    {
      fs = 125.0f;
    }
    else if (fs < 251.0f)
    {
      fs = 250.0f;
    }
    else if (fs < 501.0f)
    {
      fs = 500.0f;
    }
    else if (fs < 1001.0f)
    {
      fs = 1000.0f;
    }
    else if (fs < 2001.0f)
    {
      fs = 2000.0f;
    }
    else
    {
      fs = 4000.0f;
    }

    _this->gyro_sensor_status.type.mems.fs = fs;
    _this->gyro_sensor_status.type.mems.sensitivity = 0.000035f * _this->gyro_sensor_status.type.mems.fs;
  }
  else if (id == _this->mlc_id)
  {
    res = SYS_TASK_INVALID_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV16BX: warning - MLC SetFS() not supported.\r\n"));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t LSM6DSV16BXTaskSensorSetFifoWM(LSM6DSV16BXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (report.sensorMessage.nSensorId == _this->mlc_id)
  {
    res = SYS_TASK_INVALID_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

    SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("LSM6DSV16BX: warning - MLC SetFifoWM() not supported.\r\n"));
  }

#if LSM6DSV16BX_FIFO_ENABLED

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t lsm6dsv16bx_wtm_level = (uint16_t)report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if ((id == _this->acc_id) || (id == _this->gyro_id))
  {
    /* acc and gyro share the FIFO, so size should be increased w.r.t. previous setup */
    lsm6dsv16bx_wtm_level = lsm6dsv16bx_wtm_level + _this->samples_per_it;

    if (lsm6dsv16bx_wtm_level > LSM6DSV16BX_MAX_WTM_LEVEL)
    {
      lsm6dsv16bx_wtm_level = LSM6DSV16BX_MAX_WTM_LEVEL;
    }
    _this->samples_per_it = lsm6dsv16bx_wtm_level;

    /* Setup wtm for FIFO */
    lsm6dsv16bx_fifo_watermark_set(p_sensor_drv, _this->samples_per_it);

    lsm6dsv16bx_fifo_mode_set(p_sensor_drv, LSM6DSV16BX_STREAM_MODE);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }
#endif

  return res;
}

static sys_error_code_t LSM6DSV16BXTaskSensorEnable(LSM6DSV16BXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    _this->acc_sensor_status.is_active = TRUE;

    /* Changing sensor configuration must disable MLC sensor: MLC can work properly only when setup from UCF */
    _this->mlc_enable = FALSE;
    _this->mlc_sensor_status.is_active = FALSE;
  }
  else if (id == _this->gyro_id)
  {
    _this->gyro_sensor_status.is_active = TRUE;

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

static sys_error_code_t LSM6DSV16BXTaskSensorDisable(LSM6DSV16BXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    _this->acc_sensor_status.is_active = FALSE;
    lsm6dsv16bx_xl_data_rate_set(p_sensor_drv, LSM6DSV16BX_XL_ODR_OFF);

    /* Changing sensor configuration must disable MLC sensor: MLC can work properly only when setup from UCF */
    _this->mlc_enable = FALSE;
    _this->mlc_sensor_status.is_active = FALSE;
  }
  else if (id == _this->gyro_id)
  {
    _this->gyro_sensor_status.is_active = FALSE;
    lsm6dsv16bx_gy_data_rate_set(p_sensor_drv, LSM6DSV16BX_GY_ODR_OFF);

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

static boolean_t LSM6DSV16BXTaskSensorIsActive(const LSM6DSV16BXTask *_this)
{
  assert_param(_this != NULL);
  return (_this->acc_sensor_status.is_active || _this->gyro_sensor_status.is_active);
}

static sys_error_code_t LSM6DSV16BXTaskEnterLowPowerMode(const LSM6DSV16BXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  lsm6dsv16bx_xl_data_rate_t lsm6dsv16bx_xl_data_rate = LSM6DSV16BX_XL_ODR_OFF;
  lsm6dsv16bx_fifo_xl_batch_t lsm6dsv16bx_fifo_xl_batch = LSM6DSV16BX_XL_NOT_BATCHED;
  lsm6dsv16bx_gy_data_rate_t lsm6dsv16bx_gy_data_rate = LSM6DSV16BX_GY_ODR_OFF;
  lsm6dsv16bx_fifo_gy_batch_t lsm6dsv16bx_fifo_gy_batch = LSM6DSV16BX_GY_NOT_BATCHED;

  lsm6dsv16bx_xl_data_rate_set(p_sensor_drv, lsm6dsv16bx_xl_data_rate);
  lsm6dsv16bx_fifo_xl_batch_set(p_sensor_drv, lsm6dsv16bx_fifo_xl_batch);
  lsm6dsv16bx_gy_data_rate_set(p_sensor_drv, lsm6dsv16bx_gy_data_rate);
  lsm6dsv16bx_fifo_gy_batch_set(p_sensor_drv, lsm6dsv16bx_fifo_gy_batch);

  return res;
}

static sys_error_code_t LSM6DSV16BXTaskConfigureIrqPin(const LSM6DSV16BXTask *_this, boolean_t LowPower)
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

static sys_error_code_t LSM6DSV16BXTaskConfigureMLCPin(const LSM6DSV16BXTask *_this, boolean_t LowPower)
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

static inline LSM6DSV16BXTask *LSM6DSV16BXTaskGetOwnerFromISensorIF(ISensor_t *p_if)
{
  assert_param(p_if != NULL);
  LSM6DSV16BXTask *p_if_owner = NULL;

  /* check if the virtual function has been called from the mlc sensor IF  */
  p_if_owner = (LSM6DSV16BXTask *)((uint32_t) p_if - offsetof(LSM6DSV16BXTask, mlc_sensor_if));
  if (!(p_if_owner->gyro_sensor_if.vptr == &sTheClass.gyro_sensor_if_vtbl)
      || !(p_if_owner->super.vptr == &sTheClass.vtbl))
  {
    /* then the virtual function has been called from the gyro IF  */
    p_if_owner = (LSM6DSV16BXTask *)((uint32_t) p_if - offsetof(LSM6DSV16BXTask, gyro_sensor_if));
  }
  if (!(p_if_owner->acc_sensor_if.vptr == &sTheClass.acc_sensor_if_vtbl) || !(p_if_owner->super.vptr == &sTheClass.vtbl))
  {
    /* then the virtual function has been called from the acc IF  */
    p_if_owner = (LSM6DSV16BXTask *)((uint32_t) p_if - offsetof(LSM6DSV16BXTask, acc_sensor_if));
  }

  return p_if_owner;
}

static inline LSM6DSV16BXTask *LSM6DSV16BXTaskGetOwnerFromISensorLLIF(ISensorLL_t *p_if)
{
  assert_param(p_if != NULL);
  LSM6DSV16BXTask *p_if_owner = NULL;
  p_if_owner = (LSM6DSV16BXTask *)((uint32_t) p_if - offsetof(LSM6DSV16BXTask, sensor_ll_if));

  return p_if_owner;
}

static void LSM6DSV16BXTaskTimerCallbackFunction(ULONG param)
{
  LSM6DSV16BXTask *p_obj = (LSM6DSV16BXTask *) param;
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

static void LSM6DSV16BXTaskMLCTimerCallbackFunction(ULONG param)
{
  LSM6DSV16BXTask *p_obj = (LSM6DSV16BXTask *) param;
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

void LSM6DSV16BXTask_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((LSM6DSV16BXTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}

void INT2_DSV16BX_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY_MLC;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((LSM6DSV16BXTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}

static sys_error_code_t LSM6DSV16BX_ODR_Sync(LSM6DSV16BXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  float odr = 0.0f;
  lsm6dsv16bx_xl_data_rate_t lsm6dsv16bx_odr_xl;
  if (lsm6dsv16bx_xl_data_rate_get(p_sensor_drv, &lsm6dsv16bx_odr_xl) == 0)
  {
    _this->acc_sensor_status.is_active = TRUE;

    /* Update only the model */
    switch (lsm6dsv16bx_odr_xl)
    {
      case LSM6DSV16BX_XL_ODR_OFF:
        _this->acc_sensor_status.is_active = FALSE;
        /* Do not update the model in case of odr = 0 */
        odr = _this->acc_sensor_status.type.mems.odr;
        break;
      case LSM6DSV16BX_XL_ODR_AT_7Hz5:
        odr = 7.5f;
        break;
      case LSM6DSV16BX_XL_ODR_AT_15Hz:
        odr = 15.0f;
        break;
      case LSM6DSV16BX_XL_ODR_AT_30Hz:
        odr = 30.0f;
        break;
      case LSM6DSV16BX_XL_ODR_AT_60Hz:
        odr = 60.0f;
        break;
      case LSM6DSV16BX_XL_ODR_AT_120Hz:
        odr = 120.0f;
        break;
      case LSM6DSV16BX_XL_ODR_AT_240Hz:
        odr = 240.0f;
        break;
      case LSM6DSV16BX_XL_ODR_AT_480Hz:
        odr = 480.0f;
        break;
      case LSM6DSV16BX_XL_ODR_AT_960Hz:
        odr = 960.0f;
        break;
      case LSM6DSV16BX_XL_ODR_AT_1920Hz:
        odr = 1920.0f;
        break;
      case LSM6DSV16BX_XL_ODR_AT_3840Hz:
        odr = 3840.0f;
        break;
      case LSM6DSV16BX_XL_ODR_AT_7680Hz:
        odr = 7680.0f;
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
  lsm6dsv16bx_gy_data_rate_t lsm6dsv16bx_odr_g;
  if (lsm6dsv16bx_gy_data_rate_get(p_sensor_drv, &lsm6dsv16bx_odr_g) == 0)
  {
    _this->gyro_sensor_status.is_active = TRUE;

    /* Update only the model */
    switch (lsm6dsv16bx_odr_g)
    {
      case LSM6DSV16BX_GY_ODR_OFF:
        _this->gyro_sensor_status.is_active = FALSE;
        /* Do not update the model in case of odr = 0 */
        odr = _this->gyro_sensor_status.type.mems.odr;
        break;
      case LSM6DSV16BX_GY_ODR_AT_7Hz5:
        odr = 7.5f;
        break;
      case LSM6DSV16BX_GY_ODR_AT_15Hz:
        odr = 15.0f;
        break;
      case LSM6DSV16BX_GY_ODR_AT_30Hz:
        odr = 30.0f;
        break;
      case LSM6DSV16BX_GY_ODR_AT_60Hz:
        odr = 60.0f;
        break;
      case LSM6DSV16BX_GY_ODR_AT_120Hz:
        odr = 120.0f;
        break;
      case LSM6DSV16BX_GY_ODR_AT_240Hz:
        odr = 240.0f;
        break;
      case LSM6DSV16BX_GY_ODR_AT_480Hz:
        odr = 480.0f;
        break;
      case LSM6DSV16BX_GY_ODR_AT_960Hz:
        odr = 960.0f;
        break;
      case LSM6DSV16BX_GY_ODR_AT_1920Hz:
        odr = 1920.0f;
        break;
      case LSM6DSV16BX_GY_ODR_AT_3840Hz:
        odr = 3840.0f;
        break;
      case LSM6DSV16BX_GY_ODR_AT_7680Hz:
        odr = 7680.0f;
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
  _this->samples_per_it = 0;
  return res;
}

static sys_error_code_t LSM6DSV16BX_FS_Sync(LSM6DSV16BXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  float fs = 2.0;
  lsm6dsv16bx_xl_full_scale_t fs_xl;
  if (lsm6dsv16bx_xl_full_scale_get(p_sensor_drv, &fs_xl) == 0)
  {
    switch (fs_xl)
    {
      case LSM6DSV16BX_2g:
        fs = 2.0;
        break;
      case LSM6DSV16BX_4g:
        fs = 4.0;
        break;
      case LSM6DSV16BX_8g:
        fs = 8.0;
        break;
      case LSM6DSV16BX_16g:
        fs = 16.0;
        break;
      default:
        break;
    }
    _this->acc_sensor_status.type.mems.fs = fs;
    _this->acc_sensor_status.type.mems.sensitivity = 0.0000305f * _this->acc_sensor_status.type.mems.fs;
  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }

  fs = 125;
  lsm6dsv16bx_gy_full_scale_t fs_g;
  if (lsm6dsv16bx_gy_full_scale_get(p_sensor_drv, &fs_g) == 0)
  {
    switch (fs_g)
    {
      case LSM6DSV16BX_125dps:
        fs = 125;
        break;
      case LSM6DSV16BX_250dps:
        fs = 250;
        break;
      case LSM6DSV16BX_500dps:
        fs = 500;
        break;
      case LSM6DSV16BX_1000dps:
        fs = 1000;
        break;
      case LSM6DSV16BX_2000dps:
        fs = 2000;
        break;
      case LSM6DSV16BX_4000dps:
        fs = 4000;
        break;
      default:
        break;
    }
    _this->gyro_sensor_status.type.mems.fs = fs;
    _this->gyro_sensor_status.type.mems.sensitivity = 0.000035f * _this->gyro_sensor_status.type.mems.fs;
  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }
  return res;
}

