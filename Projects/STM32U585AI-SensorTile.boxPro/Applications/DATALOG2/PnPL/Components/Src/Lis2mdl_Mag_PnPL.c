/**
  ******************************************************************************
  * @file    Lis2mdl_Mag_PnPL.c
  * @author  SRA
  * @brief   Lis2mdl_Mag PnPL Component Manager
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:lis2mdl_mag;5
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

#include "Lis2mdl_Mag_PnPL.h"

static const IPnPLComponent_vtbl sLis2mdl_Mag_PnPL_CompIF_vtbl =
{
  Lis2mdl_Mag_PnPL_vtblGetKey,
  Lis2mdl_Mag_PnPL_vtblGetNCommands,
  Lis2mdl_Mag_PnPL_vtblGetCommandKey,
  Lis2mdl_Mag_PnPL_vtblGetStatus,
  Lis2mdl_Mag_PnPL_vtblSetProperty,
  Lis2mdl_Mag_PnPL_vtblExecuteFunction
};

/**
  *  Lis2mdl_Mag_PnPL internal structure.
  */
struct _Lis2mdl_Mag_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Lis2mdl_Mag_PnPL sLis2mdl_Mag_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Lis2mdl_Mag_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sLis2mdl_Mag_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sLis2mdl_Mag_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Lis2mdl_Mag_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  lis2mdl_mag_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Lis2mdl_Mag_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return lis2mdl_mag_get_key();
}

uint8_t Lis2mdl_Mag_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Lis2mdl_Mag_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Lis2mdl_Mag_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  pnpl_lis2mdl_mag_odr_t temp_odr_e = (pnpl_lis2mdl_mag_odr_t)0;
  lis2mdl_mag_get_odr(&temp_odr_e);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.odr", temp_odr_e);
  pnpl_lis2mdl_mag_fs_t temp_fs_e = (pnpl_lis2mdl_mag_fs_t)0;
  lis2mdl_mag_get_fs(&temp_fs_e);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.fs", temp_fs_e);
  bool temp_b = 0;
  lis2mdl_mag_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "lis2mdl_mag.enable", temp_b);
  int32_t temp_i = 0;
  lis2mdl_mag_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.samples_per_ts", temp_i);
  lis2mdl_mag_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.dim", temp_i);
  float temp_f = 0;
  lis2mdl_mag_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.ioffset", temp_f);
  lis2mdl_mag_get_measodr(&temp_f);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.measodr", temp_f);
  lis2mdl_mag_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.usb_dps", temp_i);
  lis2mdl_mag_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.sd_dps", temp_i);
  lis2mdl_mag_get_sensitivity(&temp_f);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.sensitivity", temp_f);
  char *temp_s = "";
  lis2mdl_mag_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "lis2mdl_mag.data_type", temp_s);
  lis2mdl_mag_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "lis2mdl_mag.sensor_annotation", temp_s);
  lis2mdl_mag_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.sensor_category", temp_i);
  lis2mdl_mag_get_st_ble_stream__id(&temp_i);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.st_ble_stream.id", temp_i);
  lis2mdl_mag_get_st_ble_stream__mag__enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "lis2mdl_mag.st_ble_stream.mag.enable", temp_b);
  lis2mdl_mag_get_st_ble_stream__mag__unit(&temp_s);
  json_object_dotset_string(JSON_Status, "lis2mdl_mag.st_ble_stream.mag.unit", temp_s);
  lis2mdl_mag_get_st_ble_stream__mag__format(&temp_s);
  json_object_dotset_string(JSON_Status, "lis2mdl_mag.st_ble_stream.mag.format", temp_s);
  lis2mdl_mag_get_st_ble_stream__mag__elements(&temp_i);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.st_ble_stream.mag.elements", temp_i);
  lis2mdl_mag_get_st_ble_stream__mag__channels(&temp_i);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.st_ble_stream.mag.channels", temp_i);
  lis2mdl_mag_get_st_ble_stream__mag__multiply_factor(&temp_f);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.st_ble_stream.mag.multiply_factor", temp_f);
  lis2mdl_mag_get_st_ble_stream__mag__odr(&temp_i);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.st_ble_stream.mag.odr", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  lis2mdl_mag_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.stream_id", temp_int8);
  lis2mdl_mag_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.ep_id", temp_int8);

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

uint8_t Lis2mdl_Mag_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size,
                                         uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.odr"))
  {
    int32_t odr = (int32_t)json_object_dotget_number(tempJSONObject, "lis2mdl_mag.odr");
    ret = lis2mdl_mag_set_odr((pnpl_lis2mdl_mag_odr_t)odr, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", odr);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_lis2mdl_mag_odr_t old_odr;
      lis2mdl_mag_get_odr(&old_odr);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_odr);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "lis2mdl_mag.enable");
    ret = lis2mdl_mag_set_enable(enable, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_enable;
      lis2mdl_mag_get_enable(&old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.samples_per_ts"))
  {
    int32_t samples_per_ts = (int32_t)json_object_dotget_number(tempJSONObject, "lis2mdl_mag.samples_per_ts");
    ret = lis2mdl_mag_set_samples_per_ts(samples_per_ts, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", samples_per_ts);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_samples_per_ts;
      lis2mdl_mag_get_samples_per_ts(&old_samples_per_ts);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_samples_per_ts);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "lis2mdl_mag.sensor_annotation");
    ret = lis2mdl_mag_set_sensor_annotation(sensor_annotation, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_sensor_annotation;
      lis2mdl_mag_get_sensor_annotation(&old_sensor_annotation);
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.st_ble_stream.id"))
    {
      int32_t st_ble_stream__id = (int32_t)json_object_dotget_number(tempJSONObject, "lis2mdl_mag.st_ble_stream.id");
      ret = lis2mdl_mag_set_st_ble_stream__id(st_ble_stream__id, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__id);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__id;
        lis2mdl_mag_get_st_ble_stream__id(&old_st_ble_stream__id);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__id);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.st_ble_stream.mag.enable"))
    {
      bool st_ble_stream__mag__enable = json_object_dotget_boolean(tempJSONObject, "lis2mdl_mag.st_ble_stream.mag.enable");
      ret = lis2mdl_mag_set_st_ble_stream__mag__enable(st_ble_stream__mag__enable, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", st_ble_stream__mag__enable);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_st_ble_stream__mag__enable;
        lis2mdl_mag_get_st_ble_stream__mag__enable(&old_st_ble_stream__mag__enable);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_st_ble_stream__mag__enable);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.st_ble_stream.mag.unit"))
    {
      const char *st_ble_stream__mag__unit = json_object_dotget_string(tempJSONObject, "lis2mdl_mag.st_ble_stream.mag.unit");
      ret = lis2mdl_mag_set_st_ble_stream__mag__unit(st_ble_stream__mag__unit, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", st_ble_stream__mag__unit);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_st_ble_stream__mag__unit;
        lis2mdl_mag_get_st_ble_stream__mag__unit(&old_st_ble_stream__mag__unit);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_st_ble_stream__mag__unit);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.st_ble_stream.mag.format"))
    {
      const char *st_ble_stream__mag__format = json_object_dotget_string(tempJSONObject,
                                                                         "lis2mdl_mag.st_ble_stream.mag.format");
      ret = lis2mdl_mag_set_st_ble_stream__mag__format(st_ble_stream__mag__format, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", st_ble_stream__mag__format);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_st_ble_stream__mag__format;
        lis2mdl_mag_get_st_ble_stream__mag__format(&old_st_ble_stream__mag__format);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_st_ble_stream__mag__format);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.st_ble_stream.mag.elements"))
    {
      int32_t st_ble_stream__mag__elements = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                                "lis2mdl_mag.st_ble_stream.mag.elements");
      ret = lis2mdl_mag_set_st_ble_stream__mag__elements(st_ble_stream__mag__elements, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__mag__elements);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__mag__elements;
        lis2mdl_mag_get_st_ble_stream__mag__elements(&old_st_ble_stream__mag__elements);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__mag__elements);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.st_ble_stream.mag.channels"))
    {
      int32_t st_ble_stream__mag__channels = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                                "lis2mdl_mag.st_ble_stream.mag.channels");
      ret = lis2mdl_mag_set_st_ble_stream__mag__channels(st_ble_stream__mag__channels, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__mag__channels);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__mag__channels;
        lis2mdl_mag_get_st_ble_stream__mag__channels(&old_st_ble_stream__mag__channels);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__mag__channels);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.st_ble_stream.mag.multiply_factor"))
    {
      float st_ble_stream__mag__multiply_factor = (float)json_object_dotget_number(tempJSONObject,
                                                                                   "lis2mdl_mag.st_ble_stream.mag.multiply_factor");
      ret = lis2mdl_mag_set_st_ble_stream__mag__multiply_factor(st_ble_stream__mag__multiply_factor, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__mag__multiply_factor);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        float old_st_ble_stream__mag__multiply_factor;
        lis2mdl_mag_get_st_ble_stream__mag__multiply_factor(&old_st_ble_stream__mag__multiply_factor);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__mag__multiply_factor);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.st_ble_stream.mag.odr"))
    {
      int32_t st_ble_stream__mag__odr = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                           "lis2mdl_mag.st_ble_stream.mag.odr");
      ret = lis2mdl_mag_set_st_ble_stream__mag__odr(st_ble_stream__mag__odr, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__mag__odr);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__mag__odr;
        lis2mdl_mag_get_st_ble_stream__mag__odr(&old_st_ble_stream__mag__odr);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__mag__odr);
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


uint8_t Lis2mdl_Mag_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                             uint32_t *size, uint8_t pretty)
{
  return PNPL_NO_COMMANDS_ERROR_CODE;
}

