/**
  ******************************************************************************
  * @file    SPIBusTask.c
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

#include "SPIBusTask.h"
#include "SPIBusTask_vtbl.h"
#include "drivers/SPIMasterDriver.h"
#include "drivers/SPIMasterDriver_vtbl.h"
#include "SMMessageParser.h"
#include "SensorManager.h"
#include "services/sysdebug.h"

#ifndef SPIBUS_TASK_CFG_STACK_DEPTH
#define SPIBUS_TASK_CFG_STACK_DEPTH        120
#endif

#ifndef SPIBUS_TASK_CFG_PRIORITY
#define SPIBUS_TASK_CFG_PRIORITY           (3)
#endif

#ifndef SPIBUS_TASK_CFG_INQUEUE_LENGTH
#define SPIBUS_TASK_CFG_INQUEUE_LENGTH     20
#endif

#define SPIBUS_OP_WAIT_MS                  50

#define SYS_DEBUGF(level, message)         SYS_DEBUGF3(SYS_DBG_SPIBUS, level, message)


typedef struct _SPIBusTaskIBus
{
  IBus super;
  SPIBusTask *m_pxOwner;
} SPIBusTaskIBus;

/**
  * Class object declaration
  */
typedef struct _SPIBusTaskClass
{
  /**
    * SPIBusTask class virtual table.
    */
  AManagedTaskEx_vtbl vtbl;

  /**
    * SPIBusTask (PM_STATE, ExecuteStepFunc) map.
    */
  pExecuteStepFunc_t p_pm_state2func_map[3];
} SPIBusTaskClass_t;

/* Private member function declaration */
/***************************************/

/**
  * Execute one step of the task control loop while the system is in RUN mode.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
  */
static sys_error_code_t SPIBusTaskExecuteStep(AManagedTask *_this);

static int32_t SPIBusTaskWrite(void *p_sensor, uint16_t reg, uint8_t *p_data, uint16_t size);
static int32_t SPIBusTaskRead(void *p_sensor, uint16_t reg, uint8_t *p_data, uint16_t size);

/* Inline function forward declaration */


/* Objects instance */

/**
  * IBus virtual table.
  */
static const IBus_vtbl s_xIBus_vtbl =
{
  SPIBusTask_vtblCtrl,
  SPIBusTask_vtblConnectDevice,
  SPIBusTask_vtblDisconnectDevice
};


/**
  * The class object.
  */
static const SPIBusTaskClass_t sTheClass =
{
  /* Class virtual table */
  {
    SPIBusTask_vtblHardwareInit,
    SPIBusTask_vtblOnCreateTask,
    SPIBusTask_vtblDoEnterPowerMode,
    SPIBusTask_vtblHandleError,
    SPIBusTask_vtblOnEnterTaskControlLoop,
    SPIBusTask_vtblForceExecuteStep,
    SPIBusTask_vtblOnEnterPowerMode
  },

  /* class (PM_STATE, ExecuteStepFunc) map */
  {
    SPIBusTaskExecuteStep,
    NULL,
    SPIBusTaskExecuteStep,
  }
};

/* Public API definition */

AManagedTaskEx *SPIBusTaskAlloc(const void *p_mx_drv_cfg)
{
  SPIBusTask *p_task = SysAlloc(sizeof(SPIBusTask));

  /* Initialize the super class */
  AMTInitEx(&p_task->super);

  p_task->super.vptr = &sTheClass.vtbl;
  p_task->p_mx_drv_cfg = p_mx_drv_cfg;

  return (AManagedTaskEx *) p_task;
}

sys_error_code_t SPIBusTaskConnectDevice(SPIBusTask *_this, SPIBusIF *p_bus_if)
{
  assert_param(_this);

  ((ABusIF *)p_bus_if)->p_request_queue = &_this->in_queue;

  return IBusConnectDevice(_this->p_bus_if, &p_bus_if->super);
}

sys_error_code_t SPIBusTaskDisconnectDevice(SPIBusTask *_this, SPIBusIF *p_bus_if)
{
  assert_param(_this);

  return IBusDisconnectDevice(_this->p_bus_if, &p_bus_if->super);
}

IBus *SPIBusTaskGetBusIF(SPIBusTask *_this)
{
  assert_param(_this);

  return _this->p_bus_if;
}

/* AManagedTask virtual functions definition */

sys_error_code_t SPIBusTask_vtblHardwareInit(AManagedTask *_this, void *pParams)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SPIBusTask *p_obj = (SPIBusTask *) _this;

  p_obj->p_driver = SPIMasterDriverAlloc();
  if (p_obj->p_driver == NULL)
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("SPIBus task: unable to alloc driver object.\r\n"));
    res = SYS_GET_LAST_LOW_LEVEL_ERROR_CODE();
  }
  else
  {
    SPIMasterDriverParams_t driver_cfg =
    {
      .p_mx_spi_cfg = (void *) p_obj->p_mx_drv_cfg
    };
    res = IDrvInit((IDriver *) p_obj->p_driver, &driver_cfg);
    if (SYS_IS_ERROR_CODE(res))
    {
      SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("SPIBus task: error during driver initialization\r\n"));
    }
  }

  return res;
}

sys_error_code_t SPIBusTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                             VOID **pvStackStart,
                                             ULONG *pnStackDepth, UINT *pxPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                             ULONG *pParams)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SPIBusTask *p_obj = (SPIBusTask *) _this;

  /* initialize the software resources. */

  uint32_t item_size = (uint32_t)SMMessageGetSize(SM_MESSAGE_ID_SPI_BUS_READ);
  VOID *p_queue_items_buff = SysAlloc(SPIBUS_TASK_CFG_INQUEUE_LENGTH * item_size);

  if (p_queue_items_buff != NULL)
  {
    if (TX_SUCCESS == tx_queue_create(&p_obj->in_queue, "SPIBUS_Q", item_size / 4u, p_queue_items_buff,
                                      SPIBUS_TASK_CFG_INQUEUE_LENGTH * item_size))
    {
      p_obj->p_bus_if = SysAlloc(sizeof(SPIBusTaskIBus));
      if (p_obj->p_bus_if != NULL)
      {
        p_obj->p_bus_if->vptr = &s_xIBus_vtbl;
        ((SPIBusTaskIBus *) p_obj->p_bus_if)->m_pxOwner = p_obj;
        p_obj->connected_devices = 0;
        _this->m_pfPMState2FuncMap = sTheClass.p_pm_state2func_map;

        *pvTaskCode = AMTExRun;
        *pcName = "SPIBUS";
        *pvStackStart = NULL; // allocate the task stack in the system memory pool.
        *pnStackDepth = SPIBUS_TASK_CFG_STACK_DEPTH;
        *pParams = (ULONG) _this;
        *pxPriority = SPIBUS_TASK_CFG_PRIORITY;
        *pnPreemptThreshold = SPIBUS_TASK_CFG_PRIORITY;
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

sys_error_code_t SPIBusTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode,
                                                 const EPowerMode eNewPowerMode)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SPIBusTask *p_obj = (SPIBusTask *) _this;

  IDrvDoEnterPowerMode((IDriver *) p_obj->p_driver, eActivePowerMode, eNewPowerMode);

  if (eNewPowerMode == E_POWER_MODE_SLEEP_1)
  {
    tx_queue_flush(&p_obj->in_queue);
  }

  if ((eActivePowerMode == E_POWER_MODE_SENSORS_ACTIVE) && (eNewPowerMode == E_POWER_MODE_STATE1))
  {
    tx_queue_flush(&p_obj->in_queue);
  }

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SPIBUS: -> %d\r\n", eNewPowerMode));

  return res;
}

sys_error_code_t SPIBusTask_vtblHandleError(AManagedTask *_this, SysEvent xError)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t SPIBusTask_vtblOnEnterTaskControlLoop(AManagedTask *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SPIBusTask *p_obj = (SPIBusTask *) _this;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SPI: start.\r\n"));

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("SPIBUS: start the driver.\r\n"));

#if defined(ENABLE_THREADX_DBG_PIN) && defined (SPIBUS_TASK_CFG_TAG)
  p_obj->super.m_xTaskHandle.pxTaskTag = SPIBUS_TASK_CFG_TAG;
#endif

  res = IDrvStart((IDriver *) p_obj->p_driver);
  if (SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SPIBUS - Driver start failed.\r\n"));
    res = SYS_BASE_LOW_LEVEL_ERROR_CODE;
  }

  return res;
}

sys_error_code_t SPIBusTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SPIBusTask *p_obj = (SPIBusTask *) _this;

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

        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SPIBUS: unable to resume the task.\r\n"));

        res = SYS_SPIBUS_TASK_RESUME_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SPIBUS_TASK_RESUME_ERROR_CODE);
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

sys_error_code_t SPIBusTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode eActivePowerMode,
                                                 const EPowerMode eNewPowerMode)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  AMTExSetPMClass(_this, E_PM_CLASS_1);

  return res;
}

// IBus virtual functions definition
// *********************************

sys_error_code_t SPIBusTask_vtblCtrl(IBus *_this, EBusCtrlCmd eCtrlCmd, uint32_t nParams)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t SPIBusTask_vtblConnectDevice(IBus *_this, ABusIF *pxBusIF)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (pxBusIF != NULL)
  {
    pxBusIF->m_xConnector.pfReadReg = SPIBusTaskRead;
    pxBusIF->m_xConnector.pfWriteReg = SPIBusTaskWrite;
    pxBusIF->m_xConnector.pfDelay = (ABusDelayF)tx_thread_sleep;
//    pxBusIF->m_pfBusCtrl = SPIBusTaskCtrl;
    pxBusIF->m_pxBus = _this;
    ((SPIBusTaskIBus *) _this)->m_pxOwner->connected_devices++;

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SPIBUS: connected device: %d\r\n",
                                       ((SPIBusTaskIBus *)_this)->m_pxOwner->connected_devices));
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SPIBusTask_vtblDisconnectDevice(IBus *_this, ABusIF *pxBusIF)
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
    ((SPIBusTaskIBus *) _this)->m_pxOwner->connected_devices--;

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SPIBUS: connected device: %d\r\n",
                                       ((SPIBusTaskIBus *)_this)->m_pxOwner->connected_devices));
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

static sys_error_code_t SPIBusTaskExecuteStep(AManagedTask *_this)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SPIBusTask *p_obj = (SPIBusTask *) _this;

  struct spiIOMessage_t xMsg =
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

      case SM_MESSAGE_ID_SPI_BUS_READ:
        SPIMasterDriverSelectDevice((SPIMasterDriver_t *) p_obj->p_driver, xMsg.pxSensor->p_cs_gpio_port,
                                    xMsg.pxSensor->cs_gpio_pin);
        res = IIODrvRead(p_obj->p_driver, xMsg.pnData, xMsg.nDataSize, xMsg.nRegAddr);
        SPIMasterDriverDeselectDevice((SPIMasterDriver_t *) p_obj->p_driver, xMsg.pxSensor->p_cs_gpio_port,
                                      xMsg.pxSensor->cs_gpio_pin);
        if (!SYS_IS_ERROR_CODE(res))
        {
          res = SPIBusIFNotifyIOComplete(xMsg.pxSensor);
        }
        break;

      case SM_MESSAGE_ID_SPI_BUS_WRITE:
        SPIMasterDriverSelectDevice((SPIMasterDriver_t *) p_obj->p_driver, xMsg.pxSensor->p_cs_gpio_port,
                                    xMsg.pxSensor->cs_gpio_pin);
        res = IIODrvWrite(p_obj->p_driver, xMsg.pnData, xMsg.nDataSize, xMsg.nRegAddr);
        SPIMasterDriverDeselectDevice((SPIMasterDriver_t *) p_obj->p_driver, xMsg.pxSensor->p_cs_gpio_port,
                                      xMsg.pxSensor->cs_gpio_pin);
        if (!SYS_IS_ERROR_CODE(res))
        {
          res = SPIBusIFNotifyIOComplete(xMsg.pxSensor);
        }
        break;

      default:
        //TODO: STF -  need to notify the error
        break;
    }
  }

  return res;
}

static int32_t SPIBusTaskWrite(void *p_sensor, uint16_t reg, uint8_t *p_data, uint16_t size)
{
  assert_param(p_sensor);
  SPIBusIF *p_spi_sensor = (SPIBusIF *) p_sensor;
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint16_t auto_inc = p_spi_sensor->auto_inc;

  struct spiIOMessage_t msg =
  {
    .messageId = SM_MESSAGE_ID_SPI_BUS_WRITE,
    .pxSensor = p_spi_sensor,
    .nRegAddr = reg | auto_inc,
    .pnData = p_data,
    .nDataSize = size
  };

  // if (s_xTaskObj.m_xInQueue != NULL) {//TODO: STF.Port - how to know if the task has been initialized ??
  if (SYS_IS_CALLED_FROM_ISR())
  {
    /* we cannot read and write in the SPI BUS from an ISR. Notify the error */
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SPIBUS_TASK_IO_ERROR_CODE);
    res = SYS_SPIBUS_TASK_IO_ERROR_CODE;
  }
  else
  {
    if (TX_SUCCESS != tx_queue_send(p_spi_sensor->super.p_request_queue, &msg, AMT_MS_TO_TICKS(SPIBUS_OP_WAIT_MS)))
    {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SPIBUS_TASK_IO_ERROR_CODE);
      res = SYS_SPIBUS_TASK_IO_ERROR_CODE;
    }
  }
//  }

  if (!SYS_IS_ERROR_CODE(res))
  {
    /* Wait until the operation is completed */
    res = SPIBusIFWaitIOComplete(p_spi_sensor);
  }

  return res;
}

static int32_t SPIBusTaskRead(void *p_sensor, uint16_t reg, uint8_t *p_data, uint16_t size)
{
  assert_param(p_sensor);
  SPIBusIF *p_spi_sensor = (SPIBusIF *) p_sensor;
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint16_t auto_inc = p_spi_sensor->auto_inc;

  struct spiIOMessage_t msg =
  {
    .messageId = SM_MESSAGE_ID_SPI_BUS_READ,
    .pxSensor = p_spi_sensor,
    .nRegAddr = reg | 0x80 | auto_inc,
    .pnData = p_data,
    .nDataSize = size
  };

  // if (s_xTaskObj.m_xInQueue != NULL) { //TODO: STF.Port - how to know if the task has been initialized ??
  if (SYS_IS_CALLED_FROM_ISR())
  {
    /* we cannot read and write in the SPI BUS from an ISR. Notify the error */
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SPIBUS_TASK_IO_ERROR_CODE);
    res = SYS_SPIBUS_TASK_IO_ERROR_CODE;
  }
  else
  {
    if (TX_SUCCESS != tx_queue_send(p_spi_sensor->super.p_request_queue, &msg, AMT_MS_TO_TICKS(SPIBUS_OP_WAIT_MS)))
    {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SPIBUS_TASK_IO_ERROR_CODE);
      res = SYS_SPIBUS_TASK_IO_ERROR_CODE;
    }
  }
//  }

  if (!SYS_IS_ERROR_CODE(res))
  {
    /* Wait until the operation is completed */
    res = SPIBusIFWaitIOComplete(p_spi_sensor);
  }

  return res;
}
