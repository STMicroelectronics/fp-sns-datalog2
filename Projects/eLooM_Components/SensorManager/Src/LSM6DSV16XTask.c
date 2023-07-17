/**
  ******************************************************************************
  * @file    LSM6DSV16XTask.c
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

#include "LSM6DSV16XTask.h"
#include "LSM6DSV16XTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "lsm6dsv16x_reg.h"
#include <string.h>
#include <stdlib.h>
#include "services/sysdebug.h"
#include "mx.h"

#ifndef LSM6DSV16X_TASK_CFG_STACK_DEPTH
#define LSM6DSV16X_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*8)
#endif

#ifndef LSM6DSV16X_TASK_CFG_PRIORITY
#define LSM6DSV16X_TASK_CFG_PRIORITY                 (4/*TX_MAX_PRIORITIES - 1*/)
#endif

#ifndef LSM6DSV16X_TASK_CFG_IN_QUEUE_LENGTH
#define LSM6DSV16X_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define LSM6DSV16X_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef LSM6DSV16X_TASK_CFG_TIMER_PERIOD_MS
#define LSM6DSV16X_TASK_CFG_TIMER_PERIOD_MS          1000
#endif
#ifndef LSM6DSV16X_TASK_CFG_MLCT_IMER_PERIOD_MS
#define LSM6DSV16X_TASK_CFG_MLC_TIMER_PERIOD_MS      200
#endif

#define LSM6DSV16X_TAG_ACC                           (0x02)

#define SYS_DEBUGF(level, message)                   SYS_DEBUGF3(SYS_DBG_LSM6DSV16X, level, message)

#if defined(DEBUG) || defined (SYS_DEBUG)
#define sTaskObj                                     sLSM6DSV16XTaskObj
#endif

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static int16_t dummyDataCounter_acc = 0;
static int16_t dummyDataCounter_gyro = 0;
#endif

/**
  *  LSM6DSV16XTask internal structure.
  */
struct _LSM6DSV16XTask
{
  /**
    * Base class object.
    */
  AManagedTaskEx super;

  // Task variables should be added here.

  /**
    * IRQ GPIO configuration parameters.
    */
  const MX_GPIOParams_t *pIRQConfig;

  /**
    * MLC GPIO configuration parameters.
    */
  const MX_GPIOParams_t *pMLCConfig;

  /**
    * SPI CS GPIO configuration parameters.
    */
  const MX_GPIOParams_t *pCSConfig;

  /**
    * Bus IF object used to connect the sensor task to the specific bus.
    */
  ABusIF *p_sensor_bus_if;

  /**
    * Implements the accelerometer ISensor interface.
    */
  ISensor_t acc_sensor_if;

  /**
    * Implements the gyroscope ISensor interface.
    */
  ISensor_t gyro_sensor_if;

  /**
    * Implements the mlc ISensor interface.
    */
  ISensor_t mlc_sensor_if;

  /**
    * Implements the ISensorLL interface - Sensor Low-level.
    */
  ISensorLL_t sensor_ll_if;

  /**
    * Specifies accelerometer sensor capabilities.
    */
  const SensorDescriptor_t *acc_sensor_descriptor;

  /**
    * Specifies accelerometer sensor configuration.
    */
  SensorStatus_t acc_sensor_status;

  EMData_t data_acc;
  /**
    * Specifies gyroscope sensor capabilities.
    */
  const SensorDescriptor_t *gyro_sensor_descriptor;

  /**
    * Specifies gyroscope sensor configuration.
    */
  SensorStatus_t gyro_sensor_status;

  EMData_t data_gyro;
  /**
    * Specifies mlc sensor capabilities.
    */
  const SensorDescriptor_t *mlc_sensor_descriptor;

  /**
    * Specifies mlc sensor configuration.
    */
  SensorStatus_t mlc_sensor_status;

  EMData_t data;
  /**
    * Specifies the sensor ID for the accelerometer subsensor.
    */
  uint8_t acc_id;

  /**
    * Specifies the sensor ID for the gyroscope subsensor.
    */
  uint8_t gyro_id;

  /**
    * Specifies the sensor ID for the mlc subsensor.
    */
  uint8_t mlc_id;

  /**
    * Specifies mlc status.
    */
  boolean_t mlc_enable;

  /**
    * Synchronization object used to send command to the task.
    */
  TX_QUEUE in_queue;

#if LSM6DSV16X_FIFO_ENABLED
  /**
    * Buffer to store the data read from the sensor FIFO.
    * It is reused also to save data from the faster subsensor
    */
  uint8_t p_fast_sensor_data_buff[LSM6DSV16X_MAX_SAMPLES_PER_IT * 7];

  /**
    * Buffer to store the data from the slower subsensor
    */
  uint8_t p_slow_sensor_data_buff[LSM6DSV16X_MAX_SAMPLES_PER_IT / 2 * 6];
#else
  /**
    * Buffer to store the data read from the sensor FIFO.
    * It is reused also to save data from the faster subsensor
    */
  uint8_t p_acc_sample[6];

  /**
    * Buffer to store the data from the slower subsensor
   */
  uint8_t p_gyro_sample[6];

  /**
    * Save acc data ready status
   */
  uint8_t acc_drdy;

  /**
    * Save gyro data ready status
    */
  uint8_t gyro_drdy;
#endif

  /**
    * Buffer to store the data from mlc
    */
  uint8_t p_mlc_sensor_data_buff[5];

  /**
    * Specifies the FIFO level
    */
  uint16_t fifo_level;

  /**
    * Specifies the FIFO watermark level (it depends from ODR)
    */
  uint16_t samples_per_it;

  /**
    * If both subsensors are active, specifies the amount of ACC samples in the FIFO
    */
  uint16_t acc_samples_count;

  /**
    * If both subsensors are active, specifies the amount of GYRO samples in the FIFO
    */
  uint16_t gyro_samples_count;

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_acc_event_src;

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_gyro_event_src;

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_mlc_event_src;

  /**
    * Software timer used to generate the read command
    */
  TX_TIMER read_timer;

  /**
    * Timer period used to schedule the read command
    */
  ULONG lsm6dsv16x_task_cfg_timer_period_ms;

  /**
    * Software timer used to generate the mlc read command
    */
  TX_TIMER mlc_timer;

  /**
    * Used to update the instantaneous ODR.
    */
  double prev_timestamp;

  /**
    * Internal model (FW) is in sync with the component (HW registers)
    */
  bool sync;
};

/**
  * Class object declaration
  */
typedef struct _LSM6DSV16XTaskClass
{
  /**
    * LSM6DSV16XTask class virtual table.
    */
  AManagedTaskEx_vtbl vtbl;

  /**
    * Accelerometer IF virtual table.
    */
  ISensor_vtbl acc_sensor_if_vtbl;

  /**
    * Gyro IF virtual table.
    */
  ISensor_vtbl gyro_sensor_if_vtbl;

  /**
    * mlc IF virtual table.
    */
  ISensor_vtbl mlc_sensor_if_vtbl;

  /**
    * SensorLL IF virtual table.
    */
  ISensorLL_vtbl sensor_ll_if_vtbl;

  /**
    * Specifies accelerometer sensor capabilities.
    */
  SensorDescriptor_t acc_class_descriptor;

  /**
    * Specifies gyroscope sensor capabilities.
    */
  SensorDescriptor_t gyro_class_descriptor;

  /**
    * Specifies mlc sensor capabilities.
    */
  SensorDescriptor_t mlc_class_descriptor;

  /**
    * LSM6DSV16XTask (PM_STATE, ExecuteStepFunc) map.
    */
  pExecuteStepFunc_t p_pm_state2func_map[3];
} LSM6DSV16XTaskClass_t;

/* Private member function declaration */

/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LSM6DSV16XTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LSM6DSV16XTaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LSM6DSV16XTaskSensorInit(LSM6DSV16XTask *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LSM6DSV16XTaskSensorReadData(LSM6DSV16XTask *_this);

/**
  * Read the data from the mlc.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LSM6DSV16XTaskSensorReadMLC(LSM6DSV16XTask *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t LSM6DSV16XTaskSensorRegister(LSM6DSV16XTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t LSM6DSV16XTaskSensorInitTaskParams(LSM6DSV16XTask *_this);

/**
  * Private implementation of sensor interface methods for LSM6DSV16X sensor
  */

static sys_error_code_t LSM6DSV16XTaskSensorSetODR(LSM6DSV16XTask *_this, SMMessage report);
static sys_error_code_t LSM6DSV16XTaskSensorSetFS(LSM6DSV16XTask *_this, SMMessage report);
static sys_error_code_t LSM6DSV16XTaskSensorSetFifoWM(LSM6DSV16XTask *_this, SMMessage report);
static sys_error_code_t LSM6DSV16XTaskSensorEnable(LSM6DSV16XTask *_this, SMMessage report);
static sys_error_code_t LSM6DSV16XTaskSensorDisable(LSM6DSV16XTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t LSM6DSV16XTaskSensorIsActive(const LSM6DSV16XTask *_this);

static sys_error_code_t LSM6DSV16XTaskEnterLowPowerMode(const LSM6DSV16XTask *_this);

static sys_error_code_t LSM6DSV16XTaskConfigureIrqPin(const LSM6DSV16XTask *_this, boolean_t LowPower);
static sys_error_code_t LSM6DSV16XTaskConfigureMLCPin(const LSM6DSV16XTask *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param timer [IN] specifies the handle of the expired timer.
  */
static void LSM6DSV16XTaskTimerCallbackFunction(ULONG timer);

/**
  * Callback function called when the mlc software timer expires.
  *
  * @param timer [IN] specifies the handle of the expired timer.
  */
static void LSM6DSV16XTaskMLCTimerCallbackFunction(ULONG timer);

/**
  * Given a interface pointer it return the instance of the object that implement the interface.
  *
  * @param p_if [IN] specifies a sensor interface implemented by the task object.
  * @return the instance of the task object that implements the given interface.
  */
static inline LSM6DSV16XTask *LSM6DSV16XTaskGetOwnerFromISensorIF(ISensor_t *p_if);

/**
  * Given a interface pointer it return the instance of the object that implement the interface.
  *
  * @param p_if [IN] specifies a ISensorLL interface implemented by the task object.
  * @return the instance of the task object that implements the given interface.
  */
static inline LSM6DSV16XTask *LSM6DSV16XTaskGetOwnerFromISensorLLIF(ISensorLL_t *p_if);

/**
  * Interrupt callback
  */
void LSM6DSV16XTask_EXTI_Callback(uint16_t Pin);
void INT2_DSV16X_EXTI_Callback(uint16_t Pin);

/**
  * Read the ODR value from the sensor and update the internal model
  */
static sys_error_code_t LSM6DSV16X_ODR_Sync(LSM6DSV16XTask *_this);

/**
  * Read the ODR value from the sensor and update the internal model
  */
static sys_error_code_t LSM6DSV16X_FS_Sync(LSM6DSV16XTask *_this);

/* Inline function forward declaration */

/**
  * Private function used to post a report into the front of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t LSM6DSV16XTaskPostReportToFront(LSM6DSV16XTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t LSM6DSV16XTaskPostReportToBack(LSM6DSV16XTask *_this, SMMessage *pReport);

/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
static LSM6DSV16XTask sTaskObj;

/**
  * The class object.
  */
static const LSM6DSV16XTaskClass_t sTheClass =
{
  /* class virtual table */
  {
    LSM6DSV16XTask_vtblHardwareInit,
    LSM6DSV16XTask_vtblOnCreateTask,
    LSM6DSV16XTask_vtblDoEnterPowerMode,
    LSM6DSV16XTask_vtblHandleError,
    LSM6DSV16XTask_vtblOnEnterTaskControlLoop,
    LSM6DSV16XTask_vtblForceExecuteStep,
    LSM6DSV16XTask_vtblOnEnterPowerMode
  },

  /* class::acc_sensor_if_vtbl virtual table */
  {
    LSM6DSV16XTask_vtblAccGetId,
    LSM6DSV16XTask_vtblAccGetEventSourceIF,
    LSM6DSV16XTask_vtblAccGetDataInfo,
    LSM6DSV16XTask_vtblAccGetODR,
    LSM6DSV16XTask_vtblAccGetFS,
    LSM6DSV16XTask_vtblAccGetSensitivity,
    LSM6DSV16XTask_vtblSensorSetODR,
    LSM6DSV16XTask_vtblSensorSetFS,
    LSM6DSV16XTask_vtblSensorSetFifoWM,
    LSM6DSV16XTask_vtblSensorEnable,
    LSM6DSV16XTask_vtblSensorDisable,
    LSM6DSV16XTask_vtblSensorIsEnabled,
    LSM6DSV16XTask_vtblAccGetDescription,
    LSM6DSV16XTask_vtblAccGetStatus
  },

  /* class::gyro_sensor_if_vtbl virtual table */
  {
    LSM6DSV16XTask_vtblGyroGetId,
    LSM6DSV16XTask_vtblGyroGetEventSourceIF,
    LSM6DSV16XTask_vtblGyroGetDataInfo,
    LSM6DSV16XTask_vtblGyroGetODR,
    LSM6DSV16XTask_vtblGyroGetFS,
    LSM6DSV16XTask_vtblGyroGetSensitivity,
    LSM6DSV16XTask_vtblSensorSetODR,
    LSM6DSV16XTask_vtblSensorSetFS,
    LSM6DSV16XTask_vtblSensorSetFifoWM,
    LSM6DSV16XTask_vtblSensorEnable,
    LSM6DSV16XTask_vtblSensorDisable,
    LSM6DSV16XTask_vtblSensorIsEnabled,
    LSM6DSV16XTask_vtblGyroGetDescription,
    LSM6DSV16XTask_vtblGyroGetStatus
  },

  /* class::mlc_sensor_if_vtbl virtual table */
  {
    LSM6DSV16XTask_vtblMlcGetId,
    LSM6DSV16XTask_vtblMlcGetEventSourceIF,
    LSM6DSV16XTask_vtblMlcGetDataInfo,
    LSM6DSV16XTask_vtblMlcGetODR,
    LSM6DSV16XTask_vtblMlcGetFS,
    LSM6DSV16XTask_vtblMlcGetSensitivity,
    LSM6DSV16XTask_vtblSensorSetODR,
    LSM6DSV16XTask_vtblSensorSetFS,
    LSM6DSV16XTask_vtblSensorSetFifoWM,
    LSM6DSV16XTask_vtblSensorEnable,
    LSM6DSV16XTask_vtblSensorDisable,
    LSM6DSV16XTask_vtblSensorIsEnabled,
    LSM6DSV16XTask_vtblMlcGetDescription,
    LSM6DSV16XTask_vtblMlcGetStatus
  },

  /* class::sensor_ll_if_vtbl virtual table */
  {
    LSM6DSV16XTask_vtblSensorReadReg,
    LSM6DSV16XTask_vtblSensorWriteReg,
    LSM6DSV16XTask_vtblSensorSyncModel
  },

  /* ACCELEROMETER DESCRIPTOR */
  {
    "lsm6dsv16x",
    COM_TYPE_ACC,
    {
      7.5,
      15,
      30,
      60,
      120,
      240,
      480,
      960,
      1920,
      3840,
      7680,
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
    "lsm6dsv16x",
    COM_TYPE_GYRO,
    {
      7.5,
      15,
      30,
      60,
      120,
      240,
      420,
      960,
      1920,
      3840,
      7680,
      COM_END_OF_LIST_FLOAT,
    },
    {
      125,
      250,
      500,
      1000,
      2000,
      4000,
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

  /* MLC DESCRIPTOR */
  {
    "lsm6dsv16x",
    COM_TYPE_MLC,
    {
      1,
      COM_END_OF_LIST_FLOAT,
    },
    {
      1,
      COM_END_OF_LIST_FLOAT,
    },
    {
      "mlc",
    },
    "out",
    {
      0,
      1,
    }
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    LSM6DSV16XTaskExecuteStepState1,
    NULL,
    LSM6DSV16XTaskExecuteStepDatalog,
  }
};

/* Public API definition */

ISourceObservable *LSM6DSV16XTaskGetAccSensorIF(LSM6DSV16XTask *_this)
{
  return (ISourceObservable *) & (_this->acc_sensor_if);
}

ISourceObservable *LSM6DSV16XTaskGetGyroSensorIF(LSM6DSV16XTask *_this)
{
  return (ISourceObservable *) & (_this->gyro_sensor_if);
}

ISourceObservable *LSM6DSV16XTaskGetMlcSensorIF(LSM6DSV16XTask *_this)
{
  return (ISourceObservable *) & (_this->mlc_sensor_if);
}

ISensorLL_t *LSM6DSV16XTaskGetSensorLLIF(LSM6DSV16XTask *_this)
{
  return (ISensorLL_t *) & (_this->sensor_ll_if);
}

AManagedTaskEx *LSM6DSV16XTaskAlloc(const void *pIRQConfig, const void *pMLCConfig, const void *pCSConfig)
{
  /* This allocator implements the singleton design pattern. */

  /* Initialize the super class */
  AMTInitEx(&sTaskObj.super);

  sTaskObj.super.vptr = &sTheClass.vtbl;
  sTaskObj.acc_sensor_if.vptr = &sTheClass.acc_sensor_if_vtbl;
  sTaskObj.gyro_sensor_if.vptr = &sTheClass.gyro_sensor_if_vtbl;
  sTaskObj.mlc_sensor_if.vptr = &sTheClass.mlc_sensor_if_vtbl;
  sTaskObj.sensor_ll_if.vptr = &sTheClass.sensor_ll_if_vtbl;
  sTaskObj.acc_sensor_descriptor = &sTheClass.acc_class_descriptor;
  sTaskObj.gyro_sensor_descriptor = &sTheClass.gyro_class_descriptor;
  sTaskObj.mlc_sensor_descriptor = &sTheClass.mlc_class_descriptor;

  sTaskObj.pIRQConfig = (MX_GPIOParams_t *) pIRQConfig;
  sTaskObj.pMLCConfig = (MX_GPIOParams_t *) pMLCConfig;
  sTaskObj.pCSConfig = (MX_GPIOParams_t *) pCSConfig;

  strcpy(sTaskObj.acc_sensor_status.Name, sTheClass.acc_class_descriptor.Name);
  strcpy(sTaskObj.gyro_sensor_status.Name, sTheClass.gyro_class_descriptor.Name);
  strcpy(sTaskObj.mlc_sensor_status.Name, sTheClass.mlc_class_descriptor.Name);

  return (AManagedTaskEx *) &sTaskObj;
}

ABusIF *LSM6DSV16XTaskGetSensorIF(LSM6DSV16XTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *LSM6DSV16XTaskGetAccEventSrcIF(LSM6DSV16XTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_acc_event_src;
}

IEventSrc *LSM6DSV16XTaskGetGyroEventSrcIF(LSM6DSV16XTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_gyro_event_src;
}

IEventSrc *LSM6DSV16XTaskGetMlcEventSrcIF(LSM6DSV16XTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_mlc_event_src;
}

/* AManagedTaskEx virtual functions definition */

sys_error_code_t LSM6DSV16XTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16XTask *p_obj = (LSM6DSV16XTask *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t LSM6DSV16XTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                                 VOID **pvStackStart,
                                                 ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                                 ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16XTask *p_obj = (LSM6DSV16XTask *) _this;
  p_obj->sync = true;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) LSM6DSV16X_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(LSM6DSV16X_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
  }
  else if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "LSM6DSV16X_Q", item_size / 4u, p_queue_items_buff,
                                         LSM6DSV16X_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
  } /* create the software timer*/
  else if (TX_SUCCESS
           != tx_timer_create(&p_obj->read_timer, "LSM6DSV16X_T", LSM6DSV16XTaskTimerCallbackFunction, (ULONG)TX_NULL,
                              AMT_MS_TO_TICKS(LSM6DSV16X_TASK_CFG_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
  } /* create the mlc software timer*/
  else if (TX_SUCCESS
           != tx_timer_create(&p_obj->mlc_timer, "LSM6DSV16X_MLC_T", LSM6DSV16XTaskMLCTimerCallbackFunction, (ULONG)TX_NULL,
                              AMT_MS_TO_TICKS(LSM6DSV16X_TASK_CFG_MLC_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
  } /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  else if (p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(LSM6DSV16X_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(LSM6DSV16X_ID, LSM6DSV16X_I2C_ADD_H, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }

  if (!SYS_IS_ERROR_CODE(res))
  {
    /* Initialize the EventSrc interface, take the ownership of the interface. */
    p_obj->p_acc_event_src = DataEventSrcAlloc();
    if (p_obj->p_acc_event_src == NULL)
    {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
      res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    }
    else
    {
      IEventSrcInit(p_obj->p_acc_event_src);

      p_obj->p_gyro_event_src = DataEventSrcAlloc();
      if (p_obj->p_gyro_event_src == NULL)
      {
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
        res = SYS_OUT_OF_MEMORY_ERROR_CODE;
      }
      else
      {
        IEventSrcInit(p_obj->p_gyro_event_src);

        p_obj->p_mlc_event_src = DataEventSrcAlloc();
        if (p_obj->p_mlc_event_src == NULL)
        {
          SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
          res = SYS_OUT_OF_MEMORY_ERROR_CODE;
        }
        else
        {
          IEventSrcInit(p_obj->p_mlc_event_src);

#if LSM6DSV16X_FIFO_ENABLED
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
          _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

          *pTaskCode = AMTExRun;
          *pName = "LSM6DSV16X";
          *pvStackStart = NULL; // allocate the task stack in the system memory pool.
          *pStackDepth = LSM6DSV16X_TASK_CFG_STACK_DEPTH;
          *pParams = (ULONG) _this;
          *pPriority = LSM6DSV16X_TASK_CFG_PRIORITY;
          *pPreemptThreshold = LSM6DSV16X_TASK_CFG_PRIORITY;
          *pTimeSlice = TX_NO_TIME_SLICE;
          *pAutoStart = TX_AUTO_START;

          res = LSM6DSV16XTaskSensorInitTaskParams(p_obj);
          if (SYS_IS_ERROR_CODE(res))
          {
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
            res = SYS_OUT_OF_MEMORY_ERROR_CODE;
          }
          else
          {
            res = LSM6DSV16XTaskSensorRegister(p_obj);
            if (SYS_IS_ERROR_CODE(res))
            {
              SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV16X: unable to register with DB\r\n"));
              sys_error_handler();
            }
          }
        }
      }
    }
  }
  return res;
}

sys_error_code_t LSM6DSV16XTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16XTask *p_obj = (LSM6DSV16XTask *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (LSM6DSV16XTaskSensorIsActive(p_obj))
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

      // reset the variables for the time stamp computation.
      p_obj->prev_timestamp = 0.0f;
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV16X: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      /* Deactivate the sensor */
      lsm6dsv16x_xl_data_rate_set(p_sensor_drv, LSM6DSV16X_ODR_OFF);
      lsm6dsv16x_gy_data_rate_set(p_sensor_drv, LSM6DSV16X_ODR_OFF);
      lsm6dsv16x_fifo_gy_batch_set(p_sensor_drv, LSM6DSV16X_GY_NOT_BATCHED);
      lsm6dsv16x_fifo_xl_batch_set(p_sensor_drv, LSM6DSV16X_XL_NOT_BATCHED);
      lsm6dsv16x_fifo_mode_set(p_sensor_drv, LSM6DSV16X_BYPASS_MODE);
      p_obj->samples_per_it = 0;

      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_timer);
      }
      else
      {
        LSM6DSV16XTaskConfigureIrqPin(p_obj, TRUE);
      }
      if (p_obj->pMLCConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->mlc_timer);
      }
      else
      {
        LSM6DSV16XTaskConfigureMLCPin(p_obj, TRUE);
      }
    }
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV16X: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = LSM6DSV16XTaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      sys_error_handler();
    }
    if (p_obj->pIRQConfig != NULL)
    {
      LSM6DSV16XTaskConfigureIrqPin(p_obj, TRUE);
    }
    if (p_obj->pMLCConfig != NULL)
    {
      LSM6DSV16XTaskConfigureMLCPin(p_obj, TRUE);
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV16X: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t LSM6DSV16XTask_vtblHandleError(AManagedTask *_this, SysEvent Error)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t LSM6DSV16XTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV16X: start.\r\n"));

#if defined (ENABLE_THREADX_DBG_PIN) && defined (LSM6DSV16X_TASK_CFG_TAG)
  LSM6DSV16XTask *p_obj = (LSM6DSV16XTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = LSM6DSV16X_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t LSM6DSV16XTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16XTask *p_obj = (LSM6DSV16XTask *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = LSM6DSV16XTaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t LSM6DSV16XTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  LSM6DSV16XTask *p_obj = (LSM6DSV16XTask*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t LSM6DSV16XTask_vtblAccGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = (LSM6DSV16XTask *)((uint32_t) _this - offsetof(LSM6DSV16XTask, acc_sensor_if));
  uint8_t res = p_if_owner->acc_id;

  return res;
}

uint8_t LSM6DSV16XTask_vtblGyroGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = (LSM6DSV16XTask *)((uint32_t) _this - offsetof(LSM6DSV16XTask, gyro_sensor_if));
  uint8_t res = p_if_owner->gyro_id;

  return res;
}

uint8_t LSM6DSV16XTask_vtblMlcGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = (LSM6DSV16XTask *)((uint32_t) _this - offsetof(LSM6DSV16XTask, mlc_sensor_if));
  uint8_t res = p_if_owner->mlc_id;

  return res;
}

IEventSrc *LSM6DSV16XTask_vtblAccGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = (LSM6DSV16XTask *)((uint32_t) _this - offsetof(LSM6DSV16XTask, acc_sensor_if));

  return p_if_owner->p_acc_event_src;
}

IEventSrc *LSM6DSV16XTask_vtblGyroGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = (LSM6DSV16XTask *)((uint32_t) _this - offsetof(LSM6DSV16XTask, gyro_sensor_if));
  return p_if_owner->p_gyro_event_src;
}

IEventSrc *LSM6DSV16XTask_vtblMlcGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = (LSM6DSV16XTask *)((uint32_t) _this - offsetof(LSM6DSV16XTask, mlc_sensor_if));
  return p_if_owner->p_mlc_event_src;
}

sys_error_code_t LSM6DSV16XTask_vtblAccGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LSM6DSV16XTask *p_if_owner = (LSM6DSV16XTask *)((uint32_t) _this - offsetof(LSM6DSV16XTask, acc_sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if ((p_measured == NULL) || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = p_if_owner->acc_sensor_status.MeasuredODR;
    *p_nominal = p_if_owner->acc_sensor_status.ODR;
  }

  return res;
}

float LSM6DSV16XTask_vtblAccGetFS(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = (LSM6DSV16XTask *)((uint32_t) _this - offsetof(LSM6DSV16XTask, acc_sensor_if));
  float res = p_if_owner->acc_sensor_status.FS;

  return res;
}

float LSM6DSV16XTask_vtblAccGetSensitivity(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = (LSM6DSV16XTask *)((uint32_t) _this - offsetof(LSM6DSV16XTask, acc_sensor_if));
  float res = p_if_owner->acc_sensor_status.Sensitivity;

  return res;
}

EMData_t LSM6DSV16XTask_vtblAccGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = (LSM6DSV16XTask *)((uint32_t) _this - offsetof(LSM6DSV16XTask, acc_sensor_if));
  EMData_t res = p_if_owner->data_acc;

  return res;
}

sys_error_code_t LSM6DSV16XTask_vtblGyroGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LSM6DSV16XTask *p_if_owner = (LSM6DSV16XTask *)((uint32_t) _this - offsetof(LSM6DSV16XTask, gyro_sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if ((p_measured == NULL) || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = p_if_owner->gyro_sensor_status.MeasuredODR;
    *p_nominal = p_if_owner->gyro_sensor_status.ODR;
  }

  return res;
}

float LSM6DSV16XTask_vtblGyroGetFS(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = (LSM6DSV16XTask *)((uint32_t) _this - offsetof(LSM6DSV16XTask, gyro_sensor_if));
  float res = p_if_owner->gyro_sensor_status.FS;

  return res;
}

float LSM6DSV16XTask_vtblGyroGetSensitivity(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = (LSM6DSV16XTask *)((uint32_t) _this - offsetof(LSM6DSV16XTask, gyro_sensor_if));
  float res = p_if_owner->gyro_sensor_status.Sensitivity;

  return res;
}

EMData_t LSM6DSV16XTask_vtblGyroGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = (LSM6DSV16XTask *)((uint32_t) _this - offsetof(LSM6DSV16XTask, gyro_sensor_if));
  EMData_t res = p_if_owner->data_gyro;

  return res;
}

sys_error_code_t LSM6DSV16XTask_vtblMlcGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LSM6DSV16XTask *p_if_owner = (LSM6DSV16XTask *)((uint32_t) _this - offsetof(LSM6DSV16XTask, mlc_sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if ((p_measured == NULL) || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = p_if_owner->mlc_sensor_status.MeasuredODR;
    *p_nominal = p_if_owner->mlc_sensor_status.ODR;
  }

  return res;
}

float LSM6DSV16XTask_vtblMlcGetFS(ISourceObservable *_this)
{
  assert_param(_this != NULL);

  /* MLC does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV16X: warning - MLC GetFS() not supported.\r\n"));

  return -1.0f;
}

float LSM6DSV16XTask_vtblMlcGetSensitivity(ISourceObservable *_this)
{
  assert_param(_this != NULL);

  /* MLC does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV16X: warning - MLC GetSensitivity() not supported.\r\n"));

  return -1.0f;
}

EMData_t LSM6DSV16XTask_vtblMlcGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = (LSM6DSV16XTask *)((uint32_t) _this - offsetof(LSM6DSV16XTask, mlc_sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t LSM6DSV16XTask_vtblSensorSetODR(ISensor_t *_this, float ODR)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16XTask *p_if_owner = LSM6DSV16XTaskGetOwnerFromISensorIF(_this);

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
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_ODR,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) ODR
    };
    res = LSM6DSV16XTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t LSM6DSV16XTask_vtblSensorSetFS(ISensor_t *_this, float FS)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16XTask *p_if_owner = LSM6DSV16XTaskGetOwnerFromISensorIF(_this);

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
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FS,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) FS
    };
    res = LSM6DSV16XTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t LSM6DSV16XTask_vtblSensorSetFifoWM(ISensor_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16XTask *p_if_owner = LSM6DSV16XTaskGetOwnerFromISensorIF(_this);

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
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FIFO_WM,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint16_t) fifoWM
    };
    res = LSM6DSV16XTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t LSM6DSV16XTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16XTask *p_if_owner = LSM6DSV16XTaskGetOwnerFromISensorIF(_this);

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
    res = LSM6DSV16XTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t LSM6DSV16XTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16XTask *p_if_owner = LSM6DSV16XTaskGetOwnerFromISensorIF(_this);

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
    res = LSM6DSV16XTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t LSM6DSV16XTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  LSM6DSV16XTask *p_if_owner = LSM6DSV16XTaskGetOwnerFromISensorIF(_this);

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->acc_id)
  {
    res = p_if_owner->acc_sensor_status.IsActive;
  }
  else if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->gyro_id)
  {
    res = p_if_owner->gyro_sensor_status.IsActive;
  }
  else if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->mlc_id)
  {
    res = p_if_owner->mlc_sensor_status.IsActive;
  }

  return res;
}

SensorDescriptor_t LSM6DSV16XTask_vtblAccGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = LSM6DSV16XTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->acc_sensor_descriptor;
}

SensorDescriptor_t LSM6DSV16XTask_vtblGyroGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = LSM6DSV16XTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->gyro_sensor_descriptor;
}

SensorDescriptor_t LSM6DSV16XTask_vtblMlcGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = LSM6DSV16XTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->mlc_sensor_descriptor;
}

SensorStatus_t LSM6DSV16XTask_vtblAccGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = LSM6DSV16XTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->acc_sensor_status;
}

SensorStatus_t LSM6DSV16XTask_vtblGyroGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = LSM6DSV16XTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->gyro_sensor_status;
}

SensorStatus_t LSM6DSV16XTask_vtblMlcGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV16XTask *p_if_owner = LSM6DSV16XTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->mlc_sensor_status;
}

sys_error_code_t LSM6DSV16XTask_vtblSensorReadReg(ISensorLL_t *_this, uint16_t reg, uint8_t *data, uint16_t len)
{
  assert_param(_this != NULL);
  assert_param(reg <= 0xFFU);
  assert_param(data != NULL);
  assert_param(len != 0U);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16XTask *p_if_owner = LSM6DSV16XTaskGetOwnerFromISensorLLIF(_this);
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_if_owner->p_sensor_bus_if->m_xConnector;
  uint8_t reg8 = (uint8_t)(reg & 0x00FF);

  if (lsm6dsv16x_read_reg(p_sensor_drv, reg8, data, len) != 0)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t LSM6DSV16XTask_vtblSensorWriteReg(ISensorLL_t *_this, uint16_t reg, const uint8_t *data, uint16_t len)
{
  assert_param(_this != NULL);
  assert_param(reg <= 0xFFU);
  assert_param(data != NULL);
  assert_param(len != 0U);

  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16XTask *p_if_owner = LSM6DSV16XTaskGetOwnerFromISensorLLIF(_this);
  uint8_t reg8 = (uint8_t)(reg & 0x00FF);

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_if_owner->p_sensor_bus_if->m_xConnector;

  /* This generic register write operation could mean that the model is out of sync with the HW */
  p_if_owner->sync = false;

  if (lsm6dsv16x_write_reg(p_sensor_drv, reg8, (uint8_t *) data, len) != 0)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t LSM6DSV16XTask_vtblSensorSyncModel(ISensorLL_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16XTask *p_if_owner = LSM6DSV16XTaskGetOwnerFromISensorLLIF(_this);

  if (LSM6DSV16X_ODR_Sync(p_if_owner) != SYS_NO_ERROR_CODE)
  {
    res = SYS_BASE_ERROR_CODE;
  }
  if (LSM6DSV16X_FS_Sync(p_if_owner) != SYS_NO_ERROR_CODE)
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
static sys_error_code_t LSM6DSV16XTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16XTask *p_obj = (LSM6DSV16XTask *) _this;
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
            res = LSM6DSV16XTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = LSM6DSV16XTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = LSM6DSV16XTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = LSM6DSV16XTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = LSM6DSV16XTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV16X: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV16X: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t LSM6DSV16XTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV16XTask *p_obj = (LSM6DSV16XTask *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("LSM6DSV16X: new data.\r\n"));
        if (p_obj->pIRQConfig == NULL)
        {
          if (TX_SUCCESS
              != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->lsm6dsv16x_task_cfg_timer_period_ms),
                                 AMT_MS_TO_TICKS(p_obj->lsm6dsv16x_task_cfg_timer_period_ms)))
          {
            return SYS_UNDEFINED_ERROR_CODE;
          }
        }

        res = LSM6DSV16XTaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
#if LSM6DSV16X_FIFO_ENABLED
          if (p_obj->fifo_level != 0)
          {
#endif
            // notify the listeners...
            double timestamp = report.sensorDataReadyMessage.fTimestamp;
            double delta_timestamp = timestamp - p_obj->prev_timestamp;
            p_obj->prev_timestamp = timestamp;
            DataEvent_t evt_acc, evt_gyro;

#if LSM6DSV16X_FIFO_ENABLED

            if ((p_obj->acc_sensor_status.IsActive) && (p_obj->gyro_sensor_status.IsActive)) /* Read both ACC and GYRO */
            {
              /* update measuredODR */
              p_obj->acc_sensor_status.MeasuredODR = (float) p_obj->acc_samples_count / (float) delta_timestamp;
              p_obj->gyro_sensor_status.MeasuredODR = (float) p_obj->gyro_samples_count / (float) delta_timestamp;

              if (p_obj->acc_sensor_status.ODR > p_obj->gyro_sensor_status.ODR) /* Acc is faster than Gyro */
              {
                /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue:
                 * [X0, Y0, Z0]
                 * [X1, Y1, Z1]
                 * ...
                 * [Xm-1, Ym-1, Zm-1]
                 */
                EMD_Init(&p_obj->data_acc, p_obj->p_fast_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                         p_obj->acc_samples_count, 3);
                DataEventInit((IEvent *) &evt_acc, p_obj->p_acc_event_src, &p_obj->data_acc, timestamp, p_obj->acc_id);

                EMD_Init(&p_obj->data_gyro, p_obj->p_slow_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                         p_obj->gyro_samples_count, 3);
                DataEventInit((IEvent *) &evt_gyro, p_obj->p_gyro_event_src, &p_obj->data_gyro, timestamp, p_obj->gyro_id);

                IEventSrcSendEvent(p_obj->p_acc_event_src, (IEvent *) &evt_acc, NULL);
                IEventSrcSendEvent(p_obj->p_gyro_event_src, (IEvent *) &evt_gyro, NULL);
              }
              else
              {
                /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue:
                 * [X0, Y0, Z0]
                 * [X1, Y1, Z1]
                 * ...
                 * [Xm-1, Ym-1, Zm-1]
                 */
                EMD_Init(&p_obj->data_acc, p_obj->p_slow_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                         p_obj->acc_samples_count, 3);
                DataEventInit((IEvent *) &evt_acc, p_obj->p_acc_event_src, &p_obj->data_acc, timestamp, p_obj->acc_id);

                EMD_Init(&p_obj->data_gyro, p_obj->p_fast_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                         p_obj->gyro_samples_count, 3);
                DataEventInit((IEvent *) &evt_gyro, p_obj->p_gyro_event_src, &p_obj->data_gyro, timestamp, p_obj->gyro_id);

                IEventSrcSendEvent(p_obj->p_acc_event_src, (IEvent *) &evt_acc, NULL);
                IEventSrcSendEvent(p_obj->p_gyro_event_src, (IEvent *) &evt_gyro, NULL);
              }

            }
            else /* Only 1 out of 2 is active */
            {
              if (p_obj->acc_sensor_status.IsActive)
              {
                /* update measuredODR */
                p_obj->acc_sensor_status.MeasuredODR = (float) p_obj->acc_samples_count / (float) delta_timestamp;

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
              else if (p_obj->gyro_sensor_status.IsActive)
              {
                /* update measuredODR */
                p_obj->gyro_sensor_status.MeasuredODR = (float) p_obj->gyro_samples_count / (float) delta_timestamp;

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
            if (p_obj->acc_sensor_status.IsActive && p_obj->acc_drdy)
            {
              /* update measuredODR */
              p_obj->acc_sensor_status.MeasuredODR = (float)p_obj->acc_samples_count / (float)delta_timestamp;

              EMD_Init(&p_obj->data_acc, p_obj->p_acc_sample, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, p_obj->acc_samples_count, 3);
              DataEventInit((IEvent *) &evt_acc, p_obj->p_acc_event_src, &p_obj->data_acc, timestamp, p_obj->acc_id);

              IEventSrcSendEvent(p_obj->p_acc_event_src, (IEvent *) &evt_acc, NULL);
              p_obj->acc_drdy = 0;
            }
            if (p_obj->gyro_sensor_status.IsActive && p_obj->gyro_drdy)
            {
              /* update measuredODR */
              p_obj->gyro_sensor_status.MeasuredODR = (float)p_obj->gyro_samples_count / (float)delta_timestamp;

              EMD_Init(&p_obj->data_gyro, p_obj->p_gyro_sample, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, p_obj->gyro_samples_count, 3);
              DataEventInit((IEvent *) &evt_gyro, p_obj->p_gyro_event_src, &p_obj->data_gyro, timestamp, p_obj->gyro_id);

              IEventSrcSendEvent(p_obj->p_gyro_event_src, (IEvent *) &evt_gyro, NULL);
              p_obj->gyro_drdy = 0;
            }
#endif

            SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("LSM6DSV16X: ts = %f\r\n", (float)timestamp));
#if LSM6DSV16X_FIFO_ENABLED
          }
#endif
        }
        if (p_obj->pIRQConfig == NULL)
        {
          if (TX_SUCCESS != tx_timer_activate(&p_obj->read_timer))
          {
            res = SYS_UNDEFINED_ERROR_CODE;
          }
        }

        break;
      }

      case SM_MESSAGE_ID_DATA_READY_MLC:
      {
        if (p_obj->pMLCConfig == NULL)
        {
          if (TX_SUCCESS
              != tx_timer_change(&p_obj->mlc_timer, AMT_MS_TO_TICKS(LSM6DSV16X_TASK_CFG_MLC_TIMER_PERIOD_MS),
                                 AMT_MS_TO_TICKS(LSM6DSV16X_TASK_CFG_MLC_TIMER_PERIOD_MS)))
          {
            return SYS_UNDEFINED_ERROR_CODE;
          }
        }
        res = LSM6DSV16XTaskSensorReadMLC(p_obj);
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

        if (p_obj->pMLCConfig == NULL)
        {
          if (TX_SUCCESS != tx_timer_activate(&p_obj->mlc_timer))
          {
            res = SYS_UNDEFINED_ERROR_CODE;
          }
        }

        break;
      }

      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
            res = LSM6DSV16XTaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->acc_sensor_status.IsActive == true || p_obj->gyro_sensor_status.IsActive == true)
              {
                if (p_obj->pIRQConfig == NULL)
                {
                  if (TX_SUCCESS != tx_timer_activate(&p_obj->read_timer))
                  {
                    res = SYS_UNDEFINED_ERROR_CODE;
                  }
                }
                else
                {
                  LSM6DSV16XTaskConfigureIrqPin(p_obj, FALSE);
                }
              }
            }
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->mlc_sensor_status.IsActive == true)
              {
                if (p_obj->pMLCConfig == NULL)
                {
                  if (TX_SUCCESS != tx_timer_activate(&p_obj->mlc_timer))
                  {
                    res = SYS_UNDEFINED_ERROR_CODE;
                  }
                }
                else
                {
                  LSM6DSV16XTaskConfigureMLCPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = LSM6DSV16XTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = LSM6DSV16XTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = LSM6DSV16XTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = LSM6DSV16XTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = LSM6DSV16XTaskSensorDisable(p_obj, report);
            break;
          default:
          {
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV16X: unexpected report in Datalog: %i\r\n", report.messageID));
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

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV16X: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static inline sys_error_code_t LSM6DSV16XTaskPostReportToFront(LSM6DSV16XTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t LSM6DSV16XTaskPostReportToBack(LSM6DSV16XTask *_this, SMMessage *pReport)
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

uint8_t mlc_int1;
uint8_t mlc_int2;
uint8_t emb_func_fifo_en_a;
uint8_t emb_func_en_b;
lsm6dsv16x_pin_int_route_t int1_route;

static sys_error_code_t LSM6DSV16XTaskSensorInit(LSM6DSV16XTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  /* Setup acc and gyro */
  uint8_t reg0 = 0;
  lsm6dsv16x_data_rate_t lsm6dsv16x_xl_data_rate = LSM6DSV16X_ODR_OFF;
  lsm6dsv16x_fifo_xl_batch_t lsm6dsv16x_fifo_xl_batch = LSM6DSV16X_XL_NOT_BATCHED;
  lsm6dsv16x_data_rate_t lsm6dsv16x_gy_data_rate = LSM6DSV16X_ODR_OFF;
  lsm6dsv16x_fifo_gy_batch_t lsm6dsv16x_fifo_gy_batch = LSM6DSV16X_GY_NOT_BATCHED;
  int32_t ret_val = 0;

  lsm6dsv16x_reset_t rst;

  /* Restore default configuration */
  ret_val = lsm6dsv16x_reset_set(p_sensor_drv, LSM6DSV16X_RESTORE_CTRL_REGS);
  do
  {
    lsm6dsv16x_reset_get(p_sensor_drv, &rst);
  } while (rst != LSM6DSV16X_READY);

  /* Enable Block Data Update */
  ret_val = lsm6dsv16x_block_data_update_set(p_sensor_drv, PROPERTY_ENABLE);

  ret_val = lsm6dsv16x_device_id_get(p_sensor_drv, (uint8_t *) &reg0);
  if (!ret_val)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, reg0);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV16X: sensor - I am 0x%x.\r\n", reg0));

  /* AXL FS */
  if (_this->acc_sensor_status.FS < 3.0f)
  {
    lsm6dsv16x_xl_full_scale_set(p_sensor_drv, LSM6DSV16X_2g);
  }
  else if (_this->acc_sensor_status.FS < 5.0f)
  {
    lsm6dsv16x_xl_full_scale_set(p_sensor_drv, LSM6DSV16X_4g);
  }
  else if (_this->acc_sensor_status.FS < 9.0f)
  {
    lsm6dsv16x_xl_full_scale_set(p_sensor_drv, LSM6DSV16X_8g);
  }
  else
    lsm6dsv16x_xl_full_scale_set(p_sensor_drv, LSM6DSV16X_16g);

  /* GYRO FS */
  if (_this->gyro_sensor_status.FS < 126.0f)
  {
    lsm6dsv16x_gy_full_scale_set(p_sensor_drv, LSM6DSV16X_125dps);
  }
  else if (_this->gyro_sensor_status.FS < 251.0f)
  {
    lsm6dsv16x_gy_full_scale_set(p_sensor_drv, LSM6DSV16X_250dps);
  }
  else if (_this->gyro_sensor_status.FS < 501.0f)
  {
    lsm6dsv16x_gy_full_scale_set(p_sensor_drv, LSM6DSV16X_500dps);
  }
  else if (_this->gyro_sensor_status.FS < 1001.0f)
  {
    lsm6dsv16x_gy_full_scale_set(p_sensor_drv, LSM6DSV16X_1000dps);
  }
  else if (_this->gyro_sensor_status.FS < 2001.0f)
  {
    lsm6dsv16x_gy_full_scale_set(p_sensor_drv, LSM6DSV16X_2000dps);
  }
  else
    lsm6dsv16x_gy_full_scale_set(p_sensor_drv, LSM6DSV16X_4000dps);

  if (_this->acc_sensor_status.ODR < 8.0f)
  {
    lsm6dsv16x_xl_data_rate = LSM6DSV16X_ODR_AT_7Hz5;
    lsm6dsv16x_fifo_xl_batch = LSM6DSV16X_XL_BATCHED_AT_7Hz5;
  }
  else if (_this->acc_sensor_status.ODR < 16.0f)
  {
    lsm6dsv16x_xl_data_rate = LSM6DSV16X_ODR_AT_15Hz;
    lsm6dsv16x_fifo_xl_batch = LSM6DSV16X_XL_BATCHED_AT_15Hz;
  }
  else if (_this->acc_sensor_status.ODR < 31.0f)
  {
    lsm6dsv16x_xl_data_rate = LSM6DSV16X_ODR_AT_30Hz;
    lsm6dsv16x_fifo_xl_batch = LSM6DSV16X_XL_BATCHED_AT_30Hz;
  }
  else if (_this->acc_sensor_status.ODR < 61.0f)
  {
    lsm6dsv16x_xl_data_rate = LSM6DSV16X_ODR_AT_60Hz;
    lsm6dsv16x_fifo_xl_batch = LSM6DSV16X_XL_BATCHED_AT_60Hz;
  }
  else if (_this->acc_sensor_status.ODR < 121.0f)
  {
    lsm6dsv16x_xl_data_rate = LSM6DSV16X_ODR_AT_120Hz;
    lsm6dsv16x_fifo_xl_batch = LSM6DSV16X_XL_BATCHED_AT_120Hz;
  }
  else if (_this->acc_sensor_status.ODR < 241.0f)
  {
    lsm6dsv16x_xl_data_rate = LSM6DSV16X_ODR_AT_240Hz;
    lsm6dsv16x_fifo_xl_batch = LSM6DSV16X_XL_BATCHED_AT_240Hz;
  }
  else if (_this->acc_sensor_status.ODR < 481.0f)
  {
    lsm6dsv16x_xl_data_rate = LSM6DSV16X_ODR_AT_480Hz;
    lsm6dsv16x_fifo_xl_batch = LSM6DSV16X_XL_BATCHED_AT_480Hz;
  }
  else if (_this->acc_sensor_status.ODR < 961.0f)
  {
    lsm6dsv16x_xl_data_rate = LSM6DSV16X_ODR_AT_960Hz;
    lsm6dsv16x_fifo_xl_batch = LSM6DSV16X_XL_BATCHED_AT_960Hz;
  }
  else if (_this->acc_sensor_status.ODR < 1921.0f)
  {
    lsm6dsv16x_xl_data_rate = LSM6DSV16X_ODR_AT_1920Hz;
    lsm6dsv16x_fifo_xl_batch = LSM6DSV16X_XL_BATCHED_AT_1920Hz;
  }
  else if (_this->acc_sensor_status.ODR < 3841.0f)
  {
    lsm6dsv16x_xl_data_rate = LSM6DSV16X_ODR_AT_3840Hz;
    lsm6dsv16x_fifo_xl_batch = LSM6DSV16X_XL_BATCHED_AT_3840Hz;
  }
  else
  {
    lsm6dsv16x_xl_data_rate = LSM6DSV16X_ODR_AT_7680Hz;
    lsm6dsv16x_fifo_xl_batch = LSM6DSV16X_XL_BATCHED_AT_7680Hz;
  }

  if (_this->gyro_sensor_status.ODR < 8.0f)
  {
    lsm6dsv16x_gy_data_rate = LSM6DSV16X_ODR_AT_7Hz5;
    lsm6dsv16x_fifo_gy_batch = LSM6DSV16X_GY_BATCHED_AT_7Hz5;
  }
  else if (_this->gyro_sensor_status.ODR < 16.0f)
  {
    lsm6dsv16x_gy_data_rate = LSM6DSV16X_ODR_AT_15Hz;
    lsm6dsv16x_fifo_gy_batch = LSM6DSV16X_GY_BATCHED_AT_15Hz;
  }
  else if (_this->gyro_sensor_status.ODR < 31.0f)
  {
    lsm6dsv16x_gy_data_rate = LSM6DSV16X_ODR_AT_30Hz;
    lsm6dsv16x_fifo_gy_batch = LSM6DSV16X_GY_BATCHED_AT_30Hz;
  }
  else if (_this->gyro_sensor_status.ODR < 61.0f)
  {
    lsm6dsv16x_gy_data_rate = LSM6DSV16X_ODR_AT_60Hz;
    lsm6dsv16x_fifo_gy_batch = LSM6DSV16X_GY_BATCHED_AT_60Hz;
  }
  else if (_this->gyro_sensor_status.ODR < 121.0f)
  {
    lsm6dsv16x_gy_data_rate = LSM6DSV16X_ODR_AT_120Hz;
    lsm6dsv16x_fifo_gy_batch = LSM6DSV16X_GY_BATCHED_AT_120Hz;
  }
  else if (_this->gyro_sensor_status.ODR < 241.0f)
  {
    lsm6dsv16x_gy_data_rate = LSM6DSV16X_ODR_AT_240Hz;
    lsm6dsv16x_fifo_gy_batch = LSM6DSV16X_GY_BATCHED_AT_240Hz;
  }
  else if (_this->gyro_sensor_status.ODR < 481.0f)
  {
    lsm6dsv16x_gy_data_rate = LSM6DSV16X_ODR_AT_480Hz;
    lsm6dsv16x_fifo_gy_batch = LSM6DSV16X_GY_BATCHED_AT_480Hz;
  }
  else if (_this->gyro_sensor_status.ODR < 961.0f)
  {
    lsm6dsv16x_gy_data_rate = LSM6DSV16X_ODR_AT_960Hz;
    lsm6dsv16x_fifo_gy_batch = LSM6DSV16X_GY_BATCHED_AT_960Hz;
  }
  else if (_this->gyro_sensor_status.ODR < 1921.0f)
  {
    lsm6dsv16x_gy_data_rate = LSM6DSV16X_ODR_AT_1920Hz;
    lsm6dsv16x_fifo_gy_batch = LSM6DSV16X_GY_BATCHED_AT_1920Hz;
  }
  else if (_this->gyro_sensor_status.ODR < 3841.0f)
  {
    lsm6dsv16x_gy_data_rate = LSM6DSV16X_ODR_AT_3840Hz;
    lsm6dsv16x_fifo_gy_batch = LSM6DSV16X_GY_BATCHED_AT_3840Hz;
  }
  else
  {
    lsm6dsv16x_gy_data_rate = LSM6DSV16X_ODR_AT_7680Hz;
    lsm6dsv16x_fifo_gy_batch = LSM6DSV16X_GY_BATCHED_AT_7680Hz;
  }

  if (_this->acc_sensor_status.IsActive)
  {
    lsm6dsv16x_xl_data_rate_set(p_sensor_drv, lsm6dsv16x_xl_data_rate);
    lsm6dsv16x_fifo_xl_batch_set(p_sensor_drv, lsm6dsv16x_fifo_xl_batch);
  }
  else
  {
    lsm6dsv16x_xl_data_rate_set(p_sensor_drv, LSM6DSV16X_ODR_OFF);
    lsm6dsv16x_fifo_xl_batch_set(p_sensor_drv, LSM6DSV16X_XL_NOT_BATCHED);
    _this->acc_sensor_status.IsActive = false;
  }

  if (_this->gyro_sensor_status.IsActive)
  {
    lsm6dsv16x_gy_data_rate_set(p_sensor_drv, lsm6dsv16x_gy_data_rate);
    lsm6dsv16x_fifo_gy_batch_set(p_sensor_drv, lsm6dsv16x_fifo_gy_batch);
  }
  else
  {
    lsm6dsv16x_gy_data_rate_set(p_sensor_drv, LSM6DSV16X_ODR_OFF);
    lsm6dsv16x_fifo_gy_batch_set(p_sensor_drv, LSM6DSV16X_GY_NOT_BATCHED);
    _this->gyro_sensor_status.IsActive = false;
  }

#if LSM6DSV16X_FIFO_ENABLED
  uint16_t lsm6dsv16x_wtm_level = 0;
  uint16_t lsm6dsv16x_wtm_level_acc;
  uint16_t lsm6dsv16x_wtm_level_gyro;

  if (_this->samples_per_it == 0)
  {
    /* Calculation of watermark and samples per int*/
    lsm6dsv16x_wtm_level_acc = ((uint16_t) _this->acc_sensor_status.ODR * (uint16_t) LSM6DSV16X_MAX_DRDY_PERIOD);
    lsm6dsv16x_wtm_level_gyro = ((uint16_t) _this->gyro_sensor_status.ODR * (uint16_t) LSM6DSV16X_MAX_DRDY_PERIOD);

    if (_this->acc_sensor_status.IsActive && _this->gyro_sensor_status.IsActive) /* Both subSensor is active */
    {
      if (lsm6dsv16x_wtm_level_acc > lsm6dsv16x_wtm_level_gyro)
      {
        lsm6dsv16x_wtm_level = lsm6dsv16x_wtm_level_acc;
      }
      else
      {
        lsm6dsv16x_wtm_level = lsm6dsv16x_wtm_level_gyro;
      }
    }
    else /* Only one subSensor is active */
    {
      if (_this->acc_sensor_status.IsActive)
      {
        lsm6dsv16x_wtm_level = lsm6dsv16x_wtm_level_acc;
      }
      else
      {
        lsm6dsv16x_wtm_level = lsm6dsv16x_wtm_level_gyro;
      }
    }

    if (lsm6dsv16x_wtm_level > LSM6DSV16X_MAX_WTM_LEVEL)
    {
      lsm6dsv16x_wtm_level = LSM6DSV16X_MAX_WTM_LEVEL;
    }
    else if (lsm6dsv16x_wtm_level < LSM6DSV16X_MIN_WTM_LEVEL)
    {
      lsm6dsv16x_wtm_level = LSM6DSV16X_MIN_WTM_LEVEL;
    }
    _this->samples_per_it = lsm6dsv16x_wtm_level;
  }

  /* Setup wtm for FIFO */
  lsm6dsv16x_fifo_watermark_set(p_sensor_drv, _this->samples_per_it);

  if (_this->pIRQConfig != NULL)
  {
    int1_route.fifo_th = PROPERTY_ENABLE;
  }
  else
  {
    int1_route.fifo_th = PROPERTY_DISABLE;
  }

  lsm6dsv16x_pin_int1_route_set(p_sensor_drv, &int1_route);

  lsm6dsv16x_fifo_mode_set(p_sensor_drv, LSM6DSV16X_STREAM_MODE);

#else

  uint8_t buff[6];
  lsm6dsv16x_read_reg(p_sensor_drv, LSM6DSV16X_OUTX_L_A, &buff[0], 6);
  lsm6dsv16x_read_reg(p_sensor_drv, LSM6DSV16X_OUTX_L_G, &buff[0], 6);

  _this->samples_per_it = 1;
  if (_this->pIRQConfig != NULL)
  {
    if (_this->acc_sensor_status.IsActive && _this->gyro_sensor_status.IsActive) /* Both subSensor is active */
    {
      int1_route.drdy_xl = PROPERTY_ENABLE;
      int1_route.drdy_g = PROPERTY_ENABLE;
    }
    else if (_this->acc_sensor_status.IsActive)
    {
      int1_route.drdy_xl = PROPERTY_ENABLE;
    }
    else
    {
      int1_route.drdy_g = PROPERTY_ENABLE;
    }
  }
  else
  {
    int1_route.drdy_xl = PROPERTY_DISABLE;
    int1_route.drdy_g = PROPERTY_DISABLE;
  }
  lsm6dsv16x_pin_int1_route_set(p_sensor_drv, &int1_route);

#endif /* LSM6DSV16X_FIFO_ENABLED */

  /* Check configuration from UCF */
  lsm6dsv16x_mem_bank_set(p_sensor_drv, LSM6DSV16X_EMBED_FUNC_MEM_BANK);
  lsm6dsv16x_read_reg(p_sensor_drv, LSM6DSV16X_MLC_INT1, &mlc_int1, 1);
  lsm6dsv16x_read_reg(p_sensor_drv, LSM6DSV16X_MLC_INT2, &mlc_int2, 1);

  /* MLC must be handled in polling due to HW limitations: disable MLC INT */
  mlc_int1 = 0;
  mlc_int2 = 0;
  lsm6dsv16x_write_reg(p_sensor_drv, LSM6DSV16X_MLC_INT1, &mlc_int1, 1);
  lsm6dsv16x_write_reg(p_sensor_drv, LSM6DSV16X_MLC_INT2, &mlc_int2, 1);
  lsm6dsv16x_mem_bank_set(p_sensor_drv, LSM6DSV16X_MAIN_MEM_BANK);

  if (_this->mlc_enable == false)
  {
    lsm6dsv16x_mlc_set(p_sensor_drv, LSM6DSV16X_MLC_OFF);
  }
#if LSM6DSV16X_FIFO_ENABLED
  uint8_t reg[2];
  /* Check FIFO_WTM_IA and fifo level. We do not use PID in order to avoid reading one register twice */
  lsm6dsv16x_read_reg(p_sensor_drv, LSM6DSV16X_FIFO_STATUS1, reg, 2);

  _this->fifo_level = ((reg[1] & 0x03) << 8) + reg[0];

  if (_this->fifo_level >= _this->samples_per_it)
  {
    lsm6dsv16x_read_reg(p_sensor_drv, LSM6DSV16X_FIFO_DATA_OUT_TAG, _this->p_fast_sensor_data_buff,
                        _this->samples_per_it * 7);
  }
#endif

  _this->lsm6dsv16x_task_cfg_timer_period_ms = (uint16_t)(_this->acc_sensor_status.ODR < _this->gyro_sensor_status.ODR ? _this->acc_sensor_status.ODR : _this->gyro_sensor_status.ODR);
#if LSM6DSV16X_FIFO_ENABLED
  _this->lsm6dsv16x_task_cfg_timer_period_ms = (uint16_t)((1000.0f / _this->lsm6dsv16x_task_cfg_timer_period_ms) * (((float)(_this->samples_per_it)) / 2.0f));
#else
  _this->lsm6dsv16x_task_cfg_timer_period_ms = (uint16_t)(1000.0f / _this->lsm6dsv16x_task_cfg_timer_period_ms);
#endif

  return res;
}

static sys_error_code_t LSM6DSV16XTaskSensorReadData(LSM6DSV16XTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t samples_per_it = _this->samples_per_it;

#if LSM6DSV16X_FIFO_ENABLED
  uint8_t reg[2];
  uint16_t i;

  /* Check FIFO_WTM_IA and fifo level. We do not use PID in order to avoid reading one register twice */
  lsm6dsv16x_read_reg(p_sensor_drv, LSM6DSV16X_FIFO_STATUS1, reg, 2);

  _this->fifo_level = ((reg[1] & 0x01) << 8) + reg[0];

  if (((reg[1]) & 0x80) && (_this->fifo_level >= samples_per_it))
  {
    lsm6dsv16x_read_reg(p_sensor_drv, LSM6DSV16X_FIFO_DATA_OUT_TAG, _this->p_fast_sensor_data_buff, samples_per_it * 7);

#if (HSD_USE_DUMMY_DATA == 1)
    int16_t *p16 = (int16_t *)(_this->p_fast_sensor_data_buff);

    for (i = 0; i < samples_per_it; i++)
    {
      p16 = (int16_t *)(&_this->p_fast_sensor_data_buff[i * 7] + 1);
      if ((_this->p_fast_sensor_data_buff[i * 7] >> 3) == LSM6DSV16X_TAG_ACC)
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
    if ((_this->acc_sensor_status.IsActive) && (_this->gyro_sensor_status.IsActive))
    {
      /* Read both ACC and GYRO */

      uint32_t odr_acc = (uint32_t) _this->acc_sensor_status.ODR;
      uint32_t odr_gyro = (uint32_t) _this->gyro_sensor_status.ODR;

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
        if (((*p_tag) >> 3) == LSM6DSV16X_TAG_ACC)
        {
          p16_src = (int16_t *)(p_tag + 1);
          *p_acc++ = *p16_src++;
          *p_acc++ = *p16_src++;
          *p_acc++ = *p16_src++;
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
      for (i = 0; i < samples_per_it; i++)
      {
        p16_src = (int16_t *) & ((uint8_t *)(p16_src))[1];
        *p16_dest++ = *p16_src++;
        *p16_dest++ = *p16_src++;
        *p16_dest++ = *p16_src++;
      }
      if (_this->acc_sensor_status.IsActive)
      {
        _this->acc_samples_count = samples_per_it;
      }
      else
      {
        _this->gyro_samples_count = samples_per_it;
      }
    }

  }
  else
  {
    _this->fifo_level = 0;
    res = SYS_BASE_ERROR_CODE;
  }
#else
  if ((_this->acc_sensor_status.IsActive) && (_this->gyro_sensor_status.IsActive))
  {
    lsm6dsv16x_status_reg_t val;
    uint32_t odr_acc = (uint32_t) _this->acc_sensor_status.ODR;
    uint32_t odr_gyro = (uint32_t) _this->gyro_sensor_status.ODR;

    if (odr_acc != odr_gyro)
    {
      /* Need to read which sensor generated the INT in case of different ODR; */
      lsm6dsv16x_read_reg(p_sensor_drv, LSM6DSV16X_STATUS_REG, (uint8_t *) &val, 1);
    }
    else
    {
      /* Manually set the variable to read both sensors (avoid to loose time with a read) */
      val.xlda = 1U;
      val.gda = 1U;
    }

    if (val.xlda == 1U)
    {
      lsm6dsv16x_read_reg(p_sensor_drv, LSM6DSV16X_OUTX_L_A, _this->p_acc_sample, 6);
#if (HSD_USE_DUMMY_DATA == 1)
      int16_t *p16 = (int16_t *)(_this->p_acc_sample);
      *p16++ = dummyDataCounter_acc++;
      *p16++ = dummyDataCounter_acc++;
      *p16++ = dummyDataCounter_acc++;
#endif
      _this->acc_samples_count = 1U;
      _this->acc_drdy = 1;
    }
    if (val.gda == 1U)
    {
      lsm6dsv16x_read_reg(p_sensor_drv, LSM6DSV16X_OUTX_L_G, _this->p_gyro_sample, 6);
#if (HSD_USE_DUMMY_DATA == 1)
      int16_t *p16 = (int16_t *)(_this->p_gyro_sample);
      *p16++ = dummyDataCounter_gyro++;
      *p16++ = dummyDataCounter_gyro++;
      *p16++ = dummyDataCounter_gyro++;
#endif
      _this->gyro_samples_count = 1U;
      _this->gyro_drdy = 1;
    }
  }
  else if (_this->acc_sensor_status.IsActive)
  {
    lsm6dsv16x_read_reg(p_sensor_drv, LSM6DSV16X_OUTX_L_A, _this->p_acc_sample, samples_per_it * 6);
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
  else if (_this->gyro_sensor_status.IsActive)
  {
    lsm6dsv16x_read_reg(p_sensor_drv, LSM6DSV16X_OUTX_L_G, _this->p_acc_sample, samples_per_it * 6);
#if (HSD_USE_DUMMY_DATA == 1)
    uint16_t i = 0;
    int16_t *p16 = (int16_t *)(_this->p_gyro_sample);
    for (i = 0; i < samples_per_it * 3; i++)
    {
      *p16++ = dummyDataCounter_gyro++;
    }
#endif
    _this->gyro_samples_count = 1U;
    _this->gyro_drdy = 1;
  }
  else
  {
  }
  _this->fifo_level = 1;
#endif /* LSM6DSV16X_FIFO_ENABLED */

  return res;
}

static sys_error_code_t LSM6DSV16XTaskSensorReadMLC(LSM6DSV16XTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_BASE_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  lsm6dsv16x_mlc_status_t mlc_status;
  uint8_t mlc_output[4];

  if (_this->mlc_enable)
  {
    lsm6dsv16x_mem_bank_set(p_sensor_drv, LSM6DSV16X_EMBED_FUNC_MEM_BANK);
    lsm6dsv16x_read_reg(p_sensor_drv, LSM6DSV16X_MLC_STATUS, (uint8_t *)(&mlc_status), 1);
    _this->p_mlc_sensor_data_buff[4] = (mlc_status.is_mlc1) | (mlc_status.is_mlc2 << 1) | (mlc_status.is_mlc3 << 2) | (mlc_status.is_mlc4 << 3);

    lsm6dsv16x_read_reg(p_sensor_drv, LSM6DSV16X_MLC1_SRC, (uint8_t *)(&mlc_output[0]), 4);
    if (memcmp(&_this->p_mlc_sensor_data_buff[0], &mlc_output[0], 4) != 0)
    {
      memcpy(&_this->p_mlc_sensor_data_buff[0], &mlc_output[0], 4);
      res = SYS_NO_ERROR_CODE;
    }
    else
    {
      res = SYS_SENSOR_TASK_NO_DRDY_ERROR_CODE;
    }
    lsm6dsv16x_mem_bank_set(p_sensor_drv, LSM6DSV16X_MAIN_MEM_BANK);
  }

  return res;
}

static sys_error_code_t LSM6DSV16XTaskSensorRegister(LSM6DSV16XTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *acc_if = (ISensor_t *) LSM6DSV16XTaskGetAccSensorIF(_this);
  ISensor_t *gyro_if = (ISensor_t *) LSM6DSV16XTaskGetGyroSensorIF(_this);
  ISensor_t *mlc_if = (ISensor_t *) LSM6DSV16XTaskGetMlcSensorIF(_this);

  _this->acc_id = SMAddSensor(acc_if);
  _this->gyro_id = SMAddSensor(gyro_if);
  _this->mlc_id = SMAddSensor(mlc_if);

  return res;
}

static sys_error_code_t LSM6DSV16XTaskSensorInitTaskParams(LSM6DSV16XTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* ACCELEROMETER STATUS */
  _this->acc_sensor_status.IsActive = TRUE;
  _this->acc_sensor_status.FS = 16.0f;
  _this->acc_sensor_status.Sensitivity = 0.0000305f * _this->acc_sensor_status.FS;
  _this->acc_sensor_status.ODR = 7680.0f;
  _this->acc_sensor_status.MeasuredODR = 0.0f;
#if LSM6DSV16X_FIFO_ENABLED
  EMD_Init(&_this->data_acc, _this->p_fast_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);
#else
  EMD_Init(&_this->data_acc, _this->p_acc_sample, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);
#endif

  /* GYROSCOPE STATUS */
  _this->gyro_sensor_status.IsActive = TRUE;
  _this->gyro_sensor_status.FS = 4000.0f;
  _this->gyro_sensor_status.ODR = 7680.0f;
  _this->gyro_sensor_status.Sensitivity = 0.035f * _this->gyro_sensor_status.FS;
  _this->gyro_sensor_status.MeasuredODR = 0.0f;
#if LSM6DSV16X_FIFO_ENABLED
  EMD_Init(&_this->data_gyro, _this->p_slow_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);
#else
  EMD_Init(&_this->data_gyro, _this->p_gyro_sample, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);
#endif

  /* MLC STATUS */
  _this->mlc_sensor_status.IsActive = FALSE;
  _this->mlc_sensor_status.FS = 1.0f;
  _this->mlc_sensor_status.Sensitivity = 1.0f;
  _this->mlc_sensor_status.ODR = 1.0f;
  _this->mlc_sensor_status.MeasuredODR = 0.0f;
  EMD_Init(&_this->data, _this->p_mlc_sensor_data_buff, E_EM_UINT8, E_EM_MODE_INTERLEAVED, 2, 1, 5);

  return res;
}

static sys_error_code_t LSM6DSV16XTaskSensorSetODR(LSM6DSV16XTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float ODR = (float) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    if (ODR < 1.0f)
    {
      lsm6dsv16x_xl_data_rate_set(p_sensor_drv, LSM6DSV16X_ODR_OFF);
      /* Do not update the model in case of ODR = 0 */
      ODR = _this->acc_sensor_status.ODR;
    }
    else
    {
      /* Changing ODR must disable MLC sensor: MLC can work properly only when setup from UCF */
      _this->mlc_enable = FALSE;
      _this->mlc_sensor_status.IsActive = FALSE;

      if (ODR < 8.0f)
      {
        ODR = 7.5f;
      }
      else if (ODR < 16.0f)
      {
        ODR = 15.0f;
      }
      else if (ODR < 31.0f)
      {
        ODR = 30.0f;
      }
      else if (ODR < 61.0f)
      {
        ODR = 60.0f;
      }
      else if (ODR < 121.0f)
      {
        ODR = 120.0f;
      }
      else if (ODR < 241.0f)
      {
        ODR = 240.0f;
      }
      else if (ODR < 481.0f)
      {
        ODR = 480.0f;
      }
      else if (ODR < 961.0f)
      {
        ODR = 960.0f;
      }
      else if (ODR < 1921.0f)
      {
        ODR = 1920.0f;
      }
      else if (ODR < 3841.0f)
      {
        ODR = 3840.0f;
      }
      else
      {
        ODR = 7680;
      }
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->acc_sensor_status.ODR = ODR;
      _this->acc_sensor_status.MeasuredODR = 0.0f;
    }
  }
  else if (id == _this->gyro_id)
  {
    if (ODR < 1.0f)
    {
      lsm6dsv16x_gy_data_rate_set(p_sensor_drv, LSM6DSV16X_ODR_OFF);
      /* Do not update the model in case of ODR = 0 */
      ODR = _this->gyro_sensor_status.ODR;
    }
    else
    {
      /* Changing ODR must disable MLC sensor: MLC can work properly only when setup from UCF */
      _this->mlc_enable = FALSE;
      _this->mlc_sensor_status.IsActive = FALSE;

      if (ODR < 8.0f)
      {
        ODR = 7.5f;
      }
      else if (ODR < 16.0f)
      {
        ODR = 15.0f;
      }
      else if (ODR < 31.0f)
      {
        ODR = 30.0f;
      }
      else if (ODR < 61.0f)
      {
        ODR = 60.0f;
      }
      else if (ODR < 121.0f)
      {
        ODR = 120.0f;
      }
      else if (ODR < 241.0f)
      {
        ODR = 240.0f;
      }
      else if (ODR < 481.0f)
      {
        ODR = 480.0f;
      }
      else if (ODR < 961.0f)
      {
        ODR = 960.0f;
      }
      else if (ODR < 1921.0f)
      {
        ODR = 1920.0f;
      }
      else if (ODR < 3841.0f)
      {
        ODR = 3840.0f;
      }
      else
      {
        ODR = 7680;
      }
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->gyro_sensor_status.ODR = ODR;
      _this->gyro_sensor_status.MeasuredODR = 0.0f;
    }
  }
  else if (id == _this->mlc_id)
  {
    res = SYS_TASK_INVALID_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("ISM330DHCX: warning - MLC SetODR() not supported.\r\n"));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  /* when ODR changes the samples_per_it must be recalculated */
  _this->samples_per_it = 0;

  return res;
}

static sys_error_code_t LSM6DSV16XTaskSensorSetFS(LSM6DSV16XTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float FS = (float) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  /* Changing FS must disable MLC sensor: MLC can work properly only when setup from UCF */
  _this->mlc_enable = FALSE;
  _this->mlc_sensor_status.IsActive = FALSE;

  if (id == _this->acc_id)
  {
    if (FS < 3.0f)
    {
      lsm6dsv16x_xl_full_scale_set(p_sensor_drv, LSM6DSV16X_2g);
      FS = 2.0f;
    }
    else if (FS < 5.0f)
    {
      lsm6dsv16x_xl_full_scale_set(p_sensor_drv, LSM6DSV16X_4g);
      FS = 4.0f;
    }
    else if (FS < 9.0f)
    {
      lsm6dsv16x_xl_full_scale_set(p_sensor_drv, LSM6DSV16X_8g);
      FS = 8.0f;
    }
    else
    {
      lsm6dsv16x_xl_full_scale_set(p_sensor_drv, LSM6DSV16X_16g);
      FS = 16.0f;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->acc_sensor_status.FS = FS;
      _this->acc_sensor_status.Sensitivity = 0.0000305f * _this->acc_sensor_status.FS;
    }
  }
  else if (id == _this->gyro_id)
  {
    if (FS < 126.0f)
    {
      lsm6dsv16x_gy_full_scale_set(p_sensor_drv, LSM6DSV16X_125dps);
      FS = 125.0f;
    }
    else if (FS < 251.0f)
    {
      lsm6dsv16x_gy_full_scale_set(p_sensor_drv, LSM6DSV16X_250dps);
      FS = 250.0f;
    }
    else if (FS < 501.0f)
    {
      lsm6dsv16x_gy_full_scale_set(p_sensor_drv, LSM6DSV16X_500dps);
      FS = 500.0f;
    }
    else if (FS < 1001.0f)
    {
      lsm6dsv16x_gy_full_scale_set(p_sensor_drv, LSM6DSV16X_1000dps);
      FS = 1000.0f;
    }
    else if (FS < 2001.0f)
    {
      lsm6dsv16x_gy_full_scale_set(p_sensor_drv, LSM6DSV16X_2000dps);
      FS = 2000.0f;
    }
    else
    {
      lsm6dsv16x_gy_full_scale_set(p_sensor_drv, LSM6DSV16X_4000dps);
      FS = 4000.0f;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->gyro_sensor_status.FS = FS;
      _this->gyro_sensor_status.Sensitivity = 0.035f * _this->gyro_sensor_status.FS;
    }
  }
  else if (id == _this->mlc_id)
  {
    res = SYS_TASK_INVALID_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("ISM330DHCX: warning - MLC SetFS() not supported.\r\n"));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t LSM6DSV16XTaskSensorSetFifoWM(LSM6DSV16XTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (report.sensorMessage.nSensorId == _this->mlc_id)
  {
    res = SYS_TASK_INVALID_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

    SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("LSM6DSV16X: warning - MLC SetFifoWM() not supported.\r\n"));
  }

#if LSM6DSV16X_FIFO_ENABLED

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t lsm6dsv16x_wtm_level = report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if ((id == _this->acc_id) || (id == _this->gyro_id))
  {
    /* acc and gyro share the FIFO, so size should be increased w.r.t. previous setup */
    lsm6dsv16x_wtm_level = lsm6dsv16x_wtm_level + _this->samples_per_it;

    if (lsm6dsv16x_wtm_level > LSM6DSV16X_MAX_WTM_LEVEL)
    {
      lsm6dsv16x_wtm_level = LSM6DSV16X_MAX_WTM_LEVEL;
    }
    _this->samples_per_it = lsm6dsv16x_wtm_level;

    /* Setup wtm for FIFO */
    lsm6dsv16x_fifo_watermark_set(p_sensor_drv, _this->samples_per_it);

    lsm6dsv16x_fifo_mode_set(p_sensor_drv, LSM6DSV16X_STREAM_MODE);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }
#endif

  return res;
}

static sys_error_code_t LSM6DSV16XTaskSensorEnable(LSM6DSV16XTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    _this->acc_sensor_status.IsActive = TRUE;

    /* Changing sensor configuration must disable MLC sensor: MLC can work properly only when setup from UCF */
    _this->mlc_enable = FALSE;
    _this->mlc_sensor_status.IsActive = FALSE;
  }
  else if (id == _this->gyro_id)
  {
    _this->gyro_sensor_status.IsActive = TRUE;

    /* Changing sensor configuration must disable MLC sensor: MLC can work properly only when setup from UCF */
    _this->mlc_enable = FALSE;
    _this->mlc_sensor_status.IsActive = FALSE;
  }
  else if (id == _this->mlc_id)
  {
    _this->mlc_sensor_status.IsActive = TRUE;
    _this->mlc_enable = TRUE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t LSM6DSV16XTaskSensorDisable(LSM6DSV16XTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->acc_id)
  {
    _this->acc_sensor_status.IsActive = FALSE;
    lsm6dsv16x_xl_data_rate_set(p_sensor_drv, LSM6DSV16X_ODR_OFF);

    /* Changing sensor configuration must disable MLC sensor: MLC can work properly only when setup from UCF */
    _this->mlc_enable = FALSE;
    _this->mlc_sensor_status.IsActive = FALSE;
  }
  else if (id == _this->gyro_id)
  {
    _this->gyro_sensor_status.IsActive = FALSE;
    lsm6dsv16x_gy_data_rate_set(p_sensor_drv, LSM6DSV16X_ODR_OFF);

    /* Changing sensor configuration must disable MLC sensor: MLC can work properly only when setup from UCF */
    _this->mlc_enable = FALSE;
    _this->mlc_sensor_status.IsActive = FALSE;
  }
  else if (id == _this->mlc_id)
  {
    _this->mlc_sensor_status.IsActive = FALSE;
    _this->mlc_enable = FALSE;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t LSM6DSV16XTaskSensorIsActive(const LSM6DSV16XTask *_this)
{
  assert_param(_this != NULL);
  return (_this->acc_sensor_status.IsActive || _this->gyro_sensor_status.IsActive);
}

static sys_error_code_t LSM6DSV16XTaskEnterLowPowerMode(const LSM6DSV16XTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  lsm6dsv16x_data_rate_t lsm6dsv16x_xl_data_rate = LSM6DSV16X_ODR_OFF;
  lsm6dsv16x_fifo_xl_batch_t lsm6dsv16x_fifo_xl_batch = LSM6DSV16X_XL_NOT_BATCHED;
  lsm6dsv16x_data_rate_t lsm6dsv16x_gy_data_rate = LSM6DSV16X_ODR_OFF;
  lsm6dsv16x_fifo_gy_batch_t lsm6dsv16x_fifo_gy_batch = LSM6DSV16X_GY_NOT_BATCHED;

  lsm6dsv16x_xl_data_rate_set(p_sensor_drv, lsm6dsv16x_xl_data_rate);
  lsm6dsv16x_fifo_xl_batch_set(p_sensor_drv, lsm6dsv16x_fifo_xl_batch);
  lsm6dsv16x_gy_data_rate_set(p_sensor_drv, lsm6dsv16x_gy_data_rate);
  lsm6dsv16x_fifo_gy_batch_set(p_sensor_drv, lsm6dsv16x_fifo_gy_batch);

  return res;
}

static sys_error_code_t LSM6DSV16XTaskConfigureIrqPin(const LSM6DSV16XTask *_this, boolean_t LowPower)
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

static sys_error_code_t LSM6DSV16XTaskConfigureMLCPin(const LSM6DSV16XTask *_this, boolean_t LowPower)
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

static inline LSM6DSV16XTask *LSM6DSV16XTaskGetOwnerFromISensorIF(ISensor_t *p_if)
{
  assert_param(p_if != NULL);
  LSM6DSV16XTask *p_if_owner = NULL;

  /* check if the virtual function has been called from the mlc fake sensor IF  */
  p_if_owner = (LSM6DSV16XTask *)((uint32_t) p_if - offsetof(LSM6DSV16XTask, mlc_sensor_if));
  if (!(p_if_owner->gyro_sensor_if.vptr == &sTheClass.gyro_sensor_if_vtbl)
      || !(p_if_owner->super.vptr == &sTheClass.vtbl))
  {
    /* then the virtual function has been called from the gyro IF  */
    p_if_owner = (LSM6DSV16XTask *)((uint32_t) p_if - offsetof(LSM6DSV16XTask, gyro_sensor_if));
  }
  if (!(p_if_owner->acc_sensor_if.vptr == &sTheClass.acc_sensor_if_vtbl) || !(p_if_owner->super.vptr == &sTheClass.vtbl))
  {
    /* then the virtual function has been called from the acc IF  */
    p_if_owner = (LSM6DSV16XTask *)((uint32_t) p_if - offsetof(LSM6DSV16XTask, acc_sensor_if));
  }

  return p_if_owner;
}

static inline LSM6DSV16XTask *LSM6DSV16XTaskGetOwnerFromISensorLLIF(ISensorLL_t *p_if)
{
  assert_param(p_if != NULL);
  LSM6DSV16XTask *p_if_owner = NULL;
  p_if_owner = (LSM6DSV16XTask *)((uint32_t) p_if - offsetof(LSM6DSV16XTask, sensor_ll_if));

  return p_if_owner;
}

static void LSM6DSV16XTaskTimerCallbackFunction(ULONG timer)
{
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  // if (sTaskObj.in_queue != NULL ) {//TODO: STF.Port - how to check if the queue has been initialized ??
  if (TX_SUCCESS != tx_queue_send(&sTaskObj.in_queue, &report, TX_NO_WAIT))
  {
    // unable to send the message. Signal the error
    sys_error_handler();
  }
  //}
}

static void LSM6DSV16XTaskMLCTimerCallbackFunction(ULONG timer)
{
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY_MLC;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  // if (sTaskObj.in_queue != NULL ) {//TODO: STF.Port - how to check if the queue has been initialized ??
  if (TX_SUCCESS != tx_queue_send(&sTaskObj.in_queue, &report, TX_NO_WAIT))
  {
    /* unable to send the report. Signal the error */
    sys_error_handler();
  }
  //}
}

/* CubeMX integration */
// ******************
/**
  * Interrupt callback
  */
void LSM6DSV16XTask_EXTI_Callback(uint16_t Pin)
{
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

//  if (sTaskObj.in_queue != NULL) { //TODO: STF.Port - how to check if the queue has been initialized ??
  if (TX_SUCCESS != tx_queue_send(&sTaskObj.in_queue, &report, TX_NO_WAIT))
  {
    /* unable to send the report. Signal the error */
    sys_error_handler();
  }
//  }
}

void INT2_DSV16X_EXTI_Callback(uint16_t Pin)
{
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY_MLC;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

//  if (sTaskObj.in_queue != NULL) { //TODO: STF.Port - how to check if the queue has been initialized ??
  if (TX_SUCCESS != tx_queue_send(&sTaskObj.in_queue, &report, TX_NO_WAIT))
  {
    /* unable to send the report. Signal the error */
    sys_error_handler();
  }
//  }
}

static sys_error_code_t LSM6DSV16X_ODR_Sync(LSM6DSV16XTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  float odr = 0.0f;
  lsm6dsv16x_data_rate_t lsm6dsv16x_odr_xl;
  if (lsm6dsv16x_xl_data_rate_get(p_sensor_drv, &lsm6dsv16x_odr_xl) == 0)
  {
    _this->acc_sensor_status.IsActive = TRUE;

    /* Update only the model */
    switch (lsm6dsv16x_odr_xl)
    {
      case LSM6DSV16X_ODR_OFF:
        _this->acc_sensor_status.IsActive = FALSE;
        /* Do not update the model in case of ODR = 0 */
        odr = _this->acc_sensor_status.ODR;
        break;
      case LSM6DSV16X_ODR_AT_7Hz5:
        odr = 7.5f;
        break;
      case LSM6DSV16X_ODR_AT_15Hz:
        odr = 15.0f;
        break;
      case LSM6DSV16X_ODR_AT_30Hz:
        odr = 30.0f;
        break;
      case LSM6DSV16X_ODR_AT_60Hz:
        odr = 60.0f;
        break;
      case LSM6DSV16X_ODR_AT_120Hz:
        odr = 120.0f;
        break;
      case LSM6DSV16X_ODR_AT_240Hz:
        odr = 240.0f;
        break;
      case LSM6DSV16X_ODR_AT_480Hz:
        odr = 480.0f;
        break;
      case LSM6DSV16X_ODR_AT_960Hz:
        odr = 960.0f;
        break;
      case LSM6DSV16X_ODR_AT_1920Hz:
        odr = 1920.0f;
        break;
      case LSM6DSV16X_ODR_AT_3840Hz:
        odr = 3840.0f;
        break;
      case LSM6DSV16X_ODR_AT_7680Hz:
        odr = 7680.0f;
        break;
      default:
        break;
    }
    _this->acc_sensor_status.ODR = odr;
    _this->acc_sensor_status.MeasuredODR = 0.0f;

  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }

  odr = 0;
  lsm6dsv16x_data_rate_t lsm6dsv16x_odr_g;
  if (lsm6dsv16x_gy_data_rate_get(p_sensor_drv, &lsm6dsv16x_odr_g) == 0)
  {
    _this->gyro_sensor_status.IsActive = TRUE;

    /* Update only the model */
    switch (lsm6dsv16x_odr_g)
    {
      case LSM6DSV16X_ODR_OFF:
        _this->gyro_sensor_status.IsActive = FALSE;
        /* Do not update the model in case of ODR = 0 */
        odr = _this->gyro_sensor_status.ODR;
        break;
      case LSM6DSV16X_ODR_AT_7Hz5:
        odr = 7.5f;
        break;
      case LSM6DSV16X_ODR_AT_15Hz:
        odr = 15.0f;
        break;
      case LSM6DSV16X_ODR_AT_30Hz:
        odr = 30.0f;
        break;
      case LSM6DSV16X_ODR_AT_60Hz:
        odr = 60.0f;
        break;
      case LSM6DSV16X_ODR_AT_120Hz:
        odr = 120.0f;
        break;
      case LSM6DSV16X_ODR_AT_240Hz:
        odr = 240.0f;
        break;
      case LSM6DSV16X_ODR_AT_480Hz:
        odr = 480.0f;
        break;
      case LSM6DSV16X_ODR_AT_960Hz:
        odr = 960.0f;
        break;
      case LSM6DSV16X_ODR_AT_1920Hz:
        odr = 1920.0f;
        break;
      case LSM6DSV16X_ODR_AT_3840Hz:
        odr = 3840.0f;
        break;
      case LSM6DSV16X_ODR_AT_7680Hz:
        odr = 7680.0f;
        break;
      default:
        break;
    }
    _this->gyro_sensor_status.ODR = odr;
    _this->gyro_sensor_status.MeasuredODR = 0.0f;
  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }
  _this->samples_per_it = 0;
  return res;
}

static sys_error_code_t LSM6DSV16X_FS_Sync(LSM6DSV16XTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  float fs = 2.0;
  lsm6dsv16x_xl_full_scale_t fs_xl;
  if (lsm6dsv16x_xl_full_scale_get(p_sensor_drv, &fs_xl) == 0)
  {
    switch (fs_xl)
    {
      case LSM6DSV16X_2g:
        fs = 2.0;
        break;
      case LSM6DSV16X_4g:
        fs = 4.0;
        break;
      case LSM6DSV16X_8g:
        fs = 8.0;
        break;
      case LSM6DSV16X_16g:
        fs = 16.0;
        break;
      default:
        break;
    }
    _this->acc_sensor_status.FS = fs;
    _this->acc_sensor_status.Sensitivity = 0.0000305f * _this->acc_sensor_status.FS;
  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }

  fs = 125;
  lsm6dsv16x_gy_full_scale_t fs_g;
  if (lsm6dsv16x_gy_full_scale_get(p_sensor_drv, &fs_g) == 0)
  {
    switch (fs_g)
    {
      case LSM6DSV16X_125dps:
        fs = 125;
        break;
      case LSM6DSV16X_250dps:
        fs = 250;
        break;
      case LSM6DSV16X_500dps:
        fs = 500;
        break;
      case LSM6DSV16X_1000dps:
        fs = 1000;
        break;
      case LSM6DSV16X_2000dps:
        fs = 2000;
        break;
      case LSM6DSV16X_4000dps:
        fs = 4000;
        break;
      default:
        break;
    }
    _this->gyro_sensor_status.FS = fs;
    _this->gyro_sensor_status.Sensitivity = 0.035f * _this->gyro_sensor_status.FS;
  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }
  return res;
}

