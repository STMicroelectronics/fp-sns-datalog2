/**
 ******************************************************************************
 * @file    STWIN.box_env_sensors_ex.c
 * @author  SRA
 * @brief   This file provides Extended BSP Environmental Sensors interface
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
#include "STWIN.box_env_sensors_ex.h"

/** @addtogroup BSP BSP
 * @{
 */

/** @addtogroup STWIN_BOX STWIN_BOX
 * @{
 */

/** @defgroup STWIN_BOX_ENV_SENSOR_EX STWIN ENV SENSOR EX
 * @{
 */

/** @defgroup STWIN_BOX_ENV_SENSOR_EX_Imported_Variables STWIN ENV SENSOR EX Imported Variables
 * @{
 */

extern void *EnvCompObj[ENV_INSTANCES_NBR];

/**
 * @}
 */

/** @defgroup STWIN_BOX_ENV_SENSOR_EX_Exported_Functions STWIN ENV SENSOR EX Exported Functions
 * @{
 */

/**
 * @brief  Get the status of data ready bit (available only for LPS22HH sensors)
 * @param  Instance the device instance
 * @param  Function Environmental sensor function. Could be:
 *         - ENV_PRESSURE or ENV_TEMPERATURE for instance LPS22HH_0
 *         - ENV_TEMPERATURE for instance STTS751_0
 * @param  Status the pointer to the status
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Get_DRDY_Status(uint32_t Instance, uint32_t Function, uint8_t *Status)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      if ((Function & ENV_PRESSURE) == ENV_PRESSURE)
      {
        if (LPS22HH_PRESS_Get_DRDY_Status(EnvCompObj[Instance], Status) != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          ret = BSP_ERROR_NONE;
        }
      }
      else if ((Function & ENV_TEMPERATURE) == ENV_TEMPERATURE)
      {
        if (LPS22HH_TEMP_Get_DRDY_Status(EnvCompObj[Instance], Status) != BSP_ERROR_NONE)
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
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      if ((Function & ENV_TEMPERATURE) == ENV_TEMPERATURE)
      {
        if (STTS751_TEMP_Get_DRDY_Status(EnvCompObj[Instance], Status) != BSP_ERROR_NONE)
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
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get the register value (available only for LPS22HH, STTS751 sensors)
 * @param  Instance the device instance
 * @param  Reg address to be read
 * @param  Data pointer where the value is written to
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Read_Register(uint32_t Instance, uint8_t Reg, uint8_t *Data)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      if (LPS22HH_Read_Reg(EnvCompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      if (STTS751_Read_Reg(EnvCompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the register value (available only for LPS22HH, STTS751 sensors)
 * @param  Instance the device instance
 * @param  Reg address to be read
 * @param  Data value to be written
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Write_Register(uint32_t Instance, uint8_t Reg, uint8_t Data)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      if (LPS22HH_Write_Reg(EnvCompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      if (STTS751_Write_Reg(EnvCompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the high temperature threshold value (available only STTS751 sensors)
 * @param  Instance the device instance
 * @param  Value the high temperature threshold to be set
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Set_High_Temperature_Threshold(uint32_t Instance, float Value)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      if (STTS751_TEMP_SetHighTemperatureThreshold(EnvCompObj[Instance], Value) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the low temperature threshold value (available only STTS751 sensors)
 * @param  Instance the device instance
 * @param  Value the low temperature threshold to be set
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Set_Low_Temperature_Threshold(uint32_t Instance, float Value)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      if (STTS751_TEMP_SetLowTemperatureThreshold(EnvCompObj[Instance], Value) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get temperature limit status (available only STTS751 sensors)
 * @param  Instance the device instance
 * @param  HighLimit indicates that high temperature limit has been exceeded
 * @param  LowLimit indicates that low temperature limit has been exceeded
 * @param  ThermLimit indicates that therm temperature limit has been exceeded
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Get_Temperature_Limit_Status(uint32_t Instance, uint8_t *HighLimit, uint8_t *LowLimit, uint8_t *ThermLimit)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      if (STTS751_TEMP_GetTemperatureLimitStatus(EnvCompObj[Instance], HighLimit, LowLimit, ThermLimit) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Enable or disable interrupt on EVENT pin (available only STTS751 sensors)
 * @param  Instance the device instance
 * @param  Enable 0 disable the EVENT pin, 1 enable EVENT pin
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Set_Event_Pin(uint32_t Instance, uint8_t Enable)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      if (STTS751_TEMP_SetEventPin(EnvCompObj[Instance], Enable) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get the data stored in FIFO (available only for LPS22HH sensor)
 * @param  Instance the device instance
 * @param  Press the pressure data
 * @param  Temp the temperature data
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_FIFO_Get_Data(uint32_t Instance, float *Press, float *Temp)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      if (LPS22HH_FIFO_Get_Data(EnvCompObj[Instance], Press, Temp) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get FIFO THR status (available only for LPS22HH sensor)
 * @param  Instance the device instance
 * @param  Status the pointer to the status
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_FIFO_Get_Fth_Status(uint32_t Instance, uint8_t *Status)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      if (LPS22HH_FIFO_Get_FTh_Status(EnvCompObj[Instance], Status) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get FIFO Full status (available only for LPS22HH sensor)
 * @param  Instance the device instance
 * @param  Status the pointer to the status
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_FIFO_Get_Full_Status(uint32_t Instance, uint8_t *Status)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      if (LPS22HH_FIFO_Get_Full_Status(EnvCompObj[Instance], Status) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get the number of unread samples in FIFO (available only for LPS22HH sensor)
 * @param  Instance the device instance
 * @param  NumSamples the number of unread FIFO samples
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_FIFO_Get_Num_Samples(uint32_t Instance, uint8_t *NumSamples)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      if (LPS22HH_FIFO_Get_Level(EnvCompObj[Instance], NumSamples) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get FIFO OVR status (available only for LPS22HH sensor)
 * @param  Instance the device instance
 * @param  Status the pointer to the status
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_FIFO_Get_Ovr_Status(uint32_t Instance, uint8_t *Status)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      if (LPS22HH_FIFO_Get_Ovr_Status(EnvCompObj[Instance], Status) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Reset FIFO Interrupt (available only for LPS22HH sensor)
 * @param  Instance the device instance
 * @param  Interrupt FIFO interrupt. Could be: FTH, FULL or OVR
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_FIFO_Reset_Interrupt(uint32_t Instance, uint8_t Interrupt)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      if (LPS22HH_FIFO_Reset_Interrupt(EnvCompObj[Instance], Interrupt) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set FIFO Interrupt (available only for LPS22HH sensor)
 * @param  Instance the device instance
 * @param  Interrupt FIFO interrupt. Could be: FTH, FULL or OVR
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_FIFO_Set_Interrupt(uint32_t Instance, uint8_t Interrupt)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      if (LPS22HH_FIFO_Set_Interrupt(EnvCompObj[Instance], Interrupt) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set FIFO mode  (available only for LPS22HH sensor)
 * @param  Instance the device instance
 * @param  Mode FIFO mode
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_FIFO_Set_Mode(uint32_t Instance, uint8_t Mode)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      if (LPS22HH_FIFO_Set_Mode(EnvCompObj[Instance], Mode) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set FIFO watermark  (available only for LPS22HH sensor)
 * @param  Instance the device instance
 * @param  Watermark FIFO data level threshold
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_FIFO_Set_Watermark_Level(uint32_t Instance, uint8_t Watermark)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      if (LPS22HH_FIFO_Set_Watermark_Level(EnvCompObj[Instance], Watermark) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the stop on watermark function (available only for LPS22HH sensor)
 * @param  Instance the device instance
 * @param  Stop the state of stop on watermark function
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_FIFO_Stop_On_Watermark(uint32_t Instance, uint8_t Stop)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22HH_0 == 1)
    case LPS22HH_0:
      if (LPS22HH_FIFO_Stop_On_Watermark(EnvCompObj[Instance], Stop) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_ENV_SENSOR_STTS751_0 == 1)
    case STTS751_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set environmental sensor one shot mode
 * @param  Instance environmental sensor instance to be used
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Set_One_Shot(uint32_t Instance)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_STTS22H_0 == 1)
    case STTS22H_0:
      if(STTS22H_Set_One_Shot(EnvCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif  

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get environmental sensor one shot status
 * @param  Instance environmental sensor instance to be used
 * @param  Status pointer to the one shot status (1 means measurements available, 0 means measurements not available yet)
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Get_One_Shot_Status(uint32_t Instance, uint8_t *Status)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_STTS22H_0 == 1)
    case STTS22H_0:
      if(STTS22H_Get_One_Shot_Status(EnvCompObj[Instance], Status) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif
    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
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
