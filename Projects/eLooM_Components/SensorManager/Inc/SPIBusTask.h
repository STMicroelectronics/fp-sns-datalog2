/**
  ******************************************************************************
  * @file    SPIBusTask.h
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
#ifndef SPIBUSTASK_H_
#define SPIBUSTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "services/AManagedTaskEx.h"
#include "services/AManagedTaskEx_vtbl.h"
#include "drivers/IIODriver.h"
#include "drivers/IIODriver_vtbl.h"
#include "SPIBusIF.h"
#include "IBus.h"
#include "IBus_vtbl.h"

#include "SMMessageParser.h"


/* task error code */
/*******************/

#ifndef SYS_NO_ERROR_CODE
#define SYS_NO_ERROR_CODE                                   0
#endif
#ifndef SYS_BASE_SPIBUS_TASK_ERROR_CODE
#define SYS_BASE_SPIBUS_TASK_ERROR_CODE                     1
#endif
#define SYS_SPIBUS_TASK_IO_ERROR_CODE                       SYS_BASE_SPIBUS_TASK_ERROR_CODE + 1
#define SYS_SPIBUS_TASK_RESUME_ERROR_CODE                   SYS_BASE_SPIBUS_TASK_ERROR_CODE + 2
#define SYS_SPIBUS_TASK_UNSUPPORTED_CMD_ERROR_CODE          SYS_BASE_SPIBUS_TASK_ERROR_CODE + 3


/**
  * Create  type name for _SPIBusTask.
  */
typedef struct _SPIBusTask SPIBusTask;

/**
  *  SPIBusTask internal structure.
  */
struct _SPIBusTask
{
  /**
    * Base class object.
    */
  AManagedTaskEx super;

  /* Task variables should be added here. */

  /**
    * Driver object.
    */
  IIODriver *p_driver;

  /**
    * HAL driver configuration parameters.
    */
  const void *p_mx_drv_cfg;

  /**
    * Bus interface used to connect and disconnect devices to this object.
    */
  IBus *p_bus_if;

  /**
    * Task message queue. Read and write request are wrapped into message posted in this queue.
    */
  TX_QUEUE in_queue;

  /**
    * Count the number of devices connected to the bus. It can be used in further version to
    * de-initialize the SPI IP in some of the PM state.
    */
  uint8_t connected_devices;
};


// Public API declaration
//***********************

/**
  * Allocate an instance of SPIBusTask.
  *
  * @param p_mx_drv_cfg [IN] specifies a ::MX_SPIParams_t instance declared in the mx.h file.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *SPIBusTaskAlloc(const void *p_mx_drv_cfg);

/**
  * Connect a device to the bus using its interface.
  *
  * @param _this [IN] specifies a task object.
  * @param pxBusIF [IN] specifies the device interface to connect.
  * @return SYS_NO_ERROR_CODE is success, SYS_INVALID_PARAMETER_ERROR_CODE if pxBuff is NULL.
  */
sys_error_code_t SPIBusTaskConnectDevice(SPIBusTask *_this, SPIBusIF *p_bus_if);

/**
  * Disconnect a device from the bus using its interface.
  *
  * @param _this [IN] specifies a task object.
  * @param pxBusIF [IN] specifies the device interface to connect.
  * @return SYS_NO_ERROR_CODE is success, SYS_INVALID_PARAMETER_ERROR_CODE if pxBuff is NULL.
  */
sys_error_code_t SPIBusTaskDisconnectDevice(SPIBusTask *_this, SPIBusIF *p_bus_if);

/**
  * Get the ::IBus interface of the task.
  *
  * @param _this [IN] specifies a task object.
  * @return the ::IBus interface of the task.
  */
IBus *SPIBusTaskGetBusIF(SPIBusTask *_this);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* SPIBUSTASK_H_ */
