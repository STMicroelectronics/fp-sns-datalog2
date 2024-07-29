/**
  ******************************************************************************
  * @file    Wifi_Config_PnPL.c
  * @author  SRA
  * @brief   Wifi_Config PnPL Component Manager
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

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "App_model.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"
#include "PnPLCompManager.h"

#include "Wifi_Config_PnPL.h"

static const IPnPLComponent_vtbl sWifi_Config_PnPL_CompIF_vtbl =
{
  Wifi_Config_PnPL_vtblGetKey,
  Wifi_Config_PnPL_vtblGetNCommands,
  Wifi_Config_PnPL_vtblGetCommandKey,
  Wifi_Config_PnPL_vtblGetStatus,
  Wifi_Config_PnPL_vtblSetProperty,
  Wifi_Config_PnPL_vtblExecuteFunction
};

/**
  *  Wifi_Config_PnPL internal structure.
  */
struct _Wifi_Config_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Wifi_Config_PnPL sWifi_Config_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Wifi_Config_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sWifi_Config_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sWifi_Config_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Wifi_Config_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  wifi_config_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Wifi_Config_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return wifi_config_get_key();
}

uint8_t Wifi_Config_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 3;
}

char *Wifi_Config_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  switch (id)
  {
    case 0:
      return "wifi_config*wifi_connect";
      break;
    case 1:
      return "wifi_config*wifi_disconnect";
      break;
    case 2:
      return "wifi_config*set_ftp_credentials";
      break;
  }
  return "";
}

uint8_t Wifi_Config_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  char *temp_s = "";
  wifi_config_get_ssid(&temp_s);
  json_object_dotset_string(JSON_Status, "wifi_config.ssid", temp_s);
  wifi_config_get_ip(&temp_s);
  json_object_dotset_string(JSON_Status, "wifi_config.ip", temp_s);
  wifi_config_get_ftp_username(&temp_s);
  json_object_dotset_string(JSON_Status, "wifi_config.ftp_username", temp_s);
  json_object_dotset_number(JSON_Status, "wifi_config.c_type", COMP_TYPE_OTHER);

  if (pretty == 1)
  {
    *serializedJSON = json_serialize_to_string_pretty(tempJSON);
    *size = json_serialization_size_pretty(tempJSON);
  }
  else
  {
    *serializedJSON = json_serialize_to_string(tempJSON);
    *size = json_serialization_size(tempJSON);
  }

  /* No need to free temp_j as it is part of tempJSON */
  json_value_free(tempJSON);

  return PNPL_NO_ERROR_CODE;
}

uint8_t Wifi_Config_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size,
                                         uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "wifi_config.ssid"))
  {
    const char *ssid = json_object_dotget_string(tempJSONObject, "wifi_config.ssid");
    ret = wifi_config_set_ssid(ssid, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", ssid);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_ssid;
      wifi_config_get_ssid(&old_ssid);
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_ssid);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "wifi_config.ftp_username"))
  {
    const char *ftp_username = json_object_dotget_string(tempJSONObject, "wifi_config.ftp_username");
    ret = wifi_config_set_ftp_username(ftp_username, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", ftp_username);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_ftp_username;
      wifi_config_get_ftp_username(&old_ftp_username);
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_ftp_username);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  json_value_free(tempJSON);
  if (pretty == 1)
  {
    *response = json_serialize_to_string_pretty(respJSON);
    *size = json_serialization_size_pretty(respJSON);
  }
  else
  {
    *response = json_serialize_to_string(respJSON);
    *size = json_serialization_size(respJSON);
  }
  json_value_free(respJSON);
  return ret;
}


uint8_t Wifi_Config_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                             uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (json_object_dothas_value(tempJSONObject, "wifi_config*wifi_connect.password"))
  {
    const char *password = json_object_dotget_string(tempJSONObject, "wifi_config*wifi_connect.password");
    ret = wifi_config_wifi_connect(password);
  }
  if (json_object_dothas_value(tempJSONObject, "wifi_config*wifi_disconnect"))
  {
    ret = wifi_config_wifi_disconnect();
  }
  if (json_object_dothas_value(tempJSONObject, "wifi_config*set_ftp_credentials.password"))
  {
    const char *password = json_object_dotget_string(tempJSONObject, "wifi_config*set_ftp_credentials.password");
    ret = wifi_config_set_ftp_credentials(password);
  }
  json_value_free(tempJSON);
  return ret;
}

