/**
 ******************************************************************************
 * @file    STWIN.box_bus.c
 * @author  SRA
 * @brief   This file provides BSP BUS IO driver
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

#include "STWIN.box_bus.h"
#include "STWIN.box.h"
#include "STWIN.box_errno.h"

#define TIMEOUT_DURATION 1000
/** @addtogroup BSP
 * @{
 */

__weak HAL_StatusTypeDef MX_I2C1_Init(I2C_HandleTypeDef *hi2c);
__weak HAL_StatusTypeDef MX_I2C2_Init(I2C_HandleTypeDef *hi2c);
__weak HAL_StatusTypeDef MX_I2C3_Init(I2C_HandleTypeDef *hi2c);
__weak HAL_StatusTypeDef MX_SPI1_Init(SPI_HandleTypeDef *hspi);
__weak HAL_StatusTypeDef MX_SPI2_Init(SPI_HandleTypeDef *hspi);
__weak HAL_StatusTypeDef MX_SPI3_Init(SPI_HandleTypeDef *hspi);
__weak HAL_StatusTypeDef MX_USART3_UART_Init(UART_HandleTypeDef *huart);

/** @addtogroup STWIN
 * @{
 */

/** @defgroup STWIN_BOX_BUS STWIN BUS
 * @{
 */

/** @defgroup STWIN_BOX_Private_Variables BUS Private Variables
 * @{
 */
static I2C_HandleTypeDef hi2c1;
static I2C_HandleTypeDef hi2c2;
static I2C_HandleTypeDef hi2c3;
static SPI_HandleTypeDef hbusspi1;
static SPI_HandleTypeDef hbusspi2;
static SPI_HandleTypeDef hbusspi3;
static UART_HandleTypeDef huart3;

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
static uint32_t IsI2C1MspCbValid = 0;
static uint32_t IsI2C2MspCbValid = 0;
static uint32_t IsI2C3MspCbValid = 0;
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */				
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)						
static uint32_t IsSPI1MspCbValid = 0;
static uint32_t IsSPI2MspCbValid = 0;
static uint32_t IsSPI3MspCbValid = 0;	
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */			
/**
 * @}
 */

/** @defgroup STWIN_BOX_Private_FunctionPrototypes  Private Function Prototypes
 * @{
 */

static void I2C1_MspInit(I2C_HandleTypeDef *i2cHandle);
static void I2C1_MspDeInit(I2C_HandleTypeDef *i2cHandle);
static void I2C2_MspInit(I2C_HandleTypeDef *i2cHandle);
static void I2C2_MspDeInit(I2C_HandleTypeDef *i2cHandle);
static void I2C3_MspInit(I2C_HandleTypeDef *i2cHandle);
static void I2C3_MspDeInit(I2C_HandleTypeDef *i2cHandle);
static void SPI1_MspInit(SPI_HandleTypeDef *spiHandle);
static void SPI1_MspDeInit(SPI_HandleTypeDef *spiHandle);
static void SPI2_MspInit(SPI_HandleTypeDef *spiHandle);
static void SPI2_MspDeInit(SPI_HandleTypeDef *spiHandle);
static void SPI3_MspInit(SPI_HandleTypeDef *spiHandle);
static void SPI3_MspDeInit(SPI_HandleTypeDef *spiHandle);
static void USART3_MspInit(UART_HandleTypeDef *huart);
static void USART3_MspDeInit(UART_HandleTypeDef *huart);

/**
 * @}
 */

/** @defgroup STWIN_BOX_LOW_LEVEL_Private_Functions STWIN LOW LEVEL Private Functions
 * @{
 */

/** @defgroup STWIN_BOX_BUS_Exported_Functions STWIN_BOX_BUS Exported Functions
 * @{
 */
/* BUS IO driver over I2C Peripheral */
/*******************************************************************************
 BUS OPERATIONS OVER I2C
 *******************************************************************************/
/**
 * @brief Initialize a bus
 * @param None
 * @retval BSP status
 */
int32_t BSP_I2C1_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  hi2c1.Instance = I2C1;

  if(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_RESET)
  {
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
    /* Init the I2C Msp */
    I2C1_MspInit(&hi2c1);
#else
      if(IsI2C1MspCbValid == 0U)
      {
        if(BSP_I2C1_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
        {
          return BSP_ERROR_MSP_FAILURE;
        }
      }
    #endif

    /* Init the I2C */
    if(MX_I2C1_Init(&hi2c1) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
    else if(HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  return ret;
}

/**
 * @brief  DeInitialize a bus
 * @param None
 * @retval BSP status
 */
int32_t BSP_I2C1_DeInit(void)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
  /* DeInit the I2C */
  I2C1_MspDeInit(&hi2c1);
#endif

  if(HAL_I2C_DeInit(&hi2c1) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief Return the status of the Bus
 *	@retval bool
 */
int32_t BSP_I2C1_IsReady(uint16_t DevAddr, uint32_t Trials)
{
  int32_t ret = BSP_ERROR_NONE;

  if(HAL_I2C_IsDeviceReady(&hi2c1, DevAddr, Trials, BUS_I2C1_POLL_TIMEOUT) != HAL_OK)
  {
    ret = BSP_ERROR_BUSY;
  }

  return ret;
}

/**
 * @brief  Write registers through bus (8 bits)
 * @param  Addr: Device address on Bus.
 * @param  Reg: The target register address to write
 * @param  Value: The target register value to be written
 * @retval BSP status
 */
int32_t BSP_I2C1_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Write(&hi2c1, (uint8_t) DevAddr, (uint16_t) Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*) pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Read registers through a bus (8 bits)
 * @param  DevAddr: Device address on BUS
 * @param  Reg: The target register address to read
 * @retval BSP status
 */
int32_t BSP_I2C1_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Read(&hi2c1, DevAddr, (uint16_t) Reg,
  I2C_MEMADD_SIZE_8BIT,
                      pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = (int32_t) HAL_OK;
  }

  return ret;
}

/**
 * @brief  Write registers through bus (16 bits)
 * @param  Addr: Device address on Bus.
 * @param  Reg: The target register address to write
 * @param  Value: The target register value to be written
 * @retval BSP status
 */
int32_t BSP_I2C1_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Write(&hi2c1, (uint8_t) DevAddr, (uint16_t) Reg, I2C_MEMADD_SIZE_16BIT, (uint8_t*) pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Read registers through a bus (16 bits)
 * @param  DevAddr: Device address on BUS
 * @param  Reg: The target register address to read
 * @retval BSP status
 */
int32_t BSP_I2C1_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Read(&hi2c1, DevAddr, (uint16_t) Reg,
  I2C_MEMADD_SIZE_16BIT,
                      pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Send an amount width data through bus (Simplex)
 * @param  DevAddr: Device address on Bus.
 * @param  pData: Data pointer
 * @param  Length: Data length
 * @retval BSP status
 */
int32_t BSP_I2C1_Send(uint16_t DevAddr, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Master_Transmit(&hi2c1, DevAddr, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Receive an amount of data through a bus (Simplex)
 * @param  DevAddr: Device address on Bus.
 * @param  pData: Data pointer
 * @param  Length: Data length
 * @retval BSP status
 */
int32_t BSP_I2C1_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Master_Receive(&hi2c1, DevAddr, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Send and receive an amount of data through bus (Full duplex)
 * @param  DevAddr: Device address on Bus.
 * @param  pTxdata: Transmit data pointer
 * @param 	pRxdata: Receive data pointer
 * @param  Length: Data length
 * @retval BSP status
 */
int32_t BSP_I2C1_SendRecv(uint16_t DevAddr, uint8_t *pTxdata, uint8_t *pRxdata, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  /*
   * Send and receive an amount of data through bus (Full duplex)
   * I2C is Half-Duplex protocol
   */
  if(BSP_I2C1_Send(DevAddr, pTxdata, len) == (int32_t) len)
  {
    if(BSP_I2C1_Recv(DevAddr, pRxdata, len) == (int32_t) len)
    {
      ret = (int32_t) len;
    }
  }

  return ret;
}

/**
 * @brief  Initialize a bus
 * @param None
 * @retval BSP status
 */
int32_t BSP_I2C2_Init(void)
{

  int32_t ret = BSP_ERROR_NONE;

  hi2c2.Instance = I2C2;

  if(HAL_I2C_GetState(&hi2c2) == HAL_I2C_STATE_RESET)
  {
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
    /* Init the I2C Msp */
    I2C2_MspInit(&hi2c2);
#else
      if(IsI2C2MspCbValid == 0U)
      {
        if(BSP_I2C2_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
        {
          return BSP_ERROR_MSP_FAILURE;
        }
      }
    #endif

    /* Init the I2C */
    if(MX_I2C2_Init(&hi2c2) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
    else if(HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  return ret;
}

/**
 * @brief  DeInitialize a bus
 * @param None
 * @retval BSP status
 */
int32_t BSP_I2C2_DeInit(void)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
  /* DeInit the I2C */
  I2C2_MspDeInit(&hi2c2);
#endif

  if(HAL_I2C_DeInit(&hi2c2) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Check whether the I2C bus is ready.
 * @param DevAddr : I2C device address
 * @param Trials : Check trials number
 * @retval BSP status
 */
int32_t BSP_I2C2_IsReady(uint16_t DevAddr, uint32_t Trials)
{
  int32_t ret = BSP_ERROR_NONE;

  if(HAL_I2C_IsDeviceReady(&hi2c2, DevAddr, Trials, BUS_I2C2_POLL_TIMEOUT) != HAL_OK)
  {
    ret = BSP_ERROR_BUSY;
  }

  return ret;
}

/**
 * @brief  Write registers through bus (8 bits)
 * @param  Addr: Device address on Bus.
 * @param  Reg: The target register address to write
 * @param  Value: The target register value to be written
 * @retval BSP status
 */
int32_t BSP_I2C2_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Write(&hi2c2, (uint8_t) DevAddr, (uint16_t) Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*) pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Read registers through a bus (8 bits)
 * @param  DevAddr: Device address on BUS
 * @param  Reg: The target register address to read
 * @retval BSP status
 */
int32_t BSP_I2C2_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Read(&hi2c2, DevAddr, (uint16_t) Reg,
  I2C_MEMADD_SIZE_8BIT,
                      pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = (int32_t) HAL_OK;
  }

  return ret;
}

/**
 * @brief  Write registers through bus (16 bits)
 * @param  Addr: Device address on Bus.
 * @param  Reg: The target register address to write
 * @param  Value: The target register value to be written
 * @retval BSP status
 */
int32_t BSP_I2C2_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Write(&hi2c2, (uint8_t) DevAddr, (uint16_t) Reg, I2C_MEMADD_SIZE_16BIT, (uint8_t*) pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Read registers through a bus (16 bits)
 * @param  DevAddr: Device address on BUS
 * @param  Reg: The target register address to read
 * @retval BSP status
 */
int32_t BSP_I2C2_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Read(&hi2c2, DevAddr, (uint16_t) Reg,
  I2C_MEMADD_SIZE_16BIT,
                      pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Send an amount width data through bus (Simplex)
 * @param  DevAddr: Device address on Bus.
 * @param  pData: Data pointer
 * @param  Length: Data length
 * @retval BSP status
 */
int32_t BSP_I2C2_Send(uint16_t DevAddr, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Master_Transmit(&hi2c2, DevAddr, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Receive an amount of data through a bus (Simplex)
 * @param  DevAddr: Device address on Bus.
 * @param  pData: Data pointer
 * @param  Length: Data length
 * @retval BSP status
 */
int32_t BSP_I2C2_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Master_Receive(&hi2c2, DevAddr, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Send and receive an amount of data through bus (Full duplex)
 * @param  DevAddr: Device address on Bus.
 * @param  pTxdata: Transmit data pointer
 * @param 	pRxdata: Receive data pointer
 * @param  Length: Data length
 * @retval BSP status
 */
int32_t BSP_I2C2_SendRecv(uint16_t DevAddr, uint8_t *pTxdata, uint8_t *pRxdata, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  /*
   * Send and receive an amount of data through bus (Full duplex)
   * I2C is Half-Duplex protocol
   */
  if(BSP_I2C2_Send(DevAddr, pTxdata, len) == (int32_t) len)
  {
    if(BSP_I2C2_Recv(DevAddr, pRxdata, len) == (int32_t) len)
    {
      ret = BSP_ERROR_NONE;
    }
  }

  return ret;
}

/**
 * @brief  Initialize a bus
 * @param None
 * @retval BSP status
 */
int32_t BSP_I2C3_Init(void)
{

  int32_t ret = BSP_ERROR_NONE;

  hi2c3.Instance = I2C3;

  if(HAL_I2C_GetState(&hi2c3) == HAL_I2C_STATE_RESET)
  {
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
    /* Init the I2C Msp */
    I2C3_MspInit(&hi2c3);
#else
      if(IsI2C3MspCbValid == 0U)
      {
        if(BSP_I2C3_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
        {
          return BSP_ERROR_MSP_FAILURE;
        }
      }
    #endif

    /* Init the I2C */
    if(MX_I2C3_Init(&hi2c3) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
    else if(HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  return ret;
}

/**
 * @brief  DeInitialize a bus
 * @param None
 * @retval BSP status
 */
int32_t BSP_I2C3_DeInit(void)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
  /* DeInit the I2C */
  I2C3_MspDeInit(&hi2c3);
#endif

  if(HAL_I2C_DeInit(&hi2c3) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief Return the status of the Bus
 *	@retval bool
 */
int32_t BSP_I2C3_IsReady(uint16_t DevAddr, uint32_t Trials)
{
  int32_t ret = BSP_ERROR_NONE;

  if(HAL_I2C_IsDeviceReady(&hi2c3, DevAddr, Trials, BUS_I2C1_POLL_TIMEOUT) != HAL_OK)
  {
    ret = BSP_ERROR_BUSY;
  }

  return ret;
}

/**
 * @brief  Write registers through bus (8 bits)
 * @param  Addr: Device address on Bus.
 * @param  Reg: The target register address to write
 * @param  Value: The target register value to be written
 * @retval BSP status
 */
int32_t BSP_I2C3_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Write(&hi2c3, (uint8_t) DevAddr, (uint16_t) Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*) pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Read registers through a bus (8 bits)
 * @param  DevAddr: Device address on BUS
 * @param  Reg: The target register address to read
 * @retval BSP status
 */
int32_t BSP_I2C3_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Read(&hi2c3, DevAddr, (uint16_t) Reg,
  I2C_MEMADD_SIZE_8BIT,
                      pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Write registers through bus (16 bits)
 * @param  Addr: Device address on Bus.
 * @param  Reg: The target register address to write
 * @param  Value: The target register value to be written
 * @retval BSP status
 */
int32_t BSP_I2C3_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Write(&hi2c3, (uint8_t) DevAddr, (uint16_t) Reg, I2C_MEMADD_SIZE_16BIT, (uint8_t*) pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Read registers through a bus (16 bits)
 * @param  DevAddr: Device address on BUS
 * @param  Reg: The target register address to read
 * @retval BSP status
 */
int32_t BSP_I2C3_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Read(&hi2c3, DevAddr, (uint16_t) Reg,
  I2C_MEMADD_SIZE_16BIT,
                      pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Send an amount width data through bus (Simplex)
 * @param  DevAddr: Device address on Bus.
 * @param  pData: Data pointer
 * @param  Length: Data length
 * @retval BSP status
 */
int32_t BSP_I2C3_Send(uint16_t DevAddr, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Master_Transmit(&hi2c3, DevAddr, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Receive an amount of data through a bus (Simplex)
 * @param  DevAddr: Device address on Bus.
 * @param  pData: Data pointer
 * @param  Length: Data length
 * @retval BSP status
 */
int32_t BSP_I2C3_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Master_Receive(&hi2c3, DevAddr, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Send and receive an amount of data through bus (Full duplex)
 * @param  DevAddr: Device address on Bus.
 * @param  pTxdata: Transmit data pointer
 * @param 	pRxdata: Receive data pointer
 * @param  Length: Data length
 * @retval BSP status
 */
int32_t BSP_I2C3_SendRecv(uint16_t DevAddr, uint8_t *pTxdata, uint8_t *pRxdata, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  /*
   * Send and receive an amount of data through bus (Full duplex)
   * I2C is Half-Duplex protocol
   */
  if(BSP_I2C3_Send(DevAddr, pTxdata, len) == (int32_t) len)
  {
    if(BSP_I2C3_Recv(DevAddr, pRxdata, len) == (int32_t) len)
    {
      ret = BSP_ERROR_NONE;
    }
  }

  return ret;
}

/* BUS IO driver over SPI Peripheral */
/*******************************************************************************
 BUS OPERATIONS OVER SPI
 *******************************************************************************/
/**
 * @brief  Initializes SPI HAL.
 * @retval None
 * @retval BSP status
 */
int32_t BSP_SPI1_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  hbusspi1.Instance = SPI1;
  if(HAL_SPI_GetState(&hbusspi1) == HAL_SPI_STATE_RESET)
  {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
    /* Init the SPI Msp */
    SPI1_MspInit(&hbusspi1);
#else
    if(IsSPI1MspCbValid == 0U)
    {
      if(BSP_SPI1_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
      {
        return BSP_ERROR_MSP_FAILURE;
      }
    }
#endif   

    /* Init the SPI */
    if(MX_SPI1_Init(&hbusspi1) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
  }

  return ret;
}

/**
 * @brief  DeInitializes SPI HAL.
 * @retval None
 * @retval BSP status
 */
int32_t BSP_SPI1_DeInit(void)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
  SPI1_MspDeInit(&hbusspi1);
#endif  

  if(HAL_SPI_DeInit(&hbusspi1) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Write Data through SPI BUS.
 * @param  pData: Data
 * @param  len: Length of data in byte
 * @retval BSP status
 */
int32_t BSP_SPI1_Send(uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_Transmit(&hbusspi1, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
 * @brief  Receive Data from SPI BUS
 * @param  pData: Data
 * @param  len: Length of data in byte
 * @retval BSP status
 */
int32_t BSP_SPI1_Recv(uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_Receive(&hbusspi1, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
 * @brief  Send and Receive data to/from SPI BUS (Full duplex)
 * @param  pData: Data
 * @param  len: Length of data in byte
 * @retval BSP status
 */
int32_t BSP_SPI1_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint16_t len)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_TransmitReceive(&hbusspi1, pTxData, pRxData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
 * @brief  Initializes SPI HAL.
 * @retval None
 * @retval BSP status
 */
int32_t BSP_SPI2_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  hbusspi2.Instance = SPI2;
  if(HAL_SPI_GetState(&hbusspi2) == HAL_SPI_STATE_RESET)
  {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
    /* Init the SPI Msp */
    SPI2_MspInit(&hbusspi2);
#else
    if(IsSPI2MspCbValid == 0U)
    {
      if(BSP_SPI2_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
      {
        return BSP_ERROR_MSP_FAILURE;
      }
    }
#endif   

    /* Init the SPI */
    if(MX_SPI2_Init(&hbusspi2) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
  }

  __HAL_SPI_ENABLE(&hbusspi2);

  return ret;
}

/**
 * @brief  DeInitializes SPI HAL.
 * @retval None
 * @retval BSP status
 */
int32_t BSP_SPI2_DeInit(void)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
  SPI2_MspDeInit(&hbusspi2);
#endif  

  if(HAL_SPI_DeInit(&hbusspi2) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Write Data through SPI BUS.
 * @param  pData: Data
 * @param  len: Length of data in byte
 * @retval BSP status
 */
int32_t BSP_SPI2_Send(uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_Transmit(&hbusspi2, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
 * @brief  Receive Data from SPI BUS
 * @param  pData: Data
 * @param  len: Length of data in byte
 * @retval BSP status
 */
int32_t BSP_SPI2_Recv(uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_Receive(&hbusspi2, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
 * @brief  Send and Receive data to/from SPI BUS (Full duplex)
 * @param  pData: Data
 * @param  len: Length of data in byte
 * @retval BSP status
 */
int32_t BSP_SPI2_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint16_t len)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_TransmitReceive(&hbusspi2, pTxData, pRxData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
 * @brief  Initializes SPI HAL.
 * @retval None
 * @retval BSP status
 */
int32_t BSP_SPI3_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  hbusspi3.Instance = SPI3;
  if(HAL_SPI_GetState(&hbusspi3) == HAL_SPI_STATE_RESET)
  {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
    /* Init the SPI Msp */
    SPI3_MspInit(&hbusspi3);
#else
    if(IsSPI3MspCbValid == 0U)
    {
      if(BSP_SPI3_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
      {
        return BSP_ERROR_MSP_FAILURE;
      }
    }
#endif   

    /* Init the SPI */
    if(MX_SPI3_Init(&hbusspi3) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
  }

  return ret;
}

/**
 * @brief  DeInitializes SPI HAL.
 * @retval None
 * @retval BSP status
 */
int32_t BSP_SPI3_DeInit(void)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
  SPI3_MspDeInit(&hbusspi3);
#endif  

  if(HAL_SPI_DeInit(&hbusspi3) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Write Data through SPI BUS.
 * @param  pData: Data
 * @param  len: Length of data in byte
 * @retval BSP status
 */
int32_t BSP_SPI3_Send(uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_Transmit(&hbusspi3, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
 * @brief  Receive Data from SPI BUS
 * @param  pData: Data
 * @param  len: Length of data in byte
 * @retval BSP status
 */
int32_t BSP_SPI3_Recv(uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_Receive(&hbusspi3, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
 * @brief  Send and Receive data to/from SPI BUS (Full duplex)
 * @param  pData: Data
 * @param  len: Length of data in byte
 * @retval BSP status
 */
int32_t BSP_SPI3_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint16_t len)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_TransmitReceive(&hbusspi3, pTxData, pRxData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/* BUS IO driver over USART Peripheral */
/*******************************************************************************
 BUS OPERATIONS OVER USART
 *******************************************************************************/
/**
 * @brief  Configures USART.
 * @param  UART_Init: Pointer to a UART_HandleTypeDef structure that contains the
 *                    configuration information for the specified USART peripheral.
 * @retval BSP error code
 */
int32_t BSP_USART3_Init(void)
{
  huart3.Instance = USART3;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
  /* Init the UART Msp */
  USART3_MspInit(&huart3);
#else
  if(IsUsart3MspCbValid == 0U)
  {
    if(BSP_USART3_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
    {
      return BSP_ERROR_MSP_FAILURE;
    }
  }
#endif

  (void) MX_USART3_UART_Init(&huart3);

  return BSP_ERROR_NONE;
}

/**
 * @brief  DeInit USART.
 * @param  COM COM port to be configured.
 *             This parameter can be COM1
 * @retval BSP status
 */
int32_t BSP_USART3_DeInit(void)
{
  int32_t ret = BSP_ERROR_PERIPH_FAILURE;

  huart3.Instance = USART3;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)  
  USART3_MspDeInit(&huart3);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 0) */

  if(HAL_UART_DeInit(&huart3) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)  
/**
  * @brief Register Default BSP I2C1 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_I2C1_RegisterDefaultMspCallbacks (void)
{

  __HAL_I2C_RESET_HANDLE_STATE(&hi2c1);
  
  /* Register MspInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c1, HAL_I2C_MSPINIT_CB_ID, I2C1_MspInit)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c1, HAL_I2C_MSPDEINIT_CB_ID, I2C1_MspDeInit) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  IsI2C1MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}

/**
  * @brief BSP I2C1 Bus Msp Callback registering
  * @param Callbacks     pointer to I2C1 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_I2C1_RegisterMspCallbacks (BSP_I2C_Cb_t *Callbacks)
{
  /* Prevent unused argument(s) compilation warning */
  __HAL_I2C_RESET_HANDLE_STATE(&hi2c1);  
 
   /* Register MspInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c1, HAL_I2C_MSPINIT_CB_ID, Callbacks->pMspI2cInitCb)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c1, HAL_I2C_MSPDEINIT_CB_ID, Callbacks->pMspI2cDeInitCb) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  IsI2C1MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}
	
/**
  * @brief Register Default BSP I2C2 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_I2C2_RegisterDefaultMspCallbacks (void)
{

  __HAL_I2C_RESET_HANDLE_STATE(&hi2c2);
  
  /* Register MspInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c2, HAL_I2C_MSPINIT_CB_ID, I2C2_MspInit)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c2, HAL_I2C_MSPDEINIT_CB_ID, I2C2_MspDeInit) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  IsI2C2MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}

/**
  * @brief BSP I2C2 Bus Msp Callback registering
  * @param Callbacks     pointer to I2C2 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_I2C2_RegisterMspCallbacks (BSP_I2C_Cb_t *Callbacks)
{
  /* Prevent unused argument(s) compilation warning */
  __HAL_I2C_RESET_HANDLE_STATE(&hi2c2);  
 
   /* Register MspInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c2, HAL_I2C_MSPINIT_CB_ID, Callbacks->pMspI2cInitCb)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c2, HAL_I2C_MSPDEINIT_CB_ID, Callbacks->pMspI2cDeInitCb) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  IsI2C2MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}
	
/**
  * @brief Register Default BSP I2C3 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_I2C3_RegisterDefaultMspCallbacks (void)
{

  __HAL_I2C_RESET_HANDLE_STATE(&hi2c3);
  
  /* Register MspInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c3, HAL_I2C_MSPINIT_CB_ID, I2C3_MspInit)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c3, HAL_I2C_MSPDEINIT_CB_ID, I2C3_MspDeInit) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  IsI2C3MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}

/**
  * @brief BSP I2C2 Bus Msp Callback registering
  * @param Callbacks     pointer to I2C2 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_I2C3_RegisterMspCallbacks (BSP_I2C_Cb_t *Callbacks)
{
  /* Prevent unused argument(s) compilation warning */
  __HAL_I2C_RESET_HANDLE_STATE(&hi2c3);  
 
   /* Register MspInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c3, HAL_I2C_MSPINIT_CB_ID, Callbacks->pMspI2cInitCb)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c3, HAL_I2C_MSPDEINIT_CB_ID, Callbacks->pMspI2cDeInitCb) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  IsI2C3MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}



#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)  
/**
  * @brief Register Default BSP SPI1 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_SPI1_RegisterDefaultMspCallbacks (void)
{

  __HAL_SPI_RESET_HANDLE_STATE(&hbusspi1);
  
  /* Register MspInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi1, HAL_SPI_MSPINIT_CB_ID, SPI1_MspInit)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi1, HAL_SPI_MSPDEINIT_CB_ID, SPI1_MspDeInit) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  IsSPI1MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}

/**
  * @brief BSP SPI1 Bus Msp Callback registering
  * @param Callbacks     pointer to SPI1 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_SPI1_RegisterMspCallbacks (BSP_SPI_Cb_t *Callbacks)
{
  /* Prevent unused argument(s) compilation warning */
  __HAL_SPI_RESET_HANDLE_STATE(&hbusspi1);  
 
   /* Register MspInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi1, HAL_SPI_MSPINIT_CB_ID, Callbacks->pMspSpiInitCb)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi1, HAL_SPI_MSPDEINIT_CB_ID, Callbacks->pMspSpiDeInitCb) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  IsSPI1MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}

/**
  * @brief Register Default BSP SPI2 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_SPI2_RegisterDefaultMspCallbacks (void)
{

  __HAL_SPI_RESET_HANDLE_STATE(&hbusspi2);
  
  /* Register MspInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi2, HAL_SPI_MSPINIT_CB_ID, SPI2_MspInit)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi2, HAL_SPI_MSPDEINIT_CB_ID, SPI2_MspDeInit) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  IsSPI2MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}

/**
  * @brief BSP SPI1 Bus Msp Callback registering
  * @param Callbacks     pointer to SPI1 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_SPI2_RegisterMspCallbacks (BSP_SPI_Cb_t *Callbacks)
{
  /* Prevent unused argument(s) compilation warning */
  __HAL_SPI_RESET_HANDLE_STATE(&hbusspi2);  
 
   /* Register MspInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi2, HAL_SPI_MSPINIT_CB_ID, Callbacks->pMspSpiInitCb)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi2, HAL_SPI_MSPDEINIT_CB_ID, Callbacks->pMspSpiDeInitCb) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  IsSPI2MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}


/**
  * @brief Register Default BSP SPI3 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_SPI3_RegisterDefaultMspCallbacks (void)
{

  __HAL_SPI_RESET_HANDLE_STATE(&hbusspi3);
  
  /* Register MspInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi3, HAL_SPI_MSPINIT_CB_ID, SPI3_MspInit)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi3, HAL_SPI_MSPDEINIT_CB_ID, SPI3_MspDeInit) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  IsSPI3MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}

/**
  * @brief BSP SPI3 Bus Msp Callback registering
  * @param Callbacks     pointer to SPI3 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_SPI3_RegisterMspCallbacks (BSP_SPI_Cb_t *Callbacks)
{
  /* Prevent unused argument(s) compilation warning */
  __HAL_SPI_RESET_HANDLE_STATE(&hbusspi3);  
 
   /* Register MspInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi3, HAL_SPI_MSPINIT_CB_ID, Callbacks->pMspSpiInitCb)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi3, HAL_SPI_MSPDEINIT_CB_ID, Callbacks->pMspSpiDeInitCb) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  IsSPI3MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */

/**
 * @brief  Return system tick in ms
 * @retval Current HAL time base time stamp
 */
int32_t BSP_GetTick(void)
{
  return (int32_t) HAL_GetTick();
}

/* SPI1 init function */

__weak HAL_StatusTypeDef MX_SPI1_Init(SPI_HandleTypeDef *hspi)
{
  HAL_StatusTypeDef ret = HAL_OK;

  SPI_AutonomousModeConfTypeDef HAL_SPI_AutonomousMode_Cfg_Struct =
  {
      0 };

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi->Instance = SPI1;
  hspi->Init.Mode = SPI_MODE_MASTER;
  hspi->Init.Direction = SPI_DIRECTION_2LINES;
  hspi->Init.DataSize = SPI_DATASIZE_8BIT;
  hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi->Init.NSS = SPI_NSS_SOFT;
  hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi->Init.TIMode = SPI_TIMODE_DISABLE;
  hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi->Init.CRCPolynomial = 0x7;
  hspi->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  hspi->Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi->Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi->Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi->Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi->Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi->Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi->Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi->Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi->Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi->Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  hspi->Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
  if(HAL_SPI_Init(hspi) != HAL_OK)
  {
    ret = HAL_ERROR;
  }
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerState = SPI_AUTO_MODE_DISABLE;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerSelection = SPI_GRP1_GPDMA_CH0_TCF_TRG;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
  if(HAL_SPIEx_SetConfigAutonomousMode(hspi, &HAL_SPI_AutonomousMode_Cfg_Struct) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

static void SPI1_MspInit(SPI_HandleTypeDef *spiHandle)
{
  UNUSED(spiHandle);
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable Peripheral clock */
  __HAL_RCC_SPI1_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableIO2VM();
  while(!(PWR->SVMCR & PWR_SVMCR_IO2VMEN)){}
  HAL_PWREx_EnableVddIO2();
  while(!(PWR->SVMSR & PWR_SVMSR_VDDIO2RDY)){}

  /**SPI1 GPIO Configuration    
   PG2     ------> SPI1_SCK
   PG4     ------> SPI1_MOSI
   PG3     ------> SPI1_MISO
   */
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

static void SPI1_MspDeInit(SPI_HandleTypeDef *spiHandle)
{
  UNUSED(spiHandle);

  /* Peripheral clock disable */
  __HAL_RCC_SPI1_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();

  /**SPI1 GPIO Configuration    
   PG2     ------> SPI1_SCK
   PG4     ------> SPI1_MOSI
   PG3     ------> SPI1_MISO
   */
  HAL_GPIO_DeInit(GPIOG, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
}

/* SPI2 init function */
__weak HAL_StatusTypeDef MX_SPI2_Init(SPI_HandleTypeDef *hspi)
{
  HAL_StatusTypeDef ret = HAL_OK;

  SPI_AutonomousModeConfTypeDef HAL_SPI_AutonomousMode_Cfg_Struct =
  {
      0 };

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi->Instance = SPI2;
  hspi->Init.Mode = SPI_MODE_MASTER;
  hspi->Init.Direction = SPI_DIRECTION_2LINES;
  hspi->Init.DataSize = SPI_DATASIZE_8BIT;
  hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi->Init.NSS = SPI_NSS_SOFT;
  hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi->Init.TIMode = SPI_TIMODE_DISABLE;
  hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi->Init.CRCPolynomial = 0x7;
  hspi->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  hspi->Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi->Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi->Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi->Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi->Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi->Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi->Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi->Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi->Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi->Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  hspi->Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
  if(HAL_SPI_Init(hspi) != HAL_OK)
  {
    ret = HAL_ERROR;
  }
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerState = SPI_AUTO_MODE_DISABLE;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerSelection = SPI_GRP1_GPDMA_CH0_TCF_TRG;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
  if(HAL_SPIEx_SetConfigAutonomousMode(hspi, &HAL_SPI_AutonomousMode_Cfg_Struct) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

static void SPI2_MspInit(SPI_HandleTypeDef *spiHandle)
{
  UNUSED(spiHandle);
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable Peripheral clock */
  __HAL_RCC_SPI2_CLK_ENABLE();

  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  /**SPI2 GPIO Configuration
   PI1     ------> SPI2_SCK
   PD3     ------> SPI2_MISO
   PI3     ------> SPI2_MOSI
   */

  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

static void SPI2_MspDeInit(SPI_HandleTypeDef *spiHandle)
{
  UNUSED(spiHandle);

  /* Peripheral clock disable */
  __HAL_RCC_SPI2_CLK_DISABLE();

  /**SPI2 GPIO Configuration
   PI1     ------> SPI2_SCK
   PD3     ------> SPI2_MISO
   PI3     ------> SPI2_MOSI
   */
  HAL_GPIO_DeInit(GPIOI, GPIO_PIN_1 | GPIO_PIN_3);
  HAL_GPIO_DeInit(GPIOD, GPIO_PIN_3);
}

/* SPI3 init function */
__weak HAL_StatusTypeDef MX_SPI3_Init(SPI_HandleTypeDef *hspi)
{
  HAL_StatusTypeDef ret = HAL_OK;

  SPI_AutonomousModeConfTypeDef HAL_SPI_AutonomousMode_Cfg_Struct =
  {
      0 };

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi->Instance = SPI3;
  hspi->Init.Mode = SPI_MODE_MASTER;
  hspi->Init.Direction = SPI_DIRECTION_2LINES;
  hspi->Init.DataSize = SPI_DATASIZE_8BIT;
  hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi->Init.NSS = SPI_NSS_SOFT;
  hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; // SPI3 CLK Source is HSI (16MHz)
  hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi->Init.TIMode = SPI_TIMODE_DISABLE;
  hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi->Init.CRCPolynomial = 0x7;
  hspi->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  hspi->Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi->Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi->Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi->Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi->Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi->Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi->Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi->Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi->Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi->Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  hspi->Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
  if(HAL_SPI_Init(hspi) != HAL_OK)
  {
    ret = HAL_ERROR;
  }
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerState = SPI_AUTO_MODE_DISABLE;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerSelection = SPI_GRP1_GPDMA_CH0_TCF_TRG;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
  if(HAL_SPIEx_SetConfigAutonomousMode(hspi, &HAL_SPI_AutonomousMode_Cfg_Struct) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  __HAL_SPI_ENABLE(hspi);

  return ret;
}

static void SPI3_MspInit(SPI_HandleTypeDef *spiHandle)
{
  UNUSED(spiHandle);
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  
  /* Enable Peripheral clock */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SPI3;
  PeriphClkInit.Spi3ClockSelection = RCC_SPI3CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    for(;;){}   /* Blocking error */
  }
  
  __HAL_RCC_SPI3_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableIO2VM();
  while(!(PWR->SVMCR & PWR_SVMCR_IO2VMEN)){}
  HAL_PWREx_EnableVddIO2();
  while(!(PWR->SVMSR & PWR_SVMSR_VDDIO2RDY)){}
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /**SPI3 GPIO Configuration
   PG9     ------> SPI3_SCK
   PB4 (NJTRST)     ------> SPI3_MISO
   PB5     ------> SPI3_MOSI
   */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void SPI3_MspDeInit(SPI_HandleTypeDef *spiHandle)
{
  UNUSED(spiHandle);
  /* Peripheral clock disable */
  __HAL_RCC_SPI3_CLK_DISABLE();

  /**SPI3 GPIO Configuration
   PG9     ------> SPI3_SCK
   PB4 (NJTRST)     ------> SPI3_MISO
   PB5     ------> SPI3_MOSI
   */
  HAL_GPIO_DeInit(GPIOG, GPIO_PIN_9);

  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4 | GPIO_PIN_5);
}

/* I2C2 init function */

__weak HAL_StatusTypeDef MX_I2C1_Init(I2C_HandleTypeDef *hi2c)
{
  HAL_StatusTypeDef ret = HAL_OK;
  hi2c->Instance = I2C1;
  hi2c->Init.Timing = 0x00F07BFF;
  hi2c->Init.OwnAddress1 = 0;
  hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c->Init.OwnAddress2 = 0;
  hi2c->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if(HAL_I2C_Init(hi2c) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if(HAL_I2CEx_ConfigAnalogFilter(hi2c, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if(HAL_I2CEx_ConfigDigitalFilter(hi2c, 0) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

static void I2C1_MspInit(I2C_HandleTypeDef *i2cHandle)
{
  UNUSED(i2cHandle);
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOB_CLK_ENABLE();
  /**I2C1 GPIO Configuration
   PB6     ------> I2C1_SCL
   PB9     ------> I2C1_SDA
   */
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Peripheral clock enable */
  __HAL_RCC_I2C1_CLK_ENABLE();
}

static void I2C1_MspDeInit(I2C_HandleTypeDef *i2cHandle)
{
  UNUSED(i2cHandle);

  /* Peripheral clock disable */
  __HAL_RCC_I2C1_CLK_DISABLE();

  /**I2C1 GPIO Configuration
   PB6     ------> I2C1_SCL
   PB9     ------> I2C1_SDA
   */
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);
}

__weak HAL_StatusTypeDef MX_I2C2_Init(I2C_HandleTypeDef *hi2c)
{
  HAL_StatusTypeDef ret = HAL_OK;

  hi2c->Instance = I2C2;
  hi2c->Init.Timing = 0x2040184B;
  hi2c->Init.OwnAddress1 = 0;
  hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c->Init.OwnAddress2 = 0;
  hi2c->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if(HAL_I2C_Init(hi2c) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if(HAL_I2CEx_ConfigAnalogFilter(hi2c, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if(HAL_I2CEx_ConfigDigitalFilter(hi2c, 0) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

static void I2C2_MspInit(I2C_HandleTypeDef *i2cHandle)
{
  UNUSED(i2cHandle);
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  /** Initializes the peripherals clock */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {

  }

  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  /**I2C2 GPIO Configuration
   PF0     ------> I2C2_SDA
   PH4     ------> I2C2_SCL
   */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /* Peripheral clock enable */
  __HAL_RCC_I2C2_CLK_ENABLE();

  /* Peripheral clock enable */
  __HAL_RCC_I2C2_CLK_ENABLE();
  __I2C2_FORCE_RESET();
  __I2C2_RELEASE_RESET();

  /* Enable and set I2C_STWIN Interrupt to the highest priority */
  HAL_NVIC_SetPriority(I2C2_EV_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);

  /* Enable and set I2C_STWIN Interrupt to the highest priority */
  HAL_NVIC_SetPriority(I2C2_ER_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
}

static void I2C2_MspDeInit(I2C_HandleTypeDef *i2cHandle)
{
  UNUSED(i2cHandle);

  /* Peripheral clock disable */
  __HAL_RCC_I2C2_CLK_DISABLE();

  /**I2C2 GPIO Configuration
   PF0     ------> I2C2_SDA
   PH4     ------> I2C2_SCL
   */
  HAL_GPIO_DeInit(GPIOF, GPIO_PIN_0);

  HAL_GPIO_DeInit(GPIOH, GPIO_PIN_4);
}

__weak HAL_StatusTypeDef MX_I2C3_Init(I2C_HandleTypeDef *hi2c)
{
  HAL_StatusTypeDef ret = HAL_OK;
  hi2c->Instance = I2C3;
  hi2c->Init.Timing = 0x00F07BFF;
  hi2c->Init.OwnAddress1 = 0;
  hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c->Init.OwnAddress2 = 0;
  hi2c->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if(HAL_I2C_Init(hi2c) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if(HAL_I2CEx_ConfigAnalogFilter(hi2c, I2C_ANALOGFILTER_DISABLE) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if(HAL_I2CEx_ConfigDigitalFilter(hi2c, 2) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

static void I2C3_MspInit(I2C_HandleTypeDef *i2cHandle)
{
  UNUSED(i2cHandle);
  GPIO_InitTypeDef gpio_init;

  /* Enable I2C clock */
  __HAL_RCC_I2C3_CLK_ENABLE();

  /**I2C3 GPIO Configuration    
   PG7     ------> I2C3_SCL
   PG8     ------> I2C3_SDA
   */

  /* Enable GPIO clock */
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableIO2VM();
  while(!(PWR->SVMCR & PWR_SVMCR_IO2VMEN)){}
  HAL_PWREx_EnableVddIO2();
  while(!(PWR->SVMSR & PWR_SVMSR_VDDIO2RDY)){}

  /* Configure I2C SDA Line */
  gpio_init.Pin = GPIO_PIN_8;
  gpio_init.Mode = GPIO_MODE_AF_OD;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init.Alternate = GPIO_AF4_I2C3;
  HAL_GPIO_Init(GPIOG, &gpio_init);

  /* Configure I2C SCL Line */
  gpio_init.Pin = GPIO_PIN_7;
  gpio_init.Mode = GPIO_MODE_AF_OD;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init.Alternate = GPIO_AF4_I2C3;
  HAL_GPIO_Init(GPIOG, &gpio_init);

}

static void I2C3_MspDeInit(I2C_HandleTypeDef *i2cHandle)
{
  UNUSED(i2cHandle);

  /* Peripheral clock disable */
  __HAL_RCC_I2C3_CLK_DISABLE();

  /**I2C3 GPIO Configuration
   PG7     ------> I2C3_SCL
   PG8     ------> I2C3_SDA
   */
  HAL_GPIO_DeInit(GPIOG, GPIO_PIN_7);

  HAL_GPIO_DeInit(GPIOG, GPIO_PIN_8);
}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
__weak HAL_StatusTypeDef MX_USART3_UART_Init(UART_HandleTypeDef *huart)
{
  HAL_StatusTypeDef ret = HAL_OK;
  huart->Instance = USART3;
  huart->Init.BaudRate = 115200;
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = UART_STOPBITS_1;
  huart->Init.Parity = UART_PARITY_NONE;
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
  huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if(HAL_UART_Init(huart) != HAL_OK)
  {
    ret = HAL_ERROR;
  }
  if(HAL_UARTEx_SetTxFifoThreshold(huart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    ret = HAL_ERROR;
  }
  if(HAL_UARTEx_SetRxFifoThreshold(huart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    ret = HAL_ERROR;
  }
  if(HAL_UARTEx_DisableFifoMode(huart) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

/**
 * @brief UART MSP DeInitialization
 * This function configures the hardware resources used in this example
 * @param huart: UART handle pointer
 * @retval None
 */
void USART3_MspInit(UART_HandleTypeDef *huart)
{

  GPIO_InitTypeDef GPIO_InitStruct =
  {
      0 };
  if(huart->Instance == USART3)
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**USART3 GPIO Configuration    
     PA5     ------> USART3_RX
     PA7     ------> USART3_TX
     PD12     ------> USART3_RTS
     PD11     ------> USART3_CTS
     */
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  }
}

/**
 * @brief UART MSP Initialization
 * This function configures the hardware resources used in this example
 * @param huart: UART handle pointer
 * @retval None
 */
void USART3_MspDeInit(UART_HandleTypeDef *huart)
{
  UNUSED(huart);

  /* Peripheral clock disable */
  __HAL_RCC_USART3_CLK_DISABLE();

  /**USART3 GPIO Configuration    
   PA5     ------> USART3_RX
   PA7     ------> USART3_TX
   PD12     ------> USART3_RTS
   PD11     ------> USART3_CTS
   */
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5 | GPIO_PIN_7);
  HAL_GPIO_DeInit(GPIOD, GPIO_PIN_11 | GPIO_PIN_12);
}
