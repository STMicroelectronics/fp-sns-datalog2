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
  * dtmi:vespucci:steval_stwinbx1:fp_sns_datalog2:sensors:ism330dhcx_gyro;1
  *
  * Created by: DTDL2PnPL_cGen version 0.9.0
  *
  * WARNING! All changes made in this file will be lost relaunching the
  *          generation process!
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_ISM330DHCX_GYRO_H_
#define _PNPL_ISM330DHCX_GYRO_H_

#define hz12_5 12.5
#define hz26 26
#define hz52 52
#define hz104 104
#define hz208 208
#define hz416 416
#define hz833 833
#define hz1666 1666
#define hz3332 3332
#define hz6667 6667
#define dps125 125
#define dps250 250
#define dps500 500
#define dps1000 1000
#define dps2000 2000
#define dps4000 4000


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

// Public API declaration
//***********************

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