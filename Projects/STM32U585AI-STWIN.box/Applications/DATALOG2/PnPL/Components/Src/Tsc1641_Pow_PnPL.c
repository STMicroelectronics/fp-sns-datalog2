/**
  ******************************************************************************
  * @file    Tsc1641_Pow_PnPL.c
  * @author  SRA
  * @brief   Tsc1641_Pow PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:sensors:tsc1641_pow;1
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

#include "Tsc1641_Pow_PnPL.h"

static const IPnPLComponent_vtbl sTsc1641_Pow_PnPL_CompIF_vtbl =
{
  Tsc1641_Pow_PnPL_vtblGetKey,
  Tsc1641_Pow_PnPL_vtblGetNCommands,
  Tsc1641_Pow_PnPL_vtblGetCommandKey,
  Tsc1641_Pow_PnPL_vtblGetStatus,
  Tsc1641_Pow_PnPL_vtblSetProperty,
  Tsc1641_Pow_PnPL_vtblExecuteFunction
};

/**
  *  Tsc1641_Pow_PnPL internal structure.
  */
struct _Tsc1641_Pow_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Tsc1641_Pow_PnPL sTsc1641_Pow_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Tsc1641_Pow_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sTsc1641_Pow_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sTsc1641_Pow_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Tsc1641_Pow_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  tsc1641_pow_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Tsc1641_Pow_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return tsc1641_pow_get_key();
}

uint8_t Tsc1641_Pow_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Tsc1641_Pow_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Tsc1641_Pow_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  pnpl_tsc1641_pow_adc_conversion_time_t temp_adc_conversion_time_e = (pnpl_tsc1641_pow_adc_conversion_time_t)0;
  tsc1641_pow_get_adc_conversion_time(&temp_adc_conversion_time_e);
  json_object_dotset_number(JSON_Status, "tsc1641_pow.adc_conversion_time", temp_adc_conversion_time_e);
  int32_t temp_i = 0;
  tsc1641_pow_get_r_shunt(&temp_i);
  json_object_dotset_number(JSON_Status, "tsc1641_pow.r_shunt", temp_i);
  bool temp_b = 0;
  tsc1641_pow_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tsc1641_pow.enable", temp_b);
  tsc1641_pow_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "tsc1641_pow.samples_per_ts", temp_i);
  tsc1641_pow_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "tsc1641_pow.dim", temp_i);
  float temp_f = 0;
  tsc1641_pow_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "tsc1641_pow.ioffset", temp_f);
  char *temp_s = "";
  tsc1641_pow_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "tsc1641_pow.data_type", temp_s);
  tsc1641_pow_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "tsc1641_pow.usb_dps", temp_i);
  tsc1641_pow_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "tsc1641_pow.sd_dps", temp_i);
  tsc1641_pow_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "tsc1641_pow.sensor_annotation", temp_s);
  tsc1641_pow_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "tsc1641_pow.sensor_category", temp_i);
  tsc1641_pow_get_mounted(&temp_b);
  json_object_dotset_boolean(JSON_Status, "tsc1641_pow.mounted", temp_b);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "tsc1641_pow.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  tsc1641_pow_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "tsc1641_pow.stream_id", temp_int8);
  tsc1641_pow_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "tsc1641_pow.ep_id", temp_int8);

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

uint8_t Tsc1641_Pow_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size,
                                         uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  bool valid_property = false;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "tsc1641_pow.adc_conversion_time"))
  {
    valid_property = true;
    int32_t adc_conversion_time = (int32_t)json_object_dotget_number(tempJSONObject, "tsc1641_pow.adc_conversion_time");
    ret = tsc1641_pow_set_adc_conversion_time((pnpl_tsc1641_pow_adc_conversion_time_t)adc_conversion_time, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", adc_conversion_time);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_tsc1641_pow_adc_conversion_time_t old_adc_conversion_time;
      tsc1641_pow_get_adc_conversion_time(&old_adc_conversion_time);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_adc_conversion_time);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tsc1641_pow.enable"))
  {
    valid_property = true;
    bool enable = json_object_dotget_boolean(tempJSONObject, "tsc1641_pow.enable");
    ret = tsc1641_pow_set_enable(enable, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_enable;
      tsc1641_pow_get_enable(&old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "tsc1641_pow.sensor_annotation"))
  {
    valid_property = true;
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "tsc1641_pow.sensor_annotation");
    ret = tsc1641_pow_set_sensor_annotation(sensor_annotation, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_sensor_annotation;
      tsc1641_pow_get_sensor_annotation(&old_sensor_annotation);
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  json_value_free(tempJSON);
  /* Check if received a request to modify an existing property */
  if (valid_property)
  {
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
  }
  else
  {
    /* Set property is not containing a valid property/parameter: PnPL_Error */
    char *log_message = "Invalid property for Tsc1641_Pow";
    PnPLCreateLogMessage(response, size, log_message, PNPL_LOG_ERROR);
  }
  json_value_free(respJSON);
  return ret;
}


uint8_t Tsc1641_Pow_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                             uint32_t *size, uint8_t pretty)
{
  return PNPL_NO_COMMANDS_ERROR_CODE;
}

