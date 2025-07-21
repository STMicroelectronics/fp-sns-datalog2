/**
  ******************************************************************************
  * @file    Lsm6dsv80x_H_Acc_PnPL.h
  * @author  SRA
  * @brief   Lsm6dsv80x_H_Acc PnPL Component Manager
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:lsm6dsv80x_h_acc;1
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_LSM6DSV80X_H_ACC_H_
#define _PNPL_LSM6DSV80X_H_ACC_H_

typedef enum
{
  pnpl_lsm6dsv80x_h_acc_odr_hz480 = 0,
  pnpl_lsm6dsv80x_h_acc_odr_hz960 = 1,
  pnpl_lsm6dsv80x_h_acc_odr_hz1920 = 2,
  pnpl_lsm6dsv80x_h_acc_odr_hz3840 = 3,
  pnpl_lsm6dsv80x_h_acc_odr_hz7680 = 4,
} pnpl_lsm6dsv80x_h_acc_odr_t;
typedef enum
{
  pnpl_lsm6dsv80x_h_acc_fs_g32 = 0,
  pnpl_lsm6dsv80x_h_acc_fs_g64 = 1,
  pnpl_lsm6dsv80x_h_acc_fs_g80 = 2,
} pnpl_lsm6dsv80x_h_acc_fs_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *Lsm6dsv80x_H_Acc_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Lsm6dsv80x_H_Acc_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Lsm6dsv80x_H_Acc_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Lsm6dsv80x_H_Acc_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                            uint8_t pretty);
uint8_t Lsm6dsv80x_H_Acc_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                              uint32_t *size, uint8_t pretty);
uint8_t Lsm6dsv80x_H_Acc_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                  uint32_t *size, uint8_t pretty);

/**
  * Create a type name for _Lsm6dsv80x_H_Acc_PnPL.
 */
typedef struct _Lsm6dsv80x_H_Acc_PnPL Lsm6dsv80x_H_Acc_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Lsm6dsv80x_H_Acc_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Lsm6dsv80x_H_Acc_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Lsm6dsv80x_H_Acc_PnPLGetComponentIF(Lsm6dsv80x_H_Acc_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_LSM6DSV80X_H_ACC_H_ */
