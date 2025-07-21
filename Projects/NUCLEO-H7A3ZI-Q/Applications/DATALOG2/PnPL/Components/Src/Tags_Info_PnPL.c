/**
  ******************************************************************************
  * @file    Tags_Info_PnPL.c
  * @author  SRA
  * @brief   Tags_Info PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:other:tags_info;1
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

#include "Tags_Info_PnPL.h"

static const IPnPLComponent_vtbl sTags_Info_PnPL_CompIF_vtbl =
{
  Tags_Info_PnPL_vtblGetKey,
  Tags_Info_PnPL_vtblGetNCommands,
  Tags_Info_PnPL_vtblGetCommandKey,
  Tags_Info_PnPL_vtblGetStatus,
  Tags_Info_PnPL_vtblSetProperty,
  Tags_Info_PnPL_vtblExecuteFunction
};

/**
  *  Tags_Info_PnPL internal structure.
  */
struct _Tags_Info_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Tags_Info_PnPL sTags_Info_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Tags_Info_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sTags_Info_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sTags_Info_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Tags_Info_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  tags_info_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Tags_Info_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return tags_info_get_key();
}

uint8_t Tags_Info_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Tags_Info_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Tags_Info_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  int32_t temp_i = 0;
  tags_info_get_max_tags_num(&temp_i);
  json_object_dotset_number(JSON_Status, "tags_info.max_tags_num", temp_i);
  char *temp_s = "";
  tags_info_get_sw_tag0__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.sw_tag0.label", temp_s);
  bool temp_b = 0;
  tags_info_get_sw_tag0__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag0.enabled", temp_b);
  tags_info_get_sw_tag0__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag0.status", temp_b);
  tags_info_get_sw_tag1__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.sw_tag1.label", temp_s);
  tags_info_get_sw_tag1__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag1.enabled", temp_b);
  tags_info_get_sw_tag1__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag1.status", temp_b);
  tags_info_get_sw_tag2__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.sw_tag2.label", temp_s);
  tags_info_get_sw_tag2__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag2.enabled", temp_b);
  tags_info_get_sw_tag2__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag2.status", temp_b);
  tags_info_get_sw_tag3__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.sw_tag3.label", temp_s);
  tags_info_get_sw_tag3__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag3.enabled", temp_b);
  tags_info_get_sw_tag3__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag3.status", temp_b);
  tags_info_get_sw_tag4__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.sw_tag4.label", temp_s);
  tags_info_get_sw_tag4__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag4.enabled", temp_b);
  tags_info_get_sw_tag4__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag4.status", temp_b);
  json_object_dotset_number(JSON_Status, "tags_info.c_type", COMP_TYPE_OTHER);

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

uint8_t Tags_Info_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size,
                                       uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  bool valid_property = false;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag0"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag0.label"))
    {
      const char *sw_tag0__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag0.label");
      valid_property = true;
      ret = tags_info_set_sw_tag0__label(sw_tag0__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", sw_tag0__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_sw_tag0__label;
        tags_info_get_sw_tag0__label(&old_sw_tag0__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sw_tag0__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag0"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag0.enabled"))
    {
      bool sw_tag0__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag0.enabled");
      valid_property = true;
      ret = tags_info_set_sw_tag0__enabled(sw_tag0__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag0__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag0__enabled;
        tags_info_get_sw_tag0__enabled(&old_sw_tag0__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag0__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag0"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag0.status"))
    {
      bool sw_tag0__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag0.status");
      valid_property = true;
      ret = tags_info_set_sw_tag0__status(sw_tag0__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag0__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag0__status;
        tags_info_get_sw_tag0__status(&old_sw_tag0__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag0__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag1"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag1.label"))
    {
      const char *sw_tag1__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag1.label");
      valid_property = true;
      ret = tags_info_set_sw_tag1__label(sw_tag1__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", sw_tag1__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_sw_tag1__label;
        tags_info_get_sw_tag1__label(&old_sw_tag1__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sw_tag1__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag1"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag1.enabled"))
    {
      bool sw_tag1__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag1.enabled");
      valid_property = true;
      ret = tags_info_set_sw_tag1__enabled(sw_tag1__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag1__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag1__enabled;
        tags_info_get_sw_tag1__enabled(&old_sw_tag1__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag1__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag1"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag1.status"))
    {
      bool sw_tag1__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag1.status");
      valid_property = true;
      ret = tags_info_set_sw_tag1__status(sw_tag1__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag1__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag1__status;
        tags_info_get_sw_tag1__status(&old_sw_tag1__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag1__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag2"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag2.label"))
    {
      const char *sw_tag2__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag2.label");
      valid_property = true;
      ret = tags_info_set_sw_tag2__label(sw_tag2__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", sw_tag2__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_sw_tag2__label;
        tags_info_get_sw_tag2__label(&old_sw_tag2__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sw_tag2__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag2"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag2.enabled"))
    {
      bool sw_tag2__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag2.enabled");
      valid_property = true;
      ret = tags_info_set_sw_tag2__enabled(sw_tag2__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag2__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag2__enabled;
        tags_info_get_sw_tag2__enabled(&old_sw_tag2__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag2__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag2"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag2.status"))
    {
      bool sw_tag2__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag2.status");
      valid_property = true;
      ret = tags_info_set_sw_tag2__status(sw_tag2__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag2__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag2__status;
        tags_info_get_sw_tag2__status(&old_sw_tag2__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag2__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag3"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag3.label"))
    {
      const char *sw_tag3__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag3.label");
      valid_property = true;
      ret = tags_info_set_sw_tag3__label(sw_tag3__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", sw_tag3__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_sw_tag3__label;
        tags_info_get_sw_tag3__label(&old_sw_tag3__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sw_tag3__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag3"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag3.enabled"))
    {
      bool sw_tag3__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag3.enabled");
      valid_property = true;
      ret = tags_info_set_sw_tag3__enabled(sw_tag3__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag3__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag3__enabled;
        tags_info_get_sw_tag3__enabled(&old_sw_tag3__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag3__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag3"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag3.status"))
    {
      bool sw_tag3__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag3.status");
      valid_property = true;
      ret = tags_info_set_sw_tag3__status(sw_tag3__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag3__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag3__status;
        tags_info_get_sw_tag3__status(&old_sw_tag3__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag3__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag4"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag4.label"))
    {
      const char *sw_tag4__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag4.label");
      valid_property = true;
      ret = tags_info_set_sw_tag4__label(sw_tag4__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", sw_tag4__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_sw_tag4__label;
        tags_info_get_sw_tag4__label(&old_sw_tag4__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sw_tag4__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag4"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag4.enabled"))
    {
      bool sw_tag4__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag4.enabled");
      valid_property = true;
      ret = tags_info_set_sw_tag4__enabled(sw_tag4__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag4__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag4__enabled;
        tags_info_get_sw_tag4__enabled(&old_sw_tag4__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag4__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag4"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag4.status"))
    {
      bool sw_tag4__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag4.status");
      valid_property = true;
      ret = tags_info_set_sw_tag4__status(sw_tag4__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag4__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag4__status;
        tags_info_get_sw_tag4__status(&old_sw_tag4__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag4__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
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
    char *log_message = "Invalid property for tags_info";
    PnPLCreateLogMessage(response, size, log_message, PNPL_LOG_ERROR);
    ret = PNPL_BASE_ERROR_CODE;
  }
  json_value_free(respJSON);
  return ret;
}


uint8_t Tags_Info_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                           uint32_t *size, uint8_t pretty)
{
  return PNPL_NO_COMMANDS_ERROR_CODE;
}

