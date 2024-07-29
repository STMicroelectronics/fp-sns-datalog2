/**
  ******************************************************************************
  * @file    Lps22df_Press_PnPL.c
  * @author  SRA
  * @brief   Lps22df_Press PnPL Component Manager
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:lps22df_press;5
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

#include "Lps22df_Press_PnPL.h"

static const IPnPLComponent_vtbl sLps22df_Press_PnPL_CompIF_vtbl =
{
  Lps22df_Press_PnPL_vtblGetKey,
  Lps22df_Press_PnPL_vtblGetNCommands,
  Lps22df_Press_PnPL_vtblGetCommandKey,
  Lps22df_Press_PnPL_vtblGetStatus,
  Lps22df_Press_PnPL_vtblSetProperty,
  Lps22df_Press_PnPL_vtblExecuteFunction
};

/**
  *  Lps22df_Press_PnPL internal structure.
  */
struct _Lps22df_Press_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Lps22df_Press_PnPL sLps22df_Press_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Lps22df_Press_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sLps22df_Press_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sLps22df_Press_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Lps22df_Press_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  lps22df_press_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Lps22df_Press_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return lps22df_press_get_key();
}

uint8_t Lps22df_Press_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Lps22df_Press_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Lps22df_Press_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  pnpl_lps22df_press_odr_t temp_odr_e = (pnpl_lps22df_press_odr_t)0;
  lps22df_press_get_odr(&temp_odr_e);
  json_object_dotset_number(JSON_Status, "lps22df_press.odr", temp_odr_e);
  pnpl_lps22df_press_fs_t temp_fs_e = (pnpl_lps22df_press_fs_t)0;
  lps22df_press_get_fs(&temp_fs_e);
  json_object_dotset_number(JSON_Status, "lps22df_press.fs", temp_fs_e);
  bool temp_b = 0;
  lps22df_press_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "lps22df_press.enable", temp_b);
  int32_t temp_i = 0;
  lps22df_press_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "lps22df_press.samples_per_ts", temp_i);
  lps22df_press_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "lps22df_press.dim", temp_i);
  float temp_f = 0;
  lps22df_press_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "lps22df_press.ioffset", temp_f);
  lps22df_press_get_measodr(&temp_f);
  json_object_dotset_number(JSON_Status, "lps22df_press.measodr", temp_f);
  lps22df_press_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "lps22df_press.usb_dps", temp_i);
  lps22df_press_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "lps22df_press.sd_dps", temp_i);
  lps22df_press_get_sensitivity(&temp_f);
  json_object_dotset_number(JSON_Status, "lps22df_press.sensitivity", temp_f);
  char *temp_s = "";
  lps22df_press_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "lps22df_press.data_type", temp_s);
  lps22df_press_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "lps22df_press.sensor_annotation", temp_s);
  lps22df_press_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "lps22df_press.sensor_category", temp_i);
  lps22df_press_get_st_ble_stream__id(&temp_i);
  json_object_dotset_number(JSON_Status, "lps22df_press.st_ble_stream.id", temp_i);
  lps22df_press_get_st_ble_stream__press__enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "lps22df_press.st_ble_stream.press.enable", temp_b);
  lps22df_press_get_st_ble_stream__press__unit(&temp_s);
  json_object_dotset_string(JSON_Status, "lps22df_press.st_ble_stream.press.unit", temp_s);
  lps22df_press_get_st_ble_stream__press__format(&temp_s);
  json_object_dotset_string(JSON_Status, "lps22df_press.st_ble_stream.press.format", temp_s);
  lps22df_press_get_st_ble_stream__press__elements(&temp_i);
  json_object_dotset_number(JSON_Status, "lps22df_press.st_ble_stream.press.elements", temp_i);
  lps22df_press_get_st_ble_stream__press__channels(&temp_i);
  json_object_dotset_number(JSON_Status, "lps22df_press.st_ble_stream.press.channels", temp_i);
  lps22df_press_get_st_ble_stream__press__multiply_factor(&temp_f);
  json_object_dotset_number(JSON_Status, "lps22df_press.st_ble_stream.press.multiply_factor", temp_f);
  lps22df_press_get_st_ble_stream__press__odr(&temp_i);
  json_object_dotset_number(JSON_Status, "lps22df_press.st_ble_stream.press.odr", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "lps22df_press.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  lps22df_press_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "lps22df_press.stream_id", temp_int8);
  lps22df_press_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "lps22df_press.ep_id", temp_int8);

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

uint8_t Lps22df_Press_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                           uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "lps22df_press.odr"))
  {
    int32_t odr = (int32_t)json_object_dotget_number(tempJSONObject, "lps22df_press.odr");
    ret = lps22df_press_set_odr((pnpl_lps22df_press_odr_t)odr, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", odr);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_lps22df_press_odr_t old_odr;
      lps22df_press_get_odr(&old_odr);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_odr);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lps22df_press.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "lps22df_press.enable");
    ret = lps22df_press_set_enable(enable, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_enable;
      lps22df_press_get_enable(&old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lps22df_press.samples_per_ts"))
  {
    int32_t samples_per_ts = (int32_t)json_object_dotget_number(tempJSONObject, "lps22df_press.samples_per_ts");
    ret = lps22df_press_set_samples_per_ts(samples_per_ts, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", samples_per_ts);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_samples_per_ts;
      lps22df_press_get_samples_per_ts(&old_samples_per_ts);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_samples_per_ts);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lps22df_press.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "lps22df_press.sensor_annotation");
    ret = lps22df_press_set_sensor_annotation(sensor_annotation, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_sensor_annotation;
      lps22df_press_get_sensor_annotation(&old_sensor_annotation);
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lps22df_press.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "lps22df_press.st_ble_stream.id"))
    {
      int32_t st_ble_stream__id = (int32_t)json_object_dotget_number(tempJSONObject, "lps22df_press.st_ble_stream.id");
      ret = lps22df_press_set_st_ble_stream__id(st_ble_stream__id, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__id);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__id;
        lps22df_press_get_st_ble_stream__id(&old_st_ble_stream__id);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__id);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lps22df_press.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "lps22df_press.st_ble_stream.press.enable"))
    {
      bool st_ble_stream__press__enable = json_object_dotget_boolean(tempJSONObject,
                                                                     "lps22df_press.st_ble_stream.press.enable");
      ret = lps22df_press_set_st_ble_stream__press__enable(st_ble_stream__press__enable, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", st_ble_stream__press__enable);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        bool old_st_ble_stream__press__enable;
        lps22df_press_get_st_ble_stream__press__enable(&old_st_ble_stream__press__enable);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_st_ble_stream__press__enable);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lps22df_press.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "lps22df_press.st_ble_stream.press.unit"))
    {
      const char *st_ble_stream__press__unit = json_object_dotget_string(tempJSONObject,
                                                                         "lps22df_press.st_ble_stream.press.unit");
      ret = lps22df_press_set_st_ble_stream__press__unit(st_ble_stream__press__unit, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", st_ble_stream__press__unit);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_st_ble_stream__press__unit;
        lps22df_press_get_st_ble_stream__press__unit(&old_st_ble_stream__press__unit);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_st_ble_stream__press__unit);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lps22df_press.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "lps22df_press.st_ble_stream.press.format"))
    {
      const char *st_ble_stream__press__format = json_object_dotget_string(tempJSONObject,
                                                                           "lps22df_press.st_ble_stream.press.format");
      ret = lps22df_press_set_st_ble_stream__press__format(st_ble_stream__press__format, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", st_ble_stream__press__format);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        char *old_st_ble_stream__press__format;
        lps22df_press_get_st_ble_stream__press__format(&old_st_ble_stream__press__format);
        json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_st_ble_stream__press__format);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lps22df_press.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "lps22df_press.st_ble_stream.press.elements"))
    {
      int32_t st_ble_stream__press__elements = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                                  "lps22df_press.st_ble_stream.press.elements");
      ret = lps22df_press_set_st_ble_stream__press__elements(st_ble_stream__press__elements, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__press__elements);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__press__elements;
        lps22df_press_get_st_ble_stream__press__elements(&old_st_ble_stream__press__elements);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__press__elements);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lps22df_press.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "lps22df_press.st_ble_stream.press.channels"))
    {
      int32_t st_ble_stream__press__channels = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                                  "lps22df_press.st_ble_stream.press.channels");
      ret = lps22df_press_set_st_ble_stream__press__channels(st_ble_stream__press__channels, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__press__channels);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__press__channels;
        lps22df_press_get_st_ble_stream__press__channels(&old_st_ble_stream__press__channels);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__press__channels);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lps22df_press.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "lps22df_press.st_ble_stream.press.multiply_factor"))
    {
      float st_ble_stream__press__multiply_factor = (float)json_object_dotget_number(tempJSONObject,
                                                                                     "lps22df_press.st_ble_stream.press.multiply_factor");
      ret = lps22df_press_set_st_ble_stream__press__multiply_factor(st_ble_stream__press__multiply_factor, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__press__multiply_factor);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        float old_st_ble_stream__press__multiply_factor;
        lps22df_press_get_st_ble_stream__press__multiply_factor(&old_st_ble_stream__press__multiply_factor);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__press__multiply_factor);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
      }
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lps22df_press.st_ble_stream"))
  {
    if (json_object_dothas_value(tempJSONObject, "lps22df_press.st_ble_stream.press.odr"))
    {
      int32_t st_ble_stream__press__odr = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                             "lps22df_press.st_ble_stream.press.odr");
      ret = lps22df_press_set_st_ble_stream__press__odr(st_ble_stream__press__odr, &resp_msg);
      json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
      if (ret == PNPL_NO_ERROR_CODE)
      {
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", st_ble_stream__press__odr);
        json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
      }
      else
      {
        int32_t old_st_ble_stream__press__odr;
        lps22df_press_get_st_ble_stream__press__odr(&old_st_ble_stream__press__odr);
        json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_st_ble_stream__press__odr);
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


uint8_t Lps22df_Press_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                               uint32_t *size, uint8_t pretty)
{
  return PNPL_NO_COMMANDS_ERROR_CODE;
}

