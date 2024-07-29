/**
  ******************************************************************************
  * @file    Vl53l8cx_3_Tof_PnPL.h
  * @author  SRA
  * @brief   Vl53l8cx_3_Tof PnPL Component Manager
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */

/**
  ******************************************************************************
  * This file has been auto generated from the following DTDL Component:
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect:sensors:vl53l8cx_3_tof;1
  *
  * Created by: DTDL2PnPL_cGen version 2.0.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_VL53L8CX_3_TOF_H_
#define _PNPL_VL53L8CX_3_TOF_H_

typedef enum
{
  pnpl_vl53l8cx_3_tof_resolution_n4x4 = 0,
  pnpl_vl53l8cx_3_tof_resolution_n8x8 = 1,
} pnpl_vl53l8cx_3_tof_resolution_t;
typedef enum
{
  pnpl_vl53l8cx_3_tof_ranging_mode_continuous = 0,
  pnpl_vl53l8cx_3_tof_ranging_mode_autonomous = 1,
} pnpl_vl53l8cx_3_tof_ranging_mode_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *Vl53l8cx_3_Tof_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Vl53l8cx_3_Tof_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Vl53l8cx_3_Tof_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Vl53l8cx_3_Tof_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                          uint8_t pretty);
uint8_t Vl53l8cx_3_Tof_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                            uint32_t *size, uint8_t pretty);
uint8_t Vl53l8cx_3_Tof_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                uint32_t *size, uint8_t pretty);

/**
  * Create a type name for _Vl53l8cx_3_Tof_PnPL.
 */
typedef struct _Vl53l8cx_3_Tof_PnPL Vl53l8cx_3_Tof_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Vl53l8cx_3_Tof_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Vl53l8cx_3_Tof_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Vl53l8cx_3_Tof_PnPLGetComponentIF(Vl53l8cx_3_Tof_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_VL53L8CX_3_TOF_H_ */
