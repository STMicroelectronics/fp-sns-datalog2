/**
  ******************************************************************************
  * @file    Iis2iclx_Acc_PnPL.h
  * @author  SRA
  * @brief   Iis2iclx_Acc PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:sensors:iis2iclx_acc;3
  *
  * Created by: DTDL2PnPL_cGen version 1.2.3
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_IIS2ICLX_ACC_H_
#define _PNPL_IIS2ICLX_ACC_H_

#define iis2iclx_acc_odr_hz12_5 12.5
#define iis2iclx_acc_odr_hz26 26
#define iis2iclx_acc_odr_hz52 52
#define iis2iclx_acc_odr_hz104 104
#define iis2iclx_acc_odr_hz208 208
#define iis2iclx_acc_odr_hz416 416
#define iis2iclx_acc_odr_hz833 833
#define iis2iclx_acc_fs_g0_5 0.5
#define iis2iclx_acc_fs_g1 1
#define iis2iclx_acc_fs_g2 2
#define iis2iclx_acc_fs_g3 3


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"


/**
  * Create a type name for _Iis2iclx_Acc_PnPL.
 */
typedef struct _Iis2iclx_Acc_PnPL Iis2iclx_Acc_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Iis2iclx_Acc_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Iis2iclx_Acc_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Iis2iclx_Acc_PnPLGetComponentIF(Iis2iclx_Acc_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_IIS2ICLX_ACC_H_ */
