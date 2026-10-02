/**
  ******************************************************************************
  * @file    I3CMasterDriver.c
  * @author  SRA - MCD
  * @brief I3C driver definition.
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

#include "drivers/I3CMasterDriver.h"
#include "drivers/I3CMasterDriver_vtbl.h"
#include "services/sysdebug.h"
#include "SensorManager.h"
#include "drivers/HWDriverMap.h"

#ifndef I3CDRV_CFG_HARDWARE_PERIPHERALS_COUNT
#define I3CDRV_CFG_HARDWARE_PERIPHERALS_COUNT   1
#endif

#define I3C_CCC_DISEC                           0x01U
#define I3C_CCC_RSTDAA                          0x06U
#define I3C_CCC_SETDASA                         0x87U
#define I3C_CCC_DISEC_HOT_JOIN_MASK             0x08U
#define COUNTOF(__BUFFER__)                     (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
#define I3C_DRV_OP_TIMEOUT_MS                   1000U
#define I3C_MAX_DMA_FRAME_SIZE                  32U
#define I3C_REG_ADDR_SIZE_8BIT                  1U
#define I3C_REG_ADDR_SIZE_16BIT                 2U

#define SYS_DEBUGF(level, message)              SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)


/**
  * Class object declaration
  */
typedef struct _I3CMasterDriverClass
{
  /**
    * I3CMasterDriver class virtual table.
    */
  const IIODriver_vtbl vtbl;

  /**
    * Memory buffer used to allocate the map (hardware IP, eLoom driver).
    */
  HWDriverMapElement_t ip_drv_map_elements[I3CDRV_CFG_HARDWARE_PERIPHERALS_COUNT];

  /**
    * This map is used to link an hardware I3C with an instance of the driver object. The key of the map is the address of the hardware IP.
    */
  HWDriverMap_t ip_drv_map;

} I3CMasterDriverClass_t;


/* Private objects definition.*/
/******************************/

/**
  * The only class instance.
  */
static I3CMasterDriverClass_t sTheClass =
{
  /*
   * vtbl
   */
  {
    I3CMasterDriver_vtblInit,
    I3CMasterDriver_vtblStart,
    I3CMasterDriver_vtblStop,
    I3CMasterDriver_vtblDoEnterPowerMode,
    I3CMasterDriver_vtblReset,
    I3CMasterDriver_vtblWrite,
    I3CMasterDriver_vtblRead
  },

  {{0}}, /* ip_drv_map_elements */
  {0}  /* ip_drv_map */
};

static uint8_t s_disec_done = 0U;
static uint8_t s_rstdaa_done = 0U;


/* Private member function declaration */
/***************************************/

/**
  * HAL callback.
  * @param hi3c [IN] specifies an handle of an I3C.
  */
static void I3CMasterDriverTxRxCpltCallback(I3C_HandleTypeDef *p_i3c);

/**
  * Align I3C DMA channels configuration with current FIFO thresholds.
  */
static void I3CMasterDriverSyncDMAConfig(I3C_HandleTypeDef *p_i3c);

/**
  * Transmit one CCC descriptor in blocking mode.
  */
static sys_error_code_t I3CMasterDriverTransmitCCC(I3CMasterDriver_t *_this, uint8_t target_addr, uint8_t ccc,
                                                   uint8_t *p_payload, uint16_t payload_size, uint32_t transfer_mode);

/* Public API definition */
/*************************/

IIODriver *I3CMasterDriverAlloc(void)
{
  IIODriver *p_new_driver = (IIODriver *) SysAlloc(sizeof(I3CMasterDriver_t));
  if (p_new_driver == NULL)
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("I3CMasterDriver - alloc failed.\r\n"));
  }
  else
  {
    p_new_driver->vptr = &sTheClass.vtbl;
  }

  return p_new_driver;
}

sys_error_code_t I3CMasterDriver_vtblInit(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  assert_param(p_params != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  UINT nRes = TX_SUCCESS;
  I3CMasterDriver_t *p_obj = (I3CMasterDriver_t *) _this;
  p_obj->mx_handle.p_mx_i3c_cfg = ((I3CMasterDriverParams_t *) p_params)->p_mx_i3c_cfg;
  I3C_HandleTypeDef *p_i3c = p_obj->mx_handle.p_mx_i3c_cfg->p_i3c_handle;

  p_obj->mx_handle.p_mx_i3c_cfg->p_mx_dma_init_f();
  p_obj->mx_handle.p_mx_i3c_cfg->p_mx_init_f();
  p_obj->reg_addr_size = I3C_REG_ADDR_SIZE_8BIT;

  I3CMasterDriverSyncDMAConfig(p_i3c);

  /* Register I3C DMA complete Callback */
  if (HAL_OK != HAL_I3C_RegisterCallback(p_i3c, HAL_I3C_CTRL_RX_COMPLETE_CB_ID, I3CMasterDriverTxRxCpltCallback))
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }
  else if (HAL_OK != HAL_I3C_RegisterCallback(p_i3c, HAL_I3C_CTRL_TX_COMPLETE_CB_ID, I3CMasterDriverTxRxCpltCallback))
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }
  else
  {
    if (!HWDriverMap_IsInitialized(&sTheClass.ip_drv_map))
    {
      (void) HWDriverMap_Init(&sTheClass.ip_drv_map, sTheClass.ip_drv_map_elements, I3CDRV_CFG_HARDWARE_PERIPHERALS_COUNT);
    }

    /* Add the driver to the map.
     * Use the peripheral address as unique key for the map. */
    HWDriverMapElement_t *p_element = NULL;
    uint32_t key = (uint32_t) p_obj->mx_handle.p_mx_i3c_cfg->p_i3c_handle->Instance;
    p_element = HWDriverMap_AddElement(&sTheClass.ip_drv_map, key, _this);


    if (p_element == NULL)
    {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
    }
    else
    {
      nRes = tx_semaphore_create(&p_obj->sync_obj, "I3CDrv", 0);
      if (nRes != TX_SUCCESS)
      {
        SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
        res = SYS_OUT_OF_MEMORY_ERROR_CODE;

        (void) HWDriverMap_RemoveElement(&sTheClass.ip_drv_map, key);
      }
    }
  }

  SYS_DEBUGF(SYS_DBG_LEVEL_DEFAULT, ("I3CMasterDriver: initialization done: %d.\r\n", res));

  return res;
}

sys_error_code_t I3CMasterDriver_vtblStart(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  I3CMasterDriver_t *p_obj = (I3CMasterDriver_t *) _this;

  /* I3C interrupt enable */
  HAL_NVIC_EnableIRQ(p_obj->mx_handle.p_mx_i3c_cfg->i3c_ev_irq_n);
  HAL_NVIC_EnableIRQ(p_obj->mx_handle.p_mx_i3c_cfg->i3c_er_irq_n);

  /* DMA RX and TX Channels IRQn interrupt enable */
  HAL_NVIC_EnableIRQ(p_obj->mx_handle.p_mx_i3c_cfg->i3c_dma_tc_irq_n);
  HAL_NVIC_EnableIRQ(p_obj->mx_handle.p_mx_i3c_cfg->i3c_dma_rx_irq_n);
  HAL_NVIC_EnableIRQ(p_obj->mx_handle.p_mx_i3c_cfg->i3c_dma_tx_irq_n);

  return res;
}

sys_error_code_t I3CMasterDriver_vtblStop(IDriver *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  I3CMasterDriver_t *p_obj = (I3CMasterDriver_t *) _this;

  /* I3C interrupt disable */
  HAL_NVIC_DisableIRQ(p_obj->mx_handle.p_mx_i3c_cfg->i3c_ev_irq_n);
  HAL_NVIC_DisableIRQ(p_obj->mx_handle.p_mx_i3c_cfg->i3c_er_irq_n);

  /* DMA RX and TX Channels IRQn interrupt disable */
  HAL_NVIC_DisableIRQ(p_obj->mx_handle.p_mx_i3c_cfg->i3c_dma_tc_irq_n);
  HAL_NVIC_DisableIRQ(p_obj->mx_handle.p_mx_i3c_cfg->i3c_dma_rx_irq_n);
  HAL_NVIC_DisableIRQ(p_obj->mx_handle.p_mx_i3c_cfg->i3c_dma_tx_irq_n);

  return res;
}

sys_error_code_t I3CMasterDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode active_power_mode,
                                                      const EPowerMode new_power_mode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  /*I3CMasterDriver *p_obj = (I3CMasterDriver*)_this;*/

  return res;
}

sys_error_code_t I3CMasterDriver_vtblReset(IDriver *_this, void *p_params)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  /*I3CMasterDriver *p_obj = (I3CMasterDriver*)_this;*/

  return res;
}

sys_error_code_t I3CMasterDriver_vtblWrite(IIODriver *_this, uint8_t *p_data_buffer, uint16_t data_size,
                                           uint16_t channel)
{
  assert_param(_this != NULL);
  assert_param(p_data_buffer != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  I3CMasterDriver_t *p_obj = (I3CMasterDriver_t *) _this;
  I3C_HandleTypeDef *p_i3c = p_obj->mx_handle.p_mx_i3c_cfg->p_i3c_handle;

  /* Context buffer related to Frame context, contain different buffer value for a communication */
  I3C_XferTypeDef contextBuffer;
  /* Buffer used by HAL to compute control data for the Private Communication */
  uint32_t controlBuffer[0xF];
  I3C_PrivateTypeDef privateDescriptor;
  uint8_t data[32] = {0};
  uint8_t reg_addr_size = p_obj->reg_addr_size;
  uint16_t bytes_remaining = data_size;
  uint16_t bytes_written = 0U;
  uint16_t chunk_size;
  uint16_t chunk_reg;
  uint16_t max_chunk_payload;

  if ((reg_addr_size != I3C_REG_ADDR_SIZE_8BIT) && (reg_addr_size != I3C_REG_ADDR_SIZE_16BIT))
  {
    reg_addr_size = I3C_REG_ADDR_SIZE_8BIT;
  }

  max_chunk_payload = (uint16_t)(sizeof(data) - reg_addr_size);
  if (max_chunk_payload == 0U)
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_I3C_M_WRITE_ERROR_CODE);
    return SYS_I3C_M_WRITE_ERROR_CODE;
  }

  while ((bytes_remaining > 0U) && !SYS_IS_ERROR_CODE(res))
  {
    chunk_size = (bytes_remaining > max_chunk_payload) ? max_chunk_payload : bytes_remaining;
    chunk_reg = (uint16_t)(channel + bytes_written);

    if (reg_addr_size == I3C_REG_ADDR_SIZE_16BIT)
    {
      data[0] = (uint8_t)(chunk_reg >> 8);
      data[1] = (uint8_t)chunk_reg;
      memcpy(&data[2], &p_data_buffer[bytes_written], chunk_size);
    }
    else
    {
      data[0] = (uint8_t)chunk_reg;
      memcpy(&data[1], &p_data_buffer[bytes_written], chunk_size);
    }

    privateDescriptor.TargetAddr = p_obj->target_device_addr;
    privateDescriptor.TxBuf.pBuffer = data;
    privateDescriptor.TxBuf.Size = (reg_addr_size + chunk_size);
    privateDescriptor.RxBuf.pBuffer = NULL;
    privateDescriptor.RxBuf.Size = 0;
    privateDescriptor.Direction = HAL_I3C_DIRECTION_WRITE;

    memset((void *)&contextBuffer, 0x0, sizeof(I3C_XferTypeDef));
    contextBuffer.CtrlBuf.pBuffer = controlBuffer;
    contextBuffer.CtrlBuf.Size    = 1;
    contextBuffer.TxBuf.pBuffer   = data;
    contextBuffer.TxBuf.Size      = (reg_addr_size + chunk_size);

    res = I3CMasterDriverTransmitRegAddr(p_obj, &contextBuffer, &privateDescriptor, 500);
    if (SYS_IS_ERROR_CODE(res))
    {
      break;
    }

    if (HAL_I3C_Ctrl_Transmit_DMA(p_i3c, &contextBuffer) != HAL_OK)
    {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_I3C_M_WRITE_ERROR_CODE);
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("I3CMasterDriver - Write failed. size=%u offset=%u\r\n",
                                         chunk_size,
                                         bytes_written));
      res = SYS_I3C_M_WRITE_ERROR_CODE;
    }
    else if (TX_SUCCESS != tx_semaphore_get(&p_obj->sync_obj, TX_WAIT_FOREVER))
    {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_I3C_M_WRITE_ERROR_CODE);
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("I3CMasterDriver - Write timeout. size=%u offset=%u\r\n",
                                         chunk_size,
                                         bytes_written));
      res = SYS_I3C_M_WRITE_ERROR_CODE;
    }
    else
    {
      bytes_written += chunk_size;
      bytes_remaining -= chunk_size;
    }
  }

  return res;
}

sys_error_code_t I3CMasterDriver_vtblRead(IIODriver *_this, uint8_t *p_data_buffer, uint16_t data_size,
                                          uint16_t channel)
{
  assert_param(_this != NULL);
  assert_param(p_data_buffer != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  I3CMasterDriver_t *p_obj = (I3CMasterDriver_t *) _this;
  I3C_HandleTypeDef *p_i3c = p_obj->mx_handle.p_mx_i3c_cfg->p_i3c_handle;

  I3C_XferTypeDef contextBuffer;
  uint32_t controlBuffer[0xF];
  I3C_PrivateTypeDef privateDescriptor;
  uint8_t myReg[2] = {0};
  uint16_t bytes_remaining = data_size;
  uint16_t bytes_read = 0U;
  uint16_t chunk_size;
  uint8_t reg_addr_size = p_obj->reg_addr_size;

  if ((reg_addr_size != I3C_REG_ADDR_SIZE_8BIT) && (reg_addr_size != I3C_REG_ADDR_SIZE_16BIT))
  {
    reg_addr_size = I3C_REG_ADDR_SIZE_8BIT;
  }

  if (reg_addr_size == I3C_REG_ADDR_SIZE_16BIT)
  {
    myReg[0] = (uint8_t)(channel >> 8);
    myReg[1] = (uint8_t)channel;
  }
  else
  {
    myReg[0] = (uint8_t)channel;
  }

  privateDescriptor.TargetAddr = p_obj->target_device_addr;
  privateDescriptor.TxBuf.pBuffer = myReg;
  privateDescriptor.TxBuf.Size = reg_addr_size;
  privateDescriptor.RxBuf.pBuffer = NULL;
  privateDescriptor.RxBuf.Size = 0;
  privateDescriptor.Direction = HAL_I3C_DIRECTION_WRITE;

  memset((void *)&contextBuffer, 0x0, sizeof(I3C_XferTypeDef));
  contextBuffer.CtrlBuf.pBuffer = controlBuffer;
  contextBuffer.CtrlBuf.Size    = 1;
  contextBuffer.TxBuf.pBuffer   = myReg;
  contextBuffer.TxBuf.Size      = reg_addr_size;

  res = I3CMasterDriverTransmitRegAddr(p_obj, &contextBuffer, &privateDescriptor, 500);
  if (!SYS_IS_ERROR_CODE(res))
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("I3CMasterDriver - About to TX reg addr: target=0x%02x, reg=0x%04x, size=%u\r\n",
                                       privateDescriptor.TargetAddr, channel, reg_addr_size));
    if (HAL_I3C_Ctrl_Transmit(p_i3c, &contextBuffer, 500) != HAL_OK)
    {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_I3C_M_WRITE_ERROR_CODE);
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("I3CMasterDriver - Read preamble failed. err=0x%08lx state=%lu\r\n",
                                         (unsigned long)HAL_I3C_GetError(p_i3c), (unsigned long)HAL_I3C_GetState(p_i3c)));
      res = SYS_I3C_M_WRITE_ERROR_CODE;
    }
    else
    {
      /* Read data with automatic chunking for transfers > 32 bytes */
      while ((bytes_remaining > 0U) && !SYS_IS_ERROR_CODE(res))
      {
        /* Calculate chunk size (limit to max DMA frame size) */
        chunk_size = (bytes_remaining > I3C_MAX_DMA_FRAME_SIZE) ?
                     I3C_MAX_DMA_FRAME_SIZE : bytes_remaining;

        privateDescriptor.TargetAddr = p_obj->target_device_addr;
        privateDescriptor.TxBuf.pBuffer = NULL;
        privateDescriptor.TxBuf.Size = 0;
        privateDescriptor.RxBuf.pBuffer = &p_data_buffer[bytes_read];
        privateDescriptor.RxBuf.Size = chunk_size;
        privateDescriptor.Direction = HAL_I3C_DIRECTION_READ;

        memset((void *)&contextBuffer, 0x0, sizeof(I3C_XferTypeDef));
        contextBuffer.CtrlBuf.pBuffer = controlBuffer;
        contextBuffer.CtrlBuf.Size    = 1;
        contextBuffer.RxBuf.pBuffer   = &p_data_buffer[bytes_read];
        contextBuffer.RxBuf.Size      = chunk_size;

        if (HAL_I3C_AddDescToFrame(p_i3c,
                                   NULL,
                                   &privateDescriptor,
                                   &contextBuffer,
                                   contextBuffer.CtrlBuf.Size,
                                   I3C_PRIVATE_WITH_ARB_STOP) != HAL_OK)
        {
          res = SYS_I3C_M_WRITE_ERROR_CODE;
          SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_I3C_M_WRITE_ERROR_CODE);
          SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("I3CMasterDriver - Read frame add failed. size=%u\r\n", chunk_size));
        }
        else if (HAL_I3C_Ctrl_Receive_DMA(p_i3c, &contextBuffer) != HAL_OK)
        {
#ifdef SYS_DEBUG
          uint32_t hal_error = HAL_I3C_GetError(p_i3c);
          uint32_t hal_state = (uint32_t)HAL_I3C_GetState(p_i3c);
          SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("I3CMasterDriver - Read DMA start failed. size=%u err=0x%08lx state=%lu\r\n",
                                             chunk_size,
                                             (unsigned long)hal_error,
                                             (unsigned long)hal_state));
#endif
          SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_I3C_M_READ_ERROR_CODE);
          res = SYS_I3C_M_READ_ERROR_CODE;
        }

        if (!SYS_IS_ERROR_CODE(res))
        {
          if (TX_SUCCESS != tx_semaphore_get(&p_obj->sync_obj, TX_WAIT_FOREVER))
          {
            SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_I3C_M_READ_ERROR_CODE);
            SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("I3CMasterDriver - Read timeout. size=%u offset=%u\r\n",
                                               chunk_size, bytes_read));
            res = SYS_I3C_M_READ_ERROR_CODE;
          }
          else
          {
            /* Chunk transferred successfully, move to next chunk */
            bytes_read += chunk_size;
            bytes_remaining -= chunk_size;
          }
        }
      }
    }
  }

  return res;
}

sys_error_code_t I3CMasterDriverTransmitRegAddr(I3CMasterDriver_t *_this, I3C_XferTypeDef *contextBuffer,
                                                I3C_PrivateTypeDef *privateDescriptor, uint32_t timeout_ms)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  I3CMasterDriver_t *p_obj = (I3CMasterDriver_t *) _this;
  I3C_HandleTypeDef *p_i3c = p_obj->mx_handle.p_mx_i3c_cfg->p_i3c_handle;

  if (HAL_I3C_AddDescToFrame(p_i3c,
                             NULL,
                             privateDescriptor,
                             contextBuffer,
                             contextBuffer->CtrlBuf.Size,
                             I3C_PRIVATE_WITH_ARB_STOP) != HAL_OK)
  {
    res = SYS_I3C_M_WRITE_ERROR_CODE;
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_I3C_M_WRITE_ERROR_CODE);
    /* block the application*/
    sys_error_handler();
  }

  return res;
}

sys_error_code_t I3CMasterDriverSetDeviceAddr(I3CMasterDriver_t *_this, uint16_t address)
{
  assert_param(_this);

  _this->target_device_addr = address;
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("I3CMasterDriver_SetDeviceAddr: Set to 0x%02x\r\n", address & 0xFF));

  return SYS_NO_ERROR_CODE ;
}

sys_error_code_t I3CMasterDriverSetRegAddrSize(I3CMasterDriver_t *_this, uint8_t reg_addr_size)
{
  assert_param(_this);

  if ((reg_addr_size != I3C_REG_ADDR_SIZE_8BIT) && (reg_addr_size != I3C_REG_ADDR_SIZE_16BIT))
  {
    return SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  _this->reg_addr_size = reg_addr_size;

  return SYS_NO_ERROR_CODE;
}

sys_error_code_t I3CMasterDriverConfigureTarget(I3CMasterDriver_t *_this, uint16_t static_address,
                                                uint16_t dynamic_address)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint8_t static_addr = (uint8_t)(static_address); // & 0x7FU);
  uint8_t dynamic_addr = (uint8_t)(dynamic_address); // & 0x7FU);

  if ((static_addr == 0U) && (dynamic_addr == 0U))
  {
    return SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  if (!s_disec_done)
  {
    uint8_t disec_data = I3C_CCC_DISEC_HOT_JOIN_MASK;
    res = I3CMasterDriverTransmitCCC(_this, 0x00U, I3C_CCC_DISEC, &disec_data, 1U, I3C_BROADCAST_WITHOUT_DEFBYTE_RESTART);
    if (SYS_IS_ERROR_CODE(res))
    {
      return res;
    }
    s_disec_done = 1U;
  }

  if (!s_rstdaa_done)
  {
    res = I3CMasterDriverTransmitCCC(_this, 0x00U, I3C_CCC_RSTDAA, NULL, 0U, I3C_BROADCAST_WITHOUT_DEFBYTE_RESTART);
    if (SYS_IS_ERROR_CODE(res))
    {
      return res;
    }
    s_rstdaa_done = 1U;
  }

  if ((static_addr != 0U) && (dynamic_addr != 0U))
  {
    uint8_t dyn_addr_payload = (uint8_t)(dynamic_addr << 1);
    res = I3CMasterDriverTransmitCCC(_this, static_addr, I3C_CCC_SETDASA, &dyn_addr_payload, 1U,
                                     I3C_DIRECT_WITHOUT_DEFBYTE_RESTART);
    if (SYS_IS_ERROR_CODE(res))
    {
      return res;
    }

    /* Register the device in the HAL controller device table so that
     * private read/write transfers can be addressed to the dynamic address. */
    I3C_HandleTypeDef *p_i3c = _this->mx_handle.p_mx_i3c_cfg->p_i3c_handle;
    I3C_DeviceConfTypeDef dev_conf = {0};
    dev_conf.DeviceIndex       = 1U;
    dev_conf.TargetDynamicAddr = dynamic_addr;
    dev_conf.IBIAck            = DISABLE;
    dev_conf.IBIPayload        = DISABLE;
    dev_conf.CtrlRoleReqAck    = DISABLE;
    dev_conf.CtrlStopTransfer  = DISABLE;
    if (HAL_I3C_Ctrl_ConfigBusDevices(p_i3c, &dev_conf, 1U) != HAL_OK)
    {
      return SYS_I3C_M_WRITE_ERROR_CODE;
    }

    _this->target_device_addr = dynamic_addr;
  }
  else
  {
    _this->target_device_addr = (dynamic_addr != 0U) ? dynamic_addr : static_addr;
  }

  return SYS_NO_ERROR_CODE;
}

/* Private function definition */
/*******************************/

/* CubeMX integration */
/**********************/

static void I3CMasterDriverTxRxCpltCallback(I3C_HandleTypeDef *p_i3c)
{
  HWDriverMapValue_t *p_val;
  TX_SEMAPHORE *sync_obj;

  p_val = HWDriverMap_FindByKey(&sTheClass.ip_drv_map, (uint32_t) p_i3c->Instance);

  if (p_val != NULL)
  {
#if (SYS_DBG_ENABLE_TA4 == 1)
    if (xTraceIsRecordingEnabled())
    {
      vTraceStoreISRBegin(((I3CPeripheralResources_t *) p_element->p_static_param)->m_xI3cTraceHandle);
    }
#endif

    sync_obj = &((I3CMasterDriver_t *)p_val->p_driver_obj)->sync_obj;

    if (sync_obj != NULL)
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

static void I3CMasterDriverSyncDMAConfig(I3C_HandleTypeDef *p_i3c)
{
  if (p_i3c == NULL)
  {
    return;
  }

  if (p_i3c->hdmacr != NULL)
  {
    p_i3c->hdmacr->Init.Direction = DMA_MEMORY_TO_PERIPH;
    p_i3c->hdmacr->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
    p_i3c->hdmacr->Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
    (void)HAL_DMA_Init(p_i3c->hdmacr);
  }

  if (p_i3c->hdmatx != NULL)
  {
    p_i3c->hdmatx->Init.Direction = DMA_MEMORY_TO_PERIPH;

    if (LL_I3C_GetTxFIFOThreshold(p_i3c->Instance) == LL_I3C_TXFIFO_THRESHOLD_1_4)
    {
      p_i3c->hdmatx->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
      p_i3c->hdmatx->Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    }
    else
    {
      p_i3c->hdmatx->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
      p_i3c->hdmatx->Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
    }

    (void)HAL_DMA_Init(p_i3c->hdmatx);
  }

  if (p_i3c->hdmarx != NULL)
  {
    p_i3c->hdmarx->Init.Direction = DMA_PERIPH_TO_MEMORY;

    if (LL_I3C_GetRxFIFOThreshold(p_i3c->Instance) == LL_I3C_RXFIFO_THRESHOLD_1_4)
    {
      p_i3c->hdmarx->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
      p_i3c->hdmarx->Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    }
    else
    {
      p_i3c->hdmarx->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
      p_i3c->hdmarx->Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
    }

    (void)HAL_DMA_Init(p_i3c->hdmarx);
  }
}

static sys_error_code_t I3CMasterDriverTransmitCCC(I3CMasterDriver_t *_this, uint8_t target_addr, uint8_t ccc,
                                                   uint8_t *p_payload, uint16_t payload_size, uint32_t transfer_mode)
{
  assert_param(_this != NULL);
  I3CMasterDriver_t *p_obj = (I3CMasterDriver_t *) _this;
  I3C_HandleTypeDef *p_i3c = p_obj->mx_handle.p_mx_i3c_cfg->p_i3c_handle;
  I3C_XferTypeDef context_buffer;
  uint32_t control_buffer[0x10];
  uint8_t tx_buffer[0x0F];
  I3C_CCCTypeDef ccc_desc[1] =
  {
    {target_addr, ccc, {p_payload, payload_size}, LL_I3C_DIRECTION_WRITE}
  };

  memset((void *)&context_buffer, 0x00, sizeof(I3C_XferTypeDef));
  context_buffer.CtrlBuf.pBuffer = control_buffer;
  context_buffer.CtrlBuf.Size = COUNTOF(control_buffer);
  context_buffer.TxBuf.pBuffer = tx_buffer;
  context_buffer.TxBuf.Size = COUNTOF(tx_buffer);

  if (HAL_I3C_AddDescToFrame(p_i3c,
                             ccc_desc,
                             NULL,
                             &context_buffer,
                             COUNTOF(ccc_desc),
                             transfer_mode) != HAL_OK)
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_I3C_M_WRITE_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("I3CMasterDriver - CCC frame add failed.\r\n"));
    return SYS_I3C_M_WRITE_ERROR_CODE;
  }

  if (HAL_I3C_Ctrl_TransmitCCC(p_i3c, &context_buffer, 1000U) != HAL_OK)
  {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_I3C_M_WRITE_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("I3CMasterDriver - CCC transmit failed.\r\n"));
    return SYS_I3C_M_WRITE_ERROR_CODE;
  }

  return SYS_NO_ERROR_CODE;
}
