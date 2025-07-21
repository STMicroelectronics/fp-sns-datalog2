/**
  ******************************************************************************
  * @file    Vd6283tx_3_Als_PnPL.h
  * @author  SRA
  * @brief   Vd6283tx_3_Als PnPL Component Manager
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect:sensors:vd6283tx_3_als;2
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_VD6283TX_3_ALS_H_
#define _PNPL_VD6283TX_3_ALS_H_

typedef enum
{
  pnpl_vd6283tx_3_als_channel1_gain_n0_71 = 0,
  pnpl_vd6283tx_3_als_channel1_gain_n0_83 = 1,
  pnpl_vd6283tx_3_als_channel1_gain_n1 = 2,
  pnpl_vd6283tx_3_als_channel1_gain_n1_25 = 3,
  pnpl_vd6283tx_3_als_channel1_gain_n1_67 = 4,
  pnpl_vd6283tx_3_als_channel1_gain_n2_5 = 5,
  pnpl_vd6283tx_3_als_channel1_gain_n3_33 = 6,
  pnpl_vd6283tx_3_als_channel1_gain_n5 = 7,
  pnpl_vd6283tx_3_als_channel1_gain_n7_1 = 8,
  pnpl_vd6283tx_3_als_channel1_gain_n10 = 9,
  pnpl_vd6283tx_3_als_channel1_gain_n16 = 10,
  pnpl_vd6283tx_3_als_channel1_gain_n25 = 11,
  pnpl_vd6283tx_3_als_channel1_gain_n33 = 12,
  pnpl_vd6283tx_3_als_channel1_gain_n50 = 13,
  pnpl_vd6283tx_3_als_channel1_gain_n66_6 = 14,
} pnpl_vd6283tx_3_als_channel1_gain_t;
typedef enum
{
  pnpl_vd6283tx_3_als_channel2_gain_n0_71 = 0,
  pnpl_vd6283tx_3_als_channel2_gain_n0_83 = 1,
  pnpl_vd6283tx_3_als_channel2_gain_n1 = 2,
  pnpl_vd6283tx_3_als_channel2_gain_n1_25 = 3,
  pnpl_vd6283tx_3_als_channel2_gain_n1_67 = 4,
  pnpl_vd6283tx_3_als_channel2_gain_n2_5 = 5,
  pnpl_vd6283tx_3_als_channel2_gain_n3_33 = 6,
  pnpl_vd6283tx_3_als_channel2_gain_n5 = 7,
  pnpl_vd6283tx_3_als_channel2_gain_n7_1 = 8,
  pnpl_vd6283tx_3_als_channel2_gain_n10 = 9,
  pnpl_vd6283tx_3_als_channel2_gain_n16 = 10,
  pnpl_vd6283tx_3_als_channel2_gain_n25 = 11,
  pnpl_vd6283tx_3_als_channel2_gain_n33 = 12,
  pnpl_vd6283tx_3_als_channel2_gain_n50 = 13,
  pnpl_vd6283tx_3_als_channel2_gain_n66_6 = 14,
} pnpl_vd6283tx_3_als_channel2_gain_t;
typedef enum
{
  pnpl_vd6283tx_3_als_channel3_gain_n0_71 = 0,
  pnpl_vd6283tx_3_als_channel3_gain_n0_83 = 1,
  pnpl_vd6283tx_3_als_channel3_gain_n1 = 2,
  pnpl_vd6283tx_3_als_channel3_gain_n1_25 = 3,
  pnpl_vd6283tx_3_als_channel3_gain_n1_67 = 4,
  pnpl_vd6283tx_3_als_channel3_gain_n2_5 = 5,
  pnpl_vd6283tx_3_als_channel3_gain_n3_33 = 6,
  pnpl_vd6283tx_3_als_channel3_gain_n5 = 7,
  pnpl_vd6283tx_3_als_channel3_gain_n7_1 = 8,
  pnpl_vd6283tx_3_als_channel3_gain_n10 = 9,
  pnpl_vd6283tx_3_als_channel3_gain_n16 = 10,
  pnpl_vd6283tx_3_als_channel3_gain_n25 = 11,
  pnpl_vd6283tx_3_als_channel3_gain_n33 = 12,
  pnpl_vd6283tx_3_als_channel3_gain_n50 = 13,
  pnpl_vd6283tx_3_als_channel3_gain_n66_6 = 14,
} pnpl_vd6283tx_3_als_channel3_gain_t;
typedef enum
{
  pnpl_vd6283tx_3_als_channel4_gain_n0_71 = 0,
  pnpl_vd6283tx_3_als_channel4_gain_n0_83 = 1,
  pnpl_vd6283tx_3_als_channel4_gain_n1 = 2,
  pnpl_vd6283tx_3_als_channel4_gain_n1_25 = 3,
  pnpl_vd6283tx_3_als_channel4_gain_n1_67 = 4,
  pnpl_vd6283tx_3_als_channel4_gain_n2_5 = 5,
  pnpl_vd6283tx_3_als_channel4_gain_n3_33 = 6,
  pnpl_vd6283tx_3_als_channel4_gain_n5 = 7,
  pnpl_vd6283tx_3_als_channel4_gain_n7_1 = 8,
  pnpl_vd6283tx_3_als_channel4_gain_n10 = 9,
  pnpl_vd6283tx_3_als_channel4_gain_n16 = 10,
  pnpl_vd6283tx_3_als_channel4_gain_n25 = 11,
  pnpl_vd6283tx_3_als_channel4_gain_n33 = 12,
  pnpl_vd6283tx_3_als_channel4_gain_n50 = 13,
  pnpl_vd6283tx_3_als_channel4_gain_n66_6 = 14,
} pnpl_vd6283tx_3_als_channel4_gain_t;
typedef enum
{
  pnpl_vd6283tx_3_als_channel5_gain_n0_71 = 0,
  pnpl_vd6283tx_3_als_channel5_gain_n0_83 = 1,
  pnpl_vd6283tx_3_als_channel5_gain_n1 = 2,
  pnpl_vd6283tx_3_als_channel5_gain_n1_25 = 3,
  pnpl_vd6283tx_3_als_channel5_gain_n1_67 = 4,
  pnpl_vd6283tx_3_als_channel5_gain_n2_5 = 5,
  pnpl_vd6283tx_3_als_channel5_gain_n3_33 = 6,
  pnpl_vd6283tx_3_als_channel5_gain_n5 = 7,
  pnpl_vd6283tx_3_als_channel5_gain_n7_1 = 8,
  pnpl_vd6283tx_3_als_channel5_gain_n10 = 9,
  pnpl_vd6283tx_3_als_channel5_gain_n16 = 10,
  pnpl_vd6283tx_3_als_channel5_gain_n25 = 11,
  pnpl_vd6283tx_3_als_channel5_gain_n33 = 12,
  pnpl_vd6283tx_3_als_channel5_gain_n50 = 13,
  pnpl_vd6283tx_3_als_channel5_gain_n66_6 = 14,
} pnpl_vd6283tx_3_als_channel5_gain_t;
typedef enum
{
  pnpl_vd6283tx_3_als_channel6_gain_n0_71 = 0,
  pnpl_vd6283tx_3_als_channel6_gain_n0_83 = 1,
  pnpl_vd6283tx_3_als_channel6_gain_n1 = 2,
  pnpl_vd6283tx_3_als_channel6_gain_n1_25 = 3,
  pnpl_vd6283tx_3_als_channel6_gain_n1_67 = 4,
  pnpl_vd6283tx_3_als_channel6_gain_n2_5 = 5,
  pnpl_vd6283tx_3_als_channel6_gain_n3_33 = 6,
  pnpl_vd6283tx_3_als_channel6_gain_n5 = 7,
  pnpl_vd6283tx_3_als_channel6_gain_n7_1 = 8,
  pnpl_vd6283tx_3_als_channel6_gain_n10 = 9,
  pnpl_vd6283tx_3_als_channel6_gain_n16 = 10,
  pnpl_vd6283tx_3_als_channel6_gain_n25 = 11,
  pnpl_vd6283tx_3_als_channel6_gain_n33 = 12,
  pnpl_vd6283tx_3_als_channel6_gain_n50 = 13,
  pnpl_vd6283tx_3_als_channel6_gain_n66_6 = 14,
} pnpl_vd6283tx_3_als_channel6_gain_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "parson.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

/* VTBL Functions ------------------------------------------------------------*/
char *Vd6283tx_3_Als_PnPL_vtblGetKey(IPnPLComponent_t *_this);
uint8_t Vd6283tx_3_Als_PnPL_vtblGetNCommands(IPnPLComponent_t *_this);
char *Vd6283tx_3_Als_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
uint8_t Vd6283tx_3_Als_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                          uint8_t pretty);
uint8_t Vd6283tx_3_Als_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                            uint32_t *size, uint8_t pretty);
uint8_t Vd6283tx_3_Als_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                uint32_t *size, uint8_t pretty);

/**
  * Create a type name for _Vd6283tx_3_Als_PnPL.
 */
typedef struct _Vd6283tx_3_Als_PnPL Vd6283tx_3_Als_PnPL;

/* Public API declaration ----------------------------------------------------*/

IPnPLComponent_t *Vd6283tx_3_Als_PnPLAlloc(void);

/**
  * Initialize the default parameters.
  *
 */
uint8_t Vd6283tx_3_Als_PnPLInit(IPnPLComponent_t *_this);


/**
  * Get the IPnPLComponent interface for the component.
  * @param _this [IN] specifies a pointer to a PnPL component.
  * @return a pointer to the generic object ::IPnPLComponent if success,
  * or NULL if out of memory error occurs.
 */
IPnPLComponent_t *Vd6283tx_3_Als_PnPLGetComponentIF(Vd6283tx_3_Als_PnPL *_this);

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_VD6283TX_3_ALS_H_ */
