/**
  ******************************************************************************
  * @file    Ism330is_Gyro_PnPL.c
  * @author  SRA
  * @brief   Ism330is_Gyro PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:sensors:ism330is_gyro;2
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

#include "Ism330is_Gyro_PnPL.h"

static const IPnPLComponent_vtbl sIsm330is_Gyro_PnPL_CompIF_vtbl =
{
  Ism330is_Gyro_PnPL_vtblGetKey,
  Ism330is_Gyro_PnPL_vtblGetNCommands,
  Ism330is_Gyro_PnPL_vtblGetCommandKey,
  Ism330is_Gyro_PnPL_vtblGetStatus,
  Ism330is_Gyro_PnPL_vtblSetProperty,
  Ism330is_Gyro_PnPL_vtblExecuteFunction
};

/**
  *  Ism330is_Gyro_PnPL internal structure.
  */
struct _Ism330is_Gyro_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Ism330is_Gyro_PnPL sIsm330is_Gyro_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Ism330is_Gyro_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sIsm330is_Gyro_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sIsm330is_Gyro_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Ism330is_Gyro_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  ism330is_gyro_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Ism330is_Gyro_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return ism330is_gyro_get_key();
}

uint8_t Ism330is_Gyro_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Ism330is_Gyro_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Ism330is_Gyro_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  pnpl_ism330is_gyro_odr_t temp_odr_e = (pnpl_ism330is_gyro_odr_t)0;
  ism330is_gyro_get_odr(&temp_odr_e);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.odr", temp_odr_e);
  pnpl_ism330is_gyro_fs_t temp_fs_e = (pnpl_ism330is_gyro_fs_t)0;
  ism330is_gyro_get_fs(&temp_fs_e);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.fs", temp_fs_e);
  bool temp_b = 0;
  ism330is_gyro_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "ism330is_gyro.enable", temp_b);
  int32_t temp_i = 0;
  ism330is_gyro_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.samples_per_ts", temp_i);
  ism330is_gyro_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.dim", temp_i);
  float temp_f = 0;
  ism330is_gyro_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.ioffset", temp_f);
  ism330is_gyro_get_measodr(&temp_f);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.measodr", temp_f);
  ism330is_gyro_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.usb_dps", temp_i);
  ism330is_gyro_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.sd_dps", temp_i);
  ism330is_gyro_get_sensitivity(&temp_f);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.sensitivity", temp_f);
  char *temp_s = "";
  ism330is_gyro_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "ism330is_gyro.data_type", temp_s);
  ism330is_gyro_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "ism330is_gyro.sensor_annotation", temp_s);
  ism330is_gyro_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.sensor_category", temp_i);
  ism330is_gyro_get_mounted(&temp_b);
  json_object_dotset_boolean(JSON_Status, "ism330is_gyro.mounted", temp_b);
  ism330is_gyro_get_st_ble_stream__id(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.st_ble_stream.id", temp_i);
  ism330is_gyro_get_st_ble_stream__gyro__enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "ism330is_gyro.st_ble_stream.gyro.enable", temp_b);
  ism330is_gyro_get_st_ble_stream__gyro__unit(&temp_s);
  json_object_dotset_string(JSON_Status, "ism330is_gyro.st_ble_stream.gyro.unit", temp_s);
  ism330is_gyro_get_st_ble_stream__gyro__format(&temp_s);
  json_object_dotset_string(JSON_Status, "ism330is_gyro.st_ble_stream.gyro.format", temp_s);
  ism330is_gyro_get_st_ble_stream__gyro__elements(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.st_ble_stream.gyro.elements", temp_i);
  ism330is_gyro_get_st_ble_stream__gyro__channels(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.st_ble_stream.gyro.channels", temp_i);
  ism330is_gyro_get_st_ble_stream__gyro__multiply_factor(&temp_f);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.st_ble_stream.gyro.multiply_factor", temp_f);
  ism330is_gyro_get_st_ble_stream__gyro__odr(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.st_ble_stream.gyro.odr", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "ism330is_gyro.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  ism330is_gyro_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.stream_id", temp_int8);
  ism330is_gyro_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "ism330is_gyro.ep_id", temp_int8);

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

uint8_t Ism330is_Gyro_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                           uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  bool valid_property = false;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.odr"))
  {
    valid_property = true;
    int32_t odr = (int32_t)json_object_dotget_number(tempJSONObject, "ism330is_gyro.odr");
    ret = ism330is_gyro_set_odr((pnpl_ism330is_gyro_odr_t)odr, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", odr);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_ism330is_gyro_odr_t old_odr;
      ism330is_gyro_get_odr(&old_odr);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_odr);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.fs"))
  {
    valid_property = true;
    int32_t fs = (int32_t)json_object_dotget_number(tempJSONObject, "ism330is_gyro.fs");
    ret = ism330is_gyro_set_fs((pnpl_ism330is_gyro_fs_t)fs, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", fs);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_ism330is_gyro_fs_t old_fs;
      ism330is_gyro_get_fs(&old_fs);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_fs);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.enable"))
  {
    valid_property = true;
    bool enable = json_object_dotget_boolean(tempJSONObject, "ism330is_gyro.enable");
    ret = ism330is_gyro_set_enable(enable, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_enable;
      ism330is_gyro_get_enable(&old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.samples_per_ts"))
  {
    valid_property = true;
    int32_t samples_per_ts = (int32_t)json_object_dotget_number(tempJSONObject, "ism330is_gyro.samples_per_ts");
    ret = ism330is_gyro_set_samples_per_ts(samples_per_ts, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", samples_per_ts);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_samples_per_ts;
      ism330is_gyro_get_samples_per_ts(&old_samples_per_ts);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_samples_per_ts);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.sensor_annotation"))
  {
    valid_property = true;
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "ism330is_gyro.sensor_annotation");
    ret = ism330is_gyro_set_sensor_annotation(sensor_annotation, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_sensor_annotation;
      ism330is_gyro_get_sensor_annotation(&old_sensor_annotation);
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.st_ble_stream.id"))
    {
      valid_property = true;
      int32_t st_ble_stream__id = (int32_t)json_object_dotget_number(tempJSONObject, "ism330is_gyro.st_ble_stream.id");
      ret = ism330is_gyro_set_st_ble_stream__id(st_ble_stream__id, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__id);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__id;
        ism330is_gyro_get_st_ble_stream__id(&old_st_ble_stream__id);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__id);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.st_ble_stream.gyro.enable"))
    {
      valid_property = true;
      bool st_ble_stream__gyro__enable = json_object_dotget_boolean(tempJSONObject, "ism330is_gyro.st_ble_stream.gyro.enable");
      ret = ism330is_gyro_set_st_ble_stream__gyro__enable(st_ble_stream__gyro__enable, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", st_ble_stream__gyro__enable);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_st_ble_stream__gyro__enable;
        ism330is_gyro_get_st_ble_stream__gyro__enable(&old_st_ble_stream__gyro__enable);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_st_ble_stream__gyro__enable);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.st_ble_stream.gyro.unit"))
    {
      valid_property = true;
      const char *st_ble_stream__gyro__unit = json_object_dotget_string(tempJSONObject,
                                                                        "ism330is_gyro.st_ble_stream.gyro.unit");
      ret = ism330is_gyro_set_st_ble_stream__gyro__unit(st_ble_stream__gyro__unit, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", st_ble_stream__gyro__unit);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_st_ble_stream__gyro__unit;
        ism330is_gyro_get_st_ble_stream__gyro__unit(&old_st_ble_stream__gyro__unit);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_st_ble_stream__gyro__unit);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.st_ble_stream.gyro.format"))
    {
      valid_property = true;
      const char *st_ble_stream__gyro__format = json_object_dotget_string(tempJSONObject,
                                                                          "ism330is_gyro.st_ble_stream.gyro.format");
      ret = ism330is_gyro_set_st_ble_stream__gyro__format(st_ble_stream__gyro__format, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", st_ble_stream__gyro__format);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_st_ble_stream__gyro__format;
        ism330is_gyro_get_st_ble_stream__gyro__format(&old_st_ble_stream__gyro__format);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_st_ble_stream__gyro__format);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.st_ble_stream.gyro.elements"))
    {
      valid_property = true;
      int32_t st_ble_stream__gyro__elements = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                                 "ism330is_gyro.st_ble_stream.gyro.elements");
      ret = ism330is_gyro_set_st_ble_stream__gyro__elements(st_ble_stream__gyro__elements, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__gyro__elements);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__gyro__elements;
        ism330is_gyro_get_st_ble_stream__gyro__elements(&old_st_ble_stream__gyro__elements);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__gyro__elements);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.st_ble_stream.gyro.channels"))
    {
      valid_property = true;
      int32_t st_ble_stream__gyro__channels = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                                 "ism330is_gyro.st_ble_stream.gyro.channels");
      ret = ism330is_gyro_set_st_ble_stream__gyro__channels(st_ble_stream__gyro__channels, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__gyro__channels);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__gyro__channels;
        ism330is_gyro_get_st_ble_stream__gyro__channels(&old_st_ble_stream__gyro__channels);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__gyro__channels);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.st_ble_stream.gyro.multiply_factor"))
    {
      valid_property = true;
      float st_ble_stream__gyro__multiply_factor = (float)json_object_dotget_number(tempJSONObject,
                                                                                    "ism330is_gyro.st_ble_stream.gyro.multiply_factor");
      ret = ism330is_gyro_set_st_ble_stream__gyro__multiply_factor(st_ble_stream__gyro__multiply_factor, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__gyro__multiply_factor);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        float old_st_ble_stream__gyro__multiply_factor;
        ism330is_gyro_get_st_ble_stream__gyro__multiply_factor(&old_st_ble_stream__gyro__multiply_factor);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__gyro__multiply_factor);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "ism330is_gyro.st_ble_stream.gyro.odr"))
    {
      valid_property = true;
      int32_t st_ble_stream__gyro__odr = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                            "ism330is_gyro.st_ble_stream.gyro.odr");
      ret = ism330is_gyro_set_st_ble_stream__gyro__odr(st_ble_stream__gyro__odr, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__gyro__odr);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__gyro__odr;
        ism330is_gyro_get_st_ble_stream__gyro__odr(&old_st_ble_stream__gyro__odr);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__gyro__odr);
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
    char *log_message = "Invalid property for Ism330is_Gyro";
    PnPLCreateLogMessage(response, size, log_message, PNPL_LOG_ERROR);
  }
  json_value_free(respJSON);
  return ret;
}


uint8_t Ism330is_Gyro_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                               uint32_t *size, uint8_t pretty)
{
  return PNPL_NO_COMMANDS_ERROR_CODE;
}

