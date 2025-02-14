/**
  ******************************************************************************
  * @file    Log_Controller_PnPL.c
  * @author  SRA
  * @brief   Log_Controller PnPL Component Manager
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
  * dtmi:vespucci:other:log_controller;3
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

#include "Log_Controller_PnPL.h"

static const IPnPLComponent_vtbl sLog_Controller_PnPL_CompIF_vtbl =
{
  Log_Controller_PnPL_vtblGetKey,
  Log_Controller_PnPL_vtblGetNCommands,
  Log_Controller_PnPL_vtblGetCommandKey,
  Log_Controller_PnPL_vtblGetStatus,
  Log_Controller_PnPL_vtblSetProperty,
  Log_Controller_PnPL_vtblExecuteFunction
};

/**
  *  Log_Controller_PnPL internal structure.
  */
struct _Log_Controller_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Log_Controller_PnPL sLog_Controller_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Log_Controller_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sLog_Controller_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sLog_Controller_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Log_Controller_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  log_controller_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Log_Controller_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return log_controller_get_key();
}

uint8_t Log_Controller_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 7;
}

char *Log_Controller_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  switch (id)
  {
    case 0:
      return "log_controller*save_config";
      break;
    case 1:
      return "log_controller*start_log";
      break;
    case 2:
      return "log_controller*stop_log";
      break;
    case 3:
      return "log_controller*set_time";
      break;
    case 4:
      return "log_controller*switch_bank";
      break;
    case 5:
      return "log_controller*set_dfu_mode";
      break;
    case 6:
      return "log_controller*enable_all";
      break;
  }
  return "";
}

uint8_t Log_Controller_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                          uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  bool temp_b = 0;
  log_controller_get_log_status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "log_controller.log_status", temp_b);
  log_controller_get_sd_mounted(&temp_b);
  json_object_dotset_boolean(JSON_Status, "log_controller.sd_mounted", temp_b);
  log_controller_get_sd_failed(&temp_b);
  json_object_dotset_boolean(JSON_Status, "log_controller.sd_failed", temp_b);
  int32_t temp_i = 0;
  log_controller_get_controller_type(&temp_i);
  json_object_dotset_number(JSON_Status, "log_controller.controller_type", temp_i);
  json_object_dotset_number(JSON_Status, "log_controller.c_type", COMP_TYPE_OTHER);

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

uint8_t Log_Controller_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                            uint32_t *size, uint8_t pretty)
{
  *size = 61;
  *response = (char *)pnpl_malloc(*size);
  (void)strcpy(*response, "{\"PnPL_Error\":\"No writable Properties in DeviceInformation\"}\0");
  return PNPL_NO_ERROR_CODE;
}


uint8_t Log_Controller_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  bool valid_function = false;
  if (json_object_dothas_value(tempJSONObject, "log_controller*save_config"))
  {
    valid_function = true;
    ret = log_controller_save_config();
  }
  if (json_object_dothas_value(tempJSONObject, "log_controller*start_log.interface"))
  {
    valid_function = true;
    int32_t interface = (int32_t)json_object_dotget_number(tempJSONObject, "log_controller*start_log.interface");
    ret = log_controller_start_log(interface);
  }
  if (json_object_dothas_value(tempJSONObject, "log_controller*stop_log"))
  {
    valid_function = true;
    ret = log_controller_stop_log();
  }
  if (json_object_dothas_value(tempJSONObject, "log_controller*set_time.datetime"))
  {
    valid_function = true;
    const char *datetime = json_object_dotget_string(tempJSONObject, "log_controller*set_time.datetime");
    ret = log_controller_set_time(datetime);
  }
  if (json_object_dothas_value(tempJSONObject, "log_controller*switch_bank"))
  {
    valid_function = true;
    ret = log_controller_switch_bank();
  }
  if (json_object_dothas_value(tempJSONObject, "log_controller*set_dfu_mode"))
  {
    valid_function = true;
    ret = log_controller_set_dfu_mode();
  }
  if (json_object_dothas_value(tempJSONObject, "log_controller*enable_all.status"))
  {
    valid_function = true;
    bool status = json_object_dotget_boolean(tempJSONObject, "log_controller*enable_all.status");
    ret = log_controller_enable_all(status);
  }
  /* Check if received a valid function to modify an existing property */
  if (valid_function == false)
  {
    char log_message[100];
    (void) sprintf(log_message, "%s Invalid command", serializedJSON);
    PnPLCreateLogMessage(response, size, log_message, PNPL_LOG_ERROR);
    ret = PNPL_BASE_ERROR_CODE;
  }
  json_value_free(tempJSON);
  return ret;
}

