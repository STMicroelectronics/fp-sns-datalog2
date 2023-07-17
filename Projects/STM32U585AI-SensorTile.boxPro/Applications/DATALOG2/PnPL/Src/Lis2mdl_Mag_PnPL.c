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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:lis2mdl_mag;2
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

#include "Lis2mdl_Mag_PnPL.h"
#include "Lis2mdl_Mag_PnPL_vtbl.h"

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
  /**
    * Implements the IPnPLComponent interface.
    */
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
  return 0;
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

  float temp_f = 0;
  lis2mdl_mag_get_odr(&temp_f);
  uint8_t enum_id = 0;
  if(temp_f == lis2mdl_mag_odr_hz10)
  {
    enum_id = 0;
  }
  else if(temp_f == lis2mdl_mag_odr_hz20)
  {
    enum_id = 1;
  }
  else if(temp_f == lis2mdl_mag_odr_hz50)
  {
    enum_id = 2;
  }
  else if(temp_f == lis2mdl_mag_odr_hz100)
  {
    enum_id = 3;
  }
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.odr", enum_id);
  lis2mdl_mag_get_fs(&temp_f);
  enum_id = 0;
  if(temp_f == lis2mdl_mag_fs_g50)
  {
    enum_id = 0;
  }
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.fs", enum_id);
  bool temp_b = 0;
  lis2mdl_mag_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "lis2mdl_mag.enable", temp_b);
  int32_t temp_i = 0;
  lis2mdl_mag_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.samples_per_ts", temp_i);
  lis2mdl_mag_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.dim", temp_i);
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
  lis2mdl_mag_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "lis2mdl_mag.sensor_category", temp_i);
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

  return 0;
}

uint8_t Lis2mdl_Mag_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);

  uint8_t ret = 0;
  if(json_object_dothas_value(tempJSONObject, "lis2mdl_mag.odr"))
  {
    int odr = (int)json_object_dotget_number(tempJSONObject, "lis2mdl_mag.odr");
    switch(odr)
    {
    case 0:
      lis2mdl_mag_set_odr(lis2mdl_mag_odr_hz10);
      break;
    case 1:
      lis2mdl_mag_set_odr(lis2mdl_mag_odr_hz20);
      break;
    case 2:
      lis2mdl_mag_set_odr(lis2mdl_mag_odr_hz50);
      break;
    case 3:
      lis2mdl_mag_set_odr(lis2mdl_mag_odr_hz100);
      break;
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "lis2mdl_mag.enable");
    lis2mdl_mag_set_enable(enable);
  }
  if (json_object_dothas_value(tempJSONObject, "lis2mdl_mag.samples_per_ts"))
  {
    int32_t samples_per_ts =(int32_t) json_object_dotget_number(tempJSONObject, "lis2mdl_mag.samples_per_ts");
    lis2mdl_mag_set_samples_per_ts(samples_per_ts);
  }
  json_value_free(tempJSON);
  return ret;
}

uint8_t Lis2mdl_Mag_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON)
{
  return 1;
}
