/**
  ******************************************************************************
  * @file    ILPS28QSWTask.h
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
#ifndef ILPS28QSWTASK_H_
#define ILPS28QSWTASK_H_

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

#define ILPS28QSW_MAX_DRDY_PERIOD           (1.0)  /* seconds */

#ifndef ILPS28QSW_MAX_WTM_LEVEL
#define ILPS28QSW_MAX_WTM_LEVEL             (128)  /* samples */
#endif

#define ILPS28QSW_MIN_WTM_LEVEL             (16)
#define ILPS28QSW_MAX_SAMPLES_PER_IT        (ILPS28QSW_MAX_WTM_LEVEL)


#define ILPS28QSW_CFG_MAX_LISTENERS         2

/**
  * Create  type name for _ILPS28QSWTask.
  */
typedef struct _ILPS28QSWTask ILPS28QSWTask;

/**
  *  ILPS28QSWTask internal structure.
  */
struct _ILPS28QSWTask
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
    * Implements the pressure ISensor interface.
    */
  ISensorMems_t sensor_if;

  /**
    * Specifies pressure sensor capabilities.
    */
  const SensorDescriptor_t *sensor_descriptor;

  /**
    * Specifies pressure sensor configuration.
    */
  SensorStatus_t sensor_status;

  /**
     * Data
     */
  EMData_t data;
  /**
    * Specifies the sensor ID for the pressure subsensor.
    */
  uint8_t press_id;

  /**
    * Synchronization object used to send command to the task.
    */
  TX_QUEUE in_queue;

  /**
    * Pressure data
    */
  float_t p_press_data_buff[ILPS28QSW_MAX_WTM_LEVEL];

  /**
    * Sensor data from FIFO
    */
  uint8_t p_fifo_data_buff[ILPS28QSW_MAX_WTM_LEVEL * 3];

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
  * Get the ISourceObservable interface for the gyroscope.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *ILPS28QSWTaskGetPressSensorIF(ILPS28QSWTask *_this);

/**
  * Allocate an instance of ILPS28QSWTask in the system heap.
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ILPS28QSW sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *ILPS28QSWTaskAlloc(const void *pIRQConfig, const void *pCSConfig);

/**
  * Call the default ::ILPS28QSWTaskAlloc and then it overwrite sensor name
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ILPS28QSW sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *ILPS28QSWTaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const char *p_name);

/**
  * Allocate an instance of ::ILPS28QSWTask in a memory block specified by the application.
  * The size of the memory block must be greater or equal to `sizeof(ILPS28QSWTask)`.
  * This allocator allows the application to avoid the dynamic allocation.
  *
  * \code
  * ILPS28QSWTask sensor_task;
  * ILPS28QSWTaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(ILPS28QSWTask)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ILPS28QSW sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *ILPS28QSWTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig);

/**
  * Call the default ::ILPS28QSWTaskAlloc and then it overwrite sensor name
  *
  * \code
  * ILPS28QSWTask sensor_task;
  * ILPS28QSWTaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(ILPS28QSWTask)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ILPS28QSW sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *ILPS28QSWTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                                const char *p_name);

/**
  * Get the Bus interface for the sensor task.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the Bus interface of the sensor.
  */
ABusIF *ILPS28QSWTaskGetSensorIF(ILPS28QSWTask *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *ILPS28QSWTaskGetPressEventSrcIF(ILPS28QSWTask *_this);

/**
  * IRQ callback
  */
void ILPS28QSWTask_EXTI_Callback(uint16_t nPin);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* ILPS28QSWTASK_H_ */
