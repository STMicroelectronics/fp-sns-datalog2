/**
  ******************************************************************************
  * @file    Lsm6dsv16x_Mlc_PnPL.c
  * @author  SRA
  * @brief   Lsm6dsv16x_Mlc PnPL Component Manager
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
  * dtmi:vespucci:nucleo_u575zi_q:x_nucleo_iks4a1:FP_SNS_DATALOG2_Datalog2:sensors:lsm6dsv16x_mlc;1
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

#include "Lsm6dsv16x_Mlc_PnPL.h"

static const IPnPLComponent_vtbl sLsm6dsv16x_Mlc_PnPL_CompIF_vtbl =
{
  Lsm6dsv16x_Mlc_PnPL_vtblGetKey,
  Lsm6dsv16x_Mlc_PnPL_vtblGetNCommands,
  Lsm6dsv16x_Mlc_PnPL_vtblGetCommandKey,
  Lsm6dsv16x_Mlc_PnPL_vtblGetStatus,
  Lsm6dsv16x_Mlc_PnPL_vtblSetProperty,
  Lsm6dsv16x_Mlc_PnPL_vtblExecuteFunction
};

/**
  *  Lsm6dsv16x_Mlc_PnPL internal structure.
  */
struct _Lsm6dsv16x_Mlc_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Lsm6dsv16x_Mlc_PnPL sLsm6dsv16x_Mlc_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Lsm6dsv16x_Mlc_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sLsm6dsv16x_Mlc_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sLsm6dsv16x_Mlc_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Lsm6dsv16x_Mlc_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  lsm6dsv16x_mlc_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Lsm6dsv16x_Mlc_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return lsm6dsv16x_mlc_get_key();
}

uint8_t Lsm6dsv16x_Mlc_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 1;
}

char *Lsm6dsv16x_Mlc_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  switch (id)
  {
    case 0:
      return "lsm6dsv16x_mlc*load_file";
      break;
  }
  return "";
}

uint8_t Lsm6dsv16x_Mlc_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                          uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  bool temp_b = 0;
  lsm6dsv16x_mlc_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "lsm6dsv16x_mlc.enable", temp_b);
  int32_t temp_i = 0;
  lsm6dsv16x_mlc_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.samples_per_ts", temp_i);
  lsm6dsv16x_mlc_get_ucf_status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "lsm6dsv16x_mlc.ucf_status", temp_b);
  lsm6dsv16x_mlc_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.dim", temp_i);
  float_t temp_f = 0;
  lsm6dsv16x_mlc_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.ioffset", temp_f);
  char *temp_s = "";
  lsm6dsv16x_mlc_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "lsm6dsv16x_mlc.data_type", temp_s);
  lsm6dsv16x_mlc_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.usb_dps", temp_i);
  lsm6dsv16x_mlc_get_sd_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.sd_dps", temp_i);
  lsm6dsv16x_mlc_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "lsm6dsv16x_mlc.sensor_annotation", temp_s);
  lsm6dsv16x_mlc_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.sensor_category", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  lsm6dsv16x_mlc_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.stream_id", temp_int8);
  lsm6dsv16x_mlc_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "lsm6dsv16x_mlc.ep_id", temp_int8);

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

uint8_t Lsm6dsv16x_Mlc_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                            uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  bool valid_property = false;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "lsm6dsv16x_mlc.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "lsm6dsv16x_mlc.enable");
    valid_property = true;
    ret = lsm6dsv16x_mlc_set_enable(enable, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_enable;
      lsm6dsv16x_mlc_get_enable(&old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lsm6dsv16x_mlc.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "lsm6dsv16x_mlc.sensor_annotation");
    valid_property = true;
    ret = lsm6dsv16x_mlc_set_sensor_annotation(sensor_annotation, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_sensor_annotation;
      lsm6dsv16x_mlc_get_sensor_annotation(&old_sensor_annotation);
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
    char *log_message = "Invalid property for lsm6dsv16x_mlc";
    PnPLCreateLogMessage(response, size, log_message, PNPL_LOG_ERROR);
    ret = PNPL_BASE_ERROR_CODE;
  }
  json_value_free(respJSON);
  return ret;
}


uint8_t Lsm6dsv16x_Mlc_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  bool valid_function = false;
  if (json_object_dothas_value(tempJSONObject, "lsm6dsv16x_mlc*load_file.ucf_data"))
  {
    valid_function = true;
    const char *data;
    int32_t size;
    if (json_object_dothas_value(tempJSONObject, "lsm6dsv16x_mlc*load_file.ucf_data.data"))
    {
      data =  json_object_dotget_string(tempJSONObject, "lsm6dsv16x_mlc*load_file.ucf_data.data");
      if (json_object_dothas_value(tempJSONObject, "lsm6dsv16x_mlc*load_file.ucf_data.size"))
      {
        size = (int32_t) json_object_dotget_number(tempJSONObject, "lsm6dsv16x_mlc*load_file.ucf_data.size");
        ret = lsm6dsv16x_mlc_load_file((char *) data, size);
      }
    }
  }
  /* Check if received a valid function to modify an existing property */
  if (valid_function == false)
  {
    char log_message[100];
    (void) sprintf(log_message, "%s Invalid command", serializedJSON);
    PnPLCreateLogMessage(response, size, log_message, PNPL_LOG_ERROR);
    ret = PNPL_BASE_ERROR_CODE;
  }
  json_value_free(tempJSON);
  return ret;
}

