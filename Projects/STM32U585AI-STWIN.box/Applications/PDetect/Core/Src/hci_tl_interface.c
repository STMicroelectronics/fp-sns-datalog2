/**
  ******************************************************************************
  * @file    hci_tl_interface.c
  * @author  SRA
  * @brief   This file provides the implementation for all functions prototypes
  *          for the STM32 BlueNRG HCI Transport Layer interface
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#define HCI_TL
#define HCI_TL_INTERFACE

#ifdef HCI_TL
#include "hci_tl.h"
#include "hci.h"
#else
#include "hci_tl_interface.h"
#endif /* HCI_TL */


#include "stm32u5xx_hal_exti.h"
#include "spi.h"

/* Defines -------------------------------------------------------------------*/

#define HEADER_SIZE       5U
#define MAX_BUFFER_SIZE   255U
#define TIMEOUT_DURATION  15U
#define TIMEOUT_IRQ_HIGH  1000U

/* Private variables ---------------------------------------------------------*/
EXTI_HandleTypeDef hexti1;

/* Private function prototypes -----------------------------------------------*/
static void HCI_TL_SPI_Enable_IRQ(void);
static void HCI_TL_SPI_Disable_IRQ(void);

/******************** IO Operation and BUS services ***************************/

/**
  * @brief  Enable SPI IRQ.
  * @param  None
  * @retval None
  */
static void HCI_TL_SPI_Enable_IRQ(void)
{
  HAL_NVIC_EnableIRQ(HCI_TL_SPI_EXTI_IRQn);
}

/**
  * @brief  Disable SPI IRQ.
  * @param  None
  * @retval None
  */
static void HCI_TL_SPI_Disable_IRQ(void)
{
  HAL_NVIC_DisableIRQ(HCI_TL_SPI_EXTI_IRQn);
}

/**
  * @brief  Initializes the peripherals communication with the BlueNRG
  *         Expansion Board (via SPI, I2C, USART, ...)
  *
  * @param  void* Pointer to configuration struct
  * @retval int32_t Status
  */
int32_t HCI_TL_SPI_Init(void *pConf)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* Configure EXTI Line */
  GPIO_InitStruct.Pin = BLE_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BLE_INT_GPIO_Port, &GPIO_InitStruct);
  HAL_NVIC_SetPriority(EXTI14_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI14_IRQn);

  GPIO_InitStruct.Pin = BLE_SPI_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BLE_SPI_CS_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BLE_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BLE_RST_GPIO_Port, &GPIO_InitStruct);

  MX_SPI3_Init();
  return 0;
}

/**
  * @brief  DeInitializes the peripherals communication with the BlueNRG
  *         Expansion Board (via SPI, I2C, USART, ...)
  *
  * @param  None
  * @retval int32_t 0
  */
int32_t HCI_TL_SPI_DeInit(void)
{
  return 0;
}

/**
  * @brief Reset BlueNRG module.
  *
  * @param  None
  * @retval int32_t 0
  */
int32_t HCI_TL_SPI_Reset(void)
{
  /* Deselect CS PIN for BlueNRG to avoid spurious commands */
  HAL_GPIO_WritePin(BLE_SPI_CS_GPIO_Port, BLE_SPI_CS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(BLE_RST_GPIO_Port, BLE_RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(5);
  HAL_GPIO_WritePin(BLE_RST_GPIO_Port, BLE_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(5);
  return 0;
}

int32_t HCI_TL_GetTick(void)
{
  return (int32_t)HAL_GetTick();
}
/**
  * @brief  Reads from BlueNRG SPI buffer and store data into local buffer.
  *
  * @param  buffer : Buffer where data from SPI are stored
  * @param  size   : Buffer size
  * @retval int32_t: Number of read bytes
  */
int32_t HCI_TL_SPI_Receive(uint8_t *buffer, uint16_t size)
{
  uint16_t byte_count;
  uint8_t len = 0;
  uint8_t char_ff = 0xff;
  volatile uint8_t read_char;

  uint8_t header_master[HEADER_SIZE] = {0x0b, 0x00, 0x00, 0x00, 0x00};
  uint8_t header_slave[HEADER_SIZE];

  HCI_TL_SPI_Disable_IRQ();

  /* CS reset */
  HAL_GPIO_WritePin(BLE_SPI_CS_GPIO_Port, BLE_SPI_CS_Pin, GPIO_PIN_RESET);

  /* Read the header */
  HAL_SPI_TransmitReceive(&hspi3, header_master, header_slave, HEADER_SIZE, 1000);

  /* device is ready */
  byte_count = (header_slave[4] << 8) | header_slave[3];

  if (byte_count > 0)
  {
    /* avoid to read more data that size of the buffer */
    if (byte_count > size)
    {
      byte_count = size;
    }

    for (len = 0; len < byte_count; len++)
    {
      HAL_SPI_TransmitReceive(&hspi3, &char_ff, (uint8_t *)&read_char, 1, 1000);
      buffer[len] = read_char;
    }
  }

  /**
    * To be aligned to the SPI protocol.
    * Can bring to a delay inside the frame, due to the BlueNRG-2 that needs
    * to check if the header is received or not.
    */
  uint32_t tickstart = HAL_GetTick();
  while ((HAL_GetTick() - tickstart) < TIMEOUT_IRQ_HIGH)
  {
    if (HAL_GPIO_ReadPin(BLE_INT_GPIO_Port, BLE_INT_Pin) == GPIO_PIN_RESET)
    {
      break;
    }
  }

  HCI_TL_SPI_Enable_IRQ();

  /* Release CS line */
  HAL_GPIO_WritePin(BLE_SPI_CS_GPIO_Port, BLE_SPI_CS_Pin, GPIO_PIN_SET);

  return len;
}

/**
  * @brief  Writes data from local buffer to SPI.
  *
  * @param  buffer : data buffer to be written
  * @param  size   : size of first data buffer to be written
  * @retval int32_t: Number of read bytes
  */
int32_t HCI_TL_SPI_Send(uint8_t *buffer, uint16_t size)
{
  int32_t result;
  uint16_t rx_bytes;

  uint8_t header_master[HEADER_SIZE] = {0x0a, 0x00, 0x00, 0x00, 0x00};
  uint8_t header_slave[HEADER_SIZE];

  static uint8_t read_char_buf[MAX_BUFFER_SIZE];
  uint32_t tickstart = HAL_GetTick();

  HCI_TL_SPI_Disable_IRQ();

  do
  {
    uint32_t tickstart_data_available = HAL_GetTick();

    result = 0;

    /* CS reset */
    HAL_GPIO_WritePin(BLE_SPI_CS_GPIO_Port, BLE_SPI_CS_Pin, GPIO_PIN_RESET);

    /*
    * Wait until BlueNRG-2 is ready.
    * When ready it will raise the IRQ pin.
    */
    while (!IsDataAvailable())
    {
      if ((HAL_GetTick() - tickstart_data_available) > TIMEOUT_DURATION)
      {
        result = -3;
        break;
      }
    }
    if (result == -3)
    {
      /* The break causes the exiting from the "while", so the CS line must be released */
      HAL_GPIO_WritePin(BLE_SPI_CS_GPIO_Port, BLE_SPI_CS_Pin, GPIO_PIN_SET);
      break;
    }


    /* Read header */
    HAL_SPI_TransmitReceive(&hspi3, header_master, header_slave, HEADER_SIZE, 1000);

    rx_bytes = (((uint16_t)header_slave[2]) << 8) | ((uint16_t)header_slave[1]);

    if (rx_bytes >= size)
    {
      /* Buffer is big enough */
      HAL_SPI_TransmitReceive(&hspi3, buffer, read_char_buf, size, 1000);
    }
    else
    {
      /* Buffer is too small */
      result = -2;
    }

    /* Release CS line */
    HAL_GPIO_WritePin(BLE_SPI_CS_GPIO_Port, BLE_SPI_CS_Pin, GPIO_PIN_SET);

    if ((HAL_GetTick() - tickstart) > TIMEOUT_DURATION)
    {
      result = -3;
      break;
    }

  } while (result < 0);

  /**
    * To be aligned to the SPI protocol.
    * Can bring to a delay inside the frame, due to the BlueNRG-2 that needs
    * to check if the header is received or not.
    */
  tickstart = HAL_GetTick();
  while ((HAL_GetTick() - tickstart) < TIMEOUT_IRQ_HIGH)
  {
    if (HAL_GPIO_ReadPin(BLE_INT_GPIO_Port, BLE_INT_Pin) == GPIO_PIN_RESET)
    {
      break;
    }
  }

  HCI_TL_SPI_Enable_IRQ();

  return result;
}

#ifdef HCI_TL
/**
  * @brief  Reports if the BlueNRG has data for the host micro.
  *
  * @param  None
  * @retval int32_t: 1 if data are present, 0 otherwise
  */
int32_t IsDataAvailable(void)
{
  return (HAL_GPIO_ReadPin(BLE_INT_GPIO_Port, BLE_INT_Pin) == GPIO_PIN_SET);
}
#endif /* HCI_TL */

/***************************** hci_tl_interface main functions *****************************/
/**
  * @brief  Register hci_tl_interface IO bus services
  *
  * @param  None
  * @retval None
  */
void hci_tl_lowlevel_init(void)
{

#ifdef HCI_TL
  tHciIO fops;

  /* Register IO bus services */
  fops.Init    = HCI_TL_SPI_Init;
  fops.DeInit  = HCI_TL_SPI_DeInit;
  fops.Send    = HCI_TL_SPI_Send;
  fops.Receive = HCI_TL_SPI_Receive;
  fops.Reset   = HCI_TL_SPI_Reset;
  fops.GetTick = HCI_TL_GetTick;

  hci_register_io_bus(&fops);

#endif /* HCI_TL */

}


