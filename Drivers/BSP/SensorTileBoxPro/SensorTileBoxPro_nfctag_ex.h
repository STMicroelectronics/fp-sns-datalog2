/**
  ******************************************************************************
  * @file    SensorTileBoxPro_nfctag_ex.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.1.0
  * @date    20-July-2023
  * @brief   This file contains definitions for the SensorTileBoxPro_nfctag_ex.c
  *          specific functions.
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
#ifndef __SENSORTILEBOXPRO_NFCTAG_EX_H__
#define __SENSORTILEBOXPRO_NFCTAG_EX_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "SensorTileBoxPro_conf.h"
#include "st25dv.h"

/* Exported function	--------------------------------------------------------*/

int32_t BSP_NFCTAG_ChangeITPulse(uint32_t MsbPasswd,uint32_t LsbPasswd,const uint8_t ITtime);
int32_t BSP_NFCTAG_ChangeMBMode(uint32_t MsbPasswd,uint32_t LsbPasswd,const uint8_t MB_mode);
int32_t BSP_NFCTAG_ChangeI2CPassword(uint32_t MsbPasswd,uint32_t LsbPasswd);
int32_t BSP_NFCTAG_WriteConfigIT(uint32_t MsbPasswd,uint32_t LsbPasswd,const uint16_t ITConfig);
int32_t BSP_NFCTAG_ChangeMBWDG(uint32_t MsbPasswd,uint32_t LsbPasswd,const uint8_t WdgDelay);

#endif /* __SENSORTILEBOXPRO_NFCTAG_EX_H__ */

