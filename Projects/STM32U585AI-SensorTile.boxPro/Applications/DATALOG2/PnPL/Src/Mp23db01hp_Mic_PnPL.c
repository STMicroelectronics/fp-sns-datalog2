/**
  ******************************************************************************
  * @file    Mp23db01hp_Mic_PnPL.c
  * @author  SRA
  * @brief   Mp23db01hp_Mic PnPL Component Manager
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:mp23db01hp_mic;2
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

#include "Mp23db01hp_Mic_PnPL.h"
#include "Mp23db01hp_Mic_PnPL_vtbl.h"

static const IPnPLComponent_vtbl sMp23db01hp_Mic_PnPL_CompIF_vtbl =
{
  Mp23db01hp_Mic_PnPL_vtblGetKey,
  Mp23db01hp_Mic_PnPL_vtblGetNCommands,
  Mp23db01hp_Mic_PnPL_vtblGetCommandKey,
  Mp23db01hp_Mic_PnPL_vtblGetStatus,
  Mp23db01hp_Mic_PnPL_vtblSetProperty,
  Mp23db01hp_Mic_PnPL_vtblExecuteFunction
};

/**
  *  Mp23db01hp_Mic_PnPL internal structure.
  */
struct _Mp23db01hp_Mic_PnPL
{
  /**
    * Implements the IPnPLComponent interface.
    */
  IPnPLComponent_t component_if;

};

/* Objects instance ----------------------------------------------------------*/
static Mp23db01hp_Mic_PnPL sMp23db01hp_Mic_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Mp23db01hp_Mic_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sMp23db01hp_Mic_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sMp23db01hp_Mic_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Mp23db01hp_Mic_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  mp23db01hp_mic_comp_init();
  return 0;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Mp23db01hp_Mic_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return mp23db01hp_mic_get_key();
}

uint8_t Mp23db01hp_Mic_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Mp23db01hp_Mic_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Mp23db01hp_Mic_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  float temp_f = 0;
  mp23db01hp_mic_get_odr(&temp_f);
  uint8_t enum_id = 0;
  if(temp_f == mp23db01hp_mic_odr_hz16000)
  {
    enum_id = 0;
  }
  else if(temp_f == mp23db01hp_mic_odr_hz32000)
  {
    enum_id = 1;
  }
  else if(temp_f == mp23db01hp_mic_odr_hz48000)
  {
    enum_id = 2;
  }
  json_object_dotset_number(JSON_Status, "mp23db01hp_mic.odr", enum_id);
  mp23db01hp_mic_get_aop(&temp_f);
  enum_id = 0;
  if(temp_f == mp23db01hp_mic_aop_dbspl130)
  {
    enum_id = 0;
  }
  json_object_dotset_number(JSON_Status, "mp23db01hp_mic.aop", enum_id);
  bool temp_b = 0;
  mp23db01hp_mic_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "mp23db01hp_mic.enable", temp_b);
  int32_t temp_i = 0;
  mp23db01hp_mic_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "mp23db01hp_mic.samples_per_ts", temp_i);
  mp23db01hp_mic_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "mp23db01hp_mic.dim", temp_i);
  mp23db01hp_mic_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "mp23db01hp_mic.ioffset", temp_f);
  mp23db01hp_mic_get_measodr(&temp_f);
  json_object_dotset_number(JSON_Status, "mp23db01hp_mic.measodr", temp_f);
  mp23db01hp_mic_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "mp23db01hp_mic.usb_dps", temp_i);
  mp23db01hp_mic_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "mp23db01hp_mic.sd_dps", temp_i);
  mp23db01hp_mic_get_sensitivity(&temp_f);
  json_object_dotset_number(JSON_Status, "mp23db01hp_mic.sensitivity", temp_f);
  char *temp_s = "";
  mp23db01hp_mic_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "mp23db01hp_mic.data_type", temp_s);
  mp23db01hp_mic_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "mp23db01hp_mic.sensor_category", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "mp23db01hp_mic.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  mp23db01hp_mic_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "mp23db01hp_mic.stream_id", temp_int8);
  mp23db01hp_mic_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "mp23db01hp_mic.ep_id", temp_int8);

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

uint8_t Mp23db01hp_Mic_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);

  uint8_t ret = 0;
  if(json_object_dothas_value(tempJSONObject, "mp23db01hp_mic.odr"))
  {
    int odr = (int)json_object_dotget_number(tempJSONObject, "mp23db01hp_mic.odr");
    switch(odr)
    {
    case 0:
      mp23db01hp_mic_set_odr(mp23db01hp_mic_odr_hz16000);
      break;
    case 1:
      mp23db01hp_mic_set_odr(mp23db01hp_mic_odr_hz32000);
      break;
    case 2:
      mp23db01hp_mic_set_odr(mp23db01hp_mic_odr_hz48000);
      break;
    }
  }
  if (json_object_dothas_value(tempJSONObject, "mp23db01hp_mic.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "mp23db01hp_mic.enable");
    mp23db01hp_mic_set_enable(enable);
  }
  if (json_object_dothas_value(tempJSONObject, "mp23db01hp_mic.samples_per_ts"))
  {
    int32_t samples_per_ts =(int32_t) json_object_dotget_number(tempJSONObject, "mp23db01hp_mic.samples_per_ts");
    mp23db01hp_mic_set_samples_per_ts(samples_per_ts);
  }
  json_value_free(tempJSON);
  return ret;
}

uint8_t Mp23db01hp_Mic_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON)
{
  return 1;
}
