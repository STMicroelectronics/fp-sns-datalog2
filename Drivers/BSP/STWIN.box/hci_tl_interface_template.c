/**
  ******************************************************************************
  * @file  : hci_tl_interface_template.c
  * @brief : This file provides the implementation for all functions prototypes 
  *          for the STM32 BlueNRG HCI Transport Layer interface
  *		     This file must be moved in the application folder by the user 
  *		     and renamed 'hci_tl_interface'		
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
#else
#include "hci_tl_interface.h"
#endif /* HCI_TL */

#include "stm32l4xx_hal_exti.h"

/* Defines -------------------------------------------------------------------*/

#define HEADER_SIZE       5U
#define MAX_BUFFER_SIZE   255U
#define TIMEOUT_DURATION  15U

/* Private variables ---------------------------------------------------------*/
EXTI_HandleTypeDef hexti1;

/* Private function prototypes -----------------------------------------------*/
static void HCI_TL_SPI_Enable_IRQ(void);
static void HCI_TL_SPI_Disable_IRQ(void);
static int32_t IsDataAvailable(void);

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
int32_t HCI_TL_SPI_Init(void* pConf)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* Enable GPIO Ports Clock */  
  __GPIOA_CLK_ENABLE();
  __GPIOD_CLK_ENABLE();
  __GPIOC_CLK_ENABLE();
  __GPIOG_CLK_ENABLE();
  
  /* Enable SPI clock */
  __SPI2_CLK_ENABLE();  
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableVddUSB();  
  HAL_PWREx_EnableVddIO2();
  
  /*Configure EXTI Line */
  GPIO_InitStruct.Pin = HCI_TL_SPI_EXTI_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(HCI_TL_SPI_EXTI_PORT, &GPIO_InitStruct);
   
  /*Configure CS & RESET Line */
  GPIO_InitStruct.Pin =  HCI_TL_RST_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(HCI_TL_RST_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = HCI_TL_SPI_CS_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(HCI_TL_SPI_CS_PORT, &GPIO_InitStruct);
    
  return BSP_SPI2_Init();
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
  HAL_GPIO_DeInit(HCI_TL_SPI_EXTI_PORT, HCI_TL_SPI_EXTI_PIN); 
  HAL_GPIO_DeInit(HCI_TL_SPI_CS_PORT, HCI_TL_SPI_CS_PIN); 
  HAL_GPIO_DeInit(HCI_TL_RST_PORT, HCI_TL_RST_PIN);   
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
  HAL_GPIO_WritePin(HCI_TL_SPI_CS_PORT, HCI_TL_SPI_CS_PIN, GPIO_PIN_SET);

  HAL_GPIO_WritePin(HCI_TL_RST_PORT, HCI_TL_RST_PIN, GPIO_PIN_RESET);
  HAL_Delay(5);
  HAL_GPIO_WritePin(HCI_TL_RST_PORT, HCI_TL_RST_PIN, GPIO_PIN_SET);
  HAL_Delay(5);    
  
  return 0;
}  

/**
 * @brief  Reads from BlueNRG SPI buffer and store data into local buffer.
 *
 * @param  buffer : Buffer where data from SPI are stored
 * @param  size   : Buffer size
 * @retval int32_t: Number of read bytes
 */
int32_t HCI_TL_SPI_Receive(uint8_t* buffer, uint16_t size)
{
  uint16_t byte_count;
  uint8_t len = 0;
  uint8_t char_ff = 0xff;
  volatile uint8_t read_char;

  uint8_t header_master[HEADER_SIZE] = {0x0b, 0x00, 0x00, 0x00, 0x00};
  uint8_t header_slave[HEADER_SIZE];

  HCI_TL_SPI_Disable_IRQ();

  /* CS reset */
  HAL_GPIO_WritePin(HCI_TL_SPI_CS_PORT, HCI_TL_SPI_CS_PIN, GPIO_PIN_RESET);

  /* Read the header */
  BSP_SPI2_SendRecv(header_master, header_slave, HEADER_SIZE);

  /* device is ready */
  byte_count = (header_slave[4] << 8)| header_slave[3];

  if(byte_count > 0)
  {

    /* avoid to read more data that size of the buffer */

    if (byte_count > size)
	{
      byte_count = size;
    }

    for(len = 0; len < byte_count; len++)
    {           
      BSP_SPI2_SendRecv(&char_ff, (uint8_t*)&read_char, 1);
      buffer[len] = read_char;
    }
  }  

  /* Release CS line */
  HAL_GPIO_WritePin(HCI_TL_SPI_CS_PORT, HCI_TL_SPI_CS_PIN, GPIO_PIN_SET);

  HCI_TL_SPI_Enable_IRQ();

#if PRINT_CSV_FORMAT
  if (len > 0)
  {
    PRINT_CSV("BTOH->>\n");
    print_csv_time();
    for (int i=0; i<len; i++)
	{
      PRINT_CSV(" %02x", buffer[i]);
    }
    PRINT_CSV("\n");
  }
#endif

  return len;
}

/**
 * @brief  Writes data from local buffer to SPI.
 *
 * @param  buffer : data buffer to be written
 * @param  size   : size of first data buffer to be written
 * @retval int32_t: Number of read bytes
 */
int32_t HCI_TL_SPI_Send(uint8_t* buffer, uint16_t size)
{  
#if PRINT_CSV_FORMAT
  PRINT_CSV("HTOB->>\n");
  print_csv_time();
  for (int i=0; i<size; i++)
  {
    PRINT_CSV(" %02x", buffer[i]);
  }
  PRINT_CSV("\n");
#endif

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
    HAL_GPIO_WritePin(HCI_TL_SPI_CS_PORT, HCI_TL_SPI_CS_PIN, GPIO_PIN_RESET);

    /*
     * Wait until BlueNRG-2 is ready.
     * When ready it will raise the IRQ pin.
     */
    while(!IsDataAvailable())
    {
      if((HAL_GetTick() - tickstart_data_available) > TIMEOUT_DURATION)
      {
        result = -3;
        break;
      }
    }
    if(result != -3)
    {

    /* Read header */
    BSP_SPI2_SendRecv(header_master, header_slave, HEADER_SIZE);

    rx_bytes = (((uint16_t)header_slave[2])<<8) | ((uint16_t)header_slave[1]);

    if(rx_bytes >= size)
    {
      /* Buffer is big enough */
      BSP_SPI2_SendRecv(buffer, read_char_buf, size);
    }
	else
    {
      /* Buffer is too small */
      result = -2;
    }
	}
    /* Release CS line */
    HAL_GPIO_WritePin(HCI_TL_SPI_CS_PORT, HCI_TL_SPI_CS_PIN, GPIO_PIN_SET);

    if((HAL_GetTick() - tickstart) > TIMEOUT_DURATION)
    {
      result = -3;
	  break;
    }
  } while(result < 0);

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
static int32_t IsDataAvailable(void)
{
  return (HAL_GPIO_ReadPin(HCI_TL_SPI_EXTI_PORT, HCI_TL_SPI_EXTI_PIN) == GPIO_PIN_SET);
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
  /* USER CODE BEGIN hci_tl_lowlevel_init 1 */
  
  /* USER CODE END hci_tl_lowlevel_init 1 */
#ifdef HCI_TL
  tHciIO fops;  
  
  /* Register IO bus services */
  fops.Init    = HCI_TL_SPI_Init;
  fops.DeInit  = HCI_TL_SPI_DeInit;
  fops.Send    = HCI_TL_SPI_Send;
  fops.Receive = HCI_TL_SPI_Receive;
  fops.Reset   = HCI_TL_SPI_Reset;
  fops.GetTick = BSP_GetTick;
  
  hci_register_io_bus (&fops);
  
  /* USER CODE BEGIN hci_tl_lowlevel_init 2 */
  
  /* USER CODE END hci_tl_lowlevel_init 2 */
  
  /* Register event irq handler */
  HAL_NVIC_SetPriority(HCI_TL_SPI_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(HCI_TL_SPI_EXTI_IRQn);
#endif /* HCI_TL */  

  /* USER CODE BEGIN hci_tl_lowlevel_init 3 */
  
  /* USER CODE END hci_tl_lowlevel_init 3 */

}

/**
  * @brief HCI Transport Layer Low Level Interrupt Service Routine
  *
  * @param  None
  * @retval None
  */
void hci_tl_lowlevel_isr(void)
{
  /* Call hci_notify_asynch_evt() */
#ifdef HCI_TL
  while(IsDataAvailable())
  {        
    hci_notify_asynch_evt(NULL);
  }
#endif /* HCI_TL */

  /* USER CODE BEGIN hci_tl_lowlevel_isr */

  /* USER CODE END hci_tl_lowlevel_isr */ 
}

