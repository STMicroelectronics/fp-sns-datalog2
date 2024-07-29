/**
  ******************************************************************************
  * @file    SensorTileBoxPro_motion_sensors.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.2.0
  * @date    03-Jun-2024
  * @brief   This file provides BSP Motion Sensors interface for
  *          SensorTileBoxPro
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
#include "SensorTileBoxPro_motion_sensors.h"

extern void
*MotionCompObj[BSP_MOTION_INSTANCES_NBR]; /* This "redundant" line is here to fulfil MISRA C-2012 rule 8.4 */
void *MotionCompObj[BSP_MOTION_INSTANCES_NBR];

/* We define a jump table in order to get the correct index from the desired function. */
/* This table should have a size equal to the maximum value of a function plus 1.      */
static uint32_t FunctionIndex[5] = {0, 0, 1, 1, 2};
static MOTION_SENSOR_FuncDrv_t *MotionFuncDrv[BSP_MOTION_INSTANCES_NBR][BSP_MOTION_FUNCTIONS_NBR];
static MOTION_SENSOR_CommonDrv_t *MotionDrv[BSP_MOTION_INSTANCES_NBR];
static BSP_MOTION_SENSOR_Ctx_t MotionCtx[BSP_MOTION_INSTANCES_NBR];

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
static int32_t LIS2MDL_0_Probe(uint32_t Functions);
#endif /* USE_MOTION_SENSOR_LIS2MDL_0 == 1 */
#if (USE_MOTION_SENSOR_LIS2DU12_0 == 1)
static int32_t LIS2DU12_0_Probe(uint32_t Functions);
#endif /* USE_MOTION_SENSOR_LIS2DU12_0 == 1 */
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
static int32_t LSM6DSV16X_0_Probe(uint32_t Functions);
#endif /* USE_MOTION_SENSOR_LSM6DSV16X_0 == 1 */

#ifndef ALL_SENSORS_I2C
#if (USE_MOTION_SENSOR_LIS2DU12_0 == 1)
static int32_t BSP_LIS2DU12_Init(void);
static int32_t BSP_LIS2DU12_DeInit(void);
static int32_t BSP_LIS2DU12_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
static int32_t BSP_LIS2DU12_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
#endif /* USE_MOTION_SENSOR_LIS2DU12_0 == 1 */
#endif /* ALL_SENSORS_I2C */

#ifndef ALL_SENSORS_I2C
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
static int32_t BSP_LSM6DSV16X_Init(void);
static int32_t BSP_LSM6DSV16X_DeInit(void);
static int32_t BSP_LSM6DSV16X_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
static int32_t BSP_LSM6DSV16X_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
#endif /* USE_MOTION_SENSOR_LSM6DSV16X_0 == 1 */
#endif /* ALL_SENSORS_I2C */

/**
  * @brief  Initializes the motion sensors
  * @param  Instance Motion sensor instance
  * @param  Functions Motion sensor functions. Could be :
  *         - MOTION_GYRO
  *         - MOTION_ACCELERO
  *         - MOTION_MAGNETO
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_Init(uint32_t Instance, uint32_t Functions)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t function = MOTION_GYRO;
  uint32_t i;
  uint32_t component_functions = 0;
  BSP_MOTION_SENSOR_Capabilities_t cap;

  switch (Instance)
  {
#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      if (LIS2MDL_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if (MotionDrv[Instance]->GetCapabilities(MotionCompObj[Instance], (void *)&cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if (cap.Acc == 1U)
      {
        component_functions |= MOTION_ACCELERO;
      }
      if (cap.Gyro == 1U)
      {
        component_functions |= MOTION_GYRO;
      }
      if (cap.Magneto == 1U)
      {
        component_functions |= MOTION_MAGNETO;
      }
      break;
#endif /* USE_MOTION_SENSOR_LIS2MDL_0 == 1 */
#if (USE_MOTION_SENSOR_LIS2DU12_0 == 1)
    case LIS2DU12_0:
      if (LIS2DU12_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if (MotionDrv[Instance]->GetCapabilities(MotionCompObj[Instance], (void *)&cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if (cap.Acc == 1U)
      {
        component_functions |= MOTION_ACCELERO;
      }
      if (cap.Gyro == 1U)
      {
        component_functions |= MOTION_GYRO;
      }
      if (cap.Magneto == 1U)
      {
        component_functions |= MOTION_MAGNETO;
      }
      break;
#endif /* USE_MOTION_SENSOR_LIS2DU12_0 == 1 */
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if (MotionDrv[Instance]->GetCapabilities(MotionCompObj[Instance], (void *)&cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if (cap.Acc == 1U)
      {
        component_functions |= MOTION_ACCELERO;
      }
      if (cap.Gyro == 1U)
      {
        component_functions |= MOTION_GYRO;
      }
      if (cap.Magneto == 1U)
      {
        component_functions |= MOTION_MAGNETO;
      }
      break;
#endif /* USE_MOTION_SENSOR_LSM6DSV16X_0 == 1 */
    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  if (ret != BSP_ERROR_NONE)
  {
    return ret;
  }

  for (i = 0; i < BSP_MOTION_FUNCTIONS_NBR; i++)
  {
    if (((Functions & function) == function) && ((component_functions & function) == function))
    {
      if (MotionFuncDrv[Instance][FunctionIndex[function]]->Enable(MotionCompObj[Instance]) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_COMPONENT_FAILURE;
      }
    }
    function = function << 1;
  }

  return ret;
}

/**
  * @brief  Deinitialize Motion sensor
  * @param  Instance Motion sensor instance
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_DeInit(uint32_t Instance)
{
  int32_t ret;

  if (Instance >= BSP_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (MotionDrv[Instance]->DeInit(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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
  * @brief  Get motion sensor instance capabilities
  * @param  Instance Motion sensor instance
  * @param  Capabilities pointer to motion sensor capabilities
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_GetCapabilities(uint32_t Instance, BSP_MOTION_SENSOR_Capabilities_t *Capabilities)
{
  int32_t ret;

  if (Instance >= BSP_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (MotionDrv[Instance]->GetCapabilities(MotionCompObj[Instance], Capabilities) != BSP_ERROR_NONE)
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
  * @param  Instance Motion sensor instance
  * @param  Id WHOAMI value
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_ReadID(uint32_t Instance, uint8_t *Id)
{
  int32_t ret;

  if (Instance >= BSP_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (MotionDrv[Instance]->ReadID(MotionCompObj[Instance], Id) != BSP_ERROR_NONE)
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
  * @brief  Enable Motion sensor
  * @param  Instance Motion sensor instance
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO
  *         - MOTION_ACCELERO
  *         - MOTION_MAGNETO
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_Enable(uint32_t Instance, uint32_t Function)
{
  int32_t ret;

  if (Instance >= BSP_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->Enable(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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
  * @brief  Disable Motion sensor
  * @param  Instance Motion sensor instance
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO
  *         - MOTION_ACCELERO
  *         - MOTION_MAGNETO
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_Disable(uint32_t Instance, uint32_t Function)
{
  int32_t ret;

  if (Instance >= BSP_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->Disable(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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
  * @brief  Get motion sensor axes data
  * @param  Instance Motion sensor instance
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO
  *         - MOTION_ACCELERO
  *         - MOTION_MAGNETO
  * @param  Axes pointer to axes data structure
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_GetAxes(uint32_t Instance, uint32_t Function, BSP_MOTION_SENSOR_Axes_t *Axes)
{
  int32_t ret;

  if (Instance >= BSP_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->GetAxes(MotionCompObj[Instance], Axes) != BSP_ERROR_NONE)
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
  * @brief  Get motion sensor axes raw data
  * @param  Instance Motion sensor instance
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO
  *         - MOTION_ACCELERO
  *         - MOTION_MAGNETO
  * @param  Axes pointer to axes raw data structure
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_GetAxesRaw(uint32_t Instance, uint32_t Function, BSP_MOTION_SENSOR_AxesRaw_t *Axes)
{
  int32_t ret;

  if (Instance >= BSP_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->GetAxesRaw(MotionCompObj[Instance], Axes) != BSP_ERROR_NONE)
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
  * @brief  Get motion sensor sensitivity
  * @param  Instance Motion sensor instance
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO
  *         - MOTION_ACCELERO
  *         - MOTION_MAGNETO
  * @param  Sensitivity pointer to sensitivity read value
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_GetSensitivity(uint32_t Instance, uint32_t Function, float *Sensitivity)
{
  int32_t ret;

  if (Instance >= BSP_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->GetSensitivity(MotionCompObj[Instance],
                                                                           Sensitivity) != BSP_ERROR_NONE)
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
  * @brief  Get motion sensor Output Data Rate
  * @param  Instance Motion sensor instance
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO
  *         - MOTION_ACCELERO
  *         - MOTION_MAGNETO
  * @param  Odr pointer to Output Data Rate read value
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float *Odr)
{
  int32_t ret;

  if (Instance >= BSP_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->GetOutputDataRate(MotionCompObj[Instance],
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

  return ret;
}

/**
  * @brief  Get motion sensor Full Scale
  * @param  Instance Motion sensor instance
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO
  *         - MOTION_ACCELERO
  *         - MOTION_MAGNETO
  * @param  Fullscale pointer to Fullscale read value
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_GetFullScale(uint32_t Instance, uint32_t Function, int32_t *Fullscale)
{
  int32_t ret;

  if (Instance >= BSP_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->GetFullScale(MotionCompObj[Instance],
                                                                         Fullscale) != BSP_ERROR_NONE)
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
  * @brief  Set motion sensor Output Data Rate
  * @param  Instance Motion sensor instance
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO
  *         - MOTION_ACCELERO
  *         - MOTION_MAGNETO
  * @param  Odr Output Data Rate value to be set
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float Odr)
{
  int32_t ret;

  if (Instance >= BSP_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->SetOutputDataRate(MotionCompObj[Instance],
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

  return ret;
}

/**
  * @brief  Set motion sensor Full Scale
  * @param  Instance Motion sensor instance
  * @param  Function Motion sensor function. Could be :
  *         - MOTION_GYRO
  *         - MOTION_ACCELERO
  *         - MOTION_MAGNETO
  * @param  Fullscale Fullscale value to be set
  * @retval BSP status
  */
int32_t BSP_MOTION_SENSOR_SetFullScale(uint32_t Instance, uint32_t Function, int32_t Fullscale)
{
  int32_t ret;

  if (Instance >= BSP_MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((MotionCtx[Instance].Functions & Function) == Function)
    {
      if (MotionFuncDrv[Instance][FunctionIndex[Function]]->SetFullScale(MotionCompObj[Instance],
                                                                         Fullscale) != BSP_ERROR_NONE)
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

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
/**
  * @brief  Register Bus IOs for LIS2MDL instance
  * @param  Functions Motion sensor functions. Could be :
  *         - MOTION_MAGNETO
  * @retval BSP status
  */
static int32_t LIS2MDL_0_Probe(uint32_t Functions)
{
  LIS2MDL_IO_t            io_ctx;
  uint8_t                 id;
  static LIS2MDL_Object_t lis2mdl_obj_0;
  LIS2MDL_Capabilities_t  cap;
  int32_t                 ret = BSP_ERROR_NONE;

  /* Configure the driver */
  io_ctx.BusType     = LIS2MDL_I2C_BUS; /* I2C */
  io_ctx.Address     = LIS2MDL_I2C_ADD;
  io_ctx.Init        = BSP_LIS2MDL_0_I2C_INIT;
  io_ctx.DeInit      = BSP_LIS2MDL_0_I2C_DEINIT;
  io_ctx.ReadReg     = BSP_LIS2MDL_0_I2C_READ_REG;
  io_ctx.WriteReg    = BSP_LIS2MDL_0_I2C_WRITE_REG;
  io_ctx.GetTick     = BSP_GetTick;

  if (LIS2MDL_RegisterBusIO(&lis2mdl_obj_0, &io_ctx) != LIS2MDL_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (LIS2MDL_ReadID(&lis2mdl_obj_0, &id) != LIS2MDL_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != (uint8_t)LIS2MDL_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void)LIS2MDL_GetCapabilities(&lis2mdl_obj_0, &cap);
    MotionCtx[LIS2MDL_0].Functions = ((uint32_t)cap.Gyro) | ((uint32_t)cap.Acc << 1) | ((uint32_t)cap.Magneto << 2);

    MotionCompObj[LIS2MDL_0] = &lis2mdl_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[LIS2MDL_0] = (MOTION_SENSOR_CommonDrv_t *)(void *)&LIS2MDL_COMMON_Driver;

    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_MAGNETO) == MOTION_MAGNETO) && (cap.Magneto == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[LIS2MDL_0][FunctionIndex[MOTION_MAGNETO]] = (MOTION_SENSOR_FuncDrv_t *)(void *)&LIS2MDL_MAG_Driver;

      if (MotionDrv[LIS2MDL_0]->Init(MotionCompObj[LIS2MDL_0]) != LIS2MDL_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_ACCELERO) == MOTION_ACCELERO))
    {
      /* Return an error if the application try to initialize a function not supported by the component */
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_GYRO) == MOTION_GYRO))
    {
      /* Return an error if the application try to initialize a function not supported by the component */
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}
#endif /* USE_MOTION_SENSOR_LIS2MDL_0 == 1 */

#if (USE_MOTION_SENSOR_LIS2DU12_0  == 1)
/**
  * @brief  Register Bus IOs for LIS2DU12 instance
  * @param  Functions Motion sensor functions. Could be :
  *         - MOTION_GYRO and/or MOTION_ACCELERO
  * @retval BSP status
  */
static int32_t LIS2DU12_0_Probe(uint32_t Functions)
{
  LIS2DU12_IO_t            io_ctx;
  uint8_t                  id;
  static LIS2DU12_Object_t lis2du12_obj_0;
  LIS2DU12_Capabilities_t  cap;
  int32_t                  ret = BSP_ERROR_NONE;

  /* Configure the driver */
#ifndef ALL_SENSORS_I2C
  io_ctx.BusType     = LIS2DU12_SPI_4WIRES_BUS; /* SPI 4-Wires */
  io_ctx.Address     = 0x0;
  io_ctx.Init        = BSP_LIS2DU12_Init;
  io_ctx.DeInit      = BSP_LIS2DU12_DeInit;
  io_ctx.ReadReg     = BSP_LIS2DU12_ReadReg;
  io_ctx.WriteReg    = BSP_LIS2DU12_WriteReg;
#else
  io_ctx.BusType     = LIS2DU12_I2C_BUS; /* I2C */
  io_ctx.Address     = LIS2DU12_I2C_ADD_H;
  io_ctx.Init        = BSP_LIS2DU12_0_I2C_INIT;
  io_ctx.DeInit      = BSP_LIS2DU12_0_I2C_DEINIT;
  io_ctx.ReadReg     = BSP_LIS2DU12_0_I2C_READ_REG;
  io_ctx.WriteReg    = BSP_LIS2DU12_0_I2C_WRITE_REG;
#endif /* ALL_SENSORS_I2C */
  io_ctx.GetTick     = BSP_GetTick;

  if (LIS2DU12_RegisterBusIO(&lis2du12_obj_0, &io_ctx) != LIS2DU12_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (LIS2DU12_ReadID(&lis2du12_obj_0, &id) != LIS2DU12_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != LIS2DU12_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void)LIS2DU12_GetCapabilities(&lis2du12_obj_0, &cap);
    MotionCtx[LIS2DU12_0].Functions = ((uint32_t)cap.Gyro) | ((uint32_t)cap.Acc << 1) | ((uint32_t)cap.Magneto << 2);

    MotionCompObj[LIS2DU12_0] = &lis2du12_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[LIS2DU12_0] = (MOTION_SENSOR_CommonDrv_t *)(void *)&LIS2DU12_COMMON_Driver;

    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_ACCELERO) == MOTION_ACCELERO) && (cap.Acc == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[LIS2DU12_0][FunctionIndex[MOTION_ACCELERO]] =
        (MOTION_SENSOR_FuncDrv_t *)(void *)&LIS2DU12_ACC_Driver;

      if (MotionDrv[LIS2DU12_0]->Init(MotionCompObj[LIS2DU12_0]) != LIS2DU12_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_GYRO) == MOTION_GYRO))
    {
      /* Return an error if the application try to initialize a function not supported by the component */
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_MAGNETO) == MOTION_MAGNETO))
    {
      /* Return an error if the application try to initialize a function not supported by the component */
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}

#ifndef ALL_SENSORS_I2C
/**
  * @brief  Initialize SPI bus for LIS2DU12
  * @retval BSP status
  */
static int32_t BSP_LIS2DU12_Init(void)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if (BSP_LIS2DU12_0_SPI_INIT() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  DeInitialize SPI bus for LIS2DU12
  * @retval BSP status
  */
static int32_t BSP_LIS2DU12_DeInit(void)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if (BSP_LIS2DU12_0_SPI_DEINIT() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Write register by SPI bus for LIS2DU12
  * @param  Addr not used, it is only for BSP compatibility
  * @param  Reg the starting register address to be written
  * @param  pdata the pointer to the data to be written
  * @param  len the length of the data to be written
  * @retval BSP status
  */
static int32_t BSP_LIS2DU12_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  int32_t ret = BSP_ERROR_NONE;
  uint8_t dataReg = (uint8_t)Reg;

  /* CS Enable */
  HAL_GPIO_WritePin(BSP_LIS2DU12_CS_PORT, BSP_LIS2DU12_CS_PIN, GPIO_PIN_RESET);

  if (BSP_LIS2DU12_0_SPI_SEND(&dataReg, 1) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_UNKNOWN_FAILURE;
  }

  if (BSP_LIS2DU12_0_SPI_SEND(pdata, len) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_UNKNOWN_FAILURE;
  }

  /* CS Disable */
  HAL_GPIO_WritePin(BSP_LIS2DU12_CS_PORT, BSP_LIS2DU12_CS_PIN, GPIO_PIN_SET);

  return ret;
}

/**
  * @brief  Read register by SPI bus for LIS2DU12
  * @param  Addr not used, it is only for BSP compatibility
  * @param  Reg the starting register address to be read
  * @param  pdata the pointer to the data to be read
  * @param  len the length of the data to be read
  * @retval BSP status
  */
static int32_t BSP_LIS2DU12_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  int32_t ret = BSP_ERROR_NONE;
  uint8_t dataReg = (uint8_t)Reg;

  dataReg |= 0x80;

  /* CS Enable */
  HAL_GPIO_WritePin(BSP_LIS2DU12_CS_PORT, BSP_LIS2DU12_CS_PIN, GPIO_PIN_RESET);

  if (BSP_LIS2DU12_0_SPI_SEND(&dataReg, 1) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_UNKNOWN_FAILURE;
  }

  if (BSP_LIS2DU12_0_SPI_RECV(pdata, len) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_UNKNOWN_FAILURE;
  }

  /* CS Disable */
  HAL_GPIO_WritePin(BSP_LIS2DU12_CS_PORT, BSP_LIS2DU12_CS_PIN, GPIO_PIN_SET);

  return ret;
}
#endif /* ALL_SENSORS_I2C */
#endif /* USE_MOTION_SENSOR_LIS2DU12_0  == 1 */

#if (USE_MOTION_SENSOR_LSM6DSV16X_0  == 1)
/**
  * @brief  Register Bus IOs for LSM6DSV16X instance
  * @param  Functions Motion sensor functions. Could be :
  *         - MOTION_GYRO and/or MOTION_ACCELERO
  * @retval BSP status
  */
static int32_t LSM6DSV16X_0_Probe(uint32_t Functions)
{
  LSM6DSV16X_IO_t            io_ctx;
  uint8_t                    id;
  static LSM6DSV16X_Object_t lsm6dsv16x_obj_0;
  LSM6DSV16X_Capabilities_t  cap;
  int32_t                    ret = BSP_ERROR_NONE;

  /* Configure the driver */
#ifndef ALL_SENSORS_I2C
  io_ctx.BusType     = LSM6DSV16X_SPI_4WIRES_BUS; /* SPI 4-Wires */
  io_ctx.Address     = 0x0;
  io_ctx.Init        = BSP_LSM6DSV16X_Init;
  io_ctx.DeInit      = BSP_LSM6DSV16X_DeInit;
  io_ctx.ReadReg     = BSP_LSM6DSV16X_ReadReg;
  io_ctx.WriteReg    = BSP_LSM6DSV16X_WriteReg;
#else
  io_ctx.BusType     = LSM6DSV16X_I2C_BUS; /* I2C */
  io_ctx.Address     = LSM6DSV16X_I2C_ADD_H;
  io_ctx.Init        = BSP_LSM6DSV16X_0_I2C_INIT;
  io_ctx.DeInit      = BSP_LSM6DSV16X_0_I2C_DEINIT;
  io_ctx.ReadReg     = BSP_LSM6DSV16X_0_I2C_READ_REG;
  io_ctx.WriteReg    = BSP_LSM6DSV16X_0_I2C_WRITE_REG;
#endif /* ALL_SENSORS_I2C */
  io_ctx.GetTick     = BSP_GetTick;

  if (LSM6DSV16X_RegisterBusIO(&lsm6dsv16x_obj_0, &io_ctx) != LSM6DSV16X_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (LSM6DSV16X_ReadID(&lsm6dsv16x_obj_0, &id) != LSM6DSV16X_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if (id != LSM6DSV16X_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void)LSM6DSV16X_GetCapabilities(&lsm6dsv16x_obj_0, &cap);
    MotionCtx[LSM6DSV16X_0].Functions = ((uint32_t)cap.Gyro) | ((uint32_t)cap.Acc << 1) | ((uint32_t)cap.Magneto << 2);

    MotionCompObj[LSM6DSV16X_0] = &lsm6dsv16x_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[LSM6DSV16X_0] = (MOTION_SENSOR_CommonDrv_t *)(void *)&LSM6DSV16X_COMMON_Driver;

    if ((ret == BSP_ERROR_NONE) && ((Functions & MOTION_GYRO) == MOTION_GYRO) && (cap.Gyro == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[LSM6DSV16X_0][FunctionIndex[MOTION_GYRO]] =
        (MOTION_SENSOR_FuncDrv_t *)(void *)&LSM6DSV16X_GYRO_Driver;

      if (MotionDrv[LSM6DSV16X_0]->Init(MotionCompObj[LSM6DSV16X_0]) != LSM6DSV16X_OK)
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
      MotionFuncDrv[LSM6DSV16X_0][FunctionIndex[MOTION_ACCELERO]] =
        (MOTION_SENSOR_FuncDrv_t *)(void *)&LSM6DSV16X_ACC_Driver;

      if (MotionDrv[LSM6DSV16X_0]->Init(MotionCompObj[LSM6DSV16X_0]) != LSM6DSV16X_OK)
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

#ifndef ALL_SENSORS_I2C
/**
  * @brief  Initialize SPI bus for LSM6DSV16X
  * @retval BSP status
  */
static int32_t BSP_LSM6DSV16X_Init(void)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if (BSP_LSM6DSV16X_0_SPI_INIT() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  DeInitialize SPI bus for LSM6DSV16X
  * @retval BSP status
  */
static int32_t BSP_LSM6DSV16X_DeInit(void)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if (BSP_LSM6DSV16X_0_SPI_DEINIT() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Write register by SPI bus for LSM6DSV16X
  * @param  Addr not used, it is only for BSP compatibility
  * @param  Reg the starting register address to be written
  * @param  pdata the pointer to the data to be written
  * @param  len the length of the data to be written
  * @retval BSP status
  */
static int32_t BSP_LSM6DSV16X_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  int32_t ret = BSP_ERROR_NONE;
  uint8_t dataReg = (uint8_t)Reg;

  /* CS Enable */
  HAL_GPIO_WritePin(BSP_LSM6DSV16X_CS_PORT, BSP_LSM6DSV16X_CS_PIN, GPIO_PIN_RESET);

  if (BSP_LSM6DSV16X_0_SPI_SEND(&dataReg, 1) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_UNKNOWN_FAILURE;
  }

  if (BSP_LSM6DSV16X_0_SPI_SEND(pdata, len) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_UNKNOWN_FAILURE;
  }

  /* CS Disable */
  HAL_GPIO_WritePin(BSP_LSM6DSV16X_CS_PORT, BSP_LSM6DSV16X_CS_PIN, GPIO_PIN_SET);

  return ret;
}

/**
  * @brief  Read register by SPI bus for LSM6DSV16X
  * @param  Addr not used, it is only for BSP compatibility
  * @param  Reg the starting register address to be read
  * @param  pdata the pointer to the data to be read
  * @param  len the length of the data to be read
  * @retval BSP status
  */
static int32_t BSP_LSM6DSV16X_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  int32_t ret = BSP_ERROR_NONE;
  uint8_t dataReg = (uint8_t)Reg;

  dataReg |= 0x80;

  /* CS Enable */
  HAL_GPIO_WritePin(BSP_LSM6DSV16X_CS_PORT, BSP_LSM6DSV16X_CS_PIN, GPIO_PIN_RESET);

  if (BSP_LSM6DSV16X_0_SPI_SEND(&dataReg, 1) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_UNKNOWN_FAILURE;
  }

  if (BSP_LSM6DSV16X_0_SPI_RECV(pdata, len) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_UNKNOWN_FAILURE;
  }

  /* CS Disable */
  HAL_GPIO_WritePin(BSP_LSM6DSV16X_CS_PORT, BSP_LSM6DSV16X_CS_PIN, GPIO_PIN_SET);

  return ret;
}
#endif /* ALL_SENSORS_I2C */
#endif /* USE_MOTION_SENSOR_LSM6DSV16X_0  == 1 */

