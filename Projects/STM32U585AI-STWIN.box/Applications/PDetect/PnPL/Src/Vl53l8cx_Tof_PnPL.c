/**
  ******************************************************************************
  * @file    Vl53l8cx_Tof_PnPL.c
  * @author  SRA
  * @brief   Vl53l8cx_Tof PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect:sensors:vl53l8cx_tof;1
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

#include "Vl53l8cx_Tof_PnPL.h"
#include "Vl53l8cx_Tof_PnPL_vtbl.h"

static const IPnPLComponent_vtbl sVl53l8cx_Tof_PnPL_CompIF_vtbl =
{
  Vl53l8cx_Tof_PnPL_vtblGetKey,
  Vl53l8cx_Tof_PnPL_vtblGetNCommands,
  Vl53l8cx_Tof_PnPL_vtblGetCommandKey,
  Vl53l8cx_Tof_PnPL_vtblGetStatus,
  Vl53l8cx_Tof_PnPL_vtblSetProperty,
  Vl53l8cx_Tof_PnPL_vtblExecuteFunction
};

/**
  *  Vl53l8cx_Tof_PnPL internal structure.
  */
struct _Vl53l8cx_Tof_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;

};

/* Objects instance ----------------------------------------------------------*/
static Vl53l8cx_Tof_PnPL sVl53l8cx_Tof_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Vl53l8cx_Tof_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sVl53l8cx_Tof_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sVl53l8cx_Tof_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Vl53l8cx_Tof_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  vl53l8cx_tof_comp_init();
  return 0;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Vl53l8cx_Tof_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return vl53l8cx_tof_get_key();
}

uint8_t Vl53l8cx_Tof_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Vl53l8cx_Tof_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Vl53l8cx_Tof_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  bool temp_b = 0;
  vl53l8cx_tof_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "vl53l8cx_tof.enable", temp_b);
  int32_t temp_i = 0;
  vl53l8cx_tof_get_odr(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_tof.odr", temp_i);
  char *temp_s = "";
  vl53l8cx_tof_get_resolution(&temp_s);
  uint8_t enum_id = 0;
  if (strcmp(temp_s, vl53l8cx_tof_resolution_n4x4) == 0)
  {
    enum_id = 0;
  }
  else if (strcmp(temp_s, vl53l8cx_tof_resolution_n8x8) == 0)
  {
    enum_id = 1;
  }
  json_object_dotset_number(JSON_Status, "vl53l8cx_tof.resolution", enum_id);
  vl53l8cx_tof_get_ranging_mode(&temp_s);
  enum_id = 0;
  if (strcmp(temp_s, vl53l8cx_tof_ranging_mode_continuous) == 0)
  {
    enum_id = 0;
  }
  else if (strcmp(temp_s, vl53l8cx_tof_ranging_mode_autonomous) == 0)
  {
    enum_id = 1;
  }
  json_object_dotset_number(JSON_Status, "vl53l8cx_tof.ranging_mode", enum_id);
  vl53l8cx_tof_get_integration_time(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_tof.integration_time", temp_i);
  vl53l8cx_tof_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_tof.samples_per_ts", temp_i);
  float temp_f = 0;
  vl53l8cx_tof_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "vl53l8cx_tof.ioffset", temp_f);
  vl53l8cx_tof_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_tof.usb_dps", temp_i);
  vl53l8cx_tof_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_tof.sd_dps", temp_i);
  vl53l8cx_tof_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "vl53l8cx_tof.data_type", temp_s);
  vl53l8cx_tof_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "vl53l8cx_tof.sensor_annotation", temp_s);
  vl53l8cx_tof_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_tof.sensor_category", temp_i);
  vl53l8cx_tof_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "vl53l8cx_tof.dim", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "vl53l8cx_tof.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  vl53l8cx_tof_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "vl53l8cx_tof.stream_id", temp_int8);
  vl53l8cx_tof_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "vl53l8cx_tof.ep_id", temp_int8);

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

uint8_t Vl53l8cx_Tof_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                          uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = 0;
  if (json_object_dothas_value(tempJSONObject, "vl53l8cx_tof.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "vl53l8cx_tof.enable");
    ret = vl53l8cx_tof_set_enable(enable);
    if (ret == 0)
    {
      json_object_dotset_boolean(respJSONObject, "vl53l8cx_tof.enable.value", enable);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vl53l8cx_tof.enable.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vl53l8cx_tof.odr"))
  {
    int32_t odr = (int32_t) json_object_dotget_number(tempJSONObject, "vl53l8cx_tof.odr");
    ret = vl53l8cx_tof_set_odr(odr);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "vl53l8cx_tof.odr.value", odr);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vl53l8cx_tof.odr.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vl53l8cx_tof.resolution"))
  {
    int resolution = (int)json_object_dotget_number(tempJSONObject, "vl53l8cx_tof.resolution");
    switch(resolution)
    {
      case 0:
        ret = vl53l8cx_tof_set_resolution(vl53l8cx_tof_resolution_n4x4);
        break;
      case 1:
        ret = vl53l8cx_tof_set_resolution(vl53l8cx_tof_resolution_n8x8);
        break;
    }
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "vl53l8cx_tof.resolution.value",resolution);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vl53l8cx_tof.resolution.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vl53l8cx_tof.ranging_mode"))
  {
    int ranging_mode = (int)json_object_dotget_number(tempJSONObject, "vl53l8cx_tof.ranging_mode");
    switch(ranging_mode)
    {
      case 0:
        ret = vl53l8cx_tof_set_ranging_mode(vl53l8cx_tof_ranging_mode_continuous);
        break;
      case 1:
        ret = vl53l8cx_tof_set_ranging_mode(vl53l8cx_tof_ranging_mode_autonomous);
        break;
    }
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "vl53l8cx_tof.ranging_mode.value",ranging_mode);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vl53l8cx_tof.ranging_mode.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vl53l8cx_tof.integration_time"))
  {
    int32_t integration_time = (int32_t) json_object_dotget_number(tempJSONObject, "vl53l8cx_tof.integration_time");
    ret = vl53l8cx_tof_set_integration_time(integration_time);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "vl53l8cx_tof.integration_time.value", integration_time);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vl53l8cx_tof.integration_time.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vl53l8cx_tof.samples_per_ts"))
  {
    int32_t samples_per_ts = (int32_t) json_object_dotget_number(tempJSONObject, "vl53l8cx_tof.samples_per_ts");
    ret = vl53l8cx_tof_set_samples_per_ts(samples_per_ts);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "vl53l8cx_tof.samples_per_ts.value", samples_per_ts);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vl53l8cx_tof.samples_per_ts.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vl53l8cx_tof.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "vl53l8cx_tof.sensor_annotation");
    ret = vl53l8cx_tof_set_sensor_annotation(sensor_annotation);
    if (ret == 0)
    {
      json_object_dotset_string(respJSONObject, "vl53l8cx_tof.sensor_annotation.value", sensor_annotation);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vl53l8cx_tof.sensor_annotation.value", "PNPL_SET_ERROR");
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


uint8_t Vl53l8cx_Tof_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                              uint32_t *size, uint8_t pretty)
{
  return 1;
}

