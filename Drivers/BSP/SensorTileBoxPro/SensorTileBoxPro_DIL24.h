/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SensorTileBoxPro_DIL24.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V0.1
  * @date    
  * @brief   header file for the BSP _DIL24 Common driver
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
*/
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SENSORTILEBOXPRO_DIL24_H
#define __SENSORTILEBOXPRO_DIL24_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "SensorTileBoxPro_conf.h"
#include "SensorTileBoxPro_env_sensors.h"
#include "SensorTileBoxPro_motion_sensors.h"

/* includes for DIL24 supported sensors */
   
#define  N_SUPPORTED_ADAPTERS  3

/* ENVIRONMENTAL (AND TMOS) */
#if (USE_DIL_SENSOR_SHT40AD1B_0 == 1)
  #include "sht40ad1b.h"
  #define SHT40AD1B_0 (0)
#endif

#if (USE_DIL_SENSOR_STHS34PF80_0 == 1)
  #include "sths34pf80.h"
  #define STHS34PF80_0 (USE_DIL_SENSOR_SHT40AD1B_0)
  #include "tmos_sensor.h"
#endif

/* MOTION */
#if (USE_DIL_SENSOR_LSM6DSO32_0 == 1)
  #include "lsm6dso32.h"
  #define LSM6DSO32_0 (0)
#endif

#if (USE_DIL_SENSOR_LSM6DSV16B_0 == 1)
  #include "lsm6dsv16b.h"
  #define LSM6DSV16B_0 (USE_DIL_SENSOR_LSM6DSO32_0)
#endif


/// example on how to implement new sensors
//#if (USE_DIL_SENSOR_ENVIRONMENTAL_XXX_0 == 1)
//  #include "ENVIRONMENTAL_XXX.h"
//  #define ENVIRONMENTAL_XXX (USE_DIL_OTHER_ENVIRONMENTAL_0 + USE_DIL_OTHER_ENVIRONMENTAL_0 + XXX-1)
//#endif



#define DIL24_OK        0
#define DIL24_ERROR     -1
   
#define NO_COM_SEL      0
#define DIL24_SPI       1
#define DIL24_I2C       2

#define ENVIRONMENTAL   1
#define MOTION          2

   


typedef struct
{
  uint32_t Functions;
} BSP_DIL24_SENSOR_Ctx_t;

typedef enum
{
  DILENV = 0,
  DILMOTION = 1,
  DILOTHER = 2
} sensortype_t;

typedef struct{
  uint32_t sensorhandle;
  uint8_t function;
  uint8_t uid;
  sensortype_t type;
} adapter_data_t;


typedef struct
{
  float_t env;
  BSP_MOTION_SENSOR_Axes_t motion;
  BSP_TMOS_Data_t tmos;
} BSP_DIL24_Data_t;

#ifndef DIL24_TEMPERATURE
#define DIL24_TEMPERATURE      1U
#endif
#ifndef DIL24_PRESSURE
#define DIL24_PRESSURE         2U
#endif
#ifndef DIL24_HUMIDITY
#define DIL24_HUMIDITY         4U
#endif

#ifndef DIL24_GYRO
#define DIL24_GYRO             8U
#endif
#ifndef DIL24_ACCELERO
#define DIL24_ACCELERO         16U
#endif
#ifndef DIL24_MAGNETO
#define DIL24_MAGNETO          32U
#endif

#ifndef DIL24_TMOS
#define DIL24_TMOS             64U
#endif


#define BSP_DIL_ENV_FUNCTIONS_NBR    3U
#define BSP_DIL_ENV_INSTANCES_NBR    (USE_DIL_SENSOR_SHT40AD1B_0 + USE_DIL_SENSOR_STHS34PF80_0)
#define BSP_DIL_MOTION_FUNCTIONS_NBR    3U
#define BSP_DIL_MOTION_INSTANCES_NBR    (USE_DIL_SENSOR_LSM6DSO32_0 + USE_DIL_SENSOR_LSM6DSV16B_0)


/* comm functions */
int32_t BSP_DIL24_I2C_Init(void);
int32_t BSP_DIL24_I2C_DeInit(void);
int32_t BSP_DIL24_I2C_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_DIL24_I2C_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_DIL24_SPI_Init(void);
int32_t BSP_DIL24_SPI_DeInit(void);
int32_t BSP_DIL24_SPI_Read(void);
int32_t BSP_DIL24_SPI_Write(void);
int32_t BSP_DIL24_Read(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_DIL24_Write(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C_Sensirion_Read(uint16_t DevAddr, uint8_t *pData, uint16_t Length);


int32_t BSP_DIL_SENSOR_Init(uint32_t Instance, uint32_t Functions);
int32_t BSP_DIL_SENSOR_Enable(uint32_t Instance, uint32_t Functions);
int32_t BSP_DIL_SENSOR_Disable(uint32_t Instance, uint32_t Functions);
int32_t BSP_DIL_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Functions, float_t Odr);
int32_t BSP_DIL_SENSOR_GetData(uint32_t Instance, uint32_t Functions, BSP_DIL24_Data_t *Data);
int32_t BSP_DIL_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float *Value);
int32_t BSP_DIL_MOTION_SENSOR_GetAxes(uint32_t Instance, uint32_t Function, BSP_MOTION_SENSOR_Axes_t *Axes);

//int32_t BSP_ENV_SENSOR_DeInit(uint32_t Instance);
//int32_t BSP_ENV_SENSOR_ReadID(uint32_t Instance, uint8_t *Id);
//int32_t BSP_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function);
//int32_t BSP_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float *Odr);
//int32_t BSP_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float Odr);



static int32_t SHT40AD1B_0_Probe(uint32_t Functions);
static int32_t LSM6DSO32_0_Probe(uint32_t Functions);
static int32_t STHS34PF80_0_Probe(uint32_t Functions);
int32_t BSP_DIL_TMOS_SENSOR_GetData(uint32_t Instance, BSP_TMOS_Data_t *OutData);
int32_t BSP_DIL_TMOS_SENSOR_CompensationInit(uint32_t Instance);
int32_t BSP_DIL_TMOS_SENSOR_CompensationDeInit(uint32_t Instance);


//typedef int32_t (*DIL24_Init_Func) (void);
//typedef int32_t (*DIL24_DeInit_Func) (void);
//typedef uint32_t (*DIL24_GetTick_Func) (void);
//typedef int32_t (*DIL24_Write_Func)(uint16_t, uint16_t, const uint8_t*, uint16_t);
//typedef int32_t (*DIL24_Read_Func) (uint16_t, uint16_t, uint8_t*, uint16_t);
//typedef int32_t (*DIL24_IsReady_Func) (uint16_t, const uint32_t);
//
//typedef int32_t (*DIL24_WriteReg_Func)(void *, uint16_t, const uint8_t*, uint16_t);
//typedef int32_t (*DIL24_ReadReg_Func) (void *, uint16_t, uint8_t*, uint16_t);

//typedef struct {
//  DIL24_WriteReg_Func WriteReg;
//  DIL24_ReadReg_Func ReadReg;
//  void *handle;
//} DIL24_Ctx_t;

//typedef struct {
//  DIL24_Init_Func    Init;
//  DIL24_DeInit_Func  DeInit;
//  DIL24_IsReady_Func IsReady;
//  DIL24_Write_Func   Write;
//  DIL24_Read_Func    Read;
//  DIL24_GetTick_Func GetTick;
//} DIL24_IO_t;
//
//
//typedef struct {
//  DIL24_IO_t IO ;
//  DIL24_Ctx_t Ctx ;
//  uint32_t IsInitialized;
//} DIL24_Object_t;
//
//typedef struct
//{
//  int32_t       (*Init)( DIL24_Object_t* );
//  int32_t       (*ReadID)( DIL24_Object_t*, uint8_t * const );
//  int32_t       (*IsReady)( DIL24_Object_t*, const uint32_t );
//  int32_t       (*GetITStatus)( DIL24_Object_t*, uint16_t * const );
//  int32_t       (*ConfigIT)( DIL24_Object_t*, const uint16_t );
//  int32_t       (*ReadData)( DIL24_Object_t*, uint8_t * const, const uint16_t, const uint16_t );
//  int32_t       (*WriteData)( DIL24_Object_t*, const uint8_t * const, const uint16_t, const uint16_t );
//} DIL24_Drv_t;

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /* __SENSORTILEBOXPRO_DIL24__H */

