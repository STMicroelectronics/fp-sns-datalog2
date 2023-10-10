/**
  ******************************************************************************
  * @file    Ism330dhcx_Gyro_PnPL.h
  * @author  SRA
  * @brief   Ism330dhcx_Gyro PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:sensors:ism330dhcx_gyro;3
  *
  * Created by: DTDL2PnPL_cGen version 1.2.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_ISM330DHCX_GYRO_H_
#define _PNPL_ISM330DHCX_GYRO_H_

#define ism330dhcx_gyro_odr_hz12_5 12.5
#define ism330dhcx_gyro_odr_hz26 26
#define ism330dhcx_gyro_odr_hz52 52
#define ism330dhcx_gyro_odr_hz104 104
#define ism330dhcx_gyro_odr_hz208 208
#define ism330dhcx_gyro_odr_hz416 416
#define ism330dhcx_gyro_odr_hz833 833
#define ism330dhcx_gyro_odr_hz1666 1666
#define ism330dhcx_gyro_odr_hz3332 3332
#define ism330dhcx_gyro_odr_hz6667 6667
#define ism330dhcx_gyro_fs_dps125 125
#define ism330dhcx_gyro_fs_dps250 250
#define ism330dhcx_gyro_fs_dps500 500
#define ism330dhcx_gyro_fs_dps1000 1000
#define ism330dhcx_gyro_fs_dps2000 2000
#define ism330dhcx_gyro_fs_dps4000 4000


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"


/**
  * Create a type name for _Ism330dhcx_Gyro_PnPL.
 */
typedef struct _Ism330dhcx_Gyro_PnPL Ism330dhcx_Gyro_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Ism330dhcx_Gyro_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Ism330dhcx_Gyro_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Ism330dhcx_Gyro_PnPLGetComponentIF(Ism330dhcx_Gyro_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_ISM330DHCX_GYRO_H_ */
