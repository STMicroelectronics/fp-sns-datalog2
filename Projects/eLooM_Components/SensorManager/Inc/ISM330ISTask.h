/**
  ******************************************************************************
  * @file    ISM330ISTask.h
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

#ifndef ISM330ISTASK_H_
#define ISM330ISTASK_H_

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

#define ISM330IS_MAX_DRDY_PERIOD           (1.0)    /* seconds */

#ifndef ISM330IS_MAX_WTM_LEVEL
#define ISM330IS_MAX_WTM_LEVEL             (256)    /* samples */
#endif

#define ISM330IS_MIN_WTM_LEVEL             (16)     /* samples */
#define ISM330IS_MAX_SAMPLES_PER_IT        (ISM330IS_MAX_WTM_LEVEL)


#define ISM330IS_CFG_MAX_LISTENERS         2

/**
  * Create  type name for _ISM330ISTask.
  */
typedef struct _ISM330ISTask ISM330ISTask;

/**
  *  ISM330ISTask internal structure.
  */
struct _ISM330ISTask
{
  /**
    * Base class object.
    */
  AManagedTaskEx super;

  // Task variables should be added here.

  /**
    * IRQ GPIO configuration parameters.
    */
  const MX_GPIOParams_t *p_irq_config;

  /**
    * ISPU GPIO configuration parameters.
    */
  const MX_GPIOParams_t *p_ispu_config;

  /**
    * SPI CS GPIO configuration parameters.
    */
  const MX_GPIOParams_t *p_cs_config;

  /**
    * Bus IF object used to connect the sensor task to the specific bus.
    */
  ABusIF *p_sensor_bus_if;

  /**
    * Implements the accelerometer ISensor interface.
    */
  ISensorMems_t acc_sensor_if;

  /**
    * Implements the gyroscope ISensor interface.
    */
  ISensorMems_t gyro_sensor_if;

  /**
    * Implements the ispu ISensor interface.
    */
  ISensorMems_t ispu_sensor_if;

  /**
    * Implements the ISensorLL interface - Sensor Low-level.
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
    * Specifies ispu sensor capabilities.
    */
  const SensorDescriptor_t *ispu_sensor_descriptor;

  /**
    * Specifies ispu sensor configuration.
    */
  SensorStatus_t ispu_sensor_status;

  /**
    * Specifies ispu output data
    */
  EMData_t data_ispu;

  /**
    * Specifies the sensor ID for the accelerometer subsensor.
    */
  uint8_t acc_id;

  /**
    * Specifies the sensor ID for the gyroscope subsensor.
    */
  uint8_t gyro_id;

  /**
    * Specifies the sensor ID for the ispu subsensor.
    */
  uint8_t ispu_id;

  /**
    * Specifies ispu status.
    */
  boolean_t ispu_enable;

  /**
    * Synchronization object used to send command to the task.
    */
  TX_QUEUE in_queue;

  /**
    * Buffer to store the data read from the accelerometer
    */
  uint8_t p_acc_sample[6];

  /**
    * Buffer to store the data read from the gyroscope
    */
  uint8_t p_gyro_sample[6];

  /**
    * Buffer to store the data from ispu
    */
  uint16_t p_ispu_output_buff[32];

  /**
    * Specifies the FIFO watermark level (it depends from odr)
    */
  uint16_t samples_per_it;

  /**
    * If both subsensors are active, specifies the amount of ACC samples in the FIFO
    */
  uint16_t acc_samples_count;

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
  IEventSrc *p_gyro_event_src;

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_ispu_event_src;

  /**
    * Software timer used to generate the read command
    */
  TX_TIMER read_timer;

  /**
    * Timer period used to schedule the read command
    */
  ULONG ism330is_task_cfg_timer_period_ms;

  /**
    * Software timer used to generate the ispu read command
    */
  TX_TIMER ispu_timer;

  /**
    * Used to update the instantaneous odr.
    */
  double prev_timestamp;

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
ISourceObservable *ISM330ISTaskGetAccSensorIF(ISM330ISTask *_this);

/**
  * Get the ISourceObservable interface for the gyroscope.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *ISM330ISTaskGetGyroSensorIF(ISM330ISTask *_this);

/**
  * Get the ISourceObservable fake interface for ispu.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *ISM330ISTaskGetIspuSensorIF(ISM330ISTask *_this);

/**
  * Get the ::ISensorLL_t interface the sensor.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISensorLL_t
  */
ISensorLL_t *ISM330ISTaskGetSensorLLIF(ISM330ISTask *_this);

/**
  * Allocate an instance of ISM330ISTask in the system heap.
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM330IS sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pISPUConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM330IS ISPU and configured in EXTI mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *ISM330ISTaskAlloc(const void *pIRQConfig, const void *pISPUConfig, const void *pCSConfig);

/**
  * Call the default ::ISM330ISTaskAlloc and then it overwrite sensor name
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM330IS sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pISPUConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM330IS ISPU and configured in EXTI mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *ISM330ISTaskAllocSetName(const void *pIRQConfig, const void *pISPUConfig, const void *pCSConfig,
                                         const char *p_name);

/**
  * Allocate an instance of ::ISM330ISTask in a memory block specified by the application.
  * The size of the memory block must be greater or equal to `sizeof(ISM330ISTask)`.
  * This allocator allows the application to avoid the dynamic allocation.
  *
  * \code
  * ISM330ISTask sensor_task;
  * ISM330ISTaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(ISM330ISTask)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM330IS sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pISPUConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM330IS ISPU and configured in EXTI mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *ISM330ISTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pISPUConfig,
                                        const void *pCSConfig);

/**
  * Call the default ::ISM330ISTaskAlloc and then it overwrite sensor name
  *
  * \code
  * ISM330ISTask sensor_task;
  * ISM330ISTaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(ISM330ISTask)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM330IS sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pISPUConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the ISM330IS ISPU and configured in EXTI mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *ISM330ISTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pISPUConfig,
                                               const void *pCSConfig,
                                               const char *p_name);

/**
  * Get the Bus interface for the sensor task.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the Bus interface of the sensor.
  */
ABusIF *ISM330ISTaskGetSensorIF(ISM330ISTask *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *ISM330ISTaskGetAccEventSrcIF(ISM330ISTask *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *ISM330ISTaskGetGyroEventSrcIF(ISM330ISTask *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *ISM330ISTaskGetMlcEventSrcIF(ISM330ISTask *_this);

/**
  * Interrupt callback
  */
void ISM330ISTask_EXTI_Callback(uint16_t Pin);

/**
  * Interrupt callback
  */
void INT2_ISM330IS_EXTI_Callback(uint16_t Pin);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* ISM330ISTASK_H_ */
