/**
  ******************************************************************************
  * @file    Ism330is_Gyro_PnPL.h
  * @author  SRA
  * @brief   Ism330is_Gyro PnPL Component Manager
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:ism330is_gyro;3
  *
  * Created by: DTDL2PnPL_cGen version 1.2.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_ISM330IS_GYRO_H_
#define _PNPL_ISM330IS_GYRO_H_

#define ism330is_gyro_odr_hz12_5 12.5
#define ism330is_gyro_odr_hz26 26
#define ism330is_gyro_odr_hz52 52
#define ism330is_gyro_odr_hz104 104
#define ism330is_gyro_odr_hz208 208
#define ism330is_gyro_odr_hz416 416
#define ism330is_gyro_odr_hz833 833
#define ism330is_gyro_odr_hz1667 1667
#define ism330is_gyro_odr_hz3333 3333
#define ism330is_gyro_odr_hz6667 6667
#define ism330is_gyro_fs_dps125 125
#define ism330is_gyro_fs_dps250 250
#define ism330is_gyro_fs_dps500 500
#define ism330is_gyro_fs_dps1000 1000
#define ism330is_gyro_fs_dps2000 2000


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"


/**
  * Create a type name for _Ism330is_Gyro_PnPL.
 */
typedef struct _Ism330is_Gyro_PnPL Ism330is_Gyro_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Ism330is_Gyro_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Ism330is_Gyro_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Ism330is_Gyro_PnPLGetComponentIF(Ism330is_Gyro_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_ISM330IS_GYRO_H_ */