/**
  ******************************************************************************
  * @file    vd6283tx.h
  * @author  IMG SW Application Team
  * @brief   This file contains all the functions prototypes for the vd6283tx.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef VD6283TX_H
#define VD6283TX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STALS.h"
#include <stddef.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup VD6283TX
  * @{
  */

/** @defgroup VD6283TX_Exported_Constants Exported Constants
  * @{
  */
#define VD6283TX_DEVICE_ID_REG        (0x00U)
#define VD6283TX_DEVICE_ID            (0x70U)

#define VD6283TX_MAX_CHANNELS         ((uint8_t)STALS_ALS_MAX_CHANNELS)     /*!< Number of channels of the device */
#define VD6283TX_ALL_CHANNELS         ((1U << STALS_ALS_MAX_CHANNELS) - 1U) /*!< Bitmask to select all ALS channels */

#define VD6283TX_CTRL_DARK            (STALS_OUTPUT_DARK_ENABLE)

#define VD6283TX_RED_CHANNEL          (0U)   /*!< Channel 1 */
#define VD6283TX_VISIBLE_CHANNEL      (1U)   /*!< Channel 2 */
#define VD6283TX_BLUE_CHANNEL         (2U)   /*!< Channel 3 */
#define VD6283TX_GREEN_CHANNEL        (3U)   /*!< Channel 4 */
#define VD6283TX_IR_CHANNEL           (4U)   /*!< Channel 5 */
#define VD6283TX_CLEAR_CHANNEL        (5U)   /*!< Channel 6 */

/* flicker capture modes */
#define VD6283TX_FLICKER_ANALOG       (0U)
#define VD6283TX_FLICKER_DIGITAL      (1U)

/* als capture modes */
#define VD6283TX_MODE_SINGLESHOT      (0U)
#define VD6283TX_MODE_CONTINUOUS      (1U)

/* vd6283tx error codes */
#define VD6283TX_OK                   (0)
#define VD6283TX_ERROR                (-1)
#define VD6283TX_INVALID_PARAM        (-2)
/**
  * @}
  */

/** @defgroup VD6283TX_Exported_Types Exported Types
  * @{
  */
typedef int32_t (*VD6283TX_Init_Func)(void);
typedef int32_t (*VD6283TX_DeInit_Func)(void);
typedef int32_t (*VD6283TX_GetTick_Func)(void);
typedef int32_t (*VD6283TX_Delay_Func)(uint32_t);
typedef int32_t (*VD6283TX_WriteReg_Func)(uint16_t, uint16_t, uint8_t *, uint16_t);
typedef int32_t (*VD6283TX_ReadReg_Func)(uint16_t, uint16_t, uint8_t *, uint16_t);

typedef struct
{
  VD6283TX_Init_Func Init;
  VD6283TX_DeInit_Func DeInit;
  uint16_t Address;
  VD6283TX_WriteReg_Func WriteReg;
  VD6283TX_ReadReg_Func ReadReg;
  VD6283TX_GetTick_Func GetTick;
} VD6283TX_IO_t;


#ifndef MEMS_SHARED_TYPES
#define MEMS_SHARED_TYPES

typedef int32_t (*stmdev_write_ptr)(void *, uint8_t, uint8_t *, uint16_t);
typedef int32_t (*stmdev_read_ptr)(void *, uint8_t, uint8_t *, uint16_t);
typedef void (*stmdev_mdelay_ptr)(uint32_t millisec);

typedef struct
{
  /** Component mandatory fields **/
  stmdev_write_ptr  write_reg;
  stmdev_read_ptr   read_reg;
  /** Component optional fields **/
  stmdev_mdelay_ptr   mdelay;
  /** Customizable optional pointer **/
  void *handle;
} stmdev_ctx_t;

#endif /* MEMS_SHARED_TYPES */

typedef struct
{
  uint8_t NumberOfChannels;  /*!< Max: LIGHT_SENSOR_MAX_CHANNELS */
  uint8_t FlickerDetection;  /*!< Not available: 0, Available: 1 */
  uint8_t Autogain;          /*!< Not available: 0, Available: 1 */
} VD6283TX_Capabilities_t;

typedef struct
{
  stmdev_ctx_t IO;
  void *handle;  /*!< bare-driver handle */
  uint32_t FlickerOutputType;
  uint8_t IsInitialized;
  uint8_t IsContinuous;
  uint8_t IsStarted;
  uint8_t IsFlickerActive;
} VD6283TX_Object_t;

typedef struct
{
  int32_t (*Init)(VD6283TX_Object_t *);
  int32_t (*DeInit)(VD6283TX_Object_t *);
  int32_t (*ReadID)(VD6283TX_Object_t *, uint32_t *);
  int32_t (*GetCapabilities)(VD6283TX_Object_t *, VD6283TX_Capabilities_t *);
  int32_t (*SetExposureTime)(VD6283TX_Object_t *, uint32_t);
  int32_t (*GetExposureTime)(VD6283TX_Object_t *, uint32_t *);
  int32_t (*SetGain)(VD6283TX_Object_t *, uint8_t, uint32_t);
  int32_t (*GetGain)(VD6283TX_Object_t *, uint8_t, uint32_t *);
  int32_t (*SetInterMeasurementTime)(VD6283TX_Object_t *, uint32_t);
  int32_t (*GetInterMeasurementTime)(VD6283TX_Object_t *, uint32_t *);
  int32_t (*Start)(VD6283TX_Object_t *, uint8_t);
  int32_t (*Stop)(VD6283TX_Object_t *);
  int32_t (*StartFlicker)(VD6283TX_Object_t *, uint8_t, uint8_t);
  int32_t (*StopFlicker)(VD6283TX_Object_t *);
  int32_t (*GetValues)(VD6283TX_Object_t *, uint32_t *);
  int32_t (*SetControlMode)(VD6283TX_Object_t *, uint32_t, uint32_t);
} VD6283TX_LIGHT_SENSOR_Drv_t;
/**
  * @}
  */

/** @defgroup VD6283TX_Exported_Functions Exported Functions
  * @{
  */
int32_t VD6283TX_RegisterBusIO(VD6283TX_Object_t *pObj, VD6283TX_IO_t *pIO);
int32_t VD6283TX_Init(VD6283TX_Object_t *pObj);
int32_t VD6283TX_DeInit(VD6283TX_Object_t *pObj);
int32_t VD6283TX_ReadID(VD6283TX_Object_t *pObj, uint32_t *pId);
int32_t VD6283TX_GetCapabilities(VD6283TX_Object_t *pObj, VD6283TX_Capabilities_t *pCapabilities);
int32_t VD6283TX_SetExposureTime(VD6283TX_Object_t *pObj, uint32_t ExposureTime);
int32_t VD6283TX_GetExposureTime(VD6283TX_Object_t *pObj, uint32_t *pExposureTime);
int32_t VD6283TX_SetGain(VD6283TX_Object_t *pObj, uint8_t Channel, uint32_t Gain);
int32_t VD6283TX_GetGain(VD6283TX_Object_t *pObj, uint8_t Channel, uint32_t *pGain);
int32_t VD6283TX_SetInterMeasurementTime(VD6283TX_Object_t *pObj, uint32_t InterMeasurementTime);
int32_t VD6283TX_GetInterMeasurementTime(VD6283TX_Object_t *pObj, uint32_t *pInterMeasurementTime);
int32_t VD6283TX_Start(VD6283TX_Object_t *pObj, uint8_t Mode);
int32_t VD6283TX_Stop(VD6283TX_Object_t *pObj);
int32_t VD6283TX_StartFlicker(VD6283TX_Object_t *pObj, uint8_t Channel, uint8_t OutputMode);
int32_t VD6283TX_StopFlicker(VD6283TX_Object_t *pObj);
int32_t VD6283TX_GetValues(VD6283TX_Object_t *pObj, uint32_t *pResult);
int32_t VD6283TX_SetControlMode(VD6283TX_Object_t *pObj, uint32_t ControlMode, uint32_t Value);

int32_t VD6283TX_GetSaturation(VD6283TX_Object_t *pObj, uint32_t *pValue);

/* LIGHT_SENSOR driver structure */
extern VD6283TX_LIGHT_SENSOR_Drv_t   VD6283TX_LIGHT_SENSOR_Driver;
/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* VD6283TX_H */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
