/**
  ******************************************************************************
  * @file    H3lis331dl_Acc_PnPL.h
  * @author  SRA
  * @brief   H3lis331dl_Acc PnPL Component Manager
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:h3lis331dl_acc;1
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_H3LIS331DL_ACC_H_
#define _PNPL_H3LIS331DL_ACC_H_

typedef enum
{
  pnpl_h3lis331dl_acc_odr_hz0_5 = 0,
  pnpl_h3lis331dl_acc_odr_hz1 = 1,
  pnpl_h3lis331dl_acc_odr_hz2 = 2,
  pnpl_h3lis331dl_acc_odr_hz5 = 3,
  pnpl_h3lis331dl_acc_odr_hz10 = 4,
  pnpl_h3lis331dl_acc_odr_hz50 = 5,
  pnpl_h3lis331dl_acc_odr_hz100 = 6,
  pnpl_h3lis331dl_acc_odr_hz400 = 7,
  pnpl_h3lis331dl_acc_odr_hz1000 = 8,
} pnpl_h3lis331dl_acc_odr_t;
typedef enum
{
  pnpl_h3lis331dl_acc_fs_g100 = 0,
  pnpl_h3lis331dl_acc_fs_g200 = 1,
  pnpl_h3lis331dl_acc_fs_g400 = 2,
} pnpl_h3lis331dl_acc_fs_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *H3lis331dl_Acc_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t H3lis331dl_Acc_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *H3lis331dl_Acc_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t H3lis331dl_Acc_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                          uint8_t pretty);
uint8_t H3lis331dl_Acc_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                            uint32_t *size, uint8_t pretty);
uint8_t H3lis331dl_Acc_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                uint32_t *size, uint8_t pretty);

/**
  * Create a type name for _H3lis331dl_Acc_PnPL.
 */
typedef struct _H3lis331dl_Acc_PnPL H3lis331dl_Acc_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *H3lis331dl_Acc_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t H3lis331dl_Acc_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *H3lis331dl_Acc_PnPLGetComponentIF(H3lis331dl_Acc_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_H3LIS331DL_ACC_H_ */
