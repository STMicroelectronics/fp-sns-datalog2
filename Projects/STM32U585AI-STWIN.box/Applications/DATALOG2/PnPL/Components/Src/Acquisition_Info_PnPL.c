/**
  ******************************************************************************
  * @file    Acquisition_Info_PnPL.c
  * @author  SRA
  * @brief   Acquisition_Info PnPL Component Manager
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
  * dtmi:vespucci:other:acquisition_info;2
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

#include "Acquisition_Info_PnPL.h"

static const IPnPLComponent_vtbl sAcquisition_Info_PnPL_CompIF_vtbl =
{
  Acquisition_Info_PnPL_vtblGetKey,
  Acquisition_Info_PnPL_vtblGetNCommands,
  Acquisition_Info_PnPL_vtblGetCommandKey,
  Acquisition_Info_PnPL_vtblGetStatus,
  Acquisition_Info_PnPL_vtblSetProperty,
  Acquisition_Info_PnPL_vtblExecuteFunction
};

/**
  *  Acquisition_Info_PnPL internal structure.
  */
struct _Acquisition_Info_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Acquisition_Info_PnPL sAcquisition_Info_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Acquisition_Info_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sAcquisition_Info_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sAcquisition_Info_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Acquisition_Info_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  acquisition_info_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Acquisition_Info_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return acquisition_info_get_key();
}

uint8_t Acquisition_Info_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Acquisition_Info_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Acquisition_Info_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                            uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  JSON_Value *temp_j = json_value_init_object();
  acquisition_info_get_tags(temp_j);
  json_object_set_value(JSON_Status, "acquisition_info", temp_j);
  char *temp_s = "";
  acquisition_info_get_name(&temp_s);
  json_object_dotset_string(JSON_Status, "acquisition_info.name", temp_s);
  acquisition_info_get_description(&temp_s);
  json_object_dotset_string(JSON_Status, "acquisition_info.description", temp_s);
  acquisition_info_get_uuid(&temp_s);
  json_object_dotset_string(JSON_Status, "acquisition_info.uuid", temp_s);
  acquisition_info_get_start_time(&temp_s);
  json_object_dotset_string(JSON_Status, "acquisition_info.start_time", temp_s);
  acquisition_info_get_end_time(&temp_s);
  json_object_dotset_string(JSON_Status, "acquisition_info.end_time", temp_s);
  acquisition_info_get_data_ext(&temp_s);
  json_object_dotset_string(JSON_Status, "acquisition_info.data_ext", temp_s);
  acquisition_info_get_data_fmt(&temp_s);
  json_object_dotset_string(JSON_Status, "acquisition_info.data_fmt", temp_s);
  pnpl_acquisition_info_interface_t temp_interface_e = (pnpl_acquisition_info_interface_t)0;
  acquisition_info_get_interface(&temp_interface_e);
  json_object_dotset_number(JSON_Status, "acquisition_info.interface", temp_interface_e);
  acquisition_info_get_schema_version(&temp_s);
  json_object_dotset_string(JSON_Status, "acquisition_info.schema_version", temp_s);
  json_object_dotset_number(JSON_Status, "acquisition_info.c_type", COMP_TYPE_OTHER);

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

uint8_t Acquisition_Info_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                              uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "acquisition_info.name"))
  {
    const char *name = json_object_dotget_string(tempJSONObject, "acquisition_info.name");
    ret = acquisition_info_set_name(name, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", name);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_name;
      acquisition_info_get_name(&old_name);
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_name);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "acquisition_info.description"))
  {
    const char *description = json_object_dotget_string(tempJSONObject, "acquisition_info.description");
    ret = acquisition_info_set_description(description, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", description);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_description;
      acquisition_info_get_description(&old_description);
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_description);
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


uint8_t Acquisition_Info_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                  uint32_t *size, uint8_t pretty)
{
  return PNPL_NO_COMMANDS_ERROR_CODE;
}

