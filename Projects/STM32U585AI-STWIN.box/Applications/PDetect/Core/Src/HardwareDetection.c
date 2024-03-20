/**
  ******************************************************************************
  * @file    HardwareDetection.c
  * @author  SRA
  * @brief
  *
  *
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

#include "mx.h"
#include "i2c.h"
#include "services/systypes.h"
#include "HardwareDetection.h"
#include "sths34pf80_reg.h"

#define HW_DETECTION_I2C_TIMEOUT  500U

#define ST25_ADDR_DATA_I2C                ((uint8_t)0xAE)
#define ST25_ICREF_REG                    ((uint16_t)0x0017)

#define ICREF_ST25DV04                    0x24U   /* ST25DV         4Kbits ICref */
#define ICREF_ST25DV64                    0x26U   /* ST25DV     16/64Kbits ICref */
#define ICREF_ST25DV04KC                  0x50U   /* ST25DVxxKC     4Kbits ICref */
#define ICREF_ST25DV64KC                  0x51U   /* ST25DVxxKC 16/64Kbits ICref */

extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c3;

static int32_t ext_sensor_i2c_read(void *handle, uint8_t reg, uint8_t *p_data, uint16_t size);
static int32_t ext_sensor_i2c_write(void *handle, uint8_t reg, uint8_t *p_data, uint16_t size);


/* Public functions declaration */
/*********************************/
/**
  * Detect an external PDETECT
  *
  * @param device_address: return the address of the device, if found. [output]
  * @return TRUE if the sensor was found, FALSE otherwise
  */
boolean_t HardwareDetection_Check_Ext_PDETECT(uint8_t *device_address)
{
  uint8_t whoami_val = 0U;
  boolean_t found = FALSE;
  uint8_t addr = STHS34PF80_I2C_ADD;
  stmdev_ctx_t ctx;

  ctx.read_reg = ext_sensor_i2c_read;
  ctx.write_reg = ext_sensor_i2c_write;
  ctx.handle = (void *)&addr;

  MX_I2C3_Init();

  HAL_Delay(100);
  sths34pf80_device_id_get(&ctx, (uint8_t *) &whoami_val);
  if (whoami_val == STHS34PF80_ID)
  {
    found = TRUE;
  }

  HAL_I2C_DeInit(&hi2c3);

  *device_address = addr;
  return found;
}

/**
  * Detect the version of ST25DV chip
  *
  * @return hwd_st25dv_version
  */
hwd_st25dv_version HardwareDetection_Check_ST25DV(void)
{
  hwd_st25dv_version ret;
  uint8_t icref;

  MX_I2C2_Init();

  HAL_I2C_Mem_Read(&hi2c2, ST25_ADDR_DATA_I2C, ST25_ICREF_REG, I2C_MEMADD_SIZE_16BIT, &icref, 1,
                   HW_DETECTION_I2C_TIMEOUT);

  if (icref == ICREF_ST25DV04)
  {
    ret = ST25DV04;
  }
  else if (icref == ICREF_ST25DV64)
  {
    ret = ST25DV64;
  }
  else if (icref == ICREF_ST25DV04KC)
  {
    ret = ST25DV04KC;
  }
  else if (icref == ICREF_ST25DV64KC)
  {
    ret = ST25DV64KC;
  }
  else
  {
    ret = ST25DV_ERROR;
  }

  HAL_I2C_DeInit(&hi2c2);

  return ret;
}

/* Private function definition */
/*******************************/
static int32_t ext_sensor_i2c_write(void *handle, uint8_t reg, uint8_t *p_data, uint16_t size)
{
  uint8_t address = *(uint8_t *)handle;

  HAL_I2C_Mem_Write(&hi2c3, address, reg, I2C_MEMADD_SIZE_8BIT, p_data, size, HW_DETECTION_I2C_TIMEOUT);

  return 0;
}

static int32_t ext_sensor_i2c_read(void *handle, uint8_t reg, uint8_t *p_data, uint16_t size)
{
  uint8_t address = *(uint8_t *)handle;

  HAL_I2C_Mem_Read(&hi2c3, address, reg, I2C_MEMADD_SIZE_8BIT, p_data, size, HW_DETECTION_I2C_TIMEOUT);

  return 0;
}

