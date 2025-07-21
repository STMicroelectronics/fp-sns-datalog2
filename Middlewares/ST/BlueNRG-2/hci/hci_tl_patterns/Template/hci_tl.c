/**
  ******************************************************************************
  * @file    hci_tl_template.c
  * @author  AMG - RF Application Team
  * @brief   Contains the basic functions for managing the framework required
  *          for handling the HCI interface
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include "hci_const.h"
#include "hci.h"
#include "hci_tl.h"

static hci_context_t    hci_context;

/***************************** Exported functions *****************************/

/**
  * @brief Reset BlueNRG module.
  *
  * @param  None
  * @retval int32_t 0
  */
WEAK_FUNCTION(int32_t hci_tl_spi_reset(void))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_tl_spi_reset\r\n");

  return 0;
}

/**
  * @brief  Writes data from local buffer to SPI.
  *
  * @param  buffer : data buffer to be written
  * @param  size   : size of first data buffer to be written
  * @retval int32_t: Number of read bytes
  */
WEAK_FUNCTION(int32_t hci_tl_spi_send(uint8_t *buffer, uint16_t size))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_tl_spi_send\r\n");

  return 0;
}

/**
  * @brief  Reads from BlueNRG SPI buffer and store data into local buffer.
  *
  * @param  buffer : Buffer where data from SPI are stored
  * @param  size   : Buffer size
  * @retval int32_t: Number of read bytes
  */
WEAK_FUNCTION(int32_t hci_tl_spi_receive(uint8_t *buffer, uint16_t size))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_tl_spi_receive\r\n");

  return 0;
}

/*********************** HCI Transport layer functions ************************/

void hci_init(void(* user_evt_rx)(void *p_data), void *p_conf)
{
  /* USER CODE BEGIN hci_init 1 */

  /* USER CODE END hci_init 1 */

  if (user_evt_rx != NULL)
  {
    hci_context.user_evt_rx = user_evt_rx;
  }

  /* Initialize TL BLE layer */
  hci_tl_lowlevel_init();

  /* USER CODE BEGIN hci_init 2 */

  /* USER CODE END hci_init 2 */

  /* Initialize low level driver */
  hci_tl_spi_reset();
}

int32_t hci_send_req(struct hci_request *r, BOOL async)
{
  /* USER CODE BEGIN hci_send_req */

  /* USER CODE END hci_send_req */

  return 0;
}

void hci_user_evt_proc(void)
{
  /* USER CODE BEGIN hci_user_evt_proc */

  /* USER CODE END hci_user_evt_proc */
}

int32_t hci_notify_asynch_evt(void *pdata)
{
  /* USER CODE BEGIN hci_notify_asynch_evt */

  /* USER CODE END hci_notify_asynch_evt */

  return 0;
}

void hci_resume_flow(void)
{
  /* USER CODE BEGIN hci_resume_flow */

  /* USER CODE END hci_resume_flow */
}

void hci_cmd_resp_wait(uint32_t timeout)
{
  /* USER CODE BEGIN hci_cmd_resp_wait */

  /* USER CODE END hci_cmd_resp_wait */
}

void hci_cmd_resp_release(uint32_t flag)
{
  /* USER CODE BEGIN hci_cmd_resp_release */

  /* USER CODE END hci_cmd_resp_release */
}
