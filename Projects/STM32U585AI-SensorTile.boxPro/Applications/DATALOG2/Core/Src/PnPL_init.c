/**
  ******************************************************************************
  * @file    PnPL_Init.c
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

#include "PnPL_Init.h"

static IPnPLComponent_t *pLis2mdl_Mag_PnPLObj = NULL;
static IPnPLComponent_t *pLis2du12_Acc_PnPLObj = NULL;
static IPnPLComponent_t *pLsm6dsv16x_Acc_PnPLObj = NULL;
static IPnPLComponent_t *pLsm6dsv16x_Gyro_PnPLObj = NULL;
static IPnPLComponent_t *pLsm6dsv16x_Mlc_PnPLObj = NULL;
static IPnPLComponent_t *pMp23db01hp_Mic_PnPLObj = NULL;
static IPnPLComponent_t *pStts22h_Temp_PnPLObj = NULL;
static IPnPLComponent_t *pLps22df_Press_PnPLObj = NULL;
static IPnPLComponent_t *pIsm330is_Acc_PnPLObj = NULL;
static IPnPLComponent_t *pIsm330is_Gyro_PnPLObj = NULL;
static IPnPLComponent_t *pIsm330is_Ispu_PnPLObj = NULL;
static IPnPLComponent_t *pLog_Controller_PnPLObj = NULL;
static IPnPLComponent_t *pTags_Info_PnPLObj = NULL;
static IPnPLComponent_t *pAcquisition_Info_PnPLObj = NULL;
static IPnPLComponent_t *pFirmware_Info_PnPLObj = NULL;
static IPnPLComponent_t *pDeviceinformation_PnPLObj = NULL;
static IPnPLComponent_t *pAutomode_PnPLObj = NULL;

uint8_t PnPL_Components_Alloc()
{
  /* PnPL Components Allocation */
  pLis2mdl_Mag_PnPLObj = Lis2mdl_Mag_PnPLAlloc();
  pLis2du12_Acc_PnPLObj = Lis2du12_Acc_PnPLAlloc();
  pLsm6dsv16x_Acc_PnPLObj = Lsm6dsv16x_Acc_PnPLAlloc();
  pLsm6dsv16x_Gyro_PnPLObj = Lsm6dsv16x_Gyro_PnPLAlloc();
  pLsm6dsv16x_Mlc_PnPLObj = Lsm6dsv16x_Mlc_PnPLAlloc();
  pMp23db01hp_Mic_PnPLObj = Mp23db01hp_Mic_PnPLAlloc();
  pStts22h_Temp_PnPLObj = Stts22h_Temp_PnPLAlloc();
  pLps22df_Press_PnPLObj = Lps22df_Press_PnPLAlloc();
  pIsm330is_Acc_PnPLObj = Ism330is_Acc_PnPLAlloc();
  pIsm330is_Gyro_PnPLObj = Ism330is_Gyro_PnPLAlloc();
  pIsm330is_Ispu_PnPLObj = Ism330is_Ispu_PnPLAlloc();
  pLog_Controller_PnPLObj = Log_Controller_PnPLAlloc();
  pTags_Info_PnPLObj = Tags_Info_PnPLAlloc();
  pAcquisition_Info_PnPLObj = Acquisition_Info_PnPLAlloc();
  pFirmware_Info_PnPLObj = Firmware_Info_PnPLAlloc();
  pDeviceinformation_PnPLObj = Deviceinformation_PnPLAlloc();
  pAutomode_PnPLObj = Automode_PnPLAlloc();
  return 0;
}

uint8_t PnPL_Components_Init(ILsm6dsv16x_Mlc_t iLsm6dsv16x_Mlc, IIsm330is_Ispu_t iIsm330is_Ispu, ILog_Controller_t iLog_Controller)
{
  /* Init&Add PnPL Components */
  Lis2mdl_Mag_PnPLInit(pLis2mdl_Mag_PnPLObj);
  Lis2du12_Acc_PnPLInit(pLis2du12_Acc_PnPLObj);
  Lsm6dsv16x_Acc_PnPLInit(pLsm6dsv16x_Acc_PnPLObj);
  Lsm6dsv16x_Gyro_PnPLInit(pLsm6dsv16x_Gyro_PnPLObj);
  Lsm6dsv16x_Mlc_PnPLInit(pLsm6dsv16x_Mlc_PnPLObj, &iLsm6dsv16x_Mlc);
  Mp23db01hp_Mic_PnPLInit(pMp23db01hp_Mic_PnPLObj);
  Stts22h_Temp_PnPLInit(pStts22h_Temp_PnPLObj);
  Lps22df_Press_PnPLInit(pLps22df_Press_PnPLObj);
  Ism330is_Acc_PnPLInit(pIsm330is_Acc_PnPLObj);
  Ism330is_Gyro_PnPLInit(pIsm330is_Gyro_PnPLObj);
  Ism330is_Ispu_PnPLInit(pIsm330is_Ispu_PnPLObj, &iIsm330is_Ispu);
  Log_Controller_PnPLInit(pLog_Controller_PnPLObj, &iLog_Controller);
  Tags_Info_PnPLInit(pTags_Info_PnPLObj);
  Acquisition_Info_PnPLInit(pAcquisition_Info_PnPLObj);
  Firmware_Info_PnPLInit(pFirmware_Info_PnPLObj);
  Deviceinformation_PnPLInit(pDeviceinformation_PnPLObj);
  Automode_PnPLInit(pAutomode_PnPLObj);
  return 0;
}