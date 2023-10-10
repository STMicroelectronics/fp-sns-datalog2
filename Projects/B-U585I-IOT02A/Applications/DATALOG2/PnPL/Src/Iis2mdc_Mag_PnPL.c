/**
  ******************************************************************************
  * @file    Iis2mdc_Mag_PnPL.c
  * @author  SRA
  * @brief   Iis2mdc_Mag PnPL Component Manager
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
  * dtmi:vespucci:b_u585i_iot02a:fpSnsDatalog2_datalog2:sensors:iis2mdc_mag;1
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

#include "Iis2mdc_Mag_PnPL.h"
#include "Iis2mdc_Mag_PnPL_vtbl.h"

static const IPnPLComponent_vtbl sIis2mdc_Mag_PnPL_CompIF_vtbl =
{
  Iis2mdc_Mag_PnPL_vtblGetKey,
  Iis2mdc_Mag_PnPL_vtblGetNCommands,
  Iis2mdc_Mag_PnPL_vtblGetCommandKey,
  Iis2mdc_Mag_PnPL_vtblGetStatus,
  Iis2mdc_Mag_PnPL_vtblSetProperty,
  Iis2mdc_Mag_PnPL_vtblExecuteFunction
};

/**
  *  Iis2mdc_Mag_PnPL internal structure.
  */
struct _Iis2mdc_Mag_PnPL
{
  /**
    * Implements the IPnPLComponent interface.
    */
  IPnPLComponent_t component_if;

};

/* Objects instance ----------------------------------------------------------*/
static Iis2mdc_Mag_PnPL sIis2mdc_Mag_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Iis2mdc_Mag_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sIis2mdc_Mag_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sIis2mdc_Mag_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Iis2mdc_Mag_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  iis2mdc_mag_comp_init();
  return 0;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Iis2mdc_Mag_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return iis2mdc_mag_get_key();
}

uint8_t Iis2mdc_Mag_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Iis2mdc_Mag_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Iis2mdc_Mag_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  float temp_f = 0;
  iis2mdc_mag_get_odr(&temp_f);
  uint8_t enum_id = 0;
  if (temp_f == iis2mdc_mag_odr_hz10)
  {
    enum_id = 0;
  }
  else if (temp_f == iis2mdc_mag_odr_hz20)
  {
    enum_id = 1;
  }
  else if (temp_f == iis2mdc_mag_odr_hz50)
  {
    enum_id = 2;
  }
  else if (temp_f == iis2mdc_mag_odr_hz100)
  {
    enum_id = 3;
  }
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.odr", enum_id);
  iis2mdc_mag_get_fs(&temp_f);
  enum_id = 0;
  if (temp_f == iis2mdc_mag_fs_g50)
  {
    enum_id = 0;
  }
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.fs", enum_id);
  bool temp_b = 0;
  iis2mdc_mag_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "iis2mdc_mag.enable", temp_b);
  int32_t temp_i = 0;
  iis2mdc_mag_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.samples_per_ts", temp_i);
  iis2mdc_mag_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.dim", temp_i);
  iis2mdc_mag_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.ioffset", temp_f);
  iis2mdc_mag_get_measodr(&temp_f);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.measodr", temp_f);
  iis2mdc_mag_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.usb_dps", temp_i);
  iis2mdc_mag_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.sd_dps", temp_i);
  iis2mdc_mag_get_sensitivity(&temp_f);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.sensitivity", temp_f);
  char *temp_s = "";
  iis2mdc_mag_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "iis2mdc_mag.data_type", temp_s);
  iis2mdc_mag_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "iis2mdc_mag.sensor_annotation", temp_s);
  iis2mdc_mag_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.sensor_category", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  iis2mdc_mag_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.stream_id", temp_int8);
  iis2mdc_mag_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "iis2mdc_mag.ep_id", temp_int8);

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

uint8_t Iis2mdc_Mag_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);

  uint8_t ret = 0;
  if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.odr"))
  {
    int odr = (int)json_object_dotget_number(tempJSONObject, "iis2mdc_mag.odr");
    switch (odr)
    {
      case 0:
        iis2mdc_mag_set_odr(iis2mdc_mag_odr_hz10);
        break;
      case 1:
        iis2mdc_mag_set_odr(iis2mdc_mag_odr_hz20);
        break;
      case 2:
        iis2mdc_mag_set_odr(iis2mdc_mag_odr_hz50);
        break;
      case 3:
        iis2mdc_mag_set_odr(iis2mdc_mag_odr_hz100);
        break;
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "iis2mdc_mag.enable");
    iis2mdc_mag_set_enable(enable);
  }
  if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.samples_per_ts"))
  {
    int32_t samples_per_ts = (int32_t) json_object_dotget_number(tempJSONObject, "iis2mdc_mag.samples_per_ts");
    iis2mdc_mag_set_samples_per_ts(samples_per_ts);
  }
  if (json_object_dothas_value(tempJSONObject, "iis2mdc_mag.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "iis2mdc_mag.sensor_annotation");
    iis2mdc_mag_set_sensor_annotation(sensor_annotation);
  }
  json_value_free(tempJSON);
  return ret;
}

uint8_t Iis2mdc_Mag_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON)
{
  return 1;
}
