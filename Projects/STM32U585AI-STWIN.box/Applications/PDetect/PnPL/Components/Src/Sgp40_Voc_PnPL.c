/**
  ******************************************************************************
  * @file    Sgp40_Voc_PnPL.c
  * @author  SRA
  * @brief   Sgp40_Voc PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:FP_SNS_DATALOG2_PDetect:sensors:sgp40_voc;1
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

#include "Sgp40_Voc_PnPL.h"

static const IPnPLComponent_vtbl sSgp40_Voc_PnPL_CompIF_vtbl =
{
  Sgp40_Voc_PnPL_vtblGetKey,
  Sgp40_Voc_PnPL_vtblGetNCommands,
  Sgp40_Voc_PnPL_vtblGetCommandKey,
  Sgp40_Voc_PnPL_vtblGetStatus,
  Sgp40_Voc_PnPL_vtblSetProperty,
  Sgp40_Voc_PnPL_vtblExecuteFunction
};

/**
  *  Sgp40_Voc_PnPL internal structure.
  */
struct _Sgp40_Voc_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Sgp40_Voc_PnPL sSgp40_Voc_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Sgp40_Voc_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sSgp40_Voc_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sSgp40_Voc_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Sgp40_Voc_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  sgp40_voc_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Sgp40_Voc_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return sgp40_voc_get_key();
}

uint8_t Sgp40_Voc_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Sgp40_Voc_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Sgp40_Voc_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  pnpl_sgp40_voc_odr_t temp_odr_e = (pnpl_sgp40_voc_odr_t)0;
  sgp40_voc_get_odr(&temp_odr_e);
  json_object_dotset_number(JSON_Status, "sgp40_voc.odr", temp_odr_e);
  pnpl_sgp40_voc_fs_t temp_fs_e = (pnpl_sgp40_voc_fs_t)0;
  sgp40_voc_get_fs(&temp_fs_e);
  json_object_dotset_number(JSON_Status, "sgp40_voc.fs", temp_fs_e);
  bool temp_b = 0;
  sgp40_voc_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "sgp40_voc.enable", temp_b);
  int32_t temp_i = 0;
  sgp40_voc_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "sgp40_voc.samples_per_ts", temp_i);
  sgp40_voc_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "sgp40_voc.dim", temp_i);
  float temp_f = 0;
  sgp40_voc_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "sgp40_voc.ioffset", temp_f);
  sgp40_voc_get_measodr(&temp_f);
  json_object_dotset_number(JSON_Status, "sgp40_voc.measodr", temp_f);
  sgp40_voc_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "sgp40_voc.usb_dps", temp_i);
  sgp40_voc_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "sgp40_voc.sd_dps", temp_i);
  sgp40_voc_get_sensitivity(&temp_f);
  json_object_dotset_number(JSON_Status, "sgp40_voc.sensitivity", temp_f);
  char *temp_s = "";
  sgp40_voc_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "sgp40_voc.data_type", temp_s);
  sgp40_voc_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "sgp40_voc.sensor_annotation", temp_s);
  sgp40_voc_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "sgp40_voc.sensor_category", temp_i);
  sgp40_voc_get_mounted(&temp_b);
  json_object_dotset_boolean(JSON_Status, "sgp40_voc.mounted", temp_b);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "sgp40_voc.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  sgp40_voc_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "sgp40_voc.stream_id", temp_int8);
  sgp40_voc_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "sgp40_voc.ep_id", temp_int8);

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

uint8_t Sgp40_Voc_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size,
                                       uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  bool valid_property = false;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "sgp40_voc.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "sgp40_voc.enable");
    valid_property = true;
    ret = sgp40_voc_set_enable(enable, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_enable;
      sgp40_voc_get_enable(&old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sgp40_voc.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "sgp40_voc.sensor_annotation");
    valid_property = true;
    ret = sgp40_voc_set_sensor_annotation(sensor_annotation, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_sensor_annotation;
      sgp40_voc_get_sensor_annotation(&old_sensor_annotation);
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sensor_annotation);
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
    char *log_message = "Invalid property for sgp40_voc";
    PnPLCreateLogMessage(response, size, log_message, PNPL_LOG_ERROR);
    ret = PNPL_BASE_ERROR_CODE;
  }
  json_value_free(respJSON);
  return ret;
}


uint8_t Sgp40_Voc_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                           uint32_t *size, uint8_t pretty)
{
  return PNPL_NO_COMMANDS_ERROR_CODE;
}

