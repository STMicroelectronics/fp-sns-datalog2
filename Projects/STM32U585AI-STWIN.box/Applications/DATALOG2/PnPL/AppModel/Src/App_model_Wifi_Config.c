/**
  ******************************************************************************
  * @file    App_model_Wifi_Config.c
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

#include "App_model.h"

/* USER includes -------------------------------------------------------------*/
#include "app_netxduo.h"
#include "DatalogAppTask.h"

/* USER private function prototypes ------------------------------------------*/

/* USER defines --------------------------------------------------------------*/

/* Wi-Fi Configuration PnPL Component ----------------------------------------*/
extern AppModel_t app_model;

uint8_t wifi_config_comp_init(void)
{
  app_model.wifi_config_model.comp_name = wifi_config_get_key();

  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *wifi_config_get_key(void)
{
  return "wifi_config";
}


uint8_t wifi_config_get_ssid(char **value)
{
  *value = wifi_ssid;
  return PNPL_NO_ERROR_CODE;
}

uint8_t wifi_config_get_ip(char **value)
{
  extern char ip_str[];
  *value = ip_str;
  return PNPL_NO_ERROR_CODE;
}

uint8_t wifi_config_get_ftp_username(char **value)
{
  *value = ftp_username;
  return PNPL_NO_ERROR_CODE;
}


uint8_t wifi_config_set_ssid(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  if (value && (strlen(value) < SSID_MAX_LENGTH))
  {
    strcpy(wifi_ssid, value);
  }
  else
  {
    if (response_message != NULL)
    {
      char response_buffer[60]; // Make sure this buffer is large enough to hold the entire message
      snprintf(response_buffer, sizeof(response_buffer), "Max number of characters (%d) exceeded for the SSID",
               SSID_MAX_LENGTH);
      *response_message = response_buffer;
    }
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t wifi_config_set_ftp_username(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  if (value && (strlen(value) < NX_FTP_USERNAME_SIZE))
  {
    strcpy(ftp_username, value);
  }
  else
  {
    if (response_message != NULL)
    {
      char response_buffer[60]; // Make sure this buffer is large enough to hold the entire message
      snprintf(response_buffer, sizeof(response_buffer), "Max number of character (%d) exceeded for the FTP username",
               NX_FTP_USERNAME_SIZE);
      *response_message = response_buffer;
    }
  }
  return PNPL_NO_ERROR_CODE;
}


uint8_t wifi_config_wifi_connect(const char *password)
{
  uint8_t ret = TX_SUCCESS;

  if (password && (strlen(password) < PASSW_MAX_LENGTH))
  {
    DatalogAppTask_sd_stream_enable();
    IWifi_Config_wifi_connect(AppNetXDuo_GetIWifi_ConfigIF(), password);
  }
  /* Wait for connection command execution */
  if (TX_SUCCESS != netx_app_wait_command_execution())
  {
    ret = TX_WAIT_ERROR;
  }

  return ret;
}

uint8_t wifi_config_wifi_disconnect(void)
{
  uint8_t ret = TX_SUCCESS;
  IWifi_Config_wifi_disconnect(AppNetXDuo_GetIWifi_ConfigIF());
  DatalogAppTask_sd_stream_disable();

  /* Wait for connection command execution */
  if (TX_SUCCESS != netx_app_wait_command_execution())
  {
    ret = TX_WAIT_ERROR;
  }

  return ret;
}

uint8_t wifi_config_set_ftp_credentials(const char *password)
{
  IWifi_Config_set_ftp_credentials(AppNetXDuo_GetIWifi_ConfigIF(), password);
  return PNPL_NO_ERROR_CODE;
}

