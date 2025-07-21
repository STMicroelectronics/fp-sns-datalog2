/**
  ******************************************************************************
  * @file    ISM6HG256XTask.h
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
#ifndef ISM6HG256XTASK_H_
#define ISM6HG256XTASK_H_

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
#include "ISensorLL.h"
#include "ISensorLL_vtbl.h"
#include "mx.h"

#define ISM6HG256X_MAX_DRDY_PERIOD           (1.0)    /* seconds */

#ifndef ISM6HG256X_MAX_WTM_LEVEL
#define ISM6HG256X_MAX_WTM_LEVEL             (145)    /* samples */
#endif

#define ISM6HG256X_MIN_WTM_LEVEL             (16)     /* samples */
#define ISM6HG256X_MAX_SAMPLES_PER_IT        (ISM6HG256X_MAX_WTM_LEVEL)


#define ISM6HG256X_CFG_MAX_LISTENERS         2

/**
  * Create  type name for _ISM6HG256XTask.
  */
typedef struct _ISM6HG256XTask ISM6HG256XTask;

/**
  *  ISM6HG256XTask internal structure.
  */
struct _ISM6HG256XTask
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
    * Implements the accelerometer ISensorMems interface.
    */
  ISensorMems_t acc_sensor_if;

  /**
    * Implements the high-g accelerometer ISensorMems interface.
    */
  ISensorMems_t hg_acc_sensor_if;

  /**
    * Implements the gyroscope ISensorMems interface.
    */
  ISensorMems_t gyro_sensor_if;

  /**
    * Implements the mlc ISensorMems interface.
    */
  ISensorMems_t mlc_sensor_if;

  /**
    * Implements the ISensorMemsLL interface - Sensor Low-level.
    */
  ISensorLL_t sensor_ll_if;

  /**
    * Specifies accelerometer sensor capabilities.
    */
  const SensorDescriptor_t *acc_sensor_descriptor;

  /**
    * Specifies accelerometer sensor configuration.
    */
  SensorStatus_t acc_sensor_status;

  /**
    * Specifies acc output data
    */
  EMData_t data_acc;
  /**
    * Specifies high-g accelerometer sensor capabilities.
    */
  const SensorDescriptor_t *hg_acc_sensor_descriptor;

  /**
    * Specifies high-g accelerometer sensor configuration.
    */
  SensorStatus_t hg_acc_sensor_status;

  /**
    * Specifies high-g acc output data
    */
  EMData_t data_hg_acc;
  /**
    * Specifies gyroscope sensor capabilities.
    */
  const SensorDescriptor_t *gyro_sensor_descriptor;

  /**
    * Specifies gyroscope sensor configuration.
    */
  SensorStatus_t gyro_sensor_status;

  /**
    * Specifies gyro output data
    */
  EMData_t data_gyro;
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
  EMData_t data;

  /**
    * Specifies the sensor ID for the accelerometer subsensor.
    */
  uint8_t acc_id;

  /**
      * Specifies the sensor ID for the high-g accelerometer subsensor.
      */
  uint8_t hg_acc_id;

  /**
    * Specifies the sensor ID for the gyroscope subsensor.
    */
  uint8_t gyro_id;

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

#if ISM6HG256X_FIFO_ENABLED
  /**
    * Buffer to store the data read from the sensor FIFO.
    * It is reused also to save data from the faster subsensor
    */
  uint8_t p_fast_sensor_data_buff[ISM6HG256X_MAX_SAMPLES_PER_IT * 7];

  /**
    * Buffer to store the data from the slower subsensor 1
    */
  uint8_t p_medium_sensor_data_buff[ISM6HG256X_MAX_SAMPLES_PER_IT / 2 * 6];

  /**
      * Buffer to store the data from the slower subsensor 2
      */
  uint8_t p_slow_sensor_data_buff[ISM6HG256X_MAX_SAMPLES_PER_IT / 3 * 6];
#else
  /**
    * Buffer to store the data read from the sensor FIFO.
    * It is reused also to save data from the faster subsensor
    */
  uint8_t p_acc_sample[6];

  /**
    * Buffer to store the data read from the sensor FIFO.
    * It is reused also to save data from the faster subsensor
    */
  uint8_t p_hg_acc_sample[6];

  /**
    * Buffer to store the data from the slower subsensor
   */
  uint8_t p_gyro_sample[6];

  /**
    * Save acc data ready status
   */
  uint8_t acc_drdy;
  /**
    * Save acc data ready status
    */
  uint8_t hg_acc_drdy;

  /**
    * Save gyro data ready status
    */
  uint8_t gyro_drdy;
#endif

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
  uint16_t samples_per_it;

  /**
    * If both subsensors are active, specifies the amount of ACC samples in the FIFO
    */
  uint16_t acc_samples_count;

  /**
    * If both subsensors are active, specifies the amount of ACC samples in the FIFO
    */
  uint16_t hg_acc_samples_count;

  /**
    * If both subsensors are active, specifies the amount of GYRO samples in the FIFO
    */
  uint16_t gyro_samples_count;

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_acc_event_src;

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_hg_acc_event_src;

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_gyro_event_src;

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
  ULONG ism6hg256x_task_cfg_timer_period_ms;

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
};

// Public API declaration
//***********************

/**
  * Get the ISourceObservable interface for the accelerometer.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *ISM6HG256XTaskGetAccSensorIF(ISM6HG256XTask *_this);

/**
  * Get the ISourceObservable interface for the high g accelerometer.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *ISM6HG256XTaskGetHgAccSensorIF(ISM6HG256XTask *_this);

/**
  * Get the ISourceObservable interface for the gyroscope.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *ISM6HG256XTaskGetGyroSensorIF(ISM6HG256XTask *_this);

/**
  * Get the ISourceObservable interface for mlc.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *ISM6HG256XTaskGetMlcSensorIF(ISM6HG256XTask *_this);

/**
  * Get the ::ISensorMemsLL_t interface the sensor.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISensorMemsLL_t
  */
ISensorLL_t *ISM6HG256XTaskGetSensorLLIF(ISM6HG256XTask *_this);

/**
  * Allocate an instance of ISM6HG256XTask in the system heap.
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM6HG256X sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pMLCConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM33DHCX MLC and configured in EXTI mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *ISM6HG256XTaskAlloc(const void *pIRQConfig, const void *pMLCConfig, const void *pCSConfig);

/**
  * Call the default ::ISM6HG256XTaskAlloc and then it overwrite sensor name
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM6HG256X sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pMLCConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM33DHCX MLC and configured in EXTI mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *ISM6HG256XTaskAllocSetName(const void *pIRQConfig, const void *pMLCConfig, const void *pCSConfig,
                                           const char *p_name);

/**
  * Allocate an instance of ::ISM6HG256XTask in a memory block specified by the application.
  * The size of the memory block must be greater or equal to `sizeof(ISM6HG256XTask)`.
  * This allocator allows the application to avoid the dynamic allocation.
  *
  * \code
  * ISM6HG256XTask sensor_task;
  * ISM6HG256XTaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(ISM6HG256XTask)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM6HG256X sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pMLCConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM33DHCX MLC and configured in EXTI mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *ISM6HG256XTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pMLCConfig,
                                          const void *pCSConfig);

/**
  * Call the default ::ISM6HG256XTaskAlloc and then it overwrite sensor name
  *
  * \code
  * ISM6HG256XTask sensor_task;
  * ISM6HG256XTaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(ISM6HG256XTask)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM6HG256X sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pMLCConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM33DHCX MLC and configured in EXTI mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *ISM6HG256XTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pMLCConfig,
                                                 const void *pCSConfig,
                                                 const char *p_name);

/**
  * Get the Bus interface for the sensor task.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the Bus interface of the sensor.
  */
ABusIF *ISM6HG256XTaskGetSensorIF(ISM6HG256XTask *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *ISM6HG256XTaskGetAccEventSrcIF(ISM6HG256XTask *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *ISM6HG256XTaskGetHgAccEventSrcIF(ISM6HG256XTask *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *ISM6HG256XTaskGetGyroEventSrcIF(ISM6HG256XTask *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *ISM6HG256XTaskGetMlcEventSrcIF(ISM6HG256XTask *_this);

/**
  * IRQ callback
  */
void ISM6HG256XTask_EXTI_Callback(uint16_t Pin);
void INT2_DSV256X_EXTI_Callback(uint16_t Pin);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* ISM6HG256XTASK_H_ */
