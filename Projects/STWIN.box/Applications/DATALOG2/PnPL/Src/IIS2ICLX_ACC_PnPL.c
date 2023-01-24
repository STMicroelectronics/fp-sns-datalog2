/**
  ******************************************************************************
  * @file    Iis2iclx_Acc_PnPL.c
  * @author  SRA
  * @brief   Iis2iclx_Acc PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fp_sns_datalog2:sensors:iis2iclx_acc;1
  *
  * Created by: DTDL2PnPL_cGen version 0.9.0
  *
  * WARNING! All changes made in this file will be lost relaunching the
  *          generation process!
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "App_model.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"
#include "PnPLCompManager.h"

#include "Iis2iclx_Acc_PnPL.h"
#include "Iis2iclx_Acc_PnPL_vtbl.h"


static const IPnPLComponent_vtbl sIis2iclx_Acc_PnPL_CompIF_vtbl =
{
  Iis2iclx_Acc_PnPL_vtblGetKey,
  Iis2iclx_Acc_PnPL_vtblGetNCommands,
  Iis2iclx_Acc_PnPL_vtblGetCommandKey,
  Iis2iclx_Acc_PnPL_vtblGetStatus,
  Iis2iclx_Acc_PnPL_vtblSetProperty,
  Iis2iclx_Acc_PnPL_vtblExecuteFunction
};

/**
  *  Iis2iclx_Acc_PnPL internal structure.
  */
struct _Iis2iclx_Acc_PnPL
{
  /**
    * Implements the IPnPLComponent interface.
    */
  IPnPLComponent_t component_if;

};

/* Objects instance */
/********************/
static Iis2iclx_Acc_PnPL sIis2iclx_Acc_PnPL;

// Public API definition
// *********************
IPnPLComponent_t *Iis2iclx_Acc_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sIis2iclx_Acc_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sIis2iclx_Acc_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Iis2iclx_Acc_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  iis2iclx_acc_comp_init();
  return 0;
}


// IPnPLComponent virtual functions definition
// *******************************************
char *Iis2iclx_Acc_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return iis2iclx_acc_get_key();
}

uint8_t Iis2iclx_Acc_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Iis2iclx_Acc_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Iis2iclx_Acc_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  float temp_f = 0;
  iis2iclx_acc_get_odr(&temp_f);
  uint8_t enum_id = 0;
  if(temp_f == hz12_5)
  {
    enum_id = 0;
  }
  else if(temp_f == hz26)
  {
    enum_id = 1;
  }
  else if(temp_f == hz52)
  {
    enum_id = 2;
  }
  else if(temp_f == hz104)
  {
    enum_id = 3;
  }
  else if(temp_f == hz208)
  {
    enum_id = 4;
  }
  else if(temp_f == hz416)
  {
    enum_id = 5;
  }
  else if(temp_f == hz833)
  {
    enum_id = 6;
  }
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.odr", enum_id);
  iis2iclx_acc_get_fs(&temp_f);
  enum_id = 0;
  if(temp_f == g0_5)
  {
    enum_id = 0;
  }
  else if(temp_f == g1)
  {
    enum_id = 1;
  }
  else if(temp_f == g2)
  {
    enum_id = 2;
  }
  else if(temp_f == g3)
  {
    enum_id = 3;
  }
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.fs", enum_id);
  bool temp_b = 0;
  iis2iclx_acc_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "iis2iclx_acc.enable", temp_b);
  int32_t temp_i = 0;
  iis2iclx_acc_get_samples_per_ts__val(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.samples_per_ts.val", temp_i);
  iis2iclx_acc_get_samples_per_ts__min(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.samples_per_ts.min", temp_i);
  iis2iclx_acc_get_samples_per_ts__max(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.samples_per_ts.max", temp_i);
  iis2iclx_acc_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.dim", temp_i);
  iis2iclx_acc_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.ioffset", temp_f);
  iis2iclx_acc_get_measodr(&temp_f);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.measodr", temp_f);
  iis2iclx_acc_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.usb_dps", temp_i);
  iis2iclx_acc_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.sd_dps", temp_i);
  iis2iclx_acc_get_sensitivity(&temp_f);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.sensitivity", temp_f);
  char *temp_s = "";
  iis2iclx_acc_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "iis2iclx_acc.data_type", temp_s);
  //Next fields are not in DTDL model but Added looking @the component schema field (this is :sensors) ONLY for Sensors and Algorithms
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  iis2iclx_acc_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.stream_id", temp_int8);
  iis2iclx_acc_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "iis2iclx_acc.ep_id", temp_int8);

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

  //no need to free temp_j as it is part of tempJSON
  json_value_free(tempJSON);

  return 0;
}

uint8_t Iis2iclx_Acc_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);

  uint8_t ret = 0;
  if(json_object_dothas_value(tempJSONObject, "iis2iclx_acc.odr"))
  {
    int odr = (int)json_object_dotget_number(tempJSONObject, "iis2iclx_acc.odr");
    switch(odr)
    {
    case 0:
      iis2iclx_acc_set_odr(hz12_5);
      break;
    case 1:
      iis2iclx_acc_set_odr(hz26);
      break;
    case 2:
      iis2iclx_acc_set_odr(hz52);
      break;
    case 3:
      iis2iclx_acc_set_odr(hz104);
      break;
    case 4:
      iis2iclx_acc_set_odr(hz208);
      break;
    case 5:
      iis2iclx_acc_set_odr(hz416);
      break;
    case 6:
      iis2iclx_acc_set_odr(hz833);
      break;
    }
  }
  if(json_object_dothas_value(tempJSONObject, "iis2iclx_acc.fs"))
  {
    int fs = (int)json_object_dotget_number(tempJSONObject, "iis2iclx_acc.fs");
    switch(fs)
    {
    case 0:
      iis2iclx_acc_set_fs(g0_5);
      break;
    case 1:
      iis2iclx_acc_set_fs(g1);
      break;
    case 2:
      iis2iclx_acc_set_fs(g2);
      break;
    case 3:
      iis2iclx_acc_set_fs(g3);
      break;
    }
  }
  if (json_object_dothas_value(tempJSONObject, "iis2iclx_acc.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "iis2iclx_acc.enable");
    iis2iclx_acc_set_enable(enable);
  }
  if (json_object_dothas_value(tempJSONObject, "iis2iclx_acc.samples_per_ts"))
  {
    if (json_object_dothas_value(tempJSONObject, "iis2iclx_acc.samples_per_ts.val"))
    {
      int32_t samples_per_ts =(int32_t) json_object_dotget_number(tempJSONObject, "iis2iclx_acc.samples_per_ts.val");
      iis2iclx_acc_set_samples_per_ts__val(samples_per_ts);
    }
  }
  json_value_free(tempJSON);
  return ret;
}

uint8_t Iis2iclx_Acc_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON)
{
  return 1;
}