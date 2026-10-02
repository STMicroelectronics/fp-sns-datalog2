/**
  ******************************************************************************
  * @file    I3CBusTask.c
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
  ******************************************************************************
  */

#include "I3CBusTask.h"
#include "I3CBusTask_vtbl.h"
#include "drivers/I3CMasterDriver.h"
#include "drivers/I3CMasterDriver_vtbl.h"
#include "SMMessageParser.h"
#include "SensorManager.h"
#include "services/sysdebug.h"

#ifndef I3CBUS_TASK_CFG_STACK_DEPTH
#define I3CBUS_TASK_CFG_STACK_DEPTH        256
#endif

#ifndef I3CBUS_TASK_CFG_PRIORITY
#define I3CBUS_TASK_CFG_PRIORITY           (3)
#endif

#ifndef I3CBUS_TASK_CFG_INQUEUE_LENGTH
#define I3CBUS_TASK_CFG_INQUEUE_LENGTH     20
#endif

#define I3CBUS_OP_WAIT_MS                  50

#define SYS_DEBUGF(level, message)         SYS_DEBUGF3(SYS_DBG_I3CBUS, level, message)


typedef struct _I3CBusTaskIBus
{
  IBus super;
  I3CBusTask *m_pxOwner;
} I3CBusTaskIBus;

/**
  * Class object declaration
  */
typedef struct _I3CBusTaskClass
{
  /**
    * I3CBusTask class virtual table.
    */
  AManagedTaskEx_vtbl vtbl;

  /**
    * I3CBusTask (PM_STATE, ExecuteStepFunc) map.
    */
  pExecuteStepFunc_t p_pm_state2func_map[3];
} I3CBusTaskClass_t;

/* Private member function declaration */
/***************************************/

/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t I3CBusTaskExecuteStep(AManagedTask *_this);

static int32_t I3CBusTaskWrite(void *p_sensor, uint16_t reg, uint8_t *p_data, uint16_t size);
static int32_t I3CBusTaskRead(void *p_sensor, uint16_t reg, uint8_t *p_data, uint16_t size);
static uint16_t I3CBusTaskGetTargetAddr(const I3CBusIF *p_sensor);

/* Inline function forward declaration */


/* Objects instance */

/**
  * IBus virtual table.
  */
static const IBus_vtbl s_xIBus_vtbl =
{
  I3CBusTask_vtblCtrl,
  I3CBusTask_vtblConnectDevice,
  I3CBusTask_vtblDisconnectDevice
};


/**
  * The class object.
  */
static const I3CBusTaskClass_t sTheClass =
{
  /* Class virtual table */
  {
    I3CBusTask_vtblHardwareInit,
    I3CBusTask_vtblOnCreateTask,
    I3CBusTask_vtblDoEnterPowerMode,
    I3CBusTask_vtblHandleError,
    I3CBusTask_vtblOnEnterTaskControlLoop,
    I3CBusTask_vtblForceExecuteStep,
    I3CBusTask_vtblOnEnterPowerMode
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    I3CBusTaskExecuteStep,
    NULL,
    I3CBusTaskExecuteStep,
  }
};

/* Public API definition */

AManagedTaskEx *I3CBusTaskAlloc(const void *p_mx_drv_cfg)
{
  I3CBusTask *p_task = SysAlloc(sizeof(I3CBusTask));

  /* Initialize the super class */
  AMTInitEx(&p_task->super);

  p_task->super.vptr = &sTheClass.vtbl;
  p_task->p_mx_drv_cfg = p_mx_drv_cfg;

  return (AManagedTaskEx *) p_task;
}

sys_error_code_t I3CBusTaskConnectDevice(I3CBusTask *_this, I3CBusIF *p_bus_if)
{
  assert_param(_this);

  ((ABusIF *)p_bus_if)->p_request_queue = &_this->in_queue;

  return IBusConnectDevice(_this->p_bus_if, &p_bus_if->super);
}

sys_error_code_t I3CBusTaskDisconnectDevice(I3CBusTask *_this, I3CBusIF *p_bus_if)
{
  assert_param(_this);

  return IBusDisconnectDevice(_this->p_bus_if, &p_bus_if->super);
}

IBus *I3CBusTaskGetBusIF(I3CBusTask *_this)
{
  assert_param(_this);

  return _this->p_bus_if;
}

/* AManagedTask virtual functions definition */

sys_error_code_t I3CBusTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  I3CBusTask *p_obj = (I3CBusTask *) _this;

  p_obj->p_driver = I3CMasterDriverAlloc();
  if (p_obj->p_driver == NULL)
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("I3CBus task: unable to alloc driver object.\r\n"));
    res = SYS_GET_LAST_LOW_LEVEL_ERROR_CODE();
  }
  else
  {
    I3CMasterDriverParams_t driver_cfg =
    {
      .p_mx_i3c_cfg = (void *) p_obj->p_mx_drv_cfg
    };
    res = IDrvInit((IDriver *) p_obj->p_driver, &driver_cfg);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("I3CBus task: error during driver initialization\r\n"));
    }
  }

  return res;
}

sys_error_code_t I3CBusTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                             VOID **pvStackStart,
                                             ULONG *pnStackDepth, UINT *pxPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                             ULONG *pParams)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  I3CBusTask *p_obj = (I3CBusTask *) _this;

  /* initialize the software resources. */

  uint32_t item_size = (uint32_t)SMMessageGetSize(SM_MESSAGE_ID_I3C_BUS_READ);
  VOID *p_queue_items_buff = SysAlloc(I3CBUS_TASK_CFG_INQUEUE_LENGTH * item_size);

  if (p_queue_items_buff != NULL)
  {
    if (TX_SUCCESS == tx_queue_create(&p_obj->in_queue, "I3CBUS_Q", item_size / 4u, p_queue_items_buff,
                                      I3CBUS_TASK_CFG_INQUEUE_LENGTH * item_size))
    {
      p_obj->p_bus_if = SysAlloc(sizeof(I3CBusTaskIBus));
      if (p_obj->p_bus_if != NULL)
      {
        p_obj->p_bus_if->vptr = &s_xIBus_vtbl;
        ((I3CBusTaskIBus *) p_obj->p_bus_if)->m_pxOwner = p_obj;
        p_obj->connected_devices = 0;
        _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

        *pvTaskCode = AMTExRun;
        *pcName = "I3CBUS";
        *pvStackStart = NULL; // allocate the task stack in the system memory pool.
        *pnStackDepth = I3CBUS_TASK_CFG_STACK_DEPTH;
        *pParams = (ULONG) _this;
        *pxPriority = I3CBUS_TASK_CFG_PRIORITY;
        *pnPreemptThreshold = I3CBUS_TASK_CFG_PRIORITY;
        *pnTimeSlice = TX_NO_TIME_SLICE;
        *pnAutoStart = TX_AUTO_START;
      }
      else
      {
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
        res = SYS_OUT_OF_MEMORY_ERROR_CODE;
      }
    }
    else
    {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
      res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    }
  }

  return res;
}

sys_error_code_t I3CBusTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode,
                                                 const EPowerMode eNewPowerMode)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  I3CBusTask *p_obj = (I3CBusTask *) _this;

  IDrvDoEnterPowerMode((IDriver *) p_obj->p_driver, eActivePowerMode, eNewPowerMode);

  /*
   * Do not flush the request queue on PM transitions: dropping pending I/O would
   * leave waiting sensor tasks blocked in I3CBusIFWaitIOComplete().
   */

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("I3CBUS: -> %d\r\n", eNewPowerMode));

  return res;
}

sys_error_code_t I3CBusTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t I3CBusTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  I3CBusTask *p_obj = (I3CBusTask *) _this;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("I3C: start.\r\n"));

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("I3CBUS: start the driver.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (I3CBUS_TASK_CFG_TAG)
  p_obj->super.m_xTaskHandle.pxTaskTag = I3CBUS_TASK_CFG_TAG;
#endif

  res = IDrvStart((IDriver *) p_obj->p_driver);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("I3CBUS - Driver start failed.\r\n"));
    res = SYS_BASE_LOW_LEVEL_ERROR_CODE;
  }

  return res;
}

sys_error_code_t I3CBusTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  I3CBusTask *p_obj = (I3CBusTask *) _this;

  /* to resume the task we send a fake empty message. */
  SMMessage xReport =
  {
    .messageID = SM_MESSAGE_ID_FORCE_STEP
  };
  if ((eActivePowerMode == E_POWER_MODE_STATE1) || (eActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE))
  {
    if (AMTExIsTaskInactive(_this))
    {
      if (TX_SUCCESS != tx_queue_front_send(&p_obj->in_queue, &xReport, AMT_MS_TO_TICKS(100)))
      {

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("I3CBUS: unable to resume the task.\r\n"));

        res = SYS_I3CBUS_TASK_RESUME_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_I3CBUS_TASK_RESUME_ERROR_CODE);
      }
    }
    else
    {
      /* do nothing and wait for the step to complete. */
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

sys_error_code_t I3CBusTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode eActivePowerMode,
                                                 const EPowerMode eNewPowerMode)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  AMTExSetPMClass(_this, E_PM_CLASS_1);

  return res;
}

// IBus virtual functions definition
// *********************************

sys_error_code_t I3CBusTask_vtblCtrl(IBus *_this, EBusCtrlCmd eCtrlCmd, uint32_t nParams)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t I3CBusTask_vtblConnectDevice(IBus *_this, ABusIF *pxBusIF)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (pxBusIF != NULL)
  {
    pxBusIF->m_xConnector.pfReadReg = I3CBusTaskRead;
    pxBusIF->m_xConnector.pfWriteReg = I3CBusTaskWrite;
    pxBusIF->m_xConnector.pfDelay = (ABusDelayF)tx_thread_sleep;
//    pxBusIF->m_pfBusCtrl = I3CBusTaskCtrl;
    pxBusIF->m_pxBus = _this;
    ((I3CBusIF *)pxBusIF)->ccc_config_done = 0U;
    ((I3CBusTaskIBus *) _this)->m_pxOwner->connected_devices++;

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("I3CBUS: connected device: %d\r\n",
                                       ((I3CBusTaskIBus *)_this)->m_pxOwner->connected_devices));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t I3CBusTask_vtblDisconnectDevice(IBus *_this, ABusIF *pxBusIF)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (pxBusIF != NULL)
  {
    pxBusIF->m_xConnector.pfReadReg = ABusIFNullRW;
    pxBusIF->m_xConnector.pfWriteReg = ABusIFNullRW;
    pxBusIF->m_xConnector.pfDelay = NULL;
    pxBusIF->m_pfBusCtrl = NULL;
    pxBusIF->m_pxBus = NULL;
    pxBusIF->p_request_queue = NULL;
    ((I3CBusTaskIBus *) _this)->m_pxOwner->connected_devices--;

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("I3CBUS: connected device: %d\r\n",
                                       ((I3CBusTaskIBus *)_this)->m_pxOwner->connected_devices));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

/* Private function definition */
// ***************************

static sys_error_code_t I3CBusTaskExecuteStep(AManagedTask *_this)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  I3CBusTask *p_obj = (I3CBusTask *) _this;

  struct i3cIOMessage_t xMsg =
  {
    0
  };
  AMTExSetInactiveState((AManagedTaskEx *) _this, TRUE);
  if (TX_SUCCESS == tx_queue_receive(&p_obj->in_queue, &xMsg, TX_WAIT_FOREVER))
  {
    AMTExSetInactiveState((AManagedTaskEx *) _this, FALSE);
    switch (xMsg.messageId)
    {
      case SM_MESSAGE_ID_FORCE_STEP:
        __NOP();
        // do nothing. I need only to resume the task.
        break;

      case SM_MESSAGE_ID_I3C_BUS_READ:
        if (xMsg.pxSensor->ccc_config_done == 0U)
        {
          SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("CCC not done, calling ConfigureTarget\r\n"));
          res = I3CMasterDriverConfigureTarget((I3CMasterDriver_t *) p_obj->p_driver,
                                               xMsg.pxSensor->static_address,
                                               xMsg.pxSensor->dynamic_address);
          if (!SYS_IS_ERROR_CODE(res))
          {
            xMsg.pxSensor->ccc_config_done = 1U;
          }
        }
        else
        {
          SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("CCC already done, skipping\r\n"));
          I3CMasterDriverSetDeviceAddr((I3CMasterDriver_t *) p_obj->p_driver, I3CBusTaskGetTargetAddr(xMsg.pxSensor));
        }

        if (!SYS_IS_ERROR_CODE(res))
        {
          (void)I3CMasterDriverSetRegAddrSize((I3CMasterDriver_t *)p_obj->p_driver, xMsg.pxSensor->reg_addr_size);
          res = IIODrvRead(p_obj->p_driver, xMsg.pnData, xMsg.nDataSize, xMsg.nRegAddr);
        }

        xMsg.pxSensor->last_io_error = res;
        (void) I3CBusIFNotifyIOComplete(xMsg.pxSensor);
        break;

      case SM_MESSAGE_ID_I3C_BUS_WRITE:
        if (xMsg.pxSensor->ccc_config_done == 0U)
        {
          SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("CCC not done, calling ConfigureTarget\r\n"));
          res = I3CMasterDriverConfigureTarget((I3CMasterDriver_t *) p_obj->p_driver,
                                               xMsg.pxSensor->static_address,
                                               xMsg.pxSensor->dynamic_address);
          if (!SYS_IS_ERROR_CODE(res))
          {
            xMsg.pxSensor->ccc_config_done = 1U;
          }
        }
        else
        {
          SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("CCC already done, skipping\r\n"));
          I3CMasterDriverSetDeviceAddr((I3CMasterDriver_t *) p_obj->p_driver, I3CBusTaskGetTargetAddr(xMsg.pxSensor));
        }

        if (!SYS_IS_ERROR_CODE(res))
        {
          (void)I3CMasterDriverSetRegAddrSize((I3CMasterDriver_t *)p_obj->p_driver, xMsg.pxSensor->reg_addr_size);
          res = IIODrvWrite(p_obj->p_driver, xMsg.pnData, xMsg.nDataSize, xMsg.nRegAddr);
        }

        xMsg.pxSensor->last_io_error = res;
        (void) I3CBusIFNotifyIOComplete(xMsg.pxSensor);
        break;

      default:
        break;
    }
  }

  return res;
}

static int32_t I3CBusTaskWrite(void *p_sensor, uint16_t reg, uint8_t *p_data, uint16_t size)
{
  assert_param(p_sensor);
  I3CBusIF *p_i3c_sensor = (I3CBusIF *) p_sensor;
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint16_t auto_inc = p_i3c_sensor->auto_inc;

  struct i3cIOMessage_t msg =
  {
    .messageId = SM_MESSAGE_ID_I3C_BUS_WRITE,
    .pxSensor = p_i3c_sensor,
    .nRegAddr = reg | auto_inc,
    .pnData = p_data,
    .nDataSize = size
  };

  if (SYS_IS_CALLED_FROM_ISR())
  {
    /* we cannot read and write in the I3C BUS from an ISR. Notify the error */
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_I3CBUS_TASK_IO_ERROR_CODE);
    res = SYS_I3CBUS_TASK_IO_ERROR_CODE;
  }
  else
  {
    if (TX_SUCCESS != tx_queue_send(p_i3c_sensor->super.p_request_queue, &msg, AMT_MS_TO_TICKS(I3CBUS_OP_WAIT_MS)))
    {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_I3CBUS_TASK_IO_ERROR_CODE);
      res = SYS_I3CBUS_TASK_IO_ERROR_CODE;
    }
  }

  if (!SYS_IS_ERROR_CODE(res))
  {
    /* Wait until the operation is completed */
    res = I3CBusIFWaitIOComplete(p_i3c_sensor);
  }

  return res;
}

static int32_t I3CBusTaskRead(void *p_sensor, uint16_t reg, uint8_t *p_data, uint16_t size)
{
  assert_param(p_sensor);
  I3CBusIF *p_i3c_sensor = (I3CBusIF *) p_sensor;
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint16_t auto_inc = p_i3c_sensor->auto_inc;

  struct i3cIOMessage_t msg =
  {
    .messageId = SM_MESSAGE_ID_I3C_BUS_READ,
    .pxSensor = p_i3c_sensor,
    .nRegAddr = reg | auto_inc,
    .pnData = p_data,
    .nDataSize = size
  };

  if (SYS_IS_CALLED_FROM_ISR())
  {
    /* we cannot read and write in the I3C BUS from an ISR. Notify the error */
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_I3CBUS_TASK_IO_ERROR_CODE);
    res = SYS_I3CBUS_TASK_IO_ERROR_CODE;
  }
  else
  {
    if (TX_SUCCESS != tx_queue_send(p_i3c_sensor->super.p_request_queue, &msg, AMT_MS_TO_TICKS(I3CBUS_OP_WAIT_MS)))
    {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_I3CBUS_TASK_IO_ERROR_CODE);
      res = SYS_I3CBUS_TASK_IO_ERROR_CODE;
    }
  }

  if (!SYS_IS_ERROR_CODE(res))
  {
    /* Wait until the operation is completed */
    res = I3CBusIFWaitIOComplete(p_i3c_sensor);
  }

  return res;
}

static uint16_t I3CBusTaskGetTargetAddr(const I3CBusIF *p_sensor)
{
  assert_param(p_sensor != NULL);

  /* Prefer dynamic address unless a pure static target is configured. */
  uint16_t address = (p_sensor->dynamic_address != 0U) ? p_sensor->dynamic_address : p_sensor->static_address;

  /* Normalize legacy 8-bit I2C-style addresses to 7-bit format. */
  if (address > 0x7FU)
  {
    address >>= 1U;
  }

  return address;
}
