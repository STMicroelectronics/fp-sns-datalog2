/**
  ******************************************************************************
  * @file    LSM6DSV80XTask.c
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
#include "LSM6DSV80XTask.h"
#include "LSM6DSV80XTask_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "lsm6dsv80x_reg.h"
#include <string.h>
#include <stdlib.h>
#include "services/sysdebug.h"

/* Private includes ----------------------------------------------------------*/

#ifndef LSM6DSV80X_TASK_CFG_STACK_DEPTH
#define LSM6DSV80X_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*8)
#endif

#ifndef LSM6DSV80X_TASK_CFG_PRIORITY
#define LSM6DSV80X_TASK_CFG_PRIORITY                 (TX_MAX_PRIORITIES - 1)
#endif

#ifndef LSM6DSV80X_TASK_CFG_IN_QUEUE_LENGTH
#define LSM6DSV80X_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define LSM6DSV80X_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef LSM6DSV80X_TASK_CFG_TIMER_PERIOD_MS
#define LSM6DSV80X_TASK_CFG_TIMER_PERIOD_MS          1000
#endif
#ifndef LSM6DSV80X_TASK_CFG_MLC_TIMER_PERIOD_MS
#define LSM6DSV80X_TASK_CFG_MLC_TIMER_PERIOD_MS      200
#endif

#define LSM6DSV80X_TAG_GYR                           (0x01)
#define LSM6DSV80X_TAG_ACC                           (0x02)
#define LSM6DSV80X_TAG_HG_ACC                        (0x1D)

#ifndef LSM6DSV80X_TASK_CFG_MAX_INSTANCES_COUNT
#define LSM6DSV80X_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)                   SYS_DEBUGF3(SYS_DBG_LSM6DSV80X, level, message)

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static int16_t dummyDataCounter_acc = 0;
static int16_t dummyDataCounter_hg_acc = 0;
static int16_t dummyDataCounter_gyro = 0;
#endif

/**
  * Class object declaration
  */
typedef struct _LSM6DSV80XTaskClass
{
  /**
    * LSM6DSV80XTask class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * Accelerometer IF virtual table.
    */
  const ISensorMems_vtbl acc_sensor_if_vtbl;

  /**
    * Accelerometer IF virtual table.
    */
  const ISensorMems_vtbl hg_acc_sensor_if_vtbl;

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
    * Specifies high-g accelerometer sensor capabilities.
    */
  const SensorDescriptor_t hg_acc_class_descriptor;

  /**
    * Specifies gyroscope sensor capabilities.
    */
  const SensorDescriptor_t gyro_class_descriptor;

  /**
    * Specifies mlc sensor capabilities.
    */
  const SensorDescriptor_t mlc_class_descriptor;

  /**
    * LSM6DSV80XTask (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[2 * LSM6DSV80X_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} LSM6DSV80XTaskClass_t;

/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LSM6DSV80XTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LSM6DSV80XTaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LSM6DSV80XTaskSensorInit(LSM6DSV80XTask *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LSM6DSV80XTaskSensorReadData(LSM6DSV80XTask *_this);

/**
  * Read the data from the mlc.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t LSM6DSV80XTaskSensorReadMLC(LSM6DSV80XTask *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t LSM6DSV80XTaskSensorRegister(LSM6DSV80XTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t LSM6DSV80XTaskSensorInitTaskParams(LSM6DSV80XTask *_this);

/**
  * Private implementation of sensor interface methods for LSM6DSV80X sensor
  */
static sys_error_code_t LSM6DSV80XTaskSensorSetODR(LSM6DSV80XTask *_this, SMMessage report);
static sys_error_code_t LSM6DSV80XTaskSensorSetFS(LSM6DSV80XTask *_this, SMMessage report);
static sys_error_code_t LSM6DSV80XTaskSensorSetFifoWM(LSM6DSV80XTask *_this, SMMessage report);
static sys_error_code_t LSM6DSV80XTaskSensorEnable(LSM6DSV80XTask *_this, SMMessage report);
static sys_error_code_t LSM6DSV80XTaskSensorDisable(LSM6DSV80XTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t LSM6DSV80XTaskSensorIsActive(const LSM6DSV80XTask *_this);

static sys_error_code_t LSM6DSV80XTaskEnterLowPowerMode(const LSM6DSV80XTask *_this);

static sys_error_code_t LSM6DSV80XTaskConfigureIrqPin(const LSM6DSV80XTask *_this, boolean_t LowPower);
static sys_error_code_t LSM6DSV80XTaskConfigureMLCPin(const LSM6DSV80XTask *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void LSM6DSV80XTaskTimerCallbackFunction(ULONG param);

/**
  * Callback function called when the mlc software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void LSM6DSV80XTaskMLCTimerCallbackFunction(ULONG param);

/**
  * Given a interface pointer it return the instance of the object that implement the interface.
  *
  * @param p_if [IN] specifies a sensor interface implemented by the task object.
  * @return the instance of the task object that implements the given interface.
  */
static inline LSM6DSV80XTask *LSM6DSV80XTaskGetOwnerFromISensorIF(ISensor_t *p_if);

/**
  * Given a interface pointer it return the instance of the object that implement the interface.
  *
  * @param p_if [IN] specifies a ISensorLL interface implemented by the task object.
  * @return the instance of the task object that implements the given interface.
  */
static inline LSM6DSV80XTask *LSM6DSV80XTaskGetOwnerFromISensorLLIF(ISensorLL_t *p_if);

/**
  * Read the odr value from the sensor and update the internal model
  */
static sys_error_code_t LSM6DSV80X_ODR_Sync(LSM6DSV80XTask *_this);

/**
  * Read the odr value from the sensor and update the internal model
  */
static sys_error_code_t LSM6DSV80X_FS_Sync(LSM6DSV80XTask *_this);

/**
  *  Return Min of 3 non negative floats
  */
static float_t LSM6DSV80X_FindMin(float_t Val1, float_t Val2, float_t Val3);

/**
  * Return Max of 3 floats
  */
static float_t __attribute__((unused)) LSM6DSV80X_FindMax(float_t Val1, float_t Val2, float_t Val3);

/**
  * Sort the sensors based on the sensor's ODRs
  */
static sys_error_code_t LSM6DSV80X_SensorSpeedSort(float_t ODR1, float_t ODR2, float_t ODR3, uint8_t *FastSensor,
                                                   uint8_t *MediumSensor, uint8_t *SlowSensor);

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
static inline sys_error_code_t LSM6DSV80XTaskPostReportToFront(LSM6DSV80XTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t LSM6DSV80XTaskPostReportToBack(LSM6DSV80XTask *_this, SMMessage *pReport);

/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
//static LSM6DSV80XTask sTaskObj;
/**
  * The class object.
  */
static LSM6DSV80XTaskClass_t sTheClass =
{
  /* class virtual table */
  {
    LSM6DSV80XTask_vtblHardwareInit,
    LSM6DSV80XTask_vtblOnCreateTask,
    LSM6DSV80XTask_vtblDoEnterPowerMode,
    LSM6DSV80XTask_vtblHandleError,
    LSM6DSV80XTask_vtblOnEnterTaskControlLoop,
    LSM6DSV80XTask_vtblForceExecuteStep,
    LSM6DSV80XTask_vtblOnEnterPowerMode
  },

  /* class::acc_sensor_if_vtbl virtual table */
  {
    {
      {
        LSM6DSV80XTask_vtblAccGetId,
        LSM6DSV80XTask_vtblAccGetEventSourceIF,
        LSM6DSV80XTask_vtblAccGetDataInfo
      },
      LSM6DSV80XTask_vtblSensorEnable,
      LSM6DSV80XTask_vtblSensorDisable,
      LSM6DSV80XTask_vtblSensorIsEnabled,
      LSM6DSV80XTask_vtblAccGetDescription,
      LSM6DSV80XTask_vtblAccGetStatus,
      LSM6DSV80XTask_vtblAccGetStatusPointer
    },
    LSM6DSV80XTask_vtblAccGetODR,
    LSM6DSV80XTask_vtblAccGetFS,
    LSM6DSV80XTask_vtblAccGetSensitivity,
    LSM6DSV80XTask_vtblSensorSetODR,
    LSM6DSV80XTask_vtblSensorSetFS,
    LSM6DSV80XTask_vtblSensorSetFifoWM
  },

  /* class::hg_acc_sensor_if_vtbl virtual table */
  {
    {
      {
        LSM6DSV80XTask_vtblHgAccGetId,
        LSM6DSV80XTask_vtblHgAccGetEventSourceIF,
        LSM6DSV80XTask_vtblHgAccGetDataInfo
      },
      LSM6DSV80XTask_vtblSensorEnable,
      LSM6DSV80XTask_vtblSensorDisable,
      LSM6DSV80XTask_vtblSensorIsEnabled,
      LSM6DSV80XTask_vtblHgAccGetDescription,
      LSM6DSV80XTask_vtblHgAccGetStatus,
      LSM6DSV80XTask_vtblHgAccGetStatusPointer
    },
    LSM6DSV80XTask_vtblHgAccGetODR,
    LSM6DSV80XTask_vtblHgAccGetFS,
    LSM6DSV80XTask_vtblHgAccGetSensitivity,
    LSM6DSV80XTask_vtblSensorSetODR,
    LSM6DSV80XTask_vtblSensorSetFS,
    LSM6DSV80XTask_vtblSensorSetFifoWM
  },

  /* class::gyro_sensor_if_vtbl virtual table */
  {
    {
      {
        LSM6DSV80XTask_vtblGyroGetId,
        LSM6DSV80XTask_vtblGyroGetEventSourceIF,
        LSM6DSV80XTask_vtblGyroGetDataInfo
      },
      LSM6DSV80XTask_vtblSensorEnable,
      LSM6DSV80XTask_vtblSensorDisable,
      LSM6DSV80XTask_vtblSensorIsEnabled,
      LSM6DSV80XTask_vtblGyroGetDescription,
      LSM6DSV80XTask_vtblGyroGetStatus,
      LSM6DSV80XTask_vtblGyroGetStatusPointer
    },
    LSM6DSV80XTask_vtblGyroGetODR,
    LSM6DSV80XTask_vtblGyroGetFS,
    LSM6DSV80XTask_vtblGyroGetSensitivity,
    LSM6DSV80XTask_vtblSensorSetODR,
    LSM6DSV80XTask_vtblSensorSetFS,
    LSM6DSV80XTask_vtblSensorSetFifoWM
  },

  /* class::mlc_sensor_if_vtbl virtual table */
  {
    {
      {
        LSM6DSV80XTask_vtblMlcGetId,
        LSM6DSV80XTask_vtblMlcGetEventSourceIF,
        LSM6DSV80XTask_vtblMlcGetDataInfo
      },
      LSM6DSV80XTask_vtblSensorEnable,
      LSM6DSV80XTask_vtblSensorDisable,
      LSM6DSV80XTask_vtblSensorIsEnabled,
      LSM6DSV80XTask_vtblMlcGetDescription,
      LSM6DSV80XTask_vtblMlcGetStatus,
      LSM6DSV80XTask_vtblMlcGetStatusPointer
    },
    LSM6DSV80XTask_vtblMlcGetODR,
    LSM6DSV80XTask_vtblMlcGetFS,
    LSM6DSV80XTask_vtblMlcGetSensitivity,
    LSM6DSV80XTask_vtblSensorSetODR,
    LSM6DSV80XTask_vtblSensorSetFS,
    LSM6DSV80XTask_vtblSensorSetFifoWM
  },

  /* class::sensor_ll_if_vtbl virtual table */
  {
    LSM6DSV80XTask_vtblSensorReadReg,
    LSM6DSV80XTask_vtblSensorWriteReg,
    LSM6DSV80XTask_vtblSensorSyncModel
  },

  /* ACCELEROMETER DESCRIPTOR */
  {
    "lsm6dsv80x",
    COM_TYPE_ACC
    /* TODO: Remove this
    ,
      {
        1.875,
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
      }*/
  },

  /* HIGH-G ACCELEROMETER DESCRIPTOR */
  {
    "lsm6dsv80x",
    COM_TYPE_HG_ACC /* TODO: Remove this ,
    {
      480,
      960,
      1920,
      3840,
      7680,
      COM_END_OF_LIST_FLOAT,
    },
    {
      32,
      64,
      128,
      256,
      COM_END_OF_LIST_FLOAT,
    },
    {
      "hgac",
    },
      "g",
    {
      0,
      1000,
    } */
  },

  /* GYROSCOPE DESCRIPTOR */
  {
    "lsm6dsv80x",
    COM_TYPE_GYRO /* todo: remove this,
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
    } */
  },

  /* MLC DESCRIPTOR */
  {
    "lsm6dsv80x",
    COM_TYPE_MLC /* todo: remove this,
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
    }*/
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    LSM6DSV80XTaskExecuteStepState1,
    NULL,
    LSM6DSV80XTaskExecuteStepDatalog,
  },

  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *LSM6DSV80XTaskGetAccSensorIF(LSM6DSV80XTask *_this)
{
  return (ISourceObservable *) & (_this->acc_sensor_if);
}

ISourceObservable *LSM6DSV80XTaskGetHgAccSensorIF(LSM6DSV80XTask *_this)
{
  return (ISourceObservable *) & (_this->hg_acc_sensor_if);
}

ISourceObservable *LSM6DSV80XTaskGetGyroSensorIF(LSM6DSV80XTask *_this)
{
  return (ISourceObservable *) & (_this->gyro_sensor_if);
}

ISourceObservable *LSM6DSV80XTaskGetMlcSensorIF(LSM6DSV80XTask *_this)
{
  return (ISourceObservable *) & (_this->mlc_sensor_if);
}

ISensorLL_t *LSM6DSV80XTaskGetSensorLLIF(LSM6DSV80XTask *_this)
{
  return (ISensorLL_t *) & (_this->sensor_ll_if);
}

AManagedTaskEx *LSM6DSV80XTaskAlloc(const void *pIRQConfig, const void *pMLCConfig, const void *pCSConfig)
{
  LSM6DSV80XTask *p_new_obj = SysAlloc(sizeof(LSM6DSV80XTask));

  if (p_new_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_new_obj->super);

    p_new_obj->super.vptr = &sTheClass.vtbl;
    p_new_obj->acc_sensor_if.vptr = &sTheClass.acc_sensor_if_vtbl;
    p_new_obj->hg_acc_sensor_if.vptr = &sTheClass.hg_acc_sensor_if_vtbl;
    p_new_obj->gyro_sensor_if.vptr = &sTheClass.gyro_sensor_if_vtbl;
    p_new_obj->mlc_sensor_if.vptr = &sTheClass.mlc_sensor_if_vtbl;
    p_new_obj->sensor_ll_if.vptr = &sTheClass.sensor_ll_if_vtbl;
    p_new_obj->acc_sensor_descriptor = &sTheClass.acc_class_descriptor;
    p_new_obj->hg_acc_sensor_descriptor = &sTheClass.hg_acc_class_descriptor;
    p_new_obj->gyro_sensor_descriptor = &sTheClass.gyro_class_descriptor;
    p_new_obj->mlc_sensor_descriptor = &sTheClass.mlc_class_descriptor;

    p_new_obj->pIRQConfig = (MX_GPIOParams_t *) pIRQConfig;
    p_new_obj->pMLCConfig = (MX_GPIOParams_t *) pMLCConfig;
    p_new_obj->pCSConfig = (MX_GPIOParams_t *) pCSConfig;

    strcpy(p_new_obj->acc_sensor_status.p_name, sTheClass.acc_class_descriptor.p_name);
    strcpy(p_new_obj->hg_acc_sensor_status.p_name, sTheClass.hg_acc_class_descriptor.p_name);
    strcpy(p_new_obj->gyro_sensor_status.p_name, sTheClass.gyro_class_descriptor.p_name);
    strcpy(p_new_obj->mlc_sensor_status.p_name, sTheClass.mlc_class_descriptor.p_name);
  }

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *LSM6DSV80XTaskAllocSetName(const void *pIRQConfig, const void *pMLCConfig, const void *pCSConfig,
                                           const char *p_name)
{
  LSM6DSV80XTask *p_new_obj = (LSM6DSV80XTask *) LSM6DSV80XTaskAlloc(pIRQConfig, pMLCConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->acc_sensor_status.p_name, p_name);
  strcpy(p_new_obj->hg_acc_sensor_status.p_name, p_name);
  strcpy(p_new_obj->gyro_sensor_status.p_name, p_name);
  strcpy(p_new_obj->mlc_sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *LSM6DSV80XTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pMLCConfig,
                                          const void *pCSConfig)
{
  LSM6DSV80XTask *p_obj = (LSM6DSV80XTask *) p_mem_block;

  if (p_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_obj->super);

    p_obj->super.vptr = &sTheClass.vtbl;
    p_obj->acc_sensor_if.vptr = &sTheClass.acc_sensor_if_vtbl;
    p_obj->hg_acc_sensor_if.vptr = &sTheClass.hg_acc_sensor_if_vtbl;
    p_obj->gyro_sensor_if.vptr = &sTheClass.gyro_sensor_if_vtbl;
    p_obj->mlc_sensor_if.vptr = &sTheClass.mlc_sensor_if_vtbl;
    p_obj->sensor_ll_if.vptr = &sTheClass.sensor_ll_if_vtbl;
    p_obj->acc_sensor_descriptor = &sTheClass.acc_class_descriptor;
    p_obj->hg_acc_sensor_descriptor = &sTheClass.hg_acc_class_descriptor;
    p_obj->gyro_sensor_descriptor = &sTheClass.gyro_class_descriptor;
    p_obj->mlc_sensor_descriptor = &sTheClass.mlc_class_descriptor;

    p_obj->pIRQConfig = (MX_GPIOParams_t *) pIRQConfig;
    p_obj->pMLCConfig = (MX_GPIOParams_t *) pMLCConfig;
    p_obj->pCSConfig = (MX_GPIOParams_t *) pCSConfig;
  }

  return (AManagedTaskEx *) p_obj;
}

AManagedTaskEx *LSM6DSV80XTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pMLCConfig,
                                                 const void *pCSConfig, const char *p_name)
{
  LSM6DSV80XTask *p_obj = (LSM6DSV80XTask *) LSM6DSV80XTaskStaticAlloc(p_mem_block, pIRQConfig, pMLCConfig, pCSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->acc_sensor_status.p_name, p_name);
  strcpy(p_obj->hg_acc_sensor_status.p_name, p_name);
  strcpy(p_obj->gyro_sensor_status.p_name, p_name);
  strcpy(p_obj->mlc_sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *LSM6DSV80XTaskGetSensorIF(LSM6DSV80XTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *LSM6DSV80XTaskGetAccEventSrcIF(LSM6DSV80XTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_acc_event_src;
}

IEventSrc *LSM6DSV80XTaskGetHgAccEventSrcIF(LSM6DSV80XTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_hg_acc_event_src;
}

IEventSrc *LSM6DSV80XTaskGetGyroEventSrcIF(LSM6DSV80XTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_gyro_event_src;
}

IEventSrc *LSM6DSV80XTaskGetMlcEventSrcIF(LSM6DSV80XTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_mlc_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t LSM6DSV80XTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV80XTask *p_obj = (LSM6DSV80XTask *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }

  return res;
}

sys_error_code_t LSM6DSV80XTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                                 VOID **pvStackStart,
                                                 ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                                 ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV80XTask *p_obj = (LSM6DSV80XTask *) _this;
  p_obj->sync = true;

  /* Create task specific sw resources. */

  uint32_t item_size = (uint32_t) LSM6DSV80X_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(LSM6DSV80X_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "LSM6DSV80X_Q", item_size / 4u, p_queue_items_buff,
                                    LSM6DSV80X_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_timer, "LSM6DSV80X_T", LSM6DSV80XTaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(LSM6DSV80X_TASK_CFG_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* create the mlc software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->mlc_timer, "LSM6DSV80X_MLC_T", LSM6DSV80XTaskMLCTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(LSM6DSV80X_TASK_CFG_MLC_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(LSM6DSV80X_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    p_obj->p_sensor_bus_if = I2CBusIFAlloc(LSM6DSV80X_ID, LSM6DSV80X_I2C_ADD_H, 0);
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

  /* Initialize the EventSrc interface for high g accel*/
  p_obj->p_hg_acc_event_src = DataEventSrcAlloc();
  if (p_obj->p_hg_acc_event_src == NULL)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return res;
  }
  IEventSrcInit(p_obj->p_hg_acc_event_src);

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
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, 2 * LSM6DSV80X_TASK_CFG_MAX_INSTANCES_COUNT);
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

#if LSM6DSV80X_FIFO_ENABLED
  memset(p_obj->p_fast_sensor_data_buff, 0, sizeof(p_obj->p_fast_sensor_data_buff));
  memset(p_obj->p_medium_sensor_data_buff, 0, sizeof(p_obj->p_medium_sensor_data_buff));
  memset(p_obj->p_slow_sensor_data_buff, 0, sizeof(p_obj->p_slow_sensor_data_buff));
#else
  memset(p_obj->p_acc_sample, 0, sizeof(p_obj->p_acc_sample));
  memset(p_obj->p_hg_acc_sample, 0, sizeof(p_obj->p_hg_acc_sample));
  memset(p_obj->p_gyro_sample, 0, sizeof(p_obj->p_gyro_sample));
  p_obj->acc_drdy = 0;
  p_obj->hg_acc_drdy = 0;
  p_obj->gyro_drdy = 0;
#endif
  memset(p_obj->p_mlc_sensor_data_buff, 0, sizeof(p_obj->p_mlc_sensor_data_buff));
  p_obj->acc_id = 0;
  p_obj->hg_acc_id = 1;
  p_obj->gyro_id = 2;
  p_obj->mlc_enable = FALSE;
  p_obj->prev_timestamp = 0.0f;
  p_obj->acc_samples_count = 0;
  p_obj->hg_acc_samples_count = 0;
  p_obj->gyro_samples_count = 0;
  p_obj->fifo_level = 0;
  p_obj->samples_per_it = 0;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "LSM6DSV80X";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = LSM6DSV80X_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = LSM6DSV80X_TASK_CFG_PRIORITY;
  *pPreemptThreshold = LSM6DSV80X_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = LSM6DSV80XTaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = LSM6DSV80XTaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV80X: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t LSM6DSV80XTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV80XTask *p_obj = (LSM6DSV80XTask *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (LSM6DSV80XTaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV80X: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      /* Deactivate the sensor */
      lsm6dsv80x_xl_data_rate_set(p_sensor_drv, LSM6DSV80X_ODR_OFF);
      lsm6dsv80x_hg_xl_data_rate_set(p_sensor_drv, LSM6DSV80X_HG_XL_ODR_OFF, 0);
      lsm6dsv80x_gy_data_rate_set(p_sensor_drv, LSM6DSV80X_ODR_OFF);
      lsm6dsv80x_fifo_gy_batch_set(p_sensor_drv, LSM6DSV80X_GY_NOT_BATCHED);
      lsm6dsv80x_fifo_xl_batch_set(p_sensor_drv, LSM6DSV80X_XL_NOT_BATCHED);
      lsm6dsv80x_fifo_hg_xl_batch_set(p_sensor_drv, 0);
      lsm6dsv80x_fifo_mode_set(p_sensor_drv, LSM6DSV80X_BYPASS_MODE);
      p_obj->samples_per_it = 0;

      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_timer);
      }
      else
      {
        LSM6DSV80XTaskConfigureIrqPin(p_obj, TRUE);
      }
      if (p_obj->pMLCConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->mlc_timer);
      }
      else
      {
        LSM6DSV80XTaskConfigureMLCPin(p_obj, TRUE);
      }
      memset(p_obj->p_mlc_sensor_data_buff, 0, sizeof(p_obj->p_mlc_sensor_data_buff));
    }
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV80X: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = LSM6DSV80XTaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV80X - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->pIRQConfig != NULL)
    {
      LSM6DSV80XTaskConfigureIrqPin(p_obj, TRUE);
    }
    if (p_obj->pMLCConfig != NULL)
    {
      LSM6DSV80XTaskConfigureMLCPin(p_obj, TRUE);
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV80X: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t LSM6DSV80XTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  LSM6DSV80XTask *p_obj = (LSM6DSV80XTask*)_this;

  return res;
}

sys_error_code_t LSM6DSV80XTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV80X: start.\r\n"));

#if defined (ENABLE_THREADX_DBG_PIN) && defined (LSM6DSV80X_TASK_CFG_TAG)
  LSM6DSV80XTask *p_obj = (LSM6DSV80XTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = LSM6DSV80X_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t LSM6DSV80XTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV80XTask *p_obj = (LSM6DSV80XTask *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = LSM6DSV80XTaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t LSM6DSV80XTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  LSM6DSV80XTask *p_obj = (LSM6DSV80XTask*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t LSM6DSV80XTask_vtblAccGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, acc_sensor_if));
  uint8_t res = p_if_owner->acc_id;

  return res;
}

uint8_t LSM6DSV80XTask_vtblHgAccGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, hg_acc_sensor_if));
  uint8_t res = p_if_owner->hg_acc_id;

  return res;
}

uint8_t LSM6DSV80XTask_vtblGyroGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, gyro_sensor_if));
  uint8_t res = p_if_owner->gyro_id;

  return res;
}

uint8_t LSM6DSV80XTask_vtblMlcGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, mlc_sensor_if));
  uint8_t res = p_if_owner->mlc_id;

  return res;
}

IEventSrc *LSM6DSV80XTask_vtblAccGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, acc_sensor_if));

  return p_if_owner->p_acc_event_src;
}

IEventSrc *LSM6DSV80XTask_vtblHgAccGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, hg_acc_sensor_if));

  return p_if_owner->p_hg_acc_event_src;
}

IEventSrc *LSM6DSV80XTask_vtblGyroGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, gyro_sensor_if));
  return p_if_owner->p_gyro_event_src;
}

IEventSrc *LSM6DSV80XTask_vtblMlcGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, mlc_sensor_if));
  return p_if_owner->p_mlc_event_src;
}

sys_error_code_t LSM6DSV80XTask_vtblAccGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, acc_sensor_if));
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

float_t LSM6DSV80XTask_vtblAccGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, acc_sensor_if));
  float_t res = p_if_owner->acc_sensor_status.type.mems.fs;

  return res;
}

float_t LSM6DSV80XTask_vtblAccGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, acc_sensor_if));
  float_t res = p_if_owner->acc_sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t LSM6DSV80XTask_vtblAccGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, acc_sensor_if));
  EMData_t res = p_if_owner->data_acc;

  return res;
}

sys_error_code_t LSM6DSV80XTask_vtblHgAccGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, hg_acc_sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if ((p_measured == NULL) || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = p_if_owner->hg_acc_sensor_status.type.mems.measured_odr;
    *p_nominal = p_if_owner->hg_acc_sensor_status.type.mems.odr;
  }

  return res;
}

float_t LSM6DSV80XTask_vtblHgAccGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, hg_acc_sensor_if));
  float_t res = p_if_owner->hg_acc_sensor_status.type.mems.fs;

  return res;
}

float_t LSM6DSV80XTask_vtblHgAccGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, hg_acc_sensor_if));
  float_t res = p_if_owner->hg_acc_sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t LSM6DSV80XTask_vtblHgAccGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, hg_acc_sensor_if));
  EMData_t res = p_if_owner->data_hg_acc;

  return res;
}

sys_error_code_t LSM6DSV80XTask_vtblGyroGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, gyro_sensor_if));
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

float_t LSM6DSV80XTask_vtblGyroGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, gyro_sensor_if));
  float_t res = p_if_owner->gyro_sensor_status.type.mems.fs;

  return res;
}

float_t LSM6DSV80XTask_vtblGyroGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, gyro_sensor_if));
  float_t res = p_if_owner->gyro_sensor_status.type.mems.sensitivity;

  return res;
}

EMData_t LSM6DSV80XTask_vtblGyroGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, gyro_sensor_if));
  EMData_t res = p_if_owner->data_gyro;

  return res;
}

sys_error_code_t LSM6DSV80XTask_vtblMlcGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal)
{
  assert_param(_this != NULL);
  /*get the object implementing the ISourceObservable IF */
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, mlc_sensor_if));
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

float_t LSM6DSV80XTask_vtblMlcGetFS(ISensorMems_t *_this)
{
  assert_param(_this != NULL);

  /* MLC does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV80X: warning - MLC GetFS() not supported.\r\n"));

  return -1.0f;
}

float_t LSM6DSV80XTask_vtblMlcGetSensitivity(ISensorMems_t *_this)
{
  assert_param(_this != NULL);

  /* MLC does not support this virtual function.*/
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV80X: warning - MLC GetSensitivity() not supported.\r\n"));

  return -1.0f;
}

EMData_t LSM6DSV80XTask_vtblMlcGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = (LSM6DSV80XTask *)((uint32_t) _this - offsetof(LSM6DSV80XTask, mlc_sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t LSM6DSV80XTask_vtblSensorSetODR(ISensorMems_t *_this, float_t odr)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF((ISensor_t *)_this);

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
      else if (sensor_id == p_if_owner->hg_acc_id)
      {
        p_if_owner->hg_acc_sensor_status.type.mems.odr = odr;
        p_if_owner->hg_acc_sensor_status.type.mems.measured_odr = 0.0f;
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
      .sensorMessage.fParam = (float_t) odr
    };
    res = LSM6DSV80XTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t LSM6DSV80XTask_vtblSensorSetFS(ISensorMems_t *_this, float_t fs)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF((ISensor_t *)_this);

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
    else if (sensor_id == p_if_owner->hg_acc_id)
    {
      p_if_owner->hg_acc_sensor_status.type.mems.fs = fs;
      p_if_owner->hg_acc_sensor_status.type.mems.sensitivity = 0.0000305f * p_if_owner->hg_acc_sensor_status.type.mems.fs;
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
      .sensorMessage.fParam = (float_t) fs
    };
    res = LSM6DSV80XTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t LSM6DSV80XTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF((ISensor_t *)_this);

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
    res = LSM6DSV80XTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t LSM6DSV80XTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF(_this);

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
    else if (sensor_id == p_if_owner->hg_acc_id)
    {
      p_if_owner->hg_acc_sensor_status.is_active = TRUE;
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
    res = LSM6DSV80XTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t LSM6DSV80XTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF(_this);

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
    else if (sensor_id == p_if_owner->hg_acc_id)
    {
      p_if_owner->hg_acc_sensor_status.is_active = FALSE;
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
    res = LSM6DSV80XTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t LSM6DSV80XTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF(_this);

  if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->acc_id)
  {
    res = p_if_owner->acc_sensor_status.is_active;
  }
  else if (ISourceGetId((ISourceObservable *) _this) == p_if_owner->hg_acc_id)
  {
    res = p_if_owner->hg_acc_sensor_status.is_active;
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

SensorDescriptor_t LSM6DSV80XTask_vtblAccGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->acc_sensor_descriptor;
}

SensorDescriptor_t LSM6DSV80XTask_vtblHgAccGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->hg_acc_sensor_descriptor;
}

SensorDescriptor_t LSM6DSV80XTask_vtblGyroGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->gyro_sensor_descriptor;
}

SensorDescriptor_t LSM6DSV80XTask_vtblMlcGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF(_this);
  return *p_if_owner->mlc_sensor_descriptor;
}

SensorStatus_t LSM6DSV80XTask_vtblAccGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->acc_sensor_status;
}

SensorStatus_t LSM6DSV80XTask_vtblHgAccGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->hg_acc_sensor_status;
}

SensorStatus_t LSM6DSV80XTask_vtblGyroGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->gyro_sensor_status;
}

SensorStatus_t LSM6DSV80XTask_vtblMlcGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF(_this);
  return p_if_owner->mlc_sensor_status;
}

SensorStatus_t *LSM6DSV80XTask_vtblAccGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF(_this);
  return &p_if_owner->acc_sensor_status;
}

SensorStatus_t *LSM6DSV80XTask_vtblHgAccGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF(_this);
  return &p_if_owner->hg_acc_sensor_status;
}

SensorStatus_t *LSM6DSV80XTask_vtblGyroGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF(_this);
  return &p_if_owner->gyro_sensor_status;
}

SensorStatus_t *LSM6DSV80XTask_vtblMlcGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorIF(_this);
  return &p_if_owner->mlc_sensor_status;
}

sys_error_code_t LSM6DSV80XTask_vtblSensorReadReg(ISensorLL_t *_this, uint16_t reg, uint8_t *data, uint16_t len)
{
  assert_param(_this != NULL);
  assert_param(reg <= 0xFFU);
  assert_param(data != NULL);
  assert_param(len != 0U);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorLLIF(_this);
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_if_owner->p_sensor_bus_if->m_xConnector;
  uint8_t reg8 = (uint8_t)(reg & 0x00FF);

  if (lsm6dsv80x_read_reg(p_sensor_drv, reg8, data, len) != 0)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t LSM6DSV80XTask_vtblSensorWriteReg(ISensorLL_t *_this, uint16_t reg, const uint8_t *data, uint16_t len)
{
  assert_param(_this != NULL);
  assert_param(reg <= 0xFFU);
  assert_param(data != NULL);
  assert_param(len != 0U);

  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorLLIF(_this);
  uint8_t reg8 = (uint8_t)(reg & 0x00FF);

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_if_owner->p_sensor_bus_if->m_xConnector;

  /* This generic register write operation could mean that the model is out of sync with the HW */
  p_if_owner->sync = false;

  if (lsm6dsv80x_write_reg(p_sensor_drv, reg8, (uint8_t *) data, len) != 0)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t LSM6DSV80XTask_vtblSensorSyncModel(ISensorLL_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV80XTask *p_if_owner = LSM6DSV80XTaskGetOwnerFromISensorLLIF(_this);

  if (LSM6DSV80X_ODR_Sync(p_if_owner) != SYS_NO_ERROR_CODE)
  {
    res = SYS_BASE_ERROR_CODE;
  }
  if (LSM6DSV80X_FS_Sync(p_if_owner) != SYS_NO_ERROR_CODE)
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
static sys_error_code_t LSM6DSV80XTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV80XTask *p_obj = (LSM6DSV80XTask *) _this;
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
            res = LSM6DSV80XTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = LSM6DSV80XTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = LSM6DSV80XTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = LSM6DSV80XTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = LSM6DSV80XTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV80X: unexpected report in Run: %i\r\n", report.messageID));
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV80X: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static sys_error_code_t LSM6DSV80XTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  LSM6DSV80XTask *p_obj = (LSM6DSV80XTask *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("LSM6DSV80X: new data.\r\n"));
        res = LSM6DSV80XTaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
#if LSM6DSV80X_FIFO_ENABLED
          if (p_obj->fifo_level != 0)
          {
#endif
            // notify the listeners...
            double_t timestamp = report.sensorDataReadyMessage.fTimestamp;
            double_t delta_timestamp = timestamp - p_obj->prev_timestamp;
            p_obj->prev_timestamp = timestamp;

            DataEvent_t evt_hg_acc, evt_acc, evt_gyro;
            float_t hg_acc_odr = 0;
            float_t acc_odr = 0;
            float_t gyro_odr = 0;
            bool SensorActive = false;

#if LSM6DSV80X_FIFO_ENABLED
            if (p_obj->hg_acc_sensor_status.is_active)
            {
              /* update measuredODR */
              p_obj->hg_acc_sensor_status.type.mems.measured_odr = (float_t) p_obj->hg_acc_samples_count / (float_t) delta_timestamp;
              /* used for sensor speed sort */
              hg_acc_odr = p_obj->hg_acc_sensor_status.type.mems.odr;
              SensorActive = true;
            }
            if (p_obj->acc_sensor_status.is_active)
            {
              /* update measuredODR */
              p_obj->acc_sensor_status.type.mems.measured_odr = (float_t) p_obj->acc_samples_count / (float_t) delta_timestamp;
              /* used for sensor speed sort */
              acc_odr = p_obj->acc_sensor_status.type.mems.odr;
              SensorActive = true;
            }
            if (p_obj->gyro_sensor_status.is_active)
            {
              /* update measuredODR */
              p_obj->gyro_sensor_status.type.mems.measured_odr = (float_t) p_obj->gyro_samples_count / (float_t) delta_timestamp;
              /* used for sensor speed sort */
              gyro_odr = p_obj->gyro_sensor_status.type.mems.odr;
              SensorActive = true;
            }

            if (true == SensorActive)
            {
              uint8_t FastSensor = 0;
              uint8_t MediumSensor = 0;
              uint8_t SlowSensor = 0;

              /* Sort sensors by speed */
              LSM6DSV80X_SensorSpeedSort(hg_acc_odr, acc_odr, gyro_odr, &FastSensor, &MediumSensor, &SlowSensor);

              if (p_obj->hg_acc_sensor_status.is_active)
              {
                if (1 == FastSensor)
                {
                  // Highest ODR, use largest buffer
                  /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue:
                   * [X0, Y0, Z0]
                   * [X1, Y1, Z1]
                   * ...
                   * [Xm-1, Ym-1, Zm-1]
                   */
                  EMD_Init(&p_obj->data_hg_acc, p_obj->p_fast_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                           p_obj->hg_acc_samples_count, 3);
                }
                else if (1 == MediumSensor)
                {
                  // Middle ODR, use intermediate buffer
                  /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue:
                   * [X0, Y0, Z0]
                   * [X1, Y1, Z1]
                   * ...
                   * [Xm-1, Ym-1, Zm-1]
                   */
                  EMD_Init(&p_obj->data_hg_acc, p_obj->p_medium_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                           p_obj->hg_acc_samples_count, 3);
                }
                else
                {
                  // Slowest ODR, use smallest buffer
                  /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue:
                   * [X0, Y0, Z0]
                   * [X1, Y1, Z1]
                   * ...
                   * [Xm-1, Ym-1, Zm-1]
                   */
                  EMD_Init(&p_obj->data_hg_acc, p_obj->p_slow_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                           p_obj->hg_acc_samples_count, 3);
                }
                DataEventInit((IEvent *) &evt_hg_acc, p_obj->p_hg_acc_event_src, &p_obj->data_hg_acc, timestamp, p_obj->hg_acc_id);
              }

              if (p_obj->acc_sensor_status.is_active)
              {
                if (2 == FastSensor)
                {
                  // Highest ODR, use largest buffer
                  /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue:
                   * [X0, Y0, Z0]
                   * [X1, Y1, Z1]
                   * ...
                   * [Xm-1, Ym-1, Zm-1]
                   */
                  EMD_Init(&p_obj->data_acc, p_obj->p_fast_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                           p_obj->acc_samples_count, 3);
                }
                else if (2 == MediumSensor)
                {
                  // Middle ODR, use intermediate buffer
                  /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue:
                   * [X0, Y0, Z0]
                   * [X1, Y1, Z1]
                   * ...
                   * [Xm-1, Ym-1, Zm-1]
                   */
                  EMD_Init(&p_obj->data_acc, p_obj->p_medium_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                           p_obj->acc_samples_count, 3);
                }
                else
                {
                  // Slowest ODR, use smallest buffer
                  /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue:
                   * [X0, Y0, Z0]
                   * [X1, Y1, Z1]
                   * ...
                   * [Xm-1, Ym-1, Zm-1]
                   */
                  EMD_Init(&p_obj->data_acc, p_obj->p_slow_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                           p_obj->acc_samples_count, 3);
                }
                DataEventInit((IEvent *) &evt_acc, p_obj->p_acc_event_src, &p_obj->data_acc, timestamp, p_obj->acc_id);
              }

              if (p_obj->gyro_sensor_status.is_active)
              {
                if (3 == FastSensor)
                {
                  // Highest ODR, use largest buffer
                  /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue:
                   * [X0, Y0, Z0]
                   * [X1, Y1, Z1]
                   * ...
                   * [Xm-1, Ym-1, Zm-1]
                   */
                  EMD_Init(&p_obj->data_gyro, p_obj->p_fast_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                           p_obj->gyro_samples_count, 3);
                }
                else if (3 == MediumSensor)
                {
                  // Middle ODR, use intermediate buffer
                  /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue:
                   * [X0, Y0, Z0]
                   * [X1, Y1, Z1]
                   * ...
                   * [Xm-1, Ym-1, Zm-1]
                   */
                  EMD_Init(&p_obj->data_gyro, p_obj->p_medium_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                           p_obj->gyro_samples_count, 3);
                }
                else
                {
                  // Slowest ODR, use smallest buffer
                  /* Create a bidimensional data interleaved [m x 3], m is the number of samples in the sensor queue:
                   * [X0, Y0, Z0]
                   * [X1, Y1, Z1]
                   * ...
                   * [Xm-1, Ym-1, Zm-1]
                   */
                  EMD_Init(&p_obj->data_gyro, p_obj->p_slow_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2,
                           p_obj->gyro_samples_count, 3);
                }
                DataEventInit((IEvent *) &evt_gyro, p_obj->p_gyro_event_src, &p_obj->data_gyro, timestamp, p_obj->gyro_id);
              }
              if (p_obj->hg_acc_sensor_status.is_active)
              {
                IEventSrcSendEvent(p_obj->p_hg_acc_event_src, (IEvent *) &evt_hg_acc,
                                   NULL);
              }
              if (p_obj->acc_sensor_status.is_active) { IEventSrcSendEvent(p_obj->p_acc_event_src, (IEvent *) &evt_acc, NULL); }
              if (p_obj->gyro_sensor_status.is_active) { IEventSrcSendEvent(p_obj->p_gyro_event_src, (IEvent *) &evt_gyro, NULL); }
            }
            else
            {
              res = SYS_INVALID_PARAMETER_ERROR_CODE;
            }
#else
            if (p_obj->hg_acc_sensor_status.is_active && p_obj->hg_acc_drdy)
            {
              /* update measuredODR */
              p_obj->hg_acc_sensor_status.type.mems.measured_odr = (float_t)p_obj->hg_acc_samples_count / (float_t)delta_timestamp;

              EMD_Init(&p_obj->data_hg_acc, p_obj->p_hg_acc_sample, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, p_obj->hg_acc_samples_count,
                       3);
              DataEventInit((IEvent *) &evt_hg_acc, p_obj->p_hg_acc_event_src, &p_obj->data_hg_acc, timestamp, p_obj->hg_acc_id);

              IEventSrcSendEvent(p_obj->p_hg_acc_event_src, (IEvent *) &evt_hg_acc, NULL);
              p_obj->hg_acc_drdy = 0;
            }
            if (p_obj->acc_sensor_status.is_active && p_obj->acc_drdy)
            {
              /* update measuredODR */
              p_obj->acc_sensor_status.type.mems.measured_odr = (float_t)p_obj->acc_samples_count / (float_t)delta_timestamp;

              EMD_Init(&p_obj->data_acc, p_obj->p_acc_sample, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, p_obj->acc_samples_count, 3);
              DataEventInit((IEvent *) &evt_acc, p_obj->p_acc_event_src, &p_obj->data_acc, timestamp, p_obj->acc_id);

              IEventSrcSendEvent(p_obj->p_acc_event_src, (IEvent *) &evt_acc, NULL);
              p_obj->acc_drdy = 0;
            }
            if (p_obj->gyro_sensor_status.is_active && p_obj->gyro_drdy)
            {
              /* update measuredODR */
              p_obj->gyro_sensor_status.type.mems.measured_odr = (float_t)p_obj->gyro_samples_count / (float_t)delta_timestamp;

              EMD_Init(&p_obj->data_gyro, p_obj->p_gyro_sample, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, p_obj->gyro_samples_count, 3);
              DataEventInit((IEvent *) &evt_gyro, p_obj->p_gyro_event_src, &p_obj->data_gyro, timestamp, p_obj->gyro_id);

              IEventSrcSendEvent(p_obj->p_gyro_event_src, (IEvent *) &evt_gyro, NULL);
              p_obj->gyro_drdy = 0;
            }
#endif
            SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("LSM6DSV80X: ts = %f\r\n", (float_t)timestamp));
#if LSM6DSV80X_FIFO_ENABLED
          }
#endif
        }
        break;
      }

      case SM_MESSAGE_ID_DATA_READY_MLC:
      {
        res = LSM6DSV80XTaskSensorReadMLC(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          // notify the listeners...
          double_t timestamp = report.sensorDataReadyMessage.fTimestamp;

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
            res = LSM6DSV80XTaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->hg_acc_sensor_status.is_active == true || p_obj->acc_sensor_status.is_active == true
                  || p_obj->gyro_sensor_status.is_active == true)
              {
                if (p_obj->pIRQConfig == NULL)
                {
                  if (TX_SUCCESS
                      != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->lsm6dsv80x_task_cfg_timer_period_ms),
                                         AMT_MS_TO_TICKS(p_obj->lsm6dsv80x_task_cfg_timer_period_ms)))
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
                  LSM6DSV80XTaskConfigureIrqPin(p_obj, FALSE);
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
                      != tx_timer_change(&p_obj->mlc_timer, AMT_MS_TO_TICKS(LSM6DSV80X_TASK_CFG_MLC_TIMER_PERIOD_MS),
                                         AMT_MS_TO_TICKS(LSM6DSV80X_TASK_CFG_MLC_TIMER_PERIOD_MS)))
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
                  LSM6DSV80XTaskConfigureMLCPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_ODR:
            res = LSM6DSV80XTaskSensorSetODR(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FS:
            res = LSM6DSV80XTaskSensorSetFS(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_FIFO_WM:
            res = LSM6DSV80XTaskSensorSetFifoWM(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = LSM6DSV80XTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = LSM6DSV80XTaskSensorDisable(p_obj, report);
            break;
          default:
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
            ;

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV80X: unexpected report in Datalog: %i\r\n", report.messageID));
            break;
        }
        break;
      }

      default:
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV80X: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
    }
  }

  return res;
}


static inline sys_error_code_t LSM6DSV80XTaskPostReportToFront(LSM6DSV80XTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t LSM6DSV80XTaskPostReportToBack(LSM6DSV80XTask *_this, SMMessage *pReport)
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

static sys_error_code_t LSM6DSV80XTaskSensorInit(LSM6DSV80XTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  /* Setup hg_acc, acc and gyro */
  uint8_t reg0 = 0;
  lsm6dsv80x_hg_xl_data_rate_t lsm6dsv80x_hg_xl_data_rate = LSM6DSV80X_HG_XL_ODR_OFF;
  lsm6dsv80x_data_rate_t lsm6dsv80x_xl_data_rate = LSM6DSV80X_ODR_OFF;
  lsm6dsv80x_fifo_xl_batch_t lsm6dsv80x_fifo_xl_batch = LSM6DSV80X_XL_NOT_BATCHED;
  lsm6dsv80x_data_rate_t lsm6dsv80x_gy_data_rate = LSM6DSV80X_ODR_OFF;
  lsm6dsv80x_fifo_gy_batch_t lsm6dsv80x_fifo_gy_batch = LSM6DSV80X_GY_NOT_BATCHED;
  int32_t ret_val = 0;

  uint8_t mlc_int1;
  uint8_t mlc_int2;
  lsm6dsv80x_pin_int_route_t int1_route = {0};

  lsm6dsv80x_reset_t rst;

  /* Restore default configuration */
  ret_val = lsm6dsv80x_reset_set(p_sensor_drv, LSM6DSV80X_RESTORE_CTRL_REGS);
  do
  {
    lsm6dsv80x_reset_get(p_sensor_drv, &rst);
  } while (rst != LSM6DSV80X_READY);

  /* Enable Block Data Update */
  ret_val = lsm6dsv80x_block_data_update_set(p_sensor_drv, PROPERTY_ENABLE);

  ret_val = lsm6dsv80x_device_id_get(p_sensor_drv, (uint8_t *) &reg0);
  if (!ret_val)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, reg0);
  }
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("LSM6DSV80X: sensor - I am 0x%x.\r\n", reg0));

  /* HG ACC fs */
  if (_this->hg_acc_sensor_status.type.mems.fs < 33.0f)
  {
    lsm6dsv80x_hg_xl_full_scale_set(p_sensor_drv, LSM6DSV80X_32g);
  }
  else if (_this->hg_acc_sensor_status.type.mems.fs < 65.0f)
  {
    lsm6dsv80x_hg_xl_full_scale_set(p_sensor_drv, LSM6DSV80X_64g);
  }
  else
  {
    lsm6dsv80x_hg_xl_full_scale_set(p_sensor_drv, LSM6DSV80X_80g);
  }

  /* AXL fs */
  if (_this->acc_sensor_status.type.mems.fs < 3.0f)
  {
    lsm6dsv80x_xl_full_scale_set(p_sensor_drv, LSM6DSV80X_2g);
  }
  else if (_this->acc_sensor_status.type.mems.fs < 5.0f)
  {
    lsm6dsv80x_xl_full_scale_set(p_sensor_drv, LSM6DSV80X_4g);
  }
  else if (_this->acc_sensor_status.type.mems.fs < 9.0f)
  {
    lsm6dsv80x_xl_full_scale_set(p_sensor_drv, LSM6DSV80X_8g);
  }
  else
  {
    lsm6dsv80x_xl_full_scale_set(p_sensor_drv, LSM6DSV80X_16g);
  }

  /* GYRO fs */
  if (_this->gyro_sensor_status.type.mems.fs < 251.0f)
  {
    lsm6dsv80x_gy_full_scale_set(p_sensor_drv, LSM6DSV80X_250dps);
  }
  else if (_this->gyro_sensor_status.type.mems.fs < 501.0f)
  {
    lsm6dsv80x_gy_full_scale_set(p_sensor_drv, LSM6DSV80X_500dps);
  }
  else if (_this->gyro_sensor_status.type.mems.fs < 1001.0f)
  {
    lsm6dsv80x_gy_full_scale_set(p_sensor_drv, LSM6DSV80X_1000dps);
  }
  else if (_this->gyro_sensor_status.type.mems.fs < 2001.0f)
  {
    lsm6dsv80x_gy_full_scale_set(p_sensor_drv, LSM6DSV80X_2000dps);
  }
  else
    lsm6dsv80x_gy_full_scale_set(p_sensor_drv, LSM6DSV80X_4000dps);

  /* High g accel ODR */
  if (_this->hg_acc_sensor_status.type.mems.odr < 481.0f)
  {
    lsm6dsv80x_hg_xl_data_rate = LSM6DSV80X_HG_XL_ODR_AT_480Hz;
  }
  else if (_this->hg_acc_sensor_status.type.mems.odr < 961.0f)
  {
    lsm6dsv80x_hg_xl_data_rate = LSM6DSV80X_HG_XL_ODR_AT_960Hz;
  }
  else if (_this->hg_acc_sensor_status.type.mems.odr < 1921.0f)
  {
    lsm6dsv80x_hg_xl_data_rate = LSM6DSV80X_HG_XL_ODR_AT_1920Hz;
  }
  else if (_this->hg_acc_sensor_status.type.mems.odr < 3841.0f)
  {
    lsm6dsv80x_hg_xl_data_rate = LSM6DSV80X_HG_XL_ODR_AT_3840Hz;
  }
  else
  {
    lsm6dsv80x_hg_xl_data_rate = LSM6DSV80X_HG_XL_ODR_AT_7680Hz;
  }

  if (_this->acc_sensor_status.type.mems.odr < 2.0f)
  {
    lsm6dsv80x_xl_data_rate = LSM6DSV80X_ODR_AT_1Hz875;
    lsm6dsv80x_fifo_xl_batch = LSM6DSV80X_XL_BATCHED_AT_1Hz875;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 8.0f)
  {
    lsm6dsv80x_xl_data_rate = LSM6DSV80X_ODR_AT_7Hz5;
    lsm6dsv80x_fifo_xl_batch = LSM6DSV80X_XL_BATCHED_AT_7Hz5;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 16.0f)
  {
    lsm6dsv80x_xl_data_rate = LSM6DSV80X_ODR_AT_15Hz;
    lsm6dsv80x_fifo_xl_batch = LSM6DSV80X_XL_BATCHED_AT_15Hz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 31.0f)
  {
    lsm6dsv80x_xl_data_rate = LSM6DSV80X_ODR_AT_30Hz;
    lsm6dsv80x_fifo_xl_batch = LSM6DSV80X_XL_BATCHED_AT_30Hz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 61.0f)
  {
    lsm6dsv80x_xl_data_rate = LSM6DSV80X_ODR_AT_60Hz;
    lsm6dsv80x_fifo_xl_batch = LSM6DSV80X_XL_BATCHED_AT_60Hz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 121.0f)
  {
    lsm6dsv80x_xl_data_rate = LSM6DSV80X_ODR_AT_120Hz;
    lsm6dsv80x_fifo_xl_batch = LSM6DSV80X_XL_BATCHED_AT_120Hz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 241.0f)
  {
    lsm6dsv80x_xl_data_rate = LSM6DSV80X_ODR_AT_240Hz;
    lsm6dsv80x_fifo_xl_batch = LSM6DSV80X_XL_BATCHED_AT_240Hz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 481.0f)
  {
    lsm6dsv80x_xl_data_rate = LSM6DSV80X_ODR_AT_480Hz;
    lsm6dsv80x_fifo_xl_batch = LSM6DSV80X_XL_BATCHED_AT_480Hz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 961.0f)
  {
    lsm6dsv80x_xl_data_rate = LSM6DSV80X_ODR_AT_960Hz;
    lsm6dsv80x_fifo_xl_batch = LSM6DSV80X_XL_BATCHED_AT_960Hz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 1921.0f)
  {
    lsm6dsv80x_xl_data_rate = LSM6DSV80X_ODR_AT_1920Hz;
    lsm6dsv80x_fifo_xl_batch = LSM6DSV80X_XL_BATCHED_AT_1920Hz;
  }
  else if (_this->acc_sensor_status.type.mems.odr < 3841.0f)
  {
    lsm6dsv80x_xl_data_rate = LSM6DSV80X_ODR_AT_3840Hz;
    lsm6dsv80x_fifo_xl_batch = LSM6DSV80X_XL_BATCHED_AT_3840Hz;
  }
  else
  {
    lsm6dsv80x_xl_data_rate = LSM6DSV80X_ODR_AT_7680Hz;
    lsm6dsv80x_fifo_xl_batch = LSM6DSV80X_XL_BATCHED_AT_7680Hz;
  }

  if (_this->gyro_sensor_status.type.mems.odr < 8.0f)
  {
    lsm6dsv80x_gy_data_rate = LSM6DSV80X_ODR_AT_7Hz5;
    lsm6dsv80x_fifo_gy_batch = LSM6DSV80X_GY_BATCHED_AT_7Hz5;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 16.0f)
  {
    lsm6dsv80x_gy_data_rate = LSM6DSV80X_ODR_AT_15Hz;
    lsm6dsv80x_fifo_gy_batch = LSM6DSV80X_GY_BATCHED_AT_15Hz;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 31.0f)
  {
    lsm6dsv80x_gy_data_rate = LSM6DSV80X_ODR_AT_30Hz;
    lsm6dsv80x_fifo_gy_batch = LSM6DSV80X_GY_BATCHED_AT_30Hz;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 61.0f)
  {
    lsm6dsv80x_gy_data_rate = LSM6DSV80X_ODR_AT_60Hz;
    lsm6dsv80x_fifo_gy_batch = LSM6DSV80X_GY_BATCHED_AT_60Hz;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 121.0f)
  {
    lsm6dsv80x_gy_data_rate = LSM6DSV80X_ODR_AT_120Hz;
    lsm6dsv80x_fifo_gy_batch = LSM6DSV80X_GY_BATCHED_AT_120Hz;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 241.0f)
  {
    lsm6dsv80x_gy_data_rate = LSM6DSV80X_ODR_AT_240Hz;
    lsm6dsv80x_fifo_gy_batch = LSM6DSV80X_GY_BATCHED_AT_240Hz;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 481.0f)
  {
    lsm6dsv80x_gy_data_rate = LSM6DSV80X_ODR_AT_480Hz;
    lsm6dsv80x_fifo_gy_batch = LSM6DSV80X_GY_BATCHED_AT_480Hz;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 961.0f)
  {
    lsm6dsv80x_gy_data_rate = LSM6DSV80X_ODR_AT_960Hz;
    lsm6dsv80x_fifo_gy_batch = LSM6DSV80X_GY_BATCHED_AT_960Hz;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 1921.0f)
  {
    lsm6dsv80x_gy_data_rate = LSM6DSV80X_ODR_AT_1920Hz;
    lsm6dsv80x_fifo_gy_batch = LSM6DSV80X_GY_BATCHED_AT_1920Hz;
  }
  else if (_this->gyro_sensor_status.type.mems.odr < 3841.0f)
  {
    lsm6dsv80x_gy_data_rate = LSM6DSV80X_ODR_AT_3840Hz;
    lsm6dsv80x_fifo_gy_batch = LSM6DSV80X_GY_BATCHED_AT_3840Hz;
  }
  else
  {
    lsm6dsv80x_gy_data_rate = LSM6DSV80X_ODR_AT_7680Hz;
    lsm6dsv80x_fifo_gy_batch = LSM6DSV80X_GY_BATCHED_AT_7680Hz;
  }

  if (_this->hg_acc_sensor_status.is_active)
  {
    lsm6dsv80x_hg_xl_data_rate_set(p_sensor_drv, lsm6dsv80x_hg_xl_data_rate, 1);
    lsm6dsv80x_fifo_hg_xl_batch_set(p_sensor_drv, 1);
  }
  else
  {
    lsm6dsv80x_hg_xl_data_rate_set(p_sensor_drv, LSM6DSV80X_HG_XL_ODR_OFF, 0);
    lsm6dsv80x_fifo_hg_xl_batch_set(p_sensor_drv, 0);
    _this->hg_acc_sensor_status.is_active = false;
  }

  if (_this->acc_sensor_status.is_active)
  {
    lsm6dsv80x_xl_data_rate_set(p_sensor_drv, lsm6dsv80x_xl_data_rate);
    lsm6dsv80x_fifo_xl_batch_set(p_sensor_drv, lsm6dsv80x_fifo_xl_batch);
  }
  else
  {
    lsm6dsv80x_xl_data_rate_set(p_sensor_drv, LSM6DSV80X_ODR_OFF);
    lsm6dsv80x_fifo_xl_batch_set(p_sensor_drv, LSM6DSV80X_XL_NOT_BATCHED);
    _this->acc_sensor_status.is_active = false;
  }

  if (_this->gyro_sensor_status.is_active)
  {
    lsm6dsv80x_gy_data_rate_set(p_sensor_drv, lsm6dsv80x_gy_data_rate);
    lsm6dsv80x_fifo_gy_batch_set(p_sensor_drv, lsm6dsv80x_fifo_gy_batch);
  }
  else
  {
    lsm6dsv80x_gy_data_rate_set(p_sensor_drv, LSM6DSV80X_ODR_OFF);
    lsm6dsv80x_fifo_gy_batch_set(p_sensor_drv, LSM6DSV80X_GY_NOT_BATCHED);
    _this->gyro_sensor_status.is_active = false;
  }

#if LSM6DSV80X_FIFO_ENABLED
  uint16_t lsm6dsv80x_wtm_level = 0;
  uint16_t lsm6dsv80x_wtm_level_hg_acc = 0;
  uint16_t lsm6dsv80x_wtm_level_acc = 0;
  uint16_t lsm6dsv80x_wtm_level_gyro = 0;

  if (_this->samples_per_it == 0)
  {
    /* Calculation of watermark and samples per int*/
    if (_this->hg_acc_sensor_status.is_active)
    {
      lsm6dsv80x_wtm_level_hg_acc = ((uint16_t) _this->hg_acc_sensor_status.type.mems.odr * (uint16_t) LSM6DSV80X_MAX_DRDY_PERIOD);
    }
    if (_this->acc_sensor_status.is_active)
    {
      lsm6dsv80x_wtm_level_acc = ((uint16_t) _this->acc_sensor_status.type.mems.odr * (uint16_t) LSM6DSV80X_MAX_DRDY_PERIOD);
    }
    if (_this->gyro_sensor_status.is_active)
    {
      lsm6dsv80x_wtm_level_gyro = ((uint16_t) _this->gyro_sensor_status.type.mems.odr * (uint16_t) LSM6DSV80X_MAX_DRDY_PERIOD);
    }

    uint8_t WmMax = 0;
    uint8_t WmMid = 0;
    uint8_t WmMin = 0;
    LSM6DSV80X_SensorSpeedSort(lsm6dsv80x_wtm_level_hg_acc, lsm6dsv80x_wtm_level_acc, lsm6dsv80x_wtm_level_gyro, &WmMax,
                               &WmMid, &WmMin);

    switch (WmMax)
    {
      case 1:
        lsm6dsv80x_wtm_level = lsm6dsv80x_wtm_level_hg_acc;
        break;
      case 2:
        lsm6dsv80x_wtm_level = lsm6dsv80x_wtm_level_acc;
        break;
      case 3:
        lsm6dsv80x_wtm_level = lsm6dsv80x_wtm_level_gyro;
        break;
      default:
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
        return res;
    }

    if (lsm6dsv80x_wtm_level > LSM6DSV80X_MAX_WTM_LEVEL)
    {
      lsm6dsv80x_wtm_level = LSM6DSV80X_MAX_WTM_LEVEL;
    }
    _this->samples_per_it = lsm6dsv80x_wtm_level;
  }

  /* Setup wtm for FIFO */
  lsm6dsv80x_fifo_watermark_set(p_sensor_drv, _this->samples_per_it);

  if (_this->pIRQConfig != NULL)
  {
    int1_route.fifo_th = PROPERTY_ENABLE;
  }
  else
  {
    int1_route.fifo_th = PROPERTY_DISABLE;
  }

  lsm6dsv80x_pin_int1_route_set(p_sensor_drv, &int1_route);

  lsm6dsv80x_fifo_mode_set(p_sensor_drv, LSM6DSV80X_STREAM_MODE);

#else

  uint8_t buff[6];
  lsm6dsv80x_read_reg(p_sensor_drv, LSM6DSV80X_OUTX_L_A, &buff[0], 6);
  lsm6dsv80x_read_reg(p_sensor_drv, LSM6DSV80X_OUTX_L_A, &buff[0], 6);
  lsm6dsv80x_read_reg(p_sensor_drv, LSM6DSV80X_OUTX_L_G, &buff[0], 6);

  _this->samples_per_it = 1;
  if (_this->pIRQConfig != NULL)
  {
    if (!_this->hg_acc_sensor_status.is_active && !_this->acc_sensor_status.is_active
        && !_this->gyro_sensor_status.is_active)
    {
      // all sensors inactive
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      return res;
    }
    else
    {
      if (_this->hg_acc_sensor_status.is_active)
      {
        int1_route.drdy_hg_xl = PROPERTY_ENABLE;
      }
      if (_this->acc_sensor_status.is_active)
      {
        int1_route.drdy_xl = PROPERTY_ENABLE;
      }
      if (_this->gyro_sensor_status.is_active)
      {
        int1_route.drdy_g = PROPERTY_ENABLE;
      }
    }
  }
  else
  {
    int1_route.drdy_hg_xl = PROPERTY_DISABLE;
    int1_route.drdy_xl = PROPERTY_DISABLE;
    int1_route.drdy_g = PROPERTY_DISABLE;
  }
  lsm6dsv80x_pin_int1_route_set(p_sensor_drv, &int1_route);

#endif /* LSM6DSV80X_FIFO_ENABLED */

  /* Check configuration from UCF */
  lsm6dsv80x_mem_bank_set(p_sensor_drv, LSM6DSV80X_EMBED_FUNC_MEM_BANK);
  lsm6dsv80x_read_reg(p_sensor_drv, LSM6DSV80X_MLC_INT1, &mlc_int1, 1);
  lsm6dsv80x_read_reg(p_sensor_drv, LSM6DSV80X_MLC_INT2, &mlc_int2, 1);

  /* MLC must be handled in polling due to HW limitations: disable MLC INT */
  mlc_int1 = 0;
  mlc_int2 = 0;
  lsm6dsv80x_write_reg(p_sensor_drv, LSM6DSV80X_MLC_INT1, &mlc_int1, 1);
  lsm6dsv80x_write_reg(p_sensor_drv, LSM6DSV80X_MLC_INT2, &mlc_int2, 1);
  lsm6dsv80x_mem_bank_set(p_sensor_drv, LSM6DSV80X_MAIN_MEM_BANK);

  if (_this->mlc_enable == false)
  {
    lsm6dsv80x_mlc_set(p_sensor_drv, LSM6DSV80X_MLC_OFF);
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
#if LSM6DSV80X_FIFO_ENABLED
  uint8_t reg[2];
  /* Check FIFO_WTM_IA and fifo level. We do not use PID in order to avoid reading one register twice */
  lsm6dsv80x_read_reg(p_sensor_drv, LSM6DSV80X_FIFO_STATUS1, reg, 2);

  _this->fifo_level = ((reg[1] & 0x03) << 8) + reg[0];

  if (_this->fifo_level >= _this->samples_per_it)
  {
    lsm6dsv80x_read_reg(p_sensor_drv, LSM6DSV80X_FIFO_DATA_OUT_TAG, _this->p_fast_sensor_data_buff,
                        _this->samples_per_it * 7);
  }
#endif

  float_t min_odr = 0;
  min_odr = LSM6DSV80X_FindMin(_this->hg_acc_sensor_status.type.mems.odr, _this->acc_sensor_status.type.mems.odr, _this->gyro_sensor_status.type.mems.odr);

  _this->lsm6dsv80x_task_cfg_timer_period_ms = (uint16_t)(min_odr);
#if LSM6DSV80X_FIFO_ENABLED
  _this->lsm6dsv80x_task_cfg_timer_period_ms = (uint16_t)((1000.0f / _this->lsm6dsv80x_task_cfg_timer_period_ms) * (((float_t)(_this->samples_per_it)) / 2.0f));
#else
  _this->lsm6dsv80x_task_cfg_timer_period_ms = (uint16_t)(1000.0f / _this->lsm6dsv80x_task_cfg_timer_period_ms);
#endif

  return res;
}

static sys_error_code_t LSM6DSV80XTaskSensorReadData(LSM6DSV80XTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t samples_per_it = _this->samples_per_it;

#if LSM6DSV80X_FIFO_ENABLED
  uint8_t reg[2];
  uint16_t i;

  /* Check FIFO_WTM_IA and fifo level. We do not use PID in order to avoid reading one register twice */
  lsm6dsv80x_read_reg(p_sensor_drv, LSM6DSV80X_FIFO_STATUS1, reg, 2);

  _this->fifo_level = ((reg[1] & 0x01) << 8) + reg[0];

  if (((reg[1]) & 0x80) && (_this->fifo_level >= samples_per_it))
  {
    lsm6dsv80x_read_reg(p_sensor_drv, LSM6DSV80X_FIFO_DATA_OUT_TAG, _this->p_fast_sensor_data_buff, samples_per_it * 7);

#if (HSD_USE_DUMMY_DATA == 1)
    int16_t *p16 = (int16_t *)(_this->p_fast_sensor_data_buff);

    for (i = 0; i < samples_per_it; i++)
    {
      p16 = (int16_t *)(&_this->p_fast_sensor_data_buff[i * 7] + 1);
      if ((_this->p_fast_sensor_data_buff[i * 7] >> 3) == LSM6DSV80X_TAG_HG_ACC)
      {
        *p16++ = dummyDataCounter_hg_acc++;
        *p16++ = dummyDataCounter_hg_acc++;
        *p16++ = dummyDataCounter_hg_acc++;
      }
      else if ((_this->p_fast_sensor_data_buff[i * 7] >> 3) == LSM6DSV80X_TAG_ACC)
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
    {
      // assign buffers
      float_t hg_acc_odr = 0;
      float_t acc_odr = 0;
      float_t gyro_odr = 0;
      uint8_t fast_sensor = 0;
      uint8_t medium_sensor = 0;
      uint8_t slow_sensor = 0;
      int16_t *p16_src = (int16_t *) _this->p_fast_sensor_data_buff;
      int16_t *p_hg_acc = NULL, *p_acc = NULL, *p_gyro = NULL;
      if (_this->hg_acc_sensor_status.is_active)
      {
        hg_acc_odr = _this->hg_acc_sensor_status.type.mems.odr;
      }
      if (_this->acc_sensor_status.is_active)
      {
        acc_odr = _this->acc_sensor_status.type.mems.odr;
      }
      if (_this->gyro_sensor_status.is_active)
      {
        gyro_odr = _this->gyro_sensor_status.type.mems.odr;
      }
      LSM6DSV80X_SensorSpeedSort(hg_acc_odr, acc_odr, gyro_odr, &fast_sensor, &medium_sensor, &slow_sensor);
      /* Assign fast buffer */
      switch (fast_sensor)
      {
        case 1:
          p_hg_acc = (int16_t *) _this->p_fast_sensor_data_buff;
          break;
        case 2:
          p_acc = (int16_t *) _this->p_fast_sensor_data_buff;
          break;
        case 3:
          p_gyro = (int16_t *) _this->p_fast_sensor_data_buff;
          break;
      }
      switch (medium_sensor)
      {
        case 1:
          p_hg_acc = (int16_t *) _this->p_medium_sensor_data_buff;
          break;
        case 2:
          p_acc = (int16_t *) _this->p_medium_sensor_data_buff;
          break;
        case 3:
          p_gyro = (int16_t *) _this->p_medium_sensor_data_buff;
          break;
      }
      switch (slow_sensor)
      {
        case 1:
          p_hg_acc = (int16_t *) _this->p_slow_sensor_data_buff;
          break;
        case 2:
          p_acc = (int16_t *) _this->p_slow_sensor_data_buff;
          break;
        case 3:
          p_gyro = (int16_t *) _this->p_slow_sensor_data_buff;
          break;
      }

      _this->hg_acc_samples_count = 0;
      _this->acc_samples_count = 0;
      _this->gyro_samples_count = 0;

      uint8_t *p_tag = (uint8_t *) p16_src;

      for (i = 0; i < samples_per_it; i++)
      {
        if (((*p_tag) >> 3) == LSM6DSV80X_TAG_HG_ACC)
        {
          p16_src = (int16_t *)(p_tag + 1);
          *p_hg_acc++ = *p16_src++;
          *p_hg_acc++ = *p16_src++;
          *p_hg_acc++ = *p16_src++;
          _this->hg_acc_samples_count++;
        }
        else if (((*p_tag) >> 3) == LSM6DSV80X_TAG_ACC)
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
  }
  else
  {
    _this->fifo_level = 0;
    res = SYS_BASE_ERROR_CODE;
  }
#else
  {
    lsm6dsv80x_status_reg_t val = {0};
    /* Read which sensor generated the INT */
    lsm6dsv80x_read_reg(p_sensor_drv, LSM6DSV80X_STATUS_REG, (uint8_t *) &val, 1);
    /* hg_xl data available? */
    if (val.xlhgda == 1U)
    {
      lsm6dsv80x_read_reg(p_sensor_drv, LSM6DSV80X_UI_OUTZ_L_A_HG, _this->p_hg_acc_sample, 6);
#if (HSD_USE_DUMMY_DATA == 1)
      int16_t *p16 = (int16_t *)(_this->p_hg_acc_sample);
      *p16++ = dummyDataCounter_hg_acc++;
      *p16++ = dummyDataCounter_hg_acc++;
      *p16++ = dummyDataCounter_hg_acc++;
#endif
      _this->hg_acc_samples_count = 1U;
      _this->hg_acc_drdy = 1;
    }
    /* xl data available? */
    if (val.xlda == 1U)
    {
      lsm6dsv80x_read_reg(p_sensor_drv, LSM6DSV80X_OUTX_L_A, _this->p_acc_sample, 6);
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
      lsm6dsv80x_read_reg(p_sensor_drv, LSM6DSV80X_OUTX_L_G, _this->p_gyro_sample, 6);
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
  _this->fifo_level = 1;
#endif /* LSM6DSV80X_FIFO_ENABLED */

  return res;
}

static sys_error_code_t LSM6DSV80XTaskSensorReadMLC(LSM6DSV80XTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_BASE_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  lsm6dsv80x_mlc_status_t mlc_status;
  uint8_t mlc_output[4];

  if (_this->mlc_enable)
  {
    lsm6dsv80x_mem_bank_set(p_sensor_drv, LSM6DSV80X_EMBED_FUNC_MEM_BANK);
    lsm6dsv80x_read_reg(p_sensor_drv, LSM6DSV80X_MLC_STATUS, (uint8_t *)(&mlc_status), 1);
    _this->p_mlc_sensor_data_buff[4] = (mlc_status.is_mlc1) | (mlc_status.is_mlc2 << 1) | (mlc_status.is_mlc3 << 2) | (mlc_status.is_mlc4 << 3);

    lsm6dsv80x_read_reg(p_sensor_drv, LSM6DSV80X_MLC1_SRC, (uint8_t *)(&mlc_output[0]), 4);
    if (memcmp(&_this->p_mlc_sensor_data_buff[0], &mlc_output[0], 4) != 0)
    {
      memcpy(&_this->p_mlc_sensor_data_buff[0], &mlc_output[0], 4);
      res = SYS_NO_ERROR_CODE;
    }
    else
    {
      res = SYS_SENSOR_TASK_NO_DRDY_ERROR_CODE;
    }
    lsm6dsv80x_mem_bank_set(p_sensor_drv, LSM6DSV80X_MAIN_MEM_BANK);
  }

  return res;
}

static sys_error_code_t LSM6DSV80XTaskSensorRegister(LSM6DSV80XTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *hg_acc_if = (ISensor_t *) LSM6DSV80XTaskGetHgAccSensorIF(_this);
  ISensor_t *acc_if = (ISensor_t *) LSM6DSV80XTaskGetAccSensorIF(_this);
  ISensor_t *gyro_if = (ISensor_t *) LSM6DSV80XTaskGetGyroSensorIF(_this);
  ISensor_t *mlc_if = (ISensor_t *) LSM6DSV80XTaskGetMlcSensorIF(_this);

  _this->hg_acc_id = SMAddSensor(hg_acc_if);
  _this->acc_id = SMAddSensor(acc_if);
  _this->gyro_id = SMAddSensor(gyro_if);
  _this->mlc_id = SMAddSensor(mlc_if);

  return res;
}

static sys_error_code_t LSM6DSV80XTaskSensorInitTaskParams(LSM6DSV80XTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* HIGH G ACCELEROMETER STATUS */
  _this->hg_acc_sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->hg_acc_sensor_status.is_active = TRUE;
  _this->hg_acc_sensor_status.type.mems.fs = 80.0f;
  _this->hg_acc_sensor_status.type.mems.sensitivity = 0.0000305f * _this->hg_acc_sensor_status.type.mems.fs;
  _this->hg_acc_sensor_status.type.mems.odr = 7680.0f;
  _this->hg_acc_sensor_status.type.mems.measured_odr = 0.0f;
#if LSM6DSV80X_FIFO_ENABLED
  EMD_Init(&_this->data_hg_acc, _this->p_fast_sensor_data_buff, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);
#else
  EMD_Init(&_this->data_hg_acc, _this->p_hg_acc_sample, E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 3);
#endif

  /* ACCELEROMETER STATUS */
  _this->acc_sensor_status.isensor_class = ISENSOR_CLASS_MEMS;
  _this->acc_sensor_status.is_active = TRUE;
  _this->acc_sensor_status.type.mems.fs = 16.0f;
  _this->acc_sensor_status.type.mems.sensitivity = 0.0000305f * _this->acc_sensor_status.type.mems.fs;
  _this->acc_sensor_status.type.mems.odr = 7680.0f;
  _this->acc_sensor_status.type.mems.measured_odr = 0.0f;
#if LSM6DSV80X_FIFO_ENABLED
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
#if LSM6DSV80X_FIFO_ENABLED
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

static sys_error_code_t LSM6DSV80XTaskSensorSetODR(LSM6DSV80XTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float_t odr = (float_t) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->hg_acc_id)
  {
    if (odr < 1.0f)
    {
      lsm6dsv80x_hg_xl_data_rate_set(p_sensor_drv, LSM6DSV80X_HG_XL_ODR_OFF, 0);
      /* Do not update the model in case of odr = 0 */
      odr = _this->hg_acc_sensor_status.type.mems.odr;
    }
    else
    {
      /* Changing odr must disable MLC sensor: MLC can work properly only when setup from UCF */
      _this->mlc_enable = FALSE;
      _this->mlc_sensor_status.is_active = FALSE;

      if (odr < 481.0f)
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
      _this->hg_acc_sensor_status.type.mems.odr = odr;
      _this->hg_acc_sensor_status.type.mems.measured_odr = 0.0f;
    }
  }
  else if (id == _this->acc_id)
  {
    if (odr < 1.0f)
    {
      lsm6dsv80x_xl_data_rate_set(p_sensor_drv, LSM6DSV80X_ODR_OFF);
      /* Do not update the model in case of odr = 0 */
      odr = _this->acc_sensor_status.type.mems.odr;
    }
    else
    {
      /* Changing odr must disable MLC sensor: MLC can work properly only when setup from UCF */
      _this->mlc_enable = FALSE;
      _this->mlc_sensor_status.is_active = FALSE;

      if (odr < 2.0f)
      {
        odr = 1.875f;
      }
      else if (odr < 8.0f)
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
      lsm6dsv80x_gy_data_rate_set(p_sensor_drv, LSM6DSV80X_ODR_OFF);
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

    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV80X: warning - MLC SetODR() not supported.\r\n"));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  /* when odr changes the samples_per_it must be recalculated */
  _this->samples_per_it = 0;

  return res;
}

static sys_error_code_t LSM6DSV80XTaskSensorSetFS(LSM6DSV80XTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float_t fs = (float_t) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  /* Changing fs must disable MLC sensor: MLC can work properly only when setup from UCF */
  _this->mlc_enable = FALSE;
  _this->mlc_sensor_status.is_active = FALSE;

  if (id == _this->hg_acc_id)
  {
    if (fs < 33.0f)
    {
      lsm6dsv80x_hg_xl_full_scale_set(p_sensor_drv, LSM6DSV80X_32g);
      fs = 32.0f;
    }
    else if (fs < 65.0f)
    {
      lsm6dsv80x_hg_xl_full_scale_set(p_sensor_drv, LSM6DSV80X_64g);
      fs = 64.0f;
    }
    else
    {
      lsm6dsv80x_hg_xl_full_scale_set(p_sensor_drv, LSM6DSV80X_80g);
      fs = 80.0f;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->hg_acc_sensor_status.type.mems.fs = fs;
      _this->hg_acc_sensor_status.type.mems.sensitivity = 0.0000305f * _this->hg_acc_sensor_status.type.mems.fs;
    }
  }
  else if (id == _this->acc_id)
  {
    if (fs < 3.0f)
    {
      lsm6dsv80x_xl_full_scale_set(p_sensor_drv, LSM6DSV80X_2g);
      fs = 2.0f;
    }
    else if (fs < 5.0f)
    {
      lsm6dsv80x_xl_full_scale_set(p_sensor_drv, LSM6DSV80X_4g);
      fs = 4.0f;
    }
    else if (fs < 9.0f)
    {
      lsm6dsv80x_xl_full_scale_set(p_sensor_drv, LSM6DSV80X_8g);
      fs = 8.0f;
    }
    else
    {
      lsm6dsv80x_xl_full_scale_set(p_sensor_drv, LSM6DSV80X_16g);
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
    if (fs < 251.0f)
    {
      lsm6dsv80x_gy_full_scale_set(p_sensor_drv, LSM6DSV80X_250dps);
      fs = 250.0f;
    }
    else if (fs < 501.0f)
    {
      lsm6dsv80x_gy_full_scale_set(p_sensor_drv, LSM6DSV80X_500dps);
      fs = 500.0f;
    }
    else if (fs < 1001.0f)
    {
      lsm6dsv80x_gy_full_scale_set(p_sensor_drv, LSM6DSV80X_1000dps);
      fs = 1000.0f;
    }
    else if (fs < 2001.0f)
    {
      lsm6dsv80x_gy_full_scale_set(p_sensor_drv, LSM6DSV80X_2000dps);
      fs = 2000.0f;
    }
    else
    {
      lsm6dsv80x_gy_full_scale_set(p_sensor_drv, LSM6DSV80X_4000dps);
      fs = 4000.0f;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->gyro_sensor_status.type.mems.fs = fs;
      _this->gyro_sensor_status.type.mems.sensitivity = 0.000035f * _this->gyro_sensor_status.type.mems.fs;
    }
  }
  else if (id == _this->mlc_id)
  {
    res = SYS_TASK_INVALID_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("LSM6DSV80X: warning - MLC SetFS() not supported.\r\n"));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t LSM6DSV80XTaskSensorSetFifoWM(LSM6DSV80XTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (report.sensorMessage.nSensorId == _this->mlc_id)
  {
    res = SYS_TASK_INVALID_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);

    SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("LSM6DSV80X: warning - MLC SetFifoWM() not supported.\r\n"));
  }

#if LSM6DSV80X_FIFO_ENABLED

  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t lsm6dsv80x_wtm_level = (uint16_t)report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if ((id == _this->acc_id) || (id == _this->gyro_id) || (id == _this->hg_acc_id))
  {
    /* acc and gyro share the FIFO, so size should be increased w.r.t. previous setup */
    lsm6dsv80x_wtm_level = lsm6dsv80x_wtm_level + _this->samples_per_it;

    if (lsm6dsv80x_wtm_level > LSM6DSV80X_MAX_WTM_LEVEL)
    {
      lsm6dsv80x_wtm_level = LSM6DSV80X_MAX_WTM_LEVEL;
    }
    _this->samples_per_it = lsm6dsv80x_wtm_level;

    /* Setup wtm for FIFO */
    lsm6dsv80x_fifo_watermark_set(p_sensor_drv, _this->samples_per_it);

    lsm6dsv80x_fifo_mode_set(p_sensor_drv, LSM6DSV80X_STREAM_MODE);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }
#endif

  return res;
}

static sys_error_code_t LSM6DSV80XTaskSensorEnable(LSM6DSV80XTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->hg_acc_id)
  {
    _this->hg_acc_sensor_status.is_active = TRUE;

    /* Changing sensor configuration must disable MLC sensor: MLC can work properly only when setup from UCF */
    _this->mlc_enable = FALSE;
    _this->mlc_sensor_status.is_active = FALSE;
  }
  else if (id == _this->acc_id)
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

static sys_error_code_t LSM6DSV80XTaskSensorDisable(LSM6DSV80XTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->hg_acc_id)
  {
    _this->hg_acc_sensor_status.is_active = FALSE;
    lsm6dsv80x_hg_xl_data_rate_set(p_sensor_drv, LSM6DSV80X_HG_XL_ODR_OFF, 0);

    /* Changing sensor configuration must disable MLC sensor: MLC can work properly only when setup from UCF */
    _this->mlc_enable = FALSE;
    _this->mlc_sensor_status.is_active = FALSE;
  }
  else if (id == _this->acc_id)
  {
    _this->acc_sensor_status.is_active = FALSE;
    lsm6dsv80x_xl_data_rate_set(p_sensor_drv, LSM6DSV80X_ODR_OFF);

    /* Changing sensor configuration must disable MLC sensor: MLC can work properly only when setup from UCF */
    _this->mlc_enable = FALSE;
    _this->mlc_sensor_status.is_active = FALSE;
  }
  else if (id == _this->gyro_id)
  {
    _this->gyro_sensor_status.is_active = FALSE;
    lsm6dsv80x_gy_data_rate_set(p_sensor_drv, LSM6DSV80X_ODR_OFF);

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

static boolean_t LSM6DSV80XTaskSensorIsActive(const LSM6DSV80XTask *_this)
{
  assert_param(_this != NULL);
  return (_this->hg_acc_sensor_status.is_active || _this->acc_sensor_status.is_active
          || _this->gyro_sensor_status.is_active);
}

static sys_error_code_t LSM6DSV80XTaskEnterLowPowerMode(const LSM6DSV80XTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  lsm6dsv80x_hg_xl_data_rate_t lsm6dsv80x_hg_xl_data_rate = LSM6DSV80X_HG_XL_ODR_OFF;
  uint8_t lsm6dsv80x_fifo_hg_xl_batch = 0;
  lsm6dsv80x_data_rate_t lsm6dsv80x_xl_data_rate = LSM6DSV80X_ODR_OFF;
  lsm6dsv80x_fifo_xl_batch_t lsm6dsv80x_fifo_xl_batch = LSM6DSV80X_XL_NOT_BATCHED;
  lsm6dsv80x_data_rate_t lsm6dsv80x_gy_data_rate = LSM6DSV80X_ODR_OFF;
  lsm6dsv80x_fifo_gy_batch_t lsm6dsv80x_fifo_gy_batch = LSM6DSV80X_GY_NOT_BATCHED;

  lsm6dsv80x_hg_xl_data_rate_set(p_sensor_drv, lsm6dsv80x_hg_xl_data_rate, 0);
  lsm6dsv80x_fifo_hg_xl_batch_set(p_sensor_drv, lsm6dsv80x_fifo_hg_xl_batch);
  lsm6dsv80x_xl_data_rate_set(p_sensor_drv, lsm6dsv80x_xl_data_rate);
  lsm6dsv80x_fifo_xl_batch_set(p_sensor_drv, lsm6dsv80x_fifo_xl_batch);
  lsm6dsv80x_gy_data_rate_set(p_sensor_drv, lsm6dsv80x_gy_data_rate);
  lsm6dsv80x_fifo_gy_batch_set(p_sensor_drv, lsm6dsv80x_fifo_gy_batch);

  return res;
}

static sys_error_code_t LSM6DSV80XTaskConfigureIrqPin(const LSM6DSV80XTask *_this, boolean_t LowPower)
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

static sys_error_code_t LSM6DSV80XTaskConfigureMLCPin(const LSM6DSV80XTask *_this, boolean_t LowPower)
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

static inline LSM6DSV80XTask *LSM6DSV80XTaskGetOwnerFromISensorIF(ISensor_t *p_if)
{
  assert_param(p_if != NULL);
  LSM6DSV80XTask *p_if_owner = NULL;

  /* check if the virtual function has been called from the mlc sensor IF  */
  p_if_owner = (LSM6DSV80XTask *)((uint32_t) p_if - offsetof(LSM6DSV80XTask, mlc_sensor_if));
  if (!(p_if_owner->gyro_sensor_if.vptr == &sTheClass.gyro_sensor_if_vtbl)
      || !(p_if_owner->super.vptr == &sTheClass.vtbl))
  {
    /* then the virtual function has been called from the gyro IF  */
    p_if_owner = (LSM6DSV80XTask *)((uint32_t) p_if - offsetof(LSM6DSV80XTask, gyro_sensor_if));
  }
  if (!(p_if_owner->acc_sensor_if.vptr == &sTheClass.acc_sensor_if_vtbl) || !(p_if_owner->super.vptr == &sTheClass.vtbl))
  {
    /* then the virtual function has been called from the acc IF  */
    p_if_owner = (LSM6DSV80XTask *)((uint32_t) p_if - offsetof(LSM6DSV80XTask, acc_sensor_if));
  }
  if (!(p_if_owner->hg_acc_sensor_if.vptr == &sTheClass.hg_acc_sensor_if_vtbl)
      || !(p_if_owner->super.vptr == &sTheClass.vtbl))
  {
    /* then the virtual function has been called from the high-g acc IF  */
    p_if_owner = (LSM6DSV80XTask *)((uint32_t) p_if - offsetof(LSM6DSV80XTask, hg_acc_sensor_if));
  }
  return p_if_owner;
}

static inline LSM6DSV80XTask *LSM6DSV80XTaskGetOwnerFromISensorLLIF(ISensorLL_t *p_if)
{
  assert_param(p_if != NULL);
  LSM6DSV80XTask *p_if_owner = NULL;
  p_if_owner = (LSM6DSV80XTask *)((uint32_t) p_if - offsetof(LSM6DSV80XTask, sensor_ll_if));

  return p_if_owner;
}

static void LSM6DSV80XTaskTimerCallbackFunction(ULONG param)
{
  LSM6DSV80XTask *p_obj = (LSM6DSV80XTask *) param;
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

static void LSM6DSV80XTaskMLCTimerCallbackFunction(ULONG param)
{
  LSM6DSV80XTask *p_obj = (LSM6DSV80XTask *) param;
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

void LSM6DSV80XTask_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((LSM6DSV80XTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}

void INT2_DSV80X_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY_MLC;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((LSM6DSV80XTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}

static sys_error_code_t LSM6DSV80X_ODR_Sync(LSM6DSV80XTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  float_t odr = 0.0f;
  lsm6dsv80x_hg_xl_data_rate_t lsm6dsv80x_odr_hg_xl;
  uint8_t lsm6dsv80x_hg_xl_reg_out_en;
  if (lsm6dsv80x_hg_xl_data_rate_get(p_sensor_drv, &lsm6dsv80x_odr_hg_xl, &lsm6dsv80x_hg_xl_reg_out_en) == 0)
  {
    _this->hg_acc_sensor_status.is_active = TRUE;

    /* Update only the model */
    switch (lsm6dsv80x_odr_hg_xl)
    {
      case LSM6DSV80X_HG_XL_ODR_OFF:
        _this->hg_acc_sensor_status.is_active = FALSE;
        /* Do not update the model in case of odr = 0 */
        odr = _this->hg_acc_sensor_status.type.mems.odr;
        break;
      case LSM6DSV80X_HG_XL_ODR_AT_480Hz:
        odr = 480.0f;
        break;
      case LSM6DSV80X_HG_XL_ODR_AT_960Hz:
        odr = 960.0f;
        break;
      case LSM6DSV80X_HG_XL_ODR_AT_1920Hz:
        odr = 1920.0f;
        break;
      case LSM6DSV80X_HG_XL_ODR_AT_3840Hz:
        odr = 3840.0f;
        break;
      case LSM6DSV80X_HG_XL_ODR_AT_7680Hz:
        odr = 7680.0f;
        break;
      default:
        break;
    }
    _this->hg_acc_sensor_status.type.mems.odr = odr;
    _this->hg_acc_sensor_status.type.mems.measured_odr = 0.0f;

  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }

  odr = 0;
  lsm6dsv80x_data_rate_t lsm6dsv80x_odr_xl;
  if (lsm6dsv80x_xl_data_rate_get(p_sensor_drv, &lsm6dsv80x_odr_xl) == 0)
  {
    _this->acc_sensor_status.is_active = TRUE;

    /* Update only the model */
    switch (lsm6dsv80x_odr_xl)
    {
      case LSM6DSV80X_ODR_OFF:
        _this->acc_sensor_status.is_active = FALSE;
        /* Do not update the model in case of odr = 0 */
        odr = _this->acc_sensor_status.type.mems.odr;
        break;
      case LSM6DSV80X_ODR_AT_1Hz875:
        odr = 1.875f;
        break;
      case LSM6DSV80X_ODR_AT_7Hz5:
        odr = 7.5f;
        break;
      case LSM6DSV80X_ODR_AT_15Hz:
        odr = 15.0f;
        break;
      case LSM6DSV80X_ODR_AT_30Hz:
        odr = 30.0f;
        break;
      case LSM6DSV80X_ODR_AT_60Hz:
        odr = 60.0f;
        break;
      case LSM6DSV80X_ODR_AT_120Hz:
        odr = 120.0f;
        break;
      case LSM6DSV80X_ODR_AT_240Hz:
        odr = 240.0f;
        break;
      case LSM6DSV80X_ODR_AT_480Hz:
        odr = 480.0f;
        break;
      case LSM6DSV80X_ODR_AT_960Hz:
        odr = 960.0f;
        break;
      case LSM6DSV80X_ODR_AT_1920Hz:
        odr = 1920.0f;
        break;
      case LSM6DSV80X_ODR_AT_3840Hz:
        odr = 3840.0f;
        break;
      case LSM6DSV80X_ODR_AT_7680Hz:
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
  lsm6dsv80x_data_rate_t lsm6dsv80x_odr_g;
  if (lsm6dsv80x_gy_data_rate_get(p_sensor_drv, &lsm6dsv80x_odr_g) == 0)
  {
    _this->gyro_sensor_status.is_active = TRUE;

    /* Update only the model */
    switch (lsm6dsv80x_odr_g)
    {
      case LSM6DSV80X_ODR_OFF:
        _this->gyro_sensor_status.is_active = FALSE;
        /* Do not update the model in case of odr = 0 */
        odr = _this->gyro_sensor_status.type.mems.odr;
        break;
      case LSM6DSV80X_ODR_AT_7Hz5:
        odr = 7.5f;
        break;
      case LSM6DSV80X_ODR_AT_15Hz:
        odr = 15.0f;
        break;
      case LSM6DSV80X_ODR_AT_30Hz:
        odr = 30.0f;
        break;
      case LSM6DSV80X_ODR_AT_60Hz:
        odr = 60.0f;
        break;
      case LSM6DSV80X_ODR_AT_120Hz:
        odr = 120.0f;
        break;
      case LSM6DSV80X_ODR_AT_240Hz:
        odr = 240.0f;
        break;
      case LSM6DSV80X_ODR_AT_480Hz:
        odr = 480.0f;
        break;
      case LSM6DSV80X_ODR_AT_960Hz:
        odr = 960.0f;
        break;
      case LSM6DSV80X_ODR_AT_1920Hz:
        odr = 1920.0f;
        break;
      case LSM6DSV80X_ODR_AT_3840Hz:
        odr = 3840.0f;
        break;
      case LSM6DSV80X_ODR_AT_7680Hz:
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

static sys_error_code_t LSM6DSV80X_FS_Sync(LSM6DSV80XTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  float_t fs = 32.0;
  lsm6dsv80x_hg_xl_full_scale_t fs_hg_xl;
  if (lsm6dsv80x_hg_xl_full_scale_get(p_sensor_drv, &fs_hg_xl) == 0)
  {
    switch (fs_hg_xl)
    {
      case LSM6DSV80X_32g:
        fs = 32.0;
        break;
      case LSM6DSV80X_64g:
        fs = 64.0;
        break;
      case LSM6DSV80X_80g:
        fs = 80.0;
        break;
      default:
        break;
    }
    _this->hg_acc_sensor_status.type.mems.fs = fs;
    _this->hg_acc_sensor_status.type.mems.sensitivity = 0.0000305f * _this->hg_acc_sensor_status.type.mems.fs;
  }
  else
  {
    res = SYS_BASE_ERROR_CODE;
  }

  fs = 2.0;
  lsm6dsv80x_xl_full_scale_t fs_xl;
  if (lsm6dsv80x_xl_full_scale_get(p_sensor_drv, &fs_xl) == 0)
  {
    switch (fs_xl)
    {
      case LSM6DSV80X_2g:
        fs = 2.0;
        break;
      case LSM6DSV80X_4g:
        fs = 4.0;
        break;
      case LSM6DSV80X_8g:
        fs = 8.0;
        break;
      case LSM6DSV80X_16g:
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
  lsm6dsv80x_gy_full_scale_t fs_g;
  if (lsm6dsv80x_gy_full_scale_get(p_sensor_drv, &fs_g) == 0)
  {
    switch (fs_g)
    {
      case LSM6DSV80X_250dps:
        fs = 250;
        break;
      case LSM6DSV80X_500dps:
        fs = 500;
        break;
      case LSM6DSV80X_1000dps:
        fs = 1000;
        break;
      case LSM6DSV80X_2000dps:
        fs = 2000;
        break;
      case LSM6DSV80X_4000dps:
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

static float_t LSM6DSV80X_FindMax(float_t Val1,
                                  float_t Val2,
                                  float_t Val3)
{
  float_t Max = Val1;
  if (Val2 > Max)
  {
    Max = Val2;
  }
  if (Val3 > Max)
  {
    Max = Val3;
  }
  return Max;
}

/* Return Min of 3 non negative floats */
static float_t LSM6DSV80X_FindMin(float_t Val1,
                                  float_t Val2,
                                  float_t Val3)
{
  float_t Min = 0;
  if (Val1 >= 0)
  {
    Min = Val1;
    if (Val2 >= 0 && Min > Val2)
    {
      Min = Val2;
    }
    if (Val3 >= 0 && Min > Val3)
    {
      Min = Val3;
    }
  }
  else if (Val2 >= 0)
  {
    Min = Val2;
    if (Val3 >= 0 && Min > Val3)
    {
      Min = Val3;
    }
  }
  else
  {
    Min = Val3;
  }
  return Min;
}

static sys_error_code_t LSM6DSV80X_SensorSpeedSort(float_t ODR1,
                                                   float_t ODR2,
                                                   float_t ODR3,
                                                   uint8_t *FastSensor,
                                                   uint8_t *MediumSensor,
                                                   uint8_t *SlowSensor)
{
  // initial assignment
  *FastSensor = 1;
  float_t FastODR = ODR1;
  *MediumSensor = 2;
  float_t MediumODR = ODR2;
  *SlowSensor = 3;
  float_t SlowODR = ODR3;
  // temporary var for swap
  uint8_t tempSensor = 0;
  float_t tempODR = 0;

  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (FastODR < SlowODR)
  {
    // swap FastODR and SlowODR
    tempODR = FastODR;
    tempSensor = *FastSensor;
    FastODR = SlowODR;
    *FastSensor = *SlowSensor;
    SlowODR = tempODR;
    *SlowSensor = tempSensor;
  }

  if (FastODR < MediumODR)
  {
    // swap FastODR and MediumODR
    tempODR = FastODR;
    tempSensor = *FastSensor;
    FastODR = MediumODR;
    *FastSensor = *MediumSensor;
    MediumODR = tempODR;
    *MediumSensor = tempSensor;
  }

  if (MediumODR < SlowODR)
  {
    // swap MediumODR and SlowODR
    tempODR = MediumODR;
    tempSensor = *MediumSensor;
    MediumODR = SlowODR;
    *MediumSensor = *SlowSensor;
    SlowODR = tempODR;
    *SlowSensor = tempSensor;
  }
  return res;
}
