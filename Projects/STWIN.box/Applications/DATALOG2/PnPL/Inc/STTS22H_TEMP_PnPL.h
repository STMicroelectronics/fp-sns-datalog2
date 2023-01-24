/**
  ******************************************************************************
  * @file    Stts22h_Temp_PnPL.h
  * @author  SRA
  * @brief   Stts22h_Temp PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fp_sns_datalog2:sensors:stts22h_temp;1
  *
  * Created by: DTDL2PnPL_cGen version 0.9.0
  *
  * WARNING! All changes made in this file will be lost relaunching the
  *          generation process!
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_STTS22H_TEMP_H_
#define _PNPL_STTS22H_TEMP_H_

#define hz1 1
#define hz25 25
#define hz50 50
#define hz100 100
#define hz200 200
#define cdeg100 100


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/**
  * Create a type name for _Stts22h_Temp_PnPL.
 */
typedef struct _Stts22h_Temp_PnPL Stts22h_Temp_PnPL;

// Public API declaration
//***********************

IPnPLComponent_t *Stts22h_Temp_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Stts22h_Temp_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Stts22h_Temp_PnPLGetComponentIF(Stts22h_Temp_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_STTS22H_TEMP_H_ */