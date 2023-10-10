/**
  ******************************************************************************
  * @file    Iis3dwb_Acc_PnPL.h
  * @author  SRA
  * @brief   Iis3dwb_Acc PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:sensors:iis3dwb_acc;4
  *
  * Created by: DTDL2PnPL_cGen version 1.2.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_IIS3DWB_ACC_H_
#define _PNPL_IIS3DWB_ACC_H_

#define iis3dwb_acc_odr_hz26667 26667
#define iis3dwb_acc_fs_g2 2
#define iis3dwb_acc_fs_g4 4
#define iis3dwb_acc_fs_g8 8
#define iis3dwb_acc_fs_g16 16


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"


/**
  * Create a type name for _Iis3dwb_Acc_PnPL.
 */
typedef struct _Iis3dwb_Acc_PnPL Iis3dwb_Acc_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Iis3dwb_Acc_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Iis3dwb_Acc_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Iis3dwb_Acc_PnPLGetComponentIF(Iis3dwb_Acc_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_IIS3DWB_ACC_H_ */
