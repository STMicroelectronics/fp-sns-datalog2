/**
  ******************************************************************************
  * @file    Sht40_Hum_PnPL.h
  * @author  SRA
  * @brief   Sht40_Hum PnPL Component Manager
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
  * dtmi:vespucci:nucleo_u575zi_q:x_nucleo_iks4a1:FP_SNS_DATALOG2_Datalog2:sensors:sht40_hum;1
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_SHT40_HUM_H_
#define _PNPL_SHT40_HUM_H_

typedef enum
{
  pnpl_sht40_hum_odr_hz1 = 0,
} pnpl_sht40_hum_odr_t;
typedef enum
{
  pnpl_sht40_hum_fs_rh100 = 0,
} pnpl_sht40_hum_fs_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *Sht40_Hum_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Sht40_Hum_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Sht40_Hum_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Sht40_Hum_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty);
uint8_t Sht40_Hum_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size,
                                       uint8_t pretty);
uint8_t Sht40_Hum_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                           uint32_t *size, uint8_t pretty);

/**
  * Create a type name for _Sht40_Hum_PnPL.
 */
typedef struct _Sht40_Hum_PnPL Sht40_Hum_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Sht40_Hum_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Sht40_Hum_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Sht40_Hum_PnPLGetComponentIF(Sht40_Hum_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_SHT40_HUM_H_ */
