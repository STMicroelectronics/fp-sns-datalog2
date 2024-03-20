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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:other:wifi_config;1
  *
  * Created by: DTDL2PnPL_cGen version 1.2.3
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
#include "Wifi_Config_PnPL_vtbl.h"

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
  /* Contains Wifi_Config functions pointers. */
  IWifi_Config_t *cmdIF;
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

uint8_t Wifi_Config_PnPLInit(IPnPLComponent_t *_this,  IWifi_Config_t *inf)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  Wifi_Config_PnPL *p_if_owner = (Wifi_Config_PnPL *) _this;
  p_if_owner->cmdIF = inf;
  wifi_config_comp_init();
  return 0;
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
  return 0;
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

  return 0;
}

uint8_t Wifi_Config_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size,
                                         uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = 0;
  if (json_object_dothas_value(tempJSONObject, "wifi_config.ssid"))
  {
    const char *ssid = json_object_dotget_string(tempJSONObject, "wifi_config.ssid");
    ret = wifi_config_set_ssid(ssid);
    if (ret == 0)
    {
      json_object_dotset_string(respJSONObject, "wifi_config.ssid.value", ssid);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "wifi_config.ssid.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "wifi_config.ftp_username"))
  {
    const char *ftp_username = json_object_dotget_string(tempJSONObject, "wifi_config.ftp_username");
    ret = wifi_config_set_ftp_username(ftp_username);
    if (ret == 0)
    {
      json_object_dotset_string(respJSONObject, "wifi_config.ftp_username.value", ftp_username);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "wifi_config.ftp_username.value", "PNPL_SET_ERROR");
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
  Wifi_Config_PnPL *p_if_owner = (Wifi_Config_PnPL *) _this;
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = 0;
  if (json_object_dothas_value(tempJSONObject, "wifi_config*wifi_connect.password"))
  {
    const char *password = json_object_dotget_string(tempJSONObject, "wifi_config*wifi_connect.password");
    ret = wifi_config_wifi_connect(p_if_owner->cmdIF, password);
    if (ret == 0)
    {
      json_object_dotset_string(respJSONObject, "wifi_config*wifi_connect.password.response", "PNPL_CMD_OK");
    }
    else
    {
      json_object_dotset_string(respJSONObject, "wifi_config.wifi_connect.password.response", "PNPL_CMD_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "wifi_config*wifi_disconnect"))
  {
    ret = wifi_config_wifi_disconnect(p_if_owner->cmdIF);
    if (ret == 0)
    {
      json_object_dotset_string(respJSONObject, "wifi_config*wifi_disconnect.response", "PNPL_CMD_OK");
    }
    else
    {
      json_object_dotset_string(respJSONObject, "wifi_config.wifi_disconnect.response", "PNPL_CMD_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "wifi_config*set_ftp_credentials.password"))
  {
    const char *password = json_object_dotget_string(tempJSONObject, "wifi_config*set_ftp_credentials.password");
    ret = wifi_config_set_ftp_credentials(p_if_owner->cmdIF, password);
    if (ret == 0)
    {
      json_object_dotset_string(respJSONObject, "wifi_config*set_ftp_credentials.password.response", "PNPL_CMD_OK");
    }
    else
    {
      json_object_dotset_string(respJSONObject, "wifi_config.set_ftp_credentials.password.response", "PNPL_CMD_ERROR");
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

