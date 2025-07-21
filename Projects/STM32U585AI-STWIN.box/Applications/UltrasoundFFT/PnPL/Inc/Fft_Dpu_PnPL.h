/**
  ******************************************************************************
  * @file    Fft_Dpu_PnPL.h
  * @author  SRA
  * @brief   Fft_Dpu PnPL Component Manager
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_ultrasoundFft:algorithms:fft_dpu;3
  *
  * Created by: DTDL2PnPL_cGen version 1.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_FFT_DPU_H_
#define _PNPL_FFT_DPU_H_


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"


/**
  * Create a type name for _Fft_Dpu_PnPL.
 */
typedef struct _Fft_Dpu_PnPL Fft_Dpu_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Fft_Dpu_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Fft_Dpu_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Fft_Dpu_PnPLGetComponentIF(Fft_Dpu_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_FFT_DPU_H_ */
