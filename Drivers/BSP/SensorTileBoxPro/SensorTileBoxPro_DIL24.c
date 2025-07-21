/**
  ******************************************************************************
  * @file    SensorTileBoxPro_DIL24.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.2.0
  * @date    03-Jun-2024
  * @brief   Source file for the BSP Common driver
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

/* Includes ------------------------------------------------------------------*/
#include "SensorTileBoxPro_DIL24.h"
#include "SensorTileBoxPro.h"
#include "app_manager.h"

extern TargetFlags_t TargetBoardFlags;

/** @defgroup SENSORTILEBOXPRO_DIL24
  * @{
  */

/** @defgroup SENSORTILEBOXPRO_DIL24 Private Variables
  * @{
  */
uint8_t comm_protocol = NO_COM_SEL;

#if (BSP_DIL_ENV_INSTANCES_NBR > 0)
static BSP_ENV_SENSOR_Ctx_t EnvCtx[BSP_DIL_ENV_INSTANCES_NBR];
static ENV_SENSOR_FuncDrv_t *EnvFuncDrv[BSP_DIL_ENV_INSTANCES_NBR][BSP_DIL_ENV_FUNCTIONS_NBR];
static ENV_SENSOR_CommonDrv_t *EnvDrv[BSP_DIL_ENV_INSTANCES_NBR];

extern void
*DilEnvCompObj[BSP_DIL_ENV_INSTANCES_NBR]; /* This "redundant" line is here to fulfil MISRA C-2012 rule 8.4 */
void *DilEnvCompObj[BSP_DIL_ENV_INSTANCES_NBR];
#endif

#if (BSP_DIL_MOTION_INSTANCES_NBR > 0)
static BSP_MOTION_SENSOR_Ctx_t MotionCtx[BSP_DIL_MOTION_INSTANCES_NBR];
static MOTION_SENSOR_FuncDrv_t *MotionFuncDrv[BSP_DIL_MOTION_INSTANCES_NBR][BSP_DIL_MOTION_FUNCTIONS_NBR];
static MOTION_SENSOR_CommonDrv_t *MotionDrv[BSP_DIL_MOTION_INSTANCES_NBR];

extern void
*DilMotionCompObj[BSP_DIL_MOTION_INSTANCES_NBR]; /* This "redundant" line is here to fulfil MISRA C-2012 rule 8.4 */
void *DilMotionCompObj[BSP_DIL_MOTION_INSTANCES_NBR];
#endif

//#if (BSP_DIL_TMOS_INSTANCES_NBR > 0)
//static BSP_TMOS_SENSOR_Ctx_t TmosCtx[BSP_DIL_TMOS_INSTANCES_NBR];
//static TMOS_SENSOR_FuncDrv_t *TmosFuncDrv[BSP_DIL_TMOS_INSTANCES_NBR][BSP_DIL_TMOS_FUNCTIONS_NBR];
//static TMOS_SENSOR_CommonDrv_t *TmosDrv[BSP_DIL_TMOS_INSTANCES_NBR];
//
//extern void *DilTmosCompObj[BSP_DIL_TMOS_INSTANCES_NBR]; /* This "redundant" line is here to fulfil MISRA C-2012 rule 8.4 */
//void *DilTmosCompObj[BSP_DIL_TMOS_INSTANCES_NBR];
//#endif


/* We define a jump table in order to get the correct index from the desired function. */
/* This table should have a size equal to the maximum value of a function plus 1.      */

static uint32_t FunctionIndex[9] = {0xFF, 0, 1, 0xFF, 2, 0xFF, 0xFF, 0xFF, 3}; /// che cazzo e' sta cosa?!? nel canister era cosi' lo rimetto cos� e vediamo
/// this above is an array with the indexes to get a correspondence between the Functions variable called in the Probes (that are defined as 1-2-4-8-etc.) and the indexes of the driver functions

/** @defgroup SENSORTILEBOXPRO_DIL24 Private Functions
  * @{
  */



/**
  * @brief  Initialize I2C for DIL24.
  * @retval BSP status
  */
int32_t BSP_DIL24_I2C_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;
  ret = BSP_I2C3_Init();
  comm_protocol = DIL24_I2C;
  return ret;
}


/**
  * @brief  DeInitialize I2C for DIL24.
  * @retval BSP status
  */
int32_t BSP_DIL24_I2C_DeInit(void)
{
  int32_t ret = BSP_ERROR_NONE;
  ret = BSP_I2C3_DeInit();
  comm_protocol = NO_COM_SEL;
  return ret;
}


/**
  * @brief  Read DIL24 via I2C.
  * @retval BSP status
  */
int32_t BSP_DIL24_I2C_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_NONE;

  if (HAL_I2C_Mem_Read(&hi2c3, DevAddr, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length, BUS_I2C3_POLL_TIMEOUT) != HAL_OK)
  {
    if (HAL_I2C_GetError(&hi2c3) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
  return ret;
}


/**
  * @brief  Write DIL24 via I2C.
  * @retval BSP status
  */
int32_t BSP_DIL24_I2C_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_NONE;

  if (HAL_I2C_Mem_Write(&hi2c3, DevAddr, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length, BUS_I2C3_POLL_TIMEOUT) != HAL_OK)
  {
    if (HAL_I2C_GetError(&hi2c3) == HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret =  BSP_ERROR_PERIPH_FAILURE;
    }
  }
  return ret;
}


/**
  * @brief  Read Sensirion device data through BUS
  * @param  DevAddr Device address on Bus.
/// ** @param  Reg    there is no Reg parameter for sensirion humidity sensor
  * @param  pData  Pointer to data buffer to read
  * @param  Length Data Length
  * @retval BSP status
  */
int32_t BSP_I2C_Sensirion_Read(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_NONE;

  if (HAL_I2C_Master_Receive(&hi2c3, DevAddr, pData, Length, BUS_I2C3_POLL_TIMEOUT) != HAL_OK)
  {
    if (HAL_I2C_GetError(&hi2c3) != HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret =  BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return ret;
}


/**
  * @brief  Write a value in a Sensirion device through BUS.
  * @param  DevAddr Device address on Bus.
/// * @param  Reg    no reg in sensirion humidity device
  * @param  pData  Pointer to data buffer to write
  * @param  Length Data Length
  * @retval BSP status
  */
int32_t BSP_I2C_Sensirion_Write(uint16_t DevAddr, uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_NONE;

  if (HAL_I2C_Master_Transmit(&hi2c3, DevAddr, pData, 1, BUS_I2C3_POLL_TIMEOUT) != HAL_OK)
  {
    if (HAL_I2C_GetError(&hi2c3) != HAL_I2C_ERROR_AF)
    {
      ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
    }
    else
    {
      ret =  BSP_ERROR_PERIPH_FAILURE;
    }
  }
  return ret;
}


/**
  * @brief  Read Sensirion Serial number data through BUS
  * @param  DevAddr Device address on Bus.
/// ** @param  Reg    there is no Reg parameter for sensirion humidity sensor
  * @param  pData  Pointer to data buffer to read
  * @param  Length Data Length
  * @retval BSP status
  */
int32_t BSP_I2C_Sensirion_GetSerial(uint16_t DevAddr, uint32_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_NONE;
  SHT40AD1B_IO_t          io_ctx;
  uint32_t                serial_ID;
  static SHT40AD1B_Object_t sht40ad1b_obj_0;

  /* Configure the environmental sensor driver */
  io_ctx.BusType     = SHT40AD1B_I2C_BUS; /* I2C */
  io_ctx.Address     = SHT40AD1B_I2C_ADDRESS;
  io_ctx.Init        = BSP_I2C3_Init;
  io_ctx.DeInit      = BSP_I2C3_DeInit;
  io_ctx.Read        = BSP_I2C_Sensirion_Read;
  io_ctx.Write       = BSP_I2C_Sensirion_Write;
  io_ctx.GetTick     = BSP_GetTick;
  io_ctx.Delay       = HAL_Delay;       /// added this because there are delays in the drivers

  if (SHT40AD1B_RegisterBusIO(&sht40ad1b_obj_0, &io_ctx) != SHT40AD1B_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }

  if (sht40ad1b_serial_get(&sht40ad1b_obj_0.Ctx, &serial_ID) != SHT40AD1B_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }

  *pData = serial_ID;

  return ret;
}


/**
  * @}
  */



/**
  * @brief  Initialize SPI for DIL24.
  * @retval BSP status
  */
int32_t BSP_DIL24_SPI_Init(void)
{

  comm_protocol = DIL24_SPI;
  return BSP_NOT_IMPLEMENTED;
}


/**
  * @brief  DeInitialize SPI for DIL24.
  * @retval BSP status
  */
int32_t BSP_DIL24_SPI_DeInit(void)
{

  comm_protocol = NO_COM_SEL;
  return BSP_NOT_IMPLEMENTED;
}


/**
  * @brief  Read DIL24 via SPI.
  * @retval BSP status
  */
int32_t BSP_DIL24_SPI_Read(void)
{

  return BSP_NOT_IMPLEMENTED;
}


/**
  * @brief  Write DIL24 via SPI.
  * @retval BSP status
  */
int32_t BSP_DIL24_SPI_Write(void)
{

  return BSP_NOT_IMPLEMENTED;
}

/**
  * @}
  */


/**
  * @brief  Read DIL24.
  * @retval BSP status
  */
int32_t BSP_DIL24_Read(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_NONE;

  switch (comm_protocol)
  {
    case DIL24_I2C:
      ret = BSP_DIL24_I2C_ReadReg(DevAddr, Reg, pData, Length);
      break;
    case DIL24_SPI:
      ret = BSP_DIL24_SPI_Read();
      break;
    case NO_COM_SEL:
      ret = BSP_ERROR_NO_INIT;
      PRINT_DBG("ERROR: no comm protocol for DIL24 initialized\r\n");
      break;
    default:
      PRINT_DBG("ERROR: wrong comm_protocol value for read\r\n");
      break;
  }
  return ret;
}


/**
  * @brief  Read DIL24.
  * @retval BSP status
  */
int32_t BSP_DIL24_Write(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  int32_t ret = BSP_ERROR_NONE;

  switch (comm_protocol)
  {
    case DIL24_I2C:
      ret = BSP_DIL24_I2C_WriteReg(DevAddr, Reg, pData, Length);
      break;
    case DIL24_SPI:
      ret = BSP_DIL24_SPI_Write();
      break;
    case NO_COM_SEL:
      ret = BSP_ERROR_NO_INIT;
      PRINT_DBG("ERROR: no comm protocol for DIL24 initialized\r\n");
      break;
    default:
      PRINT_DBG("ERROR: wrong comm_protocol value for write\r\n");
      break;
  }
  return ret;
}



/**
  * @brief  Initializes a DIL24 sensor
  * @param  Instance: sensor instance to be used
  * @param  Functions: sensor functions. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  *         - ENV_HUMIDITY
  *         - MOTION_GYRO
  *         - MOTION_ACCELERO
  *         - MOTION_MAGNETO
  *         - DIL24_TMOS NOT YET AVAILABLE
  * @retval BSP status
  */
int32_t BSP_DIL_SENSOR_Init(uint32_t Instance, uint32_t Functions)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t function = ENV_TEMPERATURE;
  uint32_t i;
  uint32_t component_functions = 0;
#if ((BSP_DIL_ENV_INSTANCES_NBR + BSP_DIL_TMOS_INSTANCES_NBR) > 0)
  BSP_ENV_SENSOR_Capabilities_t cap_env;
#endif
#if (BSP_DIL_MOTION_INSTANCES_NBR > 0)
  BSP_MOTION_SENSOR_Capabilities_t cap_mot;
#endif

  /// to use the DIL24 functions set but use Functions right as before I make this check
  if (Functions < 8)
  {
    /// sensor is ENVIRONMENTAL
    switch (Instance)
    {
#if (USE_DIL_SENSOR_SHT40AD1B_0 == 1)
      case SHT40AD1B_0:
        if (SHT40AD1B_0_Probe(Functions) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_NO_INIT;
        }
//      if (EnvDrv[Instance]->GetCapabilities(DilEnvCompObj[Instance], (void *)&cap_env) != BSP_ERROR_NONE)
        if (EnvDrv[Instance]->GetCapabilities(DilEnvCompObj[Instance], (void *)&cap_env) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_UNKNOWN_COMPONENT;
        }
        if (cap_env.Temperature == 1U)
        {
          component_functions |= ENV_TEMPERATURE;
        }
        if (cap_env.Humidity == 1U)
        {
          component_functions |= ENV_HUMIDITY;
        }
        break;
#endif
#if (USE_DIL_SENSOR_STHS34PF80_0 == 1)
      case STHS34PF80_0:
        if (STHS34PF80_0_Probe(Functions) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_NO_INIT;
        }
//      if (EnvDrv[Instance]->GetCapabilities(DilEnvCompObj[Instance], (void *)&cap_env) != BSP_ERROR_NONE)
        if (EnvDrv[Instance]->GetCapabilities(DilEnvCompObj[Instance], (void *)&cap_env) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_UNKNOWN_COMPONENT;
        }
        if (cap_env.Temperature == 1U)
        {
          component_functions |= ENV_TEMPERATURE;
        }
        if (cap_env.Humidity == 1U)
        {
          component_functions |= ENV_HUMIDITY;
        }
        if (cap_env.Pressure == 1U)
        {
          component_functions |= ENV_PRESSURE;
        }
        break;
#endif
      default:
        ret = BSP_ERROR_WRONG_PARAM;
        break;
    }

    if (ret != BSP_ERROR_NONE)
    {
      return ret;
    }

#if (BSP_DIL_ENV_INSTANCES_NBR > 0)
    for (i = 0; i < BSP_ENV_FUNCTIONS_NBR; i++)
    {
      if (((Functions & function) == function) && ((component_functions & function) == function))
      {
//        if (EnvFuncDrv[Instance][FunctionIndex[function]]->Enable(DilEnvCompObj[Instance]) != BSP_ERROR_NONE)
        if (EnvFuncDrv[Instance][FunctionIndex[function]]->Enable(DilEnvCompObj[Instance]) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_COMPONENT_FAILURE;
        }
      }
      function = function << 1;
    }
#endif
  }
  else if (Functions < 64)
  {
    /// sensor is MOTION
    Functions = Functions >> 3; /// to use the DIL24 functions set but get Functions right as for ENV/MOT
    switch (Instance)
    {
#if (USE_DIL_SENSOR_LSM6DSO32_0 == 1)
      case LSM6DSO32_0:
        if (LSM6DSO32_0_Probe(Functions) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_NO_INIT;
        }
//      if (MotionDrv[Instance]->GetCapabilities(DilMotionCompObj[Instance], (void *)&cap_mot) != BSP_ERROR_NONE)
        if (MotionDrv[Instance]->GetCapabilities(DilMotionCompObj[Instance], (void *)&cap_mot) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_UNKNOWN_COMPONENT;
        }
        if (cap_mot.Acc == 1U)
        {
          component_functions |= MOTION_ACCELERO;
        }
        if (cap_mot.Gyro == 1U)
        {
          component_functions |= MOTION_GYRO;
        }
        if (cap_mot.Magneto == 1U)
        {
          component_functions |= MOTION_MAGNETO;
        }
        break;
#endif
        //LSM6DSO16IS
#if (USE_DIL_SENSOR_LSM6DSO16IS_0 == 1)
      case LSM6DSO16IS_0:
        if (LSM6DSO16IS_0_Probe(Functions) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_NO_INIT;
        }
//      if (MotionDrv[Instance]->GetCapabilities(DilMotionCompObj[Instance], (void *)&cap_mot) != BSP_ERROR_NONE)
        if (MotionDrv[Instance]->GetCapabilities(DilMotionCompObj[Instance], (void *)&cap_mot) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_UNKNOWN_COMPONENT;
        }
        if (cap_mot.Acc == 1U)
        {
          component_functions |= MOTION_ACCELERO;
        }
        if (cap_mot.Gyro == 1U)
        {
          component_functions |= MOTION_GYRO;
        }
        if (cap_mot.Magneto == 1U)
        {
          component_functions |= MOTION_MAGNETO;
        }
        break;
#endif
        //LSM6DSV32X
#if (USE_DIL_SENSOR_LSM6DSV32X_0 == 1)
      case LSM6DSV32X_0:
        if (LSM6DSV32X_0_Probe(Functions) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_NO_INIT;
        }
//      if (MotionDrv[Instance]->GetCapabilities(DilMotionCompObj[Instance], (void *)&cap_mot) != BSP_ERROR_NONE)
        if (MotionDrv[Instance]->GetCapabilities(DilMotionCompObj[Instance], (void *)&cap_mot) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_UNKNOWN_COMPONENT;
        }
        if (cap_mot.Acc == 1U)
        {
          component_functions |= MOTION_ACCELERO;
        }
        if (cap_mot.Gyro == 1U)
        {
          component_functions |= MOTION_GYRO;
        }
        if (cap_mot.Magneto == 1U)
        {
          component_functions |= MOTION_MAGNETO;
        }
        break;
#endif
#if (USE_DIL_SENSOR_LSM6DSV16BX_0 == 1)
      case LSM6DSV16BX_0:
        if (LSM6DSV16BX_0_Probe(Functions) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_NO_INIT;
        }
//      if (MotionDrv[Instance]->GetCapabilities(DilMotionCompObj[Instance], (void *)&cap_mot) != BSP_ERROR_NONE)
        if (MotionDrv[Instance]->GetCapabilities(DilMotionCompObj[Instance], (void *)&cap_mot) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_UNKNOWN_COMPONENT;
        }
        if (cap_mot.Acc == 1U)
        {
          component_functions |= MOTION_ACCELERO;
        }
        if (cap_mot.Gyro == 1U)
        {
          component_functions |= MOTION_GYRO;
        }
        if (cap_mot.Magneto == 1U)
        {
          component_functions |= MOTION_MAGNETO;
        }
        break;
#endif
        //LSM6DSV80X
#if (USE_DIL_SENSOR_LSM6DSV80X_0 == 1)
      case LSM6DSV80X_0:
        if (LSM6DSV80X_0_Probe(Functions) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_NO_INIT;
        }
//      if (MotionDrv[Instance]->GetCapabilities(DilMotionCompObj[Instance], (void *)&cap_mot) != BSP_ERROR_NONE)
        if (MotionDrv[Instance]->GetCapabilities(DilMotionCompObj[Instance], (void *)&cap_mot) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_UNKNOWN_COMPONENT;
        }
        if (cap_mot.Acc == 1U)
        {
          component_functions |= MOTION_ACCELERO;
        }
        if (cap_mot.Gyro == 1U)
        {
          component_functions |= MOTION_GYRO;
        }
        if (cap_mot.Magneto == 1U)
        {
          component_functions |= MOTION_MAGNETO;
        }
        break;
#endif        
          //ST1VAFE6AX
#if (USE_DIL_SENSOR_ST1VAFE6AX_0 == 1)
    case   ST1VAFE6AX_0:
      if (  ST1VAFE6AX_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if (MotionDrv[Instance]->GetCapabilities(DilMotionCompObj[Instance], (void *)&cap_mot) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if (cap_mot.Acc == 1U)
      {
        component_functions |= MOTION_ACCELERO;
      }
      if (cap_mot.Gyro == 1U)
      {
        component_functions |= MOTION_GYRO;
      }
      if (cap_mot.Magneto == 1U)
      {
        component_functions |= MOTION_MAGNETO;
      }
      break;
#endif
      //ST1VAFE3BX
#if (USE_DIL_SENSOR_ST1VAFE3BX_0 == 1)
    case ST1VAFE3BX_0:
      if (ST1VAFE3BX_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if (MotionDrv[Instance]->GetCapabilities(DilMotionCompObj[Instance], (void *)&cap_mot) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if (cap_mot.Acc == 1U)
      {
        component_functions |= MOTION_ACCELERO;
      }
      if (cap_mot.Gyro == 1U)
      {
        component_functions |= MOTION_GYRO;
      }
      if (cap_mot.Magneto == 1U)
      {
        component_functions |= MOTION_MAGNETO;
      }
      break;
#endif
    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
    }

    if (ret != BSP_ERROR_NONE)
    {
      return ret;
    }

#if (BSP_DIL_MOTION_INSTANCES_NBR > 0)
    for (i = 0; i < BSP_MOTION_FUNCTIONS_NBR; i++)
    {
      if (((Functions & function) == function) && ((component_functions & function) == function))
      {
//        if(MotionFuncDrv[Instance][FunctionIndex[function]]->Enable(DilMotionCompObj[Instance]) != BSP_ERROR_NONE)
        if (MotionFuncDrv[Instance][FunctionIndex[function]]->Enable(DilMotionCompObj[Instance]) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_COMPONENT_FAILURE;
        }
      }
      function = function << 1;
    }
#endif
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}


/**
  * @brief  Enable (turn On) a DIL24 sensor
  * @param  Instance: sensor instance to be used
  * @param  Functions: sensor functions. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  *         - ENV_HUMIDITY
  *         - MOTION_GYRO
  *         - MOTION_ACCELERO
  *         - MOTION_MAGNETO
  *         - DIL24_TMOS NOT YET AVAILABLE
  * @retval BSP status
  */
int32_t BSP_DIL_SENSOR_Enable(uint32_t Instance, uint32_t Functions)
{
  int32_t ret = BSP_ERROR_NONE;

  /// to use the DIL24 functions set but use Functions right as before I make this check
  if (Functions < 8)
  {
    /// sensor is ENVIRONMENTAL (TMOS is ENV)
    if ((EnvCtx[Instance].Functions & Functions) == Functions)
    {
      if (EnvFuncDrv[Instance][FunctionIndex[Functions]]->Enable(DilEnvCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }
  else if (Functions < 64)
  {
    /// sensor is MOTION
    Functions = Functions >> 3; /// to use the DIL24 functions set but get Functions right as for ENV/MOT
    if ((MotionCtx[Instance].Functions & Functions) == Functions)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Functions]]->Enable(DilMotionCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}


/**
  * @brief  Disable (turn On) a DIL24 sensor
  * @param  Instance: sensor instance to be used
  * @param  Functions: sensor functions. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  *         - ENV_HUMIDITY
  *         - MOTION_GYRO
  *         - MOTION_ACCELERO
  *         - MOTION_MAGNETO
  *         - DIL24_TMOS NOT YET AVAILABLE
  * @retval BSP status
  */
int32_t BSP_DIL_SENSOR_Disable(uint32_t Instance, uint32_t Functions)
{
  int32_t ret = BSP_ERROR_NONE;

  /// to use the DIL24 functions set but use Functions right as before I make this check
  if (Functions < 8)
  {
    /// sensor is ENVIRONMENTAL (TMOS is ENV)
    if ((EnvCtx[Instance].Functions & Functions) == Functions)
    {
      if (EnvFuncDrv[Instance][FunctionIndex[Functions]]->Disable(DilEnvCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }
  else if (Functions < 64)
  {
    /// sensor is MOTION
    Functions = Functions >> 3; /// to use the DIL24 functions set but get Functions right as for ENV/MOT
    if ((MotionCtx[Instance].Functions & Functions) == Functions)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Functions]]->Disable(DilMotionCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}

/**
  * @brief  Select ODR on a DIL24 sensor
  * @param  Instance: sensor instance to be used
  * @param  Functions: sensor functions. Could be :
  *         - DIL24_TEMPERATURE
  *         - DIL24_PRESSURE
  *         - DIL24_HUMIDITY
  *         - DIL24_GYRO
  *         - DIL24_ACCELERO
  *         - DIL24_MAGNETO
  *         - DIL24_TMOS // Not currently supported
  * @retval BSP status
  */
int32_t BSP_DIL_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Functions, float_t Odr)
{
  int32_t ret = BSP_ERROR_NONE;

  /// to use the DIL24 functions set but use Functions right as before I make this check
  if (Functions < 8)
  {
    /// sensor is ENVIRONMENTAL (TMOS is ENV)
    if ((EnvCtx[Instance].Functions & Functions) == Functions)
    {
      if (EnvFuncDrv[Instance][FunctionIndex[Functions]]->SetOutputDataRate(DilEnvCompObj[Instance], Odr) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }
  else if (Functions < 64)
  {
    /// sensor is MOTION
    Functions = Functions >> 3; /// to use the DIL24 functions set but get Functions right as for ENV/MOT
    if ((MotionCtx[Instance].Functions & Functions) == Functions)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Functions]]->SetOutputDataRate(DilMotionCompObj[Instance],
                                                                               Odr) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}


/**
  * @brief  Get ODR on a DIL24 sensor
  * @param  Instance: sensor instance to be used
  * @param  Functions: sensor functions. Could be :
  *         - DIL24_TEMPERATURE
  *         - DIL24_PRESSURE
  *         - DIL24_HUMIDITY
  *         - DIL24_GYRO
  *         - DIL24_ACCELERO
  *         - DIL24_MAGNETO
  *         - DIL24_TMOS // Not currently supported
  * @retval BSP status
  * TODO: check if works
  */
int32_t BSP_DIL_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Functions, float_t *Odr)
{
  int32_t ret = BSP_ERROR_NONE;

  /// to use the DIL24 functions set but use Functions right as before I make this check
  if (Functions < 8)
  {
    /// sensor is ENVIRONMENTAL (TMOS is ENV)
    if ((EnvCtx[Instance].Functions & Functions) == Functions)
    {
      if (EnvFuncDrv[Instance][FunctionIndex[Functions]]->GetOutputDataRate(DilEnvCompObj[Instance], Odr) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }
  else if (Functions < 64)
  {
    /// sensor is MOTION
    Functions = Functions >> 3; /// to use the DIL24 functions set but get Functions right as for ENV/MOT
    if ((MotionCtx[Instance].Functions & Functions) == Functions)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Functions]]->GetOutputDataRate(DilMotionCompObj[Instance],
                                                                               Odr) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }

  return ret;
}


/**
* @brief  Select Full Scale on a DIL24 sensor
* @param  Instance: sensor instance to be used
* @param  Functions: sensor functions. Could be :
*         - DIL24_TEMPERATURE
*         - DIL24_PRESSURE
*         - DIL24_HUMIDITY
*         - DIL24_GYRO
*         - DIL24_ACCELERO
*         - DIL24_MAGNETO
*         - DIL24_TMOS // Not currently supported
*         - DIL24_ACCELERO_HG
* @retval BSP status
*/
int32_t BSP_DIL_SENSOR_SetFullScale(uint32_t Instance, uint32_t Functions, float full_scale)
{
  int32_t ret = BSP_ERROR_NONE;

  /// to use the DIL24 functions set but use Functions right as before I make this check
  if(Functions < 8){
    /// ENVIRONMENTAL should not have a full scale
  }else if(Functions < 64){
    /// sensor is MOTION
    Functions = Functions>>3;   /// to use the DIL24 functions set but get Functions right as for ENV/MOT
    if ((MotionCtx[Instance].Functions & Functions) == Functions)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Functions]]->SetFullScale(DilMotionCompObj[Instance], full_scale) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }else{
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  return ret;
}



/**
  * @brief  Get DIL24 sensor data
  * @param  Instance  sensor instance to be used
  * @param  Function sensor function. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  *         - ENV_HUMIDITY
  *         - MOTION_GYRO
  *         - MOTION_ACCELERO
  *         - MOTION_MAGNETO
  *         - DIL24_TMOS
  * @param  Data pointer to sensor output
  * @retval BSP status
  */
int32_t BSP_DIL_SENSOR_GetData(uint32_t Instance, uint32_t Functions, BSP_DIL24_Data_t *Data)
{
  int32_t ret = BSP_ERROR_NONE;
  float t, p, h = 0;
  /// to use the DIL24 functions set but use Functions right as before I make this check
  if (Functions < 8)
  {
#if (BSP_DIL_ENV_INSTANCES_NBR > 0)
    /// sensor is ENVIRONMENTAL
    if ((Functions & ENV_TEMPERATURE) == ENV_TEMPERATURE)
    {
      BSP_DIL_ENV_SENSOR_GetValue(Instance, Functions, &t);
      Data->temp = (uint16_t)t;
    }
    if ((Functions & ENV_PRESSURE) == ENV_PRESSURE)
    {
      BSP_DIL_ENV_SENSOR_GetValue(Instance, Functions, &p);
      Data->press = (uint32_t)p;
    }
    if ((Functions & ENV_HUMIDITY) == ENV_HUMIDITY)
    {
      BSP_DIL_ENV_SENSOR_GetValue(Instance, Functions, &h);
      Data->hum = (uint16_t)h;
    }

  }
  else if (Functions >= 64)
  {
    /// sensor is TMOS
    BSP_DIL_TMOS_SENSOR_GetData(Instance, &Data->tmos);
#endif
  }
#if (BSP_DIL_MOTION_INSTANCES_NBR > 0)
  else if (Functions < 64)
  {
    /// sensor is MOTION
    Functions = Functions >> 3; /// to use the DIL24 functions set but get Functions right as for ENV/MOT
    if ((Functions & MOTION_GYRO) == MOTION_GYRO)
    {
      BSP_DIL_MOTION_SENSOR_GetAxes(Instance, Functions, &Data->gyro);
    }
    if ((Functions & MOTION_ACCELERO) == MOTION_ACCELERO)
    {
      BSP_DIL_MOTION_SENSOR_GetAxes(Instance, Functions, &Data->acc);
    }
    if ((Functions & MOTION_MAGNETO) == MOTION_MAGNETO)
    {
      BSP_DIL_MOTION_SENSOR_GetAxes(Instance, Functions, &Data->mag);
    }
  }
#endif
  return ret;
}


#if (BSP_DIL_ENV_INSTANCES_NBR > 0)
/**
  * @brief  Get environmental sensor value
  * @param  Instance environmental sensor instance to be used
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  * @param  Value pointer to environmental sensor value
  * @retval BSP status
  */
int32_t BSP_DIL_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float *Value)
{
  int32_t ret;

  if ((EnvCtx[Instance].Functions & Function) == Function)
  {
    if (EnvFuncDrv[Instance][FunctionIndex[Function]]->GetValue(DilEnvCompObj[Instance], Value) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }
  else
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }

  return ret;
}
#endif


#if (BSP_DIL_MOTION_INSTANCES_NBR > 0)
/**
  * @brief  Get motion sensor axes data
  * @param  Instance Motion sensor instance
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO
  *         - MOTION_ACCELERO
  *         - MOTION_MAGNETO
  * @param  Axes pointer to axes data structure
  * @retval BSP status
  */
int32_t BSP_DIL_MOTION_SENSOR_GetAxes(uint32_t Instance, uint32_t Function, BSP_MOTION_SENSOR_Axes_t *Axes)
{
  int32_t ret;

  if ((MotionCtx[Instance].Functions & Function) == Function)
  {
    if (MotionFuncDrv[Instance][FunctionIndex[Function]]->GetAxes(DilMotionCompObj[Instance], Axes) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }
  else
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }

  return ret;
}
#endif


#if (BSP_DIL_TMOS_INSTANCES_NBR > 0)
/**
  * @brief  Get TMOS sensor data
  * @param  Instance TMOS sensor instance
  * @param  OutData pointer to TMOS data structure
  * @retval BSP status
  */
int32_t BSP_DIL_TMOS_SENSOR_GetData(uint32_t Instance, uint32_t Function, BSP_TMOS_Data_t *OutData)
{
  int32_t ret;

  if ((TmosCtx[Instance].Functions & Function) == Function)
  {
    if (TmosFuncDrv[Instance][FunctionIndex[Function]]->GetData(DilTmosCompObj[Instance], OutData) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }
  else
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }

  return ret;
}
#endif


#if (USE_DIL_SENSOR_SHT40AD1B_0 == 1)
static int32_t SHT40AD1B_0_Probe(uint32_t Functions)
{
  SHT40AD1B_IO_t          io_ctx;
  uint8_t                 id;
  uint32_t                serial_ID;
  int32_t                 ret = BSP_ERROR_NONE;
  static SHT40AD1B_Object_t sht40ad1b_obj_0;
  SHT40AD1B_Capabilities_t  cap;

  /* Configure the environmental sensor driver */
  io_ctx.BusType     = SHT40AD1B_I2C_BUS; /* I2C */
  io_ctx.Address     = SHT40AD1B_I2C_ADDRESS;
  io_ctx.Init        = BSP_I2C3_Init;
  io_ctx.DeInit      = BSP_I2C3_DeInit;
  io_ctx.Read        = BSP_I2C_Sensirion_Read;
  io_ctx.Write       = BSP_I2C_Sensirion_Write;
  io_ctx.GetTick     = BSP_GetTick;
  io_ctx.Delay       = HAL_Delay;       /// added this because there are delays in the drivers

  if (SHT40AD1B_RegisterBusIO(&sht40ad1b_obj_0, &io_ctx) != SHT40AD1B_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (SHT40AD1B_ReadID(&sht40ad1b_obj_0, &id) != SHT40AD1B_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != SHT40AD1B_ID) // qui fa un controllo del cazzo, provo a leggere il serial number sotto
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (sht40ad1b_serial_get(&sht40ad1b_obj_0.Ctx, &serial_ID) != SHT40AD1B_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  //  else if (check_serial(serial_ID) != SHT40AD1B_OK) /// bello il serial number ma non so se e' utile
  //  {
  //    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  //  }
  else
  {
    (void)SHT40AD1B_GetCapabilities(&sht40ad1b_obj_0, &cap);
    EnvCtx[SHT40AD1B_0].Functions = ((uint32_t)cap.Temperature) | ((uint32_t)cap.Pressure << 1) | ((
                                      uint32_t)cap.Humidity << 2);
    DilEnvCompObj[SHT40AD1B_0] = &sht40ad1b_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    EnvDrv[SHT40AD1B_0] = (ENV_SENSOR_CommonDrv_t *)(void *)&SHT40AD1B_COMMON_Driver;

    if ((ret == BSP_ERROR_NONE) && ((Functions & ENV_HUMIDITY) == ENV_HUMIDITY) && (cap.Humidity == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      EnvFuncDrv[SHT40AD1B_0][FunctionIndex[ENV_HUMIDITY]] = (ENV_SENSOR_FuncDrv_t *)(void *)&SHT40AD1B_HUM_Driver;

      if (EnvDrv[SHT40AD1B_0]->Init(DilEnvCompObj[SHT40AD1B_0]) != SHT40AD1B_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & ENV_TEMPERATURE) == ENV_TEMPERATURE) && (cap.Temperature == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      EnvFuncDrv[SHT40AD1B_0][FunctionIndex[ENV_TEMPERATURE]] = (ENV_SENSOR_FuncDrv_t *)(void *)&SHT40AD1B_TEMP_Driver;

      if (EnvDrv[SHT40AD1B_0]->Init(DilEnvCompObj[SHT40AD1B_0]) != SHT40AD1B_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
  }

  return ret;
}
#endif


// new for LSM6DSO16IS
#if (USE_DIL_SENSOR_LSM6DSO16IS_0 == 1)
LSM6DSO16IS_Object_t lsm6dso16is_obj_0_copy;
/**
  * @brief  Register Bus IOs for LSM6DSO16IS instance
  * @param  Functions Motion sensor functions. Could be :
  *         - MOTION_GYRO and/or MOTION_ACCELERO
  * @retval BSP status
  */
static int32_t LSM6DSO16IS_0_Probe(uint32_t Functions)
{
  LSM6DSO16IS_IO_t              io_ctx;
  uint8_t                       id;
  static LSM6DSO16IS_Object_t   lsm6dso16is_obj_0;
  LSM6DSO16IS_Capabilities_t    cap;
  int32_t                       ret = BSP_ERROR_NONE;

  /* Configure the driver */

  io_ctx.BusType     = LSM6DSO16IS_I2C_BUS; /* I2C */
  io_ctx.Address     = LSM6DSO16IS_I2C_ADD_L;
  io_ctx.Init        = BSP_I2C3_Init;
  io_ctx.DeInit      = BSP_I2C3_DeInit;
  io_ctx.ReadReg     = BSP_I2C3_ReadReg;
  io_ctx.WriteReg    = BSP_I2C3_WriteReg;
  io_ctx.GetTick     = BSP_GetTick;

  if (LSM6DSO16IS_RegisterBusIO(&lsm6dso16is_obj_0, &io_ctx) != LSM6DSO16IS_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (LSM6DSO16IS_ReadID(&lsm6dso16is_obj_0, &id) != LSM6DSO16IS_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != LSM6DSO16IS_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void)LSM6DSO16IS_GetCapabilities(&lsm6dso16is_obj_0, &cap);
    MotionCtx[LSM6DSO16IS_0].Functions = ((uint32_t)cap.Gyro) | ((uint32_t)cap.Acc << 1) | ((uint32_t)cap.Magneto << 2);

    DilMotionCompObj[LSM6DSO16IS_0] = &lsm6dso16is_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[LSM6DSO16IS_0] = (MOTION_SENSOR_CommonDrv_t *)(void *)&LSM6DSO16IS_COMMON_Driver;

    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_GYRO) == MOTION_GYRO) && (cap.Gyro == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[LSM6DSO16IS_0][FunctionIndex[MOTION_GYRO]] = (MOTION_SENSOR_FuncDrv_t *)(void *)&LSM6DSO16IS_GYRO_Driver;

      if (MotionDrv[LSM6DSO16IS_0]->Init(DilMotionCompObj[LSM6DSO16IS_0]) != LSM6DSO16IS_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_ACCELERO) == MOTION_ACCELERO) && (cap.Acc == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[LSM6DSO16IS_0][FunctionIndex[MOTION_ACCELERO]] = (MOTION_SENSOR_FuncDrv_t *)(void *)&LSM6DSO16IS_ACC_Driver;

      if (MotionDrv[LSM6DSO16IS_0]->Init(DilMotionCompObj[LSM6DSO16IS_0]) != LSM6DSO16IS_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_MAGNETO) == MOTION_MAGNETO))
    {
      /* Return an error if the application try to initialize a function not supported by the component */
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  lsm6dso16is_obj_0_copy = lsm6dso16is_obj_0;
  return ret;
}
#endif




// new for LSM6DSV32X
#if (USE_DIL_SENSOR_LSM6DSV32X_0 == 1)
LSM6DSV32X_Object_t lsm6dsv32x_obj_0_copy;
/**
  * @brief  Register Bus IOs for LSM6DSV32X instance
  * @param  Functions Motion sensor functions. Could be :
  *         - MOTION_GYRO and/or MOTION_ACCELERO
  * @retval BSP status
  */
static int32_t LSM6DSV32X_0_Probe(uint32_t Functions)
{
  LSM6DSV32X_IO_t               io_ctx;
  uint8_t                       id;
  static LSM6DSV32X_Object_t    lsm6dsv32x_obj_0;
  LSM6DSV32X_Capabilities_t     cap;
  int32_t                       ret = BSP_ERROR_NONE;

  /* Configure the driver */

  io_ctx.BusType     = LSM6DSV32X_I2C_BUS; /* I2C */
  io_ctx.Address     = LSM6DSV32X_I2C_ADD_L;
  io_ctx.Init        = BSP_I2C3_Init;
  io_ctx.DeInit      = BSP_I2C3_DeInit;
  io_ctx.ReadReg     = BSP_I2C3_ReadReg;
  io_ctx.WriteReg    = BSP_I2C3_WriteReg;
  io_ctx.GetTick     = BSP_GetTick;

  if (LSM6DSV32X_RegisterBusIO(&lsm6dsv32x_obj_0, &io_ctx) != LSM6DSV32X_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (LSM6DSV32X_ReadID(&lsm6dsv32x_obj_0, &id) != LSM6DSV32X_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != LSM6DSV32X_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void)LSM6DSV32X_GetCapabilities(&lsm6dsv32x_obj_0, &cap);
    MotionCtx[LSM6DSV32X_0].Functions = ((uint32_t)cap.Gyro) | ((uint32_t)cap.Acc << 1) | ((uint32_t)cap.Magneto << 2);

    DilMotionCompObj[LSM6DSV32X_0] = &lsm6dsv32x_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[LSM6DSV32X_0] = (MOTION_SENSOR_CommonDrv_t *)(void *)&LSM6DSV32X_COMMON_Driver;

    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_GYRO) == MOTION_GYRO) && (cap.Gyro == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[LSM6DSV32X_0][FunctionIndex[MOTION_GYRO]] = (MOTION_SENSOR_FuncDrv_t *)(void *)&LSM6DSV32X_GYRO_Driver;

      if (MotionDrv[LSM6DSV32X_0]->Init(DilMotionCompObj[LSM6DSV32X_0]) != LSM6DSV32X_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_ACCELERO) == MOTION_ACCELERO) && (cap.Acc == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[LSM6DSV32X_0][FunctionIndex[MOTION_ACCELERO]] = (MOTION_SENSOR_FuncDrv_t *)(void *)&LSM6DSV32X_ACC_Driver;

      if (MotionDrv[LSM6DSV32X_0]->Init(DilMotionCompObj[LSM6DSV32X_0]) != LSM6DSV32X_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_MAGNETO) == MOTION_MAGNETO))
    {
      /* Return an error if the application try to initialize a function not supported by the component */
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  lsm6dsv32x_obj_0_copy = lsm6dsv32x_obj_0;
  return ret;
}
#endif

#if (USE_DIL_SENSOR_LSM6DSO32_0 == 1)
/**
  * @brief  Register Bus IOs for LSM6DSO32 instance
  * @param  Functions Motion sensor functions. Could be :
  *         - MOTION_GYRO and/or MOTION_ACCELERO
  * @retval BSP status
  */
static int32_t LSM6DSO32_0_Probe(uint32_t Functions)
{
  LSM6DSO32_IO_t            io_ctx;
  uint8_t                    id;
  static LSM6DSO32_Object_t lsm6dso32_obj_0;
  LSM6DSO32_Capabilities_t  cap;
  int32_t                    ret = BSP_ERROR_NONE;

  /* Configure the driver */

  io_ctx.BusType     = LSM6DSO32_I2C_BUS; /* I2C */
  io_ctx.Address     = LSM6DSO32_I2C_ADD_L;
  io_ctx.Init        = BSP_I2C3_Init;
  io_ctx.DeInit      = BSP_I2C3_DeInit;
  io_ctx.ReadReg     = BSP_I2C3_ReadReg;
  io_ctx.WriteReg    = BSP_I2C3_WriteReg;
  io_ctx.GetTick     = BSP_GetTick;

  if (LSM6DSO32_RegisterBusIO(&lsm6dso32_obj_0, &io_ctx) != LSM6DSO32_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (LSM6DSO32_ReadID(&lsm6dso32_obj_0, &id) != LSM6DSO32_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != LSM6DSO32_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void)LSM6DSO32_GetCapabilities(&lsm6dso32_obj_0, &cap);
    MotionCtx[LSM6DSO32_0].Functions = ((uint32_t)cap.Gyro) | ((uint32_t)cap.Acc << 1) | ((uint32_t)cap.Magneto << 2);

    DilMotionCompObj[LSM6DSO32_0] = &lsm6dso32_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[LSM6DSO32_0] = (MOTION_SENSOR_CommonDrv_t *)(void *)&LSM6DSO32_COMMON_Driver;

    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_GYRO) == MOTION_GYRO) && (cap.Gyro == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[LSM6DSO32_0][FunctionIndex[MOTION_GYRO]] = (MOTION_SENSOR_FuncDrv_t *)(void *)&LSM6DSO32_GYRO_Driver;

      if (MotionDrv[LSM6DSO32_0]->Init(DilMotionCompObj[LSM6DSO32_0]) != LSM6DSO32_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_ACCELERO) == MOTION_ACCELERO) && (cap.Acc == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[LSM6DSO32_0][FunctionIndex[MOTION_ACCELERO]] = (MOTION_SENSOR_FuncDrv_t *)(void *)&LSM6DSO32_ACC_Driver;

      if (MotionDrv[LSM6DSO32_0]->Init(DilMotionCompObj[LSM6DSO32_0]) != LSM6DSO32_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_MAGNETO) == MOTION_MAGNETO))
    {
      /* Return an error if the application try to initialize a function not supported by the component */
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}
#endif

#if (USE_DIL_SENSOR_LSM6DSV16BX_0 == 1)
/**
  * @brief  Register Bus IOs for LSM6DSV16X instance
  * @param  Functions Motion sensor functions. Could be :
  *         - MOTION_GYRO and/or MOTION_ACCELERO
  * @retval BSP status
  */
static int32_t LSM6DSV16BX_0_Probe(uint32_t Functions)
{
  LSM6DSV16BX_IO_t            io_ctx;
  uint8_t                    id;
  static LSM6DSV16BX_Object_t lsm6dsv16bx_obj_0;
  LSM6DSV16BX_Capabilities_t  cap;
  int32_t                    ret = BSP_ERROR_NONE;

  /* Configure the driver */

  io_ctx.BusType     = LSM6DSV16BX_I2C_BUS; /* I2C */
  io_ctx.Address     = LSM6DSV16BX_I2C_ADD_L;
  io_ctx.Init        = BSP_I2C3_Init;
  io_ctx.DeInit      = BSP_I2C3_DeInit;
  io_ctx.ReadReg     = BSP_I2C3_ReadReg;
  io_ctx.WriteReg    = BSP_I2C3_WriteReg;
  io_ctx.GetTick     = BSP_GetTick;

  if (LSM6DSV16BX_RegisterBusIO(&lsm6dsv16bx_obj_0, &io_ctx) != LSM6DSV16BX_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (LSM6DSV16BX_ReadID(&lsm6dsv16bx_obj_0, &id) != LSM6DSV16BX_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != LSM6DSV16BX_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void)LSM6DSV16BX_GetCapabilities(&lsm6dsv16bx_obj_0, &cap);
    MotionCtx[LSM6DSV16BX_0].Functions = ((uint32_t)cap.Gyro) | ((uint32_t)cap.Acc << 1) | ((uint32_t)cap.Magneto << 2);

    DilMotionCompObj[LSM6DSV16BX_0] = &lsm6dsv16bx_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[LSM6DSV16BX_0] = (MOTION_SENSOR_CommonDrv_t *)(void *)&LSM6DSV16BX_COMMON_Driver;

    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_GYRO) == MOTION_GYRO) && (cap.Gyro == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[LSM6DSV16BX_0][FunctionIndex[MOTION_GYRO]] = (MOTION_SENSOR_FuncDrv_t *)(void *)&LSM6DSV16BX_GYRO_Driver;

      if (MotionDrv[LSM6DSV16BX_0]->Init(DilMotionCompObj[LSM6DSV16BX_0]) != LSM6DSV16BX_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_ACCELERO) == MOTION_ACCELERO) && (cap.Acc == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[LSM6DSV16BX_0][FunctionIndex[MOTION_ACCELERO]] = (MOTION_SENSOR_FuncDrv_t *)(void *)&LSM6DSV16BX_ACC_Driver;

      if (MotionDrv[LSM6DSV16BX_0]->Init(DilMotionCompObj[LSM6DSV16BX_0]) != LSM6DSV16BX_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_MAGNETO) == MOTION_MAGNETO))
    {
      /* Return an error if the application try to initialize a function not supported by the component */
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}
#endif



#if (USE_DIL_SENSOR_LSM6DSV80X_0 == 1)
/**
  * @brief  Register Bus IOs for LSM6DSV16X instance
  * @param  Functions Motion sensor functions. Could be :
  *         - MOTION_GYRO and/or MOTION_ACCELERO
  * @retval BSP status
  */
static int32_t LSM6DSV80X_0_Probe(uint32_t Functions)
{
  LSM6DSV80X_IO_t            io_ctx;
  uint8_t                    id;
  static LSM6DSV80X_Object_t lsm6dsv80x_obj_0;
  LSM6DSV80X_Capabilities_t  cap;
  int32_t                    ret = BSP_ERROR_NONE;

  /* Configure the driver */

  io_ctx.BusType     = LSM6DSV80X_I2C_BUS; /* I2C */
  io_ctx.Address     = LSM6DSV80X_I2C_ADD_L;
  io_ctx.Init        = BSP_I2C3_Init;
  io_ctx.DeInit      = BSP_I2C3_DeInit;
  io_ctx.ReadReg     = BSP_I2C3_ReadReg;
  io_ctx.WriteReg    = BSP_I2C3_WriteReg;
  io_ctx.GetTick     = BSP_GetTick;

  if (LSM6DSV80X_RegisterBusIO(&lsm6dsv80x_obj_0, &io_ctx) != LSM6DSV80X_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (LSM6DSV80X_ReadID(&lsm6dsv80x_obj_0, &id) != LSM6DSV80X_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != LSM6DSV80X_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void)LSM6DSV80X_GetCapabilities(&lsm6dsv80x_obj_0, &cap);
    MotionCtx[LSM6DSV80X_0].Functions = ((uint32_t)cap.Gyro) | ((uint32_t)cap.Acc << 1) | ((uint32_t)cap.Magneto << 2);

    DilMotionCompObj[LSM6DSV80X_0] = &lsm6dsv80x_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[LSM6DSV80X_0] = (MOTION_SENSOR_CommonDrv_t *)(void *)&LSM6DSV80X_COMMON_Driver;

    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_GYRO) == MOTION_GYRO) && (cap.Gyro == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[LSM6DSV80X_0][FunctionIndex[MOTION_GYRO]] = (MOTION_SENSOR_FuncDrv_t *)(void *)&LSM6DSV80X_GYRO_Driver;

      if (MotionDrv[LSM6DSV80X_0]->Init(DilMotionCompObj[LSM6DSV80X_0]) != LSM6DSV80X_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_ACCELERO) == MOTION_ACCELERO) && (cap.Acc == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[LSM6DSV80X_0][FunctionIndex[MOTION_ACCELERO]] = (MOTION_SENSOR_FuncDrv_t *)(void *)&LSM6DSV80X_ACC_Driver;

      if (MotionDrv[LSM6DSV80X_0]->Init(DilMotionCompObj[LSM6DSV80X_0]) != LSM6DSV80X_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_MAGNETO) == MOTION_MAGNETO))
    {
      /* Return an error if the application try to initialize a function not supported by the component */
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}
#endif


#if (USE_DIL_SENSOR_ST1VAFE6AX_0 == 1)
/**
 * @brief  Register Bus IOs for ST1VAFE6AX instance
 * @param  Functions Motion sensor functions. Could be :
 *         - MOTION_GYRO and/or MOTION_ACCELERO
 * @retval BSP status
 */
static int32_t ST1VAFE6AX_0_Probe(uint32_t Functions)
{
  ST1VAFE6AX_IO_t            io_ctx;
  uint8_t                     id;
  static ST1VAFE6AX_Object_t st1vafe6ax_obj_0;
  ST1VAFE6AX_Capabilities_t  cap;
  int32_t                     ret = BSP_ERROR_NONE;

  /* Configure the driver */

  io_ctx.BusType     = ST1VAFE6AX_I2C_BUS; /* I2C */
  io_ctx.Address     = ST1VAFE6AX_I2C_ADD_L;
  io_ctx.Init        = BSP_I2C3_Init;
  io_ctx.DeInit      = BSP_I2C3_DeInit;
  io_ctx.ReadReg     = BSP_I2C3_ReadReg;
  io_ctx.WriteReg    = BSP_I2C3_WriteReg;
  io_ctx.GetTick     = BSP_GetTick;

  if (ST1VAFE6AX_RegisterBusIO(&st1vafe6ax_obj_0, &io_ctx) != ST1VAFE6AX_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (ST1VAFE6AX_ReadID(&st1vafe6ax_obj_0, &id) != ST1VAFE6AX_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != ST1VAFE6AX_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void)ST1VAFE6AX_GetCapabilities(&st1vafe6ax_obj_0, &cap);
    MotionCtx[ST1VAFE6AX_0].Functions = ((uint32_t)cap.Gyro) | ((uint32_t)cap.Acc << 1) | ((uint32_t)cap.Magneto << 2);

    DilMotionCompObj[ST1VAFE6AX_0] = &st1vafe6ax_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[ST1VAFE6AX_0] = (MOTION_SENSOR_CommonDrv_t *)(void *)&ST1VAFE6AX_COMMON_Driver;

    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_GYRO) == MOTION_GYRO) && (cap.Gyro == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[ST1VAFE6AX_0][FunctionIndex[MOTION_GYRO]] = (MOTION_SENSOR_FuncDrv_t *)(void *)&ST1VAFE6AX_GYRO_Driver;

      if (MotionDrv[ST1VAFE6AX_0]->Init(DilMotionCompObj[ST1VAFE6AX_0]) != ST1VAFE6AX_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_ACCELERO) == MOTION_ACCELERO) && (cap.Acc == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[ST1VAFE6AX_0][FunctionIndex[MOTION_ACCELERO]] = (MOTION_SENSOR_FuncDrv_t *)(void *)&ST1VAFE6AX_ACC_Driver;

      if (MotionDrv[ST1VAFE6AX_0]->Init(DilMotionCompObj[ST1VAFE6AX_0]) != ST1VAFE6AX_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_MAGNETO) == MOTION_MAGNETO))
    {
      /* Return an error if the application try to initialize a function not supported by the component */
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}
#endif



#if (USE_DIL_SENSOR_ST1VAFE3BX_0 == 1)
/**
 * @brief  Register Bus IOs for ST1VAFE3BX instance
 * @param  Functions Motion sensor functions. Could be :
 *         - MOTION_GYRO and/or MOTION_ACCELERO
 * @retval BSP status
 */
static int32_t ST1VAFE3BX_0_Probe(uint32_t Functions)
{
  ST1VAFE3BX_IO_t            io_ctx;
  uint8_t                     id;
  static ST1VAFE3BX_Object_t st1vafe3bx_obj_0;
  ST1VAFE3BX_Capabilities_t  cap;
  int32_t                     ret = BSP_ERROR_NONE;

  /* Configure the driver */

  io_ctx.BusType     = ST1VAFE3BX_I2C_BUS; /* I2C */
  io_ctx.Address     = ST1VAFE3BX_I2C_ADD_L;
  io_ctx.Init        = BSP_I2C3_Init;
  io_ctx.DeInit      = BSP_I2C3_DeInit;
  io_ctx.ReadReg     = BSP_I2C3_ReadReg;
  io_ctx.WriteReg    = BSP_I2C3_WriteReg;
  io_ctx.GetTick     = BSP_GetTick;
  io_ctx.Delay       = HAL_Delay;

  if (ST1VAFE3BX_RegisterBusIO(&st1vafe3bx_obj_0, &io_ctx) != ST1VAFE3BX_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (ST1VAFE3BX_ReadID(&st1vafe3bx_obj_0, &id) != ST1VAFE3BX_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != ST1VAFE3BX_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void)ST1VAFE3BX_GetCapabilities(&st1vafe3bx_obj_0, &cap);
    MotionCtx[ST1VAFE3BX_0].Functions = ((uint32_t)cap.Gyro) | ((uint32_t)cap.Acc << 1) | ((uint32_t)cap.Magneto << 2);

    DilMotionCompObj[ST1VAFE3BX_0] = &st1vafe3bx_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[ST1VAFE3BX_0] = (MOTION_SENSOR_CommonDrv_t *)(void *)&ST1VAFE3BX_COMMON_Driver;

    
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_ACCELERO) == MOTION_ACCELERO) && (cap.Acc == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[ST1VAFE3BX_0][FunctionIndex[MOTION_ACCELERO]] = (MOTION_SENSOR_FuncDrv_t *)(void *)&ST1VAFE3BX_ACC_Driver;

      if (MotionDrv[ST1VAFE3BX_0]->Init(DilMotionCompObj[ST1VAFE3BX_0]) != ST1VAFE3BX_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_MAGNETO) == MOTION_MAGNETO))
    {
      /* Return an error if the application try to initialize a function not supported by the component */
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}
#endif


#if (USE_DIL_SENSOR_STHS34PF80_0 == 1)
/**
  * @brief  Register Bus IOs for STHS34PF80 instance
  * @param  Functions Environmental sensor functions. Could be :
  *         - ENV_TEMPERATURE and/or ENV_PRESSURE
  * @retval BSP status
  */
static int32_t STHS34PF80_0_Probe(uint32_t Functions)
{
  STHS34PF80_IO_t            io_ctx;
  uint8_t                    id;
  int32_t                    ret = BSP_ERROR_NONE;
  static STHS34PF80_Object_t sths34pf80_obj_0;
  STHS34PF80_Capabilities_t  cap;

  /* Configure the pressure driver */
  io_ctx.BusType     = STHS34PF80_I2C_BUS; /* I2C */
  io_ctx.Address     = STHS34PF80_I2C_ADD;
  io_ctx.Init        = BSP_I2C3_Init;
  io_ctx.DeInit      = BSP_I2C3_DeInit;
  io_ctx.ReadReg     = BSP_I2C3_ReadReg;
  io_ctx.WriteReg    = BSP_I2C3_WriteReg;
  io_ctx.GetTick     = BSP_GetTick;

  if (STHS34PF80_RegisterBusIO(&sths34pf80_obj_0, &io_ctx) != STHS34PF80_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (STHS34PF80_ReadID(&sths34pf80_obj_0, &id) != STHS34PF80_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != STHS34PF80_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void)STHS34PF80_GetCapabilities(&sths34pf80_obj_0, &cap);

    EnvCtx[STHS34PF80_0].Functions = ((uint32_t)cap.Temperature) | ((uint32_t)cap.Pressure << 1) | ((
                                       uint32_t)cap.Humidity << 2);

    DilEnvCompObj[STHS34PF80_0] = &sths34pf80_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    EnvDrv[STHS34PF80_0] = (ENV_SENSOR_CommonDrv_t *)(void *)&STHS34PF80_COMMON_Driver;

    if ((ret == BSP_ERROR_NONE) && ((Functions & ENV_TEMPERATURE) == ENV_TEMPERATURE) && (cap.Temperature == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      EnvFuncDrv[STHS34PF80_0][FunctionIndex[ENV_TEMPERATURE]] = (ENV_SENSOR_FuncDrv_t *)(void *)&STHS34PF80_TEMP_Driver;

      if (EnvDrv[STHS34PF80_0]->Init(DilEnvCompObj[STHS34PF80_0]) != STHS34PF80_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & ENV_PRESSURE) == ENV_PRESSURE) && (cap.Pressure == 1U))
    {
      /* Return an error if the application try to initialize a function not supported by the component */
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & ENV_HUMIDITY) == ENV_HUMIDITY))
    {
      /* Return an error if the application try to initialize a function not supported by the component */
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  Get TMOS sensor data
  * @param  Instance TMOS sensor instance
  * @param  OutData pointer to TMOS data structure
  * @retval BSP status
  */
int32_t BSP_DIL_TMOS_SENSOR_GetData(uint32_t Instance, BSP_TMOS_Data_t *OutData)
{
  int32_t ret;

  ret = STHS34PF80_GetObjectTemperatureRaw(DilEnvCompObj[Instance], &OutData->t_obj);
  ret = STHS34PF80_GetAmbientTemperatureRaw(DilEnvCompObj[Instance], &OutData->t_amb);
  ret = STHS34PF80_GetMotionData(DilEnvCompObj[Instance], &OutData->t_motion);
  ret = STHS34PF80_GetAmbTempShock(DilEnvCompObj[Instance], &OutData->t_amb_shock);
  ret = STHS34PF80_GetObjectTempCompensated(DilEnvCompObj[Instance], &OutData->t_obj_comp);
  ret = STHS34PF80_GetPresenceTemperature(DilEnvCompObj[Instance], &OutData->t_pres);

  return ret;
}


/**
  * @brief  Enable t_object compensation
  * @param  Instance TMOS sensor instance
  * @retval BSP status
  */
int32_t BSP_DIL_TMOS_SENSOR_CompensationInit(uint32_t Instance)
{
  int32_t ret;
  uint8_t reg_odr_data, reg_config = 0;

  ret = STHS34PF80_Read_Reg(DilEnvCompObj[Instance], STHS34PF80_CTRL1, &reg_odr_data);  // save CTRL1 (ODR)
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_CTRL1, 0x00);          // stop device (ODR = 0)
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_CTRL2, 0x10);          // enable access to the embedded functions registers

  /* Read ALGO_CONFIG to back-up register except COMP_TYPE bit (0xFB mask)*/
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_PAGE_RW, 0x20);
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_FUNC_CFG_ADDR, 0x28);
  ret = STHS34PF80_Read_Reg(DilEnvCompObj[Instance], STHS34PF80_FUNC_CFG_DATA, &reg_config);
  reg_config = reg_config & 0xFB;

  /* Write COMP_TYPE bit value in OR with ALGO_CONFIG back-up */
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_PAGE_RW, 0x40);
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_FUNC_CFG_ADDR, 0x28);
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_FUNC_CFG_DATA, 0x04 | reg_config);  // enable compensation

  /* Reset algorithms */
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_FUNC_CFG_ADDR, 0x2A);
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_FUNC_CFG_DATA, 0x01);
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_PAGE_RW, 0x00);

  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_CTRL2, 0x00);          // disable access to the embedded functions registers
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_CTRL1, reg_odr_data);  // restore CTRL1

  return ret;
}


/**
  * @brief  Disable t_object compensation
  * @param  Instance TMOS sensor instance
  * @retval BSP status
  */
int32_t BSP_DIL_TMOS_SENSOR_CompensationDeInit(uint32_t Instance)
{
  int32_t ret;
  uint8_t reg_odr_data, reg_config = 0;

  ret = STHS34PF80_Read_Reg(DilEnvCompObj[Instance], STHS34PF80_CTRL1, &reg_odr_data);  // save CTRL1 (ODR)
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_CTRL1, 0x00);          // stop device (ODR = 0)
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_CTRL2, 0x10);          // enable access to the embedded functions registers

  /* Read ALGO_CONFIG to back-up register except COMP_TYPE bit (0xFB mask)*/
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_PAGE_RW, 0x20);
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_FUNC_CFG_ADDR, 0x28);
  ret = STHS34PF80_Read_Reg(DilEnvCompObj[Instance], STHS34PF80_FUNC_CFG_DATA, &reg_config);
  reg_config = reg_config & 0xFB;

  /* Write COMP_TYPE bit value in OR with ALGO_CONFIG back-up */
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_PAGE_RW, 0x40);
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_FUNC_CFG_ADDR, 0x28);
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_FUNC_CFG_DATA, 0x00 | reg_config);  // enable compensation

  /* Reset algorithms */
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_FUNC_CFG_ADDR, 0x2A);
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_FUNC_CFG_DATA, 0x01);
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_PAGE_RW, 0x00);

  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_CTRL2, 0x00);          // disable access to the embedded functions registers
  ret = STHS34PF80_Write_Reg(DilEnvCompObj[Instance], STHS34PF80_CTRL1, reg_odr_data);  // restore CTRL1

  return ret;
}

#endif

/**
  * @brief  Get DIL24 sensor data
  * @param  Instance  sensor instance to be used
  * @param  Data pointer to sensor output
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_ACC_HG_GetAxes(uint32_t Instance, BSP_DIL24_Data_t *Data)
{ //custom read function for Acc HG 80x
  int32_t ret = BSP_ERROR_NONE;
  switch (Instance)
  {
#if (USE_DIL_SENSOR_LSM6DSV80X_0 == 1)
    case LSM6DSV80X_0:
     
      if (LSM6DSV80X_ACC_HG_GetAxes(DilMotionCompObj[Instance], (LSM6DSV80X_Axes_t*)&Data->accHG) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif /* USE_DIL_SENSOR_LSM6DSV80X_0 == 1 */

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }
  
  
  
  return ret;
}

/**
  * @brief  Set FS HigG value
  * @param  Instance the device instance
  * @param  Odr FIFO ODR value
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_ACC_HG_SetOutputDataRate(uint32_t Instance, float_t Odr)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_DIL_SENSOR_LSM6DSV80X_0 == 1)
    case LSM6DSV80X_0:
      if (LSM6DSV80X_ACC_HG_SetOutputDataRate(DilMotionCompObj[Instance], Odr) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif /* USE_DIL_SENSOR_LSM6DSV80X_0 == 1 */

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
  * @brief  Set FS HigG value
  * @param  Instance the device instance
  * @param  Odr FIFO ODR value
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_ACC_HG_SetFullScale(uint32_t Instance, int32_t FullScale)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_DIL_SENSOR_LSM6DSV80X_0 == 1)
    case LSM6DSV80X_0:
      if (LSM6DSV80X_ACC_HG_SetFullScale(DilMotionCompObj[Instance], FullScale) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif /* USE_DIL_SENSOR_LSM6DSV80X_0 == 1 */

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

//uint8_t supported_adapter_address[N_SUPPORTED_ADAPTERS-1] = {STHS34PF80_I2C_ADD,LSM6DSO32_I2C_ADD_L}; /// -1 because sensirion has no address
//adapter_data_t adapter;

