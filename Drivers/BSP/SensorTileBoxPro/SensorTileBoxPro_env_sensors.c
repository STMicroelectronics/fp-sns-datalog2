/**
  ******************************************************************************
  * @file    SensorTileBoxPro_env_sensors.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.2.0
  * @date    03-Jun-2024
  * @brief   This file provides BSP Environmental Sensors interface
  *          for SensorTileBoxPro
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
#include "SensorTileBoxPro_env_sensors.h"

extern void *EnvCompObj[BSP_ENV_INSTANCES_NBR]; /* This "redundant" line is here to fulfil MISRA C-2012 rule 8.4 */
void *EnvCompObj[BSP_ENV_INSTANCES_NBR];

/* We define a jump table in order to get the correct index from the desired function. */
/* This table should have a size equal to the maximum value of a function plus 1.      */
static uint32_t FunctionIndex[5] = {0, 0, 1, 1, 2};
static ENV_SENSOR_FuncDrv_t *EnvFuncDrv[BSP_ENV_INSTANCES_NBR][BSP_ENV_FUNCTIONS_NBR];
static ENV_SENSOR_CommonDrv_t *EnvDrv[BSP_ENV_INSTANCES_NBR];
static BSP_ENV_SENSOR_Ctx_t EnvCtx[BSP_ENV_INSTANCES_NBR];


#if (USE_ENV_SENSOR_STTS22H_0 == 1)
static int32_t STTS22H_0_Probe(uint32_t Functions);
#endif /* BSP_ENV_INSTANCES_NBR == 0 */

#if (USE_ENV_SENSOR_LPS22DF_0 == 1)
static int32_t LPS22DF_0_Probe(uint32_t Functions);
#endif /* BSP_ENV_INSTANCES_NBR == 0 */

/**
  * @brief  Initializes the environmental sensor
  * @param  Instance environmental sensor instance to be used
  * @param  Functions Environmental sensor functions. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_Init(uint32_t Instance, uint32_t Functions)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t function = ENV_TEMPERATURE;
  uint32_t i;
  uint32_t component_functions = 0;
  BSP_ENV_SENSOR_Capabilities_t cap;

  switch (Instance)
  {
#if (USE_ENV_SENSOR_STTS22H_0 == 1)
    case STTS22H_0:
      if (STTS22H_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if (EnvDrv[Instance]->GetCapabilities(EnvCompObj[Instance], (void *)&cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if (cap.Temperature == 1U)
      {
        component_functions |= ENV_TEMPERATURE;
      }
      if (cap.Pressure == 1U)
      {
        component_functions |= ENV_PRESSURE;
      }
      break;
#endif /* USE_ENV_SENSOR_STTS22H_0 == 1 */
#if (USE_ENV_SENSOR_LPS22DF_0 == 1)
    case LPS22DF_0:
      if (LPS22DF_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if (EnvDrv[Instance]->GetCapabilities(EnvCompObj[Instance], (void *)&cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if (cap.Temperature == 1U)
      {
        component_functions |= ENV_TEMPERATURE;
      }
      if (cap.Pressure == 1U)
      {
        component_functions |= ENV_PRESSURE;
      }
      break;
#endif /* USE_ENV_SENSOR_LPS22DF_0 == 1 */
    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  if (ret != BSP_ERROR_NONE)
  {
    return ret;
  }

  for (i = 0; i < BSP_ENV_FUNCTIONS_NBR; i++)
  {
    if (((Functions & function) == function) && ((component_functions & function) == function))
    {
      if (EnvFuncDrv[Instance][FunctionIndex[function]]->Enable(EnvCompObj[Instance]) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_COMPONENT_FAILURE;
      }
    }
    function = function << 1;
  }

  return ret;
}

/**
  * @brief  Deinitialize environmental sensor sensor
  * @param  Instance environmental sensor instance to be used
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_DeInit(uint32_t Instance)
{
  int32_t ret;

  if (Instance >= BSP_ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (EnvDrv[Instance]->DeInit(EnvCompObj[Instance]) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Get environmental sensor instance capabilities
  * @param  Instance Environmental sensor instance
  * @param  Capabilities pointer to Environmental sensor capabilities
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_GetCapabilities(uint32_t Instance, BSP_ENV_SENSOR_Capabilities_t *Capabilities)
{
  int32_t ret;

  if (Instance >= BSP_ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (EnvDrv[Instance]->GetCapabilities(EnvCompObj[Instance], Capabilities) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Get WHOAMI value
  * @param  Instance environmental sensor instance to be used
  * @param  Id WHOAMI value
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_ReadID(uint32_t Instance, uint8_t *Id)
{
  int32_t ret;

  if (Instance >= BSP_ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (EnvDrv[Instance]->ReadID(EnvCompObj[Instance], Id) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Enable environmental sensor
  * @param  Instance environmental sensor instance to be used
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_Enable(uint32_t Instance, uint32_t Function)
{
  int32_t ret;

  if (Instance >= BSP_ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((EnvCtx[Instance].Functions & Function) == Function)
    {
      if (EnvFuncDrv[Instance][FunctionIndex[Function]]->Enable(EnvCompObj[Instance]) != BSP_ERROR_NONE)
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

  return ret;
}

/**
  * @brief  Disable environmental sensor
  * @param  Instance environmental sensor instance to be used
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function)
{
  int32_t ret;

  if (Instance >= BSP_ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((EnvCtx[Instance].Functions & Function) == Function)
    {
      if (EnvFuncDrv[Instance][FunctionIndex[Function]]->Disable(EnvCompObj[Instance]) != BSP_ERROR_NONE)
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

  return ret;
}

/**
  * @brief  Get environmental sensor Output Data Rate
  * @param  Instance environmental sensor instance to be used
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  * @param  Odr pointer to Output Data Rate read value
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float *Odr)
{
  int32_t ret;

  if (Instance >= BSP_ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((EnvCtx[Instance].Functions & Function) == Function)
    {
      if (EnvFuncDrv[Instance][FunctionIndex[Function]]->GetOutputDataRate(EnvCompObj[Instance], Odr) != BSP_ERROR_NONE)
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

  return ret;
}

/**
  * @brief  Set environmental sensor Output Data Rate
  * @param  Instance environmental sensor instance to be used
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  * @param  Odr Output Data Rate value to be set
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float Odr)
{
  int32_t ret;

  if (Instance >= BSP_ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((EnvCtx[Instance].Functions & Function) == Function)
    {
      if (EnvFuncDrv[Instance][FunctionIndex[Function]]->SetOutputDataRate(EnvCompObj[Instance], Odr) != BSP_ERROR_NONE)
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

  return ret;
}

/**
  * @brief  Get environmental sensor value
  * @param  Instance environmental sensor instance to be used
  * @param  Function Environmental sensor function. Could be :
  *         - ENV_TEMPERATURE
  *         - ENV_PRESSURE
  * @param  Value pointer to environmental sensor value
  * @retval BSP status
  */
int32_t BSP_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float *Value)
{
  int32_t ret;

  if (Instance >= BSP_ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((EnvCtx[Instance].Functions & Function) == Function)
    {
      if (EnvFuncDrv[Instance][FunctionIndex[Function]]->GetValue(EnvCompObj[Instance], Value) != BSP_ERROR_NONE)
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

  return ret;
}

#if (USE_ENV_SENSOR_STTS22H_0 == 1)
/**
  * @brief  Register Bus IOs for STTS22H instance
  * @param  Functions Environmental sensor functions. Could be :
  *         - ENV_TEMPERATURE
  * @retval BSP status
  */
static int32_t STTS22H_0_Probe(uint32_t Functions)
{
  STTS22H_IO_t            io_ctx;
  uint8_t                 id;
  int32_t                 ret = BSP_ERROR_NONE;
  static STTS22H_Object_t stts22h_obj_0;
  STTS22H_Capabilities_t  cap;

  /* Configure the environmental sensor driver */
  io_ctx.BusType     = STTS22H_I2C_BUS; /* I2C */
  io_ctx.Address     = STTS22H_I2C_ADD_H; /* ADDR pin = VDD */
  io_ctx.Init        = BSP_STTS22H_0_I2C_INIT;
  io_ctx.DeInit      = BSP_STTS22H_0_I2C_DEINIT;
  io_ctx.ReadReg     = BSP_STTS22H_0_I2C_READ_REG;
  io_ctx.WriteReg    = BSP_STTS22H_0_I2C_WRITE_REG;
  io_ctx.GetTick     = BSP_GetTick;

  if (STTS22H_RegisterBusIO(&stts22h_obj_0, &io_ctx) != STTS22H_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (STTS22H_ReadID(&stts22h_obj_0, &id) != STTS22H_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != STTS22H_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void)STTS22H_GetCapabilities(&stts22h_obj_0, &cap);
    EnvCtx[STTS22H_0].Functions = ((uint32_t)cap.Temperature) | ((uint32_t)cap.Pressure << 1) | ((
                                    uint32_t)cap.Humidity << 2);

    EnvCompObj[STTS22H_0] = &stts22h_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    EnvDrv[STTS22H_0] = (ENV_SENSOR_CommonDrv_t *)(void *)&STTS22H_COMMON_Driver;

    if ((ret == BSP_ERROR_NONE) && ((Functions & ENV_TEMPERATURE) == ENV_TEMPERATURE) && (cap.Temperature == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      EnvFuncDrv[STTS22H_0][FunctionIndex[ENV_TEMPERATURE]] = (ENV_SENSOR_FuncDrv_t *)(void *)&STTS22H_TEMP_Driver;

      if (EnvDrv[STTS22H_0]->Init(EnvCompObj[STTS22H_0]) != STTS22H_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & ENV_PRESSURE) == ENV_PRESSURE))
    {
      /* Return an error if the application try to initialize a function not supported by the component */
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}
#endif /* USE_ENV_SENSOR_STTS22H_0 == 1 */

#if (USE_ENV_SENSOR_LPS22DF_0 == 1)
/**
  * @brief  Register Bus IOs for LPS22DF instance
  * @param  Functions Environmental sensor functions. Could be :
  *         - ENV_TEMPERATURE and/or ENV_PRESSURE
  * @retval BSP status
  */
static int32_t LPS22DF_0_Probe(uint32_t Functions)
{
  LPS22DF_IO_t            io_ctx;
  uint8_t                 id;
  int32_t                 ret = BSP_ERROR_NONE;
  static LPS22DF_Object_t lps22df_obj_0;
  LPS22DF_Capabilities_t  cap;

  /* Configure the pressure driver */
  io_ctx.BusType     = LPS22DF_I2C_BUS; /* I2C */
  io_ctx.Address     = LPS22DF_I2C_ADD_H; /* SA0 = VDD */
  io_ctx.Init        = BSP_LPS22DF_0_I2C_INIT;
  io_ctx.DeInit      = BSP_LPS22DF_0_I2C_DEINIT;
  io_ctx.ReadReg     = BSP_LPS22DF_0_I2C_READ_REG;
  io_ctx.WriteReg    = BSP_LPS22DF_0_I2C_WRITE_REG;
  io_ctx.GetTick     = BSP_GetTick;

  if (LPS22DF_RegisterBusIO(&lps22df_obj_0, &io_ctx) != LPS22DF_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (LPS22DF_ReadID(&lps22df_obj_0, &id) != LPS22DF_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != LPS22DF_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void)LPS22DF_GetCapabilities(&lps22df_obj_0, &cap);

    EnvCtx[LPS22DF_0].Functions = ((uint32_t)cap.Temperature) | ((uint32_t)cap.Pressure << 1) | ((
                                    uint32_t)cap.Humidity << 2);

    EnvCompObj[LPS22DF_0] = &lps22df_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    EnvDrv[LPS22DF_0] = (ENV_SENSOR_CommonDrv_t *)(void *)&LPS22DF_COMMON_Driver;

    if ((ret == BSP_ERROR_NONE) && ((Functions & ENV_TEMPERATURE) == ENV_TEMPERATURE) && (cap.Temperature == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      EnvFuncDrv[LPS22DF_0][FunctionIndex[ENV_TEMPERATURE]] = (ENV_SENSOR_FuncDrv_t *)(void *)&LPS22DF_TEMP_Driver;

      if (EnvDrv[LPS22DF_0]->Init(EnvCompObj[LPS22DF_0]) != LPS22DF_OK)
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
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      EnvFuncDrv[LPS22DF_0][FunctionIndex[ENV_PRESSURE]] = (ENV_SENSOR_FuncDrv_t *)(void *)&LPS22DF_PRESS_Driver;

      if (EnvDrv[LPS22DF_0]->Init(EnvCompObj[LPS22DF_0]) != LPS22DF_OK)
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
#endif /* USE_ENV_SENSOR_LPS22DF_0 == 1 */

