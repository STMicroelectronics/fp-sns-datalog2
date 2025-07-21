/**
  ******************************************************************************
  * @file    Lsm6dso16is_Ispu_PnPL.c
  * @author  SRA
  * @brief   Lsm6dso16is_Ispu PnPL Component Manager
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
  * dtmi:vespucci:nucleo_u575zi_q:x_nucleo_iks4a1:FP_SNS_DATALOG2_Datalog2:sensors:lsm6dso16is_ispu;1
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

#include "Lsm6dso16is_Ispu_PnPL.h"

static const IPnPLComponent_vtbl sLsm6dso16is_Ispu_PnPL_CompIF_vtbl =
{
  Lsm6dso16is_Ispu_PnPL_vtblGetKey,
  Lsm6dso16is_Ispu_PnPL_vtblGetNCommands,
  Lsm6dso16is_Ispu_PnPL_vtblGetCommandKey,
  Lsm6dso16is_Ispu_PnPL_vtblGetStatus,
  Lsm6dso16is_Ispu_PnPL_vtblSetProperty,
  Lsm6dso16is_Ispu_PnPL_vtblExecuteFunction
};

/**
  *  Lsm6dso16is_Ispu_PnPL internal structure.
  */
struct _Lsm6dso16is_Ispu_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Lsm6dso16is_Ispu_PnPL sLsm6dso16is_Ispu_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Lsm6dso16is_Ispu_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sLsm6dso16is_Ispu_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sLsm6dso16is_Ispu_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Lsm6dso16is_Ispu_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  lsm6dso16is_ispu_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Lsm6dso16is_Ispu_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return lsm6dso16is_ispu_get_key();
}

uint8_t Lsm6dso16is_Ispu_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 1;
}

char *Lsm6dso16is_Ispu_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  switch (id)
  {
    case 0:
      return "lsm6dso16is_ispu*load_file";
      break;
  }
  return "";
}

uint8_t Lsm6dso16is_Ispu_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                            uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  bool temp_b = 0;
  lsm6dso16is_ispu_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "lsm6dso16is_ispu.enable", temp_b);
  int32_t temp_i = 0;
  lsm6dso16is_ispu_get_samples_per_ts(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dso16is_ispu.samples_per_ts", temp_i);
  lsm6dso16is_ispu_get_dim(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dso16is_ispu.dim", temp_i);
  float_t temp_f = 0;
  lsm6dso16is_ispu_get_ioffset(&temp_f);
  json_object_dotset_number(JSON_Status, "lsm6dso16is_ispu.ioffset", temp_f);
  lsm6dso16is_ispu_get_ucf_status(&temp_b);
  json_object_dotset_boolean(JSON_Status, "lsm6dso16is_ispu.ucf_status", temp_b);
  lsm6dso16is_ispu_get_usb_dps(&temp_f);
  json_object_dotset_number(JSON_Status, "lsm6dso16is_ispu.usb_dps", temp_f);
  lsm6dso16is_ispu_get_sd_dps(&temp_f);
  json_object_dotset_number(JSON_Status, "lsm6dso16is_ispu.sd_dps", temp_f);
  char *temp_s = "";
  lsm6dso16is_ispu_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "lsm6dso16is_ispu.data_type", temp_s);
  lsm6dso16is_ispu_get_sensor_annotation(&temp_s);
  json_object_dotset_string(JSON_Status, "lsm6dso16is_ispu.sensor_annotation", temp_s);
  lsm6dso16is_ispu_get_sensor_category(&temp_i);
  json_object_dotset_number(JSON_Status, "lsm6dso16is_ispu.sensor_category", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :sensors). ONLY for Sensors, Algorithms and Actuators*/
  json_object_dotset_number(JSON_Status, "lsm6dso16is_ispu.c_type", COMP_TYPE_SENSOR);
  int8_t temp_int8 = 0;
  lsm6dso16is_ispu_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "lsm6dso16is_ispu.stream_id", temp_int8);
  lsm6dso16is_ispu_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "lsm6dso16is_ispu.ep_id", temp_int8);

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

uint8_t Lsm6dso16is_Ispu_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                              uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  bool valid_property = false;
  char *resp_msg;
  if (json_object_dothas_value(tempJSONObject, "lsm6dso16is_ispu.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "lsm6dso16is_ispu.enable");
    valid_property = true;
    ret = lsm6dso16is_ispu_set_enable(enable, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      bool old_enable;
      lsm6dso16is_ispu_get_enable(&old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.value", old_enable);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", false);
    }
  }
  if (json_object_dothas_value(tempJSONObject, "lsm6dso16is_ispu.sensor_annotation"))
  {
    const char *sensor_annotation = json_object_dotget_string(tempJSONObject, "lsm6dso16is_ispu.sensor_annotation");
    valid_property = true;
    ret = lsm6dso16is_ispu_set_sensor_annotation(sensor_annotation, &resp_msg);
    json_object_dotset_string(respJSONObject, "PnPL_Response.message", resp_msg);
    if (ret == PNPL_NO_ERROR_CODE)
    {
      json_object_dotset_string(respJSONObject, "PnPL_Response.value", sensor_annotation);
      json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", true);
    }
    else
    {
      char *old_sensor_annotation;
      lsm6dso16is_ispu_get_sensor_annotation(&old_sensor_annotation);
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
    char *log_message = "Invalid property for lsm6dso16is_ispu";
    PnPLCreateLogMessage(response, size, log_message, PNPL_LOG_ERROR);
    ret = PNPL_BASE_ERROR_CODE;
  }
  json_value_free(respJSON);
  return ret;
}


uint8_t Lsm6dso16is_Ispu_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                  uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);

  uint8_t ret = PNPL_NO_ERROR_CODE;
  bool valid_function = false;
  if (json_object_dothas_value(tempJSONObject, "lsm6dso16is_ispu*load_file.files"))
  {
    valid_function = true;
    const char *ucf_data;
    int32_t ucf_size;
    const char *output_data;
    int32_t output_size;
    if (json_object_dothas_value(tempJSONObject, "lsm6dso16is_ispu*load_file.files.ucf_data"))
    {
      ucf_data =  json_object_dotget_string(tempJSONObject, "lsm6dso16is_ispu*load_file.files.ucf_data");
      if (json_object_dothas_value(tempJSONObject, "lsm6dso16is_ispu*load_file.files.ucf_size"))
      {
        ucf_size = (int32_t) json_object_dotget_number(tempJSONObject, "lsm6dso16is_ispu*load_file.files.ucf_size");
        if (json_object_dothas_value(tempJSONObject, "lsm6dso16is_ispu*load_file.files.output_data"))
        {
          output_data =  json_object_dotget_string(tempJSONObject, "lsm6dso16is_ispu*load_file.files.output_data");
          if (json_object_dothas_value(tempJSONObject, "lsm6dso16is_ispu*load_file.files.output_size"))
          {
            output_size = (int32_t) json_object_dotget_number(tempJSONObject, "lsm6dso16is_ispu*load_file.files.output_size");
            ret = lsm6dso16is_ispu_load_file((char *) ucf_data, ucf_size, (char *) output_data, output_size);
          }
        }
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

