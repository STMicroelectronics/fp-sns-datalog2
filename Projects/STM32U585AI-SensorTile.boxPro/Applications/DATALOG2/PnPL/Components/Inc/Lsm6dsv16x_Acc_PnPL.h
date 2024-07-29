/**
  ******************************************************************************
  * @file    Lsm6dsv16x_Acc_PnPL.h
  * @author  SRA
  * @brief   Lsm6dsv16x_Acc PnPL Component Manager
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:lsm6dsv16x_acc;4
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_LSM6DSV16X_ACC_H_
#define _PNPL_LSM6DSV16X_ACC_H_

typedef enum
{
  pnpl_lsm6dsv16x_acc_odr_hz7_5 = 0,
  pnpl_lsm6dsv16x_acc_odr_hz15 = 1,
  pnpl_lsm6dsv16x_acc_odr_hz30 = 2,
  pnpl_lsm6dsv16x_acc_odr_hz60 = 3,
  pnpl_lsm6dsv16x_acc_odr_hz120 = 4,
  pnpl_lsm6dsv16x_acc_odr_hz240 = 5,
  pnpl_lsm6dsv16x_acc_odr_hz480 = 6,
  pnpl_lsm6dsv16x_acc_odr_hz960 = 7,
  pnpl_lsm6dsv16x_acc_odr_hz1920 = 8,
  pnpl_lsm6dsv16x_acc_odr_hz3840 = 9,
  pnpl_lsm6dsv16x_acc_odr_hz7680 = 10,
} pnpl_lsm6dsv16x_acc_odr_t;
typedef enum
{
  pnpl_lsm6dsv16x_acc_fs_g2 = 0,
  pnpl_lsm6dsv16x_acc_fs_g4 = 1,
  pnpl_lsm6dsv16x_acc_fs_g8 = 2,
  pnpl_lsm6dsv16x_acc_fs_g16 = 3,
} pnpl_lsm6dsv16x_acc_fs_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *Lsm6dsv16x_Acc_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Lsm6dsv16x_Acc_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Lsm6dsv16x_Acc_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Lsm6dsv16x_Acc_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                          uint8_t pretty);
uint8_t Lsm6dsv16x_Acc_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                            uint32_t *size, uint8_t pretty);
uint8_t Lsm6dsv16x_Acc_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                uint32_t *size, uint8_t pretty);

/**
  * Create a type name for _Lsm6dsv16x_Acc_PnPL.
 */
typedef struct _Lsm6dsv16x_Acc_PnPL Lsm6dsv16x_Acc_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Lsm6dsv16x_Acc_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Lsm6dsv16x_Acc_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Lsm6dsv16x_Acc_PnPLGetComponentIF(Lsm6dsv16x_Acc_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_LSM6DSV16X_ACC_H_ */
