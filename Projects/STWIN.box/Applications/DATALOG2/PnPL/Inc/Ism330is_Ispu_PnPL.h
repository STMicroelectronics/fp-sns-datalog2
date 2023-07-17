/**
  ******************************************************************************
  * @file    Ism330is_Ispu_PnPL.h
  * @author  SRA
  * @brief   Ism330is_Ispu PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2Ispu:sensors:ism330is_ispu;3
  *
  * Created by: DTDL2PnPL_cGen version 1.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_ISM330IS_ISPU_H_
#define _PNPL_ISM330IS_ISPU_H_



#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"
#include "IIsm330is_Ispu.h"
#include "IIsm330is_Ispu_vtbl.h"

/**
  * Create a type name for _Ism330is_Ispu_PnPL.
 */
typedef struct _Ism330is_Ispu_PnPL Ism330is_Ispu_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Ism330is_Ispu_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Ism330is_Ispu_PnPLInit(IPnPLComponent_t *_this,  IIsm330is_Ispu_t *inf);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Ism330is_Ispu_PnPLGetComponentIF(Ism330is_Ispu_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_ISM330IS_ISPU_H_ */
