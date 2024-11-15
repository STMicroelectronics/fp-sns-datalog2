/**
  ******************************************************************************
  * @file    Imp23absu_Mic_PnPL.c
  * @author  SRA
  * @brief   Imp23absu_Mic PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_ultrasoundFft:sensors:imp23absu_mic;2
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

#include "Imp23absu_Mic_PnPL.h"
#include "Imp23absu_Mic_PnPL_vtbl.h"

static const IPnPLComponent_vtbl sImp23absu_Mic_PnPL_CompIF_vtbl =
{
  Imp23absu_Mic_PnPL_vtblGetKey,
  Imp23absu_Mic_PnPL_vtblGetNCommands,
  Imp23absu_Mic_PnPL_vtblGetCommandKey,
  Imp23absu_Mic_PnPL_vtblGetStatus,
  Imp23absu_Mic_PnPL_vtblSetProperty,
  Imp23absu_Mic_PnPL_vtblExecuteFunction
};

/**
  *  Imp23absu_Mic_PnPL internal structure.
  */
struct _Imp23absu_Mic_PnPL
{
  /**
    * Implements the IPnPLComponent interface.
    */
  IPnPLComponent_t component_if;

};

/* Objects instance ----------------------------------------------------------*/
static Imp23absu_Mic_PnPL sImp23absu_Mic_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Imp23absu_Mic_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sImp23absu_Mic_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sImp23absu_Mic_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Imp23absu_Mic_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  imp23absu_mic_comp_init();
  return 0;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Imp23absu_Mic_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return imp23absu_mic_get_key();
}

uint8_t Imp23absu_Mic_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Imp23absu_Mic_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Imp23absu_Mic_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  float temp_f = 0;
  imp23absu_mic_get_odr(&temp_f);
  uint8_t enum_id = 0;
  if (temp_f == imp23absu_mic_odr_hz192000)
  {
    enum_id = 0;
  }
  json_object_dotset_number(JSON_Status, "imp23absu_mic.odr", enum_id);
  imp23absu_mic_get_aop(&temp_f);
  enum_id = 0;
  if (temp_f == imp23absu_mic_aop_dbspl130)
  {
    enum_id = 0;
  }
  json_object_dotset_number(JSON_Status, "imp23absu_mic.aop", enum_id);
  bool temp_b = 0;
  imp23absu_mic_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "imp23absu_mic.enable", temp_b);
  int32_t temp_i = 0;
  imp23absu_mic_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "imp23absu_mic.samples_per_ts", temp_i);
  imp23absu_mic_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "imp23absu_mic.dim", temp_i);
  imp23absu_mic_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "imp23absu_mic.ioffset", temp_f);
  imp23absu_mic_get_measodr(&temp_f);
  json_object_dotset_number(JSON_Status, "imp23absu_mic.measodr", temp_f);
  imp23absu_mic_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "imp23absu_mic.usb_dps", temp_i);
  imp23absu_mic_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "imp23absu_mic.sd_dps", temp_i);
  imp23absu_mic_get_sensitivity(&temp_f);
  json_object_dotset_number(JSON_Status, "imp23absu_mic.sensitivity", temp_f);
  char *temp_s = "";
  imp23absu_mic_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "imp23absu_mic.data_type", temp_s);
  imp23absu_mic_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "imp23absu_mic.sensor_annotation", temp_s);
  imp23absu_mic_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "imp23absu_mic.sensor_category", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "imp23absu_mic.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  imp23absu_mic_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "imp23absu_mic.stream_id", temp_int8);
  imp23absu_mic_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "imp23absu_mic.ep_id", temp_int8);

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

uint8_t Imp23absu_Mic_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);

  uint8_t ret = 0;
  if (json_object_dothas_value(tempJSONObject, "imp23absu_mic.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "imp23absu_mic.enable");
    imp23absu_mic_set_enable(enable);
  }
  if (json_object_dothas_value(tempJSONObject, "imp23absu_mic.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "imp23absu_mic.sensor_annotation");
    imp23absu_mic_set_sensor_annotation(sensor_annotation);
  }
  json_value_free(tempJSON);
  return ret;
}

uint8_t Imp23absu_Mic_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON)
{
  return 1;
}
