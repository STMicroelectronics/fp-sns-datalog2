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

#include "mx.h"
#include "spi.h"
#include "i2c.h"
#include "services/systypes.h"
#include "HardwareDetection.h"
#include "iis3dwb_reg.h"
#include "iis3dwb10is_reg.h"
#include "ilps28qsw_reg.h"
#include "ism330bx_reg.h"
#include "ism330is_reg.h"
#include "ism6hg256x_reg.h"
#include "stts22h_reg.h"
#include "TSC1641.h"
#include "iis2dulpx_reg.h"
#include "tx_api.h"

#define HW_DETECTION_I2C_TIMEOUT  500U

#define ST25_ADDR_DATA_I2C                ((uint8_t)0xAE)
#define ST25_ICREF_REG                    ((uint16_t)0x0017)

#define ICREF_ST25DV04                    0x24U   /* ST25DV         4Kbits ICref */
#define ICREF_ST25DV64                    0x26U   /* ST25DV     16/64Kbits ICref */
#define ICREF_ST25DV04KC                  0x50U   /* ST25DVxxKC     4Kbits ICref */
#define ICREF_ST25DV64KC                  0x51U   /* ST25DVxxKC 16/64Kbits ICref */

extern SPI_HandleTypeDef hspi2;
extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c3;

static void HardwareDetection_SPI2_CS_Init(void);
static void HardwareDetection_SPI2_CS_DeInit(void);

static int32_t ext_sensor_spi_read(void *handle, uint8_t reg, uint8_t *p_data, uint16_t size);
static int32_t ext_sensor_spi_write(void *handle, uint8_t reg, uint8_t *p_data, uint16_t size);

static int32_t ext_sensor_i2c_read(void *handle, uint8_t reg, uint8_t *p_data, uint16_t size);
static int32_t ext_sensor_i2c_write(void *handle, uint8_t reg, uint8_t *p_data, uint16_t size);


/* Public functions declaration */
/*********************************/


/**
  * Detect an external IIS2DULPX sensor
  *
  * @return TRUE if the sensor was found, FALSE otherwise
  */
boolean_t HardwareDetection_Check_Ext_IIS2DULPX(void)
{
  uint8_t whoami_val = 0U;
  boolean_t found = FALSE;
  stmdev_ctx_t ctx;

  ctx.read_reg = ext_sensor_spi_read;
  ctx.write_reg = ext_sensor_spi_write;
  ctx.mdelay = (stmdev_mdelay_ptr)tx_thread_sleep;

  HardwareDetection_SPI2_CS_Init();
  MX_SPI2_Init();

  iis2dulpx_exit_deep_power_down(&ctx);
  iis2dulpx_device_id_get(&ctx, (uint8_t *) &whoami_val);

  HAL_SPI_DeInit(&hspi2);
  HardwareDetection_SPI2_CS_DeInit();

  if (whoami_val == IIS2DULPX_ID)
  {
    found = TRUE;
  }
  return found;
}

/**
  * Detect an external IIS3DWB sensor
  *
  * @return TRUE if the sensor was found, FALSE otherwise
  */
boolean_t HardwareDetection_Check_Ext_IIS3DWB(void)
{
  uint8_t whoami_val = 0U;
  boolean_t found = FALSE;
  stmdev_ctx_t ctx;

  ctx.read_reg = ext_sensor_spi_read;
  ctx.write_reg = ext_sensor_spi_write;

  HardwareDetection_SPI2_CS_Init();
  MX_SPI2_Init();

  iis3dwb_device_id_get(&ctx, (uint8_t *) &whoami_val);

  HAL_SPI_DeInit(&hspi2);
  HardwareDetection_SPI2_CS_DeInit();

  if (whoami_val == IIS3DWB_ID)
  {
    found = TRUE;
  }
  return found;
}

/**
  * Detect an external IIS3DWB10IS sensor
  *
  * @return TRUE if the sensor was found, FALSE otherwise
  */
boolean_t HardwareDetection_Check_Ext_IIS3DWB10IS(void)
{
  uint8_t whoami_val = 0U;
  boolean_t found = FALSE;
  stmdev_ctx_t ctx;

  ctx.read_reg = ext_sensor_spi_read;
  ctx.write_reg = ext_sensor_spi_write;

  HardwareDetection_SPI2_CS_Init();
  MX_SPI2_20MHz_Init();

  iis3dwb10is_device_id_get(&ctx, (uint8_t *) &whoami_val);

  HAL_SPI_DeInit(&hspi2);
  HardwareDetection_SPI2_CS_DeInit();

  if (whoami_val == IIS3DWB10IS_ID)
  {
    found = TRUE;
  }
  return found;
}

/**
  * Detect an external ILPS28QSW sensor
  *
  * @return TRUE if the sensor was found, FALSE otherwise
  */
boolean_t HardwareDetection_Check_Ext_ILPS28QSW(void)
{
  uint8_t whoami_val = 0U;
  boolean_t found = FALSE;

  MX_I2C3_Init();

  HAL_I2C_Mem_Read(&hi2c3, ILPS28QSW_I2C_ADD, ILPS28QSW_WHO_AM_I, I2C_MEMADD_SIZE_8BIT, &whoami_val, 1,
                   HW_DETECTION_I2C_TIMEOUT);

  HAL_I2C_DeInit(&hi2c3);

  if (whoami_val == ILPS28QSW_ID)
  {
    found = TRUE;
  }
  return found;
}

/**
  * Detect an external ISM330BX sensor
  *
  * @return TRUE if the sensor was found, FALSE otherwise
  */
boolean_t HardwareDetection_Check_Ext_ISM330BX(void)
{
  uint8_t whoami_val = 0U;
  boolean_t found = FALSE;
  stmdev_ctx_t ctx;

  ctx.read_reg = ext_sensor_spi_read;
  ctx.write_reg = ext_sensor_spi_write;

  HardwareDetection_SPI2_CS_Init();
  MX_SPI2_Init();

  ism330bx_device_id_get(&ctx, (uint8_t *) &whoami_val);

  HAL_SPI_DeInit(&hspi2);
  HardwareDetection_SPI2_CS_DeInit();

  if (whoami_val == ISM330BX_ID)
  {
    found = TRUE;
  }
  return found;
}

/**
  * Detect an external ISM330IS(N) sensor
  *
  * @return TRUE if the sensor was found, FALSE otherwise
  */
boolean_t HardwareDetection_Check_Ext_ISM330IS(void)
{
  uint8_t whoami_val = 0U;
  boolean_t found = FALSE;
  stmdev_ctx_t ctx;

  ctx.read_reg = ext_sensor_spi_read;
  ctx.write_reg = ext_sensor_spi_write;

  HardwareDetection_SPI2_CS_Init();
  MX_SPI2_Init();

  ism330is_device_id_get(&ctx, (uint8_t *) &whoami_val);

  HAL_SPI_DeInit(&hspi2);
  HardwareDetection_SPI2_CS_DeInit();

  if (whoami_val == ISM330IS_ID)
  {
    found = TRUE;
  }
  return found;
}

/**
  * Detect an external ISM6HG256X sensor
  *
  * @return TRUE if the sensor was found, FALSE otherwise
  */
boolean_t HardwareDetection_Check_Ext_ISM6HG256X(void)
{
  uint8_t whoami_val = 0U;
  boolean_t found = FALSE;
  stmdev_ctx_t ctx;

  ctx.read_reg = ext_sensor_spi_read;
  ctx.write_reg = ext_sensor_spi_write;

  HardwareDetection_SPI2_CS_Init();
  MX_SPI2_Init();

  ism6hg256x_device_id_get(&ctx, (uint8_t *) &whoami_val);

  HAL_SPI_DeInit(&hspi2);
  HardwareDetection_SPI2_CS_DeInit();

  if (whoami_val == ISM6HG256X_ID)
  {
    found = TRUE;
  }
  return found;
}


/**
  * Detect an external STTS22H sensor
  *
  * @param device_address: return the address of the device, if found. [output]
  * @return TRUE if the sensor was found, FALSE otherwise
  */
boolean_t HardwareDetection_Check_Ext_STTS22H(uint8_t *device_address)
{
  uint8_t whoami_val = 0U;
  boolean_t found = FALSE;
  uint8_t addr;
  stmdev_ctx_t ctx;

  ctx.read_reg = ext_sensor_i2c_read;
  ctx.write_reg = ext_sensor_i2c_write;
  ctx.handle = (void *)&addr;

  MX_I2C3_Init();

  addr = STTS22H_I2C_ADD_H;
  stts22h_dev_id_get(&ctx, (uint8_t *) &whoami_val);

  if (whoami_val == STTS22H_ID)
  {
    found = TRUE;

  }
  else
  {
    addr = STTS22H_I2C_ADD_L;
    stts22h_dev_id_get(&ctx, (uint8_t *) &whoami_val);
    if (whoami_val == STTS22H_ID)
    {
      found = TRUE;
    }
  }

  HAL_I2C_DeInit(&hi2c3);

  *device_address = addr;
  return found;
}

/**
  * Detect an external TSC1641 sensor
  *
  * @param device_address: return the address of the device, if found. [output]
  * @return TRUE if the sensor was found, FALSE otherwise
  */
boolean_t HardwareDetection_Check_Ext_TSC1641(void)
{
  uint8_t whoami_val = 0U;
  boolean_t found = FALSE;

  MX_I2C3_Init();

  HAL_I2C_Mem_Read(&hi2c3, I2C_TSC1641_ADD_R, TSC1641_RegAdd_DieID, I2C_MEMADD_SIZE_8BIT, &whoami_val, 2,
                   HW_DETECTION_I2C_TIMEOUT);

  if (whoami_val == 0x10)
  {
    found = TRUE;
  }
  HAL_I2C_DeInit(&hi2c3);
  return found;
}

/* Private function definition */
/*******************************/


static void HardwareDetection_SPI2_CS_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, CS_DHCX_Pin | CS_DLPC_Pin, GPIO_PIN_SET);
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOI, EX_SPI_NSS_Pin | CS_ICLX_Pin, GPIO_PIN_SET);
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_DWB_GPIO_Port, CS_DWB_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PHPin PHPin PHPin */
  GPIO_InitStruct.Pin = CS_DHCX_Pin | CS_DLPC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PIPin PIPin */
  GPIO_InitStruct.Pin = EX_SPI_NSS_Pin | CS_ICLX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = CS_DWB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_DWB_GPIO_Port, &GPIO_InitStruct);
}

static void HardwareDetection_SPI2_CS_DeInit(void)
{
  HAL_GPIO_DeInit(GPIOH, CS_DHCX_Pin);
  HAL_GPIO_DeInit(GPIOH, CS_DLPC_Pin);
  HAL_GPIO_DeInit(GPIOI, EX_SPI_NSS_Pin);
  HAL_GPIO_DeInit(GPIOI, CS_ICLX_Pin);
  HAL_GPIO_DeInit(CS_DWB_GPIO_Port, CS_DWB_Pin);

  /* GPIO Ports Clock Disable */
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();
  __HAL_RCC_GPIOI_CLK_DISABLE();
}


static int32_t ext_sensor_spi_write(void *handle, uint8_t reg, uint8_t *p_data, uint16_t size)
{
  /* CS Enable */
  HAL_GPIO_WritePin(GPIOI, EX_SPI_NSS_Pin, GPIO_PIN_RESET);

  HAL_SPI_Transmit(&hspi2, &reg, 1, 1000);
  HAL_SPI_Transmit(&hspi2, p_data, size, 1000);

  /* CS Disable */
  HAL_GPIO_WritePin(GPIOI, EX_SPI_NSS_Pin, GPIO_PIN_SET);

  return 0;
}

static int32_t ext_sensor_spi_read(void *handle, uint8_t reg, uint8_t *p_data, uint16_t size)
{
  /* CS Enable */
  HAL_GPIO_WritePin(GPIOI, EX_SPI_NSS_Pin, GPIO_PIN_RESET);

  reg |= 0x80;

  HAL_SPI_Transmit(&hspi2, &reg, 1, 1000);
  HAL_SPI_Receive(&hspi2, p_data, size, 1000);

  /* CS Disable */
  HAL_GPIO_WritePin(GPIOI, EX_SPI_NSS_Pin, GPIO_PIN_SET);

  return 0;
}


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

