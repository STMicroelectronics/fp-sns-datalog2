/**
  ******************************************************************************
  * @file    STHS34PF80Task.h
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
#ifndef STHS34PF80TASK_H_
#define STHS34PF80TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/AManagedTaskEx.h"
#include "services/AManagedTaskEx_vtbl.h"
#include "ABusIF.h"
#include "events/DataEventSrc.h"
#include "events/DataEventSrc_vtbl.h"
#include "ISensorPresence.h"
#include "ISensorPresence_vtbl.h"
#include "mx.h"
#include "crc.h"
#include "sths34pf80_reg.h"
#include "infrared_pd.h"


/**
  * Create  type name for _STHS34PF80Task.
  */
typedef struct _STHS34PF80Task STHS34PF80Task;


/**
  *  STHS34PF80Task internal structure.
  */
struct _STHS34PF80Task
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
    * I2C BS GPIO configuration parameters.
    */
  const MX_GPIOParams_t *pBSConfig;

  /**
    * Bus IF object used to connect the sensor task to the specific bus.
    */
  ABusIF *p_sensor_bus_if;

  /**
    * Software Compensation Library Algorithm Data Structure.
    */
  IPD_input_t *tmos_swlib;

  /**
    * Software Compensation Library Algorithm instance.
    */
  IPD_Instance_t *tmos_swlib_instance;

  /**
    * Implements the  ISensor interface.
    */
  ISensorPresence_t sensor_if;

  /**
    * Specifies sensor capabilities.
    */
  const SensorDescriptor_t *sensor_descriptor;

  /**
    * Specifies sensor configuration.
    */
  SensorStatus_t sensor_status;

  EMData_t data;
  /**
    * Specifies the sensor ID for the tmos sensor.
    */
  uint8_t id;

  /**
    * Synchronization object used to send command to the task.
    */
  TX_QUEUE in_queue;

  /**
    * Buffer to store the data read from the sensor
    */
  int16_t p_sensor_data_buff[11];

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
  ULONG sths34pf80_task_cfg_timer_period_ms;

  /**
    * Used to update the instantaneous ODR.
    */
  double prev_timestamp;

  /**
    * !< Interrupt: 0, Polling: 1
    */
  uint8_t IsBlocking;

  /**
    * !< One shot: 0, Continuous: 1
    */
  uint8_t IsContinuous;

};
// Public API declaration
//***********************

/**
  * Get the ISourceObservable interface for the accelerometer.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *STHS34PF80TaskGetTofSensorIF(STHS34PF80Task *_this);

/**
  * Allocate an instance.
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the STHS34PF80 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @param pBSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the I2C BS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *STHS34PF80TaskAlloc(const void *pIRQConfig, const void *pCSConfig, const void *pBSConfig);

/**
  * Call the default ::STHS34PF80TaskAlloc and then it overwrite sensor name
  *
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the STHS34PF80 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @param pBSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the I2C BS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *STHS34PF80TaskAllocSetName(const void *pIRQConfig, const void *pCSConfig, const void *pBSConfig,
                                           const char *p_name);

/**
  * Allocate an instance of ::STHS34PF80Task in a memory block specified by the application.
  * The size of the memory block must be greater or equal to `sizeof(STHS34PF80Task)`.
  * This allocator allows the application to avoid the dynamic allocation.
  *
  * \code
  * STHS34PF80Task sensor_task;
  * STHS34PF80TaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(STHS34PF80Task)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the STHS34PF80 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @param pBSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the I2C BS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *STHS34PF80TaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                          const void *pBSConfig);

/**
  * Call the default ::STHS34PF80TaskAlloc and then it overwrite sensor name
  *
  * \code
  * STHS34PF80Task sensor_task;
  * STHS34PF80TaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(STHS34PF80Task)`.
  * @param pIRQConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO connected to the STHS34PF80 sensor and configured in EXTI mode.
  *        If it is NULL then the sensor is configured in polling mode.
  * @param pCSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the SPI CS Pin.
  * @param pBSConfig [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        It must be a GPIO identifying the I2C BS Pin.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *STHS34PF80TaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pCSConfig,
                                                 const void *pBSConfig,
                                                 const char *p_name);

/**
  * Get the Bus interface for the sensor task.
  *
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the Bus interface of the sensor.
  */
ABusIF *STHS34PF80TaskGetSensorIF(STHS34PF80Task *_this);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *STHS34PF80TaskGetEventSrcIF(STHS34PF80Task *_this);

/**
  * IRQ callback
  */
void STHS34PF80Task_EXTI_Callback(uint16_t nPin);

// Inline functions definition
// ***************************

#ifdef __cplusplus
}
#endif

#endif /* STHS34PF80TASK_H_ */
