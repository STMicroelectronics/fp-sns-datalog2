/**
 ******************************************************************************
 * @file    STWIN.box_motion_sensors_ex.h
 * @author  SRA
 * @brief   This file contains definitions for STWIN.box_motion_sensors_ex.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STWIN_BOX_MOTION_SENSORS_EX_H__
#define STWIN_BOX_MOTION_SENSORS_EX_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STWIN.box_motion_sensors.h"

typedef enum
{
  MOTION_SENSOR_INT1_PIN = 0,
  MOTION_SENSOR_INT2_PIN
} MOTION_SENSOR_IntPin_t;

typedef struct
{
  unsigned int FreeFallStatus :1;
  unsigned int TapStatus :1;
  unsigned int DoubleTapStatus :1;
  unsigned int WakeUpStatus :1;
  unsigned int StepStatus :1;
  unsigned int TiltStatus :1;
  unsigned int D6DOrientationStatus :1;
  unsigned int SleepStatus :1;
} MOTION_SENSOR_Event_Status_t;

int32_t BSP_MOTION_SENSOR_Enable_HP_Filter(uint32_t Instance, uint8_t CutOff);
int32_t BSP_MOTION_SENSOR_Set_INT1_DRDY(uint32_t Instance, uint8_t Status);
int32_t BSP_MOTION_SENSOR_Set_INT2_DRDY(uint32_t Instance, uint8_t Status);
int32_t BSP_MOTION_SENSOR_DRDY_Set_Mode(uint32_t Instance, uint8_t Status);
int32_t BSP_MOTION_SENSOR_Enable_Wake_Up_Detection(uint32_t Instance, MOTION_SENSOR_IntPin_t IntPin);
int32_t BSP_MOTION_SENSOR_Disable_Wake_Up_Detection(uint32_t Instance);
int32_t BSP_MOTION_SENSOR_Set_Wake_Up_Threshold(uint32_t Instance, uint8_t Threshold);
int32_t BSP_MOTION_SENSOR_Set_Wake_Up_Duration(uint32_t Instance, uint8_t Duration);
int32_t BSP_MOTION_SENSOR_Enable_Inactivity_Detection(uint32_t Instance, MOTION_SENSOR_IntPin_t IntPin, uint8_t inact_mode);
int32_t BSP_MOTION_SENSOR_Disable_Inactivity_Detection(uint32_t Instance);
int32_t BSP_MOTION_SENSOR_Set_Sleep_Duration(uint32_t Instance, uint8_t Duration);
int32_t BSP_MOTION_SENSOR_Enable_6D_Orientation(uint32_t Instance, MOTION_SENSOR_IntPin_t IntPin);
int32_t BSP_MOTION_SENSOR_Disable_6D_Orientation(uint32_t Instance);
int32_t BSP_MOTION_SENSOR_Set_6D_Orientation_Threshold(uint32_t Instance, uint8_t Threshold);
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_XL(uint32_t Instance, uint8_t *xl);
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_XH(uint32_t Instance, uint8_t *xh);
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_YL(uint32_t Instance, uint8_t *yl);
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_YH(uint32_t Instance, uint8_t *yh);
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_ZL(uint32_t Instance, uint8_t *zl);
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_ZH(uint32_t Instance, uint8_t *zh);
int32_t BSP_MOTION_SENSOR_Get_Event_Status(uint32_t Instance, MOTION_SENSOR_Event_Status_t *Status);
int32_t BSP_MOTION_SENSOR_Get_DRDY_Status(uint32_t Instance, uint32_t Function, uint8_t *Status);
int32_t BSP_MOTION_SENSOR_Read_Register(uint32_t Instance, uint8_t Reg, uint8_t *Data);
int32_t BSP_MOTION_SENSOR_Write_Register(uint32_t Instance, uint8_t Reg, uint8_t Data);
int32_t BSP_MOTION_SENSOR_FIFO_Set_Mode(uint32_t Instance, uint8_t Mode);
int32_t BSP_MOTION_SENSOR_FIFO_Get_Full_Status(uint32_t Instance, uint16_t *Diff, uint8_t *Status);
int32_t BSP_MOTION_SENSOR_FIFO_Set_BDR(uint32_t Instance, uint32_t Function, float Bdr);
int32_t BSP_MOTION_SENSOR_FIFO_Set_INT1_FIFO_Full(uint32_t Instance, uint8_t Status);
int32_t BSP_MOTION_SENSOR_FIFO_Set_INT2_FIFO_Full(uint32_t Instance, uint8_t Status);
int32_t BSP_MOTION_SENSOR_FIFO_Set_Watermark_Level(uint32_t Instance, uint16_t Watermark);
int32_t BSP_MOTION_SENSOR_FIFO_Set_Stop_On_Fth(uint32_t Instance, uint8_t Status);
int32_t BSP_MOTION_SENSOR_FIFO_Get_Data_Word(uint32_t Instance, uint32_t Function, int16_t *Data);
int32_t BSP_MOTION_SENSOR_FIFO_Get_Num_Samples(uint32_t Instance, uint16_t *NumSamples);

#if (USE_MOTION_SENSOR_IIS3DWB_0 == 1)
int32_t BSP_MOTION_SENSOR_Clear_DRDY(uint32_t Instance);
int32_t BSP_MOTION_SENSOR_FIFO_TS_Decimation(uint32_t Instance, uint32_t Function, uint8_t Decimation);
int32_t BSP_MOTION_SENSOR_FIFO_Set_INT1_FIFO_Threshold(uint32_t Instance, uint8_t Status);
int32_t BSP_MOTION_SENSOR_FIFO_Set_INT2_FIFO_Threshold(uint32_t Instance, uint8_t Status);
int32_t BSP_MOTION_SENSOR_FIFO_T_BDR(uint32_t Instance, uint32_t Function, uint8_t bdr);
int32_t BSP_MOTION_SENSOR_FIFO_Read(uint32_t Instance, uint8_t *pBuff, uint16_t Watermark);
#endif

#ifdef __cplusplus
}
#endif

#endif /* STWIN_BOX_MOTION_SENSORS_EX_H__ */
