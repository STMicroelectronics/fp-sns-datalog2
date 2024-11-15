/**
  ******************************************************************************
  * @file    Tags_Info_PnPL.c
  * @author  SRA
  * @brief   Tags_Info PnPL Component Manager
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
  * dtmi:vespucci:other:tags_info;2
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
  tags_info_get_sw_tag5__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.sw_tag5.label", temp_s);
  tags_info_get_sw_tag5__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag5.enabled", temp_b);
  tags_info_get_sw_tag5__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag5.status", temp_b);
  tags_info_get_sw_tag6__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.sw_tag6.label", temp_s);
  tags_info_get_sw_tag6__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag6.enabled", temp_b);
  tags_info_get_sw_tag6__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag6.status", temp_b);
  tags_info_get_sw_tag7__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.sw_tag7.label", temp_s);
  tags_info_get_sw_tag7__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag7.enabled", temp_b);
  tags_info_get_sw_tag7__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag7.status", temp_b);
  tags_info_get_sw_tag8__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.sw_tag8.label", temp_s);
  tags_info_get_sw_tag8__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag8.enabled", temp_b);
  tags_info_get_sw_tag8__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag8.status", temp_b);
  tags_info_get_sw_tag9__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.sw_tag9.label", temp_s);
  tags_info_get_sw_tag9__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag9.enabled", temp_b);
  tags_info_get_sw_tag9__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag9.status", temp_b);
  tags_info_get_sw_tag10__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.sw_tag10.label", temp_s);
  tags_info_get_sw_tag10__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag10.enabled", temp_b);
  tags_info_get_sw_tag10__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag10.status", temp_b);
  tags_info_get_sw_tag11__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.sw_tag11.label", temp_s);
  tags_info_get_sw_tag11__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag11.enabled", temp_b);
  tags_info_get_sw_tag11__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag11.status", temp_b);
  tags_info_get_sw_tag12__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.sw_tag12.label", temp_s);
  tags_info_get_sw_tag12__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag12.enabled", temp_b);
  tags_info_get_sw_tag12__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag12.status", temp_b);
  tags_info_get_sw_tag13__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.sw_tag13.label", temp_s);
  tags_info_get_sw_tag13__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag13.enabled", temp_b);
  tags_info_get_sw_tag13__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag13.status", temp_b);
  tags_info_get_sw_tag14__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.sw_tag14.label", temp_s);
  tags_info_get_sw_tag14__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag14.enabled", temp_b);
  tags_info_get_sw_tag14__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag14.status", temp_b);
  tags_info_get_sw_tag15__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.sw_tag15.label", temp_s);
  tags_info_get_sw_tag15__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag15.enabled", temp_b);
  tags_info_get_sw_tag15__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.sw_tag15.status", temp_b);
  tags_info_get_hw_tag0__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.hw_tag0.label", temp_s);
  tags_info_get_hw_tag0__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.hw_tag0.enabled", temp_b);
  tags_info_get_hw_tag0__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.hw_tag0.status", temp_b);
  tags_info_get_hw_tag1__label(&temp_s);
  json_object_dotset_string(JSON_Status, "tags_info.hw_tag1.label", temp_s);
  tags_info_get_hw_tag1__enabled(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.hw_tag1.enabled", temp_b);
  tags_info_get_hw_tag1__status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tags_info.hw_tag1.status", temp_b);
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
      valid_property = true;
      const char *sw_tag0__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag0.label");
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
      valid_property = true;
      bool sw_tag0__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag0.enabled");
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
      valid_property = true;
      bool sw_tag0__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag0.status");
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
      valid_property = true;
      const char *sw_tag1__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag1.label");
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
      valid_property = true;
      bool sw_tag1__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag1.enabled");
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
      valid_property = true;
      bool sw_tag1__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag1.status");
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
      valid_property = true;
      const char *sw_tag2__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag2.label");
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
      valid_property = true;
      bool sw_tag2__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag2.enabled");
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
      valid_property = true;
      bool sw_tag2__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag2.status");
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
      valid_property = true;
      const char *sw_tag3__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag3.label");
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
      valid_property = true;
      bool sw_tag3__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag3.enabled");
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
      valid_property = true;
      bool sw_tag3__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag3.status");
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
      valid_property = true;
      const char *sw_tag4__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag4.label");
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
      valid_property = true;
      bool sw_tag4__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag4.enabled");
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
      valid_property = true;
      bool sw_tag4__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag4.status");
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
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag5"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag5.label"))
    {
      valid_property = true;
      const char *sw_tag5__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag5.label");
      ret = tags_info_set_sw_tag5__label(sw_tag5__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", sw_tag5__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_sw_tag5__label;
        tags_info_get_sw_tag5__label(&old_sw_tag5__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sw_tag5__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag5"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag5.enabled"))
    {
      valid_property = true;
      bool sw_tag5__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag5.enabled");
      ret = tags_info_set_sw_tag5__enabled(sw_tag5__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag5__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag5__enabled;
        tags_info_get_sw_tag5__enabled(&old_sw_tag5__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag5__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag5"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag5.status"))
    {
      valid_property = true;
      bool sw_tag5__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag5.status");
      ret = tags_info_set_sw_tag5__status(sw_tag5__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag5__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag5__status;
        tags_info_get_sw_tag5__status(&old_sw_tag5__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag5__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag6"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag6.label"))
    {
      valid_property = true;
      const char *sw_tag6__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag6.label");
      ret = tags_info_set_sw_tag6__label(sw_tag6__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", sw_tag6__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_sw_tag6__label;
        tags_info_get_sw_tag6__label(&old_sw_tag6__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sw_tag6__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag6"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag6.enabled"))
    {
      valid_property = true;
      bool sw_tag6__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag6.enabled");
      ret = tags_info_set_sw_tag6__enabled(sw_tag6__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag6__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag6__enabled;
        tags_info_get_sw_tag6__enabled(&old_sw_tag6__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag6__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag6"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag6.status"))
    {
      valid_property = true;
      bool sw_tag6__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag6.status");
      ret = tags_info_set_sw_tag6__status(sw_tag6__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag6__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag6__status;
        tags_info_get_sw_tag6__status(&old_sw_tag6__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag6__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag7"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag7.label"))
    {
      valid_property = true;
      const char *sw_tag7__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag7.label");
      ret = tags_info_set_sw_tag7__label(sw_tag7__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", sw_tag7__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_sw_tag7__label;
        tags_info_get_sw_tag7__label(&old_sw_tag7__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sw_tag7__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag7"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag7.enabled"))
    {
      valid_property = true;
      bool sw_tag7__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag7.enabled");
      ret = tags_info_set_sw_tag7__enabled(sw_tag7__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag7__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag7__enabled;
        tags_info_get_sw_tag7__enabled(&old_sw_tag7__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag7__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag7"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag7.status"))
    {
      valid_property = true;
      bool sw_tag7__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag7.status");
      ret = tags_info_set_sw_tag7__status(sw_tag7__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag7__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag7__status;
        tags_info_get_sw_tag7__status(&old_sw_tag7__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag7__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag8"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag8.label"))
    {
      valid_property = true;
      const char *sw_tag8__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag8.label");
      ret = tags_info_set_sw_tag8__label(sw_tag8__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", sw_tag8__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_sw_tag8__label;
        tags_info_get_sw_tag8__label(&old_sw_tag8__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sw_tag8__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag8"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag8.enabled"))
    {
      valid_property = true;
      bool sw_tag8__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag8.enabled");
      ret = tags_info_set_sw_tag8__enabled(sw_tag8__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag8__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag8__enabled;
        tags_info_get_sw_tag8__enabled(&old_sw_tag8__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag8__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag8"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag8.status"))
    {
      valid_property = true;
      bool sw_tag8__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag8.status");
      ret = tags_info_set_sw_tag8__status(sw_tag8__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag8__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag8__status;
        tags_info_get_sw_tag8__status(&old_sw_tag8__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag8__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag9"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag9.label"))
    {
      valid_property = true;
      const char *sw_tag9__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag9.label");
      ret = tags_info_set_sw_tag9__label(sw_tag9__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", sw_tag9__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_sw_tag9__label;
        tags_info_get_sw_tag9__label(&old_sw_tag9__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sw_tag9__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag9"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag9.enabled"))
    {
      valid_property = true;
      bool sw_tag9__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag9.enabled");
      ret = tags_info_set_sw_tag9__enabled(sw_tag9__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag9__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag9__enabled;
        tags_info_get_sw_tag9__enabled(&old_sw_tag9__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag9__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag9"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag9.status"))
    {
      valid_property = true;
      bool sw_tag9__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag9.status");
      ret = tags_info_set_sw_tag9__status(sw_tag9__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag9__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag9__status;
        tags_info_get_sw_tag9__status(&old_sw_tag9__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag9__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag10"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag10.label"))
    {
      valid_property = true;
      const char *sw_tag10__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag10.label");
      ret = tags_info_set_sw_tag10__label(sw_tag10__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", sw_tag10__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_sw_tag10__label;
        tags_info_get_sw_tag10__label(&old_sw_tag10__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sw_tag10__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag10"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag10.enabled"))
    {
      valid_property = true;
      bool sw_tag10__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag10.enabled");
      ret = tags_info_set_sw_tag10__enabled(sw_tag10__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag10__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag10__enabled;
        tags_info_get_sw_tag10__enabled(&old_sw_tag10__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag10__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag10"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag10.status"))
    {
      valid_property = true;
      bool sw_tag10__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag10.status");
      ret = tags_info_set_sw_tag10__status(sw_tag10__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag10__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag10__status;
        tags_info_get_sw_tag10__status(&old_sw_tag10__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag10__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag11"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag11.label"))
    {
      valid_property = true;
      const char *sw_tag11__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag11.label");
      ret = tags_info_set_sw_tag11__label(sw_tag11__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", sw_tag11__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_sw_tag11__label;
        tags_info_get_sw_tag11__label(&old_sw_tag11__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sw_tag11__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag11"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag11.enabled"))
    {
      valid_property = true;
      bool sw_tag11__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag11.enabled");
      ret = tags_info_set_sw_tag11__enabled(sw_tag11__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag11__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag11__enabled;
        tags_info_get_sw_tag11__enabled(&old_sw_tag11__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag11__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag11"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag11.status"))
    {
      valid_property = true;
      bool sw_tag11__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag11.status");
      ret = tags_info_set_sw_tag11__status(sw_tag11__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag11__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag11__status;
        tags_info_get_sw_tag11__status(&old_sw_tag11__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag11__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag12"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag12.label"))
    {
      valid_property = true;
      const char *sw_tag12__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag12.label");
      ret = tags_info_set_sw_tag12__label(sw_tag12__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", sw_tag12__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_sw_tag12__label;
        tags_info_get_sw_tag12__label(&old_sw_tag12__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sw_tag12__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag12"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag12.enabled"))
    {
      valid_property = true;
      bool sw_tag12__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag12.enabled");
      ret = tags_info_set_sw_tag12__enabled(sw_tag12__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag12__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag12__enabled;
        tags_info_get_sw_tag12__enabled(&old_sw_tag12__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag12__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag12"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag12.status"))
    {
      valid_property = true;
      bool sw_tag12__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag12.status");
      ret = tags_info_set_sw_tag12__status(sw_tag12__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag12__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag12__status;
        tags_info_get_sw_tag12__status(&old_sw_tag12__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag12__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag13"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag13.label"))
    {
      valid_property = true;
      const char *sw_tag13__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag13.label");
      ret = tags_info_set_sw_tag13__label(sw_tag13__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", sw_tag13__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_sw_tag13__label;
        tags_info_get_sw_tag13__label(&old_sw_tag13__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sw_tag13__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag13"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag13.enabled"))
    {
      valid_property = true;
      bool sw_tag13__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag13.enabled");
      ret = tags_info_set_sw_tag13__enabled(sw_tag13__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag13__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag13__enabled;
        tags_info_get_sw_tag13__enabled(&old_sw_tag13__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag13__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag13"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag13.status"))
    {
      valid_property = true;
      bool sw_tag13__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag13.status");
      ret = tags_info_set_sw_tag13__status(sw_tag13__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag13__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag13__status;
        tags_info_get_sw_tag13__status(&old_sw_tag13__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag13__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag14"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag14.label"))
    {
      valid_property = true;
      const char *sw_tag14__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag14.label");
      ret = tags_info_set_sw_tag14__label(sw_tag14__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", sw_tag14__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_sw_tag14__label;
        tags_info_get_sw_tag14__label(&old_sw_tag14__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sw_tag14__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag14"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag14.enabled"))
    {
      valid_property = true;
      bool sw_tag14__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag14.enabled");
      ret = tags_info_set_sw_tag14__enabled(sw_tag14__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag14__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag14__enabled;
        tags_info_get_sw_tag14__enabled(&old_sw_tag14__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag14__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag14"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag14.status"))
    {
      valid_property = true;
      bool sw_tag14__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag14.status");
      ret = tags_info_set_sw_tag14__status(sw_tag14__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag14__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag14__status;
        tags_info_get_sw_tag14__status(&old_sw_tag14__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag14__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag15"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag15.label"))
    {
      valid_property = true;
      const char *sw_tag15__label = json_object_dotget_string(tempJSONObject, "tags_info.sw_tag15.label");
      ret = tags_info_set_sw_tag15__label(sw_tag15__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", sw_tag15__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_sw_tag15__label;
        tags_info_get_sw_tag15__label(&old_sw_tag15__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sw_tag15__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag15"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag15.enabled"))
    {
      valid_property = true;
      bool sw_tag15__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag15.enabled");
      ret = tags_info_set_sw_tag15__enabled(sw_tag15__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag15__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag15__enabled;
        tags_info_get_sw_tag15__enabled(&old_sw_tag15__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag15__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag15"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.sw_tag15.status"))
    {
      valid_property = true;
      bool sw_tag15__status = json_object_dotget_boolean(tempJSONObject, "tags_info.sw_tag15.status");
      ret = tags_info_set_sw_tag15__status(sw_tag15__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", sw_tag15__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_sw_tag15__status;
        tags_info_get_sw_tag15__status(&old_sw_tag15__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_sw_tag15__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.hw_tag0"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.hw_tag0.label"))
    {
      valid_property = true;
      const char *hw_tag0__label = json_object_dotget_string(tempJSONObject, "tags_info.hw_tag0.label");
      ret = tags_info_set_hw_tag0__label(hw_tag0__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", hw_tag0__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_hw_tag0__label;
        tags_info_get_hw_tag0__label(&old_hw_tag0__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_hw_tag0__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.hw_tag0"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.hw_tag0.enabled"))
    {
      valid_property = true;
      bool hw_tag0__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.hw_tag0.enabled");
      ret = tags_info_set_hw_tag0__enabled(hw_tag0__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", hw_tag0__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_hw_tag0__enabled;
        tags_info_get_hw_tag0__enabled(&old_hw_tag0__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_hw_tag0__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.hw_tag0"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.hw_tag0.status"))
    {
      valid_property = true;
      bool hw_tag0__status = json_object_dotget_boolean(tempJSONObject, "tags_info.hw_tag0.status");
      ret = tags_info_set_hw_tag0__status(hw_tag0__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", hw_tag0__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_hw_tag0__status;
        tags_info_get_hw_tag0__status(&old_hw_tag0__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_hw_tag0__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.hw_tag1"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.hw_tag1.label"))
    {
      valid_property = true;
      const char *hw_tag1__label = json_object_dotget_string(tempJSONObject, "tags_info.hw_tag1.label");
      ret = tags_info_set_hw_tag1__label(hw_tag1__label, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", hw_tag1__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_hw_tag1__label;
        tags_info_get_hw_tag1__label(&old_hw_tag1__label);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_hw_tag1__label);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.hw_tag1"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.hw_tag1.enabled"))
    {
      valid_property = true;
      bool hw_tag1__enabled = json_object_dotget_boolean(tempJSONObject, "tags_info.hw_tag1.enabled");
      ret = tags_info_set_hw_tag1__enabled(hw_tag1__enabled, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", hw_tag1__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_hw_tag1__enabled;
        tags_info_get_hw_tag1__enabled(&old_hw_tag1__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_hw_tag1__enabled);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tags_info.hw_tag1"))
  {
    if (json_object_dothas_value(tempJSONObject, "tags_info.hw_tag1.status"))
    {
      valid_property = true;
      bool hw_tag1__status = json_object_dotget_boolean(tempJSONObject, "tags_info.hw_tag1.status");
      ret = tags_info_set_hw_tag1__status(hw_tag1__status, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", hw_tag1__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_hw_tag1__status;
        tags_info_get_hw_tag1__status(&old_hw_tag1__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_hw_tag1__status);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  json_value_free(tempJSON);
  /* Check if received a request to modify an existing property */
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
    char *log_message = "Invalid property for Tags_Info";
    PnPLCreateLogMessage(response, size, log_message, PNPL_LOG_ERROR);
  }
  json_value_free(respJSON);
  return ret;
}


uint8_t Tags_Info_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                           uint32_t *size, uint8_t pretty)
{
  return PNPL_NO_COMMANDS_ERROR_CODE;
}

