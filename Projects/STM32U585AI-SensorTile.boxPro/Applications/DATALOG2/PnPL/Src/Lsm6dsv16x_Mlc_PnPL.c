/**
  ******************************************************************************
  * @file    Lsm6dsv16x_Mlc_PnPL.c
  * @author  SRA
  * @brief   Lsm6dsv16x_Mlc PnPL Component Manager
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:lsm6dsv16x_mlc;3
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

#include "Lsm6dsv16x_Mlc_PnPL.h"
#include "Lsm6dsv16x_Mlc_PnPL_vtbl.h"

static const IPnPLComponent_vtbl sLsm6dsv16x_Mlc_PnPL_CompIF_vtbl =
{
  Lsm6dsv16x_Mlc_PnPL_vtblGetKey,
  Lsm6dsv16x_Mlc_PnPL_vtblGetNCommands,
  Lsm6dsv16x_Mlc_PnPL_vtblGetCommandKey,
  Lsm6dsv16x_Mlc_PnPL_vtblGetStatus,
  Lsm6dsv16x_Mlc_PnPL_vtblSetProperty,
  Lsm6dsv16x_Mlc_PnPL_vtblExecuteFunction
};

/**
  *  Lsm6dsv16x_Mlc_PnPL internal structure.
  */
struct _Lsm6dsv16x_Mlc_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
  /* Contains Lsm6dsv16x_Mlc functions pointers. */
  ILsm6dsv16x_Mlc_t *cmdIF;
};

/* Objects instance ----------------------------------------------------------*/
static Lsm6dsv16x_Mlc_PnPL sLsm6dsv16x_Mlc_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Lsm6dsv16x_Mlc_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sLsm6dsv16x_Mlc_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sLsm6dsv16x_Mlc_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Lsm6dsv16x_Mlc_PnPLInit(IPnPLComponent_t *_this,  ILsm6dsv16x_Mlc_t *inf)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  Lsm6dsv16x_Mlc_PnPL *p_if_owner = (Lsm6dsv16x_Mlc_PnPL *) _this;
  p_if_owner->cmdIF = inf;
  lsm6dsv16x_mlc_comp_init();
  return 0;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Lsm6dsv16x_Mlc_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return lsm6dsv16x_mlc_get_key();
}

uint8_t Lsm6dsv16x_Mlc_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 1;
}

char *Lsm6dsv16x_Mlc_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  switch (id)
  {
    case 0:
      return "lsm6dsv16x_mlc*load_file";
      break;
  }
  return 0;
}

uint8_t Lsm6dsv16x_Mlc_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                          uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  bool temp_b = 0;
  lsm6dsv16x_mlc_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "lsm6dsv16x_mlc.enable", temp_b);
  int32_t temp_i = 0;
  lsm6dsv16x_mlc_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.samples_per_ts", temp_i);
  lsm6dsv16x_mlc_get_ucf_status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "lsm6dsv16x_mlc.ucf_status", temp_b);
  lsm6dsv16x_mlc_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.dim", temp_i);
  float temp_f = 0;
  lsm6dsv16x_mlc_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.ioffset", temp_f);
  char *temp_s = "";
  lsm6dsv16x_mlc_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "lsm6dsv16x_mlc.data_type", temp_s);
  lsm6dsv16x_mlc_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.usb_dps", temp_i);
  lsm6dsv16x_mlc_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.sd_dps", temp_i);
  lsm6dsv16x_mlc_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "lsm6dsv16x_mlc.sensor_annotation", temp_s);
  lsm6dsv16x_mlc_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.sensor_category", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  lsm6dsv16x_mlc_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.stream_id", temp_int8);
  lsm6dsv16x_mlc_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.ep_id", temp_int8);

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

uint8_t Lsm6dsv16x_Mlc_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                            uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = 0;
  if (json_object_dothas_value(tempJSONObject, "lsm6dsv16x_mlc.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "lsm6dsv16x_mlc.enable");
    ret = lsm6dsv16x_mlc_set_enable(enable);
    if (ret == 0)
    {
      json_object_dotset_boolean(respJSONObject, "lsm6dsv16x_mlc.enable.value", enable);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "lsm6dsv16x_mlc.enable.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lsm6dsv16x_mlc.samples_per_ts"))
  {
    int32_t samples_per_ts = (int32_t) json_object_dotget_number(tempJSONObject, "lsm6dsv16x_mlc.samples_per_ts");
    ret = lsm6dsv16x_mlc_set_samples_per_ts(samples_per_ts);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "lsm6dsv16x_mlc.samples_per_ts.value", samples_per_ts);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "lsm6dsv16x_mlc.samples_per_ts.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lsm6dsv16x_mlc.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "lsm6dsv16x_mlc.sensor_annotation");
    ret = lsm6dsv16x_mlc_set_sensor_annotation(sensor_annotation);
    if (ret == 0)
    {
      json_object_dotset_string(respJSONObject, "lsm6dsv16x_mlc.sensor_annotation.value", sensor_annotation);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "lsm6dsv16x_mlc.sensor_annotation.value", "PNPL_SET_ERROR");
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


uint8_t Lsm6dsv16x_Mlc_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                uint32_t *size, uint8_t pretty)
{
  Lsm6dsv16x_Mlc_PnPL *p_if_owner = (Lsm6dsv16x_Mlc_PnPL *) _this;
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = 0;
  if (json_object_dothas_value(tempJSONObject, "lsm6dsv16x_mlc*load_file.ucf_data"))
  {
    int32_t size;
    const char *data;
    if (json_object_dothas_value(tempJSONObject, "lsm6dsv16x_mlc*load_file.ucf_data.size"))
    {
      size = (int32_t) json_object_dotget_number(tempJSONObject, "lsm6dsv16x_mlc*load_file.ucf_data.size");
      if (json_object_dothas_value(tempJSONObject, "lsm6dsv16x_mlc*load_file.ucf_data.data"))
      {
        data = json_object_dotget_string(tempJSONObject, "lsm6dsv16x_mlc*load_file.ucf_data.data");
        ret = lsm6dsv16x_mlc_load_file(p_if_owner->cmdIF, size, (char *) data);
        if (ret == 0)
        {
          json_object_dotset_string(respJSONObject, "lsm6dsv16x_mlc*load_file.response", "PNPL_CMD_OK");
        }
        else
        {
          json_object_dotset_string(respJSONObject, "lsm6dsv16x_mlc.load_file.response", "PNPL_CMD_ERROR");
        }
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

