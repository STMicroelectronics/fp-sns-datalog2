/**
  ******************************************************************************
  * @file    Sths34pf80_Tmos_PnPL.h
  * @author  SRA
  * @brief   Sths34pf80_Tmos PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect:sensors:sths34pf80_tmos;1
  *
  * Created by: DTDL2PnPL_cGen version 1.2.3
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_STHS34PF80_TMOS_H_
#define _PNPL_STHS34PF80_TMOS_H_

#define sths34pf80_tmos_odr_hz1 1
#define sths34pf80_tmos_odr_hz2 2
#define sths34pf80_tmos_odr_hz4 4
#define sths34pf80_tmos_odr_hz8 8
#define sths34pf80_tmos_odr_hz15 15
#define sths34pf80_tmos_odr_hz30 30
#define sths34pf80_tmos_avg_tobject_num_n2 2
#define sths34pf80_tmos_avg_tobject_num_n8 8
#define sths34pf80_tmos_avg_tobject_num_n32 32
#define sths34pf80_tmos_avg_tobject_num_n128 128
#define sths34pf80_tmos_avg_tobject_num_n256 256
#define sths34pf80_tmos_avg_tobject_num_n512 512
#define sths34pf80_tmos_avg_tobject_num_n1024 1024
#define sths34pf80_tmos_avg_tobject_num_n2048 2048
#define sths34pf80_tmos_avg_tambient_num_n1 1
#define sths34pf80_tmos_avg_tambient_num_n2 2
#define sths34pf80_tmos_avg_tambient_num_n4 4
#define sths34pf80_tmos_avg_tambient_num_n8 8
#define sths34pf80_tmos_lpf_p_m_bandwidth_n9 9
#define sths34pf80_tmos_lpf_p_m_bandwidth_n20 20
#define sths34pf80_tmos_lpf_p_m_bandwidth_n50 50
#define sths34pf80_tmos_lpf_p_m_bandwidth_n100 100
#define sths34pf80_tmos_lpf_p_m_bandwidth_n200 200
#define sths34pf80_tmos_lpf_p_m_bandwidth_n400 400
#define sths34pf80_tmos_lpf_p_m_bandwidth_n800 800
#define sths34pf80_tmos_lpf_p_bandwidth_n9 9
#define sths34pf80_tmos_lpf_p_bandwidth_n20 20
#define sths34pf80_tmos_lpf_p_bandwidth_n50 50
#define sths34pf80_tmos_lpf_p_bandwidth_n100 100
#define sths34pf80_tmos_lpf_p_bandwidth_n200 200
#define sths34pf80_tmos_lpf_p_bandwidth_n400 400
#define sths34pf80_tmos_lpf_p_bandwidth_n800 800
#define sths34pf80_tmos_lpf_m_bandwidth_n9 9
#define sths34pf80_tmos_lpf_m_bandwidth_n20 20
#define sths34pf80_tmos_lpf_m_bandwidth_n50 50
#define sths34pf80_tmos_lpf_m_bandwidth_n100 100
#define sths34pf80_tmos_lpf_m_bandwidth_n200 200
#define sths34pf80_tmos_lpf_m_bandwidth_n400 400
#define sths34pf80_tmos_lpf_m_bandwidth_n800 800
#define sths34pf80_tmos_compensation_type_ipd_comp_none "IPD_COMP_NONE"
#define sths34pf80_tmos_compensation_type_ipd_comp_lin "IPD_COMP_LIN"
#define sths34pf80_tmos_compensation_type_ipd_comp_nonlin "IPD_COMP_NONLIN"


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"


/**
  * Create a type name for _Sths34pf80_Tmos_PnPL.
 */
typedef struct _Sths34pf80_Tmos_PnPL Sths34pf80_Tmos_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Sths34pf80_Tmos_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Sths34pf80_Tmos_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Sths34pf80_Tmos_PnPLGetComponentIF(Sths34pf80_Tmos_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_STHS34PF80_TMOS_H_ */
