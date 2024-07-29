/**
  ******************************************************************************
  * @file    Deviceinformation_PnPL.c
  * @author  SRA
  * @brief   Deviceinformation PnPL Component Manager
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
  * dtmi:azure:DeviceManagement:DeviceInformation;1
  *
  * Created by: DTDL2PnPL_cGen version 2.0.0
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

#include "Deviceinformation_PnPL.h"

static const IPnPLComponent_vtbl sDeviceinformation_PnPL_CompIF_vtbl =
{
  Deviceinformation_PnPL_vtblGetKey,
  Deviceinformation_PnPL_vtblGetNCommands,
  Deviceinformation_PnPL_vtblGetCommandKey,
  Deviceinformation_PnPL_vtblGetStatus,
  Deviceinformation_PnPL_vtblSetProperty,
  Deviceinformation_PnPL_vtblExecuteFunction
};

/**
  *  Deviceinformation_PnPL internal structure.
  */
struct _Deviceinformation_PnPL
{
  /* Implements the IPnPLComponent interface. */
  IPnPLComponent_t component_if;
};

/* Objects instance ----------------------------------------------------------*/
static Deviceinformation_PnPL sDeviceinformation_PnPL;

/* Public API definition -----------------------------------------------------*/
IPnPLComponent_t *Deviceinformation_PnPLAlloc()
{
  IPnPLComponent_t *pxObj = (IPnPLComponent_t *) &sDeviceinformation_PnPL;
  if (pxObj != NULL)
  {
    pxObj->vptr = &sDeviceinformation_PnPL_CompIF_vtbl;
  }
  return pxObj;
}

uint8_t Deviceinformation_PnPLInit(IPnPLComponent_t *_this)
{
  IPnPLComponent_t *component_if = _this;
  PnPLAddComponent(component_if);
  DeviceInformation_comp_init();
  return PNPL_NO_ERROR_CODE;
}


/* IPnPLComponent virtual functions definition -------------------------------*/
char *Deviceinformation_PnPL_vtblGetKey(IPnPLComponent_t *_this)
{
  return DeviceInformation_get_key();
}

uint8_t Deviceinformation_PnPL_vtblGetNCommands(IPnPLComponent_t *_this)
{
  return 0;
}

char *Deviceinformation_PnPL_vtblGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return "";
}

uint8_t Deviceinformation_PnPL_vtblGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                             uint8_t pretty)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  char *temp_s = "";
  DeviceInformation_get_manufacturer(&temp_s);
  json_object_dotset_string(JSON_Status, "DeviceInformation.manufacturer", temp_s);
  DeviceInformation_get_model(&temp_s);
  json_object_dotset_string(JSON_Status, "DeviceInformation.model", temp_s);
  DeviceInformation_get_swVersion(&temp_s);
  json_object_dotset_string(JSON_Status, "DeviceInformation.swVersion", temp_s);
  DeviceInformation_get_osName(&temp_s);
  json_object_dotset_string(JSON_Status, "DeviceInformation.osName", temp_s);
  DeviceInformation_get_processorArchitecture(&temp_s);
  json_object_dotset_string(JSON_Status, "DeviceInformation.processorArchitecture", temp_s);
  DeviceInformation_get_processorManufacturer(&temp_s);
  json_object_dotset_string(JSON_Status, "DeviceInformation.processorManufacturer", temp_s);
  float temp_f = 0;
  DeviceInformation_get_totalStorage(&temp_f);
  json_object_dotset_number(JSON_Status, "DeviceInformation.totalStorage", temp_f);
  DeviceInformation_get_totalMemory(&temp_f);
  json_object_dotset_number(JSON_Status, "DeviceInformation.totalMemory", temp_f);

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

uint8_t Deviceinformation_PnPL_vtblSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                               uint32_t *size, uint8_t pretty)
{
  *size = 61;
  *response = (char *)pnpl_malloc(*size);
  (void)strcpy(*response, "{\"PnPL_Error\":\"No writable Properties in DeviceInformation\"}\0");
  return PNPL_NO_ERROR_CODE;
}


uint8_t Deviceinformation_PnPL_vtblExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response,
                                                   uint32_t *size, uint8_t pretty)
{
  return PNPL_NO_COMMANDS_ERROR_CODE;
}

