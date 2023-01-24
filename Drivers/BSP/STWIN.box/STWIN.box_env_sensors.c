/**
 ******************************************************************************
 * @file    STWIN.box_env_sensors.c
 * @author  SRA
 * @brief   This file provides BSP Environmental Sensors interface
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
#include "STWIN.box_env_sensors.h"

extern void *EnvCompObj[ENV_INSTANCES_NBR]; /* This "redundant" line is here to fulfil MISRA C-2012 rule 8.4 */
void *EnvCompObj[ENV_INSTANCES_NBR];

/* We define a jump table in order to get the correct index from the desired function. */
/* This table should have a size equal to the maximum value of a function plus 1.      */
static uint32_t FunctionIndex[5] ={0, 0, 1, 1, 2};
static ENV_SENSOR_FuncDrv_t *EnvFuncDrv[ENV_INSTANCES_NBR][ENV_FUNCTIONS_NBR];
static ENV_SENSOR_CommonDrv_t *EnvDrv[ENV_INSTANCES_NBR];
static ENV_SENSOR_Ctx_t EnvCtx[ENV_INSTANCES_NBR];

#if (USE_ENV_SENSOR_ILPS22QS_0 == 1)
static int32_t ILPS22QS_0_Probe(uint32_t Functions);
static int32_t BSP_ILPS22QS_Init(void);
static int32_t BSP_ILPS22QS_DeInit(void);
#endif

#if (USE_ENV_SENSOR_STTS22H_0 == 1)
static int32_t STTS22H_0_Probe(uint32_t Functions);
static int32_t BSP_STTS22H_Init(void);
static int32_t BSP_STTS22H_DeInit(void);
#endif

/**
 * @brief  Initializes the env sensors
 * @param  Instance Environmental sensor instance
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Init(uint32_t Instance, uint32_t Functions)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t function = ENV_TEMPERATURE;
  uint32_t i;
  uint32_t component_functions = 0;
  ENV_SENSOR_Capabilities_t cap;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_ILPS22QS_0 == 1)
    case ILPS22QS_0:
      if(ILPS22QS_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if(EnvDrv[Instance]->GetCapabilities(EnvCompObj[Instance], (void*) &cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if(cap.Temperature == 1U)
      {
        component_functions |= ENV_TEMPERATURE;
      }
      if(cap.Pressure == 1U)
      {
        component_functions |= ENV_PRESSURE;
      }
      break;
#endif
#if (USE_ENV_SENSOR_STTS22H_0 == 1)
    case STTS22H_0:
      if(STTS22H_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if(EnvDrv[Instance]->GetCapabilities(EnvCompObj[Instance], (void*) &cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if(cap.Temperature == 1U)
      {
        component_functions |= ENV_TEMPERATURE;
      }
      if(cap.Pressure == 1U)
      {
        component_functions |= ENV_PRESSURE;
      }
      break;
#endif      
    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  if(ret != BSP_ERROR_NONE)
  {
    return ret;
  }

  for(i = 0; i < ENV_FUNCTIONS_NBR; i++)
  {
    if(((Functions & function) == function) && ((component_functions & function) == function))
    {
      if(EnvFuncDrv[Instance][FunctionIndex[function]]->Enable(EnvCompObj[Instance]) != BSP_ERROR_NONE)
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

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(EnvDrv[Instance]->DeInit(EnvCompObj[Instance]) != BSP_ERROR_NONE)
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
int32_t BSP_ENV_SENSOR_GetCapabilities(uint32_t Instance, ENV_SENSOR_Capabilities_t *Capabilities)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(EnvDrv[Instance]->GetCapabilities(EnvCompObj[Instance], Capabilities) != BSP_ERROR_NONE)
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

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(EnvDrv[Instance]->ReadID(EnvCompObj[Instance], Id) != BSP_ERROR_NONE)
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
 * @brief  Enable environmental sensor sensor
 * @param  Instance environmental sensor instance to be used
 * @param  Function Environmental sensor function. Could be :
 *         - ENV_TEMPERATURE
 *         - ENV_PRESSURE
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Enable(uint32_t Instance, uint32_t Function)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((EnvCtx[Instance].Functions & Function) == Function)
    {
      if(EnvFuncDrv[Instance][FunctionIndex[Function]]->Enable(EnvCompObj[Instance]) != BSP_ERROR_NONE)
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
 * @brief  Disable environmental sensor sensor
 * @param  Instance environmental sensor instance to be used
 * @param  Function Environmental sensor function. Could be :
 *         - ENV_TEMPERATURE
 *         - ENV_PRESSURE
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((EnvCtx[Instance].Functions & Function) == Function)
    {
      if(EnvFuncDrv[Instance][FunctionIndex[Function]]->Disable(EnvCompObj[Instance]) != BSP_ERROR_NONE)
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
int32_t BSP_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float_t *Odr)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((EnvCtx[Instance].Functions & Function) == Function)
    {
      if(EnvFuncDrv[Instance][FunctionIndex[Function]]->GetOutputDataRate(EnvCompObj[Instance], Odr) != BSP_ERROR_NONE)
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
int32_t BSP_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float_t Odr)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((EnvCtx[Instance].Functions & Function) == Function)
    {
      if(EnvFuncDrv[Instance][FunctionIndex[Function]]->SetOutputDataRate(EnvCompObj[Instance], Odr) != BSP_ERROR_NONE)
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
int32_t BSP_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float_t *Value)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((EnvCtx[Instance].Functions & Function) == Function)
    {
      if(EnvFuncDrv[Instance][FunctionIndex[Function]]->GetValue(EnvCompObj[Instance], Value) != BSP_ERROR_NONE)
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

#if (USE_ENV_SENSOR_ILPS22QS_0 == 1)

/**
 * @brief  Register Bus IOs for instance 0 if ILPS22QS ID is OK
 * @param  Functions Environmental sensor functions. Could be :
 *         - ENV_TEMPERATURE and/or ENV_TEMPERATURE
 * @retval BSP status
 */
static int32_t ILPS22QS_0_Probe(uint32_t Functions)
{
  ILPS22QS_IO_t io_ctx;
  uint8_t id;
  int32_t ret = BSP_ERROR_NONE;
  static ILPS22QS_Object_t ilps22qs_obj_0;
  ILPS22QS_Capabilities_t cap;

  /* Configure the environmental sensor driver */
  io_ctx.BusType = ILPS22QS_I2C_BUS; /* I2C */
  io_ctx.Address = ILPS22QS_I2C_ADD;
  io_ctx.Init = BSP_ILPS22QS_Init;
  io_ctx.DeInit = BSP_ILPS22QS_DeInit;
  io_ctx.ReadReg = BSP_I2C2_ReadReg;
  io_ctx.WriteReg = BSP_I2C2_WriteReg;
  io_ctx.GetTick = BSP_GetTick;

  if(ILPS22QS_RegisterBusIO(&ilps22qs_obj_0, &io_ctx) != ILPS22QS_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(ILPS22QS_ReadID(&ilps22qs_obj_0, &id) != ILPS22QS_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(id != ILPS22QS_ID) //0xB4
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void) ILPS22QS_GetCapabilities(&ilps22qs_obj_0, &cap);
    EnvCtx[ILPS22QS_0].Functions = ((uint32_t) cap.Temperature) | ((uint32_t) cap.Pressure << 1) | ((uint32_t) cap.Humidity << 2);

    EnvCompObj[ILPS22QS_0] = &ilps22qs_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    EnvDrv[ILPS22QS_0] = (ENV_SENSOR_CommonDrv_t*) (void*) &ILPS22QS_COMMON_Driver;

    if(((Functions & ENV_TEMPERATURE) == ENV_TEMPERATURE) && (cap.Temperature == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      EnvFuncDrv[ILPS22QS_0][FunctionIndex[ENV_TEMPERATURE]] = (ENV_SENSOR_FuncDrv_t*) (void*) &ILPS22QS_TEMP_Driver;

      if(EnvDrv[ILPS22QS_0]->Init(EnvCompObj[ILPS22QS_0]) != ILPS22QS_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if(((Functions & ENV_PRESSURE) == ENV_PRESSURE) && (cap.Pressure == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      EnvFuncDrv[ILPS22QS_0][FunctionIndex[ENV_PRESSURE]] = (ENV_SENSOR_FuncDrv_t*) (void*) &ILPS22QS_PRESS_Driver;

      if(EnvDrv[ILPS22QS_0]->Init(EnvCompObj[ILPS22QS_0]) != ILPS22QS_OK)
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

static int32_t BSP_ILPS22QS_Init(void)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(BSP_I2C2_Init() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

static int32_t BSP_ILPS22QS_DeInit(void)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(BSP_I2C2_DeInit() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

#endif

#if (USE_ENV_SENSOR_STTS22H_0 == 1)
/**
 * @brief  Register Bus IOs for instance 2 if component ID is OK
 * @param  Functions Environmental sensor functions. Could be :
 *         - ENV_TEMPERATURE
 * @retval BSP status
 */
static int32_t STTS22H_0_Probe(uint32_t Functions)
{
  STTS22H_IO_t io_ctx;
  uint8_t id;
  int32_t ret = BSP_ERROR_NONE;
  static STTS22H_Object_t stts22h_obj_0;
  STTS22H_Capabilities_t cap;

  /* Configure the pressure driver */
  io_ctx.BusType = STTS22H_I2C_BUS; /* I2C */

#if 1
  // onboard
  io_ctx.Address = STTS22H_I2C_ADD_L;
  io_ctx.Init = BSP_STTS22H_Init;
  io_ctx.DeInit = BSP_STTS22H_DeInit;
  io_ctx.ReadReg = BSP_I2C2_ReadReg;
  io_ctx.WriteReg = BSP_I2C2_WriteReg;
#else
  // add-on 
  io_ctx.Address     = STTS22H_I2C_ADD_H;
  io_ctx.Init        = BSP_I2C3_Init;
  io_ctx.DeInit      = BSP_I2C3_DeInit;
  io_ctx.ReadReg     = BSP_I2C3_ReadReg; 
  io_ctx.WriteReg    = BSP_I2C3_WriteReg;
#endif
  io_ctx.GetTick = BSP_GetTick;

  if(STTS22H_RegisterBusIO(&stts22h_obj_0, &io_ctx) != STTS22H_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(STTS22H_ReadID(&stts22h_obj_0, &id) != STTS22H_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(id != STTS22H_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void) STTS22H_GetCapabilities(&stts22h_obj_0, &cap);

    EnvCtx[STTS22H_0].Functions = ((uint32_t) cap.Temperature) | ((uint32_t) cap.Pressure << 1) | ((uint32_t) cap.Humidity << 2);

    EnvCompObj[STTS22H_0] = &stts22h_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    EnvDrv[STTS22H_0] = (ENV_SENSOR_CommonDrv_t*) (void*) &STTS22H_COMMON_Driver;

    if((ret == BSP_ERROR_NONE) && ((Functions & ENV_TEMPERATURE) == ENV_TEMPERATURE) && (cap.Temperature == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      EnvFuncDrv[STTS22H_0][FunctionIndex[ENV_TEMPERATURE]] = (ENV_SENSOR_FuncDrv_t*) (void*) &STTS22H_TEMP_Driver;

      if(EnvDrv[STTS22H_0]->Init(EnvCompObj[STTS22H_0]) != STTS22H_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if(((Functions & ENV_PRESSURE) == ENV_PRESSURE) && (cap.Pressure == 0U))
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }
  return ret;
}

static int32_t BSP_STTS22H_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(BSP_I2C2_Init() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  /* GPIO Ports Clock Enable */
  BSP_STTS22H_INT_GPIO_CLK_ENABLE();

  /*Configure GPIO pin : PC1 */
  GPIO_InitStruct.Pin = BSP_STTS22H_INT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BSP_STTS22H_INT_PORT, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  /* Enable and set EXTI Interrupt priority */
  HAL_NVIC_SetPriority(BSP_STTS22H_INT_EXTI_IRQn, BSP_STTS22H_INT_EXTI_IRQ_PP, BSP_STTS22H_INT_EXTI_IRQ_SP);
  HAL_NVIC_EnableIRQ(BSP_STTS22H_INT_EXTI_IRQn);

  return ret;
}

static int32_t BSP_STTS22H_DeInit(void)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(BSP_I2C2_DeInit() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

#endif
