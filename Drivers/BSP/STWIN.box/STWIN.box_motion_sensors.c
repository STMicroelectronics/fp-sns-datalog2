/**
 ******************************************************************************
 * @file    STWIN.box_motion_sensors.c
 * @author  SRA
 * @brief   This file provides BSP Motion Sensors interface
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
#include "STWIN.box_motion_sensors.h"

extern void *MotionCompObj[MOTION_INSTANCES_NBR]; /* This "redundant" line is here to fulfil MISRA C-2012 rule 8.4 */
void *MotionCompObj[MOTION_INSTANCES_NBR];

/* We define a jump table in order to get the correct index from the desired function. */
/* This table should have a size equal to the maximum value of a function plus 1.      */
static uint32_t FunctionIndex[5] = { 0, 0, 1, 1, 2 };
static MOTION_SENSOR_FuncDrv_t *MotionFuncDrv[MOTION_INSTANCES_NBR][MOTION_FUNCTIONS_NBR];
static MOTION_SENSOR_CommonDrv_t *MotionDrv[MOTION_INSTANCES_NBR];
static MOTION_SENSOR_Ctx_t MotionCtx[MOTION_INSTANCES_NBR];

#if (USE_MOTION_SENSOR_IIS2DLPC_0 == 1)
static int32_t IIS2DLPC_0_Probe(uint32_t Functions);
#endif

#if (USE_MOTION_SENSOR_IIS2MDC_0 == 1)
static int32_t IIS2MDC_0_Probe(uint32_t Functions);
#endif

#if (USE_MOTION_SENSOR_IIS3DWB_0 == 1)
static int32_t IIS3DWB_0_Probe(uint32_t Functions);
#endif

#if (USE_MOTION_SENSOR_ISM330DHCX_0 == 1)
static int32_t ISM330DHCX_0_Probe(uint32_t Functions);
#endif

#if (USE_MOTION_SENSOR_IIS2ICLX_0 == 1)
static int32_t IIS2ICLX_0_Probe(uint32_t Functions);
#endif

#if (USE_MOTION_SENSOR_IIS2DLPC_0 == 1)
static int32_t BSP_IIS2DLPC_Init(void);
static int32_t BSP_IIS2DLPC_DeInit(void);
static int32_t BSP_IIS2DLPC_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
static int32_t BSP_IIS2DLPC_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
#endif

#if (USE_MOTION_SENSOR_IIS2MDC_0 == 1)
static int32_t BSP_IIS2MDC_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
static int32_t BSP_IIS2MDC_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
#endif

#if (USE_MOTION_SENSOR_IIS3DWB_0 == 1)
static int32_t BSP_IIS3DWB_Init(void);
static int32_t BSP_IIS3DWB_DeInit(void);
static int32_t BSP_IIS3DWB_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
static int32_t BSP_IIS3DWB_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
#endif

#if (USE_MOTION_SENSOR_ISM330DHCX_0 == 1)
static int32_t BSP_ISM330DHCX_Init(void);
static int32_t BSP_ISM330DHCX_DeInit(void);
static int32_t BSP_ISM330DHCX_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
static int32_t BSP_ISM330DHCX_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
#endif

#if (USE_MOTION_SENSOR_IIS2ICLX_0 == 1)
static int32_t BSP_IIS2ICLX_Init(void);
static int32_t BSP_IIS2ICLX_DeInit(void);
static int32_t BSP_IIS2ICLX_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
static int32_t BSP_IIS2ICLX_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
#endif

/**
 * @brief  Initializes the motion sensors
 * @param  Instance Motion sensor instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Init(uint32_t Instance, uint32_t Functions)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t function = MOTION_GYRO;
  uint32_t i;
  uint32_t component_functions = 0;
  MOTION_SENSOR_Capabilities_t cap;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_IIS2DLPC_0 == 1)
    case IIS2DLPC_0:
      if(IIS2DLPC_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if(MotionDrv[Instance]->GetCapabilities(MotionCompObj[Instance], (void*) &cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if(cap.Acc == 1U)
      {
        component_functions |= MOTION_ACCELERO;
      }
      if(cap.Gyro == 1U)
      {
        component_functions |= MOTION_GYRO;
      }
      if(cap.Magneto == 1U)
      {
        component_functions |= MOTION_MAGNETO;
      }
      break;
#endif

#if (USE_MOTION_SENSOR_IIS2MDC_0 == 1)
    case IIS2MDC_0:
      if(IIS2MDC_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if(MotionDrv[Instance]->GetCapabilities(MotionCompObj[Instance], (void*) &cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if(cap.Acc == 1U)
      {
        component_functions |= MOTION_ACCELERO;
      }
      if(cap.Gyro == 1U)
      {
        component_functions |= MOTION_GYRO;
      }
      if(cap.Magneto == 1U)
      {
        component_functions |= MOTION_MAGNETO;
      }
      break;
#endif

#if (USE_MOTION_SENSOR_IIS3DWB_0 == 1)
    case IIS3DWB_0:
      if(IIS3DWB_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if(MotionDrv[Instance]->GetCapabilities(MotionCompObj[Instance], (void*) &cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if(cap.Acc == 1U)
      {
        component_functions |= MOTION_ACCELERO;
      }
      if(cap.Gyro == 1U)
      {
        component_functions |= MOTION_GYRO;
      }
      if(cap.Magneto == 1U)
      {
        component_functions |= MOTION_MAGNETO;
      }
      break;
#endif

#if (USE_MOTION_SENSOR_ISM330DHCX_0 == 1)
    case ISM330DHCX_0:
      if(ISM330DHCX_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if(MotionDrv[Instance]->GetCapabilities(MotionCompObj[Instance], (void*) &cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if(cap.Acc == 1U)
      {
        component_functions |= MOTION_ACCELERO;
      }
      if(cap.Gyro == 1U)
      {
        component_functions |= MOTION_GYRO;
      }
      if(cap.Magneto == 1U)
      {
        component_functions |= MOTION_MAGNETO;
      }
      break;
#endif

#if (USE_MOTION_SENSOR_IIS2ICLX_0 == 1)
    case IIS2ICLX_0:
      if(IIS2ICLX_0_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if(MotionDrv[Instance]->GetCapabilities(MotionCompObj[Instance], (void*) &cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if(cap.Acc == 1U)
      {
        component_functions |= MOTION_ACCELERO;
      }
      if(cap.Gyro == 1U)
      {
        component_functions |= MOTION_GYRO;
      }
      if(cap.Magneto == 1U)
      {
        component_functions |= MOTION_MAGNETO;
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

  for(i = 0; i < MOTION_FUNCTIONS_NBR; i++)
  {
    if(((Functions & function) == function) && ((component_functions & function) == function))
    {
      if(MotionFuncDrv[Instance][FunctionIndex[function]]->Enable(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(MotionDrv[Instance]->DeInit(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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
int32_t BSP_MOTION_SENSOR_GetCapabilities(uint32_t Instance, MOTION_SENSOR_Capabilities_t *Capabilities)
{
  int32_t ret;

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(MotionDrv[Instance]->GetCapabilities(MotionCompObj[Instance], Capabilities) != BSP_ERROR_NONE)
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

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(MotionDrv[Instance]->ReadID(MotionCompObj[Instance], Id) != BSP_ERROR_NONE)
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

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->Enable(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->Disable(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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
 * @brief  Get accelero axes data
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

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->GetAxes(MotionCompObj[Instance], Axes) != BSP_ERROR_NONE)
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
 * @brief  Get accelero axes raw data
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

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->GetAxesRaw(MotionCompObj[Instance], Axes) != BSP_ERROR_NONE)
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
 * @brief  Get accelero sensitivity
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Sensitivity pointer to sensitivity read value
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_GetSensitivity(uint32_t Instance, uint32_t Function, float_t *Sensitivity)
{
  int32_t ret;

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->GetSensitivity(MotionCompObj[Instance], Sensitivity) != BSP_ERROR_NONE)
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
 * @brief  Get accelero Output Data Rate
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Odr pointer to Output Data Rate read value
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float_t *Odr)
{
  int32_t ret;

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->GetOutputDataRate(MotionCompObj[Instance], Odr) != BSP_ERROR_NONE)
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
 * @brief  Get accelero Full Scale
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

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->GetFullScale(MotionCompObj[Instance], Fullscale) != BSP_ERROR_NONE)
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
 * @brief  Set accelero Output Data Rate
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Odr Output Data Rate value to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float_t Odr)
{
  int32_t ret;

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->SetOutputDataRate(MotionCompObj[Instance], Odr) != BSP_ERROR_NONE)
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
 * @brief  Set accelero Full Scale
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

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->SetFullScale(MotionCompObj[Instance], Fullscale) != BSP_ERROR_NONE)
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

#if (USE_MOTION_SENSOR_IIS2DLPC_0  == 1)
/**
 * @brief  Register Bus IOs for instance 0 if component ID is OK
 * @retval BSP status
 */
static int32_t IIS2DLPC_0_Probe(uint32_t Functions)
{
  IIS2DLPC_IO_t io_ctx;
  uint8_t id;
  static IIS2DLPC_Object_t iis2dlpc_obj_0;
  IIS2DLPC_Capabilities_t cap;
  int32_t ret = BSP_ERROR_NONE;

  /* Configure the accelero driver */
  io_ctx.BusType = IIS2DLPC_SPI_4WIRES_BUS; /* SPI 4-Wires */
  io_ctx.Address = 0x0;
  io_ctx.Init = BSP_IIS2DLPC_Init;
  io_ctx.DeInit = BSP_IIS2DLPC_DeInit;
  io_ctx.ReadReg = BSP_IIS2DLPC_ReadReg;
  io_ctx.WriteReg = BSP_IIS2DLPC_WriteReg;
  io_ctx.GetTick = BSP_GetTick;

  if(IIS2DLPC_RegisterBusIO(&iis2dlpc_obj_0, &io_ctx) != IIS2DLPC_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(IIS2DLPC_ReadID(&iis2dlpc_obj_0, &id) != IIS2DLPC_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(id != IIS2DLPC_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void) IIS2DLPC_GetCapabilities(&iis2dlpc_obj_0, &cap);
    MotionCtx[IIS2DLPC_0].Functions = ((uint32_t) cap.Gyro) | ((uint32_t) cap.Acc << 1) | ((uint32_t) cap.Magneto << 2);

    MotionCompObj[IIS2DLPC_0] = &iis2dlpc_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[IIS2DLPC_0] = (MOTION_SENSOR_CommonDrv_t*) (void*) &IIS2DLPC_COMMON_Driver;

    if(((Functions & MOTION_ACCELERO) == MOTION_ACCELERO) && (cap.Acc == 1U))
    {

      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[IIS2DLPC_0][FunctionIndex[MOTION_ACCELERO]] = (MOTION_SENSOR_FuncDrv_t*) (void*) &IIS2DLPC_ACC_Driver;

      if(MotionDrv[IIS2DLPC_0]->Init(MotionCompObj[IIS2DLPC_0]) != IIS2DLPC_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if(((Functions & MOTION_GYRO) == MOTION_GYRO) && (cap.Gyro == 0U))
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    if(((Functions & MOTION_MAGNETO) == MOTION_MAGNETO) && (cap.Magneto == 0U))
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}

static int32_t BSP_IIS2DLPC_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  /* Configure IIS2DLPC INT1 pin */
  BSP_IIS2DLPC_INT1_GPIO_CLK_ENABLE();
  GPIO_InitStruct.Pin = BSP_IIS2DLPC_INT1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BSP_IIS2DLPC_INT1_PORT, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  /* Enable and set EXTI Interrupt priority */
  HAL_NVIC_SetPriority(BSP_IIS2DLPC_INT1_EXTI_IRQn, BSP_IIS2DLPC_INT1_EXTI_IRQ_PP, BSP_IIS2DLPC_INT1_EXTI_IRQ_SP);
  HAL_NVIC_EnableIRQ(BSP_IIS2DLPC_INT1_EXTI_IRQn);

  /* Configure IIS2DLPC INT2 pin */
  BSP_IIS2DLPC_INT2_GPIO_CLK_ENABLE();
  GPIO_InitStruct.Pin = BSP_IIS2DLPC_INT2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BSP_IIS2DLPC_INT2_PORT, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  /* Enable and set EXTI Interrupt priority */
  HAL_NVIC_SetPriority(BSP_IIS2DLPC_INT2_EXTI_IRQn, BSP_IIS2DLPC_INT2_EXTI_IRQ_PP, BSP_IIS2DLPC_INT2_EXTI_IRQ_SP);
  HAL_NVIC_EnableIRQ(BSP_IIS2DLPC_INT2_EXTI_IRQn);

  BSP_IIS2DLPC_CS_GPIO_CLK_ENABLE();
  /* Configure IIS2DLPC CS pin */
  HAL_GPIO_WritePin(BSP_IIS2DLPC_CS_PORT, BSP_IIS2DLPC_CS_PIN, GPIO_PIN_SET);

  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct.Pin = BSP_IIS2DLPC_CS_PIN;
  HAL_GPIO_Init(BSP_IIS2DLPC_CS_PORT, &GPIO_InitStruct);

  /* Configure SPI */
  if(BSP_SPI2_Init() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

static int32_t BSP_IIS2DLPC_DeInit(void)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(BSP_SPI2_DeInit() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

static int32_t BSP_IIS2DLPC_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  UNUSED(Addr);
  int32_t ret = BSP_ERROR_NONE;
  uint8_t dataReg = (uint8_t) Reg;

  /* CS Enable */
  HAL_GPIO_WritePin(BSP_IIS2DLPC_CS_PORT, BSP_IIS2DLPC_CS_PIN, GPIO_PIN_RESET);

  ret = BSP_SPI2_Send(&dataReg, 1);

  if(ret == BSP_ERROR_NONE)
  {
    ret = BSP_SPI2_Send(pdata, len);
  }

  /* CS Disable */
  HAL_GPIO_WritePin(BSP_IIS2DLPC_CS_PORT, BSP_IIS2DLPC_CS_PIN, GPIO_PIN_SET);

  return ret;
}

static int32_t BSP_IIS2DLPC_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  UNUSED(Addr);
  int32_t ret = BSP_ERROR_NONE;
  uint8_t dataReg = (uint8_t) Reg;

  dataReg |= 0x80U;

  /* CS Enable */
  HAL_GPIO_WritePin(BSP_IIS2DLPC_CS_PORT, BSP_IIS2DLPC_CS_PIN, GPIO_PIN_RESET);

  ret = BSP_SPI2_Send(&dataReg, 1);

  if(ret == BSP_ERROR_NONE)
  {
    ret = BSP_SPI2_Recv(pdata, len);
  }

  /* CS Disable */
  HAL_GPIO_WritePin(BSP_IIS2DLPC_CS_PORT, BSP_IIS2DLPC_CS_PIN, GPIO_PIN_SET);

  return ret;
}
#endif

#if (USE_MOTION_SENSOR_IIS2MDC_0  == 1)
/**
 * @brief  Register Bus IOs for instance 0 if component ID is OK
 * @retval BSP status
 */
static int32_t IIS2MDC_0_Probe(uint32_t Functions)
{
  IIS2MDC_IO_t io_ctx;
  uint8_t id;
  static IIS2MDC_Object_t iis2mdc_obj_0;
  IIS2MDC_Capabilities_t cap;
  int32_t ret = BSP_ERROR_NONE;

  /* Configure the accelero driver */
  io_ctx.BusType = IIS2MDC_I2C_BUS; /* I2C */
  io_ctx.Address = IIS2MDC_I2C_ADD;
  io_ctx.Init = BSP_I2C2_Init;
  io_ctx.DeInit = BSP_I2C2_DeInit;
  io_ctx.ReadReg = BSP_IIS2MDC_ReadReg;
  io_ctx.WriteReg = BSP_IIS2MDC_WriteReg;
  io_ctx.GetTick = BSP_GetTick;

  if(IIS2MDC_RegisterBusIO(&iis2mdc_obj_0, &io_ctx) != IIS2MDC_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(IIS2MDC_ReadID(&iis2mdc_obj_0, &id) != IIS2MDC_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(id != IIS2MDC_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void) IIS2MDC_GetCapabilities(&iis2mdc_obj_0, &cap);
    MotionCtx[IIS2MDC_0].Functions = ((uint32_t) cap.Gyro) | ((uint32_t) cap.Acc << 1) | ((uint32_t) cap.Magneto << 2);

    MotionCompObj[IIS2MDC_0] = &iis2mdc_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[IIS2MDC_0] = (MOTION_SENSOR_CommonDrv_t*) (void*) &IIS2MDC_COMMON_Driver;

    if(((Functions & MOTION_MAGNETO) == MOTION_MAGNETO) && (cap.Magneto == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[IIS2MDC_0][FunctionIndex[MOTION_MAGNETO]] = (MOTION_SENSOR_FuncDrv_t*) (void*) &IIS2MDC_MAG_Driver;

      if(MotionDrv[IIS2MDC_0]->Init(MotionCompObj[IIS2MDC_0]) != IIS2MDC_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if(((Functions & MOTION_GYRO) == MOTION_GYRO) && (cap.Gyro == 0U))
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    if(((Functions & MOTION_ACCELERO) == MOTION_ACCELERO) && (cap.Acc == 0U))
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}

int32_t BSP_IIS2MDC_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  int32_t ret = BSP_ERROR_NONE;
  uint16_t dataReg = (uint16_t) Reg;

  if(BSP_I2C2_WriteReg(Addr, dataReg, pdata, len) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_BUS_FAILURE;
  }

  return ret;
}

int32_t BSP_IIS2MDC_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  int32_t ret = BSP_ERROR_NONE;
  uint16_t dataReg = (uint16_t) Reg;

  dataReg |= 0x80U;

  if(BSP_I2C2_ReadReg(Addr, dataReg, pdata, len) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_BUS_FAILURE;
  }

  return ret;
}

#endif

#if (USE_MOTION_SENSOR_IIS3DWB_0  == 1)
/**
 * @brief  Register Bus IOs for instance 0 if component ID is OK
 * @retval BSP status
 */
static int32_t IIS3DWB_0_Probe(uint32_t Functions)
{
  IIS3DWB_IO_t io_ctx;
  uint8_t id;
  static IIS3DWB_Object_t iis3dwb_obj_0;
  IIS3DWB_Capabilities_t cap;
  int32_t ret = BSP_ERROR_NONE;

  /* Configure the accelero driver */
  io_ctx.BusType = IIS3DWB_SPI_4WIRES_BUS; /* SPI 4-Wires */
  io_ctx.Address = 0x0;
  io_ctx.Init = BSP_IIS3DWB_Init;
  io_ctx.DeInit = BSP_IIS3DWB_DeInit;
  io_ctx.ReadReg = BSP_IIS3DWB_ReadReg;
  io_ctx.WriteReg = BSP_IIS3DWB_WriteReg;
  io_ctx.GetTick = BSP_GetTick;

  if(IIS3DWB_RegisterBusIO(&iis3dwb_obj_0, &io_ctx) != IIS3DWB_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(IIS3DWB_ReadID(&iis3dwb_obj_0, &id) != IIS3DWB_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(id != IIS3DWB_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void) IIS3DWB_GetCapabilities(&iis3dwb_obj_0, &cap);
    MotionCtx[IIS3DWB_0].Functions = ((uint32_t) cap.Gyro) | ((uint32_t) cap.Acc << 1) | ((uint32_t) cap.Magneto << 2);

    MotionCompObj[IIS3DWB_0] = &iis3dwb_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[IIS3DWB_0] = (MOTION_SENSOR_CommonDrv_t*) (void*) &IIS3DWB_COMMON_Driver;

    if(((Functions & MOTION_ACCELERO) == MOTION_ACCELERO) && (cap.Acc == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[IIS3DWB_0][FunctionIndex[MOTION_ACCELERO]] = (MOTION_SENSOR_FuncDrv_t*) (void*) &IIS3DWB_ACC_Driver;

      if(MotionDrv[IIS3DWB_0]->Init(MotionCompObj[IIS3DWB_0]) != IIS3DWB_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if(((Functions & MOTION_GYRO) == MOTION_GYRO) && (cap.Gyro == 0U))
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    if(((Functions & MOTION_MAGNETO) == MOTION_MAGNETO) && (cap.Magneto == 0U))
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}

static int32_t BSP_IIS3DWB_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  /* Configure IIS3DWB INT1 pin */
  BSP_IIS3DWB_INT1_GPIO_CLK_ENABLE();
  GPIO_InitStruct.Pin = BSP_IIS3DWB_INT1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BSP_IIS3DWB_INT1_PORT, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  /* Enable and set EXTI Interrupt priority */
  HAL_NVIC_SetPriority(BSP_IIS3DWB_INT1_EXTI_IRQn, BSP_IIS3DWB_INT1_EXTI_IRQ_PP, BSP_IIS3DWB_INT1_EXTI_IRQ_SP);
  HAL_NVIC_EnableIRQ(BSP_IIS3DWB_INT1_EXTI_IRQn);

  BSP_IIS3DWB_CS_GPIO_CLK_ENABLE();
  /* Configure IIS3DWB CS pin */
  HAL_GPIO_WritePin(BSP_IIS3DWB_CS_PORT, BSP_IIS3DWB_CS_PIN, GPIO_PIN_SET);

  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct.Pin = BSP_IIS3DWB_CS_PIN;
  HAL_GPIO_Init(BSP_IIS3DWB_CS_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(BSP_IIS3DWB_CS_PORT, BSP_IIS3DWB_CS_PIN, GPIO_PIN_SET);

  /* Configure SPI */
  if(BSP_SPI2_Init() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

static int32_t BSP_IIS3DWB_DeInit(void)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(BSP_SPI2_DeInit() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

static int32_t BSP_IIS3DWB_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  UNUSED(Addr);
  int32_t ret = BSP_ERROR_NONE;
  uint8_t dataReg = (uint8_t) Reg;

  /* CS Enable */
  HAL_GPIO_WritePin(BSP_IIS3DWB_CS_PORT, BSP_IIS3DWB_CS_PIN, GPIO_PIN_RESET);

  ret = BSP_SPI2_Send(&dataReg, 1);

  if(ret == BSP_ERROR_NONE)
  {
    ret = BSP_SPI2_Send(pdata, len);
  }

  /* CS Disable */
  HAL_GPIO_WritePin(BSP_IIS3DWB_CS_PORT, BSP_IIS3DWB_CS_PIN, GPIO_PIN_SET);

  return ret;
}

static int32_t BSP_IIS3DWB_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  UNUSED(Addr);
  int32_t ret = BSP_ERROR_NONE;
  uint8_t dataReg = (uint8_t) Reg;

  dataReg |= 0x80U;

  /* CS Enable */
  HAL_GPIO_WritePin(BSP_IIS3DWB_CS_PORT, BSP_IIS3DWB_CS_PIN, GPIO_PIN_RESET);

  ret = BSP_SPI2_Send(&dataReg, 1);

  if(ret == BSP_ERROR_NONE)
  {
    ret = BSP_SPI2_Recv(pdata, len);
  }

  /* CS Disable */
  HAL_GPIO_WritePin(BSP_IIS3DWB_CS_PORT, BSP_IIS3DWB_CS_PIN, GPIO_PIN_SET);

  return ret;
}
#endif

#if (USE_MOTION_SENSOR_ISM330DHCX_0 == 1)

/**
 * @brief  Register Bus IOs for instance 0 if component ID is OK
 * @retval BSP status
 */
static int32_t ISM330DHCX_0_Probe(uint32_t Functions)
{
  ISM330DHCX_IO_t io_ctx;
  uint8_t id;
  static ISM330DHCX_Object_t ism330dhcx_obj_0;
  ISM330DHCX_Capabilities_t cap;
  int32_t ret = BSP_ERROR_NONE;

  /* Configure the accelero driver */
  io_ctx.BusType = ISM330DHCX_SPI_4WIRES_BUS; /* SPI 4-Wires */
  io_ctx.Address = 0x0;
  io_ctx.Init = BSP_ISM330DHCX_Init;
  io_ctx.DeInit = BSP_ISM330DHCX_DeInit;
  io_ctx.ReadReg = BSP_ISM330DHCX_ReadReg;
  io_ctx.WriteReg = BSP_ISM330DHCX_WriteReg;
  io_ctx.GetTick = BSP_GetTick;

  if(ISM330DHCX_RegisterBusIO(&ism330dhcx_obj_0, &io_ctx) != ISM330DHCX_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(ISM330DHCX_ReadID(&ism330dhcx_obj_0, &id) != ISM330DHCX_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(id != ISM330DHCX_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void) ISM330DHCX_GetCapabilities(&ism330dhcx_obj_0, &cap);
    MotionCtx[ISM330DHCX_0].Functions = ((uint32_t) cap.Gyro) | ((uint32_t) cap.Acc << 1) | ((uint32_t) cap.Magneto << 2);

    MotionCompObj[ISM330DHCX_0] = &ism330dhcx_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[ISM330DHCX_0] = (MOTION_SENSOR_CommonDrv_t*) (void*) &ISM330DHCX_COMMON_Driver;

    if(((Functions & MOTION_ACCELERO) == MOTION_ACCELERO) && (cap.Acc == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[ISM330DHCX_0][FunctionIndex[MOTION_ACCELERO]] = (MOTION_SENSOR_FuncDrv_t*) (void*) &ISM330DHCX_ACC_Driver;

      if(MotionDrv[ISM330DHCX_0]->Init(MotionCompObj[ISM330DHCX_0]) != ISM330DHCX_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }

    if(((Functions & MOTION_GYRO) == MOTION_GYRO) && (cap.Gyro == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[ISM330DHCX_0][FunctionIndex[MOTION_GYRO]] = (MOTION_SENSOR_FuncDrv_t*) (void*) &ISM330DHCX_GYRO_Driver;

      if(MotionDrv[ISM330DHCX_0]->Init(MotionCompObj[ISM330DHCX_0]) != ISM330DHCX_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if(((Functions & MOTION_MAGNETO) == MOTION_MAGNETO) && (cap.Magneto == 0U))
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}

static int32_t BSP_ISM330DHCX_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  /* Configure ISM330DHCX INT1 pin */
  BSP_ISM330DHCX_INT1_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin = BSP_ISM330DHCX_INT1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BSP_ISM330DHCX_INT1_PORT, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  /* Enable and set EXTI Interrupt priority */
  HAL_NVIC_SetPriority(BSP_ISM330DHCX_INT1_EXTI_IRQn, BSP_ISM330DHCX_INT1_EXTI_IRQ_PP, BSP_ISM330DHCX_INT1_EXTI_IRQ_SP);
  HAL_NVIC_EnableIRQ(BSP_ISM330DHCX_INT1_EXTI_IRQn);

  /* Configure ISM330DHCX INT2 pin */
  BSP_ISM330DHCX_INT2_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin = BSP_ISM330DHCX_INT2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BSP_ISM330DHCX_INT2_PORT, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  /* Enable and set EXTI Interrupt priority */
  HAL_NVIC_SetPriority(BSP_ISM330DHCX_INT2_EXTI_IRQn, BSP_ISM330DHCX_INT2_EXTI_IRQ_PP, BSP_ISM330DHCX_INT2_EXTI_IRQ_SP);
  HAL_NVIC_EnableIRQ(BSP_ISM330DHCX_INT2_EXTI_IRQn);

  /* Configure ISM330DHCX CS pin */
  BSP_ISM330DHCX_CS_GPIO_CLK_ENABLE();
  HAL_GPIO_WritePin(BSP_ISM330DHCX_CS_PORT, BSP_ISM330DHCX_CS_PIN, GPIO_PIN_SET);

  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct.Pin = BSP_ISM330DHCX_CS_PIN;
  HAL_GPIO_Init(BSP_ISM330DHCX_CS_PORT, &GPIO_InitStruct);

  if(BSP_SPI2_Init() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

static int32_t BSP_ISM330DHCX_DeInit(void)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(BSP_SPI2_DeInit() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

static int32_t BSP_ISM330DHCX_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  UNUSED(Addr);
  int32_t ret;
  uint8_t dataReg = (uint8_t) Reg;

  /* CS Enable */
  HAL_GPIO_WritePin(BSP_ISM330DHCX_CS_PORT, BSP_ISM330DHCX_CS_PIN, GPIO_PIN_RESET);

  ret = BSP_SPI2_Send(&dataReg, 1);

  if(ret == BSP_ERROR_NONE)
  {
    ret = BSP_SPI2_Send(pdata, len);
  }

  /* CS Disable */
  HAL_GPIO_WritePin(BSP_ISM330DHCX_CS_PORT, BSP_ISM330DHCX_CS_PIN, GPIO_PIN_SET);

  return ret;
}

static int32_t BSP_ISM330DHCX_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  UNUSED(Addr);
  int32_t ret;
  uint8_t dataReg = (uint8_t) Reg;

  dataReg |= 0x80U;

  /* CS Enable */
  HAL_GPIO_WritePin(BSP_ISM330DHCX_CS_PORT, BSP_ISM330DHCX_CS_PIN, GPIO_PIN_RESET);

  ret = BSP_SPI2_Send(&dataReg, 1);

  if(ret == BSP_ERROR_NONE)
  {
    ret = BSP_SPI2_Recv(pdata, len);
  }

  /* CS Disable */
  HAL_GPIO_WritePin(BSP_ISM330DHCX_CS_PORT, BSP_ISM330DHCX_CS_PIN, GPIO_PIN_SET);

  return ret;
}
#endif

#if (USE_MOTION_SENSOR_IIS2ICLX_0 == 1)

/**
 * @brief  Register Bus IOs for instance 0 if component ID is OK
 * @retval BSP status
 */
static int32_t IIS2ICLX_0_Probe(uint32_t Functions)
{
  IIS2ICLX_IO_t io_ctx;
  uint8_t id;
  static IIS2ICLX_Object_t iis2iclx_obj_0;
  IIS2ICLX_Capabilities_t cap;
  int32_t ret = BSP_ERROR_NONE;

  /* Configure the accelero driver */
  io_ctx.BusType = IIS2ICLX_SPI_4WIRES_BUS; /* SPI 4-Wires */
  io_ctx.Address = 0x0;
  io_ctx.Init = BSP_IIS2ICLX_Init;
  io_ctx.DeInit = BSP_IIS2ICLX_DeInit;
  io_ctx.ReadReg = BSP_IIS2ICLX_ReadReg;
  io_ctx.WriteReg = BSP_IIS2ICLX_WriteReg;
  io_ctx.GetTick = BSP_GetTick;

  if(IIS2ICLX_RegisterBusIO(&iis2iclx_obj_0, &io_ctx) != IIS2ICLX_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(IIS2ICLX_ReadID(&iis2iclx_obj_0, &id) != IIS2ICLX_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(id != IIS2ICLX_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void) IIS2ICLX_GetCapabilities(&iis2iclx_obj_0, &cap);
    MotionCtx[IIS2ICLX_0].Functions = ((uint32_t) cap.Gyro) | ((uint32_t) cap.Acc << 1) | ((uint32_t) cap.Magneto << 2);

    MotionCompObj[IIS2ICLX_0] = &iis2iclx_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[IIS2ICLX_0] = (MOTION_SENSOR_CommonDrv_t*) (void*) &IIS2ICLX_COMMON_Driver;

    if(((Functions & MOTION_ACCELERO) == MOTION_ACCELERO) && (cap.Acc == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[IIS2ICLX_0][FunctionIndex[MOTION_ACCELERO]] = (MOTION_SENSOR_FuncDrv_t*) (void*) &IIS2ICLX_ACC_Driver;

      if(MotionDrv[IIS2ICLX_0]->Init(MotionCompObj[IIS2ICLX_0]) != IIS2ICLX_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }

    if(((Functions & MOTION_GYRO) == MOTION_GYRO) && (cap.Gyro == 1U))
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    if(((Functions & MOTION_MAGNETO) == MOTION_MAGNETO) && (cap.Magneto == 0U))
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}

static int32_t BSP_IIS2ICLX_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  /* Configure IIS2ICLX INT1 pin */
  BSP_IIS2ICLX_INT1_GPIO_CLK_ENABLE();
  GPIO_InitStruct.Pin = BSP_IIS2ICLX_INT1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BSP_IIS2ICLX_INT1_PORT, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  /* Enable and set EXTI Interrupt priority */
  HAL_NVIC_SetPriority(BSP_IIS2ICLX_INT1_EXTI_IRQn, BSP_IIS2ICLX_INT1_EXTI_IRQ_PP, BSP_IIS2ICLX_INT1_EXTI_IRQ_SP);
  HAL_NVIC_EnableIRQ(BSP_IIS2ICLX_INT1_EXTI_IRQn);

  /* Configure IIS2ICLX INT2 pin */
  BSP_IIS2ICLX_INT2_GPIO_CLK_ENABLE();
  GPIO_InitStruct.Pin = BSP_IIS2ICLX_INT2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BSP_IIS2ICLX_INT2_PORT, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  /* Enable and set EXTI Interrupt priority */
  HAL_NVIC_SetPriority(BSP_IIS2ICLX_INT2_EXTI_IRQn, BSP_IIS2ICLX_INT2_EXTI_IRQ_PP, BSP_IIS2ICLX_INT2_EXTI_IRQ_SP);
  HAL_NVIC_EnableIRQ(BSP_IIS2ICLX_INT2_EXTI_IRQn);

  /* Configure IIS2ICLX CS pin */
  BSP_IIS2ICLX_CS_GPIO_CLK_ENABLE();
  HAL_GPIO_WritePin(BSP_IIS2ICLX_CS_PORT, BSP_IIS2ICLX_CS_PIN, GPIO_PIN_SET);

  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct.Pin = BSP_IIS2ICLX_CS_PIN;
  HAL_GPIO_Init(BSP_IIS2ICLX_CS_PORT, &GPIO_InitStruct);

  if(BSP_SPI2_Init() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

static int32_t BSP_IIS2ICLX_DeInit(void)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(BSP_SPI2_DeInit() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

static int32_t BSP_IIS2ICLX_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  UNUSED(Addr);
  int32_t ret = BSP_ERROR_NONE;
  uint8_t dataReg = (uint8_t) Reg;

  /* CS Enable */
  HAL_GPIO_WritePin(BSP_IIS2ICLX_CS_PORT, BSP_IIS2ICLX_CS_PIN, GPIO_PIN_RESET);

  ret = BSP_SPI2_Send(&dataReg, 1);

  if(ret == BSP_ERROR_NONE)
  {
    ret = BSP_SPI2_Send(pdata, len);
  }

  /* CS Disable */
  HAL_GPIO_WritePin(BSP_IIS2ICLX_CS_PORT, BSP_IIS2ICLX_CS_PIN, GPIO_PIN_SET);

  return ret;
}

static int32_t BSP_IIS2ICLX_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  UNUSED(Addr);
  int32_t ret = BSP_ERROR_NONE;
  uint8_t dataReg = (uint8_t) Reg;

  dataReg |= 0x80U;

  /* CS Enable */
  HAL_GPIO_WritePin(BSP_IIS2ICLX_CS_PORT, BSP_IIS2ICLX_CS_PIN, GPIO_PIN_RESET);

  ret = BSP_SPI2_Send(&dataReg, 1);

  if(ret == BSP_ERROR_NONE)
  {
    ret = BSP_SPI2_Recv(pdata, len);
  }

  /* CS Disable */
  HAL_GPIO_WritePin(BSP_IIS2ICLX_CS_PORT, BSP_IIS2ICLX_CS_PIN, GPIO_PIN_SET);

  return ret;
}
#endif
