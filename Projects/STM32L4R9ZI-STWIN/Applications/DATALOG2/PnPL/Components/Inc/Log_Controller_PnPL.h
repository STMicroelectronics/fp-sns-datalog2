/**
  ******************************************************************************
  * @file    Log_Controller_PnPL.h
  * @author  SRA
  * @brief   Log_Controller PnPL Component Manager
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
  * dtmi:vespucci:other:log_controller;3
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_LOG_CONTROLLER_H_
#define _PNPL_LOG_CONTROLLER_H_


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *Log_Controller_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Log_Controller_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Log_Controller_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Log_Controller_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                          uint8_t pretty);
uint8_t Log_Controller_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                            uint32_t *size, uint8_t pretty);
uint8_t Log_Controller_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                uint32_t *size, uint8_t pretty);

/**
  * Create a type name for _Log_Controller_PnPL.
 */
typedef struct _Log_Controller_PnPL Log_Controller_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Log_Controller_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Log_Controller_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Log_Controller_PnPLGetComponentIF(Log_Controller_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_LOG_CONTROLLER_H_ */