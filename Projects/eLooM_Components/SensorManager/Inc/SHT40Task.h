/**
  ******************************************************************************
  * @file    SHT40Task.h
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
#ifndef SHT40TASK_H_
#define SHT40TASK_H_

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

/**
  * Create  type name for _SHT40Task.
  */
typedef struct _SHT40Task SHT40Task;

/**
  *  SHT40Task internal structure.
  */
struct _SHT40Task
{
  /**
    * Base class object.
    */
  AManagedTaskEx super;

  // Task variables should be added here.

  /**
    * Bus IF object used to connect the sensor task to the specific bus.
    */
  ABusIF *p_sensor_bus_if;

  /**
    * Implements the temperature ISensor interface.
    */
  ISensorMems_t temp_sensor_if;

  /**
    * Implements the humidity ISensor interface.
    */
  ISensorMems_t hum_sensor_if;

  /**
    * Specifies sensor capabilities.
    */
  const SensorDescriptor_t *temp_sensor_descriptor;

  /**
    * Specifies sensor capabilities.
    */
  const SensorDescriptor_t *hum_sensor_descriptor;

  /**
    * Specifies sensor configuration.
    */
  SensorStatus_t temp_sensor_status;

  /**
    * Specifies sensor configuration.
    */
  SensorStatus_t hum_sensor_status;

  /**
    * Data
    */
  EMData_t temp_data;

  /**
    * Data
    */
  EMData_t hum_data;

  /**
    * Specifies the sensor ID for the temperature sensor.
    */
  uint8_t temp_id;

  /**
    * Specifies the sensor ID for the humidity sensor.
    */
  uint8_t hum_id;

  /**
    * Synchronization object used to send command to the task.
    */
  TX_QUEUE in_queue;

  /**
    * Buffer to store the temperature data read.
    */
  float temperature;

  /**
    * Buffer to store the humidity data read.
    */
  float humidity;

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_temp_event_src;

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_hum_event_src;

  /**
    * Used to update the instantaneous odr.
    */
  double prev_timestamp;

  /**
    * Software timer used to generate the read command
    */
  TX_TIMER read_timer;

  /**
    * Specifies the ms delay between 2 consecutive read (it depends from odr)
    */
  uint16_t task_delay;
};

// Public API declaration
//***********************

/**
  * Get the ISourceObservable interface for the temperature.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *SHT40TaskGetTempSensorIF(SHT40Task *_this);

/**
  * Get the ISourceObservable interface for the humidity.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *SHT40TaskGetHumSensorIF(SHT40Task *_this);

/**
  * Allocate an instance of SHT40Task in the system heap.
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the SHT40 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *SHT40TaskAlloc(const void *pIRQConfig, const void *pCSConfig);

/**
  * Call the default ::SHT40TaskAlloc and then it overwrite sensor name
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the SHT40 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *SHT40TaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *p_name);

/**
  * Allocate an instance of ::SHT40Task in a memory block specified by the application.
  * The size of the memory block must be greater or equal to `sizeof(SHT40Task)`.
  * This allocator allows the application to avoid the dynamic allocation.
  *
  * \code
  * SHT40Task sensor_task;
  * SHT40TaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(SHT40Task)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the SHT40 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *SHT40TaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig);

/**
  * Call the default ::SHT40TaskAlloc and then it overwrite sensor name
  *
  * \code
  * SHT40Task sensor_task;
  * SHT40TaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(SHT40Task)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the SHT40 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *SHT40TaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                            const char *p_name);

/**
  * Get the Bus interface for the sensor task.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the Bus interface of the sensor.
  */
ABusIF *SHT40TaskGetSensorIF(SHT40Task *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *SHT40TaskGetTempEventSrcIF(SHT40Task *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *SHT40TaskGetHumEventSrcIF(SHT40Task *_this);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* SHT40TASK_H_ */
