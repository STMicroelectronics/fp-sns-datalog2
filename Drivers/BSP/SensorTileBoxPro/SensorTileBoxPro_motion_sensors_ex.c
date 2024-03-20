/**
  ******************************************************************************
  * @file    SensorTileBoxPro_motion_sensors_ex.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.1.0
  * @date    20-July-2023
  * @brief   This file provides BSP Motion Sensors Extended interface
  *          for SensorTileBoxPro
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
#include "SensorTileBoxPro_motion_sensors_ex.h"

extern void *MotionCompObj[BSP_MOTION_INSTANCES_NBR];

/**
 * @brief  Get the register value
 * @param  Instance the device instance
 * @param  Reg address to be read
 * @param  Data pointer where the value is written to
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Read_Register(uint32_t Instance, uint8_t Reg, uint8_t *Data)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      if (LIS2MDL_Read_Reg(MotionCompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_MOTION_SENSOR_LIS2DU12_0 == 1)
    case LIS2DU12_0:
      if (LIS2DU12_Read_Reg(MotionCompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_Read_Reg(MotionCompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
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
 * @brief  Set the register value
 * @param  Instance the device instance
 * @param  Reg address to be read
 * @param  Data value to be written
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Write_Register(uint32_t Instance, uint8_t Reg, uint8_t Data)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      if (LIS2MDL_Write_Reg(MotionCompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_MOTION_SENSOR_LIS2DU12_0 == 1)
    case LIS2DU12_0:
      if (LIS2DU12_Write_Reg(MotionCompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_Write_Reg(MotionCompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
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
 * @brief  Get the status of data ready bit
 * @param  Instance the device instance
 * @param  Function Motion sensor function
 * @param  Status the pointer to the status
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_DRDY_Status(uint32_t Instance, uint32_t Function, uint8_t *Status)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      if ((Function & MOTION_MAGNETO) == MOTION_MAGNETO)
      {
        if (LIS2MDL_MAG_Get_DRDY_Status(MotionCompObj[Instance], Status) != BSP_ERROR_NONE)
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
 * @brief  Enable the free fall detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_Free_Fall_Detection(uint32_t Instance, BSP_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Enable_Free_Fall_Detection(MotionCompObj[Instance], (LSM6DSV16X_SensorIntPin_t)IntPin) != BSP_ERROR_NONE)
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
 * @brief  Disable the free fall detection
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_Free_Fall_Detection(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Disable_Free_Fall_Detection(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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
 * @brief  Set the free fall detection threshold
 * @param  Instance the device instance
 * @param  Threshold the threshold to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Free_Fall_Threshold(uint32_t Instance, uint8_t Threshold)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Set_Free_Fall_Threshold(MotionCompObj[Instance], Threshold) != BSP_ERROR_NONE)
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
 * @brief  Set the free fall detection duration
 * @param  Instance the device instance
 * @param  Duration the duration to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Free_Fall_Duration(uint32_t Instance, uint8_t Duration)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Set_Free_Fall_Duration(MotionCompObj[Instance], Duration) != BSP_ERROR_NONE)
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
 * @brief  Enable the pedometer detection
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_Pedometer(uint32_t Instance, BSP_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Enable_Pedometer(MotionCompObj[Instance],(LSM6DSV16X_SensorIntPin_t)IntPin) != BSP_ERROR_NONE)
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
 * @brief  Disable the pedometer detection
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_Pedometer(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Disable_Pedometer(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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
 * @brief  Set the pedometer threshold
 * @param  Instance the device instance
 * @param  Threshold the threshold to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Pedometer_Threshold(uint32_t Instance, uint8_t Threshold)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
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
 * @brief  Reset step counter
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Reset_Step_Counter(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Step_Counter_Reset(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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
 * @brief  Get step count
 * @param  Instance the device instance
 * @param  StepCount number of steps
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_Step_Count(uint32_t Instance, uint16_t *StepCount)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Get_Step_Count(MotionCompObj[Instance],StepCount) != BSP_ERROR_NONE)
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
 * @brief  Enable the single tap detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_Single_Tap_Detection(uint32_t Instance, BSP_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Enable_Single_Tap_Detection(MotionCompObj[Instance],(LSM6DSV16X_SensorIntPin_t)IntPin) != BSP_ERROR_NONE)
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
 * @brief  Disable the single tap detection
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_Single_Tap_Detection(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Disable_Single_Tap_Detection(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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
 * @brief  Enable the double tap detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_Double_Tap_Detection(uint32_t Instance, BSP_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Enable_Double_Tap_Detection(MotionCompObj[Instance],(LSM6DSV16X_SensorIntPin_t)IntPin) != BSP_ERROR_NONE)
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
 * @brief  Disable the double tap detection
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_Double_Tap_Detection(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Disable_Double_Tap_Detection(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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
 * @brief  Set the tap threshold
 * @param  Instance the device instance
 * @param  Threshold the threshold to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Tap_Threshold(uint32_t Instance, uint8_t Threshold)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Set_Tap_Threshold(MotionCompObj[Instance],Threshold) != BSP_ERROR_NONE)
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
 * @brief  Set the tap shock time
 * @param  Instance the device instance
 * @param  Time the tap shock time to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Tap_Shock_Time(uint32_t Instance, uint8_t Time)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Set_Tap_Shock_Time(MotionCompObj[Instance],Time) != BSP_ERROR_NONE)
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
 * @brief  Set the tap quiet time
 * @param  Instance the device instance
 * @param  Time the tap quiet time to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Tap_Quiet_Time(uint32_t Instance, uint8_t Time)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Set_Tap_Quiet_Time(MotionCompObj[Instance],Time) != BSP_ERROR_NONE)
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
 * @brief  Set the tap duration time
 * @param  Instance the device instance
 * @param  Time the tap duration time to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Tap_Duration_Time(uint32_t Instance, uint8_t Time)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Set_Tap_Duration_Time(MotionCompObj[Instance],Time) != BSP_ERROR_NONE)
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
 * @brief  Enable the tilt detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_Tilt_Detection(uint32_t Instance, BSP_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Enable_Tilt_Detection(MotionCompObj[Instance],(LSM6DSV16X_SensorIntPin_t)IntPin) != BSP_ERROR_NONE)
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
 * @brief  Disable the tilt detection
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_Tilt_Detection(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Disable_Tilt_Detection(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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
 * @brief  Enable the wake up detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_Wake_Up_Detection(uint32_t Instance, BSP_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Enable_Wake_Up_Detection(MotionCompObj[Instance],(LSM6DSV16X_SensorIntPin_t)IntPin) != BSP_ERROR_NONE)
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
 * @brief  Disable the wake up detection
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_Wake_Up_Detection(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Disable_Wake_Up_Detection(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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
 * @brief  Set the wake up detection threshold
 * @param  Instance the device instance
 * @param  Threshold the threshold to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Wake_Up_Threshold(uint32_t Instance, uint32_t Threshold)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Set_Wake_Up_Threshold(MotionCompObj[Instance], (uint32_t)Threshold) != BSP_ERROR_NONE)
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
 * @brief  Set the wake up detection duration
 * @param  Instance the device instance
 * @param  Duration the duration to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Wake_Up_Duration(uint32_t Instance, uint8_t Duration)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Set_Wake_Up_Duration(MotionCompObj[Instance], (uint8_t)Duration) != BSP_ERROR_NONE)
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
 * @brief  Enable the inactivity detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_Inactivity_Detection(uint32_t Instance, BSP_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
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
 * @brief  Disable the inactivity detection
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_Inactivity_Detection(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
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
 * @brief  Set the sleep duration
 * @param  Instance the device instance
 * @param  Duration the duration to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Sleep_Duration(uint32_t Instance, uint8_t Duration)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
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
 * @brief  Enable 6D Orientation
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_6D_Orientation(uint32_t Instance, BSP_MOTION_SENSOR_IntPin_t IntPin)
{
  int32_t ret;

  switch (Instance)
  {
#if (USE_MOTION_SENSOR_LIS2MDL_0_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Enable_6D_Orientation(MotionCompObj[Instance], (LSM6DSV16X_SensorIntPin_t)IntPin) != BSP_ERROR_NONE)
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
 * @brief  Disable 6D Orientation
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_6D_Orientation(uint32_t Instance)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Disable_6D_Orientation(MotionCompObj[Instance] ) != BSP_ERROR_NONE)
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
 * @brief  Set the 6D orientation threshold
 * @param  Instance the device instance
 * @param  Threshold the threshold to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_6D_Orientation_Threshold(uint32_t Instance, uint8_t Threshold)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Set_6D_Orientation_Threshold(MotionCompObj[Instance], Threshold) != BSP_ERROR_NONE)
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
 * @brief  Get 6D Orientation XL
 * @param  Instance the device instance
 * @param  xl the pointer to the 6D orientation XL axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_XL(uint32_t Instance, uint8_t *xl)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Get_6D_Orientation_XL(MotionCompObj[Instance], xl) != BSP_ERROR_NONE)
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
 * @brief  Get 6D Orientation XH
 * @param  Instance the device instance
 * @param  xh the pointer to the 6D orientation XH axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_XH(uint32_t Instance, uint8_t *xh)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Get_6D_Orientation_XH(MotionCompObj[Instance], xh) != BSP_ERROR_NONE)
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
 * @brief  Get 6D Orientation YL
 * @param  Instance the device instance
 * @param  yl the pointer to the 6D orientation YL axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_YL(uint32_t Instance, uint8_t *yl)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Get_6D_Orientation_YL(MotionCompObj[Instance], yl) != BSP_ERROR_NONE)
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
 * @brief  Get 6D Orientation YH
 * @param  Instance the device instance
 * @param  yh the pointer to the 6D orientation YH axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_YH(uint32_t Instance, uint8_t *yh)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Get_6D_Orientation_YH(MotionCompObj[Instance], yh) != BSP_ERROR_NONE)
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
 * @brief  Get 6D Orientation ZL
 * @param  Instance the device instance
 * @param  zl the pointer to the 6D orientation ZL axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_ZL(uint32_t Instance, uint8_t *zl)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Get_6D_Orientation_ZL(MotionCompObj[Instance], zl) != BSP_ERROR_NONE)
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
 * @brief  Get 6D Orientation ZH
 * @param  Instance the device instance
 * @param  zh the pointer to the 6D orientation ZH axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_ZH(uint32_t Instance, uint8_t *zh)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Get_6D_Orientation_ZH(MotionCompObj[Instance], zh) != BSP_ERROR_NONE)
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
 * @brief  Get the status of all hardware events
 * @param  Instance the device instance
 * @param  Status the pointer to the status of all hardware events
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_Event_Status(uint32_t Instance, BSP_MOTION_SENSOR_Event_Status_t *Status)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_ACC_Get_Event_Status(MotionCompObj[Instance], (LSM6DSV16X_Event_Status_t *)(void *) Status) != BSP_ERROR_NONE)
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
 * @brief  Get number of unread FIFO samples
 * @param  Instance the device instance
 * @param  NumSamples number of unread FIFO samples
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Get_Num_Samples(uint32_t Instance, uint16_t *NumSamples)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_FIFO_Get_Num_Samples(MotionCompObj[Instance], NumSamples) != BSP_ERROR_NONE)
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
 * @brief  Get FIFO full status
 * @param  Instance the device instance
 * @param  Status FIFO full status
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Get_Full_Status(uint32_t Instance, uint8_t *Status)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_FIFO_Get_Full_Status(MotionCompObj[Instance], Status) != BSP_ERROR_NONE)
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
 * @brief  Set FIFO decimation
 * @param  Instance the device instance
 * @param  Function Motion sensor function
 * @param  Decimation FIFO decimation
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Set_Decimation(uint32_t Instance, uint32_t Function, uint8_t Decimation)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
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
 * @brief  Set FIFO ODR value
 * @param  Instance the device instance
 * @param  Odr FIFO ODR value
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Set_ODR_Value(uint32_t Instance, float Odr)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
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
 * @brief  Set FIFO full interrupt on INT1 pin
 * @param  Instance the device instance
 * @param  Status FIFO full interrupt on INT1 pin
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Set_INT1_FIFO_Full(uint32_t Instance, uint8_t Status)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_FIFO_Set_INT1_FIFO_Full(MotionCompObj[Instance], Status) != BSP_ERROR_NONE)
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
 * @brief  Set FIFO full interrupt on INT2 pin
 * @param  Instance the device instance
 * @param  Status FIFO full interrupt on INT2 pin
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Set_INT2_FIFO_Full(uint32_t Instance, uint8_t Status)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_FIFO_Set_INT2_FIFO_Full(MotionCompObj[Instance], Status) != BSP_ERROR_NONE)
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
 * @brief  Set FIFO watermark level
 * @param  Instance the device instance
 * @param  Watermark FIFO watermark level
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Set_Watermark_Level(uint32_t Instance, uint8_t Watermark)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_FIFO_Set_Watermark_Level(MotionCompObj[Instance], Watermark) != BSP_ERROR_NONE)
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
 * @brief  Set FIFO stop on watermark
 * @param  Instance the device instance
 * @param  Status FIFO stop on watermark status
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Set_Stop_On_Fth(uint32_t Instance, uint8_t Status)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_FIFO_Set_Stop_On_Fth(MotionCompObj[Instance], Status) != BSP_ERROR_NONE)
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
 * @brief  Set FIFO mode
 * @param  Instance the device instance
 * @param  Mode FIFO mode
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Set_Mode(uint32_t Instance, uint8_t Mode)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_FIFO_Set_Mode(MotionCompObj[Instance], Mode) != BSP_ERROR_NONE)
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
 * @brief  Get FIFO pattern
 * @param  Instance the device instance
 * @param  Pattern FIFO pattern
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Get_Pattern(uint32_t Instance, uint16_t *Pattern)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
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
 * @brief  Get FIFO single axis data
 * @param  Instance the device instance
 * @param  Function Motion sensor function
 * @param  Data FIFO single axis data
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Get_Axis(uint32_t Instance, uint32_t Function, int32_t *Data)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
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
 * @brief  Set FIFO BDR value
 * @param  Instance the device instance
 * @param  Function Motion sensor function
 * @param  Odr FIFO BDR value
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Set_BDR(uint32_t Instance, uint32_t Function, float Bdr)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if ((Function & MOTION_ACCELERO) == MOTION_ACCELERO)
      {
        if (LSM6DSV16X_FIFO_ACC_Set_BDR(MotionCompObj[Instance], Bdr) != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          ret = BSP_ERROR_NONE;
        }
      }
      else if ((Function & MOTION_GYRO) == MOTION_GYRO)
      {
        if (LSM6DSV16X_FIFO_GYRO_Set_BDR(MotionCompObj[Instance], Bdr) != BSP_ERROR_NONE)
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
      break;
#endif
    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get FIFO tag
 * @param  Instance the device instance
 * @param  Tag FIFO tag
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Get_Tag(uint32_t Instance, uint8_t *Tag)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
      if (LSM6DSV16X_FIFO_Get_Tag(MotionCompObj[Instance], Tag) != BSP_ERROR_NONE)
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
 * @brief  Get FIFO axes data
 * @param  Instance the device instance
 * @param  Function Motion sensor function
 * @param  Data FIFO axes data
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_FIFO_Get_Axes(uint32_t Instance, uint32_t Function, BSP_MOTION_SENSOR_Axes_t *Data)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif
#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
    case LSM6DSV16X_0:
    if ((Function & MOTION_ACCELERO) == MOTION_ACCELERO)
      {
        if (LSM6DSV16X_FIFO_ACC_Get_Axes(MotionCompObj[Instance], (LSM6DSV16X_Axes_t *)Data) != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          ret = BSP_ERROR_NONE;
        }
      }
      else if ((Function & MOTION_GYRO) == MOTION_GYRO)
      {
        if (LSM6DSV16X_FIFO_GYRO_Get_Axes(MotionCompObj[Instance], (LSM6DSV16X_Axes_t *)Data) != BSP_ERROR_NONE)
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
      break;
#endif
    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set accelero self-test
 * @param  Instance the device instance
 * @param  Function Motion sensor function
 * @param  Data FIFO single axis data
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_SelfTest(uint32_t Instance, uint32_t Function, uint8_t Status)
{
  int32_t ret;

  switch (Instance)
  {

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
    case LIS2MDL_0:
      if ((Function & MOTION_MAGNETO) == MOTION_MAGNETO)
      {
        if (LIS2MDL_MAG_Set_SelfTest(MotionCompObj[Instance], Status) != BSP_ERROR_NONE)
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
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}


/**
 * @brief Enable the interrupt DRDY mode
 * @param Instance the device instance
 * @retval BSP_ERROR_NONE in case of success
 * @retval BSP_ERROR_COMPONENT_FAILURE in case of failure
 */
int32_t BSP_MOTION_SENSOR_Enable_DRDY_Interrupt(uint32_t Instance, uint8_t sensorType)
{
  int32_t ret= BSP_ERROR_NONE;

  switch (Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DU12_0 == 1)
  case LIS2DU12_0:
    if(LIS2DU12_ACC_Enable_DRDY_Interrupt(MotionCompObj[Instance]) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
    break;
#endif

#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
  case LSM6DSV16X_0:
    if(sensorType == 0U)
    {
//      if(LSM6DSV16X_ACC_Enable_DRDY_Interrupt(MotionCompObj[Instance]) != BSP_ERROR_NONE)
//      {
//        ret = BSP_ERROR_COMPONENT_FAILURE;
//      }
//      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
//      if(LSM6DSV16X_GYRO_Enable_DRDY_Interrupt(MotionCompObj[Instance]) != BSP_ERROR_NONE)
//      {
//        ret = BSP_ERROR_COMPONENT_FAILURE;
//      }
//      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    break;
#endif

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
  case LIS2MDL_0:
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
    break;
#endif

  default:
    ret = BSP_ERROR_WRONG_PARAM;
    break;
  }

  return ret;
}

/**
 * @brief Disable the interrupt DRDY mode
 * @param Instance the device instance
 * @retval BSP_ERROR_NONE in case of success
 * @retval BSP_ERROR_COMPONENT_FAILURE in case of failure
 */
int32_t BSP_MOTION_SENSOR_Disable_DRDY_Interrupt(uint32_t Instance, uint8_t sensorType)
{
  int32_t ret;

  switch (Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DU12_0 == 1)
  case LIS2DU12_0:
    if(LIS2DU12_ACC_Disable_DRDY_Interrupt(MotionCompObj[Instance]) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
    break;
#endif

#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
  case LSM6DSV16X_0:
    if(sensorType == 0U)
    {
//      if(LSM6DSV16X_ACC_Disable_DRDY_Interrupt(MotionCompObj[Instance]) != BSP_ERROR_NONE)
//      {
//        ret = BSP_ERROR_COMPONENT_FAILURE;
//      }
//      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
//      if(LSM6DSV16X_GYRO_Disable_DRDY_Interrupt(MotionCompObj[Instance]) != BSP_ERROR_NONE)
//      {
//        ret = BSP_ERROR_COMPONENT_FAILURE;
//      }
//      else
      {
        ret = BSP_ERROR_NONE;
      }
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
 * @brief Set the value of powerMode
 * @param Instance the device instance
 * @param  Functions Motion sensor functions. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 * @param powerMode: this is the value of the powerMode
 * @retval BSP_ERROR_NONE in case of success
 * @retval BSP_ERROR_COMPONENT_FAILURE in case of failure
 */
int32_t BSP_MOTION_SENSOR_Set_PowerMode(uint32_t Instance, uint32_t Functions, uint8_t powerMode)
{
  int32_t ret = BSP_ERROR_NONE;

  switch (Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DU12_0 == 1)
  case LIS2DU12_0:
      ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
    break;
#endif

#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
  case LSM6DSV16X_0:
    if(Functions & MOTION_ACCELERO) {
      if(LSM6DSV16X_ACC_Set_Power_Mode(MotionCompObj[Instance], powerMode) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if(Functions & MOTION_GYRO) {
      if(LSM6DSV16X_GYRO_Set_Power_Mode(MotionCompObj[Instance], powerMode) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    break;
#endif

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
  case LIS2MDL_0:
    if(LIS2MDL_MAG_Set_Power_Mode(MotionCompObj[Instance], powerMode) != BSP_ERROR_NONE)
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
 * @brief Set the value of filterMode
 * @param Instance the device instance
 * @param  Functions Motion sensor functions. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 * @param LowHighPassFlag  0/1 for setting low-pass/high-pass filter mode
 * @param filterMode: this is the value of the filterMode
 * @retval BSP_ERROR_NONE in case of success
 * @retval BSP_ERROR_COMPONENT_FAILURE in case of failure
 */
int32_t BSP_MOTION_SENSOR_Set_FilterMode(uint32_t Instance, uint32_t Functions, uint8_t LowHighPassFlag, uint8_t filterMode)
{
  int32_t ret=BSP_ERROR_NONE;

  switch (Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DU12_0 == 1)
  case LIS2DU12_0:
    if(LIS2DU12_ACC_Set_Filter_Mode(MotionCompObj[Instance], filterMode) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
    break;
#endif

#if (USE_MOTION_SENSOR_LSM6DSV16X_0 == 1)
  case LSM6DSV16X_0:
    if(Functions & MOTION_ACCELERO)
    {
      if(LSM6DSV16X_ACC_Set_Filter_Mode(MotionCompObj[Instance], LowHighPassFlag, filterMode) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if(Functions & MOTION_GYRO) {
      if(LSM6DSV16X_GYRO_Set_Filter_Mode(MotionCompObj[Instance], LowHighPassFlag, filterMode) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    break;
#endif

#if (USE_MOTION_SENSOR_LIS2MDL_0 == 1)
  case LIS2MDL_0:
    if(LIS2MDL_MAG_Set_Filter_Mode(MotionCompObj[Instance], filterMode) != BSP_ERROR_NONE)
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
