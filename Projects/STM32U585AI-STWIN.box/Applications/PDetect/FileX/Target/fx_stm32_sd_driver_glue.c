/**
  ******************************************************************************
  * @file    fw_stm32_sd_driver_glue.c
  * @author  SRA
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

#include "fx_stm32_sd_driver.h"

/* USER CODE BEGIN  0 */
#include "sdmmc.h"
TX_SEMAPHORE transfer_semaphore;
/* USER CODE END  0 */

/**
  * @brief Initializes the SD IP instance
  * @param UINT instance SD instance to initialize
  * @retval 0 on success error value otherwise
  */
INT fx_stm32_sd_init(UINT instance)
{
  INT ret = 0;

  /* USER CODE BEGIN PRE_FX_SD_INIT */
  UNUSED(instance);

  /* Check if SD card is present */
  SD_DetectInit();
  if (!SD_IsDetected())
  {
    return -1;
  }

  /* USER CODE END PRE_FX_SD_INIT */

#if (FX_STM32_SD_INIT == 1)
  MX_SDMMC1_SD_Init();
#endif

  /* USER CODE BEGIN POST_FX_SD_INIT */
#if (FX_STM32_SD_INIT == 1)
  if (HAL_SD_ConfigWideBusOperation(&hsd1, SDMMC_BUS_WIDE_4B) != HAL_OK)
  {
    ret = -1;
  }
  else
  {
    /* Try to switch to High Speed mode if the card supports this mode */
    (void)HAL_SD_ConfigSpeedBusOperation(&hsd1, SDMMC_SPEED_MODE_HIGH);
  }
#endif
  /* USER CODE END POST_FX_SD_INIT */

  return ret;
}

/**
  * @brief Deinitializes the SD IP instance
  * @param UINT instance SD instance to deinitialize
  * @retval 0 on success error value otherwise
  */
INT fx_stm32_sd_deinit(UINT instance)
{
  INT ret = 0;

  /* USER CODE BEGIN PRE_FX_SD_DEINIT */
  UNUSED(instance);
  /* USER CODE END PRE_FX_SD_DEINIT */

  if (HAL_SD_DeInit(&hsd1) != HAL_OK)
  {
    ret = 1;
  }

  /* USER CODE BEGIN POST_FX_SD_DEINIT */

  /* USER CODE END POST_FX_SD_DEINIT */

  return ret;
}

/**
  * @brief Check the SD IP status.
  * @param UINT instance SD instance to check
  * @retval 0 when ready 1 when busy
  */
INT fx_stm32_sd_get_status(UINT instance)
{
  INT ret = 0;

  /* USER CODE BEGIN PRE_GET_STATUS */
  UNUSED(instance);
  /* USER CODE END PRE_GET_STATUS */

  if (HAL_SD_GetCardState(&hsd1) != HAL_SD_CARD_TRANSFER)
  {
    ret = 1;
  }

  /* USER CODE BEGIN POST_GET_STATUS */

  /* USER CODE END POST_GET_STATUS */

  return ret;
}

/**
  * @brief Read Data from the SD device into a buffer.
  * @param UINT instance SD IP instance to read from.
  * @param UINT *buffer buffer into which the data is to be read.
  * @param UINT start_block the first block to start reading from.
  * @param UINT total_blocks total number of blocks to read.
  * @retval 0 on success error code otherwise
  */
INT fx_stm32_sd_read_blocks(UINT instance, UINT *buffer, UINT start_block, UINT total_blocks)
{
  INT ret = 0;

  /* USER CODE BEGIN PRE_READ_BLOCKS */
  UNUSED(instance);
  /* USER CODE END PRE_READ_BLOCKS */

  if (HAL_SD_ReadBlocks_DMA(&hsd1, (uint8_t *)buffer, start_block, total_blocks) != HAL_OK)
  {
    ret = 1;
  }

  /* USER CODE BEGIN POST_READ_BLOCKS */

  /* USER CODE END POST_READ_BLOCKS */

  return ret;
}

/**
  * @brief Write data buffer into the SD device.
  * @param UINT instance SD IP instance to write into.
  * @param UINT *buffer buffer to write into the SD device.
  * @param UINT start_block the first block to start writing into.
  * @param UINT total_blocks total number of blocks to write.
  * @retval 0 on success error code otherwise
  */
INT fx_stm32_sd_write_blocks(UINT instance, UINT *buffer, UINT start_block, UINT total_blocks)
{
  INT ret = 0;

  /* USER CODE BEGIN PRE_WRITE_BLOCKS */
  UNUSED(instance);
  /* USER CODE END PRE_WRITE_BLOCKS */

  if (HAL_SD_WriteBlocks_DMA(&hsd1, (uint8_t *)buffer, start_block, total_blocks) != HAL_OK)
  {
    ret = 1;
  }

  /* USER CODE BEGIN POST_WRITE_BLOCKS */

  /* USER CODE END POST_WRITE_BLOCKS */

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

  HAL_PWREx_EnableVddIO2();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  GPIO_InitStruct.Pin = SD_DETECT_Pin;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  HAL_GPIO_Init(SD_DETECT_GPIO_Port, &GPIO_InitStruct);
}

/**
  * @brief  Detects if SD card is correctly plugged in the memory slot or not.
  * @retval Returns if SD is detected or not
  */
bool SD_IsDetected(void)
{
  bool res = false;

  /* Check SD card detect pin */
  if (HAL_GPIO_ReadPin(SD_DETECT_GPIO_Port, SD_DETECT_Pin) == GPIO_PIN_RESET)
  {
    res = true;
  }

  return res;
}

/**
  * @brief  Get SD information about specific SD card.
  * @param  CardInfo  Pointer to HAL_SD_CardInfoTypedef structure
  * @retval BSP status
  */
int32_t SD_GetCardInfo(HAL_SD_CardInfoTypeDef *card_info)
{
  int32_t ret = 0;

  if (HAL_SD_GetCardInfo(&hsd1, card_info) != HAL_OK)
  {
    ret = -1;
  }

  return ret;
}


void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
  UNUSED(hsd);
  tx_semaphore_put(&transfer_semaphore);
}

/**
  * @brief SD DMA Rx Transfer completed callbacks
  * @param Instance the sd instance
  * @retval None
  */
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
  UNUSED(hsd);
  tx_semaphore_put(&transfer_semaphore);
}

/**
  * @brief SD Abort callbacks
  * @param hsd  SD handle
  * @retval None
  */
void HAL_SD_AbortCallback(SD_HandleTypeDef *hsd)
{
  UNUSED(hsd);
}

/* USER CODE END  1 */
