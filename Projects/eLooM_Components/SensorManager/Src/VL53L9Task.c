/**
  ******************************************************************************
  * @file    VL53L9Task.c
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
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "VL53L9Task.h"
#include "VL53L9Task_vtbl.h"
#include "SMMessageParser.h"
#include "SensorCommands.h"
#include "SensorManager.h"
#include "SensorRegister.h"
#include "I3CBusIF.h"
#include "events/IDataEventListener.h"
#include "events/IDataEventListener_vtbl.h"
#include "services/SysTimestamp.h"
#include "services/ManagedTaskMap.h"
#include "services/sysdebug.h"

#include <stddef.h>
#include <string.h>

/* Private includes ----------------------------------------------------------*/

#ifndef VL53L9_TASK_CFG_STACK_DEPTH
#define VL53L9_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*12)
#endif

#ifndef VL53L9_TASK_CFG_PRIORITY
#define VL53L9_TASK_CFG_PRIORITY                 (4)
#endif

#ifndef VL53L9_TASK_CFG_IN_QUEUE_LENGTH
#define VL53L9_TASK_CFG_IN_QUEUE_LENGTH          20u
#endif

#define VL53L9_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(SMMessage)

#ifndef VL53L9_TASK_CFG_TIMER_PERIOD_MS
#define VL53L9_TASK_CFG_TIMER_PERIOD_MS          100u
#endif

/* Default binning: 2 = 54x42 zones, matching the AR precision reference profile. */
#ifndef VL53L9_TASK_CFG_DEFAULT_BINNING
#define VL53L9_TASK_CFG_DEFAULT_BINNING          2u
#endif

#ifndef VL53L9_TASK_CFG_MAX_INSTANCES_COUNT
#define VL53L9_TASK_CFG_MAX_INSTANCES_COUNT      1u
#endif

/* Frequency (Hz) of the clock supplied to the sensor CLK_IN pin.
 * Set to 12000000UL if the STM32 drives CLK_IN via TIM PWM / MCO (SW1=INT on NUCLEO).
 * Set to 0UL if CLK_IN is not connected (sensor uses its internal oscillator).
 * A mismatch causes PLL lock failure and the sensor never enters STREAMING state. */
#ifndef VL53L9_TASK_CFG_EXT_CLOCK_HZ
#define VL53L9_TASK_CFG_EXT_CLOCK_HZ            12000000UL
#endif

#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_VL53L9, level, message)

#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#if (HSD_USE_DUMMY_DATA == 1)
static int16_t dummyDataCounter = 0;
#endif

/**
  * Class object declaration
  */
typedef struct _VL53L9TaskClass
{
  /**
    * VL53L9Task class virtual table.
    */
  const AManagedTaskEx_vtbl vtbl;

  /**
    * Time-of-flight IF virtual table.
    */
  const ISensorRanging_vtbl sensor_if_vtbl;

  /**
    * Specifies time-of-flight sensor capabilities.
    */
  const SensorDescriptor_t class_descriptor;

  /**
    * VL53L9Task (PM_STATE, ExecuteStepFunc) map.
    */
  const pExecuteStepFunc_t p_pm_state2func_map[3];

  /**
    * Memory buffer used to allocate the map (key, value).
    */
  MTMapElement_t task_map_elements[VL53L9_TASK_CFG_MAX_INSTANCES_COUNT];

  /**
    * This map links the EXTI pin callback to the corresponding task instance.
    */
  MTMap_t task_map;
} VL53L9TaskClass_t;

/* Private member function declaration */
/* *********************************** */
/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t VL53L9TaskExecuteStepState1(AManagedTask *_this);

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t VL53L9TaskExecuteStepDatalog(AManagedTask *_this);

/**
  * Initialize the sensor according to the current task parameters.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t VL53L9TaskSensorInit(VL53L9Task *_this);

/**
  * Read one frame from the sensor and decode it into the task output buffer.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t VL53L9TaskSensorReadData(VL53L9Task *_this);

/**
  * Register the sensor with the SensorManager and cache the assigned ID.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
  */
static sys_error_code_t VL53L9TaskSensorRegister(VL53L9Task *_this);

/**
  * Initialize the default sensor parameters used at startup.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
  */
static sys_error_code_t VL53L9TaskSensorInitTaskParams(VL53L9Task *_this);

/**
  * Private implementation of sensor interface methods for VL53L9 sensor.
  */
static sys_error_code_t VL53L9TaskSensorSetFrequency(VL53L9Task *_this, SMMessage report);
static sys_error_code_t VL53L9TaskSensorSetResolution(VL53L9Task *_this, SMMessage report);
static sys_error_code_t VL53L9TaskSensorSetRangingMode(VL53L9Task *_this, SMMessage report);
static sys_error_code_t VL53L9TaskSensorSetIntegrationTime(VL53L9Task *_this, SMMessage report);
static sys_error_code_t VL53L9TaskSensorConfigIt(VL53L9Task *_this, SMMessage report);
static sys_error_code_t VL53L9TaskSensorSetAddress(VL53L9Task *_this, SMMessage report);
static sys_error_code_t VL53L9TaskSensorSetPowerMode(VL53L9Task *_this, SMMessage report);
static sys_error_code_t VL53L9TaskSensorEnableCmd(VL53L9Task *_this, SMMessage report);
static sys_error_code_t VL53L9TaskSensorDisableCmd(VL53L9Task *_this, SMMessage report);

/* Inline function forward declaration */
/* *********************************** */
/**
  * Post a message to the front of the task queue.
  *
  * @param _this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_ERROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE otherwise.
  */
static inline sys_error_code_t VL53L9TaskPostReportToFront(VL53L9Task *_this, SMMessage *pReport);

/**
  * Post a message to the back of the task queue.
  *
  * @param _this [IN] specifies a pointer to the task object.
  * @param pReport [IN] specifies a report to send.
  * @return SYS_NO_ERROR_CODE if success, SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE otherwise.
  */
static inline sys_error_code_t VL53L9TaskPostReportToBack(VL53L9Task *_this, SMMessage *pReport);

/**
  * Callback function called when the software timer expires.
  *
  * @param param [IN] specifies an application defined parameter.
  */
static void VL53L9TaskTimerCallbackFunction(ULONG param);

/**
  * Map a VL53L9 driver status into the system error domain.
  *
  * @param error [IN] driver status code.
  * @return SYS_NO_ERROR_CODE if success, SYS_UNDEFINED_ERROR_CODE otherwise.
  */
static sys_error_code_t VL53L9TaskMapDriverError(int32_t error);

/**
  * Bind bus IF callbacks and electrical properties to the VL53L9 driver interface.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
  */
static sys_error_code_t VL53L9TaskBindDriverIF(VL53L9Task *_this);

/**
  * Convert matrix resolution to VL53L9 binning setting.
  */
static uint8_t VL53L9TaskResolutionToBinning(uint32_t resolution);

/**
  * Convert VL53L9 binning setting to matrix resolution.
  */
static uint16_t VL53L9TaskBinningToResolution(uint8_t binning);

/*
 * Platform specific function to control the sensor power supply.
 */
static int platform_power_reset(const VL53L9Task *_this);
static int platform_assign_dynamic_address(VL53L9Task *_this);

/**
  * Configure the optional IRQ pin for active mode or low-power mode.
  */
static sys_error_code_t VL53L9TaskConfigureIrqPin(const VL53L9Task *_this, boolean_t low_power);

/* Objects instance */
/* **************** */
/**
  * The class object.
  */
static VL53L9TaskClass_t sTheClass =
{
  {
    VL53L9Task_vtblHardwareInit,
    VL53L9Task_vtblOnCreateTask,
    VL53L9Task_vtblDoEnterPowerMode,
    VL53L9Task_vtblHandleError,
    VL53L9Task_vtblOnEnterTaskControlLoop,
    VL53L9Task_vtblForceExecuteStep,
    VL53L9Task_vtblOnEnterPowerMode
  },
  {
    {
      {
        VL53L9Task_vtblTofGetId,
        VL53L9Task_vtblGetEventSourceIF,
        VL53L9Task_vtblTofGetDataInfo
      },
      VL53L9Task_vtblSensorEnable,
      VL53L9Task_vtblSensorDisable,
      VL53L9Task_vtblSensorIsEnabled,
      VL53L9Task_vtblSensorGetDescription,
      VL53L9Task_vtblSensorGetStatus,
      VL53L9Task_vtblSensorGetStatusPointer
    },
    VL53L9Task_vtblTofGetProfile,
    VL53L9Task_vtblTofGetIT,
    VL53L9Task_vtblTofGetAddress,
    VL53L9Task_vtblTofGetPowerMode,
    VL53L9Task_vtblSensorSetFrequency,
    VL53L9Task_vtblSensorSetResolution,
    VL53L9Task_vtblSensorSetRangingMode,
    VL53L9Task_vtblSensorSetIntegrationTime,
    VL53L9Task_vtblSensorConfigIT,
    VL53L9Task_vtblSensorSetAddress,
    VL53L9Task_vtblSensorSetPowerMode
  },
  {
    "vl53l9",
    COM_TYPE_TOF
  },
  {
    VL53L9TaskExecuteStepState1,
    NULL,
    VL53L9TaskExecuteStepDatalog,
  },
  {{0}},
  {0}
};

ISourceObservable *VL53L9TaskGetTofSensorIF(VL53L9Task *_this)
{
  assert_param(_this != NULL);

  return (ISourceObservable *)&_this->sensor_if;
}

AManagedTaskEx *VL53L9TaskAlloc(const void *pIRQConfig, const void *pXSHUTConfig, const MX_TIMParams_t *pTIMConfig)
{
  VL53L9Task *p_new_obj = SysAlloc(sizeof(VL53L9Task));

  if (p_new_obj != NULL)
  {
    AMTInitEx(&p_new_obj->super);
    p_new_obj->super.vptr = &sTheClass.vtbl;
    p_new_obj->sensor_if.vptr = &sTheClass.sensor_if_vtbl;
    p_new_obj->sensor_descriptor = &sTheClass.class_descriptor;
    p_new_obj->pIRQConfig = (const MX_GPIOParams_t *)pIRQConfig;
    p_new_obj->pXSHUTConfig = (const MX_GPIOParams_t *)pXSHUTConfig;
    p_new_obj->pTIMConfig = pTIMConfig;
    strcpy(p_new_obj->sensor_status.p_name, sTheClass.class_descriptor.p_name);
  }

  return (AManagedTaskEx *)p_new_obj;
}

AManagedTaskEx *VL53L9TaskAllocSetName(const void *pIRQConfig, const void *pXSHUTConfig,
                                       const MX_TIMParams_t *pTIMConfig, const char *p_name)
{
  VL53L9Task *p_new_obj = (VL53L9Task *)VL53L9TaskAlloc(pIRQConfig, pXSHUTConfig, pTIMConfig);

  if ((p_new_obj != NULL) && (p_name != NULL))
  {
    strcpy(p_new_obj->sensor_status.p_name, p_name);
  }

  return (AManagedTaskEx *)p_new_obj;
}

AManagedTaskEx *VL53L9TaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pXSHUTConfig,
                                      const MX_TIMParams_t *pTIMConfig)
{
  VL53L9Task *p_obj = (VL53L9Task *)p_mem_block;

  if (p_obj != NULL)
  {
    AMTInitEx(&p_obj->super);
    p_obj->super.vptr = &sTheClass.vtbl;
    p_obj->sensor_if.vptr = &sTheClass.sensor_if_vtbl;
    p_obj->sensor_descriptor = &sTheClass.class_descriptor;
    p_obj->pIRQConfig = (const MX_GPIOParams_t *)pIRQConfig;
    p_obj->pXSHUTConfig = (const MX_GPIOParams_t *)pXSHUTConfig;
    p_obj->pTIMConfig = pTIMConfig;
    strcpy(p_obj->sensor_status.p_name, sTheClass.class_descriptor.p_name);
  }

  return (AManagedTaskEx *)p_obj;
}

AManagedTaskEx *VL53L9TaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pXSHUTConfig,
                                             const MX_TIMParams_t *pTIMConfig, const char *p_name)
{
  VL53L9Task *p_obj = (VL53L9Task *)VL53L9TaskStaticAlloc(p_mem_block, pIRQConfig, pXSHUTConfig, pTIMConfig);

  if ((p_obj != NULL) && (p_name != NULL))
  {
    strcpy(p_obj->sensor_status.p_name, p_name);
  }

  return (AManagedTaskEx *)p_obj;
}

ABusIF *VL53L9TaskGetSensorIF(VL53L9Task *_this)
{
  assert_param(_this != NULL);

  return _this->p_sensor_bus_if;
}

IEventSrc *VL53L9TaskGetEventSrcIF(VL53L9Task *_this)
{
  assert_param(_this != NULL);

  return _this->p_event_src;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t VL53L9Task_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this != NULL);

  (void)_this;
  (void)pParams;
  return SYS_NO_ERROR_CODE;
}

sys_error_code_t VL53L9Task_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                             VOID **pvStackStart,
                                             ULONG *pStackDepth, UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart,
                                             ULONG *pParams)
{
  assert_param(_this != NULL);

  sys_error_code_t res = SYS_NO_ERROR_CODE;
  VL53L9Task *p_obj = (VL53L9Task *)_this;
  uint32_t item_size = (uint32_t)VL53L9_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *p_queue_items_buff = SysAlloc(VL53L9_TASK_CFG_IN_QUEUE_LENGTH * item_size);

  if (p_queue_items_buff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "VL53L9_Q", item_size / 4u, p_queue_items_buff,
                                    VL53L9_TASK_CFG_IN_QUEUE_LENGTH * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  if (TX_SUCCESS != tx_timer_create(&p_obj->read_timer, "VL53L9_T", VL53L9TaskTimerCallbackFunction, (ULONG)_this,
                                    AMT_MS_TO_TICKS(VL53L9_TASK_CFG_TIMER_PERIOD_MS), 0, TX_NO_ACTIVATE))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  p_obj->p_sensor_bus_if = I3CBusIFAlloc(0U, VL53L9_TASK_CFG_STATIC_ADDRESS, 0U, 0U);
  if (p_obj->p_sensor_bus_if == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  (void)I3CBusIFSetRegAddrSize((I3CBusIF *)p_obj->p_sensor_bus_if, I3C_BUS_REG_ADDR_SIZE_16BIT);

  p_obj->p_event_src = DataEventSrcAlloc();
  if (p_obj->p_event_src == NULL)
  {
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  IEventSrcInit(p_obj->p_event_src);

  if (!MTMap_IsInitialized(&sTheClass.task_map))
  {
    /* Initialize EXTI pin-to-task lookup once for all task instances. */
    (void)MTMap_Init(&sTheClass.task_map, sTheClass.task_map_elements, VL53L9_TASK_CFG_MAX_INSTANCES_COUNT);
  }

  if (p_obj->pIRQConfig != NULL)
  {
    MTMapElement_t *p_element = NULL;
    uint32_t key = (uint32_t)p_obj->pIRQConfig->pin;
    p_element = MTMap_AddElement(&sTheClass.task_map, key, _this);
    if (p_element == NULL)
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_LOW_LEVEL_ERROR_CODE(res);
      return res;
    }
  }

  p_obj->p_raw_frame = NULL;
  p_obj->raw_frame_size = 0U;
  p_obj->current_binning = VL53L9_TASK_CFG_DEFAULT_BINNING;
  p_obj->current_resolution = VL53L9TaskBinningToResolution(p_obj->current_binning);
  p_obj->sensor_data_capacity = 0U;
  p_obj->frame_ready = FALSE;
  p_obj->frame_pending = FALSE;
  p_obj->id = 0U;
  _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  *pTaskCode = AMTExRun;
  *pName = "VL53L9";
  *pvStackStart = NULL;
  *pStackDepth = VL53L9_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG)_this;
  *pPriority = VL53L9_TASK_CFG_PRIORITY;
  *pPreemptThreshold = VL53L9_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;

  res = VL53L9TaskSensorInitTaskParams(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  res = VL53L9TaskSensorRegister(p_obj);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("VL53L9: unable to register with DB\r\n"));
    sys_error_handler();
  }

  return res;
}

sys_error_code_t VL53L9Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                 const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);

  VL53L9Task *p_obj = (VL53L9Task *)_this;
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (NewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    /* Re-initialize the sensor each time acquisition mode is entered. */
    if (p_obj->sensor_status.is_active == TRUE)
    {
      /* Reset I3C CCC config flag to force reconfiguration on re-entry. */
      if (p_obj->p_sensor_bus_if != NULL)
      {
        I3CBusIF *p_i3c_bus_if = (I3CBusIF *)p_obj->p_sensor_bus_if;
        p_i3c_bus_if->ccc_config_done = 0U;
      }

      SMMessage report =
      {
        .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
        .sensorMessage.nCmdID = SENSOR_CMD_ID_INIT,
        .sensorMessage.nSensorId = p_obj->id
      };
      res = VL53L9TaskPostReportToBack(p_obj, &report);
    }
  }
  else if ((NewPowerMode == E_POWER_MODE_STATE1) && (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    /* Leaving acquisition mode: stop triggers, stop sensor and flush the queue. */
    if (p_obj->pIRQConfig == NULL)
    {
      tx_timer_deactivate(&p_obj->read_timer);
    }
    else
    {
      VL53L9TaskConfigureIrqPin(p_obj, TRUE);
    }
    if (p_obj->sensor_status.is_active == TRUE)
    {
      (void)vl53l9_stop(&p_obj->tof_driver_if);
    }
    tx_queue_flush(&p_obj->in_queue);
    p_obj->frame_ready = FALSE;
    p_obj->frame_pending = FALSE;
    if (p_obj->p_raw_frame != NULL)
    {
      SysFree(p_obj->p_raw_frame);
      p_obj->p_raw_frame = NULL;
    }
  }
  else if (NewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    if (p_obj->pIRQConfig == NULL)
    {
      tx_timer_deactivate(&p_obj->read_timer);
    }
    else
    {
      VL53L9TaskConfigureIrqPin(p_obj, TRUE);
    }
    p_obj->frame_ready = FALSE;
    p_obj->frame_pending = FALSE;
  }

  return res;
}

sys_error_code_t VL53L9Task_vtblHandleError(AManagedTask *_this, SysEvent Error)
{
  assert_param(_this != NULL);

  (void)_this;
  (void)Error;
  return SYS_NO_ERROR_CODE;
}

sys_error_code_t VL53L9Task_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);

  (void)_this;
  return SYS_NO_ERROR_CODE;
}

sys_error_code_t VL53L9Task_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode)
{
  assert_param(_this != NULL);

  VL53L9Task *p_obj = (VL53L9Task *)_this;
  SMMessage report =
  {
    .internalMessageFE.messageId = SM_MESSAGE_ID_FORCE_STEP,
    .internalMessageFE.nData = 0
  };

  if ((ActivePowerMode == E_POWER_MODE_STATE1) || (ActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      return VL53L9TaskPostReportToFront(p_obj, &report);
    }
  }

  return SYS_NO_ERROR_CODE;
}

sys_error_code_t VL53L9Task_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                 const EPowerMode NewPowerMode)
{
  assert_param(_this != NULL);

  (void)_this;
  (void)ActivePowerMode;
  (void)NewPowerMode;
  return SYS_NO_ERROR_CODE;
}

// ISensor virtual functions definition
// *******************************************

uint8_t VL53L9Task_vtblTofGetId(ISourceObservable *_this)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  return p_if_owner->id;
}

IEventSrc *VL53L9Task_vtblGetEventSourceIF(ISourceObservable *_this)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  return p_if_owner->p_event_src;
}

EMData_t VL53L9Task_vtblTofGetDataInfo(ISourceObservable *_this)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  return p_if_owner->data;
}

sys_error_code_t VL53L9Task_vtblSensorEnable(ISensor_t *_this)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *)p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *)_this);
  SMMessage report =
  {
    .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
    .sensorMessage.nCmdID = SENSOR_CMD_ID_ENABLE,
    .sensorMessage.nSensorId = sensor_id,
  };

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    return SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }

  p_if_owner->sensor_status.is_active = TRUE;
  return VL53L9TaskPostReportToBack(p_if_owner, &report);
}

sys_error_code_t VL53L9Task_vtblSensorDisable(ISensor_t *_this)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *)p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *)_this);
  SMMessage report =
  {
    .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
    .sensorMessage.nCmdID = SENSOR_CMD_ID_DISABLE,
    .sensorMessage.nSensorId = sensor_id,
  };

  if ((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled(_this))
  {
    return SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }

  p_if_owner->sensor_status.is_active = FALSE;
  return VL53L9TaskPostReportToBack(p_if_owner, &report);
}

boolean_t VL53L9Task_vtblSensorIsEnabled(ISensor_t *_this)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  return (ISourceGetId((ISourceObservable *)_this) == p_if_owner->id) ? p_if_owner->sensor_status.is_active : FALSE;
}

SensorDescriptor_t VL53L9Task_vtblSensorGetDescription(ISensor_t *_this)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  return *p_if_owner->sensor_descriptor;
}

SensorStatus_t VL53L9Task_vtblSensorGetStatus(ISensor_t *_this)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  return p_if_owner->sensor_status;
}

SensorStatus_t *VL53L9Task_vtblSensorGetStatusPointer(ISensor_t *_this)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  return &p_if_owner->sensor_status;
}

sys_error_code_t VL53L9Task_vtblTofGetProfile(ISensorRanging_t *_this, ProfileConfig_t *p_config)
{
  assert_param(_this != NULL);
  assert_param(p_config != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  *p_config = p_if_owner->sensor_status.type.ranging.profile_config;
  return SYS_NO_ERROR_CODE;
}

sys_error_code_t VL53L9Task_vtblTofGetIT(ISensorRanging_t *_this, ITConfig_t *p_it_config)
{
  assert_param(_this != NULL);
  assert_param(p_it_config != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  *p_it_config = p_if_owner->sensor_status.type.ranging.it_config;
  return SYS_NO_ERROR_CODE;
}

uint32_t VL53L9Task_vtblTofGetAddress(ISensorRanging_t *_this)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  return p_if_owner->sensor_status.type.ranging.address;
}

uint32_t VL53L9Task_vtblTofGetPowerMode(ISensorRanging_t *_this)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  return p_if_owner->sensor_status.type.ranging.power_mode;
}

sys_error_code_t VL53L9Task_vtblSensorSetFrequency(ISensorRanging_t *_this, uint32_t frequency)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *)p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *)_this);

  p_if_owner->sensor_status.type.ranging.profile_config.frequency = frequency;

  SMMessage report =
  {
    .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
    .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_FREQUENCY,
    .sensorMessage.nSensorId = sensor_id,
    .sensorMessage.nParam = frequency
  };

  if (((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this)) || (frequency == 0U))
  {
    return SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }

  return VL53L9TaskPostReportToBack(p_if_owner, &report);
}

sys_error_code_t VL53L9Task_vtblSensorSetResolution(ISensorRanging_t *_this, uint8_t resolution)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  uint8_t sensor_id = ISourceGetId((ISourceObservable *)_this);

  p_if_owner->current_binning = resolution;
  p_if_owner->current_resolution = VL53L9TaskBinningToResolution(resolution);
  p_if_owner->sensor_status.type.ranging.profile_config.ranging_profile = p_if_owner->current_resolution;

  vl53l9_get_raw_buffer_size(p_if_owner->current_binning, &p_if_owner->raw_frame_size);

  EMD_Init(&p_if_owner->data, p_if_owner->p_raw_frame, E_EM_UINT8, E_EM_MODE_INTERLEAVED, 2, 1,
           p_if_owner->raw_frame_size);

  SMMessage report =
  {
    .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
    .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_RESOLUTION,
    .sensorMessage.nSensorId = sensor_id,
    .sensorMessage.nParam = resolution
  };

  return VL53L9TaskPostReportToBack(p_if_owner, &report);
}

sys_error_code_t VL53L9Task_vtblSensorSetRangingMode(ISensorRanging_t *_this, uint8_t mode)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *)p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *)_this);

  p_if_owner->sensor_status.type.ranging.profile_config.mode = mode;

  SMMessage report =
  {
    .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
    .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_RANGING_MODE,
    .sensorMessage.nSensorId = sensor_id,
    .sensorMessage.nParam = mode
  };

  if (((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
      || (mode > VL53L9_SYNC_AUTONOMOUS))
  {
    return SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }

  return VL53L9TaskPostReportToBack(p_if_owner, &report);
}

sys_error_code_t VL53L9Task_vtblSensorSetIntegrationTime(ISensorRanging_t *_this, uint32_t timing_budget)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *)p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *)_this);

  p_if_owner->sensor_status.type.ranging.profile_config.timing_budget = timing_budget;

  SMMessage report =
  {
    .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
    .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_INTEGRATION_TIME,
    .sensorMessage.nSensorId = sensor_id,
    .sensorMessage.nParam = timing_budget
  };

  if (((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this)) || (timing_budget < 1U)
      || (timing_budget > 30U))
  {
    return SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }

  return VL53L9TaskPostReportToBack(p_if_owner, &report);
}

sys_error_code_t VL53L9Task_vtblSensorConfigIT(ISensorRanging_t *_this, ITConfig_t *p_it_config)
{
  assert_param(_this != NULL);
  assert_param(p_it_config != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  uint8_t sensor_id = ISourceGetId((ISourceObservable *)_this);
  SMMessage report =
  {
    .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
    .sensorMessage.nCmdID = SENSOR_CMD_ID_CONFIG_IT,
    .sensorMessage.nSensorId = sensor_id,
    .sensorMessage.nParam = (uint32_t)p_it_config
  };

  return VL53L9TaskPostReportToBack(p_if_owner, &report);
}

sys_error_code_t VL53L9Task_vtblSensorSetAddress(ISensorRanging_t *_this, uint32_t address)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  uint8_t sensor_id = ISourceGetId((ISourceObservable *)_this);
  SMMessage report =
  {
    .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
    .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_ADDRESS,
    .sensorMessage.nSensorId = sensor_id,
    .sensorMessage.nParam = address
  };

  return VL53L9TaskPostReportToBack(p_if_owner, &report);
}

sys_error_code_t VL53L9Task_vtblSensorSetPowerMode(ISensorRanging_t *_this, uint32_t power_mode)
{
  assert_param(_this != NULL);

  VL53L9Task *p_if_owner = (VL53L9Task *)((uint32_t)_this - offsetof(VL53L9Task, sensor_if));
  EPowerMode log_status = AMTGetTaskPowerMode((AManagedTask *)p_if_owner);
  uint8_t sensor_id = ISourceGetId((ISourceObservable *)_this);

  p_if_owner->sensor_status.type.ranging.power_mode = power_mode;

  SMMessage report =
  {
    .sensorMessage.messageId = SM_MESSAGE_ID_SENSOR_CMD,
    .sensorMessage.nCmdID = SENSOR_CMD_ID_SET_POWERMODE,
    .sensorMessage.nSensorId = sensor_id,
    .sensorMessage.nParam = power_mode
  };

  if (((log_status == E_POWER_MODE_SENSORS_ACTIVE) && ISensorIsEnabled((ISensor_t *)_this))
      || (power_mode > VL53L9_POWER_ULTRA_LOW))
  {
    return SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }

  return VL53L9TaskPostReportToBack(p_if_owner, &report);
}

static sys_error_code_t VL53L9TaskExecuteStepState1(AManagedTask *_this)
{
  assert_param(_this != NULL);

  VL53L9Task *p_obj = (VL53L9Task *) _this;
  SMMessage report =
  {
    0
  };
  sys_error_code_t res = SYS_NO_ERROR_CODE;

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
        /* In STATE1 the task accepts configuration commands only. */
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_SET_FREQUENCY:
            res = VL53L9TaskSensorSetFrequency(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_RESOLUTION:
            res = VL53L9TaskSensorSetResolution(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_RANGING_MODE:
            res = VL53L9TaskSensorSetRangingMode(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_INTEGRATION_TIME:
            res = VL53L9TaskSensorSetIntegrationTime(p_obj, report);
            break;
          case SENSOR_CMD_ID_CONFIG_IT:
            res = VL53L9TaskSensorConfigIt(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_ADDRESS:
            res = VL53L9TaskSensorSetAddress(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_POWERMODE:
            res = VL53L9TaskSensorSetPowerMode(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = VL53L9TaskSensorEnableCmd(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = VL53L9TaskSensorDisableCmd(p_obj, report);
            break;
          default:
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            break;
        }
        break;
      }
      default:
      {
        /* unwanted report */
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("VL53L8CX: unexpected report in Run: %i\r\n", report.messageID));
        break;
      }

    }
  }

  return res;
}

static sys_error_code_t VL53L9TaskExecuteStepDatalog(AManagedTask *_this)
{
  assert_param(_this != NULL);

  VL53L9Task *p_obj = (VL53L9Task *)_this;
  SMMessage report = {0};
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  AMTExSetInactiveState((AManagedTaskEx *)_this, TRUE);
  if (TX_SUCCESS == tx_queue_receive(&p_obj->in_queue, &report, TX_WAIT_FOREVER))
  {
    AMTExSetInactiveState((AManagedTaskEx *)_this, FALSE);

    switch (report.messageID)
    {
      case SM_MESSAGE_ID_FORCE_STEP:
        /* No-op message used to wake up the task control loop. */
        break;

      case SM_MESSAGE_ID_DATA_READY:
        /* Read and publish one frame if a complete payload is available. */
        res = VL53L9TaskSensorReadData(p_obj);
        if (!SYS_IS_ERROR_CODE(res) && (p_obj->frame_ready == TRUE))
        {
          double_t timestamp = report.sensorDataReadyMessage.fTimestamp;
          DataEvent_t evt;

          EMD_Init(&p_obj->data, p_obj->p_raw_frame, E_EM_UINT8, E_EM_MODE_INTERLEAVED, 2, 1, p_obj->raw_frame_size);
          DataEventInit((IEvent *)&evt, p_obj->p_event_src, &p_obj->data, timestamp, p_obj->id);
          IEventSrcSendEvent(p_obj->p_event_src, (IEvent *)&evt, NULL);
        }
        break;

      case SM_MESSAGE_ID_SENSOR_CMD:
        /* Apply runtime commands while in SENSORS_ACTIVE mode. */
        switch (report.sensorMessage.nCmdID)
        {
          case SENSOR_CMD_ID_INIT:
            res = VL53L9TaskSensorInit(p_obj);
            if (!SYS_IS_ERROR_CODE(res) && (p_obj->sensor_status.is_active == TRUE))
            {
              if (p_obj->pIRQConfig == NULL)
              {
                if (TX_SUCCESS != tx_timer_change(&p_obj->read_timer, AMT_MS_TO_TICKS(p_obj->vl53l9_task_cfg_timer_period_ms),
                                                  AMT_MS_TO_TICKS(p_obj->vl53l9_task_cfg_timer_period_ms)))
                {
                  res = SYS_UNDEFINED_ERROR_CODE;
                }
                else if (TX_SUCCESS != tx_timer_activate(&p_obj->read_timer))
                {
                  res = SYS_UNDEFINED_ERROR_CODE;
                }
              }
              else
              {
                res = VL53L9TaskConfigureIrqPin(p_obj, FALSE);
              }
            }
            break;
          case SENSOR_CMD_ID_SET_FREQUENCY:
            res = VL53L9TaskSensorSetFrequency(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_RESOLUTION:
            res = VL53L9TaskSensorSetResolution(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_RANGING_MODE:
            res = VL53L9TaskSensorSetRangingMode(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_INTEGRATION_TIME:
            res = VL53L9TaskSensorSetIntegrationTime(p_obj, report);
            break;
          case SENSOR_CMD_ID_CONFIG_IT:
            res = VL53L9TaskSensorConfigIt(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_ADDRESS:
            res = VL53L9TaskSensorSetAddress(p_obj, report);
            break;
          case SENSOR_CMD_ID_SET_POWERMODE:
            res = VL53L9TaskSensorSetPowerMode(p_obj, report);
            break;
          case SENSOR_CMD_ID_ENABLE:
            res = VL53L9TaskSensorEnableCmd(p_obj, report);
            break;
          case SENSOR_CMD_ID_DISABLE:
            res = VL53L9TaskSensorDisableCmd(p_obj, report);
            break;
          default:
            res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
            break;
        }
        break;

      default:
        res = SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE;
        break;
    }
  }

  return res;
}

static inline sys_error_code_t VL53L9TaskPostReportToFront(VL53L9Task *_this, SMMessage *pReport)
{
  assert_param(_this != NULL);
  assert_param(pReport);

  if (SYS_IS_CALLED_FROM_ISR())
  {
    return (TX_SUCCESS == tx_queue_front_send(&_this->in_queue, pReport, TX_NO_WAIT)) ? SYS_NO_ERROR_CODE : SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
  }

  return (TX_SUCCESS == tx_queue_front_send(&_this->in_queue, pReport, AMT_MS_TO_TICKS(100))) ? SYS_NO_ERROR_CODE : SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
}

static inline sys_error_code_t VL53L9TaskPostReportToBack(VL53L9Task *_this, SMMessage *pReport)
{
  assert_param(_this != NULL);
  assert_param(pReport);

  if (SYS_IS_CALLED_FROM_ISR())
  {
    return (TX_SUCCESS == tx_queue_send(&_this->in_queue, pReport, TX_NO_WAIT)) ? SYS_NO_ERROR_CODE : SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
  }

  return (TX_SUCCESS == tx_queue_send(&_this->in_queue, pReport, AMT_MS_TO_TICKS(100))) ? SYS_NO_ERROR_CODE : SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE;
}

static sys_error_code_t VL53L9TaskSensorInit(VL53L9Task *_this)
{
  assert_param(_this != NULL);

  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint16_t raw_frame_size = 0U;
  uint32_t frequency = _this->sensor_status.type.ranging.profile_config.frequency;
  uint8_t binning = _this->current_binning;
  uint16_t resolution;

  if (_this->sensor_status.is_active != TRUE)
  {
    return SYS_NO_ERROR_CODE;
  }

  res = VL53L9TaskBindDriverIF(_this);
  if (SYS_IS_ERROR_CODE(res))
  {
    return res;
  }

  res = VL53L9TaskMapDriverError(vl53l9_get_raw_buffer_size(binning, &raw_frame_size));
  if (SYS_IS_ERROR_CODE(res))
  {
    return res;
  }

  resolution = VL53L9TaskBinningToResolution(binning);
  if (resolution == 0U)
  {
    return SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  _this->p_raw_frame = SysAlloc(raw_frame_size);
  if (_this->p_raw_frame == NULL)
  {
    return SYS_OUT_OF_MEMORY_ERROR_CODE;
  }

  _this->raw_frame_size = raw_frame_size;
  memset(_this->p_raw_frame, 0, _this->raw_frame_size);
  _this->sensor_data_capacity = resolution;
  _this->current_resolution = resolution;
  _this->sensor_status.type.ranging.profile_config.ranging_profile = resolution;

  /* Bring up external clock before sensor reset/boot.
   * VL53L9 firmware boot over I3C requires CLK_IN running. */
  if (_this->pTIMConfig != NULL)
  {
    HAL_StatusTypeDef tim_res;
    TIM_HandleTypeDef *p_tim = _this->pTIMConfig->p_tim;

    if (_this->pTIMConfig->p_mx_init_f != NULL)
    {
      _this->pTIMConfig->p_mx_init_f();
    }

    if (p_tim != NULL)
    {
      tim_res = HAL_TIM_PWM_Start(p_tim, TIM_CHANNEL_2);
      if ((tim_res != HAL_OK) && (TIM_CHANNEL_STATE_GET(p_tim, TIM_CHANNEL_2) != HAL_TIM_CHANNEL_STATE_BUSY))
      {
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING,
                   ("VL53L9: TIM3 PWM start failed. hal=%d inst=0x%08lx state=%u\r\n",
                    (int)tim_res,
                    (unsigned long)p_tim->Instance,
                    (unsigned int)TIM_CHANNEL_STATE_GET(p_tim, TIM_CHANNEL_2)));
      }
    }
  }

  res = VL53L9TaskMapDriverError(platform_power_reset(_this));
  if (SYS_IS_ERROR_CODE(res))
  {
    return res;
  }

  res = VL53L9TaskMapDriverError(platform_assign_dynamic_address(_this));
  if (SYS_IS_ERROR_CODE(res))
  {
    return res;
  }

  /* ENTDAA is complete and the HAL device table is populated. Propagate the
   * assigned dynamic address to the bus interface and mark CCC as done so
   * the SensorManager bus task does not issue another RSTDAA that would undo
   * the assignment above. */
  {
    I3CBusIF *p_i3c_bus_if = (I3CBusIF *)_this->p_sensor_bus_if;
    p_i3c_bus_if->dynamic_address = VL53L9_TASK_CFG_STATIC_ADDRESS;
    p_i3c_bus_if->ccc_config_done = 1U;
  }

  /* Give the sensor time to stabilize with CLK_IN running after address assignment.
   * This is critical for proper I3C communication startup. */
  if (_this->tof_driver_if.delay_ms != NULL)
  {
    _this->tof_driver_if.delay_ms(AMT_MS_TO_TICKS_ROUND_UP(50U));
  }

  res = VL53L9TaskMapDriverError(vl53l9_init(&_this->tof_driver_if));
  if (SYS_IS_ERROR_CODE(res))
  {
    return res;
  }

  res = VL53L9TaskMapDriverError(vl53l9_set_power_mode(&_this->tof_driver_if, VL53L9_POWER_ULTRA_LOW));
  if (SYS_IS_ERROR_CODE(res))
  {
    return res;
  }

  res = VL53L9TaskMapDriverError(vl53l9_set_context(&_this->tof_driver_if, VL53L9_CONTEXT_SHORT));
  if (SYS_IS_ERROR_CODE(res))
  {
    return res;
  }

  /* Apply binning and exposure to SHORT context. */
  res = VL53L9TaskMapDriverError(vl53l9_set_binning(&_this->tof_driver_if, VL53L9_CONTEXT_SHORT, binning));
  if (SYS_IS_ERROR_CODE(res))
  {
    return res;
  }

  res = VL53L9TaskMapDriverError(vl53l9_set_exposure(&_this->tof_driver_if, VL53L9_CONTEXT_SHORT,
                                                     (uint16_t)_this->sensor_status.type.ranging.profile_config.timing_budget));
  if (SYS_IS_ERROR_CODE(res))
  {
    return res;
  }

  if (_this->sensor_status.type.ranging.profile_config.mode == VL53L9_SYNC_AUTONOMOUS)
  {
    /* In autonomous mode, period is derived from the selected frequency. */
    uint32_t frame_period_us = 1000000UL / frequency;
    res = VL53L9TaskMapDriverError(vl53l9_set_frame_period(&_this->tof_driver_if, frame_period_us));
    if (SYS_IS_ERROR_CODE(res))
    {
      return res;
    }
  }

  res = VL53L9TaskMapDriverError(vl53l9_set_sync_mode(&_this->tof_driver_if,
                                                      (vl53l9_sync_mode_t)_this->sensor_status.type.ranging.profile_config.mode));
  if (SYS_IS_ERROR_CODE(res))
  {
    return res;
  }

  _this->frame_pending = FALSE;

  res = VL53L9TaskMapDriverError(vl53l9_start(&_this->tof_driver_if));
  if (!SYS_IS_ERROR_CODE(res))
  {
    /* Allow time for the sensor FSM to transition from STANDBY to STREAMING. */
    if (_this->tof_driver_if.delay_ms != NULL)
    {
      _this->tof_driver_if.delay_ms(AMT_MS_TO_TICKS_ROUND_UP(5U));
    }

    _this->vl53l9_task_cfg_timer_period_ms = (ULONG)(1000UL / frequency);
  }

  if (_this->sensor_status.type.ranging.profile_config.mode == VL53L9_SYNC_MANUAL)
  {
    res = VL53L9TaskMapDriverError(vl53l9_trigger_frame(&_this->tof_driver_if));
    if (SYS_IS_ERROR_CODE(res))
    {
      return res;
    }
  }

  return res;
}

static sys_error_code_t VL53L9TaskSensorReadData(VL53L9Task *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res;
  uint8_t is_ready = 0U;
  static uint32_t s_no_ready_count = 0U;
  _this->frame_ready = FALSE;

  if (_this->sensor_status.type.ranging.profile_config.mode == VL53L9_SYNC_AUTONOMOUS)
  {
    /* Data production is sensor-driven (autonomous).
     * The timer only schedules read attempts, so skip when no frame is ready yet. */
    res = VL53L9TaskMapDriverError(vl53l9_poll_frame(&_this->tof_driver_if, &is_ready));
    if (SYS_IS_ERROR_CODE(res))
    {
      return res;
    }
    if (is_ready == 0U)
    {
      s_no_ready_count++;
      return SYS_NO_ERROR_CODE;
    }
  }

  res = VL53L9TaskMapDriverError(vl53l9_get_frame(&_this->tof_driver_if, _this->p_raw_frame, _this->raw_frame_size));
  if (!SYS_IS_ERROR_CODE(res))
  {
    _this->frame_ready = TRUE;
  }

  if (_this->sensor_status.type.ranging.profile_config.mode == VL53L9_SYNC_MANUAL)
  {
    res = VL53L9TaskMapDriverError(vl53l9_trigger_frame(&_this->tof_driver_if));
    if (SYS_IS_ERROR_CODE(res))
    {
      return res;
    }
  }
  return res;
}

static sys_error_code_t VL53L9TaskSensorRegister(VL53L9Task *_this)
{
  assert_param(_this != NULL);

  ISensor_t *tof_if = (ISensor_t *)VL53L9TaskGetTofSensorIF(_this);
  _this->id = SMAddSensor(tof_if);
  return SYS_NO_ERROR_CODE;
}

static sys_error_code_t VL53L9TaskSensorInitTaskParams(VL53L9Task *_this)
{
  assert_param(_this != NULL);

  _this->sensor_status.isensor_class = ISENSOR_CLASS_RANGING;
  _this->sensor_status.is_active = TRUE;
  _this->sensor_status.type.ranging.profile_config.timing_budget = 5U;
  _this->sensor_status.type.ranging.profile_config.frequency = 30U;
  _this->sensor_status.type.ranging.profile_config.enable_ambient = FALSE;
  _this->sensor_status.type.ranging.profile_config.enable_signal = FALSE;
  _this->sensor_status.type.ranging.profile_config.mode = VL53L9_SYNC_MANUAL;
  _this->sensor_status.type.ranging.it_config.criteria = 0U;
  _this->sensor_status.type.ranging.it_config.low_threshold = 0U;
  _this->sensor_status.type.ranging.it_config.high_threshold = 0U;
  _this->sensor_status.type.ranging.address = VL53L9_TASK_CFG_STATIC_ADDRESS;
  _this->sensor_status.type.ranging.power_mode = VL53L9_POWER_REGULAR;
  _this->vl53l9_task_cfg_timer_period_ms = VL53L9_TASK_CFG_TIMER_PERIOD_MS;
  _this->current_binning = VL53L9_TASK_CFG_DEFAULT_BINNING;
  _this->current_resolution = VL53L9TaskBinningToResolution(_this->current_binning);
  vl53l9_get_raw_buffer_size(_this->current_binning, &_this->raw_frame_size);
  _this->sensor_data_capacity = 0U;
  _this->sensor_status.type.ranging.profile_config.ranging_profile = _this->current_resolution;
  memset(&_this->tof_driver_if, 0, sizeof(_this->tof_driver_if));

  EMD_Init(&_this->data, _this->p_raw_frame, E_EM_UINT8, E_EM_MODE_INTERLEAVED, 2, 1, _this->raw_frame_size);

  return SYS_NO_ERROR_CODE;
}

static sys_error_code_t VL53L9TaskSensorSetFrequency(VL53L9Task *_this, SMMessage report)
{
  assert_param(_this != NULL);

  uint32_t frequency;

  if (report.sensorMessage.nSensorId != _this->id)
  {
    return SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  frequency = (uint32_t)report.sensorMessage.nParam;
  if (frequency == 0U)
  {
    return SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  _this->sensor_status.type.ranging.profile_config.frequency = frequency;
  _this->vl53l9_task_cfg_timer_period_ms = 1000UL / _this->sensor_status.type.ranging.profile_config.frequency;
  return SYS_NO_ERROR_CODE;
}

static sys_error_code_t VL53L9TaskSensorSetResolution(VL53L9Task *_this, SMMessage report)
{
  assert_param(_this != NULL);

  uint8_t binning;
  uint32_t param;

  if (report.sensorMessage.nSensorId != _this->id)
  {
    return SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  param = report.sensorMessage.nParam;

  if ((param == 2U) || (param == 4U) || (param == 6U) || (param == 8U) || (param == 12U) || (param == 24U))
  {
    /* Backward-compatible path: parameter is already a binning value. */
    binning = (uint8_t)param;
  }
  else
  {
    /* New path: parameter is a matrix resolution value. */
    binning = VL53L9TaskResolutionToBinning(param);
  }

  if (binning == 0U)
  {
    return SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  _this->current_binning = binning;
  _this->current_resolution = VL53L9TaskBinningToResolution(binning);
  _this->sensor_status.type.ranging.profile_config.ranging_profile = _this->current_resolution;

  return SYS_NO_ERROR_CODE;
}

static sys_error_code_t VL53L9TaskSensorSetRangingMode(VL53L9Task *_this, SMMessage report)
{
  assert_param(_this != NULL);

  if (report.sensorMessage.nSensorId != _this->id)
  {
    return SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  _this->sensor_status.type.ranging.profile_config.mode = (uint32_t)report.sensorMessage.nParam;
  return SYS_NO_ERROR_CODE;
}

static sys_error_code_t VL53L9TaskSensorSetIntegrationTime(VL53L9Task *_this, SMMessage report)
{
  assert_param(_this != NULL);

  if (report.sensorMessage.nSensorId != _this->id)
  {
    return SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  _this->sensor_status.type.ranging.profile_config.timing_budget = (uint32_t)report.sensorMessage.nParam;
  return SYS_NO_ERROR_CODE;
}

static sys_error_code_t VL53L9TaskSensorConfigIt(VL53L9Task *_this, SMMessage report)
{
  assert_param(_this != NULL);

  (void)_this;
  (void)report;
  return SYS_INVALID_PARAMETER_ERROR_CODE;
}

static sys_error_code_t VL53L9TaskSensorSetAddress(VL53L9Task *_this, SMMessage report)
{
  assert_param(_this != NULL);

  I3CBusIF *p_i3c_if;

  if ((report.sensorMessage.nSensorId != _this->id) || (report.sensorMessage.nParam > 0x7FU))
  {
    return SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  _this->sensor_status.type.ranging.address = (uint32_t)report.sensorMessage.nParam;
  p_i3c_if = (I3CBusIF *)_this->p_sensor_bus_if;
  p_i3c_if->dynamic_address  = (uint8_t)report.sensorMessage.nParam;
  p_i3c_if->ccc_config_done  = 0U;

  return SYS_NO_ERROR_CODE;
}

static sys_error_code_t VL53L9TaskSensorSetPowerMode(VL53L9Task *_this, SMMessage report)
{
  assert_param(_this != NULL);

  if (report.sensorMessage.nSensorId != _this->id)
  {
    return SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  _this->sensor_status.type.ranging.power_mode = (uint32_t)report.sensorMessage.nParam;
  return SYS_NO_ERROR_CODE;
}

static sys_error_code_t VL53L9TaskSensorEnableCmd(VL53L9Task *_this, SMMessage report)
{
  assert_param(_this != NULL);

  if (report.sensorMessage.nSensorId != _this->id)
  {
    return SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  _this->sensor_status.is_active = TRUE;
  return SYS_NO_ERROR_CODE;
}

static sys_error_code_t VL53L9TaskSensorDisableCmd(VL53L9Task *_this, SMMessage report)
{
  assert_param(_this != NULL);

  if (report.sensorMessage.nSensorId != _this->id)
  {
    return SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  _this->sensor_status.is_active = FALSE;
  if (_this->pIRQConfig == NULL)
  {
    tx_timer_deactivate(&_this->read_timer);
  }
  else
  {
    (void)VL53L9TaskConfigureIrqPin(_this, TRUE);
  }
  return SYS_NO_ERROR_CODE;
}

static void VL53L9TaskTimerCallbackFunction(ULONG param)
{
  VL53L9Task *p_obj = (VL53L9Task *)param;
  SMMessage report;

  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  if (TX_SUCCESS != tx_queue_send(&p_obj->in_queue, &report, TX_NO_WAIT))
  {
    sys_error_handler();
  }
}

static sys_error_code_t VL53L9TaskConfigureIrqPin(const VL53L9Task *_this, boolean_t low_power)
{
  assert_param(_this != NULL);
  assert_param(_this->pIRQConfig != NULL);

  if (!low_power)
  {
    /* Restore board-specific EXTI setup for active operation. */
    _this->pIRQConfig->p_mx_init_f();
  }
  else
  {
    GPIO_InitTypeDef gpio_init_struct = {0};

    HAL_NVIC_DisableIRQ(_this->pIRQConfig->irq_n);
    HAL_NVIC_ClearPendingIRQ(_this->pIRQConfig->irq_n);

    gpio_init_struct.Pin = _this->pIRQConfig->pin;
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;
    gpio_init_struct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(_this->pIRQConfig->port, &gpio_init_struct);
  }

  return SYS_NO_ERROR_CODE;
}

void VL53L9Task_EXTI_Callback(uint16_t nPin)
{
  MTMapValue_t *p_val;
  TX_QUEUE *p_queue;
  SMMessage report;

  report.sensorDataReadyMessage.messageId = SM_MESSAGE_ID_DATA_READY;
  report.sensorDataReadyMessage.fTimestamp = SysTsGetTimestampF(SysGetTimestampSrv());

  p_val = MTMap_FindByKey(&sTheClass.task_map, (uint32_t)nPin);
  if (p_val != NULL)
  {
    p_queue = &((VL53L9Task *)p_val->p_mtask_obj)->in_queue;
    if (TX_SUCCESS != tx_queue_send(p_queue, &report, TX_NO_WAIT))
    {
      sys_error_handler();
    }
  }
}

static sys_error_code_t VL53L9TaskMapDriverError(int32_t error)
{
  return (error == VL53L9_ERROR_NONE) ? SYS_NO_ERROR_CODE : SYS_UNDEFINED_ERROR_CODE;
}

static sys_error_code_t VL53L9TaskBindDriverIF(VL53L9Task *_this)
{
  assert_param(_this != NULL);

  I3CBusIF *p_i3c_if = (I3CBusIF *)_this->p_sensor_bus_if;

  if ((_this->p_sensor_bus_if == NULL) || (_this->p_sensor_bus_if->m_xConnector.pfReadReg == ABusIFNullRW)
      || (_this->p_sensor_bus_if->m_xConnector.pfWriteReg == ABusIFNullRW))
  {
    return SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }

  _this->tof_driver_if.bus_handle = _this->p_sensor_bus_if;
  _this->tof_driver_if.read_reg = _this->p_sensor_bus_if->m_xConnector.pfReadReg;
  _this->tof_driver_if.write_reg = _this->p_sensor_bus_if->m_xConnector.pfWriteReg;
  _this->tof_driver_if.delay_ms = _this->p_sensor_bus_if->m_xConnector.pfDelay;
  /* Prefer dynamic address when available, otherwise use static address. */
  _this->tof_driver_if.address = (uint8_t)((p_i3c_if->dynamic_address != 0U) ? p_i3c_if->dynamic_address : p_i3c_if->static_address);
  _this->tof_driver_if.bus_property = PLATFORM_BUS_PROPERTY_NONE;
  _this->tof_driver_if.vddio = VDDIO_1V8;
  _this->tof_driver_if.vdda = VDDA_2V8;
  /* ext_clock: frequency (Hz) of the clock provided on CLK_IN pin.
   * Reference X-CUBE uses 12000000 (12 MHz from STM32 MCO/TIM PWM via SW1=INT).
   * Set to 0 if CLK_IN is not driven (sensor uses internal oscillator fallback).
   * A wrong value here prevents the sensor PLL from locking -> pll_lock error -> no streaming.
   * Check status.error.pll_lock in post-start log to diagnose. */
  _this->tof_driver_if.ext_clock = VL53L9_TASK_CFG_EXT_CLOCK_HZ;

  return SYS_NO_ERROR_CODE;
}

static uint8_t VL53L9TaskResolutionToBinning(uint32_t resolution)
{
  switch (resolution)
  {
    case VL53L9_RESOLUTION_4X4:
      return 24U;
    case VL53L9_RESOLUTION_8X8:
      return 12U;
    case VL53L9_RESOLUTION_12X10:
      return 8U;
    case VL53L9_RESOLUTION_18X14:
      return 6U;
    case VL53L9_RESOLUTION_24X24:
      return 4U;
    case VL53L9_RESOLUTION_54X42:
      return 2U;
    default:
      return 0U;
  }
}

static uint16_t VL53L9TaskBinningToResolution(uint8_t binning)
{
  switch (binning)
  {
    case 24U:
      return VL53L9_RESOLUTION_4X4;
    case 12U:
      return VL53L9_RESOLUTION_8X8;
    case 8U:
      return VL53L9_RESOLUTION_12X10;
    case 6U:
      return VL53L9_RESOLUTION_18X14;
    case 4U:
      return VL53L9_RESOLUTION_24X24;
    case 2U:
      return VL53L9_RESOLUTION_54X42;
    default:
      return 0U;
  }
}

/**
  * Helper function to read a 16-bit value in little-endian format from raw frame.
  * @param p_buffer pointer to byte buffer
  * @param offset offset into the buffer
  * @return 16-bit value interpreted as little-endian (LSB at lower address)
  */
static inline uint16_t VL53L9TaskReadLE16(const uint8_t *p_buffer, uint16_t offset)
{
  return (uint16_t)p_buffer[offset] | ((uint16_t)p_buffer[offset + 1U] << 8U);
}


/* device power management */

/**
  * @brief Reset a device
  * @param[in] id Device identifier
  * @return 0 in case of success, negative value otherwise
  */
static int platform_power_reset(const VL53L9Task *_this)
{
  assert_param(_this != NULL);
  assert_param(_this->pXSHUTConfig != NULL);
  HAL_GPIO_WritePin((GPIO_TypeDef *)_this->pXSHUTConfig->port, _this->pXSHUTConfig->pin, GPIO_PIN_RESET);
  _this->tof_driver_if.delay_ms(AMT_MS_TO_TICKS_ROUND_UP(50U));
  HAL_GPIO_WritePin((GPIO_TypeDef *)_this->pXSHUTConfig->port, _this->pXSHUTConfig->pin, GPIO_PIN_SET);
  _this->tof_driver_if.delay_ms(AMT_MS_TO_TICKS_ROUND_UP(50U));
  return 0;
}


static int platform_assign_dynamic_address(VL53L9Task *_this)
{
  HAL_StatusTypeDef status;
  uint64_t payload;
  extern I3C_HandleTypeDef hi3c1;

  /* set i3c bus frequency to 1 MHz before dynamic address assignment */
  hi3c1.Init.CtrlBusCharacteristic.SCLPPLowDuration = 0x7c;
  hi3c1.Init.CtrlBusCharacteristic.SCLI3CHighDuration = 0x7c;
  hi3c1.Init.CtrlBusCharacteristic.SCLODLowDuration = 0x7c;
  if (HAL_I3C_Init(&hi3c1) != HAL_OK)
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("platform_assign_dynamic_address: HAL_I3C_Init@1MHz failed\r\n"));
    return -1;
  }

  /* Assign the same 7-bit dynamic address as the static address (0x29).
   * Note: 0x52 was the 8-bit I2C form (0x29 << 1). The HAL takes 7-bit. */
  do
  {
    status = HAL_I3C_Ctrl_DynAddrAssign(&hi3c1, &payload, I3C_RSTDAA_THEN_ENTDAA, 5000);
    if (status == HAL_BUSY)
    {
      HAL_I3C_Ctrl_SetDynAddr(&hi3c1, VL53L9_TASK_CFG_STATIC_ADDRESS);
    }
  } while (status == HAL_BUSY);

  if (status != HAL_OK)
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("platform_assign_dynamic_address: DynAddrAssign failed with status=%d\r\n", status));
    return -1;
  }

  /* Restored 12MHz I3C speed */
  hi3c1.Init.CtrlBusCharacteristic.SCLPPLowDuration = 0x09;
  hi3c1.Init.CtrlBusCharacteristic.SCLI3CHighDuration = 0x09;
  hi3c1.Init.CtrlBusCharacteristic.SCLODLowDuration = 0x59;
  if (HAL_I3C_Init(&hi3c1) != HAL_OK)
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("platform_assign_dynamic_address: HAL_I3C_Init@12MHz failed\r\n"));
    return -1;
  }

  I3C_DeviceConfTypeDef DeviceConf;
  DeviceConf.DeviceIndex = 1;
  DeviceConf.TargetDynamicAddr = 0x29;  /* 7-bit address: 0x29 (NOT 0x52) */
  DeviceConf.IBIAck = __HAL_I3C_GET_IBI_CAPABLE(__HAL_I3C_GET_BCR(payload));
  DeviceConf.IBIPayload = __HAL_I3C_GET_IBI_PAYLOAD(__HAL_I3C_GET_BCR(payload));
  DeviceConf.CtrlRoleReqAck = __HAL_I3C_GET_CR_CAPABLE(__HAL_I3C_GET_BCR(payload));
  DeviceConf.CtrlStopTransfer = DISABLE;

  if (HAL_I3C_Ctrl_ConfigBusDevices(&hi3c1, &DeviceConf, 1U) != HAL_OK)
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("platform_assign_dynamic_address: ConfigBusDevices FAILED\r\n"));
    Error_Handler();
  }

  return 0;
}
