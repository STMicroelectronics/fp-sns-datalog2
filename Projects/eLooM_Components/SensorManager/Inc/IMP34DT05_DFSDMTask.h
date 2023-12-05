/**
  ******************************************************************************
  * @file    IMP34DT05Task.h
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
#ifndef IMP34DT05_DFSDMTASK_H_
#define IMP34DT05_DFSDMTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "services/AManagedTaskEx.h"
#include "services/AManagedTaskEx_vtbl.h"
#include "events/DataEventSrc.h"
#include "events/DataEventSrc_vtbl.h"
#include "SensorManager.h"
#include "drivers/DFSDMDriver.h"
#include "drivers/DFSDMDriver_vtbl.h"
#include "mx.h"

#ifndef MAX_DMIC_SAMPLING_FREQUENCY
#define MAX_DMIC_SAMPLING_FREQUENCY              (uint32_t)(48000)
#endif

/**
  * Create  type name for _IMP34DT05Task.
  */
typedef struct _IMP34DT05Task IMP34DT05Task;

/**
  *  IMP34DT05Task internal structure.
  */
struct _IMP34DT05Task
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
    * HAL DFSDM driver configuration parameters.
    */
  const void *p_mx_dfsdm_cfg;

  /**
    * Implements the mic ISensor interface.
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
    * Buffer used by DMA
    */
  int32_t p_dma_data_buff[((MAX_DMIC_SAMPLING_FREQUENCY / 1000) * 2)];

  /**
    * Buffer to store the data read from the sensor
    */
  int16_t p_sensor_data_buff[((MAX_DMIC_SAMPLING_FREQUENCY / 1000))];

#if (HSD_USE_DUMMY_DATA == 1)
  /**
    * Buffer to store dummy data buffer
    */
  int16_t p_dummy_data_buff[((MAX_DMIC_SAMPLING_FREQUENCY / 1000))];
#endif

  /*
    * Calibration values, used for adjusting audio gain
    */
  int32_t old_in;
  int32_t old_out;

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
ISourceObservable *IMP34DT05TaskGetMicSensorIF(IMP34DT05Task *_this);

/**
  * Allocate an instance of IMP34DT05Taskin the system heap.
  *
  * @param p_mx_dfsdm_cfg [IN] specifies a ::MX_DFSDMParams_t instance declared in the mx.h file.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IMP34DT05TaskAlloc(const void *p_mx_dfsdm_cfg);

/**
  * Call the default ::IMP34DT05TaskAlloc and then it overwrite sensor name
  *
  * @param p_mx_dfsdm_cfg [IN] specifies a ::MX_DFSDMParams_t instance declared in the mx.h file.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IMP34DT05TaskAllocSetName(const void *p_mx_dfsdm_cfg, const char *p_name);

/**
  * Allocate an instance of ::IMP34DT05Task in a memory block specified by the application.
  * The size of the memory block must be greater or equal to `sizeof(IMP34DT05Task)`.
  * This allocator allows the application to avoid the dynamic allocation.
  *
  * \code
  * IMP34DT05Task sensor_task;
  * IMP34DT05TaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(IMP34DT05Task)`.
  * @param p_mx_dfsdm_cfg [IN] specifies a ::MX_DFSDMParams_t instance declared in the mx.h file.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IMP34DT05TaskStaticAlloc(void *p_mem_block, const void *p_mx_dfsdm_cfg);

/**
  * Call the default ::IMP34DT05TaskAlloc and then it overwrite sensor name
  *
  * \code
  * IMP34DT05Task sensor_task;
  * IMP34DT05TaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(IMP34DT05Task)`.
  * @param p_mx_dfsdm_cfg [IN] specifies a ::MX_DFSDMParams_t instance declared in the mx.h file.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IMP34DT05TaskStaticAllocSetName(void *p_mem_block, const void *p_mx_dfsdm_cfg, const char *p_name);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *IMP34DT05TaskGetEventSrcIF(IMP34DT05Task *_this);

/**
  * STM32 HAL callback function.
  * @param hdfsdm_filter [IN] specifies a DFSDM instance.
  */
void DFSDM_Filter_0_Complete_Callback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);

/**
  * STM32 HAL callback function.
  * @param hdfsdm_filter [IN] specifies a DFSDM instance.
  */
void DFSDM_Filter_0_HalfComplete_Callback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);

// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* IMP34DT05_DFSDMTASK_H_ */
