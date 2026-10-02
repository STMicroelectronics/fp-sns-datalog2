/**
  ******************************************************************************
  * @file    VL53L9Task.h
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
  ******************************************************************************
  */
#ifndef VL53L9TASK_H_
#define VL53L9TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/AManagedTaskEx.h"
#include "services/AManagedTaskEx_vtbl.h"
#include "ABusIF.h"
#include "events/DataEventSrc.h"
#include "events/DataEventSrc_vtbl.h"
#include "ISensorRanging.h"
#include "ISensorRanging_vtbl.h"
#include "mx.h"
#include "vl53l9.h"
#include "interface.h"


#define VL53L9_TASK_CFG_STATIC_ADDRESS           (VL53L9_DEFAULT_ADDRESS >> 1)
#define VL53L9_RESOLUTION_4X4                    (16u)
#define VL53L9_RESOLUTION_8X8                    (64u)
#define VL53L9_RESOLUTION_12X10                  (120u)
#define VL53L9_RESOLUTION_18X14                  (252u)
#define VL53L9_RESOLUTION_24X24                  (576u)
#define VL53L9_RESOLUTION_54X42                  (2268u)
#define VL53L9_TARGET_STATUS_VALID               (5u)
#define VL53L9_TARGET_STATUS_EMPTY               (255u)

#ifndef VL53L9_DYNAMIC_ADDR
#define VL53L9_DYNAMIC_ADDR                      VL53L9_TASK_CFG_STATIC_ADDRESS
#endif

#ifdef TOF_EXTENDED
#define VL53L9_TASK_CFG_ZONE_CHANNELS            (4u)
#else
#define VL53L9_TASK_CFG_ZONE_CHANNELS            (1u)
#endif


typedef struct _VL53L9Task VL53L9Task;

struct _VL53L9Task
{
  AManagedTaskEx super;

  /**
    * IRQ GPIO configuration parameters.
    */
  const MX_GPIOParams_t *pIRQConfig;

  /*
    * xshut GPIO configuration parameters.
    */
  const MX_GPIOParams_t *pXSHUTConfig;

  /**
    * TIM configuration for CLK_IN output (12MHz PWM on TIM3_CH2).
    */
  const MX_TIMParams_t *pTIMConfig;

  ABusIF *p_sensor_bus_if;
  vl53l9_device_t tof_driver_if;
  ISensorRanging_t sensor_if;
  const SensorDescriptor_t *sensor_descriptor;
  SensorStatus_t sensor_status;
  EMData_t data;
  uint8_t id;
  TX_QUEUE in_queue;
  uint8_t *p_raw_frame;
  uint16_t raw_frame_size;
  uint8_t current_binning;
  uint16_t current_resolution;
  uint16_t sensor_data_capacity;
  IEventSrc *p_event_src;
  TX_TIMER read_timer;
  ULONG vl53l9_task_cfg_timer_period_ms;
  boolean_t frame_ready;
  /* Manual sync mode only: TRUE while a triggered frame has not been read back yet. */
  boolean_t frame_pending;
};

ISourceObservable *VL53L9TaskGetTofSensorIF(VL53L9Task *_this);
AManagedTaskEx *VL53L9TaskAlloc(const void *pIRQConfig, const void *pXSHUTConfig, const MX_TIMParams_t *pTIMConfig);
AManagedTaskEx *VL53L9TaskAllocSetName(const void *pIRQConfig, const void *pXSHUTConfig,
                                       const MX_TIMParams_t *pTIMConfig, const char *p_name);
AManagedTaskEx *VL53L9TaskStaticAlloc(void *p_mem_block, const void *pIRQConfig, const void *pXSHUTConfig,
                                      const MX_TIMParams_t *pTIMConfig);
AManagedTaskEx *VL53L9TaskStaticAllocSetName(void *p_mem_block, const void *pIRQConfig, const void *pXSHUTConfig,
                                             const MX_TIMParams_t *pTIMConfig, const char *p_name);
ABusIF *VL53L9TaskGetSensorIF(VL53L9Task *_this);
IEventSrc *VL53L9TaskGetEventSrcIF(VL53L9Task *_this);
void VL53L9Task_EXTI_Callback(uint16_t nPin);

#ifdef __cplusplus
}
#endif

#endif /* VL53L9TASK_H_ */
