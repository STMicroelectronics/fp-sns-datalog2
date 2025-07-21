/**
  ******************************************************************************
  * @file    LIS2DU12Task.h
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
#ifndef LIS2DU12TASK_H_
#define LIS2DU12TASK_H_

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

#define LIS2DU12_MAX_DRDY_PERIOD           (1.0)    /* seconds */

#ifndef LIS2DU12_MAX_WTM_LEVEL
#define LIS2DU12_MAX_WTM_LEVEL             (92)    /* samples */
#endif

#define LIS2DU12_MAX_SAMPLES_PER_IT        (LIS2DU12_MAX_WTM_LEVEL)


#define LIS2DU12_CFG_MAX_LISTENERS         2

/**
  * Create  type name for _LIS2DU12Task.
  */
typedef struct _LIS2DU12Task LIS2DU12Task;

/**
  *  LIS2DU12Task internal structure.
  */
struct _LIS2DU12Task
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
    * Implements the accelerometer ISensorMems interface.
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
  uint8_t p_sensor_data_buff[LIS2DU12_MAX_SAMPLES_PER_IT * 6];

  /**
    * Specifies the FIFO level
    */
  uint8_t fifo_level;

  /**
    * Specifies the FIFO watermark level (it depends from odr)
    */
  uint8_t samples_per_it;

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
  ULONG lis2du12_task_cfg_timer_period_ms;

  /**
    * Used to update the instantaneous odr.
    */
  double_t prev_timestamp;

  /*
    * First data ready flag: fist data must be discarded, see sensor AN
    * */
  uint8_t first_data_ready;

  uint8_t odr_count;
  float_t delta_timestamp_sum;
  uint16_t samples_sum;
};

// Public API declaration
//***********************

/**
  * Get the ISourceObservable interface for the accelerometer.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *LIS2DU12TaskGetAccSensorIF(LIS2DU12Task *_this);

/**
  * Allocate an instance of LIS2DU12Task in the system heap.
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the LIS2DU12 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *LIS2DU12TaskAlloc(const void *pIRQConfig, const void *pCSConfig);

/**
  * Call the default ::LIS2DU12TaskAlloc and then it overwrite sensor name
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the LIS2DU12 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *LIS2DU12TaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *p_name);

/**
  * Allocate an instance of ::LIS2DU12Task in a memory block specified by the application.
  * The size of the memory block must be greater or equal to `sizeof(LIS2DU12Task)`.
  * This allocator allows the application to avoid the dynamic allocation.
  *
  * \code
  * LIS2DU12Task sensor_task;
  * LIS2DU12TaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(LIS2DU12Task)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the LIS2DU12 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *LIS2DU12TaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig);

/**
  * Call the default ::LIS2DU12TaskAlloc and then it overwrite sensor name
  *
  * \code
  * LIS2DU12Task sensor_task;
  * LIS2DU12TaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(LIS2DU12Task)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the LIS2DU12 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *LIS2DU12TaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                               const char *p_name);
/**
  * Get the Bus interface for the sensor task.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the Bus interface of the sensor.
  */
ABusIF *LIS2DU12TaskGetSensorIF(LIS2DU12Task *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *LIS2DU12TaskGetEventSrcIF(LIS2DU12Task *_this);

/**
  * IRQ callback
  */
void LIS2DU12Task_EXTI_Callback(uint16_t nPin);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* LIS2DU12TASK_H_ */
