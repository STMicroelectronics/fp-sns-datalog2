/**
  ******************************************************************************
  * @file    IIsm330dhcx_Mlc_vtbl.h
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
  * dtmi:vespucci:nucleo_h7a3zi_q:x_nucleo_iks02a1:fpSnsDatalog2_datalog2:sensors:ism330dhcx_mlc;1
  *
  * Created by: DTDL2PnPL_cGen version 1.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#ifndef INCLUDE_IISM330DHCX_MLC_VTBL_H_
#define INCLUDE_IISM330DHCX_MLC_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
  * Create a type name for IIsm330dhcx_Mlc_vtbl.
  */
typedef struct _IIsm330dhcx_Mlc_vtbl IIsm330dhcx_Mlc_vtbl;

struct _IIsm330dhcx_Mlc_vtbl
{
  uint8_t (*ism330dhcx_mlc_load_file)(IIsm330dhcx_Mlc_t * _this, const char *data, uint32_t size);
};

struct _IIsm330dhcx_Mlc_t
{
  /**
    * Pointer to the virtual table for the class.
    */
  const IIsm330dhcx_Mlc_vtbl *vptr;
};

/* Inline functions definition -----------------------------------------------*/
inline uint8_t IIsm330dhcx_Mlc_load_file(IIsm330dhcx_Mlc_t *_this, const char *data, uint32_t size)
{
  return _this->vptr->ism330dhcx_mlc_load_file(_this, data, size);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_IISM330DHCX_MLC_VTBL_H_ */
