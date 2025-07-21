/**
  ******************************************************************************
  * @file    Lis2duxs12_Mlc_PnPL.h
  * @author  SRA
  * @brief   Lis2duxs12_Mlc PnPL Component Manager
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
  * dtmi:vespucci:nucleo_u575zi_q:x_nucleo_iks4a1:FP_SNS_DATALOG2_Datalog2:sensors:lis2duxs12_mlc;1
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_LIS2DUXS12_MLC_H_
#define _PNPL_LIS2DUXS12_MLC_H_


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *Lis2duxs12_Mlc_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Lis2duxs12_Mlc_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Lis2duxs12_Mlc_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Lis2duxs12_Mlc_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                          uint8_t pretty);
uint8_t Lis2duxs12_Mlc_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                            uint32_t *size, uint8_t pretty);
uint8_t Lis2duxs12_Mlc_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                uint32_t *size, uint8_t pretty);

/**
  * Create a type name for _Lis2duxs12_Mlc_PnPL.
 */
typedef struct _Lis2duxs12_Mlc_PnPL Lis2duxs12_Mlc_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Lis2duxs12_Mlc_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Lis2duxs12_Mlc_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Lis2duxs12_Mlc_PnPLGetComponentIF(Lis2duxs12_Mlc_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_LIS2DUXS12_MLC_H_ */
