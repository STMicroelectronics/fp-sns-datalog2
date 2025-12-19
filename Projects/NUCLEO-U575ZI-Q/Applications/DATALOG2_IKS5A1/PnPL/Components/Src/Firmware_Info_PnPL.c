/**
  ******************************************************************************
  * @file    Firmware_Info_PnPL.c
  * @author  SRA
  * @brief   Firmware_Info PnPL Component Manager
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
  * dtmi:vespucci:other:firmware_info;3
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

#include "Firmware_Info_PnPL.h"

static const IPnPLComponent_vtbl sFirmware_Info_PnPL_CompIF_vtbl =
{
  Firmware_Info_PnPL_vtblGetKey,
  Firmware_Info_PnPL_vtblGetNCommands,
  Firmware_Info_PnPL_vtblGetCommandKey,
  Firmware_Info_PnPL_vtblGetStatus,
  Firmware_Info_PnPL_vtblSetProperty,
  Firmware_Info_PnPL_vtblExecuteFunction
};

/**
  *  Firmware_Info_PnPL internal structure.
  */
struct _Firmware_Info_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Firmware_Info_PnPL sFirmware_Info_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Firmware_Info_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sFirmware_Info_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sFirmware_Info_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Firmware_Info_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  firmware_info_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Firmware_Info_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return firmware_info_get_key();
}

uint8_t Firmware_Info_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Firmware_Info_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Firmware_Info_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  char *temp_s = "";
  firmware_info_get_alias(&temp_s);
  json_object_dotset_string(JSON_Status, "firmware_info.alias", temp_s);
  firmware_info_get_fw_name(&temp_s);
  json_object_dotset_string(JSON_Status, "firmware_info.fw_name", temp_s);
  firmware_info_get_fw_version(&temp_s);
  json_object_dotset_string(JSON_Status, "firmware_info.fw_version", temp_s);
  firmware_info_get_part_number(&temp_s);
  json_object_dotset_string(JSON_Status, "firmware_info.part_number", temp_s);
  firmware_info_get_device_url(&temp_s);
  json_object_dotset_string(JSON_Status, "firmware_info.device_url", temp_s);
  firmware_info_get_fw_url(&temp_s);
  json_object_dotset_string(JSON_Status, "firmware_info.fw_url", temp_s);
  firmware_info_get_mac_address(&temp_s);
  json_object_dotset_string(JSON_Status, "firmware_info.mac_address", temp_s);
  json_object_dotset_number(JSON_Status, "firmware_info.c_type", COMP_TYPE_OTHER);

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

uint8_t Firmware_Info_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                           uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  bool valid_property = false;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "firmware_info.alias"))
  {
    const char *alias = json_object_dotget_string(tempJSONObject, "firmware_info.alias");
    valid_property = true;
    ret = firmware_info_set_alias(alias, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", alias);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_alias;
      firmware_info_get_alias(&old_alias);
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_alias);
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
    char *log_message = "Invalid property for firmware_info";
    PnPLCreateLogMessage(response, size, log_message, PNPL_LOG_ERROR);
    ret = PNPL_BASE_ERROR_CODE;
  }
  json_value_free(respJSON);
  return ret;
}


uint8_t Firmware_Info_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                               uint32_t *size, uint8_t pretty)
{
  return PNPL_NO_COMMANDS_ERROR_CODE;
}

