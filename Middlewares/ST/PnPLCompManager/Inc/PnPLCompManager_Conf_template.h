/**
  ******************************************************************************
  * @file    PnPLCompManager_Conf.h
  * @author  SRA
  * @brief   PnPL Components Manager configuration template file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/**
  ******************************************************************************
  * This file has been auto generated from the following Device Template Model:
  * dtmi:stdev:steval_stwinbx1:fp_motor_control2;2
  *
  * Created by: DTDL2PnPL_cGen version 1.0.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PNPL_COMP_MANAGER_CONF_H__
#define __PNPL_COMP_MANAGER_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* include the correct STM32 family */
#include "stm32xxxx_hal.h"
#include "services/sysmem.h"

/****************** Malloc/Free **************************/
#define pnpl_malloc SysAlloc
#define pnpl_free SysFree

/****************** PnPL Responses ***********************/
#define PNPL_RESPONSES

/* define the maximum length of a key, default is 32 */
//#define COMP_KEY_MAX_LENGTH     xx

#ifdef __cplusplus
}
#endif

#endif /* __PNPL_COMP_MANAGER_CONF_H__*/
