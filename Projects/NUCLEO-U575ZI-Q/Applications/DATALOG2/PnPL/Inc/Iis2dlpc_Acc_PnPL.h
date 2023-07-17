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
  * dtmi:vespucci:nucleo_u575zi_q:x_nucleo_iks02a1:fpSnsDatalog2_datalog2:sensors:iis2dlpc_acc;1
  *
  * Created by: DTDL2PnPL_cGen version 1.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_IIS2DLPC_ACC_H_
#define _PNPL_IIS2DLPC_ACC_H_

#define iis2dlpc_acc_odr_hz12_5 12.5
#define iis2dlpc_acc_odr_hz25 25
#define iis2dlpc_acc_odr_hz50 50
#define iis2dlpc_acc_odr_hz100 100
#define iis2dlpc_acc_odr_hz200 200
#define iis2dlpc_acc_odr_hz400 400
#define iis2dlpc_acc_odr_hz800 800
#define iis2dlpc_acc_odr_hz1600 1600
#define iis2dlpc_acc_fs_g2 2
#define iis2dlpc_acc_fs_g4 4
#define iis2dlpc_acc_fs_g8 8
#define iis2dlpc_acc_fs_g16 16


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

/* Public API declaration ----------------------------------------------------*/

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
