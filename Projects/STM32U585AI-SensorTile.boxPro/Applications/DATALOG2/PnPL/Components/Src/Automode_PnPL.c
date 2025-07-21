/**
  ******************************************************************************
  * @file    Automode_PnPL.c
  * @author  SRA
  * @brief   Automode PnPL Component Manager
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
  * dtmi:vespucci:other:automode;3
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

#include "Automode_PnPL.h"

static const IPnPLComponent_vtbl sAutomode_PnPL_CompIF_vtbl =
{
  Automode_PnPL_vtblGetKey,
  Automode_PnPL_vtblGetNCommands,
  Automode_PnPL_vtblGetCommandKey,
  Automode_PnPL_vtblGetStatus,
  Automode_PnPL_vtblSetProperty,
  Automode_PnPL_vtblExecuteFunction
};

/**
  *  Automode_PnPL internal structure.
  */
struct _Automode_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Automode_PnPL sAutomode_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Automode_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sAutomode_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sAutomode_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Automode_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  automode_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Automode_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return automode_get_key();
}

uint8_t Automode_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Automode_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Automode_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  bool temp_b = 0;
  automode_get_enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "automode.enabled", temp_b);
  int32_t temp_i = 0;
  automode_get_nof_acquisitions(&temp_i);
  json_object_dotset_number(JSON_Status, "automode.nof_acquisitions", temp_i);
  automode_get_start_delay_s(&temp_i);
  json_object_dotset_number(JSON_Status, "automode.start_delay_s", temp_i);
  automode_get_logging_period_s(&temp_i);
  json_object_dotset_number(JSON_Status, "automode.logging_period_s", temp_i);
  automode_get_idle_period_s(&temp_i);
  json_object_dotset_number(JSON_Status, "automode.idle_period_s", temp_i);
  json_object_dotset_number(JSON_Status, "automode.c_type", COMP_TYPE_OTHER);

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

uint8_t Automode_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size,
                                      uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  bool valid_property = false;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "automode.enabled"))
  {
    bool enabled = json_object_dotget_boolean(tempJSONObject, "automode.enabled");
    valid_property = true;
    ret = automode_set_enabled(enabled, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", enabled);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_enabled;
      automode_get_enabled(&old_enabled);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_enabled);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "automode.nof_acquisitions"))
  {
    int32_t nof_acquisitions = (int32_t)json_object_dotget_number(tempJSONObject, "automode.nof_acquisitions");
    valid_property = true;
    ret = automode_set_nof_acquisitions(nof_acquisitions, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", nof_acquisitions);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_nof_acquisitions;
      automode_get_nof_acquisitions(&old_nof_acquisitions);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_nof_acquisitions);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "automode.start_delay_s"))
  {
    int32_t start_delay_s = (int32_t)json_object_dotget_number(tempJSONObject, "automode.start_delay_s");
    valid_property = true;
    ret = automode_set_start_delay_s(start_delay_s, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", start_delay_s);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_start_delay_s;
      automode_get_start_delay_s(&old_start_delay_s);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_start_delay_s);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "automode.logging_period_s"))
  {
    int32_t logging_period_s = (int32_t)json_object_dotget_number(tempJSONObject, "automode.logging_period_s");
    valid_property = true;
    ret = automode_set_logging_period_s(logging_period_s, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", logging_period_s);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_logging_period_s;
      automode_get_logging_period_s(&old_logging_period_s);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_logging_period_s);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "automode.idle_period_s"))
  {
    int32_t idle_period_s = (int32_t)json_object_dotget_number(tempJSONObject, "automode.idle_period_s");
    valid_property = true;
    ret = automode_set_idle_period_s(idle_period_s, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", idle_period_s);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_idle_period_s;
      automode_get_idle_period_s(&old_idle_period_s);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_idle_period_s);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  json_value_free(tempJSON);
  /* Check if received a valid request to modify an existing property */
  if (valid_property)
  {
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
  }
  else
  {
    /* Set property is not containing a valid property/parameter: PnPL_Error */
    char *log_message = "Invalid property for automode";
    PnPLCreateLogMessage(response, size, log_message, PNPL_LOG_ERROR);
    ret = PNPL_BASE_ERROR_CODE;
  }
  json_value_free(respJSON);
  return ret;
}


uint8_t Automode_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                          uint32_t *size, uint8_t pretty)
{
  return PNPL_NO_COMMANDS_ERROR_CODE;
}

