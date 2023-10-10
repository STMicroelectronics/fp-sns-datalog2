/**
  ******************************************************************************
  * @file    Deviceinformation_PnPL.h
  * @author  SRA
  * @brief   Deviceinformation PnPL Component Manager
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
  * dtmi:azure:DeviceManagement:DeviceInformation;1
  *
  * Created by: DTDL2PnPL_cGen version 1.2.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_DEVICEINFORMATION_H_
#define _PNPL_DEVICEINFORMATION_H_



#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"


/**
  * Create a type name for _Deviceinformation_PnPL.
 */
typedef struct _Deviceinformation_PnPL Deviceinformation_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Deviceinformation_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Deviceinformation_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Deviceinformation_PnPLGetComponentIF(Deviceinformation_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_DEVICEINFORMATION_H_ */
