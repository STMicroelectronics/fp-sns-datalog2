/**
  ******************************************************************************
  * @file    Sths34pf80_Tmos_PnPL.c
  * @author  SRA
  * @brief   Sths34pf80_Tmos PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect:sensors:sths34pf80_tmos;1
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

#include "Sths34pf80_Tmos_PnPL.h"
#include "Sths34pf80_Tmos_PnPL_vtbl.h"

static const IPnPLComponent_vtbl sSths34pf80_Tmos_PnPL_CompIF_vtbl =
{
  Sths34pf80_Tmos_PnPL_vtblGetKey,
  Sths34pf80_Tmos_PnPL_vtblGetNCommands,
  Sths34pf80_Tmos_PnPL_vtblGetCommandKey,
  Sths34pf80_Tmos_PnPL_vtblGetStatus,
  Sths34pf80_Tmos_PnPL_vtblSetProperty,
  Sths34pf80_Tmos_PnPL_vtblExecuteFunction
};

/**
  *  Sths34pf80_Tmos_PnPL internal structure.
  */
struct _Sths34pf80_Tmos_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;

};

/* Objects instance ----------------------------------------------------------*/
static Sths34pf80_Tmos_PnPL sSths34pf80_Tmos_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Sths34pf80_Tmos_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sSths34pf80_Tmos_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sSths34pf80_Tmos_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Sths34pf80_Tmos_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  sths34pf80_tmos_comp_init();
  return 0;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Sths34pf80_Tmos_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return sths34pf80_tmos_get_key();
}

uint8_t Sths34pf80_Tmos_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Sths34pf80_Tmos_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Sths34pf80_Tmos_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                           uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  bool temp_b = 0;
  sths34pf80_tmos_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "sths34pf80_tmos.enable", temp_b);
  int32_t temp_i = 0;
  sths34pf80_tmos_get_odr(&temp_i);
  uint8_t enum_id = 0;
  if (temp_i == sths34pf80_tmos_odr_hz1)
  {
    enum_id = 0;
  }
  else if (temp_i == sths34pf80_tmos_odr_hz2)
  {
    enum_id = 1;
  }
  else if (temp_i == sths34pf80_tmos_odr_hz4)
  {
    enum_id = 2;
  }
  else if (temp_i == sths34pf80_tmos_odr_hz8)
  {
    enum_id = 3;
  }
  else if (temp_i == sths34pf80_tmos_odr_hz15)
  {
    enum_id = 4;
  }
  else if (temp_i == sths34pf80_tmos_odr_hz30)
  {
    enum_id = 5;
  }
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.odr", enum_id);
  float temp_f = 0;
  sths34pf80_tmos_get_transmittance(&temp_f);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.transmittance", temp_f);
  sths34pf80_tmos_get_avg_tobject_num(&temp_i);
  enum_id = 0;
  if (temp_i == sths34pf80_tmos_avg_tobject_num_n2)
  {
    enum_id = 0;
  }
  else if (temp_i == sths34pf80_tmos_avg_tobject_num_n8)
  {
    enum_id = 1;
  }
  else if (temp_i == sths34pf80_tmos_avg_tobject_num_n32)
  {
    enum_id = 2;
  }
  else if (temp_i == sths34pf80_tmos_avg_tobject_num_n128)
  {
    enum_id = 3;
  }
  else if (temp_i == sths34pf80_tmos_avg_tobject_num_n256)
  {
    enum_id = 4;
  }
  else if (temp_i == sths34pf80_tmos_avg_tobject_num_n512)
  {
    enum_id = 5;
  }
  else if (temp_i == sths34pf80_tmos_avg_tobject_num_n1024)
  {
    enum_id = 6;
  }
  else if (temp_i == sths34pf80_tmos_avg_tobject_num_n2048)
  {
    enum_id = 7;
  }
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.avg_tobject_num", enum_id);
  sths34pf80_tmos_get_avg_tambient_num(&temp_i);
  enum_id = 0;
  if (temp_i == sths34pf80_tmos_avg_tambient_num_n1)
  {
    enum_id = 0;
  }
  else if (temp_i == sths34pf80_tmos_avg_tambient_num_n2)
  {
    enum_id = 1;
  }
  else if (temp_i == sths34pf80_tmos_avg_tambient_num_n4)
  {
    enum_id = 2;
  }
  else if (temp_i == sths34pf80_tmos_avg_tambient_num_n8)
  {
    enum_id = 3;
  }
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.avg_tambient_num", enum_id);
  sths34pf80_tmos_get_lpf_p_m_bandwidth(&temp_i);
  enum_id = 0;
  if (temp_i == sths34pf80_tmos_lpf_p_m_bandwidth_n9)
  {
    enum_id = 0;
  }
  else if (temp_i == sths34pf80_tmos_lpf_p_m_bandwidth_n20)
  {
    enum_id = 1;
  }
  else if (temp_i == sths34pf80_tmos_lpf_p_m_bandwidth_n50)
  {
    enum_id = 2;
  }
  else if (temp_i == sths34pf80_tmos_lpf_p_m_bandwidth_n100)
  {
    enum_id = 3;
  }
  else if (temp_i == sths34pf80_tmos_lpf_p_m_bandwidth_n200)
  {
    enum_id = 4;
  }
  else if (temp_i == sths34pf80_tmos_lpf_p_m_bandwidth_n400)
  {
    enum_id = 5;
  }
  else if (temp_i == sths34pf80_tmos_lpf_p_m_bandwidth_n800)
  {
    enum_id = 6;
  }
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.lpf_p_m_bandwidth", enum_id);
  sths34pf80_tmos_get_lpf_p_bandwidth(&temp_i);
  enum_id = 0;
  if (temp_i == sths34pf80_tmos_lpf_p_bandwidth_n9)
  {
    enum_id = 0;
  }
  else if (temp_i == sths34pf80_tmos_lpf_p_bandwidth_n20)
  {
    enum_id = 1;
  }
  else if (temp_i == sths34pf80_tmos_lpf_p_bandwidth_n50)
  {
    enum_id = 2;
  }
  else if (temp_i == sths34pf80_tmos_lpf_p_bandwidth_n100)
  {
    enum_id = 3;
  }
  else if (temp_i == sths34pf80_tmos_lpf_p_bandwidth_n200)
  {
    enum_id = 4;
  }
  else if (temp_i == sths34pf80_tmos_lpf_p_bandwidth_n400)
  {
    enum_id = 5;
  }
  else if (temp_i == sths34pf80_tmos_lpf_p_bandwidth_n800)
  {
    enum_id = 6;
  }
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.lpf_p_bandwidth", enum_id);
  sths34pf80_tmos_get_lpf_m_bandwidth(&temp_i);
  enum_id = 0;
  if (temp_i == sths34pf80_tmos_lpf_m_bandwidth_n9)
  {
    enum_id = 0;
  }
  else if (temp_i == sths34pf80_tmos_lpf_m_bandwidth_n20)
  {
    enum_id = 1;
  }
  else if (temp_i == sths34pf80_tmos_lpf_m_bandwidth_n50)
  {
    enum_id = 2;
  }
  else if (temp_i == sths34pf80_tmos_lpf_m_bandwidth_n100)
  {
    enum_id = 3;
  }
  else if (temp_i == sths34pf80_tmos_lpf_m_bandwidth_n200)
  {
    enum_id = 4;
  }
  else if (temp_i == sths34pf80_tmos_lpf_m_bandwidth_n400)
  {
    enum_id = 5;
  }
  else if (temp_i == sths34pf80_tmos_lpf_m_bandwidth_n800)
  {
    enum_id = 6;
  }
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.lpf_m_bandwidth", enum_id);
  sths34pf80_tmos_get_presence_threshold(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.presence_threshold", temp_i);
  sths34pf80_tmos_get_presence_hysteresis(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.presence_hysteresis", temp_i);
  sths34pf80_tmos_get_motion_threshold(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.motion_threshold", temp_i);
  sths34pf80_tmos_get_motion_hysteresis(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.motion_hysteresis", temp_i);
  sths34pf80_tmos_get_tambient_shock_threshold(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.tambient_shock_threshold", temp_i);
  sths34pf80_tmos_get_tambient_shock_hysteresis(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.tambient_shock_hysteresis", temp_i);
  sths34pf80_tmos_get_embedded_compensation(&temp_b);
  json_object_dotset_boolean(JSON_Status, "sths34pf80_tmos.embedded_compensation", temp_b);
  sths34pf80_tmos_get_software_compensation(&temp_b);
  json_object_dotset_boolean(JSON_Status, "sths34pf80_tmos.software_compensation", temp_b);
  char *temp_s = "";
  sths34pf80_tmos_get_compensation_type(&temp_s);
  enum_id = 0;
  if (strcmp(temp_s, sths34pf80_tmos_compensation_type_ipd_comp_none) == 0)
  {
    enum_id = 0;
  }
  else if (strcmp(temp_s, sths34pf80_tmos_compensation_type_ipd_comp_lin) == 0)
  {
    enum_id = 1;
  }
  else if (strcmp(temp_s, sths34pf80_tmos_compensation_type_ipd_comp_nonlin) == 0)
  {
    enum_id = 2;
  }
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.compensation_type", enum_id);
  sths34pf80_tmos_get_sw_presence_threshold(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.sw_presence_threshold", temp_i);
  sths34pf80_tmos_get_sw_motion_threshold(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.sw_motion_threshold", temp_i);
  sths34pf80_tmos_get_compensation_filter_flag(&temp_b);
  json_object_dotset_boolean(JSON_Status, "sths34pf80_tmos.compensation_filter_flag", temp_b);
  sths34pf80_tmos_get_absence_static_flag(&temp_b);
  json_object_dotset_boolean(JSON_Status, "sths34pf80_tmos.absence_static_flag", temp_b);
  sths34pf80_tmos_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.samples_per_ts", temp_i);
  sths34pf80_tmos_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.ioffset", temp_f);
  sths34pf80_tmos_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.usb_dps", temp_i);
  sths34pf80_tmos_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.sd_dps", temp_i);
  sths34pf80_tmos_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "sths34pf80_tmos.data_type", temp_s);
  sths34pf80_tmos_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "sths34pf80_tmos.sensor_annotation", temp_s);
  sths34pf80_tmos_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.sensor_category", temp_i);
  sths34pf80_tmos_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.dim", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  sths34pf80_tmos_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.stream_id", temp_int8);
  sths34pf80_tmos_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "sths34pf80_tmos.ep_id", temp_int8);

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

uint8_t Sths34pf80_Tmos_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                             uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = 0;
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "sths34pf80_tmos.enable");
    ret = sths34pf80_tmos_set_enable(enable);
    if (ret == 0)
    {
      json_object_dotset_boolean(respJSONObject, "sths34pf80_tmos.enable.value", enable);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.enable.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.odr"))
  {
    int odr = (int)json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.odr");
    switch (odr)
    {
      case 0:
        ret = sths34pf80_tmos_set_odr(sths34pf80_tmos_odr_hz1);
        break;
      case 1:
        ret = sths34pf80_tmos_set_odr(sths34pf80_tmos_odr_hz2);
        break;
      case 2:
        ret = sths34pf80_tmos_set_odr(sths34pf80_tmos_odr_hz4);
        break;
      case 3:
        ret = sths34pf80_tmos_set_odr(sths34pf80_tmos_odr_hz8);
        break;
      case 4:
        ret = sths34pf80_tmos_set_odr(sths34pf80_tmos_odr_hz15);
        break;
      case 5:
        ret = sths34pf80_tmos_set_odr(sths34pf80_tmos_odr_hz30);
        break;
    }
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.odr.value", odr);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.odr.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.transmittance"))
  {
    float transmittance = (float) json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.transmittance");
    ret = sths34pf80_tmos_set_transmittance(transmittance);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.transmittance.value", transmittance);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.transmittance.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.avg_tobject_num"))
  {
    int avg_tobject_num = (int)json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.avg_tobject_num");
    switch (avg_tobject_num)
    {
      case 0:
        ret = sths34pf80_tmos_set_avg_tobject_num(sths34pf80_tmos_avg_tobject_num_n2);
        break;
      case 1:
        ret = sths34pf80_tmos_set_avg_tobject_num(sths34pf80_tmos_avg_tobject_num_n8);
        break;
      case 2:
        ret = sths34pf80_tmos_set_avg_tobject_num(sths34pf80_tmos_avg_tobject_num_n32);
        break;
      case 3:
        ret = sths34pf80_tmos_set_avg_tobject_num(sths34pf80_tmos_avg_tobject_num_n128);
        break;
      case 4:
        ret = sths34pf80_tmos_set_avg_tobject_num(sths34pf80_tmos_avg_tobject_num_n256);
        break;
      case 5:
        ret = sths34pf80_tmos_set_avg_tobject_num(sths34pf80_tmos_avg_tobject_num_n512);
        break;
      case 6:
        ret = sths34pf80_tmos_set_avg_tobject_num(sths34pf80_tmos_avg_tobject_num_n1024);
        break;
      case 7:
        ret = sths34pf80_tmos_set_avg_tobject_num(sths34pf80_tmos_avg_tobject_num_n2048);
        break;
    }
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.avg_tobject_num.value", avg_tobject_num);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.avg_tobject_num.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.avg_tambient_num"))
  {
    int avg_tambient_num = (int)json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.avg_tambient_num");
    switch (avg_tambient_num)
    {
      case 0:
        ret = sths34pf80_tmos_set_avg_tambient_num(sths34pf80_tmos_avg_tambient_num_n1);
        break;
      case 1:
        ret = sths34pf80_tmos_set_avg_tambient_num(sths34pf80_tmos_avg_tambient_num_n2);
        break;
      case 2:
        ret = sths34pf80_tmos_set_avg_tambient_num(sths34pf80_tmos_avg_tambient_num_n4);
        break;
      case 3:
        ret = sths34pf80_tmos_set_avg_tambient_num(sths34pf80_tmos_avg_tambient_num_n8);
        break;
    }
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.avg_tambient_num.value", avg_tambient_num);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.avg_tambient_num.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.lpf_p_m_bandwidth"))
  {
    int lpf_p_m_bandwidth = (int)json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.lpf_p_m_bandwidth");
    switch (lpf_p_m_bandwidth)
    {
      case 0:
        ret = sths34pf80_tmos_set_lpf_p_m_bandwidth(sths34pf80_tmos_lpf_p_m_bandwidth_n9);
        break;
      case 1:
        ret = sths34pf80_tmos_set_lpf_p_m_bandwidth(sths34pf80_tmos_lpf_p_m_bandwidth_n20);
        break;
      case 2:
        ret = sths34pf80_tmos_set_lpf_p_m_bandwidth(sths34pf80_tmos_lpf_p_m_bandwidth_n50);
        break;
      case 3:
        ret = sths34pf80_tmos_set_lpf_p_m_bandwidth(sths34pf80_tmos_lpf_p_m_bandwidth_n100);
        break;
      case 4:
        ret = sths34pf80_tmos_set_lpf_p_m_bandwidth(sths34pf80_tmos_lpf_p_m_bandwidth_n200);
        break;
      case 5:
        ret = sths34pf80_tmos_set_lpf_p_m_bandwidth(sths34pf80_tmos_lpf_p_m_bandwidth_n400);
        break;
      case 6:
        ret = sths34pf80_tmos_set_lpf_p_m_bandwidth(sths34pf80_tmos_lpf_p_m_bandwidth_n800);
        break;
    }
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.lpf_p_m_bandwidth.value", lpf_p_m_bandwidth);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.lpf_p_m_bandwidth.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.lpf_p_bandwidth"))
  {
    int lpf_p_bandwidth = (int)json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.lpf_p_bandwidth");
    switch (lpf_p_bandwidth)
    {
      case 0:
        ret = sths34pf80_tmos_set_lpf_p_bandwidth(sths34pf80_tmos_lpf_p_bandwidth_n9);
        break;
      case 1:
        ret = sths34pf80_tmos_set_lpf_p_bandwidth(sths34pf80_tmos_lpf_p_bandwidth_n20);
        break;
      case 2:
        ret = sths34pf80_tmos_set_lpf_p_bandwidth(sths34pf80_tmos_lpf_p_bandwidth_n50);
        break;
      case 3:
        ret = sths34pf80_tmos_set_lpf_p_bandwidth(sths34pf80_tmos_lpf_p_bandwidth_n100);
        break;
      case 4:
        ret = sths34pf80_tmos_set_lpf_p_bandwidth(sths34pf80_tmos_lpf_p_bandwidth_n200);
        break;
      case 5:
        ret = sths34pf80_tmos_set_lpf_p_bandwidth(sths34pf80_tmos_lpf_p_bandwidth_n400);
        break;
      case 6:
        ret = sths34pf80_tmos_set_lpf_p_bandwidth(sths34pf80_tmos_lpf_p_bandwidth_n800);
        break;
    }
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.lpf_p_bandwidth.value", lpf_p_bandwidth);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.lpf_p_bandwidth.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.lpf_m_bandwidth"))
  {
    int lpf_m_bandwidth = (int)json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.lpf_m_bandwidth");
    switch (lpf_m_bandwidth)
    {
      case 0:
        ret = sths34pf80_tmos_set_lpf_m_bandwidth(sths34pf80_tmos_lpf_m_bandwidth_n9);
        break;
      case 1:
        ret = sths34pf80_tmos_set_lpf_m_bandwidth(sths34pf80_tmos_lpf_m_bandwidth_n20);
        break;
      case 2:
        ret = sths34pf80_tmos_set_lpf_m_bandwidth(sths34pf80_tmos_lpf_m_bandwidth_n50);
        break;
      case 3:
        ret = sths34pf80_tmos_set_lpf_m_bandwidth(sths34pf80_tmos_lpf_m_bandwidth_n100);
        break;
      case 4:
        ret = sths34pf80_tmos_set_lpf_m_bandwidth(sths34pf80_tmos_lpf_m_bandwidth_n200);
        break;
      case 5:
        ret = sths34pf80_tmos_set_lpf_m_bandwidth(sths34pf80_tmos_lpf_m_bandwidth_n400);
        break;
      case 6:
        ret = sths34pf80_tmos_set_lpf_m_bandwidth(sths34pf80_tmos_lpf_m_bandwidth_n800);
        break;
    }
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.lpf_m_bandwidth.value", lpf_m_bandwidth);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.lpf_m_bandwidth.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.presence_threshold"))
  {
    int32_t presence_threshold = (int32_t) json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.presence_threshold");
    ret = sths34pf80_tmos_set_presence_threshold(presence_threshold);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.presence_threshold.value", presence_threshold);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.presence_threshold.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.presence_hysteresis"))
  {
    int32_t presence_hysteresis = (int32_t) json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.presence_hysteresis");
    ret = sths34pf80_tmos_set_presence_hysteresis(presence_hysteresis);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.presence_hysteresis.value", presence_hysteresis);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.presence_hysteresis.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.motion_threshold"))
  {
    int32_t motion_threshold = (int32_t) json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.motion_threshold");
    ret = sths34pf80_tmos_set_motion_threshold(motion_threshold);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.motion_threshold.value", motion_threshold);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.motion_threshold.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.motion_hysteresis"))
  {
    int32_t motion_hysteresis = (int32_t) json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.motion_hysteresis");
    ret = sths34pf80_tmos_set_motion_hysteresis(motion_hysteresis);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.motion_hysteresis.value", motion_hysteresis);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.motion_hysteresis.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.tambient_shock_threshold"))
  {
    int32_t tambient_shock_threshold = (int32_t) json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.tambient_shock_threshold");
    ret = sths34pf80_tmos_set_tambient_shock_threshold(tambient_shock_threshold);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.tambient_shock_threshold.value", tambient_shock_threshold);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.tambient_shock_threshold.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.tambient_shock_hysteresis"))
  {
    int32_t tambient_shock_hysteresis = (int32_t) json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.tambient_shock_hysteresis");
    ret = sths34pf80_tmos_set_tambient_shock_hysteresis(tambient_shock_hysteresis);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.tambient_shock_hysteresis.value", tambient_shock_hysteresis);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.tambient_shock_hysteresis.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.embedded_compensation"))
  {
    bool embedded_compensation = json_object_dotget_boolean(tempJSONObject, "sths34pf80_tmos.embedded_compensation");
    ret = sths34pf80_tmos_set_embedded_compensation(embedded_compensation);
    if (ret == 0)
    {
      json_object_dotset_boolean(respJSONObject, "sths34pf80_tmos.embedded_compensation.value", embedded_compensation);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.embedded_compensation.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.software_compensation"))
  {
    bool software_compensation = json_object_dotget_boolean(tempJSONObject, "sths34pf80_tmos.software_compensation");
    ret = sths34pf80_tmos_set_software_compensation(software_compensation);
    if (ret == 0)
    {
      json_object_dotset_boolean(respJSONObject, "sths34pf80_tmos.software_compensation.value", software_compensation);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.software_compensation.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.compensation_type"))
  {
    int compensation_type = (int)json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.compensation_type");
    switch (compensation_type)
    {
      case 0:
        ret = sths34pf80_tmos_set_compensation_type(sths34pf80_tmos_compensation_type_ipd_comp_none);
        break;
      case 1:
        ret = sths34pf80_tmos_set_compensation_type(sths34pf80_tmos_compensation_type_ipd_comp_lin);
        break;
      case 2:
        ret = sths34pf80_tmos_set_compensation_type(sths34pf80_tmos_compensation_type_ipd_comp_nonlin);
        break;
    }
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.compensation_type.value", compensation_type);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.compensation_type.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.sw_presence_threshold"))
  {
    int32_t sw_presence_threshold = (int32_t) json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.sw_presence_threshold");
    ret = sths34pf80_tmos_set_sw_presence_threshold(sw_presence_threshold);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.sw_presence_threshold.value", sw_presence_threshold);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.sw_presence_threshold.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.sw_motion_threshold"))
  {
    int32_t sw_motion_threshold = (int32_t) json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.sw_motion_threshold");
    ret = sths34pf80_tmos_set_sw_motion_threshold(sw_motion_threshold);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.sw_motion_threshold.value", sw_motion_threshold);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.sw_motion_threshold.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.compensation_filter_flag"))
  {
    bool compensation_filter_flag = json_object_dotget_boolean(tempJSONObject, "sths34pf80_tmos.compensation_filter_flag");
    ret = sths34pf80_tmos_set_compensation_filter_flag(compensation_filter_flag);
    if (ret == 0)
    {
      json_object_dotset_boolean(respJSONObject, "sths34pf80_tmos.compensation_filter_flag.value", compensation_filter_flag);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.compensation_filter_flag.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.absence_static_flag"))
  {
    bool absence_static_flag = json_object_dotget_boolean(tempJSONObject, "sths34pf80_tmos.absence_static_flag");
    ret = sths34pf80_tmos_set_absence_static_flag(absence_static_flag);
    if (ret == 0)
    {
      json_object_dotset_boolean(respJSONObject, "sths34pf80_tmos.absence_static_flag.value", absence_static_flag);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.absence_static_flag.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.samples_per_ts"))
  {
    int32_t samples_per_ts = (int32_t) json_object_dotget_number(tempJSONObject, "sths34pf80_tmos.samples_per_ts");
    ret = sths34pf80_tmos_set_samples_per_ts(samples_per_ts);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "sths34pf80_tmos.samples_per_ts.value", samples_per_ts);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.samples_per_ts.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "sths34pf80_tmos.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "sths34pf80_tmos.sensor_annotation");
    ret = sths34pf80_tmos_set_sensor_annotation(sensor_annotation);
    if (ret == 0)
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.sensor_annotation.value", sensor_annotation);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "sths34pf80_tmos.sensor_annotation.value", "PNPL_SET_ERROR");
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


uint8_t Sths34pf80_Tmos_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                 uint32_t *size, uint8_t pretty)
{
  return 1;
}

