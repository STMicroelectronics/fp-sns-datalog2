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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:stts22h_temp;6
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_STTS22H_TEMP_H_
#define _PNPL_STTS22H_TEMP_H_

typedef enum
{
  pnpl_stts22h_temp_odr_hz1 = 0,
  pnpl_stts22h_temp_odr_hz25 = 1,
  pnpl_stts22h_temp_odr_hz50 = 2,
  pnpl_stts22h_temp_odr_hz100 = 3,
  pnpl_stts22h_temp_odr_hz200 = 4,
} pnpl_stts22h_temp_odr_t;
typedef enum
{
  pnpl_stts22h_temp_fs_cdeg100 = 0,
} pnpl_stts22h_temp_fs_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *Stts22h_Temp_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Stts22h_Temp_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Stts22h_Temp_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Stts22h_Temp_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty);
uint8_t Stts22h_Temp_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                          uint32_t *size, uint8_t pretty);
uint8_t Stts22h_Temp_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                              uint32_t *size, uint8_t pretty);

/**
  * Create a type name for _Stts22h_Temp_PnPL.
 */
typedef struct _Stts22h_Temp_PnPL Stts22h_Temp_PnPL;

/* Public API declaration ----------------------------------------------------*/

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
