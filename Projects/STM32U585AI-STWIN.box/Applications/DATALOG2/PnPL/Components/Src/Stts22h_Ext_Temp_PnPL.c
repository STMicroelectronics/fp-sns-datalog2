/**
  ******************************************************************************
  * @file    Stts22h_Ext_Temp_PnPL.c
  * @author  SRA
  * @brief   Stts22h_Ext_Temp PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:sensors:stts22h_ext_temp;3
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

#include "Stts22h_Ext_Temp_PnPL.h"

static const IPnPLComponent_vtbl sStts22h_Ext_Temp_PnPL_CompIF_vtbl =
{
  Stts22h_Ext_Temp_PnPL_vtblGetKey,
  Stts22h_Ext_Temp_PnPL_vtblGetNCommands,
  Stts22h_Ext_Temp_PnPL_vtblGetCommandKey,
  Stts22h_Ext_Temp_PnPL_vtblGetStatus,
  Stts22h_Ext_Temp_PnPL_vtblSetProperty,
  Stts22h_Ext_Temp_PnPL_vtblExecuteFunction
};

/**
  *  Stts22h_Ext_Temp_PnPL internal structure.
  */
struct _Stts22h_Ext_Temp_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Stts22h_Ext_Temp_PnPL sStts22h_Ext_Temp_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Stts22h_Ext_Temp_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sStts22h_Ext_Temp_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sStts22h_Ext_Temp_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Stts22h_Ext_Temp_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  stts22h_ext_temp_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Stts22h_Ext_Temp_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return stts22h_ext_temp_get_key();
}

uint8_t Stts22h_Ext_Temp_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Stts22h_Ext_Temp_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Stts22h_Ext_Temp_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                            uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  pnpl_stts22h_ext_temp_odr_t temp_odr_e = (pnpl_stts22h_ext_temp_odr_t)0;
  stts22h_ext_temp_get_odr(&temp_odr_e);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.odr", temp_odr_e);
  pnpl_stts22h_ext_temp_fs_t temp_fs_e = (pnpl_stts22h_ext_temp_fs_t)0;
  stts22h_ext_temp_get_fs(&temp_fs_e);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.fs", temp_fs_e);
  bool temp_b = 0;
  stts22h_ext_temp_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "stts22h_ext_temp.enable", temp_b);
  int32_t temp_i = 0;
  stts22h_ext_temp_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.samples_per_ts", temp_i);
  stts22h_ext_temp_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.dim", temp_i);
  float temp_f = 0;
  stts22h_ext_temp_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.ioffset", temp_f);
  stts22h_ext_temp_get_measodr(&temp_f);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.measodr", temp_f);
  stts22h_ext_temp_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.usb_dps", temp_i);
  stts22h_ext_temp_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.sd_dps", temp_i);
  stts22h_ext_temp_get_sensitivity(&temp_f);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.sensitivity", temp_f);
  char *temp_s = "";
  stts22h_ext_temp_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "stts22h_ext_temp.data_type", temp_s);
  stts22h_ext_temp_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "stts22h_ext_temp.sensor_annotation", temp_s);
  stts22h_ext_temp_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.sensor_category", temp_i);
  stts22h_ext_temp_get_mounted(&temp_b);
  json_object_dotset_boolean(JSON_Status, "stts22h_ext_temp.mounted", temp_b);
  stts22h_ext_temp_get_st_ble_stream__id(&temp_i);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.st_ble_stream.id", temp_i);
  stts22h_ext_temp_get_st_ble_stream__temp__enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "stts22h_ext_temp.st_ble_stream.temp.enable", temp_b);
  stts22h_ext_temp_get_st_ble_stream__temp__unit(&temp_s);
  json_object_dotset_string(JSON_Status, "stts22h_ext_temp.st_ble_stream.temp.unit", temp_s);
  stts22h_ext_temp_get_st_ble_stream__temp__format(&temp_s);
  json_object_dotset_string(JSON_Status, "stts22h_ext_temp.st_ble_stream.temp.format", temp_s);
  stts22h_ext_temp_get_st_ble_stream__temp__elements(&temp_i);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.st_ble_stream.temp.elements", temp_i);
  stts22h_ext_temp_get_st_ble_stream__temp__channels(&temp_i);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.st_ble_stream.temp.channels", temp_i);
  stts22h_ext_temp_get_st_ble_stream__temp__multiply_factor(&temp_f);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.st_ble_stream.temp.multiply_factor", temp_f);
  stts22h_ext_temp_get_st_ble_stream__temp__odr(&temp_i);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.st_ble_stream.temp.odr", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  stts22h_ext_temp_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.stream_id", temp_int8);
  stts22h_ext_temp_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "stts22h_ext_temp.ep_id", temp_int8);

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

uint8_t Stts22h_Ext_Temp_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                              uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.odr"))
  {
    int32_t odr = (int32_t)json_object_dotget_number(tempJSONObject, "stts22h_ext_temp.odr");
    ret = stts22h_ext_temp_set_odr((pnpl_stts22h_ext_temp_odr_t)odr, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", odr);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_stts22h_ext_temp_odr_t old_odr;
      stts22h_ext_temp_get_odr(&old_odr);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_odr);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "stts22h_ext_temp.enable");
    ret = stts22h_ext_temp_set_enable(enable, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_enable;
      stts22h_ext_temp_get_enable(&old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.samples_per_ts"))
  {
    int32_t samples_per_ts = (int32_t)json_object_dotget_number(tempJSONObject, "stts22h_ext_temp.samples_per_ts");
    ret = stts22h_ext_temp_set_samples_per_ts(samples_per_ts, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", samples_per_ts);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_samples_per_ts;
      stts22h_ext_temp_get_samples_per_ts(&old_samples_per_ts);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_samples_per_ts);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "stts22h_ext_temp.sensor_annotation");
    ret = stts22h_ext_temp_set_sensor_annotation(sensor_annotation, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_sensor_annotation;
      stts22h_ext_temp_get_sensor_annotation(&old_sensor_annotation);
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.st_ble_stream.id"))
    {
      int32_t st_ble_stream__id = (int32_t)json_object_dotget_number(tempJSONObject, "stts22h_ext_temp.st_ble_stream.id");
      ret = stts22h_ext_temp_set_st_ble_stream__id(st_ble_stream__id, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__id);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__id;
        stts22h_ext_temp_get_st_ble_stream__id(&old_st_ble_stream__id);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__id);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.st_ble_stream.temp.enable"))
    {
      bool st_ble_stream__temp__enable = json_object_dotget_boolean(tempJSONObject,
                                                                    "stts22h_ext_temp.st_ble_stream.temp.enable");
      ret = stts22h_ext_temp_set_st_ble_stream__temp__enable(st_ble_stream__temp__enable, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", st_ble_stream__temp__enable);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_st_ble_stream__temp__enable;
        stts22h_ext_temp_get_st_ble_stream__temp__enable(&old_st_ble_stream__temp__enable);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_st_ble_stream__temp__enable);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.st_ble_stream.temp.unit"))
    {
      const char *st_ble_stream__temp__unit = json_object_dotget_string(tempJSONObject,
                                                                        "stts22h_ext_temp.st_ble_stream.temp.unit");
      ret = stts22h_ext_temp_set_st_ble_stream__temp__unit(st_ble_stream__temp__unit, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", st_ble_stream__temp__unit);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_st_ble_stream__temp__unit;
        stts22h_ext_temp_get_st_ble_stream__temp__unit(&old_st_ble_stream__temp__unit);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_st_ble_stream__temp__unit);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.st_ble_stream.temp.format"))
    {
      const char *st_ble_stream__temp__format = json_object_dotget_string(tempJSONObject,
                                                                          "stts22h_ext_temp.st_ble_stream.temp.format");
      ret = stts22h_ext_temp_set_st_ble_stream__temp__format(st_ble_stream__temp__format, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", st_ble_stream__temp__format);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_st_ble_stream__temp__format;
        stts22h_ext_temp_get_st_ble_stream__temp__format(&old_st_ble_stream__temp__format);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_st_ble_stream__temp__format);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.st_ble_stream.temp.elements"))
    {
      int32_t st_ble_stream__temp__elements = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                                 "stts22h_ext_temp.st_ble_stream.temp.elements");
      ret = stts22h_ext_temp_set_st_ble_stream__temp__elements(st_ble_stream__temp__elements, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__temp__elements);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__temp__elements;
        stts22h_ext_temp_get_st_ble_stream__temp__elements(&old_st_ble_stream__temp__elements);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__temp__elements);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.st_ble_stream.temp.channels"))
    {
      int32_t st_ble_stream__temp__channels = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                                 "stts22h_ext_temp.st_ble_stream.temp.channels");
      ret = stts22h_ext_temp_set_st_ble_stream__temp__channels(st_ble_stream__temp__channels, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__temp__channels);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__temp__channels;
        stts22h_ext_temp_get_st_ble_stream__temp__channels(&old_st_ble_stream__temp__channels);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__temp__channels);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.st_ble_stream.temp.multiply_factor"))
    {
      float st_ble_stream__temp__multiply_factor = (float)json_object_dotget_number(tempJSONObject,
                                                                                    "stts22h_ext_temp.st_ble_stream.temp.multiply_factor");
      ret = stts22h_ext_temp_set_st_ble_stream__temp__multiply_factor(st_ble_stream__temp__multiply_factor, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__temp__multiply_factor);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        float old_st_ble_stream__temp__multiply_factor;
        stts22h_ext_temp_get_st_ble_stream__temp__multiply_factor(&old_st_ble_stream__temp__multiply_factor);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__temp__multiply_factor);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "stts22h_ext_temp.st_ble_stream.temp.odr"))
    {
      int32_t st_ble_stream__temp__odr = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                            "stts22h_ext_temp.st_ble_stream.temp.odr");
      ret = stts22h_ext_temp_set_st_ble_stream__temp__odr(st_ble_stream__temp__odr, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__temp__odr);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__temp__odr;
        stts22h_ext_temp_get_st_ble_stream__temp__odr(&old_st_ble_stream__temp__odr);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__temp__odr);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
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


uint8_t Stts22h_Ext_Temp_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                  uint32_t *size, uint8_t pretty)
{
  return PNPL_NO_COMMANDS_ERROR_CODE;
}

