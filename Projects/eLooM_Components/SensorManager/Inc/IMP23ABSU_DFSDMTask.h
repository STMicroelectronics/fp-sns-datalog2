/**
  ******************************************************************************
  * @file    IMP23ABSUTask.h
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
#ifndef IMP23ABSU_DFSDMTASK_H_
#define IMP23ABSU_DFSDMTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "services/AManagedTaskEx.h"
#include "services/AManagedTaskEx_vtbl.h"
#include "events/DataEventSrc.h"
#include "events/DataEventSrc_vtbl.h"
#include "SensorManager.h"
#include "drivers/AnalogDFSDMMicDriver.h"
#include "drivers/AnalogDFSDMMicDriver_vtbl.h"
#include "mx.h"

#ifndef MAX_AMIC_SAMPLING_FREQUENCY
#define MAX_AMIC_SAMPLING_FREQUENCY             (uint32_t)(192000)
#endif

/**
  * Create  type name for _IMP23ABSUTask.
  */
typedef struct _IMP23ABSUTask IMP23ABSUTask;

/**
  *  IMP23ABSUTask internal structure.
  */
struct _IMP23ABSUTask
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
    *
    */
  const void *p_mx_adc_cfg;

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
  int32_t p_dma_data_buff[((MAX_AMIC_SAMPLING_FREQUENCY / 1000) * 2)];

  /**
    * Buffer to store the data read from the sensor
    */
  int16_t p_sensor_data_buff[((MAX_AMIC_SAMPLING_FREQUENCY / 1000))];

#if (HSD_USE_DUMMY_DATA == 1)
  /**
    * Buffer to store dummy data buffer
    */
  int16_t p_dummy_data_buff[((MAX_AMIC_SAMPLING_FREQUENCY / 1000))];
#endif

  /*
    * Calibration values, used for adjusting audio gain
    */
  int32_t old_in;
  int32_t old_out;

  /**
    * Used to update the instantaneous ODR.
    */
  double_t prev_timestamp;

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
ISourceObservable *IMP23ABSUTaskGetMicSensorIF(IMP23ABSUTask *_this);

/**
  * Allocate an instance of IMP23ABSUTaskin the system heap.
  *
  * @param p_mx_dfsdm_cfg [IN] specifies a ::MX_DFSDMParams_t instance declared in the mx.h file.
  * @param p_mx_adc_cfg [IN] specifies a ::MX_ADCParams_t instance declared in the mx.h file.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IMP23ABSUTaskAlloc(const void *p_mx_dfsdm_cfg, const void *p_mx_adc_cfg);

/**
  * Call the default ::IMP23ABSUTaskAlloc and then it overwrite sensor name
  *
  * @param p_mx_dfsdm_cfg [IN] specifies a ::MX_DFSDMParams_t instance declared in the mx.h file.
  * @param p_mx_adc_cfg [IN] specifies a ::MX_ADCParams_t instance declared in the mx.h file.
  * @return a pointer to the generic object ::AManagedTaskEx if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IMP23ABSUTaskAllocSetName(const void *p_mx_dfsdm_cfg, const void *p_mx_adc_cfg, const char *p_name);

/**
  * Allocate an instance of ::IMP23ABSUTask in a memory block specified by the application.
  * The size of the memory block must be greater or equal to `sizeof(IMP23ABSUTask)`.
  * This allocator allows the application to avoid the dynamic allocation.
  *
  * \code
  * IMP23ABSUTask sensor_task;
  * IMP23ABSUTaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(IMP23ABSUTask)`.
  * @param p_mx_dfsdm_cfg [IN] specifies a ::MX_DFSDMParams_t instance declared in the mx.h file.
  * @param p_mx_adc_cfg [IN] specifies a ::MX_ADCParams_t instance declared in the mx.h file.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IMP23ABSUTaskStaticAlloc(void *p_mem_block, const void *p_mx_dfsdm_cfg, const void *p_mx_adc_cfg);

/**
  * Call the default ::IMP23ABSUTaskAlloc and then it overwrite sensor name
  *
  * \code
  * IMP23ABSUTask sensor_task;
  * IMP23ABSUTaskStaticAlloc(&sensor_task);
  * \endcode
  *
  * @param p_mem_block [IN] specify a memory block allocated by the application.
  *        The size of the memory block must be greater or equal to `sizeof(IMP23ABSUTask)`.
  * @param p_mx_dfsdm_cfg [IN] specifies a ::MX_DFSDMParams_t instance declared in the mx.h file.
  * @param p_mx_adc_cfg [IN] specifies a ::MX_ADCParams_t instance declared in the mx.h file.
  * @return a pointer to the generic object ::AManagedTaskEx_t if success,
  * or NULL if out of memory error occurs.
  */
AManagedTaskEx *IMP23ABSUTaskStaticAllocSetName(void *p_mem_block, const void *p_mx_dfsdm_cfg, const void *p_mx_adc_cfg,
                                                const char *p_name);

/**
  * Get the ::IEventSrc interface for the sensor task.
  * @param _this [IN] specifies a pointer to a task object.
  * @return a pointer to the ::IEventSrc interface of the sensor.
  */
IEventSrc *IMP23ABSUTaskGetEventSrcIF(IMP23ABSUTask *_this);

/**
  * STM32 HAL callback function.
  * @param hdfsdm_filter [IN] specifies a DFSDM instance.
  */
void DFSDM_Filter_1_Complete_Callback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);

/**
  * STM32 HAL callback function.
  * @param hdfsdm_filter [IN] specifies a DFSDM instance.
  */
void DFSDM_Filter_1_HalfComplete_Callback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter);

// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* IMP23ABSU_DFSDMTASK_H_ */
