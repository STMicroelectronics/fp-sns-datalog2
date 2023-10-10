/**
  ******************************************************************************
  * @file    Imp23absu_Mic_PnPL.h
  * @author  SRA
  * @brief   Imp23absu_Mic PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:sensors:imp23absu_mic;4
  *
  * Created by: DTDL2PnPL_cGen version 1.2.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_IMP23ABSU_MIC_H_
#define _PNPL_IMP23ABSU_MIC_H_

#define imp23absu_mic_odr_hz16000 16000
#define imp23absu_mic_odr_hz32000 32000
#define imp23absu_mic_odr_hz48000 48000
#define imp23absu_mic_odr_hz96000 96000
#define imp23absu_mic_odr_hz192000 192000
#define imp23absu_mic_resolution_bit16 16


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"


/**
  * Create a type name for _Imp23absu_Mic_PnPL.
 */
typedef struct _Imp23absu_Mic_PnPL Imp23absu_Mic_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Imp23absu_Mic_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Imp23absu_Mic_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Imp23absu_Mic_PnPLGetComponentIF(Imp23absu_Mic_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_IMP23ABSU_MIC_H_ */
