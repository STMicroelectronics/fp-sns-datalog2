/**
  ******************************************************************************
  * @file    IIsm330dhcx_Mlc.h
  * @author  SRA
  * @brief
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
  * dtmi:vespucci:steval_stwinkt1b:fpSnsDatalog2_datalog2:sensors:ism330dhcx_mlc;1
  *
  * Created by: DTDL2PnPL_cGen version 1.2.3
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#ifndef INCLUDE_IISM330DHCX_MLC_H_
#define INCLUDE_IISM330DHCX_MLC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

/**
  * Create  type name for IIsm330dhcx_Mlc.
  */
typedef struct _IIsm330dhcx_Mlc_t IIsm330dhcx_Mlc_t;

/* Public API declarations ---------------------------------------------------*/

/* Public interface */
inline uint8_t IIsm330dhcx_Mlc_load_file(IIsm330dhcx_Mlc_t *_this, const char *data, int32_t size);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_IISM330DHCX_MLC_H_ */
