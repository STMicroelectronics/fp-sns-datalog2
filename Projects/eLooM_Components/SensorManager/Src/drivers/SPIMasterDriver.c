/**
  ******************************************************************************
  * @file    SPIMasterDriver.c
  * @author  SRA - MCD
  * @brief SPI driver definition.
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

#include "drivers/SPIMasterDriver.h"
#include "drivers/SPIMasterDriver_vtbl.h"
#include "services/sysdebug.h"
#include "drivers/HWDriverMap.h"

#ifndef SPIDRV_CFG_HARDWARE_PERIPHERALS_COUNT
#define SPIDRV_CFG_HARDWARE_PERIPHERALS_COUNT   1
#endif

#define SYS_DEBUGF(level, message)              SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)

/**
  * SPIMasterDriver Driver virtual table.
  */
static const IIODriver_vtbl sSPIMasterDriver_vtbl =
{
  SPIMasterDriver_vtblInit,
  SPIMasterDriver_vtblStart,
  SPIMasterDriver_vtblStop,
  SPIMasterDriver_vtblDoEnterPowerMode,
  SPIMasterDriver_vtblReset,
  SPIMasterDriver_vtblWrite,
  SPIMasterDriver_vtblRead
};

/**
  * Data associated to the hardware peripheral.
  */
typedef struct _SPIPeripheralResources_t
{
  /**
    * Synchronization object used by the driver to synchronize the SPI ISR with the task using the driver.
    */
  TX_SEMAPHORE *sync_obj;

#if (SYS_DBG_ENABLE_TA4 == 1)
  traceHandle m_xSpiTraceHandle;
#endif
} SPIPeripheralResources_t;


static SPIPeripheralResources_t sSPIHwResources[SPIDRV_CFG_HARDWARE_PERIPHERALS_COUNT];
static HWDriverMapElement_t sSPIDrvMapElements[SPIDRV_CFG_HARDWARE_PERIPHERALS_COUNT];
static HWDriverMap_t sSPIDrvMap = { 0 };
static uint8_t sInstances = 0;

/* Private member function declaration */
/***************************************/

/**
  * HAL callback.
  * @param hspi [IN] specifies an handle of an SPI.
  */
static void SPIMasterDriverTxRxCpltCallback(SPI_HandleTypeDef *p_spi);

/* Public API definition */
/*************************/

IIODriver *SPIMasterDriverAlloc(void)
{
  IIODriver *res = NULL;

  if(sSPIDrvMap.size == 0)
  {
    HWDriverMap_Init(&sSPIDrvMap, sSPIDrvMapElements, SPIDRV_CFG_HARDWARE_PERIPHERALS_COUNT);
  }

  HWDriverMapElement_t *p_element = NULL;
  p_element = HWDriverMap_GetFreeElement(&sSPIDrvMap);

  if(p_element != NULL)
  {
    /* Check if there is room to allocate a new instance */
    p_element->p_driver_obj = (IDriver*) SysAlloc(sizeof(SPIMasterDriver_t));

    if(p_element->p_driver_obj == NULL)
    {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SPIMasterDriver - alloc failed.\r\n"));
    }
    else
    {
      p_element->p_driver_obj->vptr = (IDriver_vtbl*) &sSPIMasterDriver_vtbl;
      p_element->p_static_param = (void*) &sSPIHwResources[sInstances];
      sInstances++;
    }
    res = (IIODriver*) p_element->p_driver_obj;
  }
  return res;
}

sys_error_code_t SPIMasterDriver_vtblInit(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  assert_param(p_params != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  UINT nRes = TX_SUCCESS;
  SPIMasterDriver_t *p_obj = (SPIMasterDriver_t *) _this;
  p_obj->mx_handle.p_mx_spi_cfg = ((SPIMasterDriverParams_t *) p_params)->p_mx_spi_cfg;
  SPI_HandleTypeDef *p_spi = p_obj->mx_handle.p_mx_spi_cfg->p_spi_handle;

  p_obj->mx_handle.p_mx_spi_cfg->p_mx_dma_init_f();
  p_obj->mx_handle.p_mx_spi_cfg->p_mx_init_f();

  /* Register SPI DMA complete Callback */
  if (HAL_OK != HAL_SPI_RegisterCallback(p_spi, HAL_SPI_RX_COMPLETE_CB_ID, SPIMasterDriverTxRxCpltCallback))
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }
  else if (HAL_OK != HAL_SPI_RegisterCallback(p_spi, HAL_SPI_TX_COMPLETE_CB_ID, SPIMasterDriverTxRxCpltCallback))
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }
  else
  {
    HWDriverMapElement_t *p_element;
    p_element = HWDriverMap_FindByInstance(&sSPIDrvMap, _this);

    if(p_element == NULL)
    {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }
    else
    {
      nRes = tx_semaphore_create(&p_obj->sync_obj, "SPIDrv", 0);
      if(nRes != TX_SUCCESS)
      {
        SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
        res = SYS_OUT_OF_MEMORY_ERROR_CODE;
      }
      else
      {
        /* Use the peripheral address as unique key for the map */
        p_element->key = (uint32_t) p_obj->mx_handle.p_mx_spi_cfg->p_spi_handle->Instance;

        ((SPIPeripheralResources_t*) p_element->p_static_param)->sync_obj = &p_obj->sync_obj;

        SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SPIMasterDriver: initialization done.\r\n"));
      }
    }
  }

  return res;
}

sys_error_code_t SPIMasterDriver_vtblStart(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SPIMasterDriver_t *p_obj = (SPIMasterDriver_t *) _this;

  /*enable the IRQ*/
  HAL_NVIC_EnableIRQ(p_obj->mx_handle.p_mx_spi_cfg->spi_dma_rx_irq_n);
  HAL_NVIC_EnableIRQ(p_obj->mx_handle.p_mx_spi_cfg->spi_dma_tx_irq_n);

  return res;
}

sys_error_code_t SPIMasterDriver_vtblStop(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SPIMasterDriver_t *p_obj = (SPIMasterDriver_t *) _this;

  /*disable the IRQ*/
  HAL_NVIC_DisableIRQ(p_obj->mx_handle.p_mx_spi_cfg->spi_dma_rx_irq_n);
  HAL_NVIC_DisableIRQ(p_obj->mx_handle.p_mx_spi_cfg->spi_dma_tx_irq_n);

  return res;
}

sys_error_code_t SPIMasterDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode active_power_mode,
                                                      const EPowerMode new_power_mode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  /*SPIMasterDriver *p_obj = (SPIMasterDriver*)_this;*/

  return res;
}

sys_error_code_t SPIMasterDriver_vtblReset(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  /*SPIMasterDriver *p_obj = (SPIMasterDriver*)_this;*/

  return res;
}

sys_error_code_t SPIMasterDriver_vtblWrite(IIODriver *_this, uint8_t *p_data_buffer, uint16_t data_size,
                                           uint16_t channel)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SPIMasterDriver_t *p_obj = (SPIMasterDriver_t *) _this;
  SPI_HandleTypeDef *p_spi = p_obj->mx_handle.p_mx_spi_cfg->p_spi_handle;

  res = SPIMasterDriverTransmitRegAddr(p_obj, (uint8_t)channel, 500);
  if (!SYS_IS_ERROR_CODE(res))
  {
    if (HAL_SPI_Transmit_DMA(p_spi, p_data_buffer, data_size) != HAL_OK)
    {
      if (HAL_SPI_GetError(p_spi) != (uint32_t)HAL_BUSY)
      {
        SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_SPI_M_WRITE_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SPIMasterDriver - Write failed.\r\n"));
      }
    }
    /* Suspend the calling task until the operation is completed.*/
    tx_semaphore_get(&p_obj->sync_obj, TX_WAIT_FOREVER);
  }

  return res;
}

sys_error_code_t SPIMasterDriver_vtblRead(IIODriver *_this, uint8_t *p_data_buffer, uint16_t data_size,
                                          uint16_t channel)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SPIMasterDriver_t *p_obj = (SPIMasterDriver_t *) _this;
  SPI_HandleTypeDef *p_spi = p_obj->mx_handle.p_mx_spi_cfg->p_spi_handle;

  res = SPIMasterDriverTransmitRegAddr(p_obj, (uint8_t)channel, 500);
  if (!SYS_IS_ERROR_CODE(res))
  {
    if (HAL_SPI_Receive_DMA(p_spi, p_data_buffer, data_size) != HAL_OK)
    {
      if (HAL_SPI_GetError(p_spi) != (uint32_t)HAL_BUSY)
      {
        SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_SPI_M_READ_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SPIMasterDriver - Read failed.\r\n"));
      }
    }
    /* Suspend the calling task until the operation is completed.*/
    tx_semaphore_get(&p_obj->sync_obj, TX_WAIT_FOREVER);
  }

  return res;
}

sys_error_code_t SPIMasterDriverTransmitRegAddr(SPIMasterDriver_t *_this, uint8_t reg_addr, uint32_t timeout_ms)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SPIMasterDriver_t *p_obj = (SPIMasterDriver_t *) _this;
  SPI_HandleTypeDef *p_spi = p_obj->mx_handle.p_mx_spi_cfg->p_spi_handle;

  if (HAL_OK != HAL_SPI_Transmit(p_spi, &reg_addr, 1, timeout_ms))
  {
    res = SYS_SPI_M_WRITE_ERROR_CODE;
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_SPI_M_WRITE_ERROR_CODE);
    /* block the application*/
    sys_error_handler();
  }

  return res;
}

sys_error_code_t SPIMasterDriverWriteRead(SPIMasterDriver_t *_this, uint8_t *p_tx_data_buffer,
                                          uint8_t *p_rx_data_buffer, uint16_t data_size)
{
  assert_param(_this != NULL);
  assert_param(p_tx_data_buffer != NULL);
  assert_param(p_tx_data_buffer != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SPIMasterDriver_t *p_obj = (SPIMasterDriver_t *) _this;
  SPI_HandleTypeDef *p_spi = p_obj->mx_handle.p_mx_spi_cfg->p_spi_handle;

  if (HAL_SPI_TransmitReceive_DMA(p_spi, p_tx_data_buffer, p_rx_data_buffer, data_size) != HAL_OK)
  {
    if (HAL_SPI_GetError(p_spi) != (uint32_t)HAL_BUSY)
    {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_SPI_M_WRITE_READ_ERROR_CODE);
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SPIMasterDriver - TransmitReceive failed.\r\n"));
    }
  }

  /* Suspend the calling task until the operation is completed.*/
  tx_semaphore_get(&p_obj->sync_obj, TX_WAIT_FOREVER);

  return res;
}

sys_error_code_t SPIMasterDriverSelectDevice(SPIMasterDriver_t *_this, GPIO_TypeDef *p_device_gpio_port,
                                             uint16_t device_gpio_pin)
{
  /* this is a class method so pointer _this is not used*/
  UNUSED(_this);

  HAL_GPIO_WritePin(p_device_gpio_port, device_gpio_pin, GPIO_PIN_RESET);

  return SYS_NO_ERROR_CODE;
}

sys_error_code_t SPIMasterDriverDeselectDevice(SPIMasterDriver_t *_this, GPIO_TypeDef *device_gpio_port,
                                               uint16_t device_gpio_pin)
{
  /* this is a class method so pointer _this is not used*/
  UNUSED(_this);

  HAL_GPIO_WritePin(device_gpio_port, device_gpio_pin, GPIO_PIN_SET);

  return SYS_NO_ERROR_CODE;
}

/* Private function definition */
/*******************************/

/* CubeMX integration */
/**********************/

static void SPIMasterDriverTxRxCpltCallback(SPI_HandleTypeDef *p_spi)
{
  HWDriverMapElement_t *p_element;
  TX_SEMAPHORE *sync_obj;

  p_element = HWDriverMap_FindByKey(&sSPIDrvMap, (uint32_t) p_spi->Instance);

  if(p_element != NULL)
  {
#if (SYS_DBG_ENABLE_TA4 == 1)
    if (xTraceIsRecordingEnabled())
    {
      vTraceStoreISRBegin(((SPIPeripheralResources_t*) p_element->p_static_param)->m_xSpiTraceHandle);
    }
#endif

    sync_obj = ((SPIPeripheralResources_t*) p_element->p_static_param)->sync_obj;

    if(sync_obj != NULL)
    {
      tx_semaphore_put(sync_obj);
    }

#if (SYS_DBG_ENABLE_TA4 == 1)
    if (xTraceIsRecordingEnabled())
    {
      vTraceStoreISREnd(0);
    }
#endif
  }
}
