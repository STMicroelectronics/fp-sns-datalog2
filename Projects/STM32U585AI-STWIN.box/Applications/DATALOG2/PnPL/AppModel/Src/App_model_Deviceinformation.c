/**
  ******************************************************************************
  * @file    App_model_Deviceinformation.c
  * @author  SRA
  * @brief   Deviceinformation PnPL Components APIs
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
  * dtmi:azure:DeviceManagement:DeviceInformation;1
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#include "App_model.h"

/* USER includes -------------------------------------------------------------*/
#include "fx_stm32_sd_driver.h"

/* USER private function prototypes ------------------------------------------*/

/* USER defines --------------------------------------------------------------*/

/* Device Information PnPL Component -----------------------------------------*/
uint8_t DeviceInformation_comp_init(void)
{

  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *DeviceInformation_get_key(void)
{
  return "DeviceInformation";
}


uint8_t DeviceInformation_get_manufacturer(char **value)
{
  *value = "STMicroelectronics";
  return PNPL_NO_ERROR_CODE;
}

uint8_t DeviceInformation_get_model(char **value)
{
  *value = "STEVAL-STWINBX1";
  return PNPL_NO_ERROR_CODE;
}

uint8_t DeviceInformation_get_swVersion(char **value)
{
  *value = FW_VERSION_MAJOR "." FW_VERSION_MINOR "." FW_VERSION_PATCH;
  return PNPL_NO_ERROR_CODE;
}

uint8_t DeviceInformation_get_osName(char **value)
{
  *value = "AzureRTOS";
  return PNPL_NO_ERROR_CODE;
}

uint8_t DeviceInformation_get_processorArchitecture(char **value)
{
  *value = "ARM Cortex-M33";
  return PNPL_NO_ERROR_CODE;
}

uint8_t DeviceInformation_get_processorManufacturer(char **value)
{
  *value = "STMicroelectronics";
  return PNPL_NO_ERROR_CODE;
}

uint8_t DeviceInformation_get_totalStorage(float *value)
{
  *value = 0;
  if (SD_IsDetected())
  {
//    BSP_SD_CardInfo CardInfo;
    HAL_SD_CardInfoTypeDef CardInfo;
    SD_GetCardInfo(&CardInfo);
    *value = round(((float)CardInfo.BlockNbr * (float)CardInfo.BlockSize) / 1000000000.0f);
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t DeviceInformation_get_totalMemory(float *value)
{
  *value = (SRAM1_SIZE + SRAM2_SIZE + SRAM3_SIZE + SRAM4_SIZE) / 1024;
  return PNPL_NO_ERROR_CODE;
}




