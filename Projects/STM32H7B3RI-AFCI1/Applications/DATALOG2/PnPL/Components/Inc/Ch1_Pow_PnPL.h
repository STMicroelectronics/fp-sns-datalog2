/**
  ******************************************************************************
  * @file    Ch1_Pow_PnPL.h
  * @author  SRA
  * @brief   Ch1_Pow PnPL Component Manager
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
  * dtmi:vespucci:afci_h7:fpSnsDatalog2_datalog2:sensors:ch1_pow;1
  *
  * Created by: DTDL2PnPL_cGen version 2.3.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_CH1_POW_H_
#define _PNPL_CH1_POW_H_

typedef enum
{
  pnpl_ch1_pow_odr_hz100000 = 0,
  pnpl_ch1_pow_odr_hz150000 = 1,
  pnpl_ch1_pow_odr_hz200000 = 2,
  pnpl_ch1_pow_odr_hz250000 = 3,
} pnpl_ch1_pow_odr_t;
typedef enum
{
  pnpl_ch1_pow_fs_ua1 = 0,
} pnpl_ch1_pow_fs_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *Ch1_Pow_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Ch1_Pow_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Ch1_Pow_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Ch1_Pow_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty);
uint8_t Ch1_Pow_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size,
                                     uint8_t pretty);
uint8_t Ch1_Pow_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size,
                                         uint8_t pretty);

/**
  * Create a type name for _Ch1_Pow_PnPL.
 */
typedef struct _Ch1_Pow_PnPL Ch1_Pow_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Ch1_Pow_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Ch1_Pow_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Ch1_Pow_PnPLGetComponentIF(Ch1_Pow_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_CH1_POW_H_ */
