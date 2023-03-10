/**
  ******************************************************************************
  * @file    Ism330is_Ispu_PnPL.c
  * @author  SRA
  * @brief   Ism330is_Ispu PnPL Component Manager
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
  * dtmi:appconfig:steval_stwinbx1:fp_sns_datalog2_ispu:sensors:ism330is_ispu;1
  *
  * Created by: DTDL2PnPL_cGen version 1.0.0
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

#include "Ism330is_Ispu_PnPL.h"
#include "Ism330is_Ispu_PnPL_vtbl.h"
#include "IIsm330is_Ispu.h"
#include "IIsm330is_Ispu_vtbl.h"

static const IPnPLComponent_vtbl sIsm330is_Ispu_PnPL_CompIF_vtbl =
{
  Ism330is_Ispu_PnPL_vtblGetKey,
  Ism330is_Ispu_PnPL_vtblGetNCommands,
  Ism330is_Ispu_PnPL_vtblGetCommandKey,
  Ism330is_Ispu_PnPL_vtblGetStatus,
  Ism330is_Ispu_PnPL_vtblSetProperty,
  Ism330is_Ispu_PnPL_vtblExecuteFunction
};

/**
  *  Ism330is_Ispu_PnPL internal structure.
  */
struct _Ism330is_Ispu_PnPL
{
  /**
    * Implements the IPnPLComponent interface.
    */
  IPnPLComponent_t component_if;
  /**
    * Contains Ism330is_Ispu functions pointers.
    */
  IIsm330is_Ispu_t *cmdIF;
};

/* Objects instance ----------------------------------------------------------*/
static Ism330is_Ispu_PnPL sIsm330is_Ispu_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Ism330is_Ispu_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sIsm330is_Ispu_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sIsm330is_Ispu_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Ism330is_Ispu_PnPLInit(IPnPLComponent_t *_this,  IIsm330is_Ispu_t *inf)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  Ism330is_Ispu_PnPL *p_if_owner = (Ism330is_Ispu_PnPL *) _this;
  p_if_owner->cmdIF = inf;
  ism330is_ispu_comp_init();
  return 0;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Ism330is_Ispu_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return ism330is_ispu_get_key();
}

uint8_t Ism330is_Ispu_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 1;
}

char *Ism330is_Ispu_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  switch (id)
  {
  case 0:
    return "ism330is_ispu*load_file";
    break;
  }
  return 0;
}

uint8_t Ism330is_Ispu_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  bool temp_b = 0;
  ism330is_ispu_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "ism330is_ispu.enable", temp_b);
  int32_t temp_i = 0;
  ism330is_ispu_get_samples_per_ts__val(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_ispu.samples_per_ts.val", temp_i);
  ism330is_ispu_get_samples_per_ts__min(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_ispu.samples_per_ts.min", temp_i);
  ism330is_ispu_get_samples_per_ts__max(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_ispu.samples_per_ts.max", temp_i);
  ism330is_ispu_get_ucf_status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "ism330is_ispu.ucf_status", temp_b);
  float temp_f = 0;
  ism330is_ispu_get_usb_dps(&temp_f);
  json_object_dotset_number(JSON_Status, "ism330is_ispu.usb_dps", temp_f);
  ism330is_ispu_get_sd_dps(&temp_f);
  json_object_dotset_number(JSON_Status, "ism330is_ispu.sd_dps", temp_f);
  char *temp_s = "";
  ism330is_ispu_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "ism330is_ispu.data_type", temp_s);
  ism330is_ispu_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "ism330is_ispu.dim", temp_i);
  ism330is_ispu_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "ism330is_ispu.ioffset", temp_f);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors and Algorithms */
  json_object_dotset_number(JSON_Status, "ism330is_ispu.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  ism330is_ispu_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "ism330is_ispu.stream_id", temp_int8);
  ism330is_ispu_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "ism330is_ispu.ep_id", temp_int8);

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

uint8_t Ism330is_Ispu_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);

  uint8_t ret = 0;
  if (json_object_dothas_value(tempJSONObject, "ism330is_ispu.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "ism330is_ispu.enable");
    ism330is_ispu_set_enable(enable);
  }
  if (json_object_dothas_value(tempJSONObject, "ism330is_ispu.samples_per_ts"))
  {
    if (json_object_dothas_value(tempJSONObject, "ism330is_ispu.samples_per_ts.val"))
    {
      int32_t samples_per_ts =(int32_t) json_object_dotget_number(tempJSONObject, "ism330is_ispu.samples_per_ts.val");
      ism330is_ispu_set_samples_per_ts__val(samples_per_ts);
    }
  }
  json_value_free(tempJSON);
  return ret;
}

uint8_t Ism330is_Ispu_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON)
{
  Ism330is_Ispu_PnPL *p_if_owner = (Ism330is_Ispu_PnPL *) _this;
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  if (json_object_dothas_value(tempJSONObject, "ism330is_ispu*load_file.files"))
  {
    const char *ucf_data;
    int32_t ucf_size;
    const char *output_data;
    int32_t output_size;
    if (json_object_dothas_value(tempJSONObject, "ism330is_ispu*load_file.files.ucf_data"))
    {
      ucf_data = json_object_dotget_string(tempJSONObject, "ism330is_ispu*load_file.files.ucf_data");
      if (json_object_dothas_value(tempJSONObject, "ism330is_ispu*load_file.files.ucf_size"))
      {
        ucf_size =(int32_t) json_object_dotget_number(tempJSONObject, "ism330is_ispu*load_file.files.ucf_size");
        if (json_object_dothas_value(tempJSONObject, "ism330is_ispu*load_file.files.output_data"))
        {
          output_data = json_object_dotget_string(tempJSONObject, "ism330is_ispu*load_file.files.output_data");
          if (json_object_dothas_value(tempJSONObject, "ism330is_ispu*load_file.files.output_size"))
          {
            output_size =(int32_t) json_object_dotget_number(tempJSONObject, "ism330is_ispu*load_file.files.output_size");
            ism330is_ispu_load_file(p_if_owner->cmdIF, (char*) ucf_data, ucf_size, (char*) output_data, output_size);
          }
        }
      }
    }
  }
  json_value_free(tempJSON);
  return 0;
}
