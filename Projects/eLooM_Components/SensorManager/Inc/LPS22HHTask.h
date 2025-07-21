/**
  ******************************************************************************
  * @file    LPS22HHTask.h
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
#ifndef LPS22HHTASK_H_
#define LPS22HHTASK_H_

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

#define LPS22HH_MAX_DRDY_PERIOD           (1.0) /* seconds */

#ifndef LPS22HH_MAX_WTM_LEVEL
#define LPS22HH_MAX_WTM_LEVEL             (64) /* samples */
#endif

#define LPS22HH_MAX_SAMPLES_PER_IT        (LPS22HH_MAX_WTM_LEVEL)


#define LPS22HH_CFG_MAX_LISTENERS         2

/**
  * Create  type name for _LPS22HHTask.
  */
typedef struct _LPS22HHTask LPS22HHTask;

/**
  *  LPS22HHTask internal structure.
  */
struct _LPS22HHTask
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
    * Implements the temperature ISensorMems interface.
    */
  ISensorMems_t temp_sensor_if;

  /**
    * Implements the pressure ISensorMems interface.
    */
  ISensorMems_t press_sensor_if;

  /**
    * Specifies temperature sensor capabilities.
    */
  const SensorDescriptor_t *temp_sensor_descriptor;

  /**
    * Specifies temperature sensor configuration.
    */
  SensorStatus_t temp_sensor_status;

  /**
    * Specifies pressure sensor capabilities.
    */
  const SensorDescriptor_t *press_sensor_descriptor;

  /**
    * Specifies pressure sensor configuration.
    */
  SensorStatus_t press_sensor_status;

  EMData_t data;

  /**
    * Specifies the sensor ID for the temperature subsensor.
    */
  uint8_t temp_id;

  /**
    * Specifies the sensor ID for the pressure subsensor.
    */
  uint8_t press_id;

  /**
    * Synchronization object used to send command to the task.
    */
  TX_QUEUE in_queue;

  /**
    * Buffer to store the data read from the sensor FIFO
    */
  uint8_t p_sensor_data_buff[LPS22HH_MAX_WTM_LEVEL * 5];

  /**
    * Temperature data
    */
  float_t p_temp_data_buff[LPS22HH_MAX_WTM_LEVEL];

  /**
    * Pressure data
    */
  float_t p_press_data_buff[LPS22HH_MAX_WTM_LEVEL];

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_temp_event_src;

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_press_event_src;

  /**
    * Specifies the FIFO level
    */
  uint8_t fifo_level;

  /**
    * Specifies the FIFO watermark level (it depends from odr)
    */
  uint8_t samples_per_it;

  /**
    * Specifies the ms delay between 2 consecutive read (it depends from odr)
    */
  uint16_t task_delay;

  /**
    * Software timer used to generate the read command
    */
  TX_TIMER read_fifo_timer;

  /**
    * Used to update the instantaneous odr.
    */
  double_t prev_timestamp;
};

// Public API declaration
//***********************

/**
  * Get the ISourceObservable interface for the pressure.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *LPS22HHTaskGetPressSensorIF(LPS22HHTask *_this);

/**
  * Allocate an instance of LPS22HHTask in the system heap.
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the LPS22HH sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *LPS22HHTaskAlloc(const void *pIRQConfig, const void *pCSConfig);

/**
  * Call the default ::LPS22HHTaskAlloc and then it overwrite sensor name
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the LPS22HH sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *LPS22HHTaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *p_name);

/**
  * Allocate an instance of ::LPS22HHTask in a memory block specified by the application.
  * The size of the memory block must be greater or equal to `sizeof(LPS22HHTask)`.
  * This allocator allows the application to avoid the dynamic allocation.
  *
  * \code
  * LPS22HHTask sensor_task;
  * LPS22HHTaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(LPS22HHTask)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the LPS22HH sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *LPS22HHTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig);

/**
  * Call the default ::LPS22HHTaskAlloc and then it overwrite sensor name
  *
  * \code
  * LPS22HHTask sensor_task;
  * LPS22HHTaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(LPS22HHTask)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the LPS22HH sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *LPS22HHTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                              const char *p_name);

/**
  * Get the Bus interface for the sensor task.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the Bus interface of the sensor.
  */
ABusIF *LPS22HHTaskGetSensorIF(LPS22HHTask *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *LPS22HHTaskGetPressEventSrcIF(LPS22HHTask *_this);

IEventSrc *LPS22HHTaskGetTempEventSrcIF(LPS22HHTask *_this);

/**
  * IRQ callback
  */
void LPS22HHTask_EXTI_Callback(uint16_t nPin);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* LPS22HHTASK_H_ */
