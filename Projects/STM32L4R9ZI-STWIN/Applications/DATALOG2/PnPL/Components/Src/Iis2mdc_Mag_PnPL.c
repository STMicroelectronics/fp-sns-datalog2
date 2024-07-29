/**
  ******************************************************************************
  * @file    Iis2mdc_Mag_PnPL.c
  * @author  SRA
  * @brief   Iis2mdc_Mag PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinkt1b:fpSnsDatalog2_datalog2:sensors:iis2mdc_mag;2
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

#include "Iis2mdc_Mag_PnPL.h"

static const IPnPLComponent_vtbl sIis2mdc_Mag_PnPL_CompIF_vtbl =
{
  Iis2mdc_Mag_PnPL_vtblGetKey,
  Iis2mdc_Mag_PnPL_vtblGetNCommands,
  Iis2mdc_Mag_PnPL_vtblGetCommandKey,
  Iis2mdc_Mag_PnPL_vtblGetStatus,
  Iis2mdc_Mag_PnPL_vtblSetProperty,
  Iis2mdc_Mag_PnPL_vtblExecuteFunction
};

/**
  *  Iis2mdc_Mag_PnPL internal structure.
  */
struct _Iis2mdc_Mag_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Iis2mdc_Mag_PnPL sIis2mdc_Mag_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Iis2mdc_Mag_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sIis2mdc_Mag_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sIis2mdc_Mag_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Iis2mdc_Mag_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  iis2mdc_mag_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Iis2mdc_Mag_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return iis2mdc_mag_get_key();
}

uint8_t Iis2mdc_Mag_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Iis2mdc_Mag_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Iis2mdc_Mag_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  pnpl_iis2mdc_mag_odr_t temp_odr_e = (pnpl_iis2mdc_mag_odr_t)0;
  iis2mdc_mag_get_odr(&temp_odr_e);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.odr", temp_odr_e);
  pnpl_iis2mdc_mag_fs_t temp_fs_e = (pnpl_iis2mdc_mag_fs_t)0;
  iis2mdc_mag_get_fs(&temp_fs_e);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.fs", temp_fs_e);
  bool temp_b = 0;
  iis2mdc_mag_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "iis2mdc_mag.enable", temp_b);
  int32_t temp_i = 0;
  iis2mdc_mag_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.samples_per_ts", temp_i);
  iis2mdc_mag_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.dim", temp_i);
  float temp_f = 0;
  iis2mdc_mag_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.ioffset", temp_f);
  iis2mdc_mag_get_measodr(&temp_f);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.measodr", temp_f);
  iis2mdc_mag_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.usb_dps", temp_i);
  iis2mdc_mag_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.sd_dps", temp_i);
  iis2mdc_mag_get_sensitivity(&temp_f);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.sensitivity", temp_f);
  char *temp_s = "";
  iis2mdc_mag_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "iis2mdc_mag.data_type", temp_s);
  iis2mdc_mag_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "iis2mdc_mag.sensor_annotation", temp_s);
  iis2mdc_mag_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.sensor_category", temp_i);
  iis2mdc_mag_get_st_ble_stream__id(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.st_ble_stream.id", temp_i);
  iis2mdc_mag_get_st_ble_stream__mag__enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "iis2mdc_mag.st_ble_stream.mag.enable", temp_b);
  iis2mdc_mag_get_st_ble_stream__mag__unit(&temp_s);
  json_object_dotset_string(JSON_Status, "iis2mdc_mag.st_ble_stream.mag.unit", temp_s);
  iis2mdc_mag_get_st_ble_stream__mag__format(&temp_s);
  json_object_dotset_string(JSON_Status, "iis2mdc_mag.st_ble_stream.mag.format", temp_s);
  iis2mdc_mag_get_st_ble_stream__mag__elements(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.st_ble_stream.mag.elements", temp_i);
  iis2mdc_mag_get_st_ble_stream__mag__channels(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.st_ble_stream.mag.channels", temp_i);
  iis2mdc_mag_get_st_ble_stream__mag__multiply_factor(&temp_f);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.st_ble_stream.mag.multiply_factor", temp_f);
  iis2mdc_mag_get_st_ble_stream__mag__odr(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.st_ble_stream.mag.odr", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  iis2mdc_mag_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.stream_id", temp_int8);
  iis2mdc_mag_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.ep_id", temp_int8);

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

uint8_t Iis2mdc_Mag_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size,
                                         uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.odr"))
  {
    int32_t odr = (int32_t)json_object_dotget_number(tempJSONObject, "iis2mdc_mag.odr");
    ret = iis2mdc_mag_set_odr((pnpl_iis2mdc_mag_odr_t)odr, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", odr);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_iis2mdc_mag_odr_t old_odr;
      iis2mdc_mag_get_odr(&old_odr);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_odr);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "iis2mdc_mag.enable");
    ret = iis2mdc_mag_set_enable(enable, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_enable;
      iis2mdc_mag_get_enable(&old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.samples_per_ts"))
  {
    int32_t samples_per_ts = (int32_t)json_object_dotget_number(tempJSONObject, "iis2mdc_mag.samples_per_ts");
    ret = iis2mdc_mag_set_samples_per_ts(samples_per_ts, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", samples_per_ts);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_samples_per_ts;
      iis2mdc_mag_get_samples_per_ts(&old_samples_per_ts);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_samples_per_ts);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "iis2mdc_mag.sensor_annotation");
    ret = iis2mdc_mag_set_sensor_annotation(sensor_annotation, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_sensor_annotation;
      iis2mdc_mag_get_sensor_annotation(&old_sensor_annotation);
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.st_ble_stream.id"))
    {
      int32_t st_ble_stream__id = (int32_t)json_object_dotget_number(tempJSONObject, "iis2mdc_mag.st_ble_stream.id");
      ret = iis2mdc_mag_set_st_ble_stream__id(st_ble_stream__id, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__id);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__id;
        iis2mdc_mag_get_st_ble_stream__id(&old_st_ble_stream__id);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__id);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.st_ble_stream.mag.enable"))
    {
      bool st_ble_stream__mag__enable = json_object_dotget_boolean(tempJSONObject, "iis2mdc_mag.st_ble_stream.mag.enable");
      ret = iis2mdc_mag_set_st_ble_stream__mag__enable(st_ble_stream__mag__enable, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", st_ble_stream__mag__enable);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_st_ble_stream__mag__enable;
        iis2mdc_mag_get_st_ble_stream__mag__enable(&old_st_ble_stream__mag__enable);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_st_ble_stream__mag__enable);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.st_ble_stream.mag.unit"))
    {
      const char *st_ble_stream__mag__unit = json_object_dotget_string(tempJSONObject, "iis2mdc_mag.st_ble_stream.mag.unit");
      ret = iis2mdc_mag_set_st_ble_stream__mag__unit(st_ble_stream__mag__unit, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", st_ble_stream__mag__unit);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_st_ble_stream__mag__unit;
        iis2mdc_mag_get_st_ble_stream__mag__unit(&old_st_ble_stream__mag__unit);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_st_ble_stream__mag__unit);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.st_ble_stream.mag.format"))
    {
      const char *st_ble_stream__mag__format = json_object_dotget_string(tempJSONObject,
                                                                         "iis2mdc_mag.st_ble_stream.mag.format");
      ret = iis2mdc_mag_set_st_ble_stream__mag__format(st_ble_stream__mag__format, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", st_ble_stream__mag__format);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_st_ble_stream__mag__format;
        iis2mdc_mag_get_st_ble_stream__mag__format(&old_st_ble_stream__mag__format);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_st_ble_stream__mag__format);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.st_ble_stream.mag.elements"))
    {
      int32_t st_ble_stream__mag__elements = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                                "iis2mdc_mag.st_ble_stream.mag.elements");
      ret = iis2mdc_mag_set_st_ble_stream__mag__elements(st_ble_stream__mag__elements, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__mag__elements);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__mag__elements;
        iis2mdc_mag_get_st_ble_stream__mag__elements(&old_st_ble_stream__mag__elements);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__mag__elements);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.st_ble_stream.mag.channels"))
    {
      int32_t st_ble_stream__mag__channels = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                                "iis2mdc_mag.st_ble_stream.mag.channels");
      ret = iis2mdc_mag_set_st_ble_stream__mag__channels(st_ble_stream__mag__channels, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__mag__channels);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__mag__channels;
        iis2mdc_mag_get_st_ble_stream__mag__channels(&old_st_ble_stream__mag__channels);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__mag__channels);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.st_ble_stream.mag.multiply_factor"))
    {
      float st_ble_stream__mag__multiply_factor = (float)json_object_dotget_number(tempJSONObject,
                                                                                   "iis2mdc_mag.st_ble_stream.mag.multiply_factor");
      ret = iis2mdc_mag_set_st_ble_stream__mag__multiply_factor(st_ble_stream__mag__multiply_factor, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__mag__multiply_factor);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        float old_st_ble_stream__mag__multiply_factor;
        iis2mdc_mag_get_st_ble_stream__mag__multiply_factor(&old_st_ble_stream__mag__multiply_factor);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__mag__multiply_factor);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.st_ble_stream.mag.odr"))
    {
      int32_t st_ble_stream__mag__odr = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                           "iis2mdc_mag.st_ble_stream.mag.odr");
      ret = iis2mdc_mag_set_st_ble_stream__mag__odr(st_ble_stream__mag__odr, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__mag__odr);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__mag__odr;
        iis2mdc_mag_get_st_ble_stream__mag__odr(&old_st_ble_stream__mag__odr);
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


uint8_t Iis2mdc_Mag_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                             uint32_t *size, uint8_t pretty)
{
  return PNPL_NO_COMMANDS_ERROR_CODE;
}

