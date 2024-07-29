/**
  ******************************************************************************
  * @file    IIS2ICLXTask.h
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
#ifndef IIS2ICLXTASK_H_
#define IIS2ICLXTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "services/AManagedTaskEx.h"
#include "services/AManagedTaskEx_vtbl.h"
#include "ABusIF.h"
#include "events/DataEventSrc.h"
#include "events/DataEventSrc_vtbl.h"
#include "ISensorMems.h"
#include "ISensorMems_vtbl.h"
#include "mx.h"

#define IIS2ICLX_MAX_DRDY_PERIOD           (1.0)  /* seconds */

#ifndef IIS2ICLX_MAX_WTM_LEVEL
#define IIS2ICLX_MAX_WTM_LEVEL             (128)  /* samples */
#endif

#define IIS2ICLX_MIN_WTM_LEVEL             (16)
#define IIS2ICLX_MAX_SAMPLES_PER_IT        (IIS2ICLX_MAX_WTM_LEVEL)


#define IIS2ICLX_CFG_MAX_LISTENERS           2

/**
  * Create  type name for _IIS2ICLXTask.
  */
typedef struct _IIS2ICLXTask IIS2ICLXTask;

/**
  *  IIS2ICLXTask internal structure.
  */
struct _IIS2ICLXTask
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
  ISensorMems_t sensor_if;

  /**
    * Specifies sensor capabilities.
    */
  const SensorDescriptor_t *sensor_descriptor;

  /**
    * Specifies sensor configuration.
    */
  SensorStatus_t sensor_status;

  /**
    * Data
    */
  EMData_t data;
  /**
    * Specifies the sensor ID for the accelerometer subsensor.
    */
  uint8_t acc_id;

  /**
    * Synchronization object used to send command to the task.
    */
  TX_QUEUE in_queue;

  /**
    * Buffer to store the data read from the sensor
    */
  uint8_t p_sensor_data_buff[IIS2ICLX_MAX_SAMPLES_PER_IT * 4];

  /**
    * Buffer to store the data read from the FIFO
    */
  uint8_t p_fifo_data_buff[IIS2ICLX_MAX_SAMPLES_PER_IT * 7];

  /**
    * Specifies the FIFO level
    */
  uint16_t fifo_level;

  /**
    * Specifies number of samples read each interrupts
    */
  uint16_t samples_per_it;

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_event_src;

  /**
    * Software timer used to generate the read command
    */
  TX_TIMER read_timer;

  /**
    * Timer period used to schedule the read command
    */
  ULONG iis2iclx_task_cfg_timer_period_ms;

  /**
    * Used to update the instantaneous odr.
    */
  double prev_timestamp;

  /*
    * First data ready flag: fist data must be discarded, see sensor AN
    * */
  uint8_t first_data_ready;
};


// Public API declaration
//***********************

/**
  * Get the ISourceObservable interface for the accelerometer.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *IIS2ICLXTaskGetAccSensorIF(IIS2ICLXTask *_this);

/**
  * Allocate an instance of IIS2ICLXTask.
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the IIS2ICLX sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IIS2ICLXTaskAlloc(const void *pIRQConfig, const void *pCSConfig);

/**
  * Call the default ::IIS2ICLXTaskAlloc and then it overwrite sensor name
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the IIS2ICLX sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IIS2ICLXTaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *p_name);

/**
  * Allocate an instance of ::IIS2ICLXTask in a memory block specified by the application.
  * The size of the memory block must be greater or equal to `sizeof(IIS2ICLXTask)`.
  * This allocator allows the application to avoid the dynamic allocation.
  *
  * \code
  * IIS2ICLXTask sensor_task;
  * IIS2ICLXTaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(IIS2ICLXTask)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the IIS2ICLX sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IIS2ICLXTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig);

/**
  * Call the default ::IIS2ICLXTaskAlloc and then it overwrite sensor name
  *
  * \code
  * IIS2ICLXTask sensor_task;
  * IIS2ICLXTaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(IIS2ICLXTask)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the IIS2ICLX sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IIS2ICLXTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                               const char *p_name);

/**
  * Get the Bus interface for the sensor task.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the Bus interface of the sensor.
  */
ABusIF *IIS2ICLXTaskGetSensorIF(IIS2ICLXTask *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *IIS2ICLXTaskGetEventSrcIF(IIS2ICLXTask *_this);

/**
  * IRQ callback
  */
void IIS2ICLXTask_EXTI_Callback(uint16_t nPin);

// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* IIS2ICLXTASK_H_ */
