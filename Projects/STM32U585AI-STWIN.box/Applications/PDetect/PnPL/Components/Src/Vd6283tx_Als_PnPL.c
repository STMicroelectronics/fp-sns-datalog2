/**
  ******************************************************************************
  * @file    Vd6283tx_Als_PnPL.c
  * @author  SRA
  * @brief   Vd6283tx_Als PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect:sensors:vd6283tx_als;1
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

#include "Vd6283tx_Als_PnPL.h"

static const IPnPLComponent_vtbl sVd6283tx_Als_PnPL_CompIF_vtbl =
{
  Vd6283tx_Als_PnPL_vtblGetKey,
  Vd6283tx_Als_PnPL_vtblGetNCommands,
  Vd6283tx_Als_PnPL_vtblGetCommandKey,
  Vd6283tx_Als_PnPL_vtblGetStatus,
  Vd6283tx_Als_PnPL_vtblSetProperty,
  Vd6283tx_Als_PnPL_vtblExecuteFunction
};

/**
  *  Vd6283tx_Als_PnPL internal structure.
  */
struct _Vd6283tx_Als_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Vd6283tx_Als_PnPL sVd6283tx_Als_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Vd6283tx_Als_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sVd6283tx_Als_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sVd6283tx_Als_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Vd6283tx_Als_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  vd6283tx_als_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Vd6283tx_Als_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return vd6283tx_als_get_key();
}

uint8_t Vd6283tx_Als_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Vd6283tx_Als_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Vd6283tx_Als_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  bool temp_b = 0;
  vd6283tx_als_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "vd6283tx_als.enable", temp_b);
  int32_t temp_i = 0;
  vd6283tx_als_get_exposure_time(&temp_i);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.exposure_time", temp_i);
  vd6283tx_als_get_intermeasurement_time(&temp_i);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.intermeasurement_time", temp_i);
  pnpl_vd6283tx_als_channel1_gain_t temp_channel1_gain_e = (pnpl_vd6283tx_als_channel1_gain_t)0;
  vd6283tx_als_get_channel1_gain(&temp_channel1_gain_e);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.channel1_gain", temp_channel1_gain_e);
  pnpl_vd6283tx_als_channel2_gain_t temp_channel2_gain_e = (pnpl_vd6283tx_als_channel2_gain_t)0;
  vd6283tx_als_get_channel2_gain(&temp_channel2_gain_e);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.channel2_gain", temp_channel2_gain_e);
  pnpl_vd6283tx_als_channel3_gain_t temp_channel3_gain_e = (pnpl_vd6283tx_als_channel3_gain_t)0;
  vd6283tx_als_get_channel3_gain(&temp_channel3_gain_e);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.channel3_gain", temp_channel3_gain_e);
  pnpl_vd6283tx_als_channel4_gain_t temp_channel4_gain_e = (pnpl_vd6283tx_als_channel4_gain_t)0;
  vd6283tx_als_get_channel4_gain(&temp_channel4_gain_e);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.channel4_gain", temp_channel4_gain_e);
  pnpl_vd6283tx_als_channel5_gain_t temp_channel5_gain_e = (pnpl_vd6283tx_als_channel5_gain_t)0;
  vd6283tx_als_get_channel5_gain(&temp_channel5_gain_e);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.channel5_gain", temp_channel5_gain_e);
  pnpl_vd6283tx_als_channel6_gain_t temp_channel6_gain_e = (pnpl_vd6283tx_als_channel6_gain_t)0;
  vd6283tx_als_get_channel6_gain(&temp_channel6_gain_e);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.channel6_gain", temp_channel6_gain_e);
  vd6283tx_als_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.samples_per_ts", temp_i);
  float temp_f = 0;
  vd6283tx_als_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.ioffset", temp_f);
  vd6283tx_als_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.usb_dps", temp_i);
  vd6283tx_als_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.sd_dps", temp_i);
  char *temp_s = "";
  vd6283tx_als_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "vd6283tx_als.data_type", temp_s);
  vd6283tx_als_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "vd6283tx_als.sensor_annotation", temp_s);
  vd6283tx_als_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.sensor_category", temp_i);
  vd6283tx_als_get_mounted(&temp_b);
  json_object_dotset_boolean(JSON_Status, "vd6283tx_als.mounted", temp_b);
  vd6283tx_als_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.dim", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "vd6283tx_als.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  vd6283tx_als_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.stream_id", temp_int8);
  vd6283tx_als_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.ep_id", temp_int8);

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

uint8_t Vd6283tx_Als_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                          uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "vd6283tx_als.enable");
    ret = vd6283tx_als_set_enable(enable, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_enable;
      vd6283tx_als_get_enable(&old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.exposure_time"))
  {
    int32_t exposure_time = (int32_t)json_object_dotget_number(tempJSONObject, "vd6283tx_als.exposure_time");
    ret = vd6283tx_als_set_exposure_time(exposure_time, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", exposure_time);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_exposure_time;
      vd6283tx_als_get_exposure_time(&old_exposure_time);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_exposure_time);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.intermeasurement_time"))
  {
    int32_t intermeasurement_time = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                       "vd6283tx_als.intermeasurement_time");
    ret = vd6283tx_als_set_intermeasurement_time(intermeasurement_time, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", intermeasurement_time);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_intermeasurement_time;
      vd6283tx_als_get_intermeasurement_time(&old_intermeasurement_time);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_intermeasurement_time);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.channel1_gain"))
  {
    int32_t channel1_gain = (int32_t)json_object_dotget_number(tempJSONObject, "vd6283tx_als.channel1_gain");
    ret = vd6283tx_als_set_channel1_gain((pnpl_vd6283tx_als_channel1_gain_t)channel1_gain, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", channel1_gain);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_vd6283tx_als_channel1_gain_t old_channel1_gain;
      vd6283tx_als_get_channel1_gain(&old_channel1_gain);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_channel1_gain);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.channel2_gain"))
  {
    int32_t channel2_gain = (int32_t)json_object_dotget_number(tempJSONObject, "vd6283tx_als.channel2_gain");
    ret = vd6283tx_als_set_channel2_gain((pnpl_vd6283tx_als_channel2_gain_t)channel2_gain, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", channel2_gain);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_vd6283tx_als_channel2_gain_t old_channel2_gain;
      vd6283tx_als_get_channel2_gain(&old_channel2_gain);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_channel2_gain);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.channel3_gain"))
  {
    int32_t channel3_gain = (int32_t)json_object_dotget_number(tempJSONObject, "vd6283tx_als.channel3_gain");
    ret = vd6283tx_als_set_channel3_gain((pnpl_vd6283tx_als_channel3_gain_t)channel3_gain, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", channel3_gain);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_vd6283tx_als_channel3_gain_t old_channel3_gain;
      vd6283tx_als_get_channel3_gain(&old_channel3_gain);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_channel3_gain);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.channel4_gain"))
  {
    int32_t channel4_gain = (int32_t)json_object_dotget_number(tempJSONObject, "vd6283tx_als.channel4_gain");
    ret = vd6283tx_als_set_channel4_gain((pnpl_vd6283tx_als_channel4_gain_t)channel4_gain, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", channel4_gain);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_vd6283tx_als_channel4_gain_t old_channel4_gain;
      vd6283tx_als_get_channel4_gain(&old_channel4_gain);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_channel4_gain);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.channel5_gain"))
  {
    int32_t channel5_gain = (int32_t)json_object_dotget_number(tempJSONObject, "vd6283tx_als.channel5_gain");
    ret = vd6283tx_als_set_channel5_gain((pnpl_vd6283tx_als_channel5_gain_t)channel5_gain, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", channel5_gain);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_vd6283tx_als_channel5_gain_t old_channel5_gain;
      vd6283tx_als_get_channel5_gain(&old_channel5_gain);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_channel5_gain);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.channel6_gain"))
  {
    int32_t channel6_gain = (int32_t)json_object_dotget_number(tempJSONObject, "vd6283tx_als.channel6_gain");
    ret = vd6283tx_als_set_channel6_gain((pnpl_vd6283tx_als_channel6_gain_t)channel6_gain, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", channel6_gain);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_vd6283tx_als_channel6_gain_t old_channel6_gain;
      vd6283tx_als_get_channel6_gain(&old_channel6_gain);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_channel6_gain);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.samples_per_ts"))
  {
    int32_t samples_per_ts = (int32_t)json_object_dotget_number(tempJSONObject, "vd6283tx_als.samples_per_ts");
    ret = vd6283tx_als_set_samples_per_ts(samples_per_ts, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", samples_per_ts);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_samples_per_ts;
      vd6283tx_als_get_samples_per_ts(&old_samples_per_ts);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_samples_per_ts);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "vd6283tx_als.sensor_annotation");
    ret = vd6283tx_als_set_sensor_annotation(sensor_annotation, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_sensor_annotation;
      vd6283tx_als_get_sensor_annotation(&old_sensor_annotation);
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
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


uint8_t Vd6283tx_Als_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                              uint32_t *size, uint8_t pretty)
{
  return PNPL_NO_COMMANDS_ERROR_CODE;
}

