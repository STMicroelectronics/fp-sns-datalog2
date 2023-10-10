/**
  ******************************************************************************
  * @file    Lsm6dsv16x_Gyro_PnPL.h
  * @author  SRA
  * @brief   Lsm6dsv16x_Gyro PnPL Component Manager
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:lsm6dsv16x_gyro;3
  *
  * Created by: DTDL2PnPL_cGen version 1.2.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_LSM6DSV16X_GYRO_H_
#define _PNPL_LSM6DSV16X_GYRO_H_

#define lsm6dsv16x_gyro_odr_hz7_5 7.5
#define lsm6dsv16x_gyro_odr_hz15 15
#define lsm6dsv16x_gyro_odr_hz30 30
#define lsm6dsv16x_gyro_odr_hz60 60
#define lsm6dsv16x_gyro_odr_hz120 120
#define lsm6dsv16x_gyro_odr_hz240 240
#define lsm6dsv16x_gyro_odr_hz480 480
#define lsm6dsv16x_gyro_odr_hz960 960
#define lsm6dsv16x_gyro_odr_hz1920 1920
#define lsm6dsv16x_gyro_odr_hz3840 3840
#define lsm6dsv16x_gyro_odr_hz7680 7680
#define lsm6dsv16x_gyro_fs_dps125 125
#define lsm6dsv16x_gyro_fs_dps250 250
#define lsm6dsv16x_gyro_fs_dps500 500
#define lsm6dsv16x_gyro_fs_dps1000 1000
#define lsm6dsv16x_gyro_fs_dps2000 2000
#define lsm6dsv16x_gyro_fs_dps4000 4000


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"


/**
  * Create a type name for _Lsm6dsv16x_Gyro_PnPL.
 */
typedef struct _Lsm6dsv16x_Gyro_PnPL Lsm6dsv16x_Gyro_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Lsm6dsv16x_Gyro_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Lsm6dsv16x_Gyro_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Lsm6dsv16x_Gyro_PnPLGetComponentIF(Lsm6dsv16x_Gyro_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_LSM6DSV16X_GYRO_H_ */
