/**
  ******************************************************************************
  * @file    MP23DB01HPTask.h
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
#ifndef MP23DB01HPTASK_H_
#define MP23DB01HPTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "services/AManagedTaskEx.h"
#include "services/AManagedTaskEx_vtbl.h"
#include "events/DataEventSrc.h"
#include "events/DataEventSrc_vtbl.h"
#include "SensorManager.h"
#include "drivers/MDFDriver.h"
#include "drivers/MDFDriver_vtbl.h"
#include "mx.h"

#ifndef MAX_DMIC_SAMPLING_FREQUENCY
#define MAX_DMIC_SAMPLING_FREQUENCY              (uint32_t)(48000)
#endif

/**
  * Create  type name for _MP23DB01HPTask.
  */
typedef struct _MP23DB01HPTask MP23DB01HPTask;

/**
  *  MP23DB01HPTask internal structure.
  */
struct _MP23DB01HPTask
{
  /**
    * Base class object.
    */
  AManagedTaskEx super;

  /**
    * Driver object.
    */
  IDriver *p_driver;

  /**
    * HAL MDF driver configuration parameters.
    */
  const void *p_mx_mdf_cfg;

  /**
    * Implements the mic ISensorAudio interface.
    */
  ISensorAudio_t sensor_if;

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
    * Specifies the sensor ID for the microphone subsensor.
    */
  uint8_t mic_id;

  /**
    * Synchronization object used to send command to the task.
    */
  TX_QUEUE in_queue;

  /**
    * ::IEventSrc interface implementation for this class.
    */
  IEventSrc *p_event_src;

  /**
    * Buffer to store the data read from the sensor
    */
  int16_t p_sensor_data_buff[((MAX_DMIC_SAMPLING_FREQUENCY / 1000) * 2)];

#if (HSD_USE_DUMMY_DATA == 1)
  /**
    * Buffer to store dummy data buffer
    */
  int16_t p_dummy_data_buff[((MAX_DMIC_SAMPLING_FREQUENCY / 1000))];
#endif

  /*
    * Calibration values, used for adjusting audio gain
    */
  int old_in;
  int old_out;

  /**
    * Used to update the instantaneous ODR.
    */
  double prev_timestamp;

  uint8_t half;

};

// Public API declaration
//***********************

/**
  * Get the ISourceObservable interface for the analog microphone.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the generic object ::ISourceObservable if success,
  * or NULL if out of memory error occurs.
  */
ISourceObservable *MP23DB01HPTaskGetMicSensorIF(MP23DB01HPTask *_this);

/**
  * Allocate an instance of MP23DB01HPTaskin the system heap.
  *
  * @param p_mx_mdf_cfg [IN] specifies a ::MX_MDFParams_t instance declared in the mx.h file.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *MP23DB01HPTaskAlloc(const void *p_mx_mdf_cfg);

/**
  * Call the default ::MP23DB01HPTaskAlloc and then it overwrite sensor name
  *
  * @param p_mx_mdf_cfg [IN] specifies a ::MX_MDFParams_t instance declared in the mx.h file.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *MP23DB01HPTaskAllocSetName(const void *p_mx_mdf_cfg, const char *p_name);

/**
  * Allocate an instance of ::MP23DB01HPTask in a memory block specified by the application.
  * The size of the memory block must be greater or equal to `sizeof(MP23DB01HPTask)`.
  * This allocator allows the application to avoid the dynamic allocation.
  *
  * \code
  * MP23DB01HPTask sensor_task;
  * MP23DB01HPTaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(MP23DB01HPTask)`.
  * @param p_mx_mdf_cfg [IN] specifies a ::MX_MDFParams_t instance declared in the mx.h file.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *MP23DB01HPTaskStaticAlloc(void *p_mem_block, const void *p_mx_mdf_cfg);

/**
  * Call the default ::MP23DB01HPTaskAlloc and then it overwrite sensor name
  *
  * \code
  * MP23DB01HPTask sensor_task;
  * MP23DB01HPTaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(MP23DB01HPTask)`.
  * @param p_mx_mdf_cfg [IN] specifies a ::MX_MDFParams_t instance declared in the mx.h file.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *MP23DB01HPTaskStaticAllocSetName(void *p_mem_block, const void *p_mx_mdf_cfg, const char *p_name);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *MP23DB01HPTaskGetEventSrcIF(MP23DB01HPTask *_this);

/**
  * STM32 HAL callback function.
  * @param hmdf [IN] specifies a MDF instance.
  */
void MDF_Filter_0_Complete_Callback(MDF_HandleTypeDef *hmdf);

/**
  * STM32 HAL callback function.
  * @param hmdf  [IN] specifies a MDF instance.
  */
void MDF_Filter_0_HalfComplete_Callback(MDF_HandleTypeDef *hmdf);

// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* MP23DB01HPTASK_H_ */
