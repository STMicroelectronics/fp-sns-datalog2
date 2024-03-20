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
  * Created by: DTDL2PnPL_cGen version 1.2.1
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
#include "Vd6283tx_Als_PnPL_vtbl.h"

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
  return 0;
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
  vd6283tx_als_get_intermeasurement_time(&temp_i);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.intermeasurement_time", temp_i);
  vd6283tx_als_get_exposure_time(&temp_i);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.exposure_time", temp_i);
  float temp_f = 0;
  vd6283tx_als_get_channel1_gain(&temp_f);
  uint8_t enum_id = 0;
  if(temp_f < vd6283tx_als_channel1_gain_n0_71 + pnpl_float_tolerance)
  {
    enum_id = 0;
  }
  else if(temp_f < vd6283tx_als_channel1_gain_n0_83 + pnpl_float_tolerance)
  {
    enum_id = 1;
  }
  else if(temp_f < vd6283tx_als_channel1_gain_n1 + pnpl_float_tolerance)
  {
    enum_id = 2;
  }
  else if(temp_f < vd6283tx_als_channel1_gain_n1_25 + pnpl_float_tolerance)
  {
    enum_id = 3;
  }
  else if(temp_f < vd6283tx_als_channel1_gain_n1_67 + pnpl_float_tolerance)
  {
    enum_id = 4;
  }
  else if(temp_f < vd6283tx_als_channel1_gain_n2_5 + pnpl_float_tolerance)
  {
    enum_id = 5;
  }
  else if(temp_f < vd6283tx_als_channel1_gain_n3_33 + pnpl_float_tolerance)
  {
    enum_id = 6;
  }
  else if(temp_f < vd6283tx_als_channel1_gain_n5 + pnpl_float_tolerance)
  {
    enum_id = 7;
  }
  else if(temp_f < vd6283tx_als_channel1_gain_n7_1 + pnpl_float_tolerance)
  {
    enum_id = 8;
  }
  else if(temp_f < vd6283tx_als_channel1_gain_n10 + pnpl_float_tolerance)
  {
    enum_id = 9;
  }
  else if(temp_f < vd6283tx_als_channel1_gain_n16 + pnpl_float_tolerance)
  {
    enum_id = 10;
  }
  else if(temp_f < vd6283tx_als_channel1_gain_n25 + pnpl_float_tolerance)
  {
    enum_id = 11;
  }
  else if(temp_f < vd6283tx_als_channel1_gain_n33 + pnpl_float_tolerance)
  {
    enum_id = 12;
  }
  else if(temp_f < vd6283tx_als_channel1_gain_n50 + pnpl_float_tolerance)
  {
    enum_id = 13;
  }
  else //if(temp_f < vd6283tx_als_channel1_gain_n66_6 + 0.05)
  {
    enum_id = 14;
  }
  json_object_dotset_number(JSON_Status, "vd6283tx_als.channel1_gain", enum_id);
  vd6283tx_als_get_channel2_gain(&temp_f);
  enum_id = 0;
  if(temp_f < vd6283tx_als_channel2_gain_n0_71 + pnpl_float_tolerance)
  {
    enum_id = 0;
  }
  else if(temp_f < vd6283tx_als_channel2_gain_n0_83 + pnpl_float_tolerance)
  {
    enum_id = 1;
  }
  else if(temp_f < vd6283tx_als_channel2_gain_n1 + pnpl_float_tolerance)
  {
    enum_id = 2;
  }
  else if(temp_f < vd6283tx_als_channel2_gain_n1_25 + pnpl_float_tolerance)
  {
    enum_id = 3;
  }
  else if(temp_f < vd6283tx_als_channel2_gain_n1_67 + pnpl_float_tolerance)
  {
    enum_id = 4;
  }
  else if(temp_f < vd6283tx_als_channel2_gain_n2_5 + pnpl_float_tolerance)
  {
    enum_id = 5;
  }
  else if(temp_f < vd6283tx_als_channel2_gain_n3_33 + pnpl_float_tolerance)
  {
    enum_id = 6;
  }
  else if(temp_f < vd6283tx_als_channel2_gain_n5 + pnpl_float_tolerance)
  {
    enum_id = 7;
  }
  else if(temp_f < vd6283tx_als_channel2_gain_n7_1 + pnpl_float_tolerance)
  {
    enum_id = 8;
  }
  else if(temp_f < vd6283tx_als_channel2_gain_n10 + pnpl_float_tolerance)
  {
    enum_id = 9;
  }
  else if(temp_f < vd6283tx_als_channel2_gain_n16 + pnpl_float_tolerance)
  {
    enum_id = 10;
  }
  else if(temp_f < vd6283tx_als_channel2_gain_n25 + pnpl_float_tolerance)
  {
    enum_id = 11;
  }
  else if(temp_f < vd6283tx_als_channel2_gain_n33 + pnpl_float_tolerance)
  {
    enum_id = 12;
  }
  else if(temp_f < vd6283tx_als_channel2_gain_n50 + pnpl_float_tolerance)
  {
    enum_id = 13;
  }
  else //if(temp_f < vd6283tx_als_channel2_gain_n66_6 + 0.05)
  {
    enum_id = 14;
  }
  json_object_dotset_number(JSON_Status, "vd6283tx_als.channel2_gain", enum_id);
  vd6283tx_als_get_channel3_gain(&temp_f);
  enum_id = 0;
  if(temp_f < vd6283tx_als_channel3_gain_n0_71 + pnpl_float_tolerance)
  {
    enum_id = 0;
  }
  else if(temp_f < vd6283tx_als_channel3_gain_n0_83 + pnpl_float_tolerance)
  {
    enum_id = 1;
  }
  else if(temp_f < vd6283tx_als_channel3_gain_n1 + pnpl_float_tolerance)
  {
    enum_id = 2;
  }
  else if(temp_f < vd6283tx_als_channel3_gain_n1_25 + pnpl_float_tolerance)
  {
    enum_id = 3;
  }
  else if(temp_f < vd6283tx_als_channel3_gain_n1_67 + pnpl_float_tolerance)
  {
    enum_id = 4;
  }
  else if(temp_f < vd6283tx_als_channel3_gain_n2_5 + pnpl_float_tolerance)
  {
    enum_id = 5;
  }
  else if(temp_f < vd6283tx_als_channel3_gain_n3_33 + pnpl_float_tolerance)
  {
    enum_id = 6;
  }
  else if(temp_f < vd6283tx_als_channel3_gain_n5 + pnpl_float_tolerance)
  {
    enum_id = 7;
  }
  else if(temp_f < vd6283tx_als_channel3_gain_n7_1 + pnpl_float_tolerance)
  {
    enum_id = 8;
  }
  else if(temp_f < vd6283tx_als_channel3_gain_n10 + pnpl_float_tolerance)
  {
    enum_id = 9;
  }
  else if(temp_f < vd6283tx_als_channel3_gain_n16 + pnpl_float_tolerance)
  {
    enum_id = 10;
  }
  else if(temp_f < vd6283tx_als_channel3_gain_n25 + pnpl_float_tolerance)
  {
    enum_id = 11;
  }
  else if(temp_f < vd6283tx_als_channel3_gain_n33 + pnpl_float_tolerance)
  {
    enum_id = 12;
  }
  else if(temp_f < vd6283tx_als_channel3_gain_n50 + pnpl_float_tolerance)
  {
    enum_id = 13;
  }
  else //if(temp_f < vd6283tx_als_channel3_gain_n66_6 + 0.05)
  {
    enum_id = 14;
  }
  json_object_dotset_number(JSON_Status, "vd6283tx_als.channel3_gain", enum_id);
  vd6283tx_als_get_channel4_gain(&temp_f);
  enum_id = 0;
  if(temp_f < vd6283tx_als_channel4_gain_n0_71 + pnpl_float_tolerance)
  {
    enum_id = 0;
  }
  else if(temp_f < vd6283tx_als_channel4_gain_n0_83 + pnpl_float_tolerance)
  {
    enum_id = 1;
  }
  else if(temp_f < vd6283tx_als_channel4_gain_n1 + pnpl_float_tolerance)
  {
    enum_id = 2;
  }
  else if(temp_f < vd6283tx_als_channel4_gain_n1_25 + pnpl_float_tolerance)
  {
    enum_id = 3;
  }
  else if(temp_f < vd6283tx_als_channel4_gain_n1_67 + pnpl_float_tolerance)
  {
    enum_id = 4;
  }
  else if(temp_f < vd6283tx_als_channel4_gain_n2_5 + pnpl_float_tolerance)
  {
    enum_id = 5;
  }
  else if(temp_f < vd6283tx_als_channel4_gain_n3_33 + pnpl_float_tolerance)
  {
    enum_id = 6;
  }
  else if(temp_f < vd6283tx_als_channel4_gain_n5 + pnpl_float_tolerance)
  {
    enum_id = 7;
  }
  else if(temp_f < vd6283tx_als_channel4_gain_n7_1 + pnpl_float_tolerance)
  {
    enum_id = 8;
  }
  else if(temp_f < vd6283tx_als_channel4_gain_n10 + pnpl_float_tolerance)
  {
    enum_id = 9;
  }
  else if(temp_f < vd6283tx_als_channel4_gain_n16 + pnpl_float_tolerance)
  {
    enum_id = 10;
  }
  else if(temp_f < vd6283tx_als_channel4_gain_n25 + pnpl_float_tolerance)
  {
    enum_id = 11;
  }
  else if(temp_f < vd6283tx_als_channel4_gain_n33 + pnpl_float_tolerance)
  {
    enum_id = 12;
  }
  else if(temp_f < vd6283tx_als_channel4_gain_n50 + pnpl_float_tolerance)
  {
    enum_id = 13;
  }
  else //if(temp_f < vd6283tx_als_channel4_gain_n66_6 + 0.05)
  {
    enum_id = 14;
  }
  json_object_dotset_number(JSON_Status, "vd6283tx_als.channel4_gain", enum_id);
  vd6283tx_als_get_channel5_gain(&temp_f);
  enum_id = 0;
  if(temp_f < vd6283tx_als_channel5_gain_n0_71 + pnpl_float_tolerance)
  {
    enum_id = 0;
  }
  else if(temp_f < vd6283tx_als_channel5_gain_n0_83 + pnpl_float_tolerance)
  {
    enum_id = 1;
  }
  else if(temp_f < vd6283tx_als_channel5_gain_n1 + pnpl_float_tolerance)
  {
    enum_id = 2;
  }
  else if(temp_f < vd6283tx_als_channel5_gain_n1_25 + pnpl_float_tolerance)
  {
    enum_id = 3;
  }
  else if(temp_f < vd6283tx_als_channel5_gain_n1_67 + pnpl_float_tolerance)
  {
    enum_id = 4;
  }
  else if(temp_f < vd6283tx_als_channel5_gain_n2_5 + pnpl_float_tolerance)
  {
    enum_id = 5;
  }
  else if(temp_f < vd6283tx_als_channel5_gain_n3_33 + pnpl_float_tolerance)
  {
    enum_id = 6;
  }
  else if(temp_f < vd6283tx_als_channel5_gain_n5 + pnpl_float_tolerance)
  {
    enum_id = 7;
  }
  else if(temp_f < vd6283tx_als_channel5_gain_n7_1 + pnpl_float_tolerance)
  {
    enum_id = 8;
  }
  else if(temp_f < vd6283tx_als_channel5_gain_n10 + pnpl_float_tolerance)
  {
    enum_id = 9;
  }
  else if(temp_f < vd6283tx_als_channel5_gain_n16 + pnpl_float_tolerance)
  {
    enum_id = 10;
  }
  else if(temp_f < vd6283tx_als_channel5_gain_n25 + pnpl_float_tolerance)
  {
    enum_id = 11;
  }
  else if(temp_f < vd6283tx_als_channel5_gain_n33 + pnpl_float_tolerance)
  {
    enum_id = 12;
  }
  else if(temp_f < vd6283tx_als_channel5_gain_n50 + pnpl_float_tolerance)
  {
    enum_id = 13;
  }
  else //if(temp_f < vd6283tx_als_channel5_gain_n66_6 + 0.05)
  {
    enum_id = 14;
  }
  json_object_dotset_number(JSON_Status, "vd6283tx_als.channel5_gain", enum_id);
  vd6283tx_als_get_channel6_gain(&temp_f);
  enum_id = 0;
  if(temp_f < vd6283tx_als_channel6_gain_n0_71 + pnpl_float_tolerance)
  {
    enum_id = 0;
  }
  else if(temp_f < vd6283tx_als_channel6_gain_n0_83 + pnpl_float_tolerance)
  {
    enum_id = 1;
  }
  else if(temp_f < vd6283tx_als_channel6_gain_n1 + pnpl_float_tolerance)
  {
    enum_id = 2;
  }
  else if(temp_f < vd6283tx_als_channel6_gain_n1_25 + pnpl_float_tolerance)
  {
    enum_id = 3;
  }
  else if(temp_f < vd6283tx_als_channel6_gain_n1_67 + pnpl_float_tolerance)
  {
    enum_id = 4;
  }
  else if(temp_f < vd6283tx_als_channel6_gain_n2_5 + pnpl_float_tolerance)
  {
    enum_id = 5;
  }
  else if(temp_f < vd6283tx_als_channel6_gain_n3_33 + pnpl_float_tolerance)
  {
    enum_id = 6;
  }
  else if(temp_f < vd6283tx_als_channel6_gain_n5 + pnpl_float_tolerance)
  {
    enum_id = 7;
  }
  else if(temp_f < vd6283tx_als_channel6_gain_n7_1 + pnpl_float_tolerance)
  {
    enum_id = 8;
  }
  else if(temp_f < vd6283tx_als_channel6_gain_n10 + pnpl_float_tolerance)
  {
    enum_id = 9;
  }
  else if(temp_f < vd6283tx_als_channel6_gain_n16 + pnpl_float_tolerance)
  {
    enum_id = 10;
  }
  else if(temp_f < vd6283tx_als_channel6_gain_n25 + pnpl_float_tolerance)
  {
    enum_id = 11;
  }
  else if(temp_f < vd6283tx_als_channel6_gain_n33 + pnpl_float_tolerance)
  {
    enum_id = 12;
  }
  else if(temp_f < vd6283tx_als_channel6_gain_n50 + pnpl_float_tolerance)
  {
    enum_id = 13;
  }
  else //if(temp_f < vd6283tx_als_channel6_gain_n66_6 + 0.05)
  {
    enum_id = 14;
  }
  json_object_dotset_number(JSON_Status, "vd6283tx_als.channel6_gain", enum_id);
  vd6283tx_als_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "vd6283tx_als.samples_per_ts", temp_i);
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

  return 0;
}

uint8_t Vd6283tx_Als_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                          uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = 0;
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "vd6283tx_als.enable");
    ret = vd6283tx_als_set_enable(enable);
    if (ret == 0)
    {
      json_object_dotset_boolean(respJSONObject, "vd6283tx_als.enable.value", enable);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vd6283tx_als.enable.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.intermeasurement_time"))
  {
    int intermeasurement_time = (int) json_object_dotget_number(tempJSONObject, "vd6283tx_als.intermeasurement_time");
    ret = vd6283tx_als_set_intermeasurement_time(intermeasurement_time);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "vd6283tx_als.intermeasurement_time.value", intermeasurement_time);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vd6283tx_als.intermeasurement_time.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.exposure_time"))
  {
    int exposure_time = (int) json_object_dotget_number(tempJSONObject, "vd6283tx_als.exposure_time");
    ret = vd6283tx_als_set_exposure_time(exposure_time);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "vd6283tx_als.exposure_time.value", exposure_time);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vd6283tx_als.exposure_time.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.channel1_gain"))
  {
    int channel1_gain = (int)json_object_dotget_number(tempJSONObject, "vd6283tx_als.channel1_gain");
    switch (channel1_gain)
    {
      case 0:
        ret = vd6283tx_als_set_channel1_gain(vd6283tx_als_channel1_gain_n0_71);
        break;
      case 1:
        ret = vd6283tx_als_set_channel1_gain(vd6283tx_als_channel1_gain_n0_83);
        break;
      case 2:
        ret = vd6283tx_als_set_channel1_gain(vd6283tx_als_channel1_gain_n1);
        break;
      case 3:
        ret = vd6283tx_als_set_channel1_gain(vd6283tx_als_channel1_gain_n1_25);
        break;
      case 4:
        ret = vd6283tx_als_set_channel1_gain(vd6283tx_als_channel1_gain_n1_67);
        break;
      case 5:
        ret = vd6283tx_als_set_channel1_gain(vd6283tx_als_channel1_gain_n2_5);
        break;
      case 6:
        ret = vd6283tx_als_set_channel1_gain(vd6283tx_als_channel1_gain_n3_33);
        break;
      case 7:
        ret = vd6283tx_als_set_channel1_gain(vd6283tx_als_channel1_gain_n5);
        break;
      case 8:
        ret = vd6283tx_als_set_channel1_gain(vd6283tx_als_channel1_gain_n7_1);
        break;
      case 9:
        ret = vd6283tx_als_set_channel1_gain(vd6283tx_als_channel1_gain_n10);
        break;
      case 10:
        ret = vd6283tx_als_set_channel1_gain(vd6283tx_als_channel1_gain_n16);
        break;
      case 11:
        ret = vd6283tx_als_set_channel1_gain(vd6283tx_als_channel1_gain_n25);
        break;
      case 12:
        ret = vd6283tx_als_set_channel1_gain(vd6283tx_als_channel1_gain_n33);
        break;
      case 13:
        ret = vd6283tx_als_set_channel1_gain(vd6283tx_als_channel1_gain_n50);
        break;
      case 14:
        ret = vd6283tx_als_set_channel1_gain(vd6283tx_als_channel1_gain_n66_6);
        break;
    }
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "vd6283tx_als.channel1_gain.value", channel1_gain);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vd6283tx_als.channel1_gain.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.channel2_gain"))
  {
    int channel2_gain = (int)json_object_dotget_number(tempJSONObject, "vd6283tx_als.channel2_gain");
    switch (channel2_gain)
    {
      case 0:
        ret = vd6283tx_als_set_channel2_gain(vd6283tx_als_channel2_gain_n0_71);
        break;
      case 1:
        ret = vd6283tx_als_set_channel2_gain(vd6283tx_als_channel2_gain_n0_83);
        break;
      case 2:
        ret = vd6283tx_als_set_channel2_gain(vd6283tx_als_channel2_gain_n1);
        break;
      case 3:
        ret = vd6283tx_als_set_channel2_gain(vd6283tx_als_channel2_gain_n1_25);
        break;
      case 4:
        ret = vd6283tx_als_set_channel2_gain(vd6283tx_als_channel2_gain_n1_67);
        break;
      case 5:
        ret = vd6283tx_als_set_channel2_gain(vd6283tx_als_channel2_gain_n2_5);
        break;
      case 6:
        ret = vd6283tx_als_set_channel2_gain(vd6283tx_als_channel2_gain_n3_33);
        break;
      case 7:
        ret = vd6283tx_als_set_channel2_gain(vd6283tx_als_channel2_gain_n5);
        break;
      case 8:
        ret = vd6283tx_als_set_channel2_gain(vd6283tx_als_channel2_gain_n7_1);
        break;
      case 9:
        ret = vd6283tx_als_set_channel2_gain(vd6283tx_als_channel2_gain_n10);
        break;
      case 10:
        ret = vd6283tx_als_set_channel2_gain(vd6283tx_als_channel2_gain_n16);
        break;
      case 11:
        ret = vd6283tx_als_set_channel2_gain(vd6283tx_als_channel2_gain_n25);
        break;
      case 12:
        ret = vd6283tx_als_set_channel2_gain(vd6283tx_als_channel2_gain_n33);
        break;
      case 13:
        ret = vd6283tx_als_set_channel2_gain(vd6283tx_als_channel2_gain_n50);
        break;
      case 14:
        ret = vd6283tx_als_set_channel2_gain(vd6283tx_als_channel2_gain_n66_6);
        break;
    }
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "vd6283tx_als.channel2_gain.value", channel2_gain);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vd6283tx_als.channel2_gain.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.channel3_gain"))
  {
    int channel3_gain = (int)json_object_dotget_number(tempJSONObject, "vd6283tx_als.channel3_gain");
    switch (channel3_gain)
    {
      case 0:
        ret = vd6283tx_als_set_channel3_gain(vd6283tx_als_channel3_gain_n0_71);
        break;
      case 1:
        ret = vd6283tx_als_set_channel3_gain(vd6283tx_als_channel3_gain_n0_83);
        break;
      case 2:
        ret = vd6283tx_als_set_channel3_gain(vd6283tx_als_channel3_gain_n1);
        break;
      case 3:
        ret = vd6283tx_als_set_channel3_gain(vd6283tx_als_channel3_gain_n1_25);
        break;
      case 4:
        ret = vd6283tx_als_set_channel3_gain(vd6283tx_als_channel3_gain_n1_67);
        break;
      case 5:
        ret = vd6283tx_als_set_channel3_gain(vd6283tx_als_channel3_gain_n2_5);
        break;
      case 6:
        ret = vd6283tx_als_set_channel3_gain(vd6283tx_als_channel3_gain_n3_33);
        break;
      case 7:
        ret = vd6283tx_als_set_channel3_gain(vd6283tx_als_channel3_gain_n5);
        break;
      case 8:
        ret = vd6283tx_als_set_channel3_gain(vd6283tx_als_channel3_gain_n7_1);
        break;
      case 9:
        ret = vd6283tx_als_set_channel3_gain(vd6283tx_als_channel3_gain_n10);
        break;
      case 10:
        ret = vd6283tx_als_set_channel3_gain(vd6283tx_als_channel3_gain_n16);
        break;
      case 11:
        ret = vd6283tx_als_set_channel3_gain(vd6283tx_als_channel3_gain_n25);
        break;
      case 12:
        ret = vd6283tx_als_set_channel3_gain(vd6283tx_als_channel3_gain_n33);
        break;
      case 13:
        ret = vd6283tx_als_set_channel3_gain(vd6283tx_als_channel3_gain_n50);
        break;
      case 14:
        ret = vd6283tx_als_set_channel3_gain(vd6283tx_als_channel3_gain_n66_6);
        break;
    }
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "vd6283tx_als.channel3_gain.value", channel3_gain);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vd6283tx_als.channel3_gain.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.channel4_gain"))
  {
    int channel4_gain = (int)json_object_dotget_number(tempJSONObject, "vd6283tx_als.channel4_gain");
    switch (channel4_gain)
    {
      case 0:
        ret = vd6283tx_als_set_channel4_gain(vd6283tx_als_channel4_gain_n0_71);
        break;
      case 1:
        ret = vd6283tx_als_set_channel4_gain(vd6283tx_als_channel4_gain_n0_83);
        break;
      case 2:
        ret = vd6283tx_als_set_channel4_gain(vd6283tx_als_channel4_gain_n1);
        break;
      case 3:
        ret = vd6283tx_als_set_channel4_gain(vd6283tx_als_channel4_gain_n1_25);
        break;
      case 4:
        ret = vd6283tx_als_set_channel4_gain(vd6283tx_als_channel4_gain_n1_67);
        break;
      case 5:
        ret = vd6283tx_als_set_channel4_gain(vd6283tx_als_channel4_gain_n2_5);
        break;
      case 6:
        ret = vd6283tx_als_set_channel4_gain(vd6283tx_als_channel4_gain_n3_33);
        break;
      case 7:
        ret = vd6283tx_als_set_channel4_gain(vd6283tx_als_channel4_gain_n5);
        break;
      case 8:
        ret = vd6283tx_als_set_channel4_gain(vd6283tx_als_channel4_gain_n7_1);
        break;
      case 9:
        ret = vd6283tx_als_set_channel4_gain(vd6283tx_als_channel4_gain_n10);
        break;
      case 10:
        ret = vd6283tx_als_set_channel4_gain(vd6283tx_als_channel4_gain_n16);
        break;
      case 11:
        ret = vd6283tx_als_set_channel4_gain(vd6283tx_als_channel4_gain_n25);
        break;
      case 12:
        ret = vd6283tx_als_set_channel4_gain(vd6283tx_als_channel4_gain_n33);
        break;
      case 13:
        ret = vd6283tx_als_set_channel4_gain(vd6283tx_als_channel4_gain_n50);
        break;
      case 14:
        ret = vd6283tx_als_set_channel4_gain(vd6283tx_als_channel4_gain_n66_6);
        break;
    }
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "vd6283tx_als.channel4_gain.value", channel4_gain);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vd6283tx_als.channel4_gain.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.channel5_gain"))
  {
    int channel5_gain = (int)json_object_dotget_number(tempJSONObject, "vd6283tx_als.channel5_gain");
    switch (channel5_gain)
    {
      case 0:
        ret = vd6283tx_als_set_channel5_gain(vd6283tx_als_channel5_gain_n0_71);
        break;
      case 1:
        ret = vd6283tx_als_set_channel5_gain(vd6283tx_als_channel5_gain_n0_83);
        break;
      case 2:
        ret = vd6283tx_als_set_channel5_gain(vd6283tx_als_channel5_gain_n1);
        break;
      case 3:
        ret = vd6283tx_als_set_channel5_gain(vd6283tx_als_channel5_gain_n1_25);
        break;
      case 4:
        ret = vd6283tx_als_set_channel5_gain(vd6283tx_als_channel5_gain_n1_67);
        break;
      case 5:
        ret = vd6283tx_als_set_channel5_gain(vd6283tx_als_channel5_gain_n2_5);
        break;
      case 6:
        ret = vd6283tx_als_set_channel5_gain(vd6283tx_als_channel5_gain_n3_33);
        break;
      case 7:
        ret = vd6283tx_als_set_channel5_gain(vd6283tx_als_channel5_gain_n5);
        break;
      case 8:
        ret = vd6283tx_als_set_channel5_gain(vd6283tx_als_channel5_gain_n7_1);
        break;
      case 9:
        ret = vd6283tx_als_set_channel5_gain(vd6283tx_als_channel5_gain_n10);
        break;
      case 10:
        ret = vd6283tx_als_set_channel5_gain(vd6283tx_als_channel5_gain_n16);
        break;
      case 11:
        ret = vd6283tx_als_set_channel5_gain(vd6283tx_als_channel5_gain_n25);
        break;
      case 12:
        ret = vd6283tx_als_set_channel5_gain(vd6283tx_als_channel5_gain_n33);
        break;
      case 13:
        ret = vd6283tx_als_set_channel5_gain(vd6283tx_als_channel5_gain_n50);
        break;
      case 14:
        ret = vd6283tx_als_set_channel5_gain(vd6283tx_als_channel5_gain_n66_6);
        break;
    }
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "vd6283tx_als.channel5_gain.value", channel5_gain);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vd6283tx_als.channel5_gain.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.channel6_gain"))
  {
    int channel6_gain = (int)json_object_dotget_number(tempJSONObject, "vd6283tx_als.channel6_gain");
    switch (channel6_gain)
    {
      case 0:
        ret = vd6283tx_als_set_channel6_gain(vd6283tx_als_channel6_gain_n0_71);
        break;
      case 1:
        ret = vd6283tx_als_set_channel6_gain(vd6283tx_als_channel6_gain_n0_83);
        break;
      case 2:
        ret = vd6283tx_als_set_channel6_gain(vd6283tx_als_channel6_gain_n1);
        break;
      case 3:
        ret = vd6283tx_als_set_channel6_gain(vd6283tx_als_channel6_gain_n1_25);
        break;
      case 4:
        ret = vd6283tx_als_set_channel6_gain(vd6283tx_als_channel6_gain_n1_67);
        break;
      case 5:
        ret = vd6283tx_als_set_channel6_gain(vd6283tx_als_channel6_gain_n2_5);
        break;
      case 6:
        ret = vd6283tx_als_set_channel6_gain(vd6283tx_als_channel6_gain_n3_33);
        break;
      case 7:
        ret = vd6283tx_als_set_channel6_gain(vd6283tx_als_channel6_gain_n5);
        break;
      case 8:
        ret = vd6283tx_als_set_channel6_gain(vd6283tx_als_channel6_gain_n7_1);
        break;
      case 9:
        ret = vd6283tx_als_set_channel6_gain(vd6283tx_als_channel6_gain_n10);
        break;
      case 10:
        ret = vd6283tx_als_set_channel6_gain(vd6283tx_als_channel6_gain_n16);
        break;
      case 11:
        ret = vd6283tx_als_set_channel6_gain(vd6283tx_als_channel6_gain_n25);
        break;
      case 12:
        ret = vd6283tx_als_set_channel6_gain(vd6283tx_als_channel6_gain_n33);
        break;
      case 13:
        ret = vd6283tx_als_set_channel6_gain(vd6283tx_als_channel6_gain_n50);
        break;
      case 14:
        ret = vd6283tx_als_set_channel6_gain(vd6283tx_als_channel6_gain_n66_6);
        break;
    }
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "vd6283tx_als.channel6_gain.value", channel6_gain);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vd6283tx_als.channel6_gain.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.samples_per_ts"))
  {
    int32_t samples_per_ts = (int32_t) json_object_dotget_number(tempJSONObject, "vd6283tx_als.samples_per_ts");
    ret = vd6283tx_als_set_samples_per_ts(samples_per_ts);
    if (ret == 0)
    {
      json_object_dotset_number(respJSONObject, "vd6283tx_als.samples_per_ts.value", samples_per_ts);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vd6283tx_als.samples_per_ts.value", "PNPL_SET_ERROR");
    }
  }
  if (json_object_dothas_value(tempJSONObject, "vd6283tx_als.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "vd6283tx_als.sensor_annotation");
    ret = vd6283tx_als_set_sensor_annotation(sensor_annotation);
    if (ret == 0)
    {
      json_object_dotset_string(respJSONObject, "vd6283tx_als.sensor_annotation.value", sensor_annotation);
    }
    else
    {
      json_object_dotset_string(respJSONObject, "vd6283tx_als.sensor_annotation.value", "PNPL_SET_ERROR");
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
  return 1;
}

