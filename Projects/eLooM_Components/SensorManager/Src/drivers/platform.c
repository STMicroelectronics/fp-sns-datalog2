/**
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "drivers/platform.h"
#include "STALS_platform.h"
#include "vd6283tx.h"

uint8_t VL53L8CX_RdByte(
  VL53L8CX_Platform *p_platform,
  uint16_t RegisterAdress,
  uint8_t *p_value)
{
  return p_platform->Read(p_platform->handle, RegisterAdress, p_value, 1U);
}

uint8_t VL53L8CX_WrByte(
  VL53L8CX_Platform *p_platform,
  uint16_t RegisterAdress,
  uint8_t value)
{
  return p_platform->Write(p_platform->handle, RegisterAdress, &value, 1U);
}

uint8_t VL53L8CX_WrMulti(
  VL53L8CX_Platform *p_platform,
  uint16_t RegisterAdress,
  uint8_t *p_values,
  uint32_t size)
{
  return p_platform->Write(p_platform->handle, RegisterAdress, p_values, size);
}

uint8_t VL53L8CX_RdMulti(
  VL53L8CX_Platform *p_platform,
  uint16_t RegisterAdress,
  uint8_t *p_values,
  uint32_t size)
{
  return p_platform->Read(p_platform->handle, RegisterAdress, p_values, size);
}

void VL53L8CX_SwapBuffer(
  uint8_t     *buffer,
  uint16_t     size)
{
  uint32_t i, tmp;

  /* Example of possible implementation using <string.h> */
  for (i = 0; i < size; i = i + 4)
  {
    tmp = (
            buffer[i] << 24)
          | (buffer[i + 1] << 16)
          | (buffer[i + 2] << 8)
          | (buffer[i + 3]);

    memcpy(&(buffer[i]), &tmp, 4);
  }
}

uint8_t VL53L8CX_WaitMs(
  VL53L8CX_Platform *p_platform,
  uint32_t TimeMs)
{
  tx_thread_sleep(TimeMs);
  return 0;
}

uint8_t RdByte(
  VL53L8CX_Platform *p_platform,
  uint16_t RegisterAdress,
  uint8_t *p_value)
{
  return p_platform->Read(p_platform->handle, RegisterAdress, p_value, 1U);
}

uint8_t WrByte(
  VL53L8CX_Platform *p_platform,
  uint16_t RegisterAdress,
  uint8_t value)
{
  return p_platform->Write(p_platform->handle, RegisterAdress, &value, 1U);
}

STALS_ErrCode_t STALS_WrByte(void *pClient, uint8_t index, uint8_t data)
{
  int32_t res;
  VD6283TX_Object_t *pObj = (VD6283TX_Object_t *) pClient;

  res = pObj->IO.write_reg(pObj->IO.handle, index, &data, 1);

  if (res)
  {
    return STALS_ERROR_WRITE;
  }

  return STALS_NO_ERROR;
}

STALS_ErrCode_t STALS_RdByte(void *pClient, uint8_t index, uint8_t *data)
{
  int32_t res;
  VD6283TX_Object_t *pObj = (VD6283TX_Object_t *) pClient;

  res = pObj->IO.read_reg(pObj->IO.handle, index, data, 1);

  if (res)
  {
    return STALS_ERROR_READ;
  }

  return STALS_NO_ERROR;
}

STALS_ErrCode_t STALS_RdMultipleBytes(void *pClient, uint8_t index, uint8_t *data, int nb)
{
  int32_t res;
  VD6283TX_Object_t *pObj = (VD6283TX_Object_t *) pClient;

  res = pObj->IO.read_reg(pObj->IO.handle, index, data, nb);

  if (res)
  {
    return STALS_ERROR_READ;
  }

  return STALS_NO_ERROR;
}

