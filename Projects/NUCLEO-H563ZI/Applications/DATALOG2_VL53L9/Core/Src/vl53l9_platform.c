/**
  ******************************************************************************
  * @file    vl53l9_platform.c
  * @author  SRA
  * @brief   VL53L9 platform adapter for SensorManager callback-based bus IF.
  *
  *          This file implements the vl53l9_platform.h API by routing all
  *          register accesses through the SensorManager ABusIF callbacks stored
  *          in the vl53l9_device_t struct (SensorManager interface.h version).
  *
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

#include "vl53l9_platform.h"
#include "interface.h"
#include <string.h>

int vl53l9_read(void *const p_dev, uint16_t address, uint8_t *p_values, uint32_t size)
{
  vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;
  return (int)p_device->read_reg(p_device->bus_handle, address, p_values, (uint16_t)size);
}

int vl53l9_read_async(void *const p_dev, uint16_t address, volatile uint8_t *p_values, uint32_t size)
{
  /* Fall back to synchronous read; no DMA path available via callback IF. */
  return vl53l9_read(p_dev, address, (uint8_t *)p_values, size);
}

int vl53l9_read8(void *const p_dev, uint16_t address, uint8_t *p_value)
{
  return vl53l9_read(p_dev, address, p_value, 1U);
}

int vl53l9_read16(void *const p_dev, uint16_t address, uint16_t *p_value)
{
  uint8_t buf[2];
  int ret = vl53l9_read(p_dev, address, buf, 2U);
  /* VL53L9 register payloads are little-endian (LSB first). */
  *p_value = (uint16_t)((buf[0] << 0) | (buf[1] << 8));
  return ret;
}

int vl53l9_read32(void *const p_dev, uint16_t address, uint32_t *p_value)
{
  uint8_t buf[4];
  int ret = vl53l9_read(p_dev, address, buf, 4U);
  /* VL53L9 register payloads are little-endian (LSB first). */
  *p_value = ((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) | ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24);
  return ret;
}

int vl53l9_write(void *const p_dev, uint16_t address, uint8_t *p_values, uint32_t size)
{
  vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;
  return (int)p_device->write_reg(p_device->bus_handle, address, p_values, (uint16_t)size);
}

int vl53l9_write8(void *const p_dev, uint16_t address, uint8_t value)
{
  uint8_t buf = value;
  return vl53l9_write(p_dev, address, &buf, 1U);
}

int vl53l9_write16(void *const p_dev, uint16_t address, uint16_t value)
{
  /* VL53L9 register payloads are little-endian (LSB first). */
  uint8_t buf[2] = { (uint8_t)(value & 0xFFU), (uint8_t)(value >> 8) };
  return vl53l9_write(p_dev, address, buf, 2U);
}

int vl53l9_write32(void *const p_dev, uint16_t address, uint32_t value)
{
  /* VL53L9 register payloads are little-endian (LSB first). */
  uint8_t buf[4] =
  {
    (uint8_t)(value & 0xFFU),
    (uint8_t)((value >> 8) & 0xFFU),
    (uint8_t)((value >> 16) & 0xFFU),
    (uint8_t)(value >> 24)
  };
  return vl53l9_write(p_dev, address, buf, 4U);
}

int vl53l9_wait_ms(void *const p_dev, uint32_t delay_ms)
{
  vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;
  p_device->delay_ms(delay_ms);
  return 0;
}

int vl53l9_get_config_vddio(void *const p_dev, vl53l9_vddio_t *voltage)
{
  vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;
  *voltage = p_device->vddio;
  return 0;
}

int vl53l9_get_config_vdda(void *const p_dev, vl53l9_vdda_t *voltage)
{
  vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;
  *voltage = p_device->vdda;
  return 0;
}

int vl53l9_get_config_ext_clock(void *const p_dev, uint32_t *ext_clock)
{
  vl53l9_device_t *p_device = (vl53l9_device_t *)p_dev;
  *ext_clock = p_device->ext_clock;
  return 0;
}
