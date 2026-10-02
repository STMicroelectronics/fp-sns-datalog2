/**
  ******************************************************************************
  * @file    interface.h
  * @author  SRA - MCD
  * @brief   VL53L9 platform adapter interface for SensorManager integration.
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
#ifndef VL53L9_INTERFACE_H_
#define VL53L9_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "vl53l9.h"
#include <stdint.h>

#define PLATFORM_BUS_PROPERTY_NONE       (0x00U)
#define PLATFORM_BUS_PROPERTY_I3C_LEGACY (0x01U)

typedef int32_t (*vl53l9_bus_write_f)(void *p_handle, uint16_t reg, uint8_t *p_data, uint16_t size);
typedef int32_t (*vl53l9_bus_read_f)(void *p_handle, uint16_t reg, uint8_t *p_data, uint16_t size);
typedef void (*vl53l9_delay_f)(uint32_t delay_ms);

typedef struct _vl53l9_device_t
{
  void *bus_handle;
  vl53l9_bus_write_f write_reg;
  vl53l9_bus_read_f read_reg;
  vl53l9_delay_f delay_ms;
  uint8_t address;
  uint8_t bus_property;
  vl53l9_vddio_t vddio;
  vl53l9_vdda_t vdda;
  uint32_t ext_clock;
} vl53l9_device_t;

#ifdef __cplusplus
}
#endif

#endif /* VL53L9_INTERFACE_H_ */
