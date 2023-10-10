/**
  ******************************************************************************
  * @file    Iis2iclx_Acc_PnPL.c
  * @author  SRA
  * @brief   Iis2iclx_Acc PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:sensors:iis2iclx_acc;3
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

#include "Iis2iclx_Acc_PnPL.h"
#include "Iis2iclx_Acc_PnPL_vtbl.h"

static const IPnPLComponent_vtbl sIis2iclx_Acc_PnPL_CompIF_vtbl =
{
  Iis2iclx_Acc_PnPL_vtblGetKey,
  Iis2iclx_Acc_PnPL_vtblGetNCommands,
  Iis2iclx_Acc_PnPL_vtblGetCommandKey,
  Iis2iclx_Acc_PnPL_vtblGetStatus,
  Iis2iclx_Acc_PnPL_vtblSetProperty,
  Iis2iclx_Acc_PnPL_vtblExecuteFunction
};

/**
  *  Iis2iclx_Acc_PnPL internal structure.
  */
struct _Iis2iclx_Acc_PnPL
{
  /**
    * Implements the IPnPLComponent interface.
    */
  IPnPLComponent_t component_if;

};

/* Objects instance ----------------------------------------------------------*/
static Iis2iclx_Acc_PnPL sIis2iclx_Acc_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Iis2iclx_Acc_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sIis2iclx_Acc_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sIis2iclx_Acc_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Iis2iclx_Acc_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  iis2iclx_acc_comp_init();
  return 0;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Iis2iclx_Acc_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return iis2iclx_acc_get_key();
}

uint8_t Iis2iclx_Acc_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Iis2iclx_Acc_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Iis2iclx_Acc_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  float temp_f = 0;
  iis2iclx_acc_get_odr(&temp_f);
  uint8_t enum_id = 0;
  if (temp_f == iis2iclx_acc_odr_hz12_5)
  {
    enum_id = 0;
  }
  else if (temp_f == iis2iclx_acc_odr_hz26)
  {
    enum_id = 1;
  }
  else if (temp_f == iis2iclx_acc_odr_hz52)
  {
    enum_id = 2;
  }
  else if (temp_f == iis2iclx_acc_odr_hz104)
  {
    enum_id = 3;
  }
  else if (temp_f == iis2iclx_acc_odr_hz208)
  {
    enum_id = 4;
  }
  else if (temp_f == iis2iclx_acc_odr_hz416)
  {
    enum_id = 5;
  }
  else if (temp_f == iis2iclx_acc_odr_hz833)
  {
    enum_id = 6;
  }
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.odr", enum_id);
  iis2iclx_acc_get_fs(&temp_f);
  enum_id = 0;
  if (temp_f == iis2iclx_acc_fs_g0_5)
  {
    enum_id = 0;
  }
  else if (temp_f == iis2iclx_acc_fs_g1)
  {
    enum_id = 1;
  }
  else if (temp_f == iis2iclx_acc_fs_g2)
  {
    enum_id = 2;
  }
  else if (temp_f == iis2iclx_acc_fs_g3)
  {
    enum_id = 3;
  }
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.fs", enum_id);
  bool temp_b = 0;
  iis2iclx_acc_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "iis2iclx_acc.enable", temp_b);
  int32_t temp_i = 0;
  iis2iclx_acc_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.samples_per_ts", temp_i);
  iis2iclx_acc_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.dim", temp_i);
  iis2iclx_acc_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.ioffset", temp_f);
  iis2iclx_acc_get_measodr(&temp_f);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.measodr", temp_f);
  iis2iclx_acc_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.usb_dps", temp_i);
  iis2iclx_acc_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.sd_dps", temp_i);
  iis2iclx_acc_get_sensitivity(&temp_f);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.sensitivity", temp_f);
  char *temp_s = "";
  iis2iclx_acc_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "iis2iclx_acc.data_type", temp_s);
  iis2iclx_acc_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "iis2iclx_acc.sensor_annotation", temp_s);
  iis2iclx_acc_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.sensor_category", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  iis2iclx_acc_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.stream_id", temp_int8);
  iis2iclx_acc_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.ep_id", temp_int8);

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

uint8_t Iis2iclx_Acc_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = 0;
  if (json_object_dothas_value(tempJSONObject, "iis2iclx_acc.odr"))
  {
    int odr = (int)json_object_dotget_number(tempJSONObject, "iis2iclx_acc.odr");
    switch (odr)
    {
      case 0:
        ret = iis2iclx_acc_set_fs(iis2iclx_acc_odr_hz12_5);
        break;
      case 1:
        ret = iis2iclx_acc_set_odr(iis2iclx_acc_odr_hz26);
        break;
      case 2:
        ret = iis2iclx_acc_set_odr(iis2iclx_acc_odr_hz52);
        break;
      case 3:
        ret = iis2iclx_acc_set_odr(iis2iclx_acc_odr_hz104);
        break;
      case 4:
        ret = iis2iclx_acc_set_odr(iis2iclx_acc_odr_hz208);
        break;
      case 5:
        ret = iis2iclx_acc_set_odr(iis2iclx_acc_odr_hz416);
        break;
      case 6:
        ret = iis2iclx_acc_set_odr(iis2iclx_acc_odr_hz833);
        break;
    }
    if(ret == 0){
      json_object_dotset_number(respJSONObject, "iis2iclx_acc.odr.value", odr);
    } else {
      json_object_dotset_string(respJSONObject, "iis2iclx_acc.odr.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2iclx_acc.fs"))
  {
    int fs = (int)json_object_dotget_number(tempJSONObject, "iis2iclx_acc.fs");
    switch (fs)
    {
      case 0:
        ret = iis2iclx_acc_set_fs(iis2iclx_acc_fs_g0_5);
        break;
      case 1:
        ret = iis2iclx_acc_set_fs(iis2iclx_acc_fs_g1);
        break;
      case 2:
        ret = iis2iclx_acc_set_fs(iis2iclx_acc_fs_g2);
        break;
      case 3:
        ret = iis2iclx_acc_set_fs(iis2iclx_acc_fs_g3);
        break;
    }
    if(ret == 0){
      json_object_dotset_number(respJSONObject, "iis2iclx_acc.fs.value", fs);
    } else {
      json_object_dotset_string(respJSONObject, "iis2iclx_acc.fs.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2iclx_acc.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "iis2iclx_acc.enable");
    ret = iis2iclx_acc_set_enable(enable);
    if(ret == 0){
      json_object_dotset_boolean(respJSONObject, "iis2iclx_acc.enable.value", enable);
    } else {
      json_object_dotset_string(respJSONObject, "iis2iclx_acc.enable.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2iclx_acc.samples_per_ts"))
  {
    int32_t samples_per_ts = (int32_t) json_object_dotget_number(tempJSONObject, "iis2iclx_acc.samples_per_ts");
    ret = iis2iclx_acc_set_samples_per_ts(samples_per_ts);
    if(ret == 0){
      json_object_dotset_number(respJSONObject, "iis2iclx_acc.samples_per_ts.value", samples_per_ts);
    } else {
      json_object_dotset_string(respJSONObject, "iis2iclx_acc.samples_per_ts.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2iclx_acc.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "iis2iclx_acc.sensor_annotation");
    ret = iis2iclx_acc_set_sensor_annotation(sensor_annotation);
    json_object_dotset_string(respJSONObject, "iis2iclx_acc.sensor_annotation.value", ret == 0 ? sensor_annotation : "PNPL_SET_ERROR");
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

uint8_t Iis2iclx_Acc_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size, uint8_t pretty)
{
  return 1;
}
