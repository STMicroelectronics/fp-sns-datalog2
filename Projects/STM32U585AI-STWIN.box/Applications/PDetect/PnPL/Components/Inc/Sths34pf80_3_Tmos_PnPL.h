/**
  ******************************************************************************
  * @file    Sths34pf80_3_Tmos_PnPL.h
  * @author  SRA
  * @brief   Sths34pf80_3_Tmos PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect:sensors:sths34pf80_3_tmos;2
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_STHS34PF80_3_TMOS_H_
#define _PNPL_STHS34PF80_3_TMOS_H_

typedef enum
{
  pnpl_sths34pf80_3_tmos_odr_hz1 = 0,
  pnpl_sths34pf80_3_tmos_odr_hz2 = 1,
  pnpl_sths34pf80_3_tmos_odr_hz4 = 2,
  pnpl_sths34pf80_3_tmos_odr_hz8 = 3,
  pnpl_sths34pf80_3_tmos_odr_hz15 = 4,
  pnpl_sths34pf80_3_tmos_odr_hz30 = 5,
} pnpl_sths34pf80_3_tmos_odr_t;
typedef enum
{
  pnpl_sths34pf80_3_tmos_avg_tobject_num_n2 = 0,
  pnpl_sths34pf80_3_tmos_avg_tobject_num_n8 = 1,
  pnpl_sths34pf80_3_tmos_avg_tobject_num_n32 = 2,
  pnpl_sths34pf80_3_tmos_avg_tobject_num_n128 = 3,
  pnpl_sths34pf80_3_tmos_avg_tobject_num_n256 = 4,
  pnpl_sths34pf80_3_tmos_avg_tobject_num_n512 = 5,
  pnpl_sths34pf80_3_tmos_avg_tobject_num_n1024 = 6,
  pnpl_sths34pf80_3_tmos_avg_tobject_num_n2048 = 7,
} pnpl_sths34pf80_3_tmos_avg_tobject_num_t;
typedef enum
{
  pnpl_sths34pf80_3_tmos_avg_tambient_num_n1 = 0,
  pnpl_sths34pf80_3_tmos_avg_tambient_num_n2 = 1,
  pnpl_sths34pf80_3_tmos_avg_tambient_num_n4 = 2,
  pnpl_sths34pf80_3_tmos_avg_tambient_num_n8 = 3,
} pnpl_sths34pf80_3_tmos_avg_tambient_num_t;
typedef enum
{
  pnpl_sths34pf80_3_tmos_lpf_p_m_bandwidth_n9 = 0,
  pnpl_sths34pf80_3_tmos_lpf_p_m_bandwidth_n20 = 1,
  pnpl_sths34pf80_3_tmos_lpf_p_m_bandwidth_n50 = 2,
  pnpl_sths34pf80_3_tmos_lpf_p_m_bandwidth_n100 = 3,
  pnpl_sths34pf80_3_tmos_lpf_p_m_bandwidth_n200 = 4,
  pnpl_sths34pf80_3_tmos_lpf_p_m_bandwidth_n400 = 5,
  pnpl_sths34pf80_3_tmos_lpf_p_m_bandwidth_n800 = 6,
} pnpl_sths34pf80_3_tmos_lpf_p_m_bandwidth_t;
typedef enum
{
  pnpl_sths34pf80_3_tmos_lpf_p_bandwidth_n9 = 0,
  pnpl_sths34pf80_3_tmos_lpf_p_bandwidth_n20 = 1,
  pnpl_sths34pf80_3_tmos_lpf_p_bandwidth_n50 = 2,
  pnpl_sths34pf80_3_tmos_lpf_p_bandwidth_n100 = 3,
  pnpl_sths34pf80_3_tmos_lpf_p_bandwidth_n200 = 4,
  pnpl_sths34pf80_3_tmos_lpf_p_bandwidth_n400 = 5,
  pnpl_sths34pf80_3_tmos_lpf_p_bandwidth_n800 = 6,
} pnpl_sths34pf80_3_tmos_lpf_p_bandwidth_t;
typedef enum
{
  pnpl_sths34pf80_3_tmos_lpf_m_bandwidth_n9 = 0,
  pnpl_sths34pf80_3_tmos_lpf_m_bandwidth_n20 = 1,
  pnpl_sths34pf80_3_tmos_lpf_m_bandwidth_n50 = 2,
  pnpl_sths34pf80_3_tmos_lpf_m_bandwidth_n100 = 3,
  pnpl_sths34pf80_3_tmos_lpf_m_bandwidth_n200 = 4,
  pnpl_sths34pf80_3_tmos_lpf_m_bandwidth_n400 = 5,
  pnpl_sths34pf80_3_tmos_lpf_m_bandwidth_n800 = 6,
} pnpl_sths34pf80_3_tmos_lpf_m_bandwidth_t;
typedef enum
{
  pnpl_sths34pf80_3_tmos_compensation_type_ipd_comp_none = 0,
  pnpl_sths34pf80_3_tmos_compensation_type_ipd_comp_lin = 1,
  pnpl_sths34pf80_3_tmos_compensation_type_ipd_comp_nonlin = 2,
} pnpl_sths34pf80_3_tmos_compensation_type_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *Sths34pf80_3_Tmos_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Sths34pf80_3_Tmos_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Sths34pf80_3_Tmos_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Sths34pf80_3_Tmos_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                             uint8_t pretty);
uint8_t Sths34pf80_3_Tmos_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                               uint32_t *size, uint8_t pretty);
uint8_t Sths34pf80_3_Tmos_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                   uint32_t *size, uint8_t pretty);

/**
  * Create a type name for _Sths34pf80_3_Tmos_PnPL.
 */
typedef struct _Sths34pf80_3_Tmos_PnPL Sths34pf80_3_Tmos_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Sths34pf80_3_Tmos_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Sths34pf80_3_Tmos_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Sths34pf80_3_Tmos_PnPLGetComponentIF(Sths34pf80_3_Tmos_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_STHS34PF80_3_TMOS_H_ */
