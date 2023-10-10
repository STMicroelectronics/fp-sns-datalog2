/**
  ******************************************************************************
  * @file    Ism330dhcx_Mlc_PnPL.h
  * @author  SRA
  * @brief   Ism330dhcx_Mlc PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:sensors:ism330dhcx_mlc;3
  *
  * Created by: DTDL2PnPL_cGen version 1.2.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_ISM330DHCX_MLC_H_
#define _PNPL_ISM330DHCX_MLC_H_



#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"
#include "IIsm330dhcx_Mlc.h"
#include "IIsm330dhcx_Mlc_vtbl.h"

/**
  * Create a type name for _Ism330dhcx_Mlc_PnPL.
 */
typedef struct _Ism330dhcx_Mlc_PnPL Ism330dhcx_Mlc_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Ism330dhcx_Mlc_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Ism330dhcx_Mlc_PnPLInit(IPnPLComponent_t *_this,  IIsm330dhcx_Mlc_t *inf);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Ism330dhcx_Mlc_PnPLGetComponentIF(Ism330dhcx_Mlc_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_ISM330DHCX_MLC_H_ */
