/**
  ******************************************************************************
  * @file    LIS2DUXS12Task.h
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
#ifndef LIS2DUXS12TASK_H_
#define LIS2DUXS12TASK_H_

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
#include "ISensorLL.h"
#include "ISensorLL_vtbl.h"
#include "mx.h"

#define LIS2DUXS12_MAX_DRDY_PERIOD           (1.0)    /* seconds */

#ifndef LIS2DUXS12_MAX_WTM_LEVEL
#define LIS2DUXS12_MAX_WTM_LEVEL             (64)    /* samples */
#endif

#define LIS2DUXS12_CFG_MAX_LISTENERS         2

/**
  * Create  type name for _LIS2DUXS12Task.
  */
typedef struct _LIS2DUXS12Task LIS2DUXS12Task;

/**
  *  LIS2DUXS12Task internal structure.
  */
struct _LIS2DUXS12Task
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
    * MLC GPIO configuration parameters.
    */
  const MX_GPIOParams_t *pMLCConfig;

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
    * Implements the mlc ISensor interface.
    */
  ISensorMems_t mlc_sensor_if;

  /**
    * Implements the ISensorLL interface - Sensor Low-level.
    */
  ISensorLL_t sensor_ll_if;

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
    * Specifies mlc sensor capabilities.
    */
  const SensorDescriptor_t *mlc_sensor_descriptor;

  /**
    * Specifies mlc sensor configuration.
    */
  SensorStatus_t mlc_sensor_status;

  /**
    * Specifies mlc output data
    */
  EMData_t data_mlc;

  /**
    * Specifies the sensor ID for the accelerometer subsensor.
    */
  uint8_t acc_id;

  /**
    * Specifies the sensor ID for the mlc subsensor.
    */
  uint8_t mlc_id;

  /**
    * Specifies mlc status.
    */
  boolean_t mlc_enable;

  /**
    * Synchronization object used to send command to the task.
    */
  TX_QUEUE in_queue;

  /**
    * Buffer to store the data read from the sensor
    */
  uint8_t p_sensor_data_buff[LIS2DUXS12_MAX_WTM_LEVEL * 7];

  /**
    * Buffer to store the data from mlc
    */
  uint8_t p_mlc_sensor_data_buff[5];

  /**
    * Specifies the FIFO level
    */
  uint16_t fifo_level;

  /**
    * Specifies the FIFO watermark level (it depends from odr)
    */
  uint8_t samples_per_it;

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_event_src;

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_mlc_event_src;

  /**
    * Software timer used to generate the read command
    */
  TX_TIMER read_timer;

  /**
    * Timer period used to schedule the read command
    */
  ULONG lis2duxs12_task_cfg_timer_period_ms;

  /**
    * Software timer used to generate the mlc read command
    */
  TX_TIMER mlc_timer;

  /**
    * Used to update the instantaneous odr.
    */
  double_t prev_timestamp;

  /**
    * Internal model (FW) is in sync with the component (HW registers)
    */
  bool sync;

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
  * Get the ISourceObserver interface for the accelerometer.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *LIS2DUXS12TaskGetAccSensorIF(LIS2DUXS12Task *_this);

/**
  * Get the ISourceObservable interface for mlc.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *LIS2DUXS12TaskGetMlcSensorIF(LIS2DUXS12Task *_this);

/**
  * Get the ::ISensorLL_t interface the sensor.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISensorLL_t
  */
ISensorLL_t *LIS2DUXS12TaskGetSensorLLIF(LIS2DUXS12Task *_this);

/**
  * Allocate an instance of LIS2DUXS12Task in the system heap.
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the LIS2DUXS12 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pMLCConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM33DHCX MLC and configured in EXTI mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *LIS2DUXS12TaskAlloc(const void *pIRQConfig, const void *pMLCConfig, const void *pCSConfig);

/**
  * Call the default ::LIS2DUXS12TaskAlloc and then it overwrite sensor name
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the LIS2DUXS12 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pMLCConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM33DHCX MLC and configured in EXTI mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *LIS2DUXS12TaskAllocSetName(const void *pIRQConfig, const void *pMLCConfig, const void *pCSConfig,
                                           const char *p_name);

/**
  * Allocate an instance of ::LIS2DUXS12Task in a memory block specified by the application.
  * The size of the memory block must be greater or equal to `sizeof(LIS2DUXS12Task)`.
  * This allocator allows the application to avoid the dynamic allocation.
  *
  * \code
  * LIS2DUXS12Task sensor_task;
  * LIS2DUXS12TaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(LIS2DUXS12Task)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the LIS2DUXS12 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pMLCConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM33DHCX MLC and configured in EXTI mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *LIS2DUXS12TaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pMLCConfig,
                                          const void *pCSConfig);

/**
  * Call the default ::LIS2DUXS12TaskAlloc and then it overwrite sensor name
  *
  * \code
  * LIS2DUXS12Task sensor_task;
  * LIS2DUXS12TaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(LIS2DUXS12Task)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the LIS2DUXS12 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pMLCConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM33DHCX MLC and configured in EXTI mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *LIS2DUXS12TaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pMLCConfig,
                                                 const void *pCSConfig,
                                                 const char *p_name);

/**
  * Get the SPI interface for the sensor task.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the Bus interface of the sensor.
  */
ABusIF *LIS2DUXS12TaskGetSensorIF(LIS2DUXS12Task *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *LIS2DUXS12TaskGetEventSrcIF(LIS2DUXS12Task *_this);
/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *LIS2DUXS12TaskGetMlcEventSrcIF(LIS2DUXS12Task *_this);

/**
  * IRQ callback
  */
void LIS2DUXS12Task_EXTI_Callback(uint16_t Pin);
void INT2_DULPX_EXTI_Callback(uint16_t Pin);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* LIS2DUXS12TASK_H_ */
