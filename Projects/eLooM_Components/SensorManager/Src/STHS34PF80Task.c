/**
  ******************************************************************************
  * @file    STHS34PF80.c
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
#include "STHS34PF80Task.h"
#include "STHS34PF80Task_vtbl.h"
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

#ifndef STHS34PF80_TASK_CFG_STACK_DEPTH
#define STHS34PF80_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*8)
#endif

#ifndef STHS34PF80_TASK_CFG_PRIORITY
#define STHS34PF80_TASK_CFG_PRIORITY                 (4)
#endif

#ifndef STHS34PF80_TASK_CFG_IN_QUEUE_LENGTH
#define STHS34PF80_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define STHS34PF80_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef STHS34PF80_TASK_CFG_TIMER_PERIOD_MS
#define STHS34PF80_TASK_CFG_TIMER_PERIOD_MS          500
#endif

#ifndef STHS34PF80_TASK_CFG_MAX_INSTANCES_COUNT
#define STHS34PF80_TASK_CFG_MAX_INSTANCES_COUNT      1
#endif

#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_STHS34PF80, level, message)

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static uint16_t dummyDataCounter = 0;
#endif

/**
  * Class object declaration
  */
typedef struct _STHS34PF80TaskClass
{
  /**
    * STHS34PF80Task class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * presence IF virtual table.
    */
  const ISensorPresence_vtbl sensor_if_vtbl;

  /**
    * Specifies presence sensor capabilities.
    */
  const SensorDescriptor_t class_descriptor;

  /**
    * STHS34PF80Task (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[STHS34PF80_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map is used to link Cube HAL callback with an instance of the sensor task object. The key of the map is the address of the task instance.   */
  MTMap_t task_map;

} STHS34PF80TaskClass_t;

/* Private member function declaration */ // ***********************************
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t STHS34PF80TaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t STHS34PF80TaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the actual parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t STHS34PF80TaskSensorInit(STHS34PF80Task *_this);

/**
  * Read the data from the sensor.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t STHS34PF80TaskSensorReadData(STHS34PF80Task *_this);

/**
  * Register the sensor with the global DB and initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t STHS34PF80TaskSensorRegister(STHS34PF80Task *_this);

/**
  * Initialize the default parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise
  */
static sys_error_code_t STHS34PF80TaskSensorInitTaskParams(STHS34PF80Task *_this);

/**
  * Private implementation of sensor interface methods for STHS34PF80 sensor
  */
static sys_error_code_t STHS34PF80TaskSensorSetDataFrequency(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorSetTransmittance(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorSetAverageTObject(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorSetAverageTAmbient(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorSetPresenceThreshold(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorSetPresenceHysteresis(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorSetMotionThreshold(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorSetMotionHysteresis(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorSetTAmbientShockThreshold(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorSetTAmbientShockHysteresis(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorSetLPF_P_M_Bandwidth(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorSetLPF_P_Bandwidth(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorSetLPF_M_Bandwidth(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorSetEmbeddedCompensation(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorSetSoftwareCompensation(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorConfigSoftwareCompensation(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorEnable(STHS34PF80Task *_this, SMMessage report);
static sys_error_code_t STHS34PF80TaskSensorDisable(STHS34PF80Task *_this, SMMessage report);

/**
  * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
  * @param _this [IN] specifies a pointer to a task object.
  * @return TRUE if the sensor is active, FALSE otherwise.
  */
static boolean_t STHS34PF80TaskSensorIsActive(const STHS34PF80Task *_this);

static sys_error_code_t STHS34PF80TaskEnterLowPowerMode(const STHS34PF80Task *_this);

static sys_error_code_t STHS34PF80TaskConfigureIrqPin(const STHS34PF80Task *_this, boolean_t LowPower);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void STHS34PF80TaskTimerCallbackFunction(ULONG param);


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
static inline sys_error_code_t STHS34PF80TaskPostReportToFront(STHS34PF80Task *_this, SMMessage *pReport);

/**
  * Private function used to post a report into the back of the task queue.
  * Used to resume the task when the required by the INIT task.
  *
  * @param this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_EROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE.
  */
static inline sys_error_code_t STHS34PF80TaskPostReportToBack(STHS34PF80Task *_this, SMMessage *pReport);

/* Objects instance */
/********************/

/**
  * The only instance of the task object.
  */
// static STHS34PF80Task sTaskObj;

/**
  * The class object.
  */
static STHS34PF80TaskClass_t sTheClass =
{
  /* Class virtual table */
  {
    STHS34PF80Task_vtblHardwareInit,
    STHS34PF80Task_vtblOnCreateTask,
    STHS34PF80Task_vtblDoEnterPowerMode,
    STHS34PF80Task_vtblHandleError,
    STHS34PF80Task_vtblOnEnterTaskControlLoop,
    STHS34PF80Task_vtblForceExecuteStep,
    STHS34PF80Task_vtblOnEnterPowerMode
  },
  /* class::sensor_if_vtbl virtual table */
  {
    {
      {
        STHS34PF80Task_vtblPresenceGetId,
        STHS34PF80Task_vtblPresenceGetEventSourceIF,
        STHS34PF80Task_vtblPresenceGetDataInfo
      },

      STHS34PF80Task_vtblSensorEnable,
      STHS34PF80Task_vtblSensorDisable,
      STHS34PF80Task_vtblSensorIsEnabled,
      STHS34PF80Task_vtblSensorGetDescription,
      STHS34PF80Task_vtblSensorGetStatus,
      STHS34PF80Task_vtblSensorGetStatusPointer
    },

    STHS34PF80Task_vtblPresenceGetDataFrequency,
    STHS34PF80Task_vtblPresenceGetTransmittance,
    STHS34PF80Task_vtblPresenceGetAverageTObject,
    STHS34PF80Task_vtblPresenceGetAverageTAmbient,
    STHS34PF80Task_vtblPresenceGetPresenceThreshold,
    STHS34PF80Task_vtblPresenceGetPresenceHysteresis,
    STHS34PF80Task_vtblPresenceGetMotionThreshold,
    STHS34PF80Task_vtblPresenceGetMotionHysteresis,
    STHS34PF80Task_vtblPresenceGetTAmbientShockThreshold,
    STHS34PF80Task_vtblPresenceGetTAmbientShockHysteresis,
    STHS34PF80Task_vtblPresenceGetLPF_P_M_Bandwidth,
    STHS34PF80Task_vtblPresenceGetLPF_P_Bandwidth,
    STHS34PF80Task_vtblPresenceGetLPF_M_Bandwidth,
    STHS34PF80Task_vtblPresenceGetEmbeddedCompensation,
    STHS34PF80Task_vtblPresenceGetSoftwareCompensation,
    STHS34PF80Task_vtblPresenceGetSoftwareCompensationAlgorithmConfig,
    STHS34PF80Task_vtblSensorSetDataFrequency,
    STHS34PF80Task_vtblSensorSetTransmittance,
    STHS34PF80Task_vtblSensorSetAverageTObject,
    STHS34PF80Task_vtblSensorSetAverageTAmbient,
    STHS34PF80Task_vtblSensorSetPresenceThreshold,
    STHS34PF80Task_vtblSensorSetPresenceHysteresis,
    STHS34PF80Task_vtblSensorSetMotionThreshold,
    STHS34PF80Task_vtblSensorSetMotionHysteresis,
    STHS34PF80Task_vtblSensorSetTAmbientShockThreshold,
    STHS34PF80Task_vtblSensorSetTAmbientShockHysteresis,
    STHS34PF80Task_vtblSensorSetLPF_P_M_Bandwidth,
    STHS34PF80Task_vtblSensorSetLPF_P_Bandwidth,
    STHS34PF80Task_vtblSensorSetLPF_M_Bandwidth,
    STHS34PF80Task_vtblSensorSetEmbeddedCompensation,
    STHS34PF80Task_vtblSensorSetSoftwareCompensation,
    STHS34PF80Task_vtblSensorSetSoftwareCompensationAlgorithmConfig
  },
  /* PRESENCE DESCRIPTOR */
  {
    "sths34pf80",
    COM_TYPE_TMOS
  },
  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    STHS34PF80TaskExecuteStepState1,
    NULL,
    STHS34PF80TaskExecuteStepDatalog,
  },
  {{0}}, /* task_map_elements */
  {0}  /* task_map */
};

/* Public API definition */
// *********************
ISourceObservable *STHS34PF80TaskGetPresenceSensorIF(STHS34PF80Task *_this)
{
  return (ISourceObservable *) & (_this->sensor_if);
}

AManagedTaskEx *STHS34PF80TaskAlloc(const void *pIRQConfig, const void *pCSConfig, const void *pBSConfig)
{
  STHS34PF80Task *p_new_obj = SysAlloc(sizeof(STHS34PF80Task));

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

AManagedTaskEx *STHS34PF80TaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const void *pBSConfig,
                                           const char *p_name)
{
  STHS34PF80Task *p_new_obj = (STHS34PF80Task *)STHS34PF80TaskAlloc(pIRQConfig, pCSConfig, pBSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_new_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_new_obj;
}

AManagedTaskEx *STHS34PF80TaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                          const void *pBSConfig)
{
  STHS34PF80Task *p_obj = (STHS34PF80Task *)p_mem_block;

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

AManagedTaskEx *STHS34PF80TaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                                 const void *pBSConfig,
                                                 const char *p_name)
{
  STHS34PF80Task *p_obj = (STHS34PF80Task *)STHS34PF80TaskStaticAlloc(p_mem_block, pIRQConfig, pCSConfig, pBSConfig);

  /* Overwrite default name with the one selected by the application */
  strcpy(p_obj->sensor_status.p_name, p_name);

  return (AManagedTaskEx *) p_obj;
}

ABusIF *STHS34PF80TaskGetSensorIF(STHS34PF80Task *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *STHS34PF80TaskGetEventSrcIF(STHS34PF80Task *_this)
{
  assert_param(_this != NULL);

  return (IEventSrc *) _this->p_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t STHS34PF80Task_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STHS34PF80Task *p_obj = (STHS34PF80Task *) _this;

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

sys_error_code_t STHS34PF80Task_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                                 VOID **pvStackStart,
                                                 ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                                 ULONG *pParams)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STHS34PF80Task *p_obj = (STHS34PF80Task *) _this;

  /* Create task specific sw resources */

  uint32_t item_size = (uint32_t) STHS34PF80_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(STHS34PF80_TASK_CFG_IN_QUEUE_LENGTH * item_size);
  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "STHS34PF80_Q", item_size / 4u, p_queue_items_buff,
                                    STHS34PF80_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* create the software timer*/
  if (TX_SUCCESS
      != tx_timer_create(&p_obj->read_timer, "STHS34PF80_T", STHS34PF80TaskTimerCallbackFunction, (ULONG)_this,
                         AMT_MS_TO_TICKS(STHS34PF80_TASK_CFG_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* Alloc the bus interface (SPI if the task is given the CS Pin configuration param, I2C otherwise) */
  if (p_obj->pCSConfig != NULL)
  {
    p_obj->p_sensor_bus_if = SPIBusIFAlloc(STHS34PF80_ID, p_obj->pCSConfig->port, (uint16_t) p_obj->pCSConfig->pin, 0);
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
      p_obj->p_sensor_bus_if = I2CBSBusIFAlloc(STHS34PF80_ID, STHS34PF80_I2C_ADD, p_obj->pBSConfig->port, (uint16_t) p_obj->pBSConfig->pin, 0);
      if (p_obj->p_sensor_bus_if == NULL)
      {
        res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
      }
    }
    else
    {
      p_obj->p_sensor_bus_if = I2CBusIFAlloc(STHS34PF80_ID, STHS34PF80_I2C_ADD, 0);
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
    (void) MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, STHS34PF80_TASK_CFG_MAX_INSTANCES_COUNT);
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
  p_obj->IsBlocking = 0;
  p_obj->IsContinuous = 0;
  p_obj->tmos_swlib = NULL;
  p_obj->tmos_swlib_instance = NULL;
  p_obj->id = 0;
  p_obj->prev_timestamp = 0.0f;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "STHS34PF80";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = STHS34PF80_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = STHS34PF80_TASK_CFG_PRIORITY;
  *pPreemptThreshold = STHS34PF80_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = STHS34PF80TaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = STHS34PF80TaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("STHS34PF80: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t STHS34PF80Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STHS34PF80Task *p_obj = (STHS34PF80Task *) _this;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &p_obj->p_sensor_bus_if->m_xConnector;
  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    if (STHS34PF80TaskSensorIsActive(p_obj))
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("STHS34PF80: -> SENSORS_ACTIVE\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_STATE1)
  {
    if (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      if (STHS34PF80TaskSensorIsActive(p_obj))
      {
        /* Deactivate the sensor */
        sths34pf80_tmos_func_status_t tmos_func_status;
        if (0 == sths34pf80_tmos_odr_set(p_sensor_drv, STHS34PF80_TMOS_ODR_OFF))
        {
          /* Clear DRDY */
          sths34pf80_tmos_func_status_get(p_sensor_drv, &tmos_func_status);
        }
      }
      /* Empty the task queue and disable INT or timer */
      tx_queue_flush(&p_obj->in_queue);
      if (p_obj->pIRQConfig == NULL)
      {
        tx_timer_deactivate(&p_obj->read_timer);
      }
      else
      {
        STHS34PF80TaskConfigureIrqPin(p_obj, TRUE);
      }
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("STHS34PF80: -> STATE1\r\n"));
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    /* the MCU is going in stop so I put the sensor in low power
     from the INIT task */
    res = STHS34PF80TaskEnterLowPowerMode(p_obj);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("STHS34PF80 - Enter Low Power Mode failed.\r\n"));
    }
    if (p_obj->pIRQConfig != NULL)
    {
      STHS34PF80TaskConfigureIrqPin(p_obj, TRUE);
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("STHS34PF80: -> SLEEP_1\r\n"));
  }

  return res;
}

sys_error_code_t STHS34PF80Task_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  STHS34PF80Task *p_obj = (STHS34PF80Task*)_this;

  return res;
}

sys_error_code_t STHS34PF80Task_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("STHS34PF80: start.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (STHS34PF80_TASK_CFG_TAG)
  STHS34PF80Task *p_obj = (STHS34PF80Task *) _this;
  p_obj->super.m_xTaskHandle.pxTaskTag = STHS34PF80_TASK_CFG_TAG;
#endif

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.

  return res;
}

sys_error_code_t STHS34PF80Task_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STHS34PF80Task *p_obj = (STHS34PF80Task *) _this;

  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      res = STHS34PF80TaskPostReportToFront(p_obj, (SMMessage *) &report);
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

sys_error_code_t STHS34PF80Task_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  //  STHS34PF80Task *p_obj = (STHS34PF80Task*)_this;

  return res;
}

// ISensor virtual functions definition
// *******************************************

uint8_t STHS34PF80Task_vtblPresenceGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  uint8_t res = p_if_owner->id;

  return res;
}

IEventSrc *STHS34PF80Task_vtblPresenceGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  return p_if_owner->p_event_src;
}

sys_error_code_t STHS34PF80Task_vtblPresenceGetDataFrequency(ISensorPresence_t *_this, float *p_measured,
                                                             float *p_nominal)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* parameter validation */
  if ((p_measured == NULL) || (p_nominal == NULL))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    *p_measured = p_if_owner->sensor_status.type.presence.measured_data_frequency;
    *p_nominal = p_if_owner->sensor_status.type.presence.data_frequency;
  }

  return res;
}

float STHS34PF80Task_vtblPresenceGetTransmittance(ISensorPresence_t *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  float res = p_if_owner->sensor_status.type.presence.Transmittance;

  return res;
}

uint16_t STHS34PF80Task_vtblPresenceGetAverageTObject(ISensorPresence_t *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  uint16_t res = p_if_owner->sensor_status.type.presence.average_tobject;

  return res;
}

uint16_t STHS34PF80Task_vtblPresenceGetAverageTAmbient(ISensorPresence_t *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  uint16_t res = p_if_owner->sensor_status.type.presence.average_tambient;

  return res;
}

uint16_t STHS34PF80Task_vtblPresenceGetPresenceThreshold(ISensorPresence_t *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  uint16_t res = p_if_owner->sensor_status.type.presence.presence_threshold;

  return res;
}

uint8_t STHS34PF80Task_vtblPresenceGetPresenceHysteresis(ISensorPresence_t *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  uint8_t res = p_if_owner->sensor_status.type.presence.presence_hysteresis;

  return res;
}

uint16_t STHS34PF80Task_vtblPresenceGetMotionThreshold(ISensorPresence_t *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  uint16_t res = p_if_owner->sensor_status.type.presence.motion_threshold;

  return res;
}

uint8_t STHS34PF80Task_vtblPresenceGetMotionHysteresis(ISensorPresence_t *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  uint8_t res = p_if_owner->sensor_status.type.presence.motion_hysteresis;

  return res;
}

uint16_t STHS34PF80Task_vtblPresenceGetTAmbientShockThreshold(ISensorPresence_t *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  uint16_t res = p_if_owner->sensor_status.type.presence.tambient_shock_threshold;

  return res;
}

uint8_t STHS34PF80Task_vtblPresenceGetTAmbientShockHysteresis(ISensorPresence_t *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  uint8_t res = p_if_owner->sensor_status.type.presence.tambient_shock_hysteresis;

  return res;
}

uint16_t STHS34PF80Task_vtblPresenceGetLPF_P_M_Bandwidth(ISensorPresence_t *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  uint16_t res = p_if_owner->sensor_status.type.presence.lpf_p_m_bandwidth;

  return res;
}

uint16_t STHS34PF80Task_vtblPresenceGetLPF_P_Bandwidth(ISensorPresence_t *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  uint16_t res = p_if_owner->sensor_status.type.presence.lpf_p_bandwidth;

  return res;
}

uint16_t STHS34PF80Task_vtblPresenceGetLPF_M_Bandwidth(ISensorPresence_t *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  uint16_t res = p_if_owner->sensor_status.type.presence.lpf_m_bandwidth;

  return res;
}

uint8_t STHS34PF80Task_vtblPresenceGetEmbeddedCompensation(ISensorPresence_t *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  uint8_t res = p_if_owner->sensor_status.type.presence.embedded_compensation;

  return res;
}

uint8_t STHS34PF80Task_vtblPresenceGetSoftwareCompensation(ISensorPresence_t *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  uint8_t res = p_if_owner->sensor_status.type.presence.software_compensation;

  return res;
}

sys_error_code_t STHS34PF80Task_vtblPresenceGetSoftwareCompensationAlgorithmConfig(ISensorPresence_t *_this,
    CompensationAlgorithmConfig_t *pAlgorithmConfig)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  *pAlgorithmConfig = p_if_owner->sensor_status.type.presence.AlgorithmConfig;

  return res;
}

EMData_t STHS34PF80Task_vtblPresenceGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EMData_t res = p_if_owner->data;

  return res;
}

sys_error_code_t STHS34PF80Task_vtblSensorSetDataFrequency(ISensorPresence_t *_this, float data_frequency)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.presence.data_frequency = data_frequency;
    p_if_owner->sensor_status.type.presence.measured_data_frequency = 0.0f;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_DATA_FREQUENCY,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.fParam = (float) data_frequency
    };
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STHS34PF80Task_vtblSensorSetTransmittance(ISensorPresence_t *_this, float Transmittance)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.presence.Transmittance = Transmittance;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_TRANSMITTANCE,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.fParam = (float) Transmittance
    };
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STHS34PF80Task_vtblSensorSetAverageTObject(ISensorPresence_t *_this, uint16_t average_tobject)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.presence.average_tobject = average_tobject;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_AVERAGE_T_OBJECT,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = average_tobject
    };
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STHS34PF80Task_vtblSensorSetAverageTAmbient(ISensorPresence_t *_this, uint16_t average_tambient)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.presence.average_tambient = average_tambient;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_AVERAGE_T_AMBIENT,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = average_tambient
    };
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STHS34PF80Task_vtblSensorSetPresenceThreshold(ISensorPresence_t *_this, uint16_t presence_threshold)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.presence.presence_threshold = presence_threshold;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_PRESENCE_THRESHOLD,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = presence_threshold
    };
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STHS34PF80Task_vtblSensorSetPresenceHysteresis(ISensorPresence_t *_this, uint8_t presence_hysteresis)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.presence.presence_hysteresis = presence_hysteresis;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_PRESENCE_HYSTERESIS,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) presence_hysteresis
    };
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t STHS34PF80Task_vtblSensorSetMotionThreshold(ISensorPresence_t *_this, uint16_t motion_threshold)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.presence.motion_threshold = motion_threshold;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_MOTION_THRESHOLD,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = motion_threshold
    };
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STHS34PF80Task_vtblSensorSetMotionHysteresis(ISensorPresence_t *_this, uint8_t motion_hysteresis)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.presence.motion_hysteresis = motion_hysteresis;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_MOTION_HYSTERESIS,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) motion_hysteresis
    };
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t STHS34PF80Task_vtblSensorSetTAmbientShockThreshold(ISensorPresence_t *_this,
                                                                    uint16_t tambient_shock_threshold)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.presence.tambient_shock_threshold = tambient_shock_threshold;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_T_AMBIENT_SHOCK_THRESHOLD,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = tambient_shock_threshold
    };
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STHS34PF80Task_vtblSensorSetTAmbientShockHysteresis(ISensorPresence_t *_this,
                                                                     uint8_t tambient_shock_hysteresis)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.presence.tambient_shock_hysteresis = tambient_shock_hysteresis;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_T_AMBIENT_SHOCK_HYSTERESIS,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) tambient_shock_hysteresis
    };
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;

}

sys_error_code_t STHS34PF80Task_vtblSensorSetLPF_P_M_Bandwidth(ISensorPresence_t *_this, uint16_t Bandwidth)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.presence.lpf_p_m_bandwidth = Bandwidth;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_LPF_P_M_BANDWIDTH,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = Bandwidth
    };
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STHS34PF80Task_vtblSensorSetLPF_P_Bandwidth(ISensorPresence_t *_this, uint16_t Bandwidth)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.presence.lpf_p_bandwidth = Bandwidth;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_LPF_P_BANDWIDTH,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = Bandwidth
    };
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STHS34PF80Task_vtblSensorSetLPF_M_Bandwidth(ISensorPresence_t *_this, uint16_t Bandwidth)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.presence.lpf_m_bandwidth = Bandwidth;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_LPF_M_BANDWIDTH,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = Bandwidth
    };
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STHS34PF80Task_vtblSensorSetEmbeddedCompensation(ISensorPresence_t *_this,
                                                                  uint8_t embedded_compensation)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.presence.embedded_compensation = embedded_compensation;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_EMBEDDED_COMPENSATION,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) embedded_compensation
    };
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STHS34PF80Task_vtblSensorSetSoftwareCompensation(ISensorPresence_t *_this,
                                                                  uint8_t software_compensation)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.presence.software_compensation = software_compensation;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_SOFTWARE_COMPENSATION,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) software_compensation
    };
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STHS34PF80Task_vtblSensorSetSoftwareCompensationAlgorithmConfig(ISensorPresence_t *_this,
    CompensationAlgorithmConfig_t *pAlgorithmConfig)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *) p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *) _this);

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *) _this))
  {
    res = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else
  {
    p_if_owner->sensor_status.type.presence.AlgorithmConfig = *pAlgorithmConfig;
    /* Set a new command message in the queue */
    SMMessage report =
    {
      .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
      .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_SW_COMPENSATION_PARAMETERS,
      .sensorMessage.nSensorId = sensor_id,
      .sensorMessage.nParam = (uint32_t) pAlgorithmConfig
    };
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STHS34PF80Task_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
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
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

sys_error_code_t STHS34PF80Task_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
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
    res = STHS34PF80TaskPostReportToBack(p_if_owner, (SMMessage *) &report);
  }

  return res;
}

boolean_t STHS34PF80Task_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res = FALSE;

  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));

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

SensorDescriptor_t STHS34PF80Task_vtblSensorGetDescription(ISensor_t *_this)
{

  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));
  return *p_if_owner->sensor_descriptor;
}

SensorStatus_t STHS34PF80Task_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));

  return p_if_owner->sensor_status;
}

SensorStatus_t *STHS34PF80Task_vtblSensorGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);
  STHS34PF80Task *p_if_owner = (STHS34PF80Task *)((uint32_t) _this - offsetof(STHS34PF80Task, sensor_if));

  return &p_if_owner->sensor_status;
}

/* Private function definition */
// ***************************
static sys_error_code_t STHS34PF80TaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STHS34PF80Task *p_obj = (STHS34PF80Task *) _this;
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
          case SENSOR_CMD_ID_SET_DATA_FREQUENCY:
            res = STHS34PF80TaskSensorSetDataFrequency(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_TRANSMITTANCE:
            res = STHS34PF80TaskSensorSetTransmittance(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_AVERAGE_T_OBJECT:
            res = STHS34PF80TaskSensorSetAverageTObject(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_AVERAGE_T_AMBIENT:
            res = STHS34PF80TaskSensorSetAverageTAmbient(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_PRESENCE_THRESHOLD:
            res = STHS34PF80TaskSensorSetPresenceThreshold(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_PRESENCE_HYSTERESIS:
            res = STHS34PF80TaskSensorSetPresenceHysteresis(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_MOTION_THRESHOLD:
            res = STHS34PF80TaskSensorSetMotionThreshold(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_MOTION_HYSTERESIS:
            res = STHS34PF80TaskSensorSetMotionHysteresis(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_T_AMBIENT_SHOCK_THRESHOLD:
            res = STHS34PF80TaskSensorSetTAmbientShockThreshold(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_T_AMBIENT_SHOCK_HYSTERESIS:
            res = STHS34PF80TaskSensorSetTAmbientShockHysteresis(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LPF_P_M_BANDWIDTH:
            res = STHS34PF80TaskSensorSetLPF_P_M_Bandwidth(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LPF_P_BANDWIDTH:
            res = STHS34PF80TaskSensorSetLPF_P_Bandwidth(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LPF_M_BANDWIDTH:
            res = STHS34PF80TaskSensorSetLPF_M_Bandwidth(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_EMBEDDED_COMPENSATION:
            res = STHS34PF80TaskSensorSetEmbeddedCompensation(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_SOFTWARE_COMPENSATION:
            res = STHS34PF80TaskSensorSetSoftwareCompensation(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_SW_COMPENSATION_PARAMETERS:
            res = STHS34PF80TaskSensorConfigSoftwareCompensation(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = STHS34PF80TaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = STHS34PF80TaskSensorDisable(p_obj, report);
            break;
          default:
          {
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("STHS34PF80: unexpected report in Run: %i\r\n", report.messageID));
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
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("STHS34PF80: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }

    }
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  STHS34PF80Task *p_obj = (STHS34PF80Task *) _this;
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
        SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("STHS34PF80: new data.\r\n"));
        res = STHS34PF80TaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res))
        {
          // notify the listeners...
          double timestamp = report.sensorDataReadyMessage.fTimestamp;
          double delta_timestamp = timestamp - p_obj->prev_timestamp;
          p_obj->prev_timestamp = timestamp;

          /* update measured data frequency: one sample in delta_timestamp time */
          p_obj->sensor_status.type.presence.measured_data_frequency = 1.0f / (float) delta_timestamp;
          EMD_Init(&p_obj->data, (uint8_t *) &p_obj->p_sensor_data_buff[0], E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 11);

          DataEvent_t evt;

          DataEventInit((IEvent *) &evt, p_obj->p_event_src, &p_obj->data, timestamp, p_obj->id);
          IEventSrcSendEvent(p_obj->p_event_src, (IEvent *) &evt, NULL);
          SYS_DEBUGF(SYS_DBG_LEVEL_ALL, ("STHS34PF80: ts = %f\r\n", (float)timestamp));
        }
        break;
      }
      case SM_MESSAGE_ID_SENSOR_CMD:
      {
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
            res = STHS34PF80TaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res))
            {
              if (p_obj->sensor_status.is_active == true)
              {
                if (p_obj->pIRQConfig == NULL)
                {
                  if (TX_SUCCESS != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->sths34pf80_task_cfg_timer_period_ms),
                                                    AMT_MS_TO_TICKS(p_obj->sths34pf80_task_cfg_timer_period_ms)))
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
                  STHS34PF80TaskConfigureIrqPin(p_obj, FALSE);
                }
              }
            }
            break;
          case SENSOR_CMD_ID_SET_DATA_FREQUENCY:
            res = STHS34PF80TaskSensorSetDataFrequency(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_TRANSMITTANCE:
            res = STHS34PF80TaskSensorSetTransmittance(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_AVERAGE_T_OBJECT:
            res = STHS34PF80TaskSensorSetAverageTObject(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_AVERAGE_T_AMBIENT:
            res = STHS34PF80TaskSensorSetAverageTAmbient(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_PRESENCE_THRESHOLD:
            res = STHS34PF80TaskSensorSetPresenceThreshold(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_PRESENCE_HYSTERESIS:
            res = STHS34PF80TaskSensorSetPresenceHysteresis(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_MOTION_THRESHOLD:
            res = STHS34PF80TaskSensorSetMotionThreshold(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_MOTION_HYSTERESIS:
            res = STHS34PF80TaskSensorSetMotionHysteresis(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_T_AMBIENT_SHOCK_THRESHOLD:
            res = STHS34PF80TaskSensorSetTAmbientShockThreshold(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_T_AMBIENT_SHOCK_HYSTERESIS:
            res = STHS34PF80TaskSensorSetTAmbientShockHysteresis(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LPF_P_M_BANDWIDTH:
            res = STHS34PF80TaskSensorSetLPF_P_M_Bandwidth(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LPF_P_BANDWIDTH:
            res = STHS34PF80TaskSensorSetLPF_P_Bandwidth(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_LPF_M_BANDWIDTH:
            res = STHS34PF80TaskSensorSetLPF_M_Bandwidth(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_EMBEDDED_COMPENSATION:
            res = STHS34PF80TaskSensorSetEmbeddedCompensation(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_SOFTWARE_COMPENSATION:
            res = STHS34PF80TaskSensorSetSoftwareCompensation(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_SW_COMPENSATION_PARAMETERS:
            res = STHS34PF80TaskSensorConfigSoftwareCompensation(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = STHS34PF80TaskSensorEnable(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = STHS34PF80TaskSensorDisable(p_obj, report);
            break;
          default:
          {
            /* unwanted report */
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);

            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("STHS34PF80: unexpected report in Datalog: %i\r\n", report.messageID));
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

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("STHS34PF80: unexpected report in Datalog: %i\r\n", report.messageID));
        break;
      }
    }
  }

  return res;
}

static inline sys_error_code_t STHS34PF80TaskPostReportToFront(STHS34PF80Task *_this, SMMessage *pReport)
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

static inline sys_error_code_t STHS34PF80TaskPostReportToBack(STHS34PF80Task *_this, SMMessage *pReport)
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

static void tmos_swlib_config(STHS34PF80Task *_this, uint16_t sensitivity_boot, IPD_algo_conf_t *swlib_algo_conf)
{
  IPD_init_err_t    init_err = IPD_INIT_OK;
  IPD_device_conf_t device_conf;

  /* Change SW library algorithms configuration for each device (optional, uncomment to change) */
  switch (_this->sensor_status.type.presence.AlgorithmConfig.comp_type)
  {
    case 0:
      swlib_algo_conf->comp_type = IPD_COMP_NONE;
      break;
    case 1:
      swlib_algo_conf->comp_type = IPD_COMP_LIN;
      break;
    case 2:
      swlib_algo_conf->comp_type = IPD_COMP_NONLIN;
      break;
    default:
      break;
  }
  swlib_algo_conf->comp_filter_flag = _this->sensor_status.type.presence.AlgorithmConfig.comp_filter_flag;
  swlib_algo_conf->mot_ths          = _this->sensor_status.type.presence.AlgorithmConfig.mot_ths;
  swlib_algo_conf->pres_ths         = _this->sensor_status.type.presence.AlgorithmConfig.pres_ths;
  swlib_algo_conf->abs_static_flag  = _this->sensor_status.type.presence.AlgorithmConfig.abs_static_flag;

  if (_this->sensor_status.type.presence.data_frequency < 0.9f)
  {
    device_conf.odr = 0;
  }
  else if (_this->sensor_status.type.presence.data_frequency < 1.1f)
  {
    device_conf.odr = 1;
  }
  else if (_this->sensor_status.type.presence.data_frequency < 3.0f)
  {
    device_conf.odr = 2;
  }
  else if (_this->sensor_status.type.presence.data_frequency < 5.0f)
  {
    device_conf.odr = 4;
  }
  else if (_this->sensor_status.type.presence.data_frequency < 9.0f)
  {
    device_conf.odr = 8;
  }
  else if (_this->sensor_status.type.presence.data_frequency < 16.0f)
  {
    device_conf.odr = 15;
  }
  else
  {
    device_conf.odr = 30;
  }

  device_conf.avg_tmos = _this->sensor_status.type.presence.average_tobject;
  device_conf.avg_t = _this->sensor_status.type.presence.average_tambient;
  device_conf.gain_factor = 1;

  device_conf.sens_data = sensitivity_boot;
  device_conf.transmittance = _this->sensor_status.type.presence.Transmittance;

  init_err = InfraredPD_Start(_this->tmos_swlib_instance, &device_conf, swlib_algo_conf);

  if (IPD_INIT_OK != init_err)
  {
    while (1);
  }
}

static sys_error_code_t STHS34PF80TaskSensorInit(STHS34PF80Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  uint8_t id;
  uint32_t status = 0;
  uint8_t boot  = 1;

  sths34pf80_tmos_odr_t tmos_odr;
  sths34pf80_avg_tobject_num_t avg_tobject_num;
  sths34pf80_avg_tambient_num_t avg_tambient_num;
  sths34pf80_lpf_bandwidth_t lpf_bandwidth;
  sths34pf80_tmos_drdy_status_t tmos_drdy_status;
  sths34pf80_tmos_func_status_t tmos_func_status;
  IPD_algo_conf_t tmos_swlib_algo_conf;  // SW library algorithms configuration (change parameters in tmos_swlib_config)
  uint16_t tmos_sensitivity_boot, tmos_sensitivity;

  /* Check power cycling */
  sths34pf80_tmos_odr_get(p_sensor_drv, &tmos_odr);

  if (tmos_odr != STHS34PF80_TMOS_ODR_OFF)
  {
    /* Wait for DRDY */
    do
    {
      sths34pf80_tmos_drdy_status_get(p_sensor_drv, &tmos_drdy_status);
    } while (!tmos_drdy_status.drdy);

    /* Set ODR to 0 */
    if (0 == sths34pf80_tmos_odr_set(p_sensor_drv, STHS34PF80_TMOS_ODR_OFF))
    {
      /* Clear DRDY */
      sths34pf80_tmos_func_status_get(p_sensor_drv, &tmos_func_status);
    }
  }

  sths34pf80_boot_set(p_sensor_drv, boot);

  do
  {
    sths34pf80_boot_get(p_sensor_drv, &boot);
  } while (boot);

  status = sths34pf80_device_id_get(p_sensor_drv, &id);

  if (status == 0U)
  {
    ABusIFSetWhoAmI(_this->p_sensor_bus_if, id);
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("STHS34PF80: sensor - I am 0x%x.\r\n", id));

    /* Set averages */
    if (_this->sensor_status.type.presence.average_tobject < 3)
    {
      avg_tobject_num = STHS34PF80_AVG_TMOS_2;
    }
    else if (_this->sensor_status.type.presence.average_tobject < 9)
    {
      avg_tobject_num = STHS34PF80_AVG_TMOS_8;
    }
    else if (_this->sensor_status.type.presence.average_tobject < 33)
    {
      avg_tobject_num = STHS34PF80_AVG_TMOS_32;
    }
    else if (_this->sensor_status.type.presence.average_tobject < 257)
    {
      avg_tobject_num = STHS34PF80_AVG_TMOS_256;
    }
    else if (_this->sensor_status.type.presence.average_tobject < 513)
    {
      avg_tobject_num = STHS34PF80_AVG_TMOS_512;
    }
    else if (_this->sensor_status.type.presence.average_tobject < 1025)
    {
      avg_tobject_num = STHS34PF80_AVG_TMOS_1024;
    }
    else
    {
      avg_tobject_num = STHS34PF80_AVG_TMOS_2048;
    }

    sths34pf80_avg_tobject_num_set(p_sensor_drv, avg_tobject_num);

    if (_this->sensor_status.type.presence.average_tambient < 2)
    {
      avg_tambient_num = STHS34PF80_AVG_T_1;
    }
    else if (_this->sensor_status.type.presence.average_tambient < 3)
    {
      avg_tambient_num = STHS34PF80_AVG_T_2;
    }
    else if (_this->sensor_status.type.presence.average_tambient < 5)
    {
      avg_tambient_num = STHS34PF80_AVG_T_4;
    }
    else
    {
      avg_tambient_num = STHS34PF80_AVG_T_8;
    }

    sths34pf80_avg_tambient_num_set(p_sensor_drv, avg_tambient_num);

    /* Set gain mode */
    sths34pf80_gain_mode_set(p_sensor_drv, STHS34PF80_GAIN_DEFAULT_MODE);

    /* Set the embedded compensation. */
    sths34pf80_tobject_algo_compensation_set(p_sensor_drv, _this->sensor_status.type.presence.embedded_compensation);

    if (_this->sensor_status.type.presence.embedded_compensation  != 0)
    {
      /* Change sensitivity */
      sths34pf80_tmos_sensitivity_get(p_sensor_drv, &tmos_sensitivity_boot);    // Get default value
      tmos_sensitivity = (uint16_t)_this->sensor_status.type.presence.Transmittance * tmos_sensitivity_boot; // Update default value
      sths34pf80_tmos_sensitivity_set(p_sensor_drv, &tmos_sensitivity);     // Set new value
    }

    /* Set BDU */
    sths34pf80_block_data_update_set(p_sensor_drv, 1);

    if (_this->pIRQConfig != NULL)
    {
      sths34pf80_tmos_route_int_set(p_sensor_drv, STHS34PF80_TMOS_INT_DRDY);

      sths34pf80_int_mode_t val;
      val.pin = STHS34PF80_PUSH_PULL;
      val.polarity = STHS34PF80_ACTIVE_HIGH;
      sths34pf80_int_mode_set(p_sensor_drv, val);
      sths34pf80_drdy_mode_set(p_sensor_drv, STHS34PF80_DRDY_LATCHED);
    }

    /* Select presence algo mode */
    sths34pf80_presence_abs_value_set(p_sensor_drv, 0);

    /* Set thresholds and hysteresis mode */
    if (_this->sensor_status.type.presence.presence_threshold != 0)
    {
      sths34pf80_presence_threshold_set(p_sensor_drv, _this->sensor_status.type.presence.presence_threshold);
    }
    if (_this->sensor_status.type.presence.presence_hysteresis != 0)
    {
      sths34pf80_presence_hysteresis_set(p_sensor_drv, _this->sensor_status.type.presence.presence_hysteresis);
    }
    if (_this->sensor_status.type.presence.motion_threshold != 0)
    {
      sths34pf80_motion_threshold_set(p_sensor_drv, _this->sensor_status.type.presence.motion_threshold);
    }
    if (_this->sensor_status.type.presence.motion_hysteresis != 0)
    {
      sths34pf80_motion_hysteresis_set(p_sensor_drv, _this->sensor_status.type.presence.motion_hysteresis);
    }
    if (_this->sensor_status.type.presence.tambient_shock_threshold != 0)
    {
      sths34pf80_tambient_shock_threshold_set(p_sensor_drv,
                                              _this->sensor_status.type.presence.tambient_shock_threshold);
    }
    if (_this->sensor_status.type.presence.tambient_shock_hysteresis != 0)
    {
      sths34pf80_tambient_shock_hysteresis_set(p_sensor_drv,
                                               _this->sensor_status.type.presence.tambient_shock_hysteresis);
    }

    /* Set algorithm filters */
    if (_this->sensor_status.type.presence.lpf_p_m_bandwidth < 10)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_9;
    }
    else if (_this->sensor_status.type.presence.lpf_p_m_bandwidth < 21)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_20;
    }
    else if (_this->sensor_status.type.presence.lpf_p_m_bandwidth < 51)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_50;
    }
    else if (_this->sensor_status.type.presence.lpf_p_m_bandwidth < 101)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_100;
    }
    else if (_this->sensor_status.type.presence.lpf_p_m_bandwidth < 201)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_200;
    }
    else if (_this->sensor_status.type.presence.lpf_p_m_bandwidth < 401)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_400;
    }
    else
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_800;
    }

    sths34pf80_lpf_p_m_bandwidth_set(p_sensor_drv, lpf_bandwidth);

    if (_this->sensor_status.type.presence.lpf_m_bandwidth < 10)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_9;
    }
    else if (_this->sensor_status.type.presence.lpf_m_bandwidth < 21)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_20;
    }
    else if (_this->sensor_status.type.presence.lpf_m_bandwidth < 51)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_50;
    }
    else if (_this->sensor_status.type.presence.lpf_m_bandwidth < 101)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_100;
    }
    else if (_this->sensor_status.type.presence.lpf_m_bandwidth < 201)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_200;
    }
    else if (_this->sensor_status.type.presence.lpf_m_bandwidth < 401)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_400;
    }
    else
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_800;
    }

    sths34pf80_lpf_m_bandwidth_set(p_sensor_drv, lpf_bandwidth);


    if (_this->sensor_status.type.presence.lpf_p_bandwidth < 10)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_9;
    }
    else if (_this->sensor_status.type.presence.lpf_p_bandwidth < 21)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_20;
    }
    else if (_this->sensor_status.type.presence.lpf_p_bandwidth < 51)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_50;
    }
    else if (_this->sensor_status.type.presence.lpf_p_bandwidth < 101)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_100;
    }
    else if (_this->sensor_status.type.presence.lpf_p_bandwidth < 201)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_200;
    }
    else if (_this->sensor_status.type.presence.lpf_p_bandwidth < 401)
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_400;
    }
    else
    {
      lpf_bandwidth = STHS34PF80_LPF_ODR_DIV_800;
    }

    sths34pf80_lpf_p_bandwidth_set(p_sensor_drv, lpf_bandwidth);

    if (_this->sensor_status.type.presence.software_compensation)
    {
      MX_CRC_Init();

      if (_this->tmos_swlib_instance != NULL)
      {
        /* sw lib already initialized --> reset and delete before new allocation */
        InfraredPD_ResetComp(_this->tmos_swlib_instance);
        InfraredPD_DeleteInstance(_this->tmos_swlib_instance);
      }
      /* sw lib alloc and init */
      InfraredPD_Initialize(IPD_MCU_STM32);
      _this->tmos_swlib_instance = InfraredPD_CreateInstance(&tmos_swlib_algo_conf);
      tmos_swlib_config(_this, tmos_sensitivity_boot, &tmos_swlib_algo_conf);

    }

    /* See AN5867 chapter 7.4: once setup algorithms and/or filters, algorithms must be reset */
    sths34pf80_reset_algo(p_sensor_drv);

    if (_this->sensor_status.type.presence.data_frequency < 2.0f)
    {
      tmos_odr = STHS34PF80_TMOS_ODR_AT_1Hz;
    }
    else if (_this->sensor_status.type.presence.data_frequency < 3.0f)
    {
      tmos_odr = STHS34PF80_TMOS_ODR_AT_2Hz;
    }
    else if (_this->sensor_status.type.presence.data_frequency < 5.0f)
    {
      tmos_odr = STHS34PF80_TMOS_ODR_AT_4Hz;
    }
    else if (_this->sensor_status.type.presence.data_frequency < 9.0f)
    {
      tmos_odr = STHS34PF80_TMOS_ODR_AT_8Hz;
    }
    else if (_this->sensor_status.type.presence.data_frequency < 16.0f)
    {
      tmos_odr = STHS34PF80_TMOS_ODR_AT_15Hz;
    }
    else
    {
      tmos_odr = STHS34PF80_TMOS_ODR_AT_30Hz;
    }

    sths34pf80_tmos_func_status_get(p_sensor_drv, &tmos_func_status); // Clear DRDY

    if (status == 0U && _this->sensor_status.is_active)
    {
      sths34pf80_tmos_odr_set(p_sensor_drv, tmos_odr);
    }
    else
    {
      sths34pf80_tmos_odr_set(p_sensor_drv, STHS34PF80_TMOS_ODR_OFF);
      _this->sensor_status.is_active = false;
    }

    if (_this->sensor_status.is_active)
    {
      _this->sths34pf80_task_cfg_timer_period_ms = (uint16_t)(1000.0f / _this->sensor_status.type.presence.data_frequency);
    }
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorReadData(STHS34PF80Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  if (_this->sensor_status.is_active)
  {
    uint32_t timeout;
    uint32_t tick_start;
    sths34pf80_tmos_drdy_status_t new_data_ready;

    if (_this->IsBlocking == 1U)
    {
      timeout = _this->sths34pf80_task_cfg_timer_period_ms;
    }
    else
    {
      timeout = 0;
    }

    tick_start = HAL_GetTick();

    do
    {
      (void) sths34pf80_tmos_drdy_status_get(p_sensor_drv, &new_data_ready);

      if (new_data_ready.drdy == 1U)
      {
        res = 0;
        break;
      }
    } while ((HAL_GetTick() - tick_start) < timeout);
  }

  /* a new measure is available if no error is returned by the poll function */
  if (res == 0)
  {
    sths34pf80_tmos_func_status_t tmos_func_status;
    int16_t tobject = 0;
    int16_t tambient = 0;
    int16_t tobj_comp = 0;
    int16_t tpresence = 0;
    int16_t tmotion = 0;
    IPD_input_t swlib_in = {0};
    IPD_output_t swlib_out = {0};

    // Clear DRDY and read flags
    if (sths34pf80_tmos_func_status_get(p_sensor_drv, &tmos_func_status) != 0)
    {
      res = -1;
    }
    else if (sths34pf80_tobject_raw_get(p_sensor_drv, &tobject) != 0)
    {
      res = -1;
    }
    else if (sths34pf80_tambient_raw_get(p_sensor_drv, &tambient) != 0)
    {
      res = -1;
    }

    if (_this->sensor_status.type.presence.embedded_compensation != 0)
    {
      if (sths34pf80_tobj_comp_raw_get(p_sensor_drv, &tobj_comp) != 0)
      {
        res = -1;
      }
    }

    if (_this->sensor_status.type.presence.software_compensation != 0)
    {
      swlib_in.t_amb = tambient;
      swlib_in.t_obj = tobject;
      InfraredPD_Update(_this->tmos_swlib_instance, &swlib_in, &swlib_out);
    }

    if (sths34pf80_tpresence_raw_get(p_sensor_drv, &tpresence) != 0)
    {
      res = -1;
    }
    else if (sths34pf80_tmotion_raw_get(p_sensor_drv, &tmotion) != 0)
    {
      res = -1;
    }

    _this->p_sensor_data_buff[0] = tambient;
    _this->p_sensor_data_buff[1] = tobject;
    _this->p_sensor_data_buff[2] = tobj_comp;
    _this->p_sensor_data_buff[3] = tpresence;
    _this->p_sensor_data_buff[4] = tmos_func_status.pres_flag;
    _this->p_sensor_data_buff[5] = tmotion;
    _this->p_sensor_data_buff[6] = tmos_func_status.mot_flag;
    _this->p_sensor_data_buff[7] = swlib_out.t_obj_comp;
    _this->p_sensor_data_buff[8] = swlib_out.t_obj_change;
    _this->p_sensor_data_buff[9] = swlib_out.mot_flag;
    _this->p_sensor_data_buff[10] = swlib_out.pres_flag;

    res = 0;
  }

#if (HSD_USE_DUMMY_DATA == 1)
  uint16_t i = 0;
  int16_t *p16 = (int16_t *)_this->p_sensor_data_buff;

  for (i = 0; i < 11; i++)
  {
    *p16++ = dummyDataCounter++;
  }
#endif

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorRegister(STHS34PF80Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ISensor_t *tof_if = (ISensor_t *) STHS34PF80TaskGetPresenceSensorIF(_this);
  _this->id = SMAddSensor(tof_if);

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorInitTaskParams(STHS34PF80Task *_this)
{

  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* PRESENCE SENSOR STATUS */
  _this->sensor_status.isensor_class = ISENSOR_CLASS_PRESENCE;
  _this->sensor_status.is_active = TRUE;
  _this->sensor_status.type.presence.data_frequency = 15.0f;
  _this->sensor_status.type.presence.Transmittance = 1.0f;
  _this->sensor_status.type.presence.average_tobject = 32;
  _this->sensor_status.type.presence.average_tambient = 8;
  _this->sensor_status.type.presence.presence_threshold = 150;
  _this->sensor_status.type.presence.presence_hysteresis = 50;
  _this->sensor_status.type.presence.motion_threshold = 120;
  _this->sensor_status.type.presence.motion_hysteresis = 50;
  _this->sensor_status.type.presence.tambient_shock_threshold = 20;
  _this->sensor_status.type.presence.tambient_shock_hysteresis = 5;
  _this->sensor_status.type.presence.lpf_p_m_bandwidth = 9;
  _this->sensor_status.type.presence.lpf_p_bandwidth = 400;
  _this->sensor_status.type.presence.lpf_m_bandwidth = 200;
  _this->sensor_status.type.presence.embedded_compensation = 0;
  _this->sensor_status.type.presence.software_compensation = 1;
  _this->sensor_status.type.presence.AlgorithmConfig.comp_type = 2;
  _this->sensor_status.type.presence.AlgorithmConfig.pres_ths = 250;
  _this->sensor_status.type.presence.AlgorithmConfig.mot_ths = 150;
  _this->sensor_status.type.presence.AlgorithmConfig.comp_filter_flag = 1;
  _this->sensor_status.type.presence.AlgorithmConfig.abs_static_flag = 0;

  EMD_Init(&_this->data, (uint8_t *) &_this->p_sensor_data_buff[0], E_EM_INT16, E_EM_MODE_INTERLEAVED, 2, 1, 11);

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorSetDataFrequency(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  float data_frequency = (float) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (data_frequency < 1.0f)
    {
      sths34pf80_tmos_odr_set(p_sensor_drv, STHS34PF80_TMOS_ODR_OFF);
      /* Do not update the model in case of ODR = 0 */
      data_frequency = _this->sensor_status.type.presence.data_frequency;
    }
    else if (data_frequency < 2.0f)
    {
      data_frequency = 1.0f;
    }
    else if (data_frequency < 3.0f)
    {
      data_frequency = 2.0f;
    }
    else if (data_frequency < 5.0f)
    {
      data_frequency = 4.0f;
    }
    else if (data_frequency < 9.0f)
    {
      data_frequency = 8.0f;
    }
    else if (data_frequency < 16.0f)
    {
      data_frequency = 15.0f;
    }
    else
    {
      data_frequency = 30.0f;
    }

    if (!SYS_IS_ERROR_CODE(res))
    {
      _this->sensor_status.type.presence.data_frequency = data_frequency;
      _this->sensor_status.type.presence.measured_data_frequency = 0.0f;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorSetTransmittance(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  float Transmittance = (float) report.sensorMessage.fParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    _this->sensor_status.type.presence.Transmittance = Transmittance;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorSetAverageTObject(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t average_tobject = report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (average_tobject < 3)
    {
      if (sths34pf80_avg_tobject_num_set(p_sensor_drv, STHS34PF80_AVG_TMOS_2) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.average_tobject = 2;
      }
    }
    else if (average_tobject < 9)
    {
      if (sths34pf80_avg_tobject_num_set(p_sensor_drv, STHS34PF80_AVG_TMOS_8) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.average_tobject = 8;
      }
    }
    else if (average_tobject < 33)
    {
      if (sths34pf80_avg_tobject_num_set(p_sensor_drv, STHS34PF80_AVG_TMOS_32) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.average_tobject = 32;
      }
    }
    else if (average_tobject < 129)
    {
      if (sths34pf80_avg_tobject_num_set(p_sensor_drv, STHS34PF80_AVG_TMOS_128) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.average_tobject = 128;
      }
    }
    else if (average_tobject < 257)
    {
      if (sths34pf80_avg_tobject_num_set(p_sensor_drv, STHS34PF80_AVG_TMOS_256) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.average_tobject = 256;
      }
    }
    else if (average_tobject < 513)
    {
      if (sths34pf80_avg_tobject_num_set(p_sensor_drv, STHS34PF80_AVG_TMOS_512) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.average_tobject = 512;
      }
    }
    else if (average_tobject < 1025)
    {
      if (sths34pf80_avg_tobject_num_set(p_sensor_drv, STHS34PF80_AVG_TMOS_1024) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.average_tobject = 1024;
      }
    }
    else
    {
      if (sths34pf80_avg_tobject_num_set(p_sensor_drv, STHS34PF80_AVG_TMOS_2048) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.average_tobject = 2048;
      }
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorSetAverageTAmbient(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t average_tambient = report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (average_tambient < 2)
    {
      if (sths34pf80_avg_tambient_num_set(p_sensor_drv, STHS34PF80_AVG_T_1) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.average_tambient = 1;
      }
    }
    else if (average_tambient < 3)
    {
      if (sths34pf80_avg_tambient_num_set(p_sensor_drv, STHS34PF80_AVG_T_2) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.average_tambient = 2;
      }
    }
    else if (average_tambient < 5)
    {
      if (sths34pf80_avg_tambient_num_set(p_sensor_drv, STHS34PF80_AVG_T_4) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.average_tambient = 4;
      }
    }
    else
    {
      if (sths34pf80_avg_tambient_num_set(p_sensor_drv, STHS34PF80_AVG_T_8) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.average_tambient = 8;
      }
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorSetPresenceThreshold(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t presence_threshold = report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (sths34pf80_presence_threshold_set(p_sensor_drv, presence_threshold) != 0)
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }
    else
    {
      _this->sensor_status.type.presence.presence_threshold = presence_threshold;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorSetPresenceHysteresis(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t presence_hysteresis = report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (sths34pf80_presence_hysteresis_set(p_sensor_drv, presence_hysteresis) != 0)
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }
    else
    {
      _this->sensor_status.type.presence.presence_hysteresis = presence_hysteresis;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorSetMotionThreshold(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t motion_threshold = report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (sths34pf80_motion_threshold_set(p_sensor_drv, motion_threshold) != 0)
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }
    else
    {
      _this->sensor_status.type.presence.motion_threshold = motion_threshold;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorSetMotionHysteresis(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint8_t motion_hysteresis = report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (sths34pf80_motion_hysteresis_set(p_sensor_drv, motion_hysteresis) != 0)
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }
    else
    {
      _this->sensor_status.type.presence.motion_hysteresis = motion_hysteresis;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorSetTAmbientShockThreshold(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t tambient_shock_threshold = report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (sths34pf80_tambient_shock_threshold_set(p_sensor_drv, tambient_shock_threshold) != 0)
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }
    else
    {
      _this->sensor_status.type.presence.tambient_shock_threshold = tambient_shock_threshold;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorSetTAmbientShockHysteresis(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t tambient_shock_hysteresis = report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (sths34pf80_tambient_shock_hysteresis_set(p_sensor_drv, tambient_shock_hysteresis) != 0)
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }
    else
    {
      _this->sensor_status.type.presence.tambient_shock_hysteresis = tambient_shock_hysteresis;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorSetLPF_P_M_Bandwidth(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t lpf_p_m_bandwidth = report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (lpf_p_m_bandwidth < 10)
    {
      if (sths34pf80_lpf_p_m_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_9) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_p_m_bandwidth = 9;
      }
    }
    else if (lpf_p_m_bandwidth < 21)
    {
      if (sths34pf80_lpf_p_m_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_20) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_p_m_bandwidth = 20;
      }
    }
    else if (lpf_p_m_bandwidth < 51)
    {
      if (sths34pf80_lpf_p_m_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_50) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_p_m_bandwidth = 50;
      }
    }
    else if (lpf_p_m_bandwidth < 101)
    {
      if (sths34pf80_lpf_p_m_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_100) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_p_m_bandwidth = 100;
      }
    }
    else if (lpf_p_m_bandwidth < 201)
    {
      if (sths34pf80_lpf_p_m_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_200) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_p_m_bandwidth = 200;
      }
    }
    else if (lpf_p_m_bandwidth < 401)
    {
      if (sths34pf80_lpf_p_m_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_400) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_p_m_bandwidth = 400;
      }
    }
    else
    {
      if (sths34pf80_lpf_p_m_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_800) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_p_m_bandwidth = 800;
      }
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorSetLPF_P_Bandwidth(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t lpf_p_bandwidth = report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (lpf_p_bandwidth < 10)
    {
      if (sths34pf80_lpf_p_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_9) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_p_bandwidth = 9;
      }
    }
    else if (lpf_p_bandwidth < 21)
    {
      if (sths34pf80_lpf_p_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_20) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_p_bandwidth = 20;
      }
    }
    else if (lpf_p_bandwidth < 51)
    {
      if (sths34pf80_lpf_p_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_50) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_p_bandwidth = 50;
      }
    }
    else if (lpf_p_bandwidth < 101)
    {
      if (sths34pf80_lpf_p_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_100) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_p_bandwidth = 100;
      }
    }
    else if (lpf_p_bandwidth < 201)
    {
      if (sths34pf80_lpf_p_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_200) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_p_bandwidth = 200;
      }
    }
    else if (lpf_p_bandwidth < 401)
    {
      if (sths34pf80_lpf_p_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_400) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_p_bandwidth = 400;
      }
    }
    else
    {
      if (sths34pf80_lpf_p_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_800) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_p_bandwidth = 800;
      }
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorSetLPF_M_Bandwidth(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint16_t lpf_m_bandwidth = report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (lpf_m_bandwidth < 10)
    {
      if (sths34pf80_lpf_m_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_9) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_m_bandwidth = 9;
      }
    }
    else if (lpf_m_bandwidth < 21)
    {
      if (sths34pf80_lpf_m_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_20) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_m_bandwidth = 20;
      }
    }
    else if (lpf_m_bandwidth < 51)
    {
      if (sths34pf80_lpf_m_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_50) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_m_bandwidth = 50;
      }
    }
    else if (lpf_m_bandwidth < 101)
    {
      if (sths34pf80_lpf_m_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_100) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_m_bandwidth = 100;
      }
    }
    else if (lpf_m_bandwidth < 201)
    {
      if (sths34pf80_lpf_m_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_200) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_m_bandwidth = 200;
      }
    }
    else if (lpf_m_bandwidth < 401)
    {
      if (sths34pf80_lpf_m_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_400) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_m_bandwidth = 400;
      }
    }
    else
    {
      if (sths34pf80_lpf_m_bandwidth_set(p_sensor_drv, STHS34PF80_LPF_ODR_DIV_800) != 0)
      {
        res = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      else
      {
        _this->sensor_status.type.presence.lpf_m_bandwidth = 800;
      }
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorSetEmbeddedCompensation(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint8_t embedded_compensation = (uint8_t)report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    if (sths34pf80_tobject_algo_compensation_set(p_sensor_drv, embedded_compensation) != 0)
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }
    else
    {
      _this->sensor_status.type.presence.embedded_compensation = embedded_compensation;
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorSetSoftwareCompensation(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint8_t software_compensation = (uint8_t)report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    _this->sensor_status.type.presence.software_compensation = software_compensation;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorConfigSoftwareCompensation(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  CompensationAlgorithmConfig_t *pConfig = (CompensationAlgorithmConfig_t *) report.sensorMessage.nParam;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    _this->sensor_status.type.presence.AlgorithmConfig.comp_type = pConfig->comp_type;
    _this->sensor_status.type.presence.AlgorithmConfig.pres_ths = pConfig->pres_ths;
    _this->sensor_status.type.presence.AlgorithmConfig.mot_ths = pConfig->mot_ths;
    _this->sensor_status.type.presence.AlgorithmConfig.comp_filter_flag = (pConfig->comp_filter_flag == 0U) ? 0U : 1U;
    _this->sensor_status.type.presence.AlgorithmConfig.abs_static_flag = (pConfig->abs_static_flag == 0U) ? 0U : 1U;
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskSensorEnable(STHS34PF80Task *_this, SMMessage report)
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

static sys_error_code_t STHS34PF80TaskSensorDisable(STHS34PF80Task *_this, SMMessage report)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;
  uint8_t id = report.sensorMessage.nSensorId;

  if (id == _this->id)
  {
    _this->sensor_status.is_active = FALSE;
    sths34pf80_tmos_odr_set(p_sensor_drv, STHS34PF80_TMOS_ODR_OFF);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  return res;
}

static boolean_t STHS34PF80TaskSensorIsActive(const STHS34PF80Task *_this)
{
  assert_param(_this != NULL);
  return _this->sensor_status.is_active;
}

static sys_error_code_t STHS34PF80TaskEnterLowPowerMode(const STHS34PF80Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *p_sensor_drv = (stmdev_ctx_t *) &_this->p_sensor_bus_if->m_xConnector;

  if (sths34pf80_tmos_odr_set(p_sensor_drv, STHS34PF80_TMOS_ODR_OFF))
  {
    res = SYS_SENSOR_TASK_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_OP_ERROR_CODE);
  }

  return res;
}

static sys_error_code_t STHS34PF80TaskConfigureIrqPin(const STHS34PF80Task *_this, boolean_t LowPower)
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

static void STHS34PF80TaskTimerCallbackFunction(ULONG param)
{
  STHS34PF80Task *p_obj = (STHS34PF80Task *) param;
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

void STHS34PF80Task_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;
  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t) nPin);
  if (p_val != NULL)
  {
    p_queue = &((STHS34PF80Task *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      /* unable to send the report. Signal the error */
      sys_error_handler();
    }
  }
}
