/**
  ******************************************************************************
  * @file    Imp34dt05_Mic_PnPL.h
  * @author  SRA
  * @brief   Imp34dt05_Mic PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect1:sensors:imp34dt05_mic;1
  *
  * Created by: DTDL2PnPL_cGen version 2.0.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_IMP34DT05_MIC_H_
#define _PNPL_IMP34DT05_MIC_H_

typedef enum
{
  pnpl_imp34dt05_mic_odr_hz16000 = 0,
  pnpl_imp34dt05_mic_odr_hz32000 = 1,
  pnpl_imp34dt05_mic_odr_hz48000 = 2,
} pnpl_imp34dt05_mic_odr_t;
typedef enum
{
  pnpl_imp34dt05_mic_aop_dbspl122 = 0,
} pnpl_imp34dt05_mic_aop_t;
typedef enum
{
  pnpl_imp34dt05_mic_resolution_bit16 = 0,
} pnpl_imp34dt05_mic_resolution_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *Imp34dt05_Mic_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Imp34dt05_Mic_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Imp34dt05_Mic_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Imp34dt05_Mic_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                         uint8_t pretty);
uint8_t Imp34dt05_Mic_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                           uint32_t *size, uint8_t pretty);
uint8_t Imp34dt05_Mic_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                               uint32_t *size, uint8_t pretty);

/**
  * Create a type name for _Imp34dt05_Mic_PnPL.
 */
typedef struct _Imp34dt05_Mic_PnPL Imp34dt05_Mic_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Imp34dt05_Mic_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Imp34dt05_Mic_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Imp34dt05_Mic_PnPLGetComponentIF(Imp34dt05_Mic_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_IMP34DT05_MIC_H_ */