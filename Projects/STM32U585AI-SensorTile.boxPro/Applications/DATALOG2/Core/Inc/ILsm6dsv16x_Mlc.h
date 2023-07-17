/**
  ******************************************************************************
  * @file    ILsm6dsv16x_Mlc.h
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:lsm6dsv16x_mlc;2
  *
  * Created by: DTDL2PnPL_cGen version 1.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#ifndef INCLUDE_ILSM6DSV16X_MLC_H_
#define INCLUDE_ILSM6DSV16X_MLC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

/**
  * Create  type name for ILsm6dsv16x_Mlc.
  */
typedef struct _ILsm6dsv16x_Mlc_t ILsm6dsv16x_Mlc_t;

/* Public API declarations ---------------------------------------------------*/

/* Public interface */
inline uint8_t ILsm6dsv16x_Mlc_load_file(ILsm6dsv16x_Mlc_t *_this, uint32_t size, const char *data);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ILSM6DSV16X_MLC_H_ */
