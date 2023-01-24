/**
  ******************************************************************************
  * @file    Acquisition_InformationPnPL.h
  * @author  SRA
  * @brief   Acquisition_Information PnPL Component Manager
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_ACQUISITION_INFORMATION_H_
#define _PNPL_ACQUISITION_INFORMATION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/**
  * Create a type name for _Acquisition_Information_PnPL.
 */
typedef struct _Acquisition_Information_PnPL Acquisition_Information_PnPL;

// Public API declaration
//***********************

IPnPLComponent_t *Acquisition_Information_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Acquisition_Information_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Acquisition_Information_PnPLGetComponentIF(Acquisition_Information_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_ACQUISITION_INFORMATION_H_ */