/**
  ******************************************************************************
  * @file    Lsm6dsv16x_Gyro_PnPL.c
  * @author  SRA
  * @brief   Lsm6dsv16x_Gyro PnPL Component Manager
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:lsm6dsv16x_gyro;2
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

#include "Lsm6dsv16x_Gyro_PnPL.h"
#include "Lsm6dsv16x_Gyro_PnPL_vtbl.h"

static const IPnPLComponent_vtbl sLsm6dsv16x_Gyro_PnPL_CompIF_vtbl =
{
  Lsm6dsv16x_Gyro_PnPL_vtblGetKey,
  Lsm6dsv16x_Gyro_PnPL_vtblGetNCommands,
  Lsm6dsv16x_Gyro_PnPL_vtblGetCommandKey,
  Lsm6dsv16x_Gyro_PnPL_vtblGetStatus,
  Lsm6dsv16x_Gyro_PnPL_vtblSetProperty,
  Lsm6dsv16x_Gyro_PnPL_vtblExecuteFunction
};

/**
  *  Lsm6dsv16x_Gyro_PnPL internal structure.
  */
struct _Lsm6dsv16x_Gyro_PnPL
{
  /**
    * Implements the IPnPLComponent interface.
    */
  IPnPLComponent_t component_if;

};

/* Objects instance ----------------------------------------------------------*/
static Lsm6dsv16x_Gyro_PnPL sLsm6dsv16x_Gyro_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Lsm6dsv16x_Gyro_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sLsm6dsv16x_Gyro_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sLsm6dsv16x_Gyro_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Lsm6dsv16x_Gyro_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  lsm6dsv16x_gyro_comp_init();
  return 0;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Lsm6dsv16x_Gyro_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return lsm6dsv16x_gyro_get_key();
}

uint8_t Lsm6dsv16x_Gyro_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Lsm6dsv16x_Gyro_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Lsm6dsv16x_Gyro_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  float temp_f = 0;
  lsm6dsv16x_gyro_get_odr(&temp_f);
  uint8_t enum_id = 0;
  if(temp_f == lsm6dsv16x_gyro_odr_hz7_5)
  {
    enum_id = 0;
  }
  else if(temp_f == lsm6dsv16x_gyro_odr_hz15)
  {
    enum_id = 1;
  }
  else if(temp_f == lsm6dsv16x_gyro_odr_hz30)
  {
    enum_id = 2;
  }
  else if(temp_f == lsm6dsv16x_gyro_odr_hz60)
  {
    enum_id = 3;
  }
  else if(temp_f == lsm6dsv16x_gyro_odr_hz120)
  {
    enum_id = 4;
  }
  else if(temp_f == lsm6dsv16x_gyro_odr_hz240)
  {
    enum_id = 5;
  }
  else if(temp_f == lsm6dsv16x_gyro_odr_hz480)
  {
    enum_id = 6;
  }
  else if(temp_f == lsm6dsv16x_gyro_odr_hz960)
  {
    enum_id = 7;
  }
  else if(temp_f == lsm6dsv16x_gyro_odr_hz1920)
  {
    enum_id = 8;
  }
  else if(temp_f == lsm6dsv16x_gyro_odr_hz3840)
  {
    enum_id = 9;
  }
  else if(temp_f == lsm6dsv16x_gyro_odr_hz7680)
  {
    enum_id = 10;
  }
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_gyro.odr", enum_id);
  lsm6dsv16x_gyro_get_fs(&temp_f);
  enum_id = 0;
  if(temp_f == lsm6dsv16x_gyro_fs_dps125)
  {
    enum_id = 0;
  }
  else if(temp_f == lsm6dsv16x_gyro_fs_dps250)
  {
    enum_id = 1;
  }
  else if(temp_f == lsm6dsv16x_gyro_fs_dps500)
  {
    enum_id = 2;
  }
  else if(temp_f == lsm6dsv16x_gyro_fs_dps1000)
  {
    enum_id = 3;
  }
  else if(temp_f == lsm6dsv16x_gyro_fs_dps2000)
  {
    enum_id = 4;
  }
  else if(temp_f == lsm6dsv16x_gyro_fs_dps4000)
  {
    enum_id = 5;
  }
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_gyro.fs", enum_id);
  bool temp_b = 0;
  lsm6dsv16x_gyro_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "lsm6dsv16x_gyro.enable", temp_b);
  int32_t temp_i = 0;
  lsm6dsv16x_gyro_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_gyro.samples_per_ts", temp_i);
  lsm6dsv16x_gyro_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_gyro.dim", temp_i);
  lsm6dsv16x_gyro_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_gyro.ioffset", temp_f);
  lsm6dsv16x_gyro_get_measodr(&temp_f);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_gyro.measodr", temp_f);
  lsm6dsv16x_gyro_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_gyro.usb_dps", temp_i);
  lsm6dsv16x_gyro_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_gyro.sd_dps", temp_i);
  lsm6dsv16x_gyro_get_sensitivity(&temp_f);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_gyro.sensitivity", temp_f);
  char *temp_s = "";
  lsm6dsv16x_gyro_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "lsm6dsv16x_gyro.data_type", temp_s);
  lsm6dsv16x_gyro_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_gyro.sensor_category", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_gyro.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  lsm6dsv16x_gyro_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_gyro.stream_id", temp_int8);
  lsm6dsv16x_gyro_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_gyro.ep_id", temp_int8);

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

uint8_t Lsm6dsv16x_Gyro_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);

  uint8_t ret = 0;
  if(json_object_dothas_value(tempJSONObject, "lsm6dsv16x_gyro.odr"))
  {
    int odr = (int)json_object_dotget_number(tempJSONObject, "lsm6dsv16x_gyro.odr");
    switch(odr)
    {
    case 0:
      lsm6dsv16x_gyro_set_odr(lsm6dsv16x_gyro_odr_hz7_5);
      break;
    case 1:
      lsm6dsv16x_gyro_set_odr(lsm6dsv16x_gyro_odr_hz15);
      break;
    case 2:
      lsm6dsv16x_gyro_set_odr(lsm6dsv16x_gyro_odr_hz30);
      break;
    case 3:
      lsm6dsv16x_gyro_set_odr(lsm6dsv16x_gyro_odr_hz60);
      break;
    case 4:
      lsm6dsv16x_gyro_set_odr(lsm6dsv16x_gyro_odr_hz120);
      break;
    case 5:
      lsm6dsv16x_gyro_set_odr(lsm6dsv16x_gyro_odr_hz240);
      break;
    case 6:
      lsm6dsv16x_gyro_set_odr(lsm6dsv16x_gyro_odr_hz480);
      break;
    case 7:
      lsm6dsv16x_gyro_set_odr(lsm6dsv16x_gyro_odr_hz960);
      break;
    case 8:
      lsm6dsv16x_gyro_set_odr(lsm6dsv16x_gyro_odr_hz1920);
      break;
    case 9:
      lsm6dsv16x_gyro_set_odr(lsm6dsv16x_gyro_odr_hz3840);
      break;
    case 10:
      lsm6dsv16x_gyro_set_odr(lsm6dsv16x_gyro_odr_hz7680);
      break;
    }
  }
  if(json_object_dothas_value(tempJSONObject, "lsm6dsv16x_gyro.fs"))
  {
    int fs = (int)json_object_dotget_number(tempJSONObject, "lsm6dsv16x_gyro.fs");
    switch(fs)
    {
    case 0:
      lsm6dsv16x_gyro_set_fs(lsm6dsv16x_gyro_fs_dps125);
      break;
    case 1:
      lsm6dsv16x_gyro_set_fs(lsm6dsv16x_gyro_fs_dps250);
      break;
    case 2:
      lsm6dsv16x_gyro_set_fs(lsm6dsv16x_gyro_fs_dps500);
      break;
    case 3:
      lsm6dsv16x_gyro_set_fs(lsm6dsv16x_gyro_fs_dps1000);
      break;
    case 4:
      lsm6dsv16x_gyro_set_fs(lsm6dsv16x_gyro_fs_dps2000);
      break;
    case 5:
      lsm6dsv16x_gyro_set_fs(lsm6dsv16x_gyro_fs_dps4000);
      break;
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lsm6dsv16x_gyro.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "lsm6dsv16x_gyro.enable");
    lsm6dsv16x_gyro_set_enable(enable);
  }
  if (json_object_dothas_value(tempJSONObject, "lsm6dsv16x_gyro.samples_per_ts"))
  {
    int32_t samples_per_ts =(int32_t) json_object_dotget_number(tempJSONObject, "lsm6dsv16x_gyro.samples_per_ts");
    lsm6dsv16x_gyro_set_samples_per_ts(samples_per_ts);
  }
  json_value_free(tempJSON);
  return ret;
}

uint8_t Lsm6dsv16x_Gyro_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON)
{
  return 1;
}
