/**
  ******************************************************************************
  * @file    IIsm330is_Ispu.h
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
  * dtmi:appconfig:steval_stwinbx1:fp_sns_datalog2_ispu:sensors:ism330is_ispu;1
  *
  * Created by: DTDL2PnPL_cGen version 1.0.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#ifndef INCLUDE_IISM330IS_ISPU_H_
#define INCLUDE_IISM330IS_ISPU_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

/**
  * Create  type name for IIsm330is_Ispu.
  */
typedef struct _IIsm330is_Ispu_t IIsm330is_Ispu_t;

/* Public API declarations ---------------------------------------------------*/

/* Public interface */
inline uint8_t IIsm330is_Ispu_load_file(IIsm330is_Ispu_t *_this, const char *ucf_data, uint32_t ucf_size, const char *output_data, uint32_t output_size);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_IISM330IS_ISPU_H_ */
