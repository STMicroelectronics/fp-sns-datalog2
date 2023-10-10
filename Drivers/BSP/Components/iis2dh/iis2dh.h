/**
******************************************************************************
* @file    iis2dh.h
* @author  SRA
* @brief   IIS2DH header driver file
******************************************************************************
* @attention
*
* <h2><center>&copy; Copyright (c) 2021 STMicroelectronics. 
* All rights reserved.</center></h2>
*
* This software component is licensed by ST under BSD 3-Clause license,
* the "License"; You may not use this file except in compliance with the 
* License. You may obtain a copy of the License at:
*                        opensource.org/licenses/BSD-3-Clause
*
******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IIS2DH_H
#define IIS2DH_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "iis2dh_reg.h"
#include <string.h>

/** @addtogroup BSP BSP
 * @{
 */

/** @addtogroup Component Component
 * @{
 */

/** @addtogroup IIS2DH IIS2DH
 * @{
 */

/** @defgroup IIS2DH_Exported_Types IIS2DH Exported Types
 * @{
 */

typedef int32_t (*IIS2DH_Init_Func)(void);
typedef int32_t (*IIS2DH_DeInit_Func)(void);
typedef int32_t (*IIS2DH_GetTick_Func)(void);
typedef int32_t (*IIS2DH_WriteReg_Func)(uint16_t, uint16_t, uint8_t *, uint16_t);
typedef int32_t (*IIS2DH_ReadReg_Func)(uint16_t, uint16_t, uint8_t *, uint16_t);

typedef enum
{
  IIS2DH_INT1_PIN,
  IIS2DH_INT2_PIN,
} IIS2DH_SensorIntPin_t;

typedef struct
{
  IIS2DH_Init_Func         Init;
  IIS2DH_DeInit_Func       DeInit;
  uint32_t                   BusType; /*0 means I2C, 1 means SPI 4-Wires, 2 means SPI-3-Wires */
  uint8_t                    Address;
  IIS2DH_WriteReg_Func     WriteReg;
  IIS2DH_ReadReg_Func      ReadReg;
  IIS2DH_GetTick_Func      GetTick;
} IIS2DH_IO_t;


typedef struct
{
  int16_t x;
  int16_t y;
  int16_t z;
} IIS2DH_AxesRaw_t;

typedef struct
{
  int32_t x;
  int32_t y;
  int32_t z;
} IIS2DH_Axes_t;

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
} IIS2DH_Event_Status_t;

typedef struct
{
  IIS2DH_IO_t       IO;
  stmdev_ctx_t      Ctx;
  uint8_t             is_initialized;
  uint8_t             acc_is_enabled;
  iis2dh_odr_t        acc_odr;
} IIS2DH_Object_t;

typedef struct
{
  uint8_t   Acc;
  uint8_t   Gyro;
  uint8_t   Magneto;
  uint8_t   LowPower;
  uint32_t  GyroMaxFS;
  uint32_t  AccMaxFS;
  uint32_t  MagMaxFS;
  float     GyroMaxOdr;
  float     AccMaxOdr;
  float     MagMaxOdr;
} IIS2DH_Capabilities_t;

typedef struct
{
  int32_t (*Init)(IIS2DH_Object_t *);
  int32_t (*DeInit)(IIS2DH_Object_t *);
  int32_t (*ReadID)(IIS2DH_Object_t *, uint8_t *);
  int32_t (*GetCapabilities)(IIS2DH_Object_t *, IIS2DH_Capabilities_t *);
} IIS2DH_CommonDrv_t;

typedef struct
{
  int32_t (*Enable)(IIS2DH_Object_t *);
  int32_t (*Disable)(IIS2DH_Object_t *);
  int32_t (*GetSensitivity)(IIS2DH_Object_t *, float *);
  int32_t (*GetOutputDataRate)(IIS2DH_Object_t *, float *);
  int32_t (*SetOutputDataRate)(IIS2DH_Object_t *, float);
  int32_t (*GetFullScale)(IIS2DH_Object_t *, int32_t *);
  int32_t (*SetFullScale)(IIS2DH_Object_t *, int32_t);
  int32_t (*GetAxes)(IIS2DH_Object_t *, IIS2DH_Axes_t *);
  int32_t (*GetAxesRaw)(IIS2DH_Object_t *, IIS2DH_AxesRaw_t *);
} IIS2DH_ACC_Drv_t;

typedef union{
  int16_t i16bit[3];
  uint8_t u8bit[6];
} iis2dh_axis3bit16_t;

typedef union{
  int16_t i16bit;
  uint8_t u8bit[2];
} iis2dh_axis1bit16_t;

typedef union{
  int32_t i32bit[3];
  uint8_t u8bit[12];
} iis2dh_axis3bit32_t;

typedef union{
  int32_t i32bit;
  uint8_t u8bit[4];
} iis2dh_axis1bit32_t;

/**
 * @}
 */

/** @defgroup IIS2DH_Exported_Constants IIS2DH Exported Constants
 * @{
 */

#define IIS2DH_OK                       0
#define IIS2DH_ERROR                   -1

#define IIS2DH_I2C_BUS                 0U
#define IIS2DH_SPI_4WIRES_BUS          1U
#define IIS2DH_SPI_3WIRES_BUS          2U

#define IIS2DH_ACC_SENSITIVITY_2G_NM      3.91f //!< Sensitivity value for 2g full scale in normal mode [mg/digit]
#define IIS2DH_ACC_SENSITIVITY_2G_HRM     0.98f //!< Sensitivity value for 2g full scale in high-resolution mode [mg/digit]
#define IIS2DH_ACC_SENSITIVITY_2G_LPM    15.63f //!< Sensitivity value for 2g full scale in low-power mode [mg/digit]
#define IIS2DH_ACC_SENSITIVITY_4G_NM      7.81f //!< Sensitivity value for 4g full scale in normal mode [mg/digit]
#define IIS2DH_ACC_SENSITIVITY_4G_HRM     1.95f //!< Sensitivity value for 4g full scale in high-resolution mode [mg/digit]
#define IIS2DH_ACC_SENSITIVITY_4G_LPM    31.25f //!< Sensitivity value for 4g full scale in low-power mode [mg/digit]
#define IIS2DH_ACC_SENSITIVITY_8G_NM     15.63f //!< Sensitivity value for 8g full scale in normal mode [mg/digit]
#define IIS2DH_ACC_SENSITIVITY_8G_HRM     3.91f //!< Sensitivity value for 8g full scale in high-resolution mode [mg/digit]
#define IIS2DH_ACC_SENSITIVITY_8G_LPM    62.50f //!< Sensitivity value for 8g full scale in low-power mode [mg/digit]
#define IIS2DH_ACC_SENSITIVITY_16G_NM    46.95f //!< Sensitivity value for 2g full scale in normal mode [mg/digit]
#define IIS2DH_ACC_SENSITIVITY_16G_HRM   11.72f //!< Sensitivity value for 2g full scale in high-resolution mode [mg/digit]
#define IIS2DH_ACC_SENSITIVITY_16G_LPM  188.68f //!< Sensitivity value for 2g full scale in low-power mode [mg/digit]

/**
 * @}
 */

/** @addtogroup IIS2DH_Exported_Functions IIS2DH Exported Functions
 * @{
 */

int32_t IIS2DH_RegisterBusIO(IIS2DH_Object_t *pObj, IIS2DH_IO_t *pIO);
int32_t IIS2DH_Init(IIS2DH_Object_t *pObj);
int32_t IIS2DH_DeInit(IIS2DH_Object_t *pObj);
int32_t IIS2DH_ReadID(IIS2DH_Object_t *pObj, uint8_t *Id);
int32_t IIS2DH_GetCapabilities(IIS2DH_Object_t *pObj, IIS2DH_Capabilities_t *Capabilities);

int32_t IIS2DH_ACC_Enable(IIS2DH_Object_t *pObj);
int32_t IIS2DH_ACC_Disable(IIS2DH_Object_t *pObj);
int32_t IIS2DH_ACC_GetSensitivity(IIS2DH_Object_t *pObj, float *Sensitivity);
int32_t IIS2DH_ACC_GetOutputDataRate(IIS2DH_Object_t *pObj, float *odr);
int32_t IIS2DH_ACC_SetOutputDataRate(IIS2DH_Object_t *pObj, float Odr);
int32_t IIS2DH_ACC_GetFullScale(IIS2DH_Object_t *pObj, int32_t *FullScale);
int32_t IIS2DH_ACC_SetFullScale(IIS2DH_Object_t *pObj, int32_t FullScale);
int32_t IIS2DH_ACC_GetAxesRaw(IIS2DH_Object_t *pObj, IIS2DH_AxesRaw_t *Value);
int32_t IIS2DH_ACC_GetAxes(IIS2DH_Object_t *pObj, IIS2DH_Axes_t *Acceleration);

int32_t IIS2DH_Read_Reg(IIS2DH_Object_t *pObj, uint8_t reg, uint8_t *Data);
int32_t IIS2DH_Write_Reg(IIS2DH_Object_t *pObj, uint8_t reg, uint8_t Data);
int32_t IIS2DH_Set_Interrupt_Latch(IIS2DH_Object_t *pObj, uint8_t Status);

int32_t IIS2DH_ACC_Enable_Wake_Up_Detection(IIS2DH_Object_t *pObj);
int32_t IIS2DH_ACC_Disable_Wake_Up_Detection(IIS2DH_Object_t *pObj);
int32_t IIS2DH_ACC_Set_Wake_Up_Threshold(IIS2DH_Object_t *pObj, uint8_t Threshold);
int32_t IIS2DH_ACC_Set_Wake_Up_Duration(IIS2DH_Object_t *pObj, uint8_t Duration);

int32_t IIS2DH_ACC_Enable_Inactivity_Detection(IIS2DH_Object_t *pObj);
int32_t IIS2DH_ACC_Disable_Inactivity_Detection(IIS2DH_Object_t *pObj);
int32_t IIS2DH_ACC_Set_Sleep_Duration(IIS2DH_Object_t *pObj, uint8_t Duration);

int32_t IIS2DH_ACC_Enable_6D_Orientation(IIS2DH_Object_t *pObj);
int32_t IIS2DH_ACC_Disable_6D_Orientation(IIS2DH_Object_t *pObj);
int32_t IIS2DH_ACC_Set_6D_Orientation_Threshold(IIS2DH_Object_t *pObj, uint8_t Threshold);
int32_t IIS2DH_ACC_Get_6D_Orientation_XL(IIS2DH_Object_t *pObj, uint8_t *XLow);
int32_t IIS2DH_ACC_Get_6D_Orientation_XH(IIS2DH_Object_t *pObj, uint8_t *XHigh);
int32_t IIS2DH_ACC_Get_6D_Orientation_YL(IIS2DH_Object_t *pObj, uint8_t *YLow);
int32_t IIS2DH_ACC_Get_6D_Orientation_YH(IIS2DH_Object_t *pObj, uint8_t *YHigh);
int32_t IIS2DH_ACC_Get_6D_Orientation_ZL(IIS2DH_Object_t *pObj, uint8_t *ZLow);
int32_t IIS2DH_ACC_Get_6D_Orientation_ZH(IIS2DH_Object_t *pObj, uint8_t *ZHigh);

int32_t IIS2DH_ACC_Get_Event_Status(IIS2DH_Object_t *pObj, IIS2DH_Event_Status_t *Status);
int32_t IIS2DH_ACC_Get_DRDY_Status(IIS2DH_Object_t *pObj, uint8_t *Status);
int32_t IIS2DH_ACC_Get_Init_Status(IIS2DH_Object_t *pObj, uint8_t *Status);

int32_t IIS2DH_FIFO_Get_Num_Samples(IIS2DH_Object_t *pObj, uint16_t *NumSamples);
int32_t IIS2DH_FIFO_Set_Mode(IIS2DH_Object_t *pObj, uint8_t Mode);

/**
 * @}
 */

/** @addtogroup IIS2DH_Exported_Variables IIS2DH Exported Variables
 * @{
 */

extern IIS2DH_CommonDrv_t IIS2DH_COMMON_Driver;
extern IIS2DH_ACC_Drv_t IIS2DH_ACC_Driver;

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
