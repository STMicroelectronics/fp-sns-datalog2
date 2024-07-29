/**
  ******************************************************************************
  * @file    App_model_Wifi_Config.h
  * @author  SRA
  * @brief   Wifi_Config PnPL Components APIs
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
  * dtmi:vespucci:other:wifi_config;1
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/**
  ******************************************************************************
  * Component APIs *************************************************************
  * - Component init function
  *    <comp_name>_comp_init(void)
  * - Component get_key function
  *    <comp_name>_get_key(void)
  * - Component GET/SET Properties APIs ****************************************
  *  - GET Functions
  *    uint8_t <comp_name>_get_<prop_name>(prop_type *value)
  *      if prop_type == char --> (char **value)
  *  - SET Functions
  *    uint8_t <comp_name>_set_<prop_name>(prop_type value)
  *      if prop_type == char --> (char *value)
  *  - Component COMMAND Reaction Functions
  *      uint8_t <comp_name>_<command_name>(
  *                     field1_type field1_name, field2_type field2_name, ...,
  *                     fieldN_type fieldN_name);
  *  - Component TELEMETRY Send Functions
  *      uint8_t <comp_name>_create_telemetry(tel1_type tel1_name,
  *                     tel2_type tel2_name, ..., telN_type telN_name,
  *                     char **telemetry, uint32_t *size)
  ******************************************************************************
  */

#ifndef APP_MODEL_WIFI_CONFIG_H_
#define APP_MODEL_WIFI_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* USER includes -------------------------------------------------------------*/

typedef struct _WifiConfigModel_t
{
  char *comp_name;
  /* WifiConfig Component Model USER code */
} WifiConfigModel_t;

/* Wi-Fi Configuration PnPL Component ----------------------------------------- */
uint8_t wifi_config_comp_init(void);
char *wifi_config_get_key(void);
uint8_t wifi_config_get_ssid(char **value);
uint8_t wifi_config_get_ip(char **value);
uint8_t wifi_config_get_ftp_username(char **value);
uint8_t wifi_config_set_ssid(const char *value, char **response_message);
uint8_t wifi_config_set_ftp_username(const char *value, char **response_message);
uint8_t wifi_config_wifi_connect(const char *password);
uint8_t wifi_config_wifi_disconnect(void);
uint8_t wifi_config_set_ftp_credentials(const char *password);


#ifdef __cplusplus
}
#endif

#endif /* APP_MODEL_WIFI_CONFIG_H_ */
