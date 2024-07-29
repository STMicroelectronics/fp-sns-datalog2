/**
  ******************************************************************************
  * @file    Lps22df_Press_PnPL.h
  * @author  SRA
  * @brief   Lps22df_Press PnPL Component Manager
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
  * dtmi:dtmi:vespucci:steval_stwinbx1:FP_SNS_DATALOG2_PDetect:sensors:lps22df_press;1
  *
  * Created by: DTDL2PnPL_cGen version 2.0.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_LPS22DF_PRESS_H_
#define _PNPL_LPS22DF_PRESS_H_

typedef enum
{
  pnpl_lps22df_press_odr_hz1 = 0,
  pnpl_lps22df_press_odr_hz4 = 1,
  pnpl_lps22df_press_odr_hz10 = 2,
  pnpl_lps22df_press_odr_hz25 = 3,
  pnpl_lps22df_press_odr_hz50 = 4,
  pnpl_lps22df_press_odr_hz75 = 5,
  pnpl_lps22df_press_odr_hz100 = 6,
  pnpl_lps22df_press_odr_hz200 = 7,
} pnpl_lps22df_press_odr_t;
typedef enum
{
  pnpl_lps22df_press_fs_hpa1260 = 0,
} pnpl_lps22df_press_fs_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *Lps22df_Press_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Lps22df_Press_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Lps22df_Press_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Lps22df_Press_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                         uint8_t pretty);
uint8_t Lps22df_Press_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                           uint32_t *size, uint8_t pretty);
uint8_t Lps22df_Press_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                               uint32_t *size, uint8_t pretty);

/**
  * Create a type name for _Lps22df_Press_PnPL.
 */
typedef struct _Lps22df_Press_PnPL Lps22df_Press_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Lps22df_Press_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Lps22df_Press_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Lps22df_Press_PnPLGetComponentIF(Lps22df_Press_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_LPS22DF_PRESS_H_ */
