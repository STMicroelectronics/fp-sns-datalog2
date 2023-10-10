/**
  ******************************************************************************
  * @file    PnPL_Init.h
  * @author  SRA
  * @brief   PnPL Components initialization functions
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
  * This file has been auto generated from the following Device Template Model:
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2;3
  *
  * Created by: DTDL2PnPL_cGen version 1.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#ifndef PNPL_INIT_H_
#define PNPL_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "PnPLCompManager.h"
#include "Lis2mdl_Mag_PnPL.h"
#include "Lis2du12_Acc_PnPL.h"
#include "Lsm6dsv16x_Acc_PnPL.h"
#include "Lsm6dsv16x_Gyro_PnPL.h"
#include "Lsm6dsv16x_Mlc_PnPL.h"
#include "Mp23db01hp_Mic_PnPL.h"
#include "Stts22h_Temp_PnPL.h"
#include "Lps22df_Press_PnPL.h"
#include "Ism330is_Acc_PnPL.h"
#include "Ism330is_Gyro_PnPL.h"
#include "Ism330is_Ispu_PnPL.h"
#include "Log_Controller_PnPL.h"
#include "Tags_Info_PnPL.h"
#include "Acquisition_Info_PnPL.h"
#include "Firmware_Info_PnPL.h"
#include "Deviceinformation_PnPL.h"
#include "Automode_PnPL.h"

#include "ILsm6dsv16x_Mlc.h"
#include "ILsm6dsv16x_Mlc_vtbl.h"
#include "IIsm330is_Ispu.h"
#include "IIsm330is_Ispu_vtbl.h"
#include "ILog_Controller.h"
#include "ILog_Controller_vtbl.h"


uint8_t PnPL_Components_Alloc();
uint8_t PnPL_Components_Init(ILsm6dsv16x_Mlc_t iLsm6dsv16x_Mlc, IIsm330is_Ispu_t iIsm330is_Ispu, ILog_Controller_t iLog_Controller);


#ifdef __cplusplus
}
#endif

#endif /* PNPL_INIT_H_ */