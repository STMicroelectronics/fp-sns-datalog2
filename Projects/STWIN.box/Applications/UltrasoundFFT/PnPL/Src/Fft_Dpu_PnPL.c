/**
  ******************************************************************************
  * @file    Fft_Dpu_PnPL.c
  * @author  SRA
  * @brief   Fft_Dpu PnPL Component Manager
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_ultrasoundFft:algorithms:fft_dpu;3
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

#include "Fft_Dpu_PnPL.h"
#include "Fft_Dpu_PnPL_vtbl.h"

static const IPnPLComponent_vtbl sFft_Dpu_PnPL_CompIF_vtbl =
{
  Fft_Dpu_PnPL_vtblGetKey,
  Fft_Dpu_PnPL_vtblGetNCommands,
  Fft_Dpu_PnPL_vtblGetCommandKey,
  Fft_Dpu_PnPL_vtblGetStatus,
  Fft_Dpu_PnPL_vtblSetProperty,
  Fft_Dpu_PnPL_vtblExecuteFunction
};

/**
  *  Fft_Dpu_PnPL internal structure.
  */
struct _Fft_Dpu_PnPL
{
  /**
    * Implements the IPnPLComponent interface.
    */
  IPnPLComponent_t component_if;

};

/* Objects instance ----------------------------------------------------------*/
static Fft_Dpu_PnPL sFft_Dpu_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Fft_Dpu_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sFft_Dpu_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sFft_Dpu_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Fft_Dpu_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  fft_dpu_comp_init();
  return 0;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Fft_Dpu_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return fft_dpu_get_key();
}

uint8_t Fft_Dpu_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Fft_Dpu_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Fft_Dpu_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  bool temp_b = 0;
  fft_dpu_get_enable(&temp_b);
  json_object_dotset_boolean(JSON_Status, "fft_dpu.enable", temp_b);
  char *temp_s = "";
  fft_dpu_get_data_type(&temp_s);
  json_object_dotset_string(JSON_Status, "fft_dpu.data_type", temp_s);
  int32_t temp_i = 0;
  fft_dpu_get_fft_length(&temp_i);
  json_object_dotset_number(JSON_Status, "fft_dpu.fft_length", temp_i);
  fft_dpu_get_fft_sample_freq(&temp_i);
  json_object_dotset_number(JSON_Status, "fft_dpu.fft_sample_freq", temp_i);
  fft_dpu_get_usb_dps(&temp_i);
  json_object_dotset_number(JSON_Status, "fft_dpu.usb_dps", temp_i);
  fft_dpu_get_algorithm_type(&temp_i);
  json_object_dotset_number(JSON_Status, "fft_dpu.algorithm_type", temp_i);
  /* Next fields are not in DTDL model but added looking @ the component schema
  field (this is :algorithms). ONLY for Sensors, Algorithms and Actuators */
  json_object_dotset_number(JSON_Status, "fft_dpu.c_type", COMP_TYPE_ALGORITHM);
  int8_t temp_int8 = 0;
  fft_dpu_get_stream_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "fft_dpu.stream_id", temp_int8);
  fft_dpu_get_ep_id(&temp_int8);
  json_object_dotset_number(JSON_Status, "fft_dpu.ep_id", temp_int8);

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

uint8_t Fft_Dpu_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON)
{
  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *tempJSONObject = json_value_get_object(tempJSON);

  uint8_t ret = 0;
  if (json_object_dothas_value(tempJSONObject, "fft_dpu.enable"))
  {
    bool enable = json_object_dotget_boolean(tempJSONObject, "fft_dpu.enable");
    fft_dpu_set_enable(enable);
  }
  json_value_free(tempJSON);
  return ret;
}

uint8_t Fft_Dpu_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON)
{
  return 1;
}
