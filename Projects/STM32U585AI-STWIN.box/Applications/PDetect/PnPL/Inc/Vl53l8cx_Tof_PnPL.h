/**
  ******************************************************************************
  * @file    Vl53l8cx_Tof_PnPL.h
  * @author  SRA
  * @brief   Vl53l8cx_Tof PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect:sensors:vl53l8cx_tof;1
  *
  * Created by: DTDL2PnPL_cGen version 1.2.3
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_VL53L8CX_TOF_H_
#define _PNPL_VL53L8CX_TOF_H_

#define vl53l8cx_tof_resolution_n4x4 "4x4"
#define vl53l8cx_tof_resolution_n8x8 "8x8"
#define vl53l8cx_tof_ranging_mode_continuous "Continuous"
#define vl53l8cx_tof_ranging_mode_autonomous "Autonomous"


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"


/**
  * Create a type name for _Vl53l8cx_Tof_PnPL.
 */
typedef struct _Vl53l8cx_Tof_PnPL Vl53l8cx_Tof_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Vl53l8cx_Tof_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Vl53l8cx_Tof_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Vl53l8cx_Tof_PnPLGetComponentIF(Vl53l8cx_Tof_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_VL53L8CX_TOF_H_ */
