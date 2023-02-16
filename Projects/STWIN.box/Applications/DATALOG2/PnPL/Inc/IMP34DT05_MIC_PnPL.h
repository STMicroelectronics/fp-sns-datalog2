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
  * dtmi:appconfig:steval_stwinbx1:fp_sns_datalog2:sensors:imp34dt05_mic;1
  *
  * Created by: DTDL2PnPL_cGen version 1.0.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_IMP34DT05_MIC_H_
#define _PNPL_IMP34DT05_MIC_H_

#define imp34dt05_mic_odr_hz16000 16000
#define imp34dt05_mic_odr_hz32000 32000
#define imp34dt05_mic_odr_hz48000 48000
#define imp34dt05_mic_aop_dbspl130 130


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

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
