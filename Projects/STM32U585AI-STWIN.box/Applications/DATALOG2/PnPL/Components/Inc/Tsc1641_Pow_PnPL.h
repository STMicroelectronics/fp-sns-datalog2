/**
  ******************************************************************************
  * @file    Tsc1641_Pow_PnPL.h
  * @author  SRA
  * @brief   Tsc1641_Pow PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:sensors:tsc1641_pow;2
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_TSC1641_POW_H_
#define _PNPL_TSC1641_POW_H_

typedef enum
{
  pnpl_tsc1641_pow_adc_conversion_time_n128 = 0,
  pnpl_tsc1641_pow_adc_conversion_time_n256 = 1,
  pnpl_tsc1641_pow_adc_conversion_time_n512 = 2,
  pnpl_tsc1641_pow_adc_conversion_time_n1024 = 3,
  pnpl_tsc1641_pow_adc_conversion_time_n2048 = 4,
  pnpl_tsc1641_pow_adc_conversion_time_n4096 = 5,
  pnpl_tsc1641_pow_adc_conversion_time_n8192 = 6,
  pnpl_tsc1641_pow_adc_conversion_time_n16384 = 7,
  pnpl_tsc1641_pow_adc_conversion_time_n32768 = 8,
} pnpl_tsc1641_pow_adc_conversion_time_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *Tsc1641_Pow_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Tsc1641_Pow_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Tsc1641_Pow_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Tsc1641_Pow_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty);
uint8_t Tsc1641_Pow_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size,
                                         uint8_t pretty);
uint8_t Tsc1641_Pow_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                             uint32_t *size, uint8_t pretty);

/**
  * Create a type name for _Tsc1641_Pow_PnPL.
 */
typedef struct _Tsc1641_Pow_PnPL Tsc1641_Pow_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Tsc1641_Pow_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Tsc1641_Pow_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Tsc1641_Pow_PnPLGetComponentIF(Tsc1641_Pow_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_TSC1641_POW_H_ */
