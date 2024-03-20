/**
  ******************************************************************************
  * @file    IWifi_Config.c
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:other:wifi_config;1
  *
  * Created by: DTDL2PnPL_cGen version 1.2.3
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#include "IWifi_Config.h"
#include "IWifi_Config_vtbl.h"

/**
  * GCC requires one function forward declaration in only one .c source
  * in order to manage the inline.
  * See also http://stackoverflow.com/questions/26503235/c-inline-function-and-gcc
  */
#if defined (__GNUC__) || defined(__ICCARM__)
extern uint8_t IWifi_Config_wifi_connect(IWifi_Config_t *_this, const char *password);
extern uint8_t IWifi_Config_wifi_disconnect(IWifi_Config_t *_this);
extern uint8_t IWifi_Config_set_ftp_credentials(IWifi_Config_t *_this, const char *password);
#endif
