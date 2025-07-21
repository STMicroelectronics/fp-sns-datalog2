/**
  ******************************************************************************
  * @file    Vl53l8cx_3_Tof_PnPL.c
  * @author  SRA
  * @brief   Vl53l8cx_3_Tof PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect:sensors:vl53l8cx_3_tof;2
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

#include "Vl53l8cx_3_Tof_PnPL.h"

static const IPnPLComponent_vtbl sVl53l8cx_3_Tof_PnPL_CompIF_vtbl =
{
  Vl53l8cx_3_Tof_PnPL_vtblGetKey,
  Vl53l8cx_3_Tof_PnPL_vtblGetNCommands,
  Vl53l8cx_3_Tof_PnPL_vtblGetCommandKey,
  Vl53l8cx_3_Tof_PnPL_vtblGetStatus,
  Vl53l8cx_3_Tof_PnPL_vtblSetProperty,
  Vl53l8cx_3_Tof_PnPL_vtblExecuteFunction
};

/**
  *  Vl53l8cx_3_Tof_PnPL internal structure.
  */
struct _Vl53l8cx_3_Tof_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Vl53l8cx_3_Tof_PnPL sVl53l8cx_3_Tof_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Vl53l8cx_3_Tof_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sVl53l8cx_3_Tof_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sVl53l8cx_3_Tof_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Vl53l8cx_3_Tof_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  vl53l8cx_3_tof_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Vl53l8cx_3_Tof_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return vl53l8cx_3_tof_get_key();
}

uint8_t Vl53l8cx_3_Tof_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Vl53l8cx_3_Tof_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Vl53l8cx_3_Tof_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                          uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  bool temp_b = 0;
  vl53l8cx_3_tof_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "vl53l8cx_3_tof.enable", temp_b);
  pnpl_vl53l8cx_3_tof_resolution_t temp_resolution_e = (pnpl_vl53l8cx_3_tof_resolution_t)0;
  vl53l8cx_3_tof_get_resolution(&temp_resolution_e);
  json_object_dotset_number(JSON_Status, "vl53l8cx_3_tof.resolution", temp_resolution_e);
  int32_t temp_i = 0;
  vl53l8cx_3_tof_get_odr(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_3_tof.odr", temp_i);
  pnpl_vl53l8cx_3_tof_ranging_mode_t temp_ranging_mode_e = (pnpl_vl53l8cx_3_tof_ranging_mode_t)0;
  vl53l8cx_3_tof_get_ranging_mode(&temp_ranging_mode_e);
  json_object_dotset_number(JSON_Status, "vl53l8cx_3_tof.ranging_mode", temp_ranging_mode_e);
  vl53l8cx_3_tof_get_integration_time(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_3_tof.integration_time", temp_i);
  vl53l8cx_3_tof_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_3_tof.samples_per_ts", temp_i);
  float_t temp_f = 0;
  vl53l8cx_3_tof_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "vl53l8cx_3_tof.ioffset", temp_f);
  vl53l8cx_3_tof_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_3_tof.usb_dps", temp_i);
  vl53l8cx_3_tof_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_3_tof.sd_dps", temp_i);
  char *temp_s = "";
  vl53l8cx_3_tof_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "vl53l8cx_3_tof.data_type", temp_s);
  vl53l8cx_3_tof_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "vl53l8cx_3_tof.sensor_annotation", temp_s);
  vl53l8cx_3_tof_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_3_tof.sensor_category", temp_i);
  vl53l8cx_3_tof_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_3_tof.dim", temp_i);
  vl53l8cx_3_tof_get_output_format__target_status_start_id(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_3_tof.output_format.target_status.start_id", temp_i);
  vl53l8cx_3_tof_get_output_format__target_distance_start_id(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_3_tof.output_format.target_distance.start_id", temp_i);
  vl53l8cx_3_tof_get_output_format__nof_outputs(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_3_tof.output_format.nof_outputs", temp_i);
  vl53l8cx_3_tof_get_mounted(&temp_b);
  json_object_dotset_boolean(JSON_Status, "vl53l8cx_3_tof.mounted", temp_b);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "vl53l8cx_3_tof.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  vl53l8cx_3_tof_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "vl53l8cx_3_tof.stream_id", temp_int8);
  vl53l8cx_3_tof_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "vl53l8cx_3_tof.ep_id", temp_int8);

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

uint8_t Vl53l8cx_3_Tof_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                            uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  bool valid_property = false;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "vl53l8cx_3_tof.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "vl53l8cx_3_tof.enable");
    valid_property = true;
    ret = vl53l8cx_3_tof_set_enable(enable, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_enable;
      vl53l8cx_3_tof_get_enable(&old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vl53l8cx_3_tof.resolution"))
  {
    int32_t resolution = (int32_t)json_object_dotget_number(tempJSONObject, "vl53l8cx_3_tof.resolution");
    valid_property = true;
    ret = vl53l8cx_3_tof_set_resolution((pnpl_vl53l8cx_3_tof_resolution_t)resolution, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", resolution);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_vl53l8cx_3_tof_resolution_t old_resolution;
      vl53l8cx_3_tof_get_resolution(&old_resolution);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_resolution);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vl53l8cx_3_tof.odr"))
  {
    int32_t odr = (int32_t)json_object_dotget_number(tempJSONObject, "vl53l8cx_3_tof.odr");
    valid_property = true;
    ret = vl53l8cx_3_tof_set_odr(odr, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", odr);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_odr;
      vl53l8cx_3_tof_get_odr(&old_odr);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_odr);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vl53l8cx_3_tof.ranging_mode"))
  {
    int32_t ranging_mode = (int32_t)json_object_dotget_number(tempJSONObject, "vl53l8cx_3_tof.ranging_mode");
    valid_property = true;
    ret = vl53l8cx_3_tof_set_ranging_mode((pnpl_vl53l8cx_3_tof_ranging_mode_t)ranging_mode, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", ranging_mode);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_vl53l8cx_3_tof_ranging_mode_t old_ranging_mode;
      vl53l8cx_3_tof_get_ranging_mode(&old_ranging_mode);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_ranging_mode);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vl53l8cx_3_tof.integration_time"))
  {
    int32_t integration_time = (int32_t)json_object_dotget_number(tempJSONObject, "vl53l8cx_3_tof.integration_time");
    valid_property = true;
    ret = vl53l8cx_3_tof_set_integration_time(integration_time, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", integration_time);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_integration_time;
      vl53l8cx_3_tof_get_integration_time(&old_integration_time);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_integration_time);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vl53l8cx_3_tof.samples_per_ts"))
  {
    int32_t samples_per_ts = (int32_t)json_object_dotget_number(tempJSONObject, "vl53l8cx_3_tof.samples_per_ts");
    valid_property = true;
    ret = vl53l8cx_3_tof_set_samples_per_ts(samples_per_ts, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", samples_per_ts);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_samples_per_ts;
      vl53l8cx_3_tof_get_samples_per_ts(&old_samples_per_ts);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_samples_per_ts);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vl53l8cx_3_tof.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "vl53l8cx_3_tof.sensor_annotation");
    valid_property = true;
    ret = vl53l8cx_3_tof_set_sensor_annotation(sensor_annotation, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_sensor_annotation;
      vl53l8cx_3_tof_get_sensor_annotation(&old_sensor_annotation);
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
    char *log_message = "Invalid property for vl53l8cx_3_tof";
    PnPLCreateLogMessage(response, size, log_message, PNPL_LOG_ERROR);
    ret = PNPL_BASE_ERROR_CODE;
  }
  json_value_free(respJSON);
  return ret;
}


uint8_t Vl53l8cx_3_Tof_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                uint32_t *size, uint8_t pretty)
{
  return PNPL_NO_COMMANDS_ERROR_CODE;
}

