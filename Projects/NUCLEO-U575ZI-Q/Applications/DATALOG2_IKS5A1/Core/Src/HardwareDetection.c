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
#include "services/systypes.h"
#include "HardwareDetection.h"
#include "iis3dwb10is_reg.h"

extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c3;

static int32_t ext_sensor_spi_read(void *handle, uint8_t reg, uint8_t *p_data, uint16_t size);
static int32_t ext_sensor_spi_write(void *handle, uint8_t reg, uint8_t *p_data, uint16_t size);


/* Public functions declaration */
/*********************************/
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

  MX_SPI1_Init();

  iis3dwb10is_device_id_get(&ctx, (uint8_t *) &whoami_val);

  HAL_SPI_DeInit(&hspi1);

  if (whoami_val == IIS3DWB10IS_ID)
  {
    found = TRUE;
  }
  return found;
}


/* Private function definition */
/*******************************/

static int32_t ext_sensor_spi_write(void *handle, uint8_t reg, uint8_t *p_data, uint16_t size)
{
  /* CS Enable */
  HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);

  HAL_SPI_Transmit(&hspi1, &reg, 1, 1000);
  HAL_SPI_Transmit(&hspi1, p_data, size, 1000);

  /* CS Disable */
  HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

  return 0;
}

static int32_t ext_sensor_spi_read(void *handle, uint8_t reg, uint8_t *p_data, uint16_t size)
{
  /* CS Enable */
  HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);

  reg |= 0x80;

  HAL_SPI_Transmit(&hspi1, &reg, 1, 1000);
  HAL_SPI_Receive(&hspi1, p_data, size, 1000);

  /* CS Disable */
  HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

  return 0;
}

