/**
  ******************************************************************************
  * @file    Ism330is_Acc_PnPL.h
  * @author  SRA
  * @brief   Ism330is_Acc PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fp_sns_datalog2_ispu:sensors:ism330is_acc;1
  *
  * Created by: DTDL2PnPL_cGen version 0.9.0
  *
  * WARNING! All changes made in this file will be lost relaunching the
  *          generation process!
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_ISM330IS_ACC_H_
#define _PNPL_ISM330IS_ACC_H_

#define hz12_5 12.5
#define hz26 26
#define hz52 52
#define hz104 104
#define hz208 208
#define hz416 416
#define hz833 833
#define hz1667 1667
#define hz3333 3333
#define hz6667 6667
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
  * Create a type name for _Ism330is_Acc_PnPL.
 */
typedef struct _Ism330is_Acc_PnPL Ism330is_Acc_PnPL;

// Public API declaration
//***********************

IPnPLComponent_t *Ism330is_Acc_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Ism330is_Acc_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Ism330is_Acc_PnPLGetComponentIF(Ism330is_Acc_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_ISM330IS_ACC_H_ */
