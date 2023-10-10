/**
  ******************************************************************************
  * @file    STWIN_wifi.c
  * @author  SRA
  *
  *
  * @brief   This file implements the IO operations to deal with the es-wifi
  *          module. It mainly Inits and Deinits the SPI interface. Send and
  *          receive data over it.
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
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "stm32l4xx_hal.h"

#include "STWIN.h"
#include "STWIN_wifi.h"

#include <core_cm4.h>
#include <string.h>
#include "es_wifi.h"
#include "es_wifi_conf.h"

/* Global variables  --------------------------------------------------------*/
SPI_HandleTypeDef hspi_wifi;

/* Function  definitions  --------------------------------------------------------*/
static void SPI_WIFI_MspInit(SPI_HandleTypeDef *hspi);

/* Private define ------------------------------------------------------------*/

#define WIFI_RESET_MODULE()                do{\
                                               HAL_GPIO_WritePin(WIFI_SPI_RESET_PORT, WIFI_SPI_RESET_PIN, GPIO_PIN_RESET);\
                                               HAL_Delay(10);\
                                               HAL_GPIO_WritePin(WIFI_SPI_RESET_PORT, WIFI_SPI_RESET_PIN, GPIO_PIN_SET);\
                                               HAL_Delay(500);\
                                             }while(0);


#define WIFI_ENABLE_NSS()                  do{ \
                                               HAL_GPIO_WritePin( WIFI_SPI_CS_PORT, WIFI_SPI_CS_PIN, GPIO_PIN_RESET );\
                                             }while(0);

#define WIFI_DISABLE_NSS()                 do{ \
                                               HAL_GPIO_WritePin( WIFI_SPI_CS_PORT, WIFI_SPI_CS_PIN, GPIO_PIN_SET );\
                                             }while(0);

#define WIFI_IS_CMDDATA_READY()            (HAL_GPIO_ReadPin(WIFI_DATA_READY_PORT, WIFI_DATA_READY_PIN) == GPIO_PIN_SET)

/* Private typedef -----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static  int volatile spi_rx_event = 0;
static  int volatile spi_tx_event = 0;
static  int volatile cmddata_rdy_rising_event = 0;

#ifdef WIFI_USE_CMSIS_OS
osMutexId es_wifi_mutex;
osMutexDef(es_wifi_mutex);

static    osMutexId spi_mutex;
osMutexDef(spi_mutex);

static    osSemaphoreId spi_rx_sem;
osSemaphoreDef(spi_rx_sem);

static    osSemaphoreId spi_tx_sem;
osSemaphoreDef(spi_tx_sem);

static    osSemaphoreId cmddata_rdy_rising_sem;
osSemaphoreDef(cmddata_rdy_rising_sem);

#endif

/* Private function prototypes -----------------------------------------------*/
static  int wait_cmddata_rdy_high(int timeout);
static  int wait_cmddata_rdy_rising_event(int timeout);
static  int wait_spi_tx_event(int timeout);
static  int wait_spi_rx_event(int timeout);
static  void SPI_WIFI_DelayUs(uint32_t);
static int8_t  SPI_WIFI_DeInit(void);
static int8_t  SPI_WIFI_Init(uint16_t mode);
static int8_t  SPI_WIFI_ResetModule(void);
static int16_t SPI_WIFI_ReceiveData(uint8_t *pData, uint16_t len, uint32_t timeout);
static int16_t SPI_WIFI_SendData(uint8_t *pData, uint16_t len, uint32_t timeout);
/* Private functions ---------------------------------------------------------*/

//int32_t wifi_probe(void **ll_drv_context);
ES_WIFIObject_t    EsWifiObj;




/*******************************************************************************
                       COM Driver Interface (SPI)
  *******************************************************************************/
/**
  * @brief  Initialize SPI MSP
  * @param  hspi: SPI handle
  * @retval None
  */
static void SPI_WIFI_MspInit(SPI_HandleTypeDef *hspi)
{
  UNUSED(hspi);
  GPIO_InitTypeDef GPIO_Init;

  HAL_PWREx_EnableVddIO2();
  BSP_Enable_DCDC2();

  WIFI_SPI_CLK_ENABLE();

  WIFI_SPI_RESET_CLK_ENABLE();

  WIFI_DATA_READY_CLK_ENABLE();
  __GPIOG_CLK_ENABLE();
  WIFI_SPI_CS_CLK_ENABLE();

  /* configure Data ready pin */
  GPIO_Init.Pin       = WIFI_DATA_READY_PIN;
  GPIO_Init.Mode      = GPIO_MODE_IT_RISING;
  GPIO_Init.Pull      = GPIO_NOPULL;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(WIFI_DATA_READY_PORT, &GPIO_Init);

  /* configure Reset pin */
  GPIO_Init.Pin       = WIFI_SPI_RESET_PIN;
  GPIO_Init.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_Init.Pull      = GPIO_NOPULL;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(WIFI_SPI_RESET_PORT, &GPIO_Init);

  /* configure SPI NSS pin pin */
  HAL_GPIO_WritePin(WIFI_SPI_CS_PORT, WIFI_SPI_CS_PIN, GPIO_PIN_SET);
  GPIO_Init.Pin       = WIFI_SPI_CS_PIN;
  GPIO_Init.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_Init.Pull      = GPIO_NOPULL;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(WIFI_SPI_CS_PORT, &GPIO_Init);

  /* configure SPI CLK pin */
  GPIO_Init.Pin       = WIFI_SPI_SCLK_PIN;
  GPIO_Init.Mode      = GPIO_MODE_AF_PP;
  GPIO_Init.Pull      = GPIO_NOPULL;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_Init.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(WIFI_SPI_SCLK_PORT, &GPIO_Init);

  /* configure SPI MOSI pin */
  GPIO_Init.Pin       = WIFI_SPI_MOSI_PIN;
  GPIO_Init.Mode      = GPIO_MODE_AF_PP;
  GPIO_Init.Pull      = GPIO_NOPULL;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_Init.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(WIFI_SPI_MOSI_PORT, &GPIO_Init);

  /* configure SPI MISO pin */
  GPIO_Init.Pin       = WIFI_SPI_MISO_PIN;
  GPIO_Init.Mode      = GPIO_MODE_AF_PP;
  GPIO_Init.Pull      = GPIO_NOPULL;
  GPIO_Init.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_Init.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(WIFI_SPI_MISO_PORT, &GPIO_Init);
}

/**
  * @brief  Initialize the SPI3
  * @param  None
  * @retval None
  */
static int8_t SPI_WIFI_Init(uint16_t mode)
{
  int8_t rc;

  if (mode == (uint16_t)ES_WIFI_INIT)
  {
    hspi_wifi.Instance               = WIFI_SPI_INSTANCE;
    SPI_WIFI_MspInit(&hspi_wifi);

    hspi_wifi.Init.Mode              = WIFI_SPI_MODE;
    hspi_wifi.Init.Direction         = WIFI_SPI_DIRECTION;
    hspi_wifi.Init.DataSize          = WIFI_SPI_DATASIZE;
    hspi_wifi.Init.CLKPolarity       = WIFI_SPI_CLKPOLARITY;
    hspi_wifi.Init.CLKPhase          = WIFI_SPI_CLKPHASE;
    hspi_wifi.Init.NSS               = WIFI_SPI_NSS;
    hspi_wifi.Init.BaudRatePrescaler = WIFI_SPI_BAUDRATEPRESCALER; /* 120/8= 15MHz (Inventek WIFI module supports up to 20MHz)*/
    hspi_wifi.Init.FirstBit          = WIFI_SPI_FIRSTBIT;
    hspi_wifi.Init.TIMode            = WIFI_SPI_TIMODE;
    hspi_wifi.Init.CRCCalculation    = WIFI_SPI_CRCCALCULATION;
    hspi_wifi.Init.CRCPolynomial     = WIFI_SPI_CRCPOLYNOMIAL;

    if (HAL_SPI_Init(&hspi_wifi) != HAL_OK)
    {
      return -1;
    }

    /* Enable Interrupt for Data Ready pin , GPIO_PIN1 */
    HAL_NVIC_SetPriority((IRQn_Type)EXTI15_10_IRQn, WIFI_SPI_INTERFACE_PRIO, 0x00);
    HAL_NVIC_EnableIRQ((IRQn_Type)EXTI15_10_IRQn);

    /* Enable Interrupt for SPI tx and rx */
    HAL_NVIC_SetPriority((IRQn_Type)WIFI_SPI_IRQn, WIFI_SPI_INTERFACE_PRIO, 0);
    HAL_NVIC_EnableIRQ((IRQn_Type)WIFI_SPI_IRQn);

#ifdef WIFI_USE_CMSIS_OS
    cmddata_rdy_rising_event = 0;
    es_wifi_mutex = osMutexCreate(osMutex(es_wifi_mutex));
    spi_mutex = osMutexCreate(osMutex(spi_mutex));
    spi_rx_sem = osSemaphoreCreate(osSemaphore(spi_rx_sem), 1);
    spi_tx_sem = osSemaphoreCreate(osSemaphore(spi_tx_sem), 1);
    cmddata_rdy_rising_sem = osSemaphoreCreate(osSemaphore(cmddata_rdy_rising_sem), 1);
    /* take semaphore */
    SEM_WAIT(cmddata_rdy_rising_sem, 1);
    SEM_WAIT(spi_rx_sem, 1);
    SEM_WAIT(spi_tx_sem, 1);

#endif
    /* first call used for calibration */
    SPI_WIFI_DelayUs(10);
  }

  rc = SPI_WIFI_ResetModule();

  return rc;
}


static int8_t SPI_WIFI_ResetModule(void)
{
  uint32_t tickstart = HAL_GetTick();
  uint8_t Prompt[6];
  uint8_t count = 0;
  HAL_StatusTypeDef  Status;

  WIFI_RESET_MODULE();
  WIFI_ENABLE_NSS();
  SPI_WIFI_DelayUs(15);

  while (WIFI_IS_CMDDATA_READY())
  {
    Status = HAL_SPI_Receive(&hspi_wifi, &Prompt[count], 1, 0xFFFF);
    count += 2U;
    if (((HAL_GetTick() - tickstart) > 0xFFFFU) || (Status != HAL_OK))
    {
      WIFI_DISABLE_NSS();
      return -1;
    }
  }

  WIFI_DISABLE_NSS();
  if ((Prompt[0] != 0x15U) || (Prompt[1] != 0x15U) || (Prompt[2] != (uint8_t)'\r') ||
      (Prompt[3] != (uint8_t)'\n') || (Prompt[4] != (uint8_t)'>') || (Prompt[5] != (uint8_t)' '))
  {
    return -1;
  }
  return 0;
}

/**
  * @brief  DeInitialize the SPI
  * @param  None
  * @retval None
  */
static int8_t SPI_WIFI_DeInit(void)
{
  (void)HAL_SPI_DeInit(&hspi_wifi);
#ifdef  WIFI_USE_CMSIS_OS
  osMutexDelete(spi_mutex);
  osMutexDelete(es_wifi_mutex);
  osSemaphoreDelete(spi_tx_sem);
  osSemaphoreDelete(spi_rx_sem);
  osSemaphoreDelete(cmddata_rdy_rising_sem);
#endif
  return 0;
}



/**
  * @brief  Receive wifi Data from SPI
  * @param  pdata : pointer to data
  * @param  len : Data length
  * @param  timeout : send timeout in mS
  * @retval Length of received data (payload)
  */

int wait_cmddata_rdy_high(int timeout)
{
  uint32_t tickstart = HAL_GetTick();
  while (WIFI_IS_CMDDATA_READY() == 0)
  {
    if ((HAL_GetTick() - tickstart) > (uint32_t)timeout)
    {
      return -1;
    }
  }
  return 0;
}



int wait_cmddata_rdy_rising_event(int timeout)
{
#ifdef SEM_WAIT
  return SEM_WAIT(cmddata_rdy_rising_sem, timeout);
#else
  uint32_t tickstart = HAL_GetTick();
  while (cmddata_rdy_rising_event == 1)
  {
    if ((HAL_GetTick() - tickstart) > (uint32_t)timeout)
    {
      return -1;
    }
  }
  return 0;
#endif
}

int wait_spi_rx_event(int timeout)
{
#ifdef SEM_WAIT
  return SEM_WAIT(spi_rx_sem, timeout);
#else
  uint32_t tickstart = HAL_GetTick();
  while (spi_rx_event == 1)
  {
    if ((HAL_GetTick() - tickstart) > (uint32_t)timeout)
    {
      return -1;
    }
  }
  return 0;
#endif
}

int wait_spi_tx_event(int timeout)
{
#ifdef SEM_WAIT
  return SEM_WAIT(spi_tx_sem, timeout);
#else
  uint32_t tickstart = HAL_GetTick();
  while (spi_tx_event == 1)
  {
    if ((HAL_GetTick() - tickstart) > (uint32_t)timeout)
    {
      return -1;
    }
  }
  return 0;
#endif
}



int16_t SPI_WIFI_ReceiveData(uint8_t *pData, uint16_t len, uint32_t timeout)
{
  int16_t length = 0;
  uint8_t tmp[2];

  WIFI_DISABLE_NSS();
  UNLOCK_SPI();
  SPI_WIFI_DelayUs(3);


  if (wait_cmddata_rdy_rising_event((int)timeout) < 0)
  {
    return ES_WIFI_ERROR_WAITING_DRDY_FALLING;
  }

  LOCK_SPI();
  WIFI_ENABLE_NSS();
  SPI_WIFI_DelayUs(15);
  while (WIFI_IS_CMDDATA_READY())
  {
    if (((uint16_t)length < len) || (len == 0U))
    {
      spi_rx_event = 1;
      if (HAL_SPI_Receive_IT(&hspi_wifi, tmp, 1) != HAL_OK)
      {
        WIFI_DISABLE_NSS();
        UNLOCK_SPI();
        return ES_WIFI_ERROR_SPI_FAILED;
      }

      (void)wait_spi_rx_event((int)timeout);

      pData[0] = tmp[0];
      pData[1] = tmp[1];
      length += 2;
      pData  += 2;

      if (length >= ES_WIFI_DATA_SIZE)
      {
        WIFI_DISABLE_NSS();
        (void)SPI_WIFI_ResetModule();
        UNLOCK_SPI();
        return ES_WIFI_ERROR_STUFFING_FOREVER;
      }
    }
    else
    {
      break;
    }
  }
  WIFI_DISABLE_NSS();
  UNLOCK_SPI();
  return length;
}
/**
  * @brief  Send wifi Data thru SPI
  * @param  pdata : pointer to data
  * @param  len : Data length
  * @param  timeout : send timeout in mS
  * @retval Length of sent data
  */
int16_t SPI_WIFI_SendData(uint8_t *pdata,  uint16_t len, uint32_t timeout)
{
  uint8_t Padding[2];

  if (wait_cmddata_rdy_high((int)timeout) < 0)
  {
    return ES_WIFI_ERROR_SPI_FAILED;
  }

  /* arm to detect rising event */
  cmddata_rdy_rising_event = 1;
  LOCK_SPI();
  WIFI_ENABLE_NSS();
  SPI_WIFI_DelayUs(15);
  if (len > 1U)
  {
    spi_tx_event = 1;
    if (HAL_SPI_Transmit_IT(&hspi_wifi, (uint8_t *)pdata, len / 2U) != HAL_OK)
    {
      WIFI_DISABLE_NSS();
      UNLOCK_SPI();
      return ES_WIFI_ERROR_SPI_FAILED;
    }
    (void)wait_spi_tx_event((int)timeout);
  }

  if (len & 1U)
  {
    Padding[0] = pdata[len - 1U];
    Padding[1] = (uint8_t)'\n';

    spi_tx_event = 1;
    if (HAL_SPI_Transmit_IT(&hspi_wifi, Padding, 1) != HAL_OK)
    {
      WIFI_DISABLE_NSS();
      UNLOCK_SPI();
      return ES_WIFI_ERROR_SPI_FAILED;
    }
    (void)wait_spi_tx_event((int)timeout);

  }
  return (int16_t)len;
}

/**
   * @brief  Delay
  * @param  Delay in us
  * @retval None
  */
void SPI_WIFI_DelayUs(uint32_t n)
{
  volatile        uint32_t ct = 0;
  uint32_t        loop_per_us;
  static uint32_t cycle_per_loop = 0;

  /* calibration happen on first call for a duration of 1 ms * nbcycle per loop */
  /* 10 cycle for STM32L4 */
  if (cycle_per_loop == 0U)
  {
    uint32_t cycle_per_ms = (SystemCoreClock / 1000UL);
    uint32_t t;
    ct = cycle_per_ms;
    t = HAL_GetTick();
    while (ct != 0U)
    {
      ct--;
    }
    cycle_per_loop = HAL_GetTick() - t;
    if (cycle_per_loop == 0U)
    {
      cycle_per_loop = 1;
    }
  }

  loop_per_us = SystemCoreClock / 1000000UL / cycle_per_loop;
  ct = n * loop_per_us;
  while (ct != 0U)
  {
    ct--;
  }
  return;
}

/**
  * @brief Rx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
  UNUSED(hspi);
  if (spi_rx_event == 1)
  {
    SEM_SIGNAL(spi_rx_sem);
    spi_rx_event = 0;
  }
}

/**
  * @brief Tx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  UNUSED(hspi);
  if (spi_tx_event == 1)
  {
    SEM_SIGNAL(spi_tx_sem);
    spi_tx_event = 0;
  }
}


/**
  * @brief  Interrupt handler for  Data RDY signal
  * @param  None
  * @retval None
  */
void    SPI_WIFI_ISR(void)
{
  if (cmddata_rdy_rising_event == 1)
  {
    SEM_SIGNAL(cmddata_rdy_rising_sem);
    cmddata_rdy_rising_event = 0;
  }
}

/**
  * @brief  probe function to register wifi to connectivity framwotk
  * @param  None
  * @retval None
  */
int32_t wifi_probe(void **ll_drv_context)
{
  if (ES_WIFI_RegisterBusIO(&EsWifiObj,
                            SPI_WIFI_Init,
                            SPI_WIFI_DeInit,
                            HAL_Delay,
                            SPI_WIFI_SendData,
                            SPI_WIFI_ReceiveData) == ES_WIFI_STATUS_OK)
  {
    *ll_drv_context = &EsWifiObj;
    return 0;
  }
  return -1;
}



/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

