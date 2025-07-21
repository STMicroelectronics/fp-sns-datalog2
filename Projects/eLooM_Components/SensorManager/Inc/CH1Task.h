/**
  ******************************************************************************
  * @file    CH1Task.h
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
#ifndef CH1TASK_H_
#define CH1TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/AManagedTaskEx.h"
#include "services/AManagedTaskEx_vtbl.h"
#include "ABusIF.h"
#include "SensorManager.h"
#include "events/DataEventSrc.h"
#include "events/DataEventSrc_vtbl.h"
#include "ISensorMems.h"
#include "ISensorMems_vtbl.h"
#include "mx.h"


/* Definition of ADCx conversions data table size */
#ifndef ADC_CONVERTED_DATA_BUFFER_SIZE
#define ADC_CONVERTED_DATA_BUFFER_SIZE   ((uint32_t)  8192)   /* Size of array aADCxConvertedData[] */
#endif


/**
  * Create  type name for _CH1Task.
  */
typedef struct _CH1Task CH1Task;

/**
  *  CH1Task internal structure.
  */
struct _CH1Task
{
  /**
    * Base class object.
    */
  AManagedTaskEx super;

  // Task variables should be added here.

  /**
    * IRQ GPIO configuration parameters.
    */
  const MX_ADCParams_t *pADCConfig;

  /**
    * Bus IF object used to connect the sensor task to the specific bus.
    */
  ABusIF *p_sensor_bus_if;

  /**
    * Implements the ISensorMems interface.
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
    * Specifies the sensor ID for the sensor.
    */
  uint8_t id;

  /**
    * Synchronization object used to send command to the task.
    */
  TX_QUEUE in_queue;

  /**
    * Buffer to store the data read from the sensor FIFO.
    */
  uint16_t aADCxConvertedData[ADC_CONVERTED_DATA_BUFFER_SIZE];
  int16_t sensor_data[ADC_CONVERTED_DATA_BUFFER_SIZE];

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_event_src;

  /**
    * Used to update the instantaneous odr.
    */
  double_t prev_timestamp;
};

// Public API declaration
//***********************

/**
  * Get the ISourceObservable interface for the accelerometer.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *CH1TaskGetPowSensorIF(CH1Task *_this);

/**
  * Allocate an instance of CH1Task in the system heap.
  *
  * @param pADCConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the CH1 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *CH1TaskAlloc(const void *pADCConfig);

/**
  * Call the default ::CH1TaskAlloc and then it overwrite sensor name
  *
  * @param pADCConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the CH1 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *CH1TaskAllocSetName(const void *pADCConfig, const char *p_name);

/**
  * Allocate an instance of ::CH1Task in a memory block specified by the application.
  * The size of the memory block must be greater or equal to `sizeof(CH1Task)`.
  * This allocator allows the application to avoid the dynamic allocation.
  *
  * \code
  * CH1Task sensor_task;
  * CH1TaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(CH1Task)`.
  * @param pADCConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the CH1 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *CH1TaskStaticAlloc(void *p_mem_block, const void *pADCConfig);

/**
  * Call the default ::CH1TaskAlloc and then it overwrite sensor name
  *
  * \code
  * CH1Task sensor_task;
  * CH1TaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(CH1Task)`.
  * @param pADCConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the CH1 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *CH1TaskStaticAllocSetName(void *p_mem_block, const void *pADCConfig, const char *p_name);

/**
  * Get the Bus interface for the sensor task.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the Bus interface of the sensor.
  */
ABusIF *CH1TaskGetSensorIF(CH1Task *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *CH1TaskGetEventSrcIF(CH1Task *_this);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* CH1TASK_H_ */
