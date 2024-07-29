/**
 ******************************************************************************
 * @file    tmos_sensor.h
 * @author  
 * @brief   This header file contains the functions prototypes for the
 *          tmos driver
 ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
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
#ifndef TMOS_SENSOR_H
#define TMOS_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif



/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup BSP BSP
 * @{
 */

/** @addtogroup COMPONENTS COMPONENTS
 * @{
 */

/** @addtogroup COMMON COMMON
 * @{
 */

/** @addtogroup TMOS_SENSOR TMOS SENSOR
 * @{
 */

/** @addtogroup TMOS_SENSOR_Public_Types TMOS SENSOR Public types
 * @{
 */
typedef struct
{
  int16_t t_obj;
  int16_t t_amb;
  int16_t t_obj_comp;
  int16_t t_pres;
  int16_t t_motion;
  int16_t t_amb_shock;
} BSP_TMOS_Data_t;
/**
 * @brief  TMOS_SENSOR driver structure definition
 */
typedef struct
{
  uint32_t Functions;
} BSP_TMOS_SENSOR_Ctx_t;

typedef struct
{
  int32_t ( *Init              ) ( void * );
  int32_t ( *DeInit            ) ( void * );
  int32_t ( *ReadID            ) ( void *, uint8_t * ); 
  int32_t ( *GetCapabilities   ) ( void *, void * );
} TMOS_SENSOR_CommonDrv_t;

typedef struct
{
  int32_t ( *Enable            ) ( void * );
  int32_t ( *Disable           ) ( void * );
  int32_t ( *GetOutputDataRate ) ( void *, float * );
  int32_t ( *SetOutputDataRate ) ( void *, float );
  int32_t ( *GetData          ) ( void *, BSP_TMOS_Data_t * );
} TMOS_SENSOR_FuncDrv_t;

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

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* TMOS_SENSOR_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
