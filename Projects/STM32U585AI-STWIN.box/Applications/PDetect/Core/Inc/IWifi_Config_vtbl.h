/**
  ******************************************************************************
  * @file    IWifi_Config_vtbl.h
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
  * dtmi:stdev:other:wifi_config;1
  *
  * Created by: DTDL2PnPL_cGen version 1.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#ifndef INCLUDE_IWIFI_CONFIG_VTBL_H_
#define INCLUDE_IWIFI_CONFIG_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
  * Create a type name for IWifi_Config_vtbl.
  */
typedef struct _IWifi_Config_vtbl IWifi_Config_vtbl;

struct _IWifi_Config_vtbl
{
  uint8_t (*wifi_config_wifi_connect)(IWifi_Config_t *_this, const char *password);
  uint8_t (*wifi_config_wifi_disconnect)(IWifi_Config_t *_this);
  uint8_t (*wifi_config_set_ftp_credentials)(IWifi_Config_t *_this, const char *password);
};

struct _IWifi_Config_t
{
  /**
    * Pointer to the virtual table for the class.
    */
  const IWifi_Config_vtbl *vptr;
};

/* Inline functions definition -----------------------------------------------*/
inline uint8_t IWifi_Config_wifi_connect(IWifi_Config_t *_this, const char *password)
{
  return _this->vptr->wifi_config_wifi_connect(_this, password);
}
inline uint8_t IWifi_Config_wifi_disconnect(IWifi_Config_t *_this)
{
  return _this->vptr->wifi_config_wifi_disconnect(_this);
}
inline uint8_t IWifi_Config_set_ftp_credentials(IWifi_Config_t *_this, const char *password)
{
  return _this->vptr->wifi_config_set_ftp_credentials(_this, password);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_IWIFI_CONFIG_VTBL_H_ */
