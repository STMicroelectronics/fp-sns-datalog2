/**
  ******************************************************************************
  * @file    Lsm6dsv16bx_Gyro_PnPL.h
  * @author  SRA
  * @brief   Lsm6dsv16bx_Gyro PnPL Component Manager
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:lsm6dsv16bx_gyro;1
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_LSM6DSV16BX_GYRO_H_
#define _PNPL_LSM6DSV16BX_GYRO_H_

typedef enum
{
  pnpl_lsm6dsv16bx_gyro_odr_hz7_5 = 0,
  pnpl_lsm6dsv16bx_gyro_odr_hz15 = 1,
  pnpl_lsm6dsv16bx_gyro_odr_hz30 = 2,
  pnpl_lsm6dsv16bx_gyro_odr_hz60 = 3,
  pnpl_lsm6dsv16bx_gyro_odr_hz120 = 4,
  pnpl_lsm6dsv16bx_gyro_odr_hz240 = 5,
  pnpl_lsm6dsv16bx_gyro_odr_hz480 = 6,
  pnpl_lsm6dsv16bx_gyro_odr_hz960 = 7,
  pnpl_lsm6dsv16bx_gyro_odr_hz1920 = 8,
  pnpl_lsm6dsv16bx_gyro_odr_hz3840 = 9,
  pnpl_lsm6dsv16bx_gyro_odr_hz7680 = 10,
} pnpl_lsm6dsv16bx_gyro_odr_t;
typedef enum
{
  pnpl_lsm6dsv16bx_gyro_fs_dps125 = 0,
  pnpl_lsm6dsv16bx_gyro_fs_dps250 = 1,
  pnpl_lsm6dsv16bx_gyro_fs_dps500 = 2,
  pnpl_lsm6dsv16bx_gyro_fs_dps1000 = 3,
  pnpl_lsm6dsv16bx_gyro_fs_dps2000 = 4,
  pnpl_lsm6dsv16bx_gyro_fs_dps4000 = 5,
} pnpl_lsm6dsv16bx_gyro_fs_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *Lsm6dsv16bx_Gyro_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Lsm6dsv16bx_Gyro_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Lsm6dsv16bx_Gyro_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Lsm6dsv16bx_Gyro_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                            uint8_t pretty);
uint8_t Lsm6dsv16bx_Gyro_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                              uint32_t *size, uint8_t pretty);
uint8_t Lsm6dsv16bx_Gyro_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                  uint32_t *size, uint8_t pretty);

/**
  * Create a type name for _Lsm6dsv16bx_Gyro_PnPL.
 */
typedef struct _Lsm6dsv16bx_Gyro_PnPL Lsm6dsv16bx_Gyro_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Lsm6dsv16bx_Gyro_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Lsm6dsv16bx_Gyro_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Lsm6dsv16bx_Gyro_PnPLGetComponentIF(Lsm6dsv16bx_Gyro_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_LSM6DSV16BX_GYRO_H_ */