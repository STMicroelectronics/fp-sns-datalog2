/**
  ******************************************************************************
  * @file    lsm6dsk320x.h
  * @author  MEMS Software Solutions Team
  * @brief   LSM6DSK320X header driver file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LSM6DSK320X_H
#define LSM6DSK320X_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "lsm6dsk320x_reg.h"
#include <string.h>

/** @addtogroup BSP BSP
  * @{
  */

/** @addtogroup Component Component
  * @{
  */

/** @addtogroup LSM6DSK320X LSM6DSK320X
  * @{
  */

/** @defgroup LSM6DSK320X_Exported_Types LSM6DSK320X Exported Types
  * @{
  */

typedef int32_t (*LSM6DSK320X_Init_Func)(void);
typedef int32_t (*LSM6DSK320X_DeInit_Func)(void);
typedef int32_t (*LSM6DSK320X_GetTick_Func)(void);
typedef void    (*LSM6DSK320X_Delay_Func)(uint32_t);
typedef int32_t (*LSM6DSK320X_WriteReg_Func)(uint16_t, uint16_t, uint8_t *, uint16_t);
typedef int32_t (*LSM6DSK320X_ReadReg_Func)(uint16_t, uint16_t, uint8_t *, uint16_t);

typedef enum
{
  LSM6DSK320X_INT1_PIN,
  LSM6DSK320X_INT2_PIN,
} LSM6DSK320X_SensorIntPin_t;

typedef enum
{
  LSM6DSK320X_ACC_HIGH_PERFORMANCE_MODE,
  LSM6DSK320X_ACC_HIGH_ACCURACY_ODR_MODE,
  LSM6DSK320X_ACC_ODR_TRIGGERED_MODE,
  LSM6DSK320X_ACC_LOW_POWER_MODE1,
  LSM6DSK320X_ACC_LOW_POWER_MODE2,
  LSM6DSK320X_ACC_LOW_POWER_MODE3,
  LSM6DSK320X_ACC_NORMAL_MODE,
} LSM6DSK320X_ACC_Operating_Mode_t;

typedef enum
{
  LSM6DSK320X_GYRO_HIGH_PERFORMANCE_MODE,
  LSM6DSK320X_GYRO_HIGH_ACCURACY_ODR_MODE,
  LSM6DSK320X_GYRO_ODR_TRIGGERED_MODE,
  LSM6DSK320X_GYRO_SLEEP_MODE,
  LSM6DSK320X_GYRO_LOW_POWER_MODE,
} LSM6DSK320X_GYRO_Operating_Mode_t;

typedef struct
{
  LSM6DSK320X_Init_Func     Init;
  LSM6DSK320X_DeInit_Func   DeInit;
  uint32_t                  BusType; /*0 means I2C, 1 means SPI 4-Wires, 2 means SPI-3-Wires, 3 means I3C */
  uint8_t                   Address;
  LSM6DSK320X_WriteReg_Func WriteReg;
  LSM6DSK320X_ReadReg_Func  ReadReg;
  LSM6DSK320X_GetTick_Func  GetTick;
  LSM6DSK320X_Delay_Func    Delay;
} LSM6DSK320X_IO_t;

typedef struct
{
  int16_t x;
  int16_t y;
  int16_t z;
} LSM6DSK320X_AxesRaw_t;

typedef struct
{
  int32_t x;
  int32_t y;
  int32_t z;
} LSM6DSK320X_Axes_t;

typedef struct
{
  unsigned int FreeFallStatus : 1;
  unsigned int TapStatus : 1;
  unsigned int DoubleTapStatus : 1;
  unsigned int WakeUpStatus : 1;
  unsigned int StepStatus : 1;
  unsigned int TiltStatus : 1;
  unsigned int D6DOrientationStatus : 1;
  unsigned int SleepStatus : 1;
} LSM6DSK320X_Event_Status_t;

typedef struct
{
  LSM6DSK320X_IO_t              IO;
  stmdev_ctx_t                  Ctx;
  uint8_t                       is_initialized;
  uint8_t                       acc_is_enabled;
  uint8_t                       acc_hg_is_enabled;
  uint8_t                       gyro_is_enabled;
  lsm6dsk320x_data_rate_t       acc_odr;
  lsm6dsk320x_hg_xl_data_rate_t acc_hg_odr;
  lsm6dsk320x_data_rate_t       gyro_odr;
} LSM6DSK320X_Object_t;

typedef struct
{
  uint8_t  Acc;
  uint8_t  Gyro;
  uint8_t  Magneto;
  uint8_t  LowPower;
  uint32_t GyroMaxFS;
  uint32_t AccMaxFS;
  uint32_t MagMaxFS;
  float_t  GyroMaxOdr;
  float_t  AccMaxOdr;
  float_t  MagMaxOdr;
} LSM6DSK320X_Capabilities_t;

typedef struct
{
  int32_t (*Init)(LSM6DSK320X_Object_t *);
  int32_t (*DeInit)(LSM6DSK320X_Object_t *);
  int32_t (*ReadID)(LSM6DSK320X_Object_t *, uint8_t *);
  int32_t (*GetCapabilities)(LSM6DSK320X_Object_t *, LSM6DSK320X_Capabilities_t *);
} LSM6DSK320X_CommonDrv_t;

typedef struct
{
  int32_t (*Enable)(LSM6DSK320X_Object_t *);
  int32_t (*Disable)(LSM6DSK320X_Object_t *);
  int32_t (*GetSensitivity)(LSM6DSK320X_Object_t *, float_t *);
  int32_t (*GetOutputDataRate)(LSM6DSK320X_Object_t *, float_t *);
  int32_t (*SetOutputDataRate)(LSM6DSK320X_Object_t *, float_t);
  int32_t (*GetFullScale)(LSM6DSK320X_Object_t *, int32_t *);
  int32_t (*SetFullScale)(LSM6DSK320X_Object_t *, int32_t);
  int32_t (*GetAxes)(LSM6DSK320X_Object_t *, LSM6DSK320X_Axes_t *);
  int32_t (*GetAxesRaw)(LSM6DSK320X_Object_t *, LSM6DSK320X_AxesRaw_t *);
} LSM6DSK320X_ACC_Drv_t;

typedef struct
{
  int32_t (*Enable)(LSM6DSK320X_Object_t *);
  int32_t (*Disable)(LSM6DSK320X_Object_t *);
  int32_t (*GetSensitivity)(LSM6DSK320X_Object_t *, float_t *);
  int32_t (*GetOutputDataRate)(LSM6DSK320X_Object_t *, float_t *);
  int32_t (*SetOutputDataRate)(LSM6DSK320X_Object_t *, float_t);
  int32_t (*GetFullScale)(LSM6DSK320X_Object_t *, int32_t *);
  int32_t (*SetFullScale)(LSM6DSK320X_Object_t *, int32_t);
  int32_t (*GetAxes)(LSM6DSK320X_Object_t *, LSM6DSK320X_Axes_t *);
  int32_t (*GetAxesRaw)(LSM6DSK320X_Object_t *, LSM6DSK320X_AxesRaw_t *);
} LSM6DSK320X_GYRO_Drv_t;

typedef union
{
  int16_t i16bit[3];
  uint8_t u8bit[6];
} lsm6dsk320x_axis3bit16_t;

typedef union
{
  int16_t i16bit;
  uint8_t u8bit[2];
} lsm6dsk320x_axis1bit16_t;

typedef union
{
  int32_t i32bit[3];
  uint8_t u8bit[12];
} lsm6dsk320x_axis3bit32_t;

typedef union
{
  int32_t i32bit;
  uint8_t u8bit[4];
} lsm6dsk320x_axis1bit32_t;

/**
  * @}
  */

/** @defgroup LSM6DSK320X_Exported_Constants LSM6DSK320X Exported Constants
  * @{
  */

#define LSM6DSK320X_OK                       0
#define LSM6DSK320X_ERROR                   -1

#define LSM6DSK320X_I2C_BUS                 0U
#define LSM6DSK320X_SPI_4WIRES_BUS          1U
#define LSM6DSK320X_SPI_3WIRES_BUS          2U
#define LSM6DSK320X_I3C_BUS                 3U

#define LSM6DSK320X_ACC_SENSITIVITY_FS_2G    0.061f
#define LSM6DSK320X_ACC_SENSITIVITY_FS_4G    0.122f
#define LSM6DSK320X_ACC_SENSITIVITY_FS_8G    0.244f
#define LSM6DSK320X_ACC_SENSITIVITY_FS_16G   0.488f
#define LSM6DSK320X_ACC_SENSITIVITY_FS_32G   0.976f
#define LSM6DSK320X_ACC_SENSITIVITY_FS_64G   1.952f
#define LSM6DSK320X_ACC_SENSITIVITY_FS_128G  3.904f
#define LSM6DSK320X_ACC_SENSITIVITY_FS_256G  7.808f
#define LSM6DSK320X_ACC_SENSITIVITY_FS_320G 10.417f

#define LSM6DSK320X_GYRO_SENSITIVITY_FS_250DPS     8.750f
#define LSM6DSK320X_GYRO_SENSITIVITY_FS_500DPS    17.500f
#define LSM6DSK320X_GYRO_SENSITIVITY_FS_1000DPS   35.000f
#define LSM6DSK320X_GYRO_SENSITIVITY_FS_2000DPS   70.000f
#define LSM6DSK320X_GYRO_SENSITIVITY_FS_4000DPS  140.000f

/**
  * @}
  */

/** @addtogroup LSM6DSK320X_Exported_Functions LSM6DSK320X Exported Functions
  * @{
  */

int32_t LSM6DSK320X_RegisterBusIO(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_IO_t *pIO);
int32_t LSM6DSK320X_Init(LSM6DSK320X_Object_t *pObj);
int32_t LSM6DSK320X_DeInit(LSM6DSK320X_Object_t *pObj);
int32_t LSM6DSK320X_ReadID(LSM6DSK320X_Object_t *pObj, uint8_t *Id);
int32_t LSM6DSK320X_GetCapabilities(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_Capabilities_t *Capabilities);

int32_t LSM6DSK320X_ACC_Enable(LSM6DSK320X_Object_t *pObj);
int32_t LSM6DSK320X_ACC_Disable(LSM6DSK320X_Object_t *pObj);
int32_t LSM6DSK320X_ACC_GetSensitivity(LSM6DSK320X_Object_t *pObj, float_t *Sensitivity);
int32_t LSM6DSK320X_ACC_GetOutputDataRate(LSM6DSK320X_Object_t *pObj, float_t *Odr);
int32_t LSM6DSK320X_ACC_SetOutputDataRate(LSM6DSK320X_Object_t *pObj, float_t Odr);
int32_t LSM6DSK320X_ACC_SetOutputDataRate_With_Mode(LSM6DSK320X_Object_t *pObj, float_t Odr,
                                                   LSM6DSK320X_ACC_Operating_Mode_t Mode);
int32_t LSM6DSK320X_ACC_GetFullScale(LSM6DSK320X_Object_t *pObj, int32_t *FullScale);
int32_t LSM6DSK320X_ACC_SetFullScale(LSM6DSK320X_Object_t *pObj, int32_t FullScale);
int32_t LSM6DSK320X_ACC_GetAxesRaw(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_AxesRaw_t *Value);
int32_t LSM6DSK320X_ACC_GetAxes(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_Axes_t *Acceleration);

int32_t LSM6DSK320X_ACC_Get_Event_Status(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_Event_Status_t *Status);

int32_t LSM6DSK320X_ACC_Enable_Free_Fall_Detection(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_SensorIntPin_t IntPin);
int32_t LSM6DSK320X_ACC_Disable_Free_Fall_Detection(LSM6DSK320X_Object_t *pObj);
int32_t LSM6DSK320X_ACC_Set_Free_Fall_Threshold(LSM6DSK320X_Object_t *pObj, uint8_t Threshold);
int32_t LSM6DSK320X_ACC_Set_Free_Fall_Duration(LSM6DSK320X_Object_t *pObj, uint8_t Duration);

int32_t LSM6DSK320X_ACC_Enable_Wake_Up_Detection(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_SensorIntPin_t IntPin);
int32_t LSM6DSK320X_ACC_Disable_Wake_Up_Detection(LSM6DSK320X_Object_t *pObj);
int32_t LSM6DSK320X_ACC_Set_Wake_Up_Threshold(LSM6DSK320X_Object_t *pObj, uint32_t Threshold);
int32_t LSM6DSK320X_ACC_Set_Wake_Up_Duration(LSM6DSK320X_Object_t *pObj, uint8_t Duration);

int32_t LSM6DSK320X_ACC_Enable_Single_Tap_Detection(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_SensorIntPin_t IntPin);
int32_t LSM6DSK320X_ACC_Disable_Single_Tap_Detection(LSM6DSK320X_Object_t *pObj);
int32_t LSM6DSK320X_ACC_Enable_Double_Tap_Detection(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_SensorIntPin_t IntPin);
int32_t LSM6DSK320X_ACC_Disable_Double_Tap_Detection(LSM6DSK320X_Object_t *pObj);
int32_t LSM6DSK320X_ACC_Set_Tap_Threshold(LSM6DSK320X_Object_t *pObj, uint8_t Threshold);
int32_t LSM6DSK320X_ACC_Set_Tap_Shock_Time(LSM6DSK320X_Object_t *pObj, uint8_t Time);
int32_t LSM6DSK320X_ACC_Set_Tap_Quiet_Time(LSM6DSK320X_Object_t *pObj, uint8_t Time);
int32_t LSM6DSK320X_ACC_Set_Tap_Duration_Time(LSM6DSK320X_Object_t *pObj, uint8_t Time);

int32_t LSM6DSK320X_ACC_Enable_6D_Orientation(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_SensorIntPin_t IntPin);
int32_t LSM6DSK320X_ACC_Disable_6D_Orientation(LSM6DSK320X_Object_t *pObj);
int32_t LSM6DSK320X_ACC_Set_6D_Orientation_Threshold(LSM6DSK320X_Object_t *pObj, uint8_t Threshold);
int32_t LSM6DSK320X_ACC_Get_6D_Orientation_XL(LSM6DSK320X_Object_t *pObj, uint8_t *XLow);
int32_t LSM6DSK320X_ACC_Get_6D_Orientation_XH(LSM6DSK320X_Object_t *pObj, uint8_t *XHigh);
int32_t LSM6DSK320X_ACC_Get_6D_Orientation_YL(LSM6DSK320X_Object_t *pObj, uint8_t *YLow);
int32_t LSM6DSK320X_ACC_Get_6D_Orientation_YH(LSM6DSK320X_Object_t *pObj, uint8_t *YHigh);
int32_t LSM6DSK320X_ACC_Get_6D_Orientation_ZL(LSM6DSK320X_Object_t *pObj, uint8_t *ZLow);
int32_t LSM6DSK320X_ACC_Get_6D_Orientation_ZH(LSM6DSK320X_Object_t *pObj, uint8_t *ZHigh);

int32_t LSM6DSK320X_ACC_Enable_Tilt_Detection(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_SensorIntPin_t IntPin);
int32_t LSM6DSK320X_ACC_Disable_Tilt_Detection(LSM6DSK320X_Object_t *pObj);

int32_t LSM6DSK320X_ACC_Enable_Pedometer(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_SensorIntPin_t IntPin);
int32_t LSM6DSK320X_ACC_Disable_Pedometer(LSM6DSK320X_Object_t *pObj);
int32_t LSM6DSK320X_ACC_Get_Step_Count(LSM6DSK320X_Object_t *pObj, uint16_t *StepCount);
int32_t LSM6DSK320X_ACC_Step_Counter_Reset(LSM6DSK320X_Object_t *pObj);

int32_t LSM6DSK320X_ACC_HG_Enable(LSM6DSK320X_Object_t *pObj);
int32_t LSM6DSK320X_ACC_HG_Disable(LSM6DSK320X_Object_t *pObj);
int32_t LSM6DSK320X_ACC_HG_GetSensitivity(LSM6DSK320X_Object_t *pObj, float_t *Sensitivity);
int32_t LSM6DSK320X_ACC_HG_GetOutputDataRate(LSM6DSK320X_Object_t *pObj, float_t *Odr);
int32_t LSM6DSK320X_ACC_HG_SetOutputDataRate(LSM6DSK320X_Object_t *pObj, float_t Odr);
int32_t LSM6DSK320X_ACC_HG_GetFullScale(LSM6DSK320X_Object_t *pObj, int32_t *FullScale);
int32_t LSM6DSK320X_ACC_HG_SetFullScale(LSM6DSK320X_Object_t *pObj, int32_t FullScale);
int32_t LSM6DSK320X_ACC_HG_GetAxesRaw(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_AxesRaw_t *Value);
int32_t LSM6DSK320X_ACC_HG_GetAxes(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_Axes_t *Acceleration);

int32_t LSM6DSK320X_FIFO_Get_Num_Samples(LSM6DSK320X_Object_t *pObj, uint16_t *NumSamples);
int32_t LSM6DSK320X_FIFO_Get_Full_Status(LSM6DSK320X_Object_t *pObj, uint8_t *Status);
int32_t LSM6DSK320X_FIFO_Set_INT1_FIFO_Full(LSM6DSK320X_Object_t *pObj, uint8_t Status);
int32_t LSM6DSK320X_FIFO_Set_INT2_FIFO_Full(LSM6DSK320X_Object_t *pObj, uint8_t Status);
int32_t LSM6DSK320X_FIFO_Set_Watermark_Level(LSM6DSK320X_Object_t *pObj, uint8_t Watermark);
int32_t LSM6DSK320X_FIFO_Set_Stop_On_Fth(LSM6DSK320X_Object_t *pObj, uint8_t Status);
int32_t LSM6DSK320X_FIFO_Set_Mode(LSM6DSK320X_Object_t *pObj, uint8_t Mode);
int32_t LSM6DSK320X_FIFO_Get_Tag(LSM6DSK320X_Object_t *pObj, uint8_t *Tag);
int32_t LSM6DSK320X_FIFO_Get_Data(LSM6DSK320X_Object_t *pObj, uint8_t *Data);
int32_t LSM6DSK320X_FIFO_ACC_Get_Axes(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_Axes_t *Acceleration);
int32_t LSM6DSK320X_FIFO_ACC_Set_BDR(LSM6DSK320X_Object_t *pObj, float_t Bdr);
int32_t LSM6DSK320X_FIFO_GYRO_Get_Axes(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_Axes_t *AngularVelocity);
int32_t LSM6DSK320X_FIFO_GYRO_Set_BDR(LSM6DSK320X_Object_t *pObj, float_t Bdr);

int32_t LSM6DSK320X_GYRO_Enable(LSM6DSK320X_Object_t *pObj);
int32_t LSM6DSK320X_GYRO_Disable(LSM6DSK320X_Object_t *pObj);
int32_t LSM6DSK320X_GYRO_GetSensitivity(LSM6DSK320X_Object_t *pObj, float_t *Sensitivity);
int32_t LSM6DSK320X_GYRO_GetOutputDataRate(LSM6DSK320X_Object_t *pObj, float_t *Odr);
int32_t LSM6DSK320X_GYRO_SetOutputDataRate(LSM6DSK320X_Object_t *pObj, float_t Odr);
int32_t LSM6DSK320X_GYRO_SetOutputDataRate_With_Mode(LSM6DSK320X_Object_t *pObj, float_t Odr,
                                                    LSM6DSK320X_GYRO_Operating_Mode_t Mode);
int32_t LSM6DSK320X_GYRO_GetFullScale(LSM6DSK320X_Object_t *pObj, int32_t *FullScale);
int32_t LSM6DSK320X_GYRO_SetFullScale(LSM6DSK320X_Object_t *pObj, int32_t FullScale);
int32_t LSM6DSK320X_GYRO_GetAxesRaw(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_AxesRaw_t *Value);
int32_t LSM6DSK320X_GYRO_GetAxes(LSM6DSK320X_Object_t *pObj, LSM6DSK320X_Axes_t *AngularRate);

int32_t LSM6DSK320X_Read_Reg(LSM6DSK320X_Object_t *pObj, uint8_t reg, uint8_t *Data);
int32_t LSM6DSK320X_Write_Reg(LSM6DSK320X_Object_t *pObj, uint8_t reg, uint8_t Data);

int32_t LSM6DSK320X_ACC_Get_DRDY_Status(LSM6DSK320X_Object_t *pObj, uint8_t *Status);
int32_t LSM6DSK320X_ACC_HG_Get_DRDY_Status(LSM6DSK320X_Object_t *pObj, uint8_t *Status);
int32_t LSM6DSK320X_GYRO_Get_DRDY_Status(LSM6DSK320X_Object_t *pObj, uint8_t *Status);

int32_t LSM6DSK320X_ACC_Set_Power_Mode(LSM6DSK320X_Object_t *pObj, uint8_t PowerMode);
int32_t LSM6DSK320X_GYRO_Set_Power_Mode(LSM6DSK320X_Object_t *pObj, uint8_t PowerMode);
int32_t LSM6DSK320X_ACC_Set_Filter_Mode(LSM6DSK320X_Object_t *pObj, uint8_t LowHighPassFlag, uint8_t FilterMode);
int32_t LSM6DSK320X_GYRO_Set_Filter_Mode(LSM6DSK320X_Object_t *pObj, uint8_t LowHighPassFlag, uint8_t FilterMode);

int32_t LSM6DSK320X_Set_Mem_Bank(LSM6DSK320X_Object_t *pObj, uint8_t Val);

/**
  * @}
  */

/** @addtogroup LSM6DSK320X_Exported_Variables LSM6DSK320X Exported Variables
  * @{
  */

extern LSM6DSK320X_CommonDrv_t LSM6DSK320X_COMMON_Driver;
extern LSM6DSK320X_ACC_Drv_t LSM6DSK320X_ACC_Driver;
extern LSM6DSK320X_GYRO_Drv_t LSM6DSK320X_GYRO_Driver;

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
