/**
  ******************************************************************************
  * @file    FDM_ACOTask.c
  * @author  SRA
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

#include "FDM_ACOTask.h"
#include "FDM_ACOTask_vtbl.h"
#include "services/sysdebug.h"

#include "app_messages_parser.h"
#include "STWIN.box_debug_pins.h"

/**#include "IMP23ABSUTask.h" */

/**
  * Define
  */

#ifndef ACO_TASK_CFG_STACK_DEPTH
#define ACO_TASK_CFG_STACK_DEPTH        configMINIMAL_STACK_SIZE*12
#endif

#ifndef ACO_TASK_CFG_PRIORITY
#define ACO_TASK_CFG_PRIORITY           (tskIDLE_PRIORITY)
#endif

#ifndef ACO_TASK_CFG_IN_QUEUE_ITEM_COUNT
#define ACO_TASK_CFG_IN_QUEUE_ITEM_COUNT 10
#endif

#ifndef ACO_TASK_CFG_IN_QUEUE_ITEM_SIZE
#define ACO_TASK_CFG_IN_QUEUE_ITEM_SIZE sizeof(APPReport)
#endif

#define FFT_DPU_INPUT_BUFFER_DIM  (512u)
#define FFT_DPU_INPUT_SIGNAL_AXIS (1u)
#define FFT_DPU_INPUT_SIGNAL_DIM  (192u)
#define FFT_DPU_OVERLAP           (0.25f)

/** Number of circular buffer item */
#define FFT_DPU_NUM_OF_CB_ITEM    (6u)

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_ACO, level, message)

/**
  * Class object declaration
  */
typedef struct _FDM_ACOTaskClass
{
  /**
    * IMP23ABSUTask class virtual table.
    */
  AManagedTaskEx_vtbl vtbl;

  /**
    * IMP23ABSUTask (PM_STATE, ExecuteStepFunc) map.
    */
  pExecuteStepFunc_t p_pm_state2func_map[];
} FDM_ACOTaskClass_t;

/**
  *  FDM_ACOTask internal structure.
  */
struct _FDM_ACOTask
{
  /**
    * Base class object.
    */
  AManagedTaskEx super;

  /**
    * Digital processing Unit specialized for the Generic FFT library.
    */
  FFT_DPU2_t *p_dpu;

  /**
    * Data buffer used by the DPU but allocated by the task. The size of the buffer depend
    * on many parameters like:
    * - the type of the data used as input by the DPU
    * - the length of the signal
    * - the number of signals to manage in a circular way in order to decouple the data producer from the data process.
    *
    * The DPU uses this buffer to store one or more input signals to be processed, depending on the size of the buffer.
    * The correct size in byte for one signal is computed by the DPU with the method ADPU2_GetInDataPayloadSize().
    */
  void *p_dpu_in_buff;

  /**
    * Data buffer used by the DPU but allocated by the task. It is used by the DPU to store the result of the processing.
    * The correct size in byte for one signal is computed by the DPU with the method ADPU2_GetOutDataPayloadSize().
    */
  void *p_dpu_out_buff;

  /**
    * Data builder object to connect the DPU to the sensor.
    */
  DefDataBuilder_t data_builder;

  ISourceObservable *mic_sensor_source;

  void *FDM_ACO_DPUItemBuff;

  /**
    * Synchronization object used to send command to the task.
    */
  TX_QUEUE in_queue;
};

/**
  * The only instance of the task object.
  */
static FDM_ACOTask sTaskObj;

/* Private member function declaration */
/***************************************/

/**
  * Execute one step of the task control loop while the system is in SENSORS_ACTIVE.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t FDM_ACOTaskExecuteStepDatalog(AManagedTask *_this);

static inline sys_error_code_t FDM_ACOTaskPostReportToFront(FDM_ACOTask *_this, APPReport *pxReport);

static inline sys_error_code_t FDM_ACOTaskPostReportToBack(FDM_ACOTask *_this, APPReport *pxReport);

/**
  * Callback used get the notification from the DPU.
  *
  * @param _this [IN] specifies the DPU that triggered the callback.
  * @param p_param [IN] specifies an application specific parameter.
  */
static void AcoTaskDpuCallback(IDPU2_t *_this, void *p_param);

/**
  * The class object.
  */
static const FDM_ACOTaskClass_t sTheClass =
{
  /**
    * Class virtual table
    */
  {
    FDM_ACOTask_vtblHardwareInit,
    FDM_ACOTask_vtblOnCreateTask,
    FDM_ACOTask_vtblDoEnterPowerMode,
    FDM_ACOTask_vtblHandleError,
    FDM_ACOTask_vtblOnEnterTaskControlLoop,
    FDM_ACOTask_vtblForceExecuteStep,
    FDM_ACOTask_vtblOnEnterPowerMode
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    NULL,
    NULL,
    FDM_ACOTaskExecuteStepDatalog,
    NULL,
  }
};

/* Public API definition */
/*************************/
AManagedTaskEx *FDM_ACOTaskAlloc()
{

  /* Initialize the super class */
  AMTInitEx(&sTaskObj.super);

  sTaskObj.super.vptr = &sTheClass.vtbl;

  return (AManagedTaskEx *) &sTaskObj;
}

void FDM_ACOTaskSetSourceIF(FDM_ACOTask *_this, ISourceObservable *source)
{
  assert_param(_this);

  _this->mic_sensor_source = source;
}

void FDM_ACOTaskAddDPUListener(FDM_ACOTask *_this, IDataEventListener_t *p_listener)
{
  assert_param(_this != NULL);
  assert_param(p_listener != NULL);

  IEventSrc *p_evt_src;

  p_evt_src = ADPU2_GetEventSrcIF((ADPU2_t *) _this->p_dpu);
  IEventSrcAddEventListener(p_evt_src, (IEventListener *) p_listener);
}

/* AManagedTask virtual functions definition */
/*********************************************/
sys_error_code_t FDM_ACOTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  /* Configure DEBUG PIN */
  BSP_DEBUG_PIN_Off(CON34_PIN_5);
  BSP_DEBUG_PIN_Off(CON34_PIN_12);
  BSP_DEBUG_PIN_Off(CON34_PIN_14);
  BSP_DEBUG_PIN_Off(CON34_PIN_16);
  BSP_DEBUG_PIN_Off(CON34_PIN_22);
  BSP_DEBUG_PIN_Off(CON34_PIN_24);
  BSP_DEBUG_PIN_Off(CON34_PIN_26);
  BSP_DEBUG_PIN_Off(CON34_PIN_28);
  BSP_DEBUG_PIN_Off(CON34_PIN_30);
  BSP_DEBUG_PIN_Off(CON34_PIN_30);

  BSP_DEBUG_PIN_Init(CON34_PIN_5);
  BSP_DEBUG_PIN_Init(CON34_PIN_12);
  BSP_DEBUG_PIN_Init(CON34_PIN_14);
  BSP_DEBUG_PIN_Init(CON34_PIN_16);
  BSP_DEBUG_PIN_Init(CON34_PIN_22);
  BSP_DEBUG_PIN_Init(CON34_PIN_24);
  BSP_DEBUG_PIN_Init(CON34_PIN_26);
  BSP_DEBUG_PIN_Init(CON34_PIN_28);
  BSP_DEBUG_PIN_Init(CON34_PIN_30);
  BSP_DEBUG_PIN_Init(CON34_PIN_30);
  //  FDM_ACOTask *p_obj = (FDM_ACOTask*)_this;

  return xRes;
}

sys_error_code_t FDM_ACOTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pTaskCode, CHAR **pName,
                                              VOID **pvStackStart, ULONG *pStackDepth,
                                              UINT *pPriority, UINT *pPreemptThreshold, ULONG *pTimeSlice, ULONG *pAutoStart, ULONG *pParams)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  FDM_ACOTask *p_obj = (FDM_ACOTask *) _this;

  uint16_t item_size = ACO_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *pvQueueItemsBuff = SysAlloc(ACO_TASK_CFG_IN_QUEUE_ITEM_COUNT * item_size);
  if (pvQueueItemsBuff == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    sys_error_handler();
    return res;
  }

  if (TX_SUCCESS != tx_queue_create(&p_obj->in_queue, "AcoTask_Q", item_size / 4, pvQueueItemsBuff,
                                    ACO_TASK_CFG_IN_QUEUE_ITEM_COUNT * item_size))
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    sys_error_handler();
    return res;
  }

  /**
    * Allocate and initialize the DPU
    */
  p_obj->p_dpu = (FFT_DPU2_t *) FFT_DPU2Alloc();
  if (p_obj->p_dpu == NULL)
  {
    sys_error_handler();
  }

  /**
    * Initialize FFT_DPU2.0
    */
  FFT_DPU2_Input_param_t FFT_DPU2_Input_param;
  FFT_DPU2_Input_param.fft_input_buf_dim = FFT_DPU_INPUT_BUFFER_DIM;
  FFT_DPU2_Input_param.input_signal_len = FFT_DPU_INPUT_SIGNAL_DIM;
  FFT_DPU2_Input_param.input_signal_n_axis = FFT_DPU_INPUT_SIGNAL_AXIS;
  FFT_DPU2_Input_param.n_average = 0u;
  FFT_DPU2_Input_param.overlap = FFT_DPU_OVERLAP;

  FFT_DPU2Init(p_obj->p_dpu, &FFT_DPU2_Input_param);

  /* Register DPU DATA READY notification callback */
  IDPU2_RegisterNotifyCallback((IDPU2_t *) p_obj->p_dpu, AcoTaskDpuCallback, p_obj);

  /**
    * Allocate the buffer for the DPU: input data buffer
    */
  size_t data_buff_size = ADPU2_GetInDataPayloadSize((ADPU2_t *) p_obj->p_dpu);
  data_buff_size *= FFT_DPU_NUM_OF_CB_ITEM;
  p_obj->p_dpu_in_buff = SysAlloc(data_buff_size);
  if (p_obj->p_dpu_in_buff == NULL)
  {
    sys_error_handler();
  }

  res = ADPU2_SetInDataBuffer((ADPU2_t *) p_obj->p_dpu, (uint8_t *) p_obj->p_dpu_in_buff, data_buff_size);

  if (SYS_IS_ERROR_CODE(res))
  {
    return res;
  }

  /*allocate the buffer for the DPU: output data buffer*/
  data_buff_size = ADPU2_GetOutDataPayloadSize((ADPU2_t *) p_obj->p_dpu);
  p_obj->p_dpu_out_buff = SysAlloc(data_buff_size);
  if (p_obj->p_dpu_in_buff == NULL)
  {
    sys_error_handler();
  }

  res = ADPU2_SetOutDataBuffer((ADPU2_t *) p_obj->p_dpu, (uint8_t *) p_obj->p_dpu_out_buff, data_buff_size);

  /* allocate the data builder*/
  DefDB_AllocStatic(&p_obj->data_builder);

  *pTaskCode = AMTExRun;
  *pName = "FDM_ACOTask";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool.
  *pStackDepth = ACO_TASK_CFG_STACK_DEPTH;
  *pParams = (ULONG) _this;
  *pPriority = ACO_TASK_CFG_PRIORITY;
  *pPreemptThreshold = ACO_TASK_CFG_PRIORITY;
  *pTimeSlice = TX_NO_TIME_SLICE;
  *pAutoStart = TX_AUTO_START;
  p_obj->super.m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

  return res;
}

sys_error_code_t FDM_ACOTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode,
                                                  const EPowerMode eNewPowerMode)
{
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  FDM_ACOTask *p_obj = (FDM_ACOTask *) _this;

  if (eNewPowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    APPReport xReport =
    {
      .acousticDomainReport.msgId = APP_REPORT_ID_ACOUSTIC_CMD,
      .acousticDomainReport.nCmdID = ACO_CMD_ID_START_ACQ
    };

    if (tx_queue_send(&p_obj->in_queue, &xReport, AMT_MS_TO_TICKS(100)) != TX_SUCCESS)
    {
      xRes = SYS_APP_TASK_REPORT_LOST_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_APP_TASK_REPORT_LOST_ERROR_CODE);
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("FDM_ACOTask: -> E_POWER_MODE_SENSORS_ACTIVE\r\n"));
    SYS_DEBUGF3(SYS_DBG_APP, SYS_DBG_LEVEL_VERBOSE, ("FDM_ACOTask: -> DATALOG\r\n"));
  }
  else if (eNewPowerMode == E_POWER_MODE_STATE1)
  {
    if (eActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
    {
      ADPU2_Reset((ADPU2_t *) p_obj->p_dpu);
      tx_queue_flush(&p_obj->in_queue);
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("FDM_ACOTask: -> E_POWER_MODE_STATE1\r\n"));
    SYS_DEBUGF3(SYS_DBG_APP, SYS_DBG_LEVEL_VERBOSE, ("FDM_ACOTask: -> RUN\r\n"));
  }
  else if (eNewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("FDM_ACOTask: -> SLEEP_1\r\n"));
    SYS_DEBUGF3(SYS_DBG_APP, SYS_DBG_LEVEL_VERBOSE, ("FDM_ACOTask: -> SLEEP_1\r\n"));
  }

  return xRes;

}

sys_error_code_t FDM_ACOTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  //  FDM_ACOTask *p_obj = (FDM_ACOTask*)_this;

  return xRes;
}

sys_error_code_t FDM_ACOTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);

  FDM_ACOTask *p_obj = (FDM_ACOTask *) _this;
  sys_error_code_t res = SYS_NO_ERROR_CODE;

#ifdef ENABLE_THREADX_DBG_PIN
  p_obj->super.m_xTaskHandle.pxTaskTag = ACO_TASK_CFG_TAG;
#endif

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("FDM_ACOTask: start.\r\n"));

  if (p_obj->mic_sensor_source != NULL)
  {
    /* Link FDM_ACO to IDPUIF */
    if (IDPU2_AttachToDataSource((IDPU2_t *) p_obj->p_dpu, p_obj->mic_sensor_source,
                                 (IDataBuilder_t *) &p_obj->data_builder, E_IDB_NO_DATA_LOSS))
    {
      sys_error_handler();
    }
  }

  return res;
}

sys_error_code_t FDM_ACOTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode)
{
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  FDM_ACOTask *p_obj = (FDM_ACOTask *) _this;

  APPReport xReport =
  {
    .internalReportFE.msgId = APP_REPORT_ID_FORCE_STEP,
    .internalReportFE.nData = 0
  };

  if ((eActivePowerMode == E_POWER_MODE_STATE1) || (eActivePowerMode == E_POWER_MODE_STARTING))
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
    else
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("FDM_ACOTask: not suspended.\r\n"));
    }
  }
  else if (eActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE)
  {
    xRes = FDM_ACOTaskPostReportToFront(p_obj, (APPReport *) &xReport);
  }
  else
  {
    ;
  }

  return xRes;
}

sys_error_code_t FDM_ACOTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode eActivePowerMode,
                                                  const EPowerMode eNewPowerMode)
{
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  //  FDM_ACOTask *p_obj = (FDM_ACOTask*)_this;

  return xRes;
}

// Private function definition
/*************************/
uint32_t fftacocount = 0;

static void AcoTaskDpuCallback(IDPU2_t *_this, void *p_param)
{
  FDM_ACOTask *p_obj = (FDM_ACOTask *) p_param;

  APPReport xReport =
  {
    .acousticDomainReport.msgId = APP_REPORT_ID_ACOUSTIC_CMD,
    .acousticDomainReport.nCmdID = ACO_CMD_ID_SET_DRY
  };

  FDM_ACOTaskPostReportToBack(p_obj, &xReport);
  fftacocount++;
}

static sys_error_code_t FDM_ACOTaskExecuteStepDatalog(AManagedTask *_this)
{

  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  FDM_ACOTask *p_obj = (FDM_ACOTask *) _this;

  APPReport xReport =
  {
    0
  };

  AMTExSetInactiveState((AManagedTaskEx *) _this, TRUE);

  if (TX_SUCCESS == tx_queue_receive(&p_obj->in_queue, &xReport, TX_WAIT_FOREVER))
  {
    AMTExSetInactiveState((AManagedTaskEx *) _this, FALSE);

    switch (xReport.msgId)
    {
      case APP_REPORT_ID_FORCE_STEP:
        /* do nothing. I need only to resume. */
        __NOP();
        break;
      case APP_REPORT_ID_ACOUSTIC_CMD:
        if (xReport.acousticDomainReport.nCmdID == ACO_CMD_ID_START_ACQ)
        {
          __NOP();
        }
        else if (xReport.acousticDomainReport.nCmdID == ACO_CMD_ID_SET_DRY)
        {
          (void) ADPU2_ProcessAndDispatch((ADPU2_t *) p_obj->p_dpu);
        }

        break;

      default:
        xRes = SYS_APP_TASK_UNKNOWN_REPORT_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_APP_TASK_UNKNOWN_REPORT_ERROR_CODE)
        ;

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("FDM_ACOTask: unexpected report in Datalog: %i\r\n", xReport.msgId));
        break;
    }
  }

  return xRes;

}

static inline sys_error_code_t FDM_ACOTaskPostReportToFront(FDM_ACOTask *_this, APPReport *pReport)
{
  assert_param(_this != NULL);
  assert_param(pReport);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (SYS_IS_CALLED_FROM_ISR())
  {
    if (TX_SUCCESS != tx_queue_front_send(&_this->in_queue, pReport, TX_NO_WAIT))
    {
      res = SYS_APP_TASK_REPORT_LOST_ERROR_CODE;
      // this function is private and the caller will ignore this return code.
    }
  }
  else
  {
    if (TX_SUCCESS != tx_queue_front_send(&_this->in_queue, pReport, AMT_MS_TO_TICKS(100)))
    {
      res = SYS_APP_TASK_REPORT_LOST_ERROR_CODE;
      /* this function is private and the caller will ignore this return code.*/
    }
  }

  return res;
}

static inline sys_error_code_t FDM_ACOTaskPostReportToBack(FDM_ACOTask *_this, APPReport *pReport)
{
  assert_param(_this != NULL);
  assert_param(pReport);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (SYS_IS_CALLED_FROM_ISR())
  {
    if (TX_SUCCESS != tx_queue_send(&_this->in_queue, pReport, TX_NO_WAIT))
    {
      res = SYS_APP_TASK_REPORT_LOST_ERROR_CODE;
      /* this function is private and the caller will ignore this return code. */
    }
  }
  else
  {
    if (TX_SUCCESS != tx_queue_send(&_this->in_queue, pReport, AMT_MS_TO_TICKS(100)))
    {
      res = SYS_APP_TASK_REPORT_LOST_ERROR_CODE;
      /* this function is private and the caller will ignore this return code. */
    }

    res = SYS_NO_ERROR_CODE;
  }

  return res;
}

