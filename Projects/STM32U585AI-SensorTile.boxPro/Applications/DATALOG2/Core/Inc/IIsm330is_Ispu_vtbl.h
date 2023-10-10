/**
  ******************************************************************************
  * @file    IIsm330is_Ispu_vtbl.h
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:ism330is_ispu;3
  *
  * Created by: DTDL2PnPL_cGen version 1.2.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#ifndef INCLUDE_IISM330IS_ISPU_VTBL_H_
#define INCLUDE_IISM330IS_ISPU_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
  * Create a type name for IIsm330is_Ispu_vtbl.
  */
typedef struct _IIsm330is_Ispu_vtbl IIsm330is_Ispu_vtbl;

struct _IIsm330is_Ispu_vtbl
{
  uint8_t (*ism330is_ispu_load_file)(IIsm330is_Ispu_t * _this, const char *ucf_data, uint32_t ucf_size, const char *output_data, uint32_t output_size);
};

struct _IIsm330is_Ispu_t
{
  /**
    * Pointer to the virtual table for the class.
    */
  const IIsm330is_Ispu_vtbl *vptr;
};

/* Inline functions definition -----------------------------------------------*/
inline uint8_t IIsm330is_Ispu_load_file(IIsm330is_Ispu_t *_this, const char *ucf_data, uint32_t ucf_size, const char *output_data, uint32_t output_size)
{
  return _this->vptr->ism330is_ispu_load_file(_this, ucf_data, ucf_size, output_data, output_size);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_IISM330IS_ISPU_VTBL_H_ */
