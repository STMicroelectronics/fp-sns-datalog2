/**
  ******************************************************************************
  * @file    ILsm6dsv16x_Mlc_vtbl.h
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

#ifndef INCLUDE_ILSM6DSV16X_MLC_VTBL_H_
#define INCLUDE_ILSM6DSV16X_MLC_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
  * Create a type name for ILsm6dsv16x_Mlc_vtbl.
  */
typedef struct _ILsm6dsv16x_Mlc_vtbl ILsm6dsv16x_Mlc_vtbl;

struct _ILsm6dsv16x_Mlc_vtbl
{
  uint8_t (*lsm6dsv16x_mlc_load_file)(ILsm6dsv16x_Mlc_t * _this, uint32_t size, const char *data);
};

struct _ILsm6dsv16x_Mlc_t
{
  /**
    * Pointer to the virtual table for the class.
    */
  const ILsm6dsv16x_Mlc_vtbl *vptr;
};

/* Inline functions definition -----------------------------------------------*/
inline uint8_t ILsm6dsv16x_Mlc_load_file(ILsm6dsv16x_Mlc_t *_this, uint32_t size, const char *data)
{
  return _this->vptr->lsm6dsv16x_mlc_load_file(_this, size, data);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ILSM6DSV16X_MLC_VTBL_H_ */
