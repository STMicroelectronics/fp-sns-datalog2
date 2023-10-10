/**
  ******************************************************************************
  * @file    I2CMasterDriver.c
  * @author  SRA - MCD
  * @brief   I2C driver definition.
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

#include "drivers/I2CMasterDriver.h"
#include "drivers/I2CMasterDriver_vtbl.h"
#include "services/sysdebug.h"
#include "SensorManager.h"
#include "drivers/HWDriverMap.h"

#ifndef I2CDRV_CFG_HARDWARE_PERIPHERALS_COUNT
#define I2CDRV_CFG_HARDWARE_PERIPHERALS_COUNT   1
#endif

#define SYS_DEBUGF(level, message)              SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)


/**
  * Class object declaration
  */
typedef struct _I2CMasterDriverClass
{
  /**
    * I2CMasterDriver class virtual table.
    */
  const IIODriver_vtbl vtbl;

  /**
    * Memory buffer used to allocate the map (hardware IP, eLoom driver).
    */
  HWDriverMapElement_t ip_drv_map_elements[I2CDRV_CFG_HARDWARE_PERIPHERALS_COUNT];

  /**
    * This map is used to link an hardware I2C with an instance of the driver object. The key of the map is the address of the hardware IP.
    */
  HWDriverMap_t ip_drv_map;

} I2CMasterDriverClass_t;


/* Private objects definition.*/
/******************************/

/**
  * The only class instance.
  */
static I2CMasterDriverClass_t sTheClass =
{
  /*
   * vtbl
   */
  {
    I2CMasterDriver_vtblInit,
    I2CMasterDriver_vtblStart,
    I2CMasterDriver_vtblStop,
    I2CMasterDriver_vtblDoEnterPowerMode,
    I2CMasterDriver_vtblReset,
    I2CMasterDriver_vtblWrite,
    I2CMasterDriver_vtblRead
  },

  {{0}}, /* ip_drv_map_elements */
  {0}  /* ip_drv_map */
};


/* Private member function declaration */
/***************************************/

static void I2CMasterDrvMemTxRxCpltCallback(I2C_HandleTypeDef *p_i2c);
static void I2CMasterDrvErrorCallback(I2C_HandleTypeDef *p_i2c);

/* Public API definition */
/*************************/

sys_error_code_t I2CMasterDriverSetDeviceAddr(I2CMasterDriver_t *_this, uint16_t address)
{
  assert_param(_this);

  _this->target_device_addr = address;

  return SYS_NO_ERROR_CODE;
}

sys_error_code_t I2CMasterDriverSetAddrSize(I2CMasterDriver_t *_this, uint8_t nAddressSize)
{
  assert_param(_this);

  _this->address_size = nAddressSize;

  return SYS_NO_ERROR_CODE;
}

/* IIODriver virtual function definition */
/*****************************************/

IIODriver *I2CMasterDriverAlloc(void)
{
  IIODriver *p_new_driver = (IIODriver *) SysAlloc(sizeof(I2CMasterDriver_t));;

  if (p_new_driver == NULL)
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("I2CMasterDriver - alloc failed.\r\n"));
  }
  else
  {
    p_new_driver->vptr = &sTheClass.vtbl;
  }

  return p_new_driver;
}

sys_error_code_t I2CMasterDriver_vtblInit(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  assert_param(p_params != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  UINT nRes = TX_SUCCESS;
  I2CMasterDriver_t *p_obj = (I2CMasterDriver_t *) _this;
  p_obj->mx_handle.p_mx_i2c_cfg = ((I2CMasterDriverParams_t *) p_params)->p_mx_i2c_cfg;
  I2C_HandleTypeDef *p_i2c = p_obj->mx_handle.p_mx_i2c_cfg->p_i2c_handle;

  p_obj->mx_handle.p_mx_i2c_cfg->p_mx_dma_init_f();
  p_obj->mx_handle.p_mx_i2c_cfg->p_mx_init_f();

  /* Register I2C DMA complete Callback*/
  if (HAL_OK != HAL_I2C_RegisterCallback(p_i2c, HAL_I2C_MEM_RX_COMPLETE_CB_ID, I2CMasterDrvMemTxRxCpltCallback))
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }
  else if (HAL_OK != HAL_I2C_RegisterCallback(p_i2c, HAL_I2C_MEM_TX_COMPLETE_CB_ID, I2CMasterDrvMemTxRxCpltCallback))
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }
  else if (HAL_OK != HAL_I2C_RegisterCallback(p_i2c, HAL_I2C_ERROR_CB_ID, I2CMasterDrvErrorCallback))
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }
  else
  {
    if (!HWDriverMap_IsInitialized(&sTheClass.ip_drv_map))
    {
      (void) HWDriverMap_Init(&sTheClass.ip_drv_map, sTheClass.ip_drv_map_elements, I2CDRV_CFG_HARDWARE_PERIPHERALS_COUNT);
    }

    /* Add the driver to the map.
     * Use the peripheral address as unique key for the map. */
    HWDriverMapElement_t *p_element = NULL;
    uint32_t key = (uint32_t) p_obj->mx_handle.p_mx_i2c_cfg->p_i2c_handle->Instance;
    p_element = HWDriverMap_AddElement(&sTheClass.ip_drv_map, key, _this);

    if (p_element == NULL)
    {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }
    else
    {
      nRes = tx_semaphore_create(&p_obj->sync_obj, "I2CDrv", 0);
      if (nRes != TX_SUCCESS)
      {
        SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
        res = SYS_OUT_OF_MEMORY_ERROR_CODE;

        (void) HWDriverMap_RemoveElement(&sTheClass.ip_drv_map, key);
      }
    }
  }

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("I2CMasterDriver: initialization done: %d.\r\n", res));

  return res;
}

sys_error_code_t I2CMasterDriver_vtblStart(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  I2CMasterDriver_t *p_obj = (I2CMasterDriver_t *) _this;

  /* I2C interrupt enable */
  HAL_NVIC_EnableIRQ(p_obj->mx_handle.p_mx_i2c_cfg->i2c_ev_irq_n);
  HAL_NVIC_EnableIRQ(p_obj->mx_handle.p_mx_i2c_cfg->i2c_er_irq_n);

  /* DMA RX and TX Channels IRQn interrupt enable */
  HAL_NVIC_EnableIRQ(p_obj->mx_handle.p_mx_i2c_cfg->i2c_dma_rx_irq_n);
  HAL_NVIC_EnableIRQ(p_obj->mx_handle.p_mx_i2c_cfg->i2c_dma_tx_irq_n);

  return res;
}

sys_error_code_t I2CMasterDriver_vtblStop(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  I2CMasterDriver_t *p_obj = (I2CMasterDriver_t *) _this;

  /* I2C interrupt disable */
  HAL_NVIC_DisableIRQ(p_obj->mx_handle.p_mx_i2c_cfg->i2c_ev_irq_n);
  HAL_NVIC_DisableIRQ(p_obj->mx_handle.p_mx_i2c_cfg->i2c_er_irq_n);

  /* DMA RX and TX Channels IRQn interrupt disable */
  HAL_NVIC_DisableIRQ(p_obj->mx_handle.p_mx_i2c_cfg->i2c_dma_rx_irq_n);
  HAL_NVIC_DisableIRQ(p_obj->mx_handle.p_mx_i2c_cfg->i2c_dma_tx_irq_n);

  return res;
}

sys_error_code_t I2CMasterDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode active_power_mode,
                                                      const EPowerMode new_powerMode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  /*I2CMasterDriver_t *p_obj = (I2CMasterDriver_t*)_this;*/

  return res;
}

sys_error_code_t I2CMasterDriver_vtblReset(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  /*I2CMasterDriver_t *p_obj = (I2CMasterDriver_t*)_this;*/

  return res;
}

sys_error_code_t I2CMasterDriver_vtblWrite(IIODriver *_this, uint8_t *p_data_buffer, uint16_t data_size,
                                           uint16_t channel)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  I2CMasterDriver_t *p_obj = (I2CMasterDriver_t *) _this;
  I2C_HandleTypeDef *p_i2c = p_obj->mx_handle.p_mx_i2c_cfg->p_i2c_handle;

  if (HAL_I2C_Mem_Write_DMA(p_i2c, p_obj->target_device_addr, channel, p_obj->address_size, p_data_buffer,
                            data_size) != HAL_OK)
  {
    if (HAL_I2C_GetError(p_i2c) != (uint32_t)HAL_BUSY)
    {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_I2C_M_WRITE_ERROR_CODE);
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("I2CMasterDriver - Write failed.\r\n"));
    }
  }
  /* Suspend the calling task until the operation is completed.*/
  tx_semaphore_get(&p_obj->sync_obj, TX_WAIT_FOREVER);

  return res;
}

sys_error_code_t I2CMasterDriver_vtblRead(IIODriver *_this, uint8_t *p_data_buffer, uint16_t data_size,
                                          uint16_t channel)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  I2CMasterDriver_t *p_obj = (I2CMasterDriver_t *) _this;
  I2C_HandleTypeDef *p_i2c = p_obj->mx_handle.p_mx_i2c_cfg->p_i2c_handle;

  if (HAL_I2C_Mem_Read_DMA(p_i2c, p_obj->target_device_addr, channel, p_obj->address_size, p_data_buffer,
                           data_size) != HAL_OK)
  {
    if (HAL_I2C_GetError(p_i2c) != (uint32_t)HAL_BUSY)
    {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_I2C_M_READ_ERROR_CODE);
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("I2CMasterDriver - Read failed.\r\n"));
    }
  }
  /* Suspend the calling task until the operation is completed.*/
  tx_semaphore_get(&p_obj->sync_obj, TX_WAIT_FOREVER);

  return res;
}

/* Private function definition */
/*******************************/

/* CubeMX integration */
/**********************/

static void I2CMasterDrvMemTxRxCpltCallback(I2C_HandleTypeDef *p_i2c)
{
  HWDriverMapValue_t *p_val;
  TX_SEMAPHORE *sync_obj;

  p_val = HWDriverMap_FindByKey(&sTheClass.ip_drv_map, (uint32_t) p_i2c->Instance);

  if (p_val != NULL)
  {
    sync_obj = &((I2CMasterDriver_t *)p_val->p_driver_obj)->sync_obj;

    if (sync_obj != NULL)
    {
      tx_semaphore_put(sync_obj);
    }
  }
}


static void I2CMasterDrvErrorCallback(I2C_HandleTypeDef *p_i2c)
{
  UNUSED(p_i2c);
}
