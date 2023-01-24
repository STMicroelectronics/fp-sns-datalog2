/**
  ******************************************************************************
  * @file    Iis2dlpc_Acc_PnPL.h
  * @author  SRA
  * @brief   Iis2dlpc_Acc PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fp_sns_datalog2:sensors:iis2dlpc_acc;1
  *
  * Created by: DTDL2PnPL_cGen version 0.9.0
  *
  * WARNING! All changes made in this file will be lost relaunching the
  *          generation process!
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_IIS2DLPC_ACC_H_
#define _PNPL_IIS2DLPC_ACC_H_

#define hz12_5 12.5
#define hz25 25
#define hz50 50
#define hz100 100
#define hz200 200
#define hz400 400
#define hz800 800
#define hz1600 1600
#define g2 2
#define g4 4
#define g8 8
#define g16 16


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/**
  * Create a type name for _Iis2dlpc_Acc_PnPL.
 */
typedef struct _Iis2dlpc_Acc_PnPL Iis2dlpc_Acc_PnPL;

// Public API declaration
//***********************

IPnPLComponent_t *Iis2dlpc_Acc_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Iis2dlpc_Acc_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Iis2dlpc_Acc_PnPLGetComponentIF(Iis2dlpc_Acc_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_IIS2DLPC_ACC_H_ */