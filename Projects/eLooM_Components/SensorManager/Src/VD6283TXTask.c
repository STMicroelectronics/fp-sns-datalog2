/**
  ******************************************************************************
  * @file    VD6283TX.c
  * @author
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
#include "VD6283TXTask.h"
#include "VD6283TXTask_vtbl.h"
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
#include "drivers/platform.h"

/* Private includes ----------------------------------------------------------*/

#ifndef VD6283TX_TASK_CFG_STACK_DEPTH
#define VD6283TX_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*8)
#endif

#ifndef VD6283TX_TASK_CFG_PRIORITY
#define VD6283TX_TASK_CFG_PRIORITY                 (4)
#endif

#ifndef VD6283TX_TASK_CFG_IN_QUEUE_LENGTH
#define VD6283TX_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define VD6283TX_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef VD6283TX_TASK_CFG_TIMER_PERIOD_MS
#define VD6283TX_TASK_CFG_TIMER_PERIOD_MS          500
#endif

#ifndef VD6283TX_TASK_CFG_MAX_INSTANCES_COUNT
#define VD6283TX_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_VD6283TX, level, message)

#define VD6283TX_I2C_ADD                          ( 0x40U )

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint32_t dummyDataCounter = 0;
#endif

static const uint16_t GainRange[] =
{
  0x42AB,   /*   66, 67 */
  0x3200,   /*   50, 00 */
  0x2154,   /*   33, 33 */
  0x1900,   /*   25, 00 */
  0x10AB,   /*   16, 67 */
  0x0A00,   /*   10, 00 */
  0x0723,   /*    7, 14 */
  0x0500,   /*    5, 00 */
  0x0354,   /*    3, 33 */
  0x0280,   /*    2, 50 */
  0x01AB,   /*    1, 67 */
  0x0140,   /*    1, 25 */
  0x0100,   /*    1, 00 */
  0x00D4,   /*    0, 83 */
  0x00B5    /*    0, 71 */
};

/**
  * Class object declaration
  */
typedef struct _VD6283TXTaskClass
{
  /**
    * VD6283TXTask class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * light IF virtual table.
    */
  const ISensorLight_vtbl sensor_if_vtbl;

  /**
    * Specifies presence sensor capabilities.
    */
  const SensorDescriptor_t class_descriptor;

  /**
    * VD6283TXTask (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[VD6283TX_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} VD6283TXTaskClass_t;

/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t VD6283TXTaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t VD6283TXTaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t VD6283TXTaskSensorInit(VD6283TXTask *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t VD6283TXTaskSensorReadData(VD6283TXTask *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t VD6283TXTaskSensorRegister(VD6283TXTask *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t VD6283TXTaskSensorInitTaskParams(VD6283TXTask *_this);

/**
  * Private implementation of sensor interface methods for VD6283TX sensor
  */
static sys_error_code_t VD6283TXTaskSensorSetIntermeasurementTime(VD6283TXTask *_this, SMMessage report);
static sys_error_code_t VD6283TXTaskSensorSetExposureTime(VD6283TXTask *_this, SMMessage report);
static sys_error_code_t VD6283TXTaskSensorSetLightGain(VD6283TXTask *_this, SMMessage report);
static sys_error_code_t VD6283TXTaskSensorEnable(VD6283TXTask *_this, SMMessage report);
static sys_error_code_t VD6283TXTaskSensorDisable(VD6283TXTask *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t VD6283TXTaskSensorIsActive(const VD6283TXTask *_this);

static sys_error_code_t VD6283TXTaskEnterLowPowerMode(const VD6283TXTask *_this);

static sys_error_code_t VD6283TXTaskConfigureIrqPin(const VD6283TXTask *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void VD6283TXTaskTimerCallbackFunction(ULONG param);


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
static inline sys_error_code_t VD6283TXTaskPostReportToFront(VD6283TXTask *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t VD6283TXTaskPostReportToBack(VD6283TXTask *_this, SMMessage *pReport);

/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
// static VD6283TXTask sTaskObj;

/**
  * The class object.
  */
static VD6283TXTaskClass_t sTheClass =
{
  /* Class virtual table */
  {
    VD6283TXTask_vtblHardwareInit,
    VD6283TXTask_vtblOnCreateTask,
    VD6283TXTask_vtblDoEnterPowerMode,
    VD6283TXTask_vtblHandleError,
    VD6283TXTask_vtblOnEnterTaskControlLoop,
    VD6283TXTask_vtblForceExecuteStep,
    VD6283TXTask_vtblOnEnterPowerMode
  },
  /* class::sensor_if_vtbl virtual table */
  {
    {
      {
        VD6283TXTask_vtblLightGetId,
        VD6283TXTask_vtblLightGetEventSourceIF,
        VD6283TXTask_vtblLightGetDataInfo
      },

      VD6283TXTask_vtblSensorEnable,
      VD6283TXTask_vtblSensorDisable,
      VD6283TXTask_vtblSensorIsEnabled,
      VD6283TXTask_vtblSensorGetDescription,
      VD6283TXTask_vtblSensorGetStatus,
      VD6283TXTask_vtblSensorGetStatusPointer
    },

    VD6283TXTask_vtblLightGetIntermeasurementTime,
    VD6283TXTask_vtblLightGetExposureTime,
    VD6283TXTask_vtblLightGetLightGain,
    VD6283TXTask_vtblSensorSetIntermeasurementTime,
    VD6283TXTask_vtblSensorSetExposureTime,
    VD6283TXTask_vtblSensorSetLightGain
  },
  /* PRESENCE DESCRIPTOR */
  {
    "vd6283tx",
    COM_TYPE_ALS
  },
  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    VD6283TXTaskExecuteStepState1,
    NULL,
    VD6283TXTaskExecuteStepDatalog,
  },
  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *VD6283TXTaskGetLightSensorIF(VD6283TXTask *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *VD6283TXTaskAlloc(const void *pIRQConfig, const void *pCSConfig, const void *pBSConfig)
{
  VD6283TXTask *p_new_obj = SysAlloc(sizeof(VD6283TXTask));

  if (p_new_obj != NULL)
  {
    /* Initialize the super class */
    AMTInitEx(&p_new_obj->super);

    p_new_obj->super.vptr = &sTheClass.vtbl;
    p_new_obj->sensor_if.vptr = &sTheClass.sensor_if_vtbl;
    p_new_obj->sensor_descriptor = &sTheClass.class_descriptor;

    p_new_obj->pIRQConfig = (MX_GPIOParams_t *) pIRQConfig;
    p_new_obj->pCSConfig = (MX_GPIOParams_t *) pCSConfig;
    p_new_obj->pBSConfig = (MX_GPIOParams_t *) pBSConfig;

    strcpy(p_new_obj->sensor_status.p_name, sTheClass.class_descriptor.p_name);
  }
  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *VD6283TXTaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const void *pBSConfig,
                                         const char *p_name)
{
  VD6283TXTask *p_new_obj = (VD6283TXTask *)VD6283TXTaskAlloc(pIRQConfig, pCSConfig, pBSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *VD6283TXTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                        const void *pBSConfig)
{
  VD6283TXTask *p_obj = (VD6283TXTask *)p_mem_block;

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
    p_obj->pBSConfig = (MX_GPIOParams_t *) pBSConfig;
  }

  return (AManagedTaskEx *)p_obj;
}

AManagedTaskEx *VD6283TXTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                               const void *pBSConfig,
                                               const char *p_name)
{
  VD6283TXTask *p_obj = (VD6283TXTask *)VD6283TXTaskStaticAlloc(p_mem_block, pIRQConfig, pCSConfig, pBSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *VD6283TXTaskGetSensorIF(VD6283TXTask *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *VD6283TXTaskGetEventSrcIF(VD6283TXTask *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t VD6283TXTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  VD6283TXTask *p_obj = (VD6283TXTask *) _this;

  /* Configure CS Pin */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->pCSConfig->p_mx_init_f();
  }
  else if (p_obj->pBSConfig != NULL)
  {
    p_obj->pBSConfig->p_mx_init_f();
  }
  else
  {
    /**/
  }

  return res;
}

sys_error_code_t VD6283TXTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                               VOID **pvStackStart,
                                               ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                               ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  VD6283TXTask *p_obj = (VD6283TXTask *) _this;

  /* Create task specific sw resources */

  uint32_t item_size = (uint32_t) VD6283TX_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(VD6283TX_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "VD6283TX_Q", item_size / 4u, p_queue_items_buff,
                                    VD6283TX_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_timer, "VD6283TX_T", VD6283TXTaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(VD6283TX_TASK_CFG_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(VD6283TX_DEVICE_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
    if (p_obj->p_sensor_bus_if == NULL)
    {
      res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    }
  }
  else
  {
    if (p_obj->pBSConfig != NULL)
    {
      p_obj->p_sensor_bus_if = I2CBSBusIFAlloc(VD6283TX_DEVICE_ID, VD6283TX_I2C_ADD, p_obj->pBSConfig->port, (uint16_t) p_obj->pBSConfig->pin, 0);
      if (p_obj->p_sensor_bus_if == NULL)
      {
        res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
      }
    }
    else
    {
      p_obj->p_sensor_bus_if = I2CBusIFAlloc(VD6283TX_DEVICE_ID, VD6283TX_I2C_ADD, 0);
      if (p_obj->p_sensor_bus_if == NULL)
      {
        res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
      }
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
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, VD6283TX_TASK_CFG_MAX_INSTANCES_COUNT);
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
  p_obj->id = 0;
  p_obj->prev_timestamp = 0.0f;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "VD6283TX";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = VD6283TX_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = VD6283TX_TASK_CFG_PRIORITY;
  *pPreemptThreshold = VD6283TX_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = VD6283TXTaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  res = VD6283TXTaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("VD6283TX: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t VD6283TXTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  VD6283TXTask *p_obj = (VD6283TXTask *) _this;
  stmdev_ctx_t *p_sensor_drv_gen = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;
  VD6283TX_Object_t *p_platform_drv = (VD6283TX_Object_t *) &p_obj->als_driver_if;
  p_platform_drv->IO = *p_sensor_drv_gen;
  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (VD6283TXTaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("VD6283TX: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      if (VD6283TXTaskSensorIsActive(p_obj))
      {
        VD6283TX_Stop(p_platform_drv);
      }
      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_timer);
      }
      else
      {
        VD6283TXTaskConfigureIrqPin(p_obj, TRUE);
      }
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("VD6283TX: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = VD6283TXTaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      sys_error_handler();
    }
    if (p_obj->pIRQConfig != NULL)
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("VD6283TX - Enter Low Power Mode failed.\r\n"));
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("VD6283TX: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t VD6283TXTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  VD6283TXTask *p_obj = (VD6283TXTask*)_this;

  return res;
}

sys_error_code_t VD6283TXTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("VD6283TX: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (VD6283TX_TASK_CFG_TAG)
  VD6283TXTask *p_obj = (VD6283TXTask *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = VD6283TX_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  stmdev_ctx_t *p_sensor_drv_gen = (stmdev_ctx_t *) & ((VD6283TXTask *)_this)->p_sensor_bus_if->m_xConnector;
  VD6283TX_Object_t *p_platform_drv = (VD6283TX_Object_t *) & ((VD6283TXTask *)_this)->als_driver_if;
  p_platform_drv->IO = *p_sensor_drv_gen;

  uint8_t id;
  uint32_t status = 0;

  status = p_sensor_drv_gen->read_reg(p_sensor_drv_gen->handle, VD6283TX_DEVICE_ID_REG, &id, 1);

  if (status == VD6283TX_OK)
  {
    ABusIFSetWhoAmI(((VD6283TXTask *)_this)->p_sensor_bus_if, id);
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("VD6283TX: sensor - I am 0x%x.\r\n", id));

    status = VD6283TX_Init(p_platform_drv);
    if (status != VD6283TX_OK)
    {
      res = SYS_BASE_LOW_LEVEL_ERROR_CODE;
    }
  }
  else
  {
    res = SYS_BASE_LOW_LEVEL_ERROR_CODE;
  }

  return res;
}

sys_error_code_t VD6283TXTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  VD6283TXTask *p_obj = (VD6283TXTask *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = VD6283TXTaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t VD6283TXTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  VD6283TXTask *p_obj = (VD6283TXTask*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t VD6283TXTask_vtblLightGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  VD6283TXTask *p_if_owner = (VD6283TXTask *)((uint32_t) _this - offsetof(VD6283TXTask, sensor_if));
  uint8_t res = p_if_owner->id;

  return res;
}

IEventSrc *VD6283TXTask_vtblLightGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  VD6283TXTask *p_if_owner = (VD6283TXTask *)((uint32_t) _this - offsetof(VD6283TXTask, sensor_if));
  return p_if_owner->p_event_src;
}

sys_error_code_t VD6283TXTask_vtblLightGetIntermeasurementTime(ISensorLight_t *_this, uint32_t *p_measured,
                                                               uint32_t *p_nominal)
{
  assert_param(_this != NULL);
  VD6283TXTask *p_if_owner = (VD6283TXTask *)((uint32_t) _this - offsetof(VD6283TXTask, sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if ((p_measured == NULL) || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = (uint32_t)p_if_owner->sensor_status.type.light.measured_intermeasurement_time;
    *p_nominal = p_if_owner->sensor_status.type.light.intermeasurement_time;
  }

  return res;
}

uint32_t VD6283TXTask_vtblLightGetExposureTime(ISensorLight_t *_this)
{
  assert_param(_this != NULL);
  VD6283TXTask *p_if_owner = (VD6283TXTask *)((uint32_t) _this - offsetof(VD6283TXTask, sensor_if));
  uint32_t res = p_if_owner->sensor_status.type.light.exposure_time;

  return res;
}

sys_error_code_t VD6283TXTask_vtblLightGetLightGain(ISensorLight_t *_this, float *LightGain)
{
  assert_param(_this != NULL);
  VD6283TXTask *p_if_owner = (VD6283TXTask *)((uint32_t) _this - offsetof(VD6283TXTask, sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  for (uint8_t channel = 0; channel < VD6283TX_MAX_CHANNELS; ++channel)
  {
    LightGain[channel] = p_if_owner->sensor_status.type.light.gain[channel];
  }

  return res;
}

EMData_t VD6283TXTask_vtblLightGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  VD6283TXTask *p_if_owner = (VD6283TXTask *)((uint32_t) _this - offsetof(VD6283TXTask, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t VD6283TXTask_vtblSensorSetIntermeasurementTime(ISensorLight_t *_this, uint32_t intermeasurement_time)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  VD6283TXTask *p_if_owner = (VD6283TXTask *)((uint32_t) _this - offsetof(VD6283TXTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  intermeasurement_time = intermeasurement_time - (intermeasurement_time % 20);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.light.intermeasurement_time = intermeasurement_time;
    p_if_owner->sensor_status.type.light.measured_intermeasurement_time = 0.0f;

    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_INTERMEASUREMENT_TIME,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) intermeasurement_time
    };
    res = VD6283TXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t VD6283TXTask_vtblSensorSetExposureTime(ISensorLight_t *_this, uint32_t exposure_time)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  VD6283TXTask *p_if_owner = (VD6283TXTask *)((uint32_t) _this - offsetof(VD6283TXTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  exposure_time = exposure_time - (exposure_time % 1600);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.light.exposure_time = exposure_time;

    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_EXPOSURE_TIME,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) exposure_time
    };
    res = VD6283TXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t VD6283TXTask_vtblSensorSetLightGain(ISensorLight_t *_this, float LightGain, uint8_t channel)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  VD6283TXTask *p_if_owner = (VD6283TXTask *)((uint32_t) _this - offsetof(VD6283TXTask, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    uint8_t cmd_id = 0;
    switch (channel)
    {
      case 1:
        cmd_id = SENSOR_CMD_ID_SET_LIGHT_CH1_GAIN;
        break;
      case 2:
        cmd_id = SENSOR_CMD_ID_SET_LIGHT_CH2_GAIN;
        break;
      case 3:
        cmd_id = SENSOR_CMD_ID_SET_LIGHT_CH3_GAIN;
        break;
      case 4:
        cmd_id = SENSOR_CMD_ID_SET_LIGHT_CH4_GAIN;
        break;
      case 5:
        cmd_id = SENSOR_CMD_ID_SET_LIGHT_CH5_GAIN;
        break;
      case 6:
        cmd_id = SENSOR_CMD_ID_SET_LIGHT_CH6_GAIN;
        break;
      default:
        cmd_id = 0;
        break;
    }

    if (cmd_id != 0)
    {
      p_if_owner->sensor_status.type.light.gain[channel - 1] = LightGain;
      /* Set a new command message in the queue */
      SMMessage report =
      {
        .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
        .sensorMessage.nCmdID = cmd_id,
        .sensorMessage.nSensorId = sensor_id,
        .sensorMessage.fParam = LightGain
      };
      res = VD6283TXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }
  }

  return res;
}

sys_error_code_t VD6283TXTask_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  VD6283TXTask *p_if_owner = (VD6283TXTask *)((uint32_t) _this - offsetof(VD6283TXTask, sensor_if));
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
    res = VD6283TXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t VD6283TXTask_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  VD6283TXTask *p_if_owner = (VD6283TXTask *)((uint32_t) _this - offsetof(VD6283TXTask, sensor_if));
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
    res = VD6283TXTaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t VD6283TXTask_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;

  VD6283TXTask *p_if_owner = (VD6283TXTask *)((uint32_t) _this - offsetof(VD6283TXTask, sensor_if));

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

SensorDescriptor_t VD6283TXTask_vtblSensorGetDescription(ISensor_t *_this)
{

  assert_param(_this != NULL);
  VD6283TXTask *p_if_owner = (VD6283TXTask *)((uint32_t) _this - offsetof(VD6283TXTask, sensor_if));
  return *p_if_owner->sensor_descriptor;
}

SensorStatus_t VD6283TXTask_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  VD6283TXTask *p_if_owner = (VD6283TXTask *)((uint32_t) _this - offsetof(VD6283TXTask, sensor_if));

  return p_if_owner->sensor_status;
}

SensorStatus_t *VD6283TXTask_vtblSensorGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  VD6283TXTask *p_if_owner = (VD6283TXTask *)((uint32_t) _this - offsetof(VD6283TXTask, sensor_if));

  return &p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t VD6283TXTaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  VD6283TXTask *p_obj = (VD6283TXTask *) _this;
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
          case SENSOR_CMD_ID_SET_INTERMEASUREMENT_TIME:
            res = VD6283TXTaskSensorSetIntermeasurementTime(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_EXPOSURE_TIME:
            res = VD6283TXTaskSensorSetExposureTime(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LIGHT_CH1_GAIN:
            res = VD6283TXTaskSensorSetLightGain(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LIGHT_CH2_GAIN:
            res = VD6283TXTaskSensorSetLightGain(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LIGHT_CH3_GAIN:
            res = VD6283TXTaskSensorSetLightGain(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LIGHT_CH4_GAIN:
            res = VD6283TXTaskSensorSetLightGain(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LIGHT_CH5_GAIN:
            res = VD6283TXTaskSensorSetLightGain(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LIGHT_CH6_GAIN:
            res = VD6283TXTaskSensorSetLightGain(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = VD6283TXTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = VD6283TXTaskSensorDisable(p_obj, report);
            break;
          default:
          {
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("VD6283TX: unexpected report in Run: %i\r\n", report.messageID));
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
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("VD6283TX: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }

    }
  }

  return res;
}

static sys_error_code_t VD6283TXTaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  VD6283TXTask *p_obj = (VD6283TXTask *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("VD6283TX: new data.\r\n"));
        res = VD6283TXTaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          // notify the listeners...
          double timestamp = report.sensorDataReadyMessage.fTimestamp;
          double delta_timestamp = timestamp - p_obj->prev_timestamp;
          p_obj->prev_timestamp = timestamp;

          /* update measured data frequency: one sample in delta_timestamp time */
          p_obj->sensor_status.type.light.measured_intermeasurement_time = (float)(delta_timestamp);
          EMD_Init(&p_obj->data, (uint8_t *) &p_obj->p_sensor_data_buff[0], E_EM_UINT32, E_EM_MODE_INTERLEAVED, 2, 1, 6);

          DataEvent_t evt;

          DataEventInit((IEvent *) &evt, p_obj->p_event_src, &p_obj->data, timestamp, p_obj->id);
          IEventSrcSendEvent(p_obj->p_event_src, (IEvent *) &evt, NULL);
          SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("VD6283TX: ts = %f\r\n", (float)timestamp));
        }
        break;
      }
      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
            res = VD6283TXTaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->sensor_status.is_active == true)
              {
                if (p_obj->pIRQConfig == NULL)
                {
                  if (TX_SUCCESS
                      != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->vd6283tx_task_cfg_timer_period_ms),
                                         AMT_MS_TO_TICKS(p_obj->vd6283tx_task_cfg_timer_period_ms)))
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
                  VD6283TXTaskConfigureIrqPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_INTERMEASUREMENT_TIME:
            res = VD6283TXTaskSensorSetIntermeasurementTime(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_EXPOSURE_TIME:
            res = VD6283TXTaskSensorSetExposureTime(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LIGHT_CH1_GAIN:
            res = VD6283TXTaskSensorSetLightGain(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LIGHT_CH2_GAIN:
            res = VD6283TXTaskSensorSetLightGain(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LIGHT_CH3_GAIN:
            res = VD6283TXTaskSensorSetLightGain(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LIGHT_CH4_GAIN:
            res = VD6283TXTaskSensorSetLightGain(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LIGHT_CH5_GAIN:
            res = VD6283TXTaskSensorSetLightGain(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LIGHT_CH6_GAIN:
            res = VD6283TXTaskSensorSetLightGain(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = VD6283TXTaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = VD6283TXTaskSensorDisable(p_obj, report);
            break;
          default:
          {
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("VD6283TX: unexpected report in Datalog: %i\r\n", report.messageID));
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

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("VD6283TX: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static inline sys_error_code_t VD6283TXTaskPostReportToFront(VD6283TXTask *_this, SMMessage *pReport)
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

static inline sys_error_code_t VD6283TXTaskPostReportToBack(VD6283TXTask *_this, SMMessage *pReport)
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

static sys_error_code_t VD6283TXTaskSensorInit(VD6283TXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv_gen = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  VD6283TX_Object_t *p_platform_drv = (VD6283TX_Object_t *) &_this->als_driver_if;
  p_platform_drv->IO = *p_sensor_drv_gen;

  uint32_t status = 0;

  status = VD6283TX_SetExposureTime(p_platform_drv, (uint32_t)(_this->sensor_status.type.light.exposure_time));

  if (status == VD6283TX_OK)
  {
    for (uint8_t channel = 0; (channel < VD6283TX_MAX_CHANNELS) && (status == VD6283TX_OK); channel++)
    {
      uint32_t ch_gain_fp;
      float ch_gain = _this->sensor_status.type.light.gain[channel];

      if (ch_gain < 0.77)
      {
        ch_gain_fp = GainRange[14];
      }
      else if (ch_gain < 0.92)
      {
        ch_gain_fp = GainRange[13];
      }
      else if (ch_gain < 1.13)
      {
        ch_gain_fp = GainRange[12];
      }
      else if (ch_gain < 1.46)
      {
        ch_gain_fp = GainRange[11];
      }
      else if (ch_gain < 2.09)
      {
        ch_gain_fp = GainRange[10];
      }
      else if (ch_gain < 2.92)
      {
        ch_gain_fp = GainRange[9];
      }
      else if (ch_gain < 4.17)
      {
        ch_gain_fp = GainRange[8];
      }
      else if (ch_gain < 6.07)
      {
        ch_gain_fp = GainRange[7];
      }
      else if (ch_gain < 8.57)
      {
        ch_gain_fp = GainRange[6];
      }
      else if (ch_gain < 13.34)
      {
        ch_gain_fp = GainRange[5];
      }
      else if (ch_gain < 20.82)
      {
        ch_gain_fp = GainRange[4];
      }
      else if (ch_gain < 29.17)
      {
        ch_gain_fp = GainRange[3];
      }
      else if (ch_gain < 41.67)
      {
        ch_gain_fp = GainRange[2];
      }
      else if (ch_gain < 58.34)
      {
        ch_gain_fp = GainRange[1];
      }
      else
      {
        ch_gain_fp = GainRange[0];
      }
      /* ch_gain float value has been converted to 8.8 fixed point format */
      status = VD6283TX_SetGain(p_platform_drv, channel, ch_gain_fp);
    }
  }

  if (status == VD6283TX_OK)
  {
    /*PID requires time values in microseconds*/
    status = VD6283TX_SetInterMeasurementTime(p_platform_drv, _this->sensor_status.type.light.intermeasurement_time * 1000);
  }

  if (status == 0U && _this->sensor_status.is_active)
  {
    status = VD6283TX_Start(p_platform_drv, VD6283TX_MODE_CONTINUOUS);
  }
  else
  {
    status = VD6283TX_Stop(p_platform_drv);

    if (status == VD6283TX_OK)
    {
      status = VD6283TX_DeInit(p_platform_drv);
    }
    _this->sensor_status.is_active = false;
  }

  if (_this->sensor_status.is_active)
  {
    /* From datasheet: After each EXTIME duration, a fixed period of time of ~6 ms takes
     * place to allow counts to be converted into digital information.
     * The user can set an intermeasurement period (intermeasurement_time). This can be
     * useful to reduce sensing frame rate and power consumption */

    if (_this->sensor_status.type.light.intermeasurement_time > _this->sensor_status.type.light.exposure_time / 1000 + 6)
    {
      _this->vd6283tx_task_cfg_timer_period_ms =  _this->sensor_status.type.light.intermeasurement_time;
    }
    else
    {
      _this->vd6283tx_task_cfg_timer_period_ms = _this->sensor_status.type.light.exposure_time / 1000 + 6;
    }
  }

  if (status != VD6283TX_OK)
  {
    res = VD6283TX_ERROR;
  }

  return res;
}

static sys_error_code_t VD6283TXTaskSensorReadData(VD6283TXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv_gen = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  VD6283TX_Object_t *p_platform_drv = (VD6283TX_Object_t *) &_this->als_driver_if;
  p_platform_drv->IO = *p_sensor_drv_gen;

  res = VD6283TX_GetValues(p_platform_drv, _this->p_sensor_data_buff);

  if (!SYS_IS_ERROR_CODE(res))
  {
#if (HSD_USE_DUMMY_DATA == 1)
    uint16_t i = 0;
    int32_t *p32 = (int32_t *)_this->p_sensor_data_buff;

    for (i = 0; i < VD6283TX_MAX_CHANNELS; i++)
    {
      *p32++ = dummyDataCounter++;
    }
#endif
  }

  return res;
}

static sys_error_code_t VD6283TXTaskSensorRegister(VD6283TXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *tof_if = (ISensor_t *) VD6283TXTaskGetLightSensorIF(_this);
  _this->id = SMAddSensor(tof_if);

  return res;
}

static sys_error_code_t VD6283TXTaskSensorInitTaskParams(VD6283TXTask *_this)
{

  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* PRESENCE SENSOR STATUS */
  _this->sensor_status.isensor_class = ISENSOR_CLASS_LIGHT;
  _this->sensor_status.is_active = TRUE;
  _this->sensor_status.type.light.intermeasurement_time = 1000;
  _this->sensor_status.type.light.exposure_time = 99200;
  _this->sensor_status.type.light.gain[ 0 ] = 1;
  _this->sensor_status.type.light.gain[ 1 ] = 1;
  _this->sensor_status.type.light.gain[ 2 ] = 1;
  _this->sensor_status.type.light.gain[ 3 ] = 1;
  _this->sensor_status.type.light.gain[ 4 ] = 1;
  _this->sensor_status.type.light.gain[ 5 ] = 1;
  EMD_Init(&_this->data, (uint8_t *) &_this->p_sensor_data_buff[0], E_EM_UINT32, E_EM_MODE_INTERLEAVED, 2, 1, 6);

  return res;
}

static sys_error_code_t VD6283TXTaskSensorSetIntermeasurementTime(VD6283TXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv_gen = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  VD6283TX_Object_t *p_platform_drv = (VD6283TX_Object_t *) &_this->als_driver_if;
  p_platform_drv->IO = *p_sensor_drv_gen;
  uint32_t intermeasurement_time = (uint32_t) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->sensor_status.type.light.intermeasurement_time = intermeasurement_time;
      _this->sensor_status.type.light.measured_intermeasurement_time = 0;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t VD6283TXTaskSensorSetExposureTime(VD6283TXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv_gen = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  VD6283TX_Object_t *p_platform_drv = (VD6283TX_Object_t *) &_this->als_driver_if;
  p_platform_drv->IO = *p_sensor_drv_gen;
  uint32_t exposure_time = (uint32_t)report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->sensor_status.type.light.exposure_time = exposure_time;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t VD6283TXTaskSensorSetLightGain(VD6283TXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv_gen = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  VD6283TX_Object_t *p_platform_drv = (VD6283TX_Object_t *) &_this->als_driver_if;
  p_platform_drv->IO = *p_sensor_drv_gen;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->sensor_status.type.light.gain[report.sensorMessage.nCmdID - 6] = report.sensorMessage.fParam;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t VD6283TXTaskSensorEnable(VD6283TXTask *_this, SMMessage report)
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

static sys_error_code_t VD6283TXTaskSensorDisable(VD6283TXTask *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv_gen = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  VD6283TX_Object_t *p_platform_drv = (VD6283TX_Object_t *) &_this->als_driver_if;
  p_platform_drv->IO = *p_sensor_drv_gen;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    _this->sensor_status.is_active = FALSE;
    res = VD6283TX_Stop(p_platform_drv);

    if (res == VD6283TX_OK)
    {
      VD6283TX_DeInit(p_platform_drv);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t VD6283TXTaskSensorIsActive(const VD6283TXTask *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.is_active;
}

static sys_error_code_t VD6283TXTaskEnterLowPowerMode(const VD6283TXTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv_gen = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  VD6283TX_Object_t *p_platform_drv = (VD6283TX_Object_t *) &_this->als_driver_if;
  p_platform_drv->IO = *p_sensor_drv_gen;

  if (VD6283TX_Stop(p_platform_drv))
  {
    VD6283TX_DeInit(p_platform_drv);
    res = SYS_SENSOR_TASK_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_OP_ERROR_CODE);
  }

  return res;
}

static sys_error_code_t VD6283TXTaskConfigureIrqPin(const VD6283TXTask *_this, boolean_t LowPower)
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

static void VD6283TXTaskTimerCallbackFunction(ULONG param)
{
  VD6283TXTask *p_obj = (VD6283TXTask *) param;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  if (TX_SUCCESS != tx_queue_send(&p_obj->in_queue, &report, TX_NO_WAIT))
  {
    /* unable to send the report. Signal the error */
    sys_error_handler();
  }
}

/* CubeMX integration */

void VD6283TXTask_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((VD6283TXTask *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}
