/**
  ******************************************************************************
  * @file    Sths34pf80_2_Tmos_PnPL.c
  * @author  SRA
  * @brief   Sths34pf80_2_Tmos PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect:sensors:sths34pf80_2_tmos;2
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

#include "Sths34pf80_2_Tmos_PnPL.h"

static const IPnPLComponent_vtbl sSths34pf80_2_Tmos_PnPL_CompIF_vtbl =
{
  Sths34pf80_2_Tmos_PnPL_vtblGetKey,
  Sths34pf80_2_Tmos_PnPL_vtblGetNCommands,
  Sths34pf80_2_Tmos_PnPL_vtblGetCommandKey,
  Sths34pf80_2_Tmos_PnPL_vtblGetStatus,
  Sths34pf80_2_Tmos_PnPL_vtblSetProperty,
  Sths34pf80_2_Tmos_PnPL_vtblExecuteFunction
};

/**
  *  Sths34pf80_2_Tmos_PnPL internal structure.
  */
struct _Sths34pf80_2_Tmos_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Sths34pf80_2_Tmos_PnPL sSths34pf80_2_Tmos_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Sths34pf80_2_Tmos_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sSths34pf80_2_Tmos_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sSths34pf80_2_Tmos_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Sths34pf80_2_Tmos_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  sths34pf80_2_tmos_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Sths34pf80_2_Tmos_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return sths34pf80_2_tmos_get_key();
}

uint8_t Sths34pf80_2_Tmos_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Sths34pf80_2_Tmos_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Sths34pf80_2_Tmos_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                             uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  bool temp_b = 0;
  sths34pf80_2_tmos_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "sths34pf80_2_tmos.enable", temp_b);
  pnpl_sths34pf80_2_tmos_odr_t temp_odr_e = (pnpl_sths34pf80_2_tmos_odr_t)0;
  sths34pf80_2_tmos_get_odr(&temp_odr_e);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.odr", temp_odr_e);
  float temp_f = 0;
  sths34pf80_2_tmos_get_transmittance(&temp_f);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.transmittance", temp_f);
  pnpl_sths34pf80_2_tmos_avg_tobject_num_t temp_avg_tobject_num_e = (pnpl_sths34pf80_2_tmos_avg_tobject_num_t)0;
  sths34pf80_2_tmos_get_avg_tobject_num(&temp_avg_tobject_num_e);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.avg_tobject_num", temp_avg_tobject_num_e);
  pnpl_sths34pf80_2_tmos_avg_tambient_num_t temp_avg_tambient_num_e = (pnpl_sths34pf80_2_tmos_avg_tambient_num_t)0;
  sths34pf80_2_tmos_get_avg_tambient_num(&temp_avg_tambient_num_e);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.avg_tambient_num", temp_avg_tambient_num_e);
  pnpl_sths34pf80_2_tmos_lpf_p_m_bandwidth_t temp_lpf_p_m_bandwidth_e = (pnpl_sths34pf80_2_tmos_lpf_p_m_bandwidth_t)0;
  sths34pf80_2_tmos_get_lpf_p_m_bandwidth(&temp_lpf_p_m_bandwidth_e);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.lpf_p_m_bandwidth", temp_lpf_p_m_bandwidth_e);
  pnpl_sths34pf80_2_tmos_lpf_p_bandwidth_t temp_lpf_p_bandwidth_e = (pnpl_sths34pf80_2_tmos_lpf_p_bandwidth_t)0;
  sths34pf80_2_tmos_get_lpf_p_bandwidth(&temp_lpf_p_bandwidth_e);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.lpf_p_bandwidth", temp_lpf_p_bandwidth_e);
  pnpl_sths34pf80_2_tmos_lpf_m_bandwidth_t temp_lpf_m_bandwidth_e = (pnpl_sths34pf80_2_tmos_lpf_m_bandwidth_t)0;
  sths34pf80_2_tmos_get_lpf_m_bandwidth(&temp_lpf_m_bandwidth_e);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.lpf_m_bandwidth", temp_lpf_m_bandwidth_e);
  int32_t temp_i = 0;
  sths34pf80_2_tmos_get_presence_threshold(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.presence_threshold", temp_i);
  sths34pf80_2_tmos_get_presence_hysteresis(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.presence_hysteresis", temp_i);
  sths34pf80_2_tmos_get_motion_threshold(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.motion_threshold", temp_i);
  sths34pf80_2_tmos_get_motion_hysteresis(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.motion_hysteresis", temp_i);
  sths34pf80_2_tmos_get_tambient_shock_threshold(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.tambient_shock_threshold", temp_i);
  sths34pf80_2_tmos_get_tambient_shock_hysteresis(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.tambient_shock_hysteresis", temp_i);
  sths34pf80_2_tmos_get_embedded_compensation(&temp_b);
  json_object_dotset_boolean(JSON_Status, "sths34pf80_2_tmos.embedded_compensation", temp_b);
  sths34pf80_2_tmos_get_software_compensation(&temp_b);
  json_object_dotset_boolean(JSON_Status, "sths34pf80_2_tmos.software_compensation", temp_b);
  pnpl_sths34pf80_2_tmos_compensation_type_t temp_compensation_type_e = (pnpl_sths34pf80_2_tmos_compensation_type_t)0;
  sths34pf80_2_tmos_get_compensation_type(&temp_compensation_type_e);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.compensation_type", temp_compensation_type_e);
  sths34pf80_2_tmos_get_sw_presence_threshold(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.sw_presence_threshold", temp_i);
  sths34pf80_2_tmos_get_sw_motion_threshold(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.sw_motion_threshold", temp_i);
  sths34pf80_2_tmos_get_compensation_filter_flag(&temp_b);
  json_object_dotset_boolean(JSON_Status, "sths34pf80_2_tmos.compensation_filter_flag", temp_b);
  sths34pf80_2_tmos_get_absence_static_flag(&temp_b);
  json_object_dotset_boolean(JSON_Status, "sths34pf80_2_tmos.absence_static_flag", temp_b);
  sths34pf80_2_tmos_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.samples_per_ts", temp_i);
  sths34pf80_2_tmos_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.ioffset", temp_f);
  sths34pf80_2_tmos_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.usb_dps", temp_i);
  sths34pf80_2_tmos_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.sd_dps", temp_i);
  char *temp_s = "";
  sths34pf80_2_tmos_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "sths34pf80_2_tmos.data_type", temp_s);
  sths34pf80_2_tmos_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "sths34pf80_2_tmos.sensor_annotation", temp_s);
  sths34pf80_2_tmos_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.sensor_category", temp_i);
  sths34pf80_2_tmos_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.dim", temp_i);
  sths34pf80_2_tmos_get_mounted(&temp_b);
  json_object_dotset_boolean(JSON_Status, "sths34pf80_2_tmos.mounted", temp_b);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  sths34pf80_2_tmos_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.stream_id", temp_int8);
  sths34pf80_2_tmos_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "sths34pf80_2_tmos.ep_id", temp_int8);

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

uint8_t Sths34pf80_2_Tmos_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                               uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  bool valid_property = false;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "sths34pf80_2_tmos.enable");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_enable(enable, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_enable;
      sths34pf80_2_tmos_get_enable(&old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.odr"))
  {
    int32_t odr = (int32_t)json_object_dotget_number(tempJSONObject, "sths34pf80_2_tmos.odr");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_odr((pnpl_sths34pf80_2_tmos_odr_t)odr, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", odr);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_sths34pf80_2_tmos_odr_t old_odr;
      sths34pf80_2_tmos_get_odr(&old_odr);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_odr);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.transmittance"))
  {
    float transmittance = (float)json_object_dotget_number(tempJSONObject, "sths34pf80_2_tmos.transmittance");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_transmittance(transmittance, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", transmittance);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      float old_transmittance;
      sths34pf80_2_tmos_get_transmittance(&old_transmittance);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_transmittance);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.avg_tobject_num"))
  {
    int32_t avg_tobject_num = (int32_t)json_object_dotget_number(tempJSONObject, "sths34pf80_2_tmos.avg_tobject_num");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_avg_tobject_num((pnpl_sths34pf80_2_tmos_avg_tobject_num_t)avg_tobject_num, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", avg_tobject_num);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_sths34pf80_2_tmos_avg_tobject_num_t old_avg_tobject_num;
      sths34pf80_2_tmos_get_avg_tobject_num(&old_avg_tobject_num);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_avg_tobject_num);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.avg_tambient_num"))
  {
    int32_t avg_tambient_num = (int32_t)json_object_dotget_number(tempJSONObject, "sths34pf80_2_tmos.avg_tambient_num");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_avg_tambient_num((pnpl_sths34pf80_2_tmos_avg_tambient_num_t)avg_tambient_num, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", avg_tambient_num);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_sths34pf80_2_tmos_avg_tambient_num_t old_avg_tambient_num;
      sths34pf80_2_tmos_get_avg_tambient_num(&old_avg_tambient_num);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_avg_tambient_num);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.lpf_p_m_bandwidth"))
  {
    int32_t lpf_p_m_bandwidth = (int32_t)json_object_dotget_number(tempJSONObject, "sths34pf80_2_tmos.lpf_p_m_bandwidth");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_lpf_p_m_bandwidth((pnpl_sths34pf80_2_tmos_lpf_p_m_bandwidth_t)lpf_p_m_bandwidth, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", lpf_p_m_bandwidth);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_sths34pf80_2_tmos_lpf_p_m_bandwidth_t old_lpf_p_m_bandwidth;
      sths34pf80_2_tmos_get_lpf_p_m_bandwidth(&old_lpf_p_m_bandwidth);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_lpf_p_m_bandwidth);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.lpf_p_bandwidth"))
  {
    int32_t lpf_p_bandwidth = (int32_t)json_object_dotget_number(tempJSONObject, "sths34pf80_2_tmos.lpf_p_bandwidth");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_lpf_p_bandwidth((pnpl_sths34pf80_2_tmos_lpf_p_bandwidth_t)lpf_p_bandwidth, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", lpf_p_bandwidth);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_sths34pf80_2_tmos_lpf_p_bandwidth_t old_lpf_p_bandwidth;
      sths34pf80_2_tmos_get_lpf_p_bandwidth(&old_lpf_p_bandwidth);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_lpf_p_bandwidth);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.lpf_m_bandwidth"))
  {
    int32_t lpf_m_bandwidth = (int32_t)json_object_dotget_number(tempJSONObject, "sths34pf80_2_tmos.lpf_m_bandwidth");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_lpf_m_bandwidth((pnpl_sths34pf80_2_tmos_lpf_m_bandwidth_t)lpf_m_bandwidth, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", lpf_m_bandwidth);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_sths34pf80_2_tmos_lpf_m_bandwidth_t old_lpf_m_bandwidth;
      sths34pf80_2_tmos_get_lpf_m_bandwidth(&old_lpf_m_bandwidth);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_lpf_m_bandwidth);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.presence_threshold"))
  {
    int32_t presence_threshold = (int32_t)json_object_dotget_number(tempJSONObject, "sths34pf80_2_tmos.presence_threshold");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_presence_threshold(presence_threshold, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", presence_threshold);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_presence_threshold;
      sths34pf80_2_tmos_get_presence_threshold(&old_presence_threshold);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_presence_threshold);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.presence_hysteresis"))
  {
    int32_t presence_hysteresis = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                     "sths34pf80_2_tmos.presence_hysteresis");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_presence_hysteresis(presence_hysteresis, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", presence_hysteresis);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_presence_hysteresis;
      sths34pf80_2_tmos_get_presence_hysteresis(&old_presence_hysteresis);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_presence_hysteresis);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.motion_threshold"))
  {
    int32_t motion_threshold = (int32_t)json_object_dotget_number(tempJSONObject, "sths34pf80_2_tmos.motion_threshold");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_motion_threshold(motion_threshold, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", motion_threshold);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_motion_threshold;
      sths34pf80_2_tmos_get_motion_threshold(&old_motion_threshold);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_motion_threshold);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.motion_hysteresis"))
  {
    int32_t motion_hysteresis = (int32_t)json_object_dotget_number(tempJSONObject, "sths34pf80_2_tmos.motion_hysteresis");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_motion_hysteresis(motion_hysteresis, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", motion_hysteresis);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_motion_hysteresis;
      sths34pf80_2_tmos_get_motion_hysteresis(&old_motion_hysteresis);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_motion_hysteresis);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.tambient_shock_threshold"))
  {
    int32_t tambient_shock_threshold = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                          "sths34pf80_2_tmos.tambient_shock_threshold");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_tambient_shock_threshold(tambient_shock_threshold, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", tambient_shock_threshold);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_tambient_shock_threshold;
      sths34pf80_2_tmos_get_tambient_shock_threshold(&old_tambient_shock_threshold);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_tambient_shock_threshold);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.tambient_shock_hysteresis"))
  {
    int32_t tambient_shock_hysteresis = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                           "sths34pf80_2_tmos.tambient_shock_hysteresis");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_tambient_shock_hysteresis(tambient_shock_hysteresis, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", tambient_shock_hysteresis);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_tambient_shock_hysteresis;
      sths34pf80_2_tmos_get_tambient_shock_hysteresis(&old_tambient_shock_hysteresis);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_tambient_shock_hysteresis);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.embedded_compensation"))
  {
    bool embedded_compensation = json_object_dotget_boolean(tempJSONObject, "sths34pf80_2_tmos.embedded_compensation");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_embedded_compensation(embedded_compensation, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", embedded_compensation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_embedded_compensation;
      sths34pf80_2_tmos_get_embedded_compensation(&old_embedded_compensation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_embedded_compensation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.software_compensation"))
  {
    bool software_compensation = json_object_dotget_boolean(tempJSONObject, "sths34pf80_2_tmos.software_compensation");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_software_compensation(software_compensation, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", software_compensation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_software_compensation;
      sths34pf80_2_tmos_get_software_compensation(&old_software_compensation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_software_compensation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.compensation_type"))
  {
    int32_t compensation_type = (int32_t)json_object_dotget_number(tempJSONObject, "sths34pf80_2_tmos.compensation_type");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_compensation_type((pnpl_sths34pf80_2_tmos_compensation_type_t)compensation_type, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", compensation_type);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      pnpl_sths34pf80_2_tmos_compensation_type_t old_compensation_type;
      sths34pf80_2_tmos_get_compensation_type(&old_compensation_type);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_compensation_type);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.sw_presence_threshold"))
  {
    int32_t sw_presence_threshold = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                       "sths34pf80_2_tmos.sw_presence_threshold");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_sw_presence_threshold(sw_presence_threshold, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", sw_presence_threshold);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_sw_presence_threshold;
      sths34pf80_2_tmos_get_sw_presence_threshold(&old_sw_presence_threshold);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_sw_presence_threshold);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.sw_motion_threshold"))
  {
    int32_t sw_motion_threshold = (int32_t)json_object_dotget_number(tempJSONObject,
                                                                     "sths34pf80_2_tmos.sw_motion_threshold");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_sw_motion_threshold(sw_motion_threshold, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", sw_motion_threshold);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_sw_motion_threshold;
      sths34pf80_2_tmos_get_sw_motion_threshold(&old_sw_motion_threshold);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_sw_motion_threshold);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.compensation_filter_flag"))
  {
    bool compensation_filter_flag = json_object_dotget_boolean(tempJSONObject,
                                                               "sths34pf80_2_tmos.compensation_filter_flag");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_compensation_filter_flag(compensation_filter_flag, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", compensation_filter_flag);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_compensation_filter_flag;
      sths34pf80_2_tmos_get_compensation_filter_flag(&old_compensation_filter_flag);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_compensation_filter_flag);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.absence_static_flag"))
  {
    bool absence_static_flag = json_object_dotget_boolean(tempJSONObject, "sths34pf80_2_tmos.absence_static_flag");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_absence_static_flag(absence_static_flag, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", absence_static_flag);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_absence_static_flag;
      sths34pf80_2_tmos_get_absence_static_flag(&old_absence_static_flag);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_absence_static_flag);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.samples_per_ts"))
  {
    int32_t samples_per_ts = (int32_t)json_object_dotget_number(tempJSONObject, "sths34pf80_2_tmos.samples_per_ts");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_samples_per_ts(samples_per_ts, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", samples_per_ts);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      int32_t old_samples_per_ts;
      sths34pf80_2_tmos_get_samples_per_ts(&old_samples_per_ts);
      json_object_dotset_number(respJSONObject, "PnPL_Response.value", old_samples_per_ts);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_2_tmos.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "sths34pf80_2_tmos.sensor_annotation");
    valid_property = true;
    ret = sths34pf80_2_tmos_set_sensor_annotation(sensor_annotation, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_sensor_annotation;
      sths34pf80_2_tmos_get_sensor_annotation(&old_sensor_annotation);
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", old_sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  json_value_free(tempJSON);
  /* Check if received a valid request to modify an existing property */
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
    char *log_message = "Invalid property for sths34pf80_2_tmos";
    PnPLCreateLogMessage(response, size, log_message, PNPL_LOG_ERROR);
    ret = PNPL_BASE_ERROR_CODE;
  }
  json_value_free(respJSON);
  return ret;
}


uint8_t Sths34pf80_2_Tmos_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                   uint32_t *size, uint8_t pretty)
{
  return PNPL_NO_COMMANDS_ERROR_CODE;
}

