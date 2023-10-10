/**
  ******************************************************************************
  * @file    Iis3dwb_Acc_PnPL_vtbl.h
  * @author  SRA
  * @brief   Iis3dwb_Acc PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:sensors:iis3dwb_acc;4
  *
  * Created by: DTDL2PnPL_cGen version 1.2.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_IIS3DWB_ACC_VTBL_H_
#define _PNPL_IIS3DWB_ACC_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

char *Iis3dwb_Acc_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Iis3dwb_Acc_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Iis3dwb_Acc_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Iis3dwb_Acc_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty);
uint8_t Iis3dwb_Acc_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size, uint8_t pretty);
uint8_t Iis3dwb_Acc_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size, uint8_t pretty);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_IIS3DWB_ACC_VTBL_H_ */
