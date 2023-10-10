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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:lps22df_press;3
  *
  * Created by: DTDL2PnPL_cGen version 1.2.0
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
#include "Lps22df_Press_PnPL_vtbl.h"

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
  /**
    * Implements the IPnPLComponent interface.
    */
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
  return 0;
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

  float temp_f = 0;
  lps22df_press_get_odr(&temp_f);
  uint8_t enum_id = 0;
  if(temp_f == lps22df_press_odr_hz1)
  {
    enum_id = 0;
  }
  else if(temp_f == lps22df_press_odr_hz4)
  {
    enum_id = 1;
  }
  else if(temp_f == lps22df_press_odr_hz10)
  {
    enum_id = 2;
  }
  else if(temp_f == lps22df_press_odr_hz25)
  {
    enum_id = 3;
  }
  else if(temp_f == lps22df_press_odr_hz50)
  {
    enum_id = 4;
  }
  else if(temp_f == lps22df_press_odr_hz75)
  {
    enum_id = 5;
  }
  else if(temp_f == lps22df_press_odr_hz100)
  {
    enum_id = 6;
  }
  else if(temp_f == lps22df_press_odr_hz200)
  {
    enum_id = 7;
  }
  json_object_dotset_number(JSON_Status, "lps22df_press.odr", enum_id);
  lps22df_press_get_fs(&temp_f);
  enum_id = 0;
  if(temp_f == lps22df_press_fs_hpa1260)
  {
    enum_id = 0;
  }
  json_object_dotset_number(JSON_Status, "lps22df_press.fs", enum_id);
  bool temp_b = 0;
  lps22df_press_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "lps22df_press.enable", temp_b);
  int32_t temp_i = 0;
  lps22df_press_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "lps22df_press.samples_per_ts", temp_i);
  lps22df_press_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "lps22df_press.dim", temp_i);
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

  return 0;
}

uint8_t Lps22df_Press_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = 0;
  if(json_object_dothas_value(tempJSONObject, "lps22df_press.odr"))
  {
    int odr = (int)json_object_dotget_number(tempJSONObject, "lps22df_press.odr");
    switch(odr)
    {
    case 0:
      ret = lps22df_press_set_odr(lps22df_press_odr_hz1);
      break;
    case 1:
      ret = lps22df_press_set_odr(lps22df_press_odr_hz4);
      break;
    case 2:
      ret = lps22df_press_set_odr(lps22df_press_odr_hz10);
      break;
    case 3:
      ret = lps22df_press_set_odr(lps22df_press_odr_hz25);
      break;
    case 4:
      ret = lps22df_press_set_odr(lps22df_press_odr_hz50);
      break;
    case 5:
      ret = lps22df_press_set_odr(lps22df_press_odr_hz75);
      break;
    case 6:
      ret = lps22df_press_set_odr(lps22df_press_odr_hz100);
      break;
    case 7:
      ret = lps22df_press_set_odr(lps22df_press_odr_hz200);
      break;
    }
  if(ret == 0){
    json_object_dotset_number(respJSONObject, "lps22df_press.odr.value", odr);
  } else {
    json_object_dotset_string(respJSONObject, "lps22df_press.odr.value", "PNPL_SET_ERROR");
  }
  }
  if (json_object_dothas_value(tempJSONObject, "lps22df_press.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "lps22df_press.enable");
    ret = lps22df_press_set_enable(enable);
  if(ret == 0){
    json_object_dotset_boolean(respJSONObject, "lps22df_press.enable.value", enable);
  } else {
    json_object_dotset_string(respJSONObject, "lps22df_press.enable.value", "PNPL_SET_ERROR");
  }
  }
  if (json_object_dothas_value(tempJSONObject, "lps22df_press.samples_per_ts"))
  {
    int32_t samples_per_ts = (int32_t) json_object_dotget_number(tempJSONObject, "lps22df_press.samples_per_ts");
    ret = lps22df_press_set_samples_per_ts(samples_per_ts);
  if(ret == 0){
    json_object_dotset_number(respJSONObject, "lps22df_press.samples_per_ts.value", samples_per_ts);
  } else {
    json_object_dotset_string(respJSONObject, "lps22df_press.samples_per_ts.value", "PNPL_SET_ERROR");
  }
  }
  if (json_object_dothas_value(tempJSONObject, "lps22df_press.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "lps22df_press.sensor_annotation");
    ret = lps22df_press_set_sensor_annotation(sensor_annotation);
  if(ret == 0){
    json_object_dotset_string(respJSONObject, "lps22df_press.sensor_annotation.value", sensor_annotation);
  } else {
    json_object_dotset_string(respJSONObject, "ism330is_acc.odr.value", "PNPL_SET_ERROR");
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
  return ret;
}

uint8_t Lps22df_Press_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size, uint8_t pretty)
{
  return 1;
}
