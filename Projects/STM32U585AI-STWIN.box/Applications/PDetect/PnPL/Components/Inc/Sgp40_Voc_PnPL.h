/**
  ******************************************************************************
  * @file    Sgp40_Voc_PnPL.h
  * @author  SRA
  * @brief   Sgp40_Voc PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:FP_SNS_DATALOG2_PDetect:sensors:sgp40_voc;1
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_SGP40_VOC_H_
#define _PNPL_SGP40_VOC_H_

typedef enum
{
  pnpl_sgp40_voc_odr_hz1 = 0,
} pnpl_sgp40_voc_odr_t;
typedef enum
{
  pnpl_sgp40_voc_fs_voc100 = 0,
} pnpl_sgp40_voc_fs_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *Sgp40_Voc_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Sgp40_Voc_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Sgp40_Voc_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Sgp40_Voc_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty);
uint8_t Sgp40_Voc_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size,
                                       uint8_t pretty);
uint8_t Sgp40_Voc_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                           uint32_t *size, uint8_t pretty);

/**
  * Create a type name for _Sgp40_Voc_PnPL.
 */
typedef struct _Sgp40_Voc_PnPL Sgp40_Voc_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Sgp40_Voc_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Sgp40_Voc_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Sgp40_Voc_PnPLGetComponentIF(Sgp40_Voc_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_SGP40_VOC_H_ */
