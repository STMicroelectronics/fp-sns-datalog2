/**
  ******************************************************************************
  * @file    IIS3DWB10ISTask.h
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

#ifndef IIS3DWB10ISTASK_H_
#define IIS3DWB10ISTASK_H_

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

#define IIS3DWB10IS_MAX_DRDY_PERIOD           (1.0) /* seconds */

#ifndef IIS3DWB10IS_MAX_WTM_LEVEL
#define IIS3DWB10IS_MAX_WTM_LEVEL             (1024) /* samples */
#endif

#define IIS3DWB10IS_MAX_SAMPLES_PER_IT        (IIS3DWB10IS_MAX_WTM_LEVEL)


#define IIS3DWB10IS_CFG_MAX_LISTENERS           2

/**
  * Create  type name for _IIS3DWB10ISTask.
  */
typedef struct _IIS3DWB10ISTask IIS3DWB10ISTask;

/**
  *  IIS3DWB10ISTask internal structure.
  */
struct _IIS3DWB10ISTask
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

#if IIS3DWB10IS_FIFO_ENABLED

  /**
    * Buffer to store the data read from the sensor FIFO.
    */
  uint8_t p_sensor_data_buff[IIS3DWB10IS_MAX_SAMPLES_PER_IT * (1 + 9)];

  /**
    * Buffer to store the data from the low g accelerometer subsensor
    */
  uint8_t p_acc_sensor_data_buff[IIS3DWB10IS_MAX_SAMPLES_PER_IT * 3 * 4];

#else

  /**
    * Buffer to store the data read from the sensor FIFO.
    * It is reused also to save data from the faster subsensor
    */
  uint8_t p_acc_sample[3 * 4];

  /**
    * Save acc data ready status
   */
  uint8_t acc_drdy;

#endif

  /**
    * Buffer to store the data from ispu
    */
  uint16_t p_ispu_output_buff[32];

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
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_acc_event_src;

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
  ULONG iis3dwb10is_task_cfg_timer_period_ms;

  /**
    * Software timer used to generate the ispu read command
    */
  TX_TIMER ispu_timer;

  /**
    * Used to update the instantaneous odr.
    */
  double_t acc_prev_timestamp;

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
  * Get the ISourceObservable interface for the accelerometer.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *IIS3DWB10ISTaskGetAccSensorIF(IIS3DWB10ISTask *_this);

/**
  * Get the ISourceObservable fake interface for ispu.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *IIS3DWB10ISTaskGetIspuSensorIF(IIS3DWB10ISTask *_this);

/**
  * Get the ::ISensorLL_t interface the sensor.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISensorLL_t
  */
ISensorLL_t *IIS3DWB10ISTaskGetSensorLLIF(IIS3DWB10ISTask *_this);

/**
  * Allocate an instance of IIS3DWB10ISTask in the system heap.
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the IIS3DWB10IS sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pISPUConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the IIS3DWB10IS ISPU and configured in EXTI mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IIS3DWB10ISTaskAlloc(const void *pIRQConfig, const void *pISPUConfig, const void *pCSConfig);

/**
  * Call the default ::IIS3DWB10ISTaskAlloc and then it overwrite sensor name
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the IIS3DWB10IS sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pISPUConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the IIS3DWB10IS ISPU and configured in EXTI mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IIS3DWB10ISTaskAllocSetName(const void *pIRQConfig, const void *pISPUConfig, const void *pCSConfig,
                                            const char *p_name);

/**
  * Allocate an instance of ::IIS3DWB10ISTask in a memory block specified by the application.
  * The size of the memory block must be greater or equal to `sizeof(IIS3DWB10ISTask)`.
  * This allocator allows the application to avoid the dynamic allocation.
  *
  * \code
  * IIS3DWB10ISTask sensor_task;
  * IIS3DWB10ISTaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(IIS3DWB10ISTask)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the IIS3DWB10IS sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pISPUConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the IIS3DWB10IS ISPU and configured in EXTI mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IIS3DWB10ISTaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pISPUConfig,
                                           const void *pCSConfig);

/**
  * Call the default ::IIS3DWB10ISTaskAlloc and then it overwrite sensor name
  *
  * \code
  * IIS3DWB10ISTask sensor_task;
  * IIS3DWB10ISTaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(IIS3DWB10ISTask)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the IIS3DWB10IS sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pISPUConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the IIS3DWB10IS ISPU and configured in EXTI mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IIS3DWB10ISTaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pISPUConfig,
                                                  const void *pCSConfig,
                                                  const char *p_name);

/**
  * Get the SPI interface for the sensor task.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the SPI interface of the sensor.
  */
ABusIF *IIS3DWB10ISTaskGetSensorIF(IIS3DWB10ISTask *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *IIS3DWB10ISTaskGetAccEventSrcIF(IIS3DWB10ISTask *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *IIS3DWB10ISTaskGetIspuEventSrcIF(IIS3DWB10ISTask *_this);

/**
  * Interrupt callback
  */
void IIS3DWB10ISTask_EXTI_Callback(uint16_t Pin);

/**
  * Interrupt callback
  */
void INT2_IIS3DWB10IS_EXTI_Callback(uint16_t Pin);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* IIS3DWB10ISTASK_H_ */
