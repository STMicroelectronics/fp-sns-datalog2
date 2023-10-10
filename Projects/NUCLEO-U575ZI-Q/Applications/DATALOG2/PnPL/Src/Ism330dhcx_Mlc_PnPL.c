/**
  ******************************************************************************
  * @file    Ism330dhcx_Mlc_PnPL.c
  * @author  SRA
  * @brief   Ism330dhcx_Mlc PnPL Component Manager
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
  * dtmi:vespucci:nucleo_u575zi_q:x_nucleo_iks02a1:fpSnsDatalog2_datalog2:sensors:ism330dhcx_mlc;1
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

#include "Ism330dhcx_Mlc_PnPL.h"
#include "Ism330dhcx_Mlc_PnPL_vtbl.h"

static const IPnPLComponent_vtbl sIsm330dhcx_Mlc_PnPL_CompIF_vtbl =
{
  Ism330dhcx_Mlc_PnPL_vtblGetKey,
  Ism330dhcx_Mlc_PnPL_vtblGetNCommands,
  Ism330dhcx_Mlc_PnPL_vtblGetCommandKey,
  Ism330dhcx_Mlc_PnPL_vtblGetStatus,
  Ism330dhcx_Mlc_PnPL_vtblSetProperty,
  Ism330dhcx_Mlc_PnPL_vtblExecuteFunction
};

/**
  *  Ism330dhcx_Mlc_PnPL internal structure.
  */
struct _Ism330dhcx_Mlc_PnPL
{
  /**
    * Implements the IPnPLComponent interface.
    */
  IPnPLComponent_t component_if;
  /**
    * Contains Ism330dhcx_Mlc functions pointers.
    */
  IIsm330dhcx_Mlc_t *cmdIF;
};

/* Objects instance ----------------------------------------------------------*/
static Ism330dhcx_Mlc_PnPL sIsm330dhcx_Mlc_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Ism330dhcx_Mlc_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sIsm330dhcx_Mlc_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sIsm330dhcx_Mlc_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Ism330dhcx_Mlc_PnPLInit(IPnPLComponent_t *_this,  IIsm330dhcx_Mlc_t *inf)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  Ism330dhcx_Mlc_PnPL *p_if_owner = (Ism330dhcx_Mlc_PnPL *) _this;
  p_if_owner->cmdIF = inf;
  ism330dhcx_mlc_comp_init();
  return 0;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Ism330dhcx_Mlc_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return ism330dhcx_mlc_get_key();
}

uint8_t Ism330dhcx_Mlc_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 1;
}

char *Ism330dhcx_Mlc_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  switch (id)
  {
    case 0:
      return "ism330dhcx_mlc*load_file";
      break;
  }
  return 0;
}

uint8_t Ism330dhcx_Mlc_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                          uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  bool temp_b = 0;
  ism330dhcx_mlc_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "ism330dhcx_mlc.enable", temp_b);
  int32_t temp_i = 0;
  ism330dhcx_mlc_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330dhcx_mlc.samples_per_ts", temp_i);
  ism330dhcx_mlc_get_ucf_status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "ism330dhcx_mlc.ucf_status", temp_b);
  ism330dhcx_mlc_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330dhcx_mlc.dim", temp_i);
  float temp_f = 0;
  ism330dhcx_mlc_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "ism330dhcx_mlc.ioffset", temp_f);
  char *temp_s = "";
  ism330dhcx_mlc_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "ism330dhcx_mlc.data_type", temp_s);
  ism330dhcx_mlc_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330dhcx_mlc.usb_dps", temp_i);
  ism330dhcx_mlc_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330dhcx_mlc.sd_dps", temp_i);
  ism330dhcx_mlc_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "ism330dhcx_mlc.sensor_annotation", temp_s);
  ism330dhcx_mlc_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330dhcx_mlc.sensor_category", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "ism330dhcx_mlc.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  ism330dhcx_mlc_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "ism330dhcx_mlc.stream_id", temp_int8);
  ism330dhcx_mlc_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "ism330dhcx_mlc.ep_id", temp_int8);

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

uint8_t Ism330dhcx_Mlc_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);

  uint8_t ret = 0;
  if (json_object_dothas_value(tempJSONObject, "ism330dhcx_mlc.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "ism330dhcx_mlc.enable");
    ism330dhcx_mlc_set_enable(enable);
  }
  if (json_object_dothas_value(tempJSONObject, "ism330dhcx_mlc.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "ism330dhcx_mlc.sensor_annotation");
    ism330dhcx_mlc_set_sensor_annotation(sensor_annotation);
  }
  json_value_free(tempJSON);
  return ret;
}

uint8_t Ism330dhcx_Mlc_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON)
{
  Ism330dhcx_Mlc_PnPL *p_if_owner = (Ism330dhcx_Mlc_PnPL *) _this;
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  if (json_object_dothas_value(tempJSONObject, "ism330dhcx_mlc*load_file.ucf_data"))
  {
    const char *data;
    int32_t size;
    if (json_object_dothas_value(tempJSONObject, "ism330dhcx_mlc*load_file.ucf_data.data"))
    {
      data = json_object_dotget_string(tempJSONObject, "ism330dhcx_mlc*load_file.ucf_data.data");
      if (json_object_dothas_value(tempJSONObject, "ism330dhcx_mlc*load_file.ucf_data.size"))
      {
        size = (int32_t) json_object_dotget_number(tempJSONObject, "ism330dhcx_mlc*load_file.ucf_data.size");
        ism330dhcx_mlc_load_file(p_if_owner->cmdIF, (char *) data, size);
      }
    }
  }
  json_value_free(tempJSON);
  return 0;
}
