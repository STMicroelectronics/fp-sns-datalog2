/**
  ******************************************************************************
  * @file    fw_stm32_sd_driver_glue.c
  * @author  SRA
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

#include "fx_stm32_sd_driver.h"

/* USER CODE BEGIN  0 */
TX_SEMAPHORE transfer_semaphore;
/* USER CODE END  0 */

/**
  * @brief Initializes the SD IP instance
  * @param uINT Instance SD instance to initialize
  * @retval 0 on success error value otherwise
  */
INT fx_stm32_sd_init(UINT instance)
{
  INT ret = 0;
  /* USER CODE BEGIN  FX_SD_INIT */
  if (BSP_SD_Init(instance) == BSP_ERROR_NONE)
  {
    ret = 0;
  }
  else
  {
    ret = 1;
  }
  /* USER CODE END  FX_SD_INIT */

  return ret;
}

/**
  * @brief Deinitializes the SD IP instance
  * @param uINT Instance SD instance to deinitialize
  * @retval 0 on success error value otherwise
  */
INT fx_stm32_sd_deinit(UINT instance)
{
  INT ret = 0;
  /* USER CODE BEGIN  FX_SD_DEINIT */
  UNUSED(instance);
  /* USER CODE END  FX_SD_DEINIT */

  return ret;
}

/**
  * @brief Check the SD IP status.
  * @param uINT Instance SD instance to check
  * @retval 0 when ready 1 when busy
  */
INT fx_stm32_sd_get_status(UINT instance)
{
  INT ret = 0;
  /* USER CODE BEGIN  GET_STATUS */
  if (BSP_SD_GetCardState(instance) == BSP_ERROR_NONE)
  {
    ret = 0;
  }
  else
  {
    ret = 1;
  }
  /* USER CODE END  GET_STATUS */
  return ret;
}

/**
  * @brief Read Data from the SD device into a buffer.
  * @param uINT *Buffer buffer into which the data is to be read.
  * @param uINT StartBlock the first block to start reading from.
  * @param uINT NbrOfBlocks total number of blocks to read.
  * @retval 0 on success error code otherwise
  */
INT fx_stm32_sd_read_blocks(UINT instance, UINT *buffer, UINT start_block, UINT total_blocks)
{
  INT ret = 0;
  /* USER CODE BEGIN  READ_BLOCKS */
  if (BSP_SD_ReadBlocks_DMA(instance, (uint32_t *) buffer, start_block, total_blocks) != BSP_ERROR_NONE)
  {
    ret = -1;
  }
  else
  {
  }
  /* USER CODE END  READ_BLOCKS */
  return ret;
}
/**
  * @brief Write data buffer into the SD device.
  * @param uINT *Buffer buffer .to write into the SD device.
  * @param uINT StartBlock the first block to start writing from.
  * @param uINT NbrOfBlocks total number of blocks to write.
  * @retval 0 on success error code otherwise
  */

INT fx_stm32_sd_write_blocks(UINT instance, UINT *buffer, UINT start_block, UINT total_blocks)
{
  INT ret = 0;
  /* USER CODE BEGIN  WRITE_BLOCKS */
  if (BSP_SD_WriteBlocks_DMA(instance, (uint32_t *) buffer, start_block, total_blocks) != BSP_ERROR_NONE)
  {
    ret = -1;
  }
  else
  {
  }
  /* USER CODE END  WRITE_BLOCKS */
  return ret;

}

/* USER CODE BEGIN  1 */

/**
  * @brief  Initializes SD Detect pin.
  */
void SD_DetectInit(void)
{
  /* GPIO Detect pin configuration */
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
  * @brief  Detects if SD card is correctly plugged in the memory slot or not.
  * @retval Returns if SD is detected or not
  */
bool SD_IsDetected(void)
{
  bool res = true;

  /* Check SD card detect pin */
  if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5) == GPIO_PIN_SET)
  {
    res = false;
  }

  return res;
}

void BSP_SD_WriteCpltCallback(uint32_t instance)
{
  tx_semaphore_put(&transfer_semaphore);
}

/**
  * @brief SD DMA Rx Transfer completed callbacks
  * @param Instance the sd instance
  * @retval None
  */
void BSP_SD_ReadCpltCallback(uint32_t instance)
{
  tx_semaphore_put(&transfer_semaphore);
}
/* USER CODE END  1 */
