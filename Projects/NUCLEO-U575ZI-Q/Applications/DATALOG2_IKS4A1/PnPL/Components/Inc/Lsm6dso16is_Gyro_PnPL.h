/**
  ******************************************************************************
  * @file    Lsm6dso16is_Gyro_PnPL.h
  * @author  SRA
  * @brief   Lsm6dso16is_Gyro PnPL Component Manager
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
  * dtmi:vespucci:nucleo_u575zi_q:x_nucleo_iks4a1:FP_SNS_DATALOG2_Datalog2:sensors:lsm6dso16is_gyro;1
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_LSM6DSO16IS_GYRO_H_
#define _PNPL_LSM6DSO16IS_GYRO_H_

typedef enum
{
  pnpl_lsm6dso16is_gyro_odr_hz12_5 = 0,
  pnpl_lsm6dso16is_gyro_odr_hz26 = 1,
  pnpl_lsm6dso16is_gyro_odr_hz52 = 2,
  pnpl_lsm6dso16is_gyro_odr_hz104 = 3,
  pnpl_lsm6dso16is_gyro_odr_hz208 = 4,
  pnpl_lsm6dso16is_gyro_odr_hz416 = 5,
  pnpl_lsm6dso16is_gyro_odr_hz833 = 6,
  pnpl_lsm6dso16is_gyro_odr_hz1667 = 7,
  pnpl_lsm6dso16is_gyro_odr_hz3333 = 8,
  pnpl_lsm6dso16is_gyro_odr_hz6667 = 9,
} pnpl_lsm6dso16is_gyro_odr_t;
typedef enum
{
  pnpl_lsm6dso16is_gyro_fs_dps125 = 0,
  pnpl_lsm6dso16is_gyro_fs_dps250 = 1,
  pnpl_lsm6dso16is_gyro_fs_dps500 = 2,
  pnpl_lsm6dso16is_gyro_fs_dps1000 = 3,
  pnpl_lsm6dso16is_gyro_fs_dps2000 = 4,
} pnpl_lsm6dso16is_gyro_fs_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *Lsm6dso16is_Gyro_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Lsm6dso16is_Gyro_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Lsm6dso16is_Gyro_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Lsm6dso16is_Gyro_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                            uint8_t pretty);
uint8_t Lsm6dso16is_Gyro_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                              uint32_t *size, uint8_t pretty);
uint8_t Lsm6dso16is_Gyro_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                  uint32_t *size, uint8_t pretty);

/**
  * Create a type name for _Lsm6dso16is_Gyro_PnPL.
 */
typedef struct _Lsm6dso16is_Gyro_PnPL Lsm6dso16is_Gyro_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Lsm6dso16is_Gyro_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Lsm6dso16is_Gyro_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Lsm6dso16is_Gyro_PnPLGetComponentIF(Lsm6dso16is_Gyro_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_LSM6DSO16IS_GYRO_H_ */
