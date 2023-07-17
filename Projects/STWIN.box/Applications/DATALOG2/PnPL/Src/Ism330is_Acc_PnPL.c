/**
  ******************************************************************************
  * @file    Ism330is_Acc_PnPL.c
  * @author  SRA
  * @brief   Ism330is_Acc PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2Ispu:sensors:ism330is_acc;3
  *
  * Created by: DTDL2PnPL_cGen version 1.1.0
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

#include "Ism330is_Acc_PnPL.h"
#include "Ism330is_Acc_PnPL_vtbl.h"

static const IPnPLComponent_vtbl sIsm330is_Acc_PnPL_CompIF_vtbl =
{
  Ism330is_Acc_PnPL_vtblGetKey,
  Ism330is_Acc_PnPL_vtblGetNCommands,
  Ism330is_Acc_PnPL_vtblGetCommandKey,
  Ism330is_Acc_PnPL_vtblGetStatus,
  Ism330is_Acc_PnPL_vtblSetProperty,
  Ism330is_Acc_PnPL_vtblExecuteFunction
};

/**
  *  Ism330is_Acc_PnPL internal structure.
  */
struct _Ism330is_Acc_PnPL
{
  /**
    * Implements the IPnPLComponent interface.
    */
  IPnPLComponent_t component_if;

};

/* Objects instance ----------------------------------------------------------*/
static Ism330is_Acc_PnPL sIsm330is_Acc_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Ism330is_Acc_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sIsm330is_Acc_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sIsm330is_Acc_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Ism330is_Acc_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  ism330is_acc_comp_init();
  return 0;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Ism330is_Acc_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return ism330is_acc_get_key();
}

uint8_t Ism330is_Acc_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Ism330is_Acc_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Ism330is_Acc_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  float temp_f = 0;
  ism330is_acc_get_odr(&temp_f);
  uint8_t enum_id = 0;
  if(temp_f == ism330is_acc_odr_hz12_5)
  {
    enum_id = 0;
  }
  else if(temp_f == ism330is_acc_odr_hz26)
  {
    enum_id = 1;
  }
  else if(temp_f == ism330is_acc_odr_hz52)
  {
    enum_id = 2;
  }
  else if(temp_f == ism330is_acc_odr_hz104)
  {
    enum_id = 3;
  }
  else if(temp_f == ism330is_acc_odr_hz208)
  {
    enum_id = 4;
  }
  else if(temp_f == ism330is_acc_odr_hz416)
  {
    enum_id = 5;
  }
  else if(temp_f == ism330is_acc_odr_hz833)
  {
    enum_id = 6;
  }
  else if(temp_f == ism330is_acc_odr_hz1667)
  {
    enum_id = 7;
  }
  else if(temp_f == ism330is_acc_odr_hz3333)
  {
    enum_id = 8;
  }
  else if(temp_f == ism330is_acc_odr_hz6667)
  {
    enum_id = 9;
  }
  json_object_dotset_number(JSON_Status, "ism330is_acc.odr", enum_id);
  ism330is_acc_get_fs(&temp_f);
  enum_id = 0;
  if(temp_f == ism330is_acc_fs_g2)
  {
    enum_id = 0;
  }
  else if(temp_f == ism330is_acc_fs_g4)
  {
    enum_id = 1;
  }
  else if(temp_f == ism330is_acc_fs_g8)
  {
    enum_id = 2;
  }
  else if(temp_f == ism330is_acc_fs_g16)
  {
    enum_id = 3;
  }
  json_object_dotset_number(JSON_Status, "ism330is_acc.fs", enum_id);
  bool temp_b = 0;
  ism330is_acc_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "ism330is_acc.enable", temp_b);
  int32_t temp_i = 0;
  ism330is_acc_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_acc.samples_per_ts", temp_i);
  ism330is_acc_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_acc.dim", temp_i);
  ism330is_acc_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "ism330is_acc.ioffset", temp_f);
  ism330is_acc_get_measodr(&temp_f);
  json_object_dotset_number(JSON_Status, "ism330is_acc.measodr", temp_f);
  ism330is_acc_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_acc.usb_dps", temp_i);
  ism330is_acc_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_acc.sd_dps", temp_i);
  ism330is_acc_get_sensitivity(&temp_f);
  json_object_dotset_number(JSON_Status, "ism330is_acc.sensitivity", temp_f);
  char *temp_s = "";
  ism330is_acc_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "ism330is_acc.data_type", temp_s);
  ism330is_acc_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "ism330is_acc.sensor_annotation", temp_s);
  ism330is_acc_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_acc.sensor_category", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "ism330is_acc.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  ism330is_acc_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "ism330is_acc.stream_id", temp_int8);
  ism330is_acc_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "ism330is_acc.ep_id", temp_int8);

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

uint8_t Ism330is_Acc_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);

  uint8_t ret = 0;
  if(json_object_dothas_value(tempJSONObject, "ism330is_acc.odr"))
  {
    int odr = (int)json_object_dotget_number(tempJSONObject, "ism330is_acc.odr");
    switch(odr)
    {
    case 0:
      ism330is_acc_set_odr(ism330is_acc_odr_hz12_5);
      break;
    case 1:
      ism330is_acc_set_odr(ism330is_acc_odr_hz26);
      break;
    case 2:
      ism330is_acc_set_odr(ism330is_acc_odr_hz52);
      break;
    case 3:
      ism330is_acc_set_odr(ism330is_acc_odr_hz104);
      break;
    case 4:
      ism330is_acc_set_odr(ism330is_acc_odr_hz208);
      break;
    case 5:
      ism330is_acc_set_odr(ism330is_acc_odr_hz416);
      break;
    case 6:
      ism330is_acc_set_odr(ism330is_acc_odr_hz833);
      break;
    case 7:
      ism330is_acc_set_odr(ism330is_acc_odr_hz1667);
      break;
    case 8:
      ism330is_acc_set_odr(ism330is_acc_odr_hz3333);
      break;
    case 9:
      ism330is_acc_set_odr(ism330is_acc_odr_hz6667);
      break;
    }
  }
  if(json_object_dothas_value(tempJSONObject, "ism330is_acc.fs"))
  {
    int fs = (int)json_object_dotget_number(tempJSONObject, "ism330is_acc.fs");
    switch(fs)
    {
    case 0:
      ism330is_acc_set_fs(ism330is_acc_fs_g2);
      break;
    case 1:
      ism330is_acc_set_fs(ism330is_acc_fs_g4);
      break;
    case 2:
      ism330is_acc_set_fs(ism330is_acc_fs_g8);
      break;
    case 3:
      ism330is_acc_set_fs(ism330is_acc_fs_g16);
      break;
    }
  }
  if (json_object_dothas_value(tempJSONObject, "ism330is_acc.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "ism330is_acc.enable");
    ism330is_acc_set_enable(enable);
  }
  if (json_object_dothas_value(tempJSONObject, "ism330is_acc.samples_per_ts"))
  {
    int32_t samples_per_ts =(int32_t) json_object_dotget_number(tempJSONObject, "ism330is_acc.samples_per_ts");
    ism330is_acc_set_samples_per_ts(samples_per_ts);
  }
  if (json_object_dothas_value(tempJSONObject, "ism330is_acc.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "ism330is_acc.sensor_annotation");
    ism330is_acc_set_sensor_annotation(sensor_annotation);
  }
  json_value_free(tempJSON);
  return ret;
}

uint8_t Ism330is_Acc_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON)
{
  return 1;
}
