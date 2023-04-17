/**
 ******************************************************************************
 * @file    PnPLCompManager.c
 * @author  SRA
 * @brief
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 ******************************************************************************
 */

#include "PnPLCompManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static PnPLCompManager_t spPnPLObj =
{
  0
};

static char global_uuid[37]; // UUID: 8 + "-" + 4 + "-" + 4 + "-" + 4 + "-" 12 = 36char + \0

#ifndef FW_ID
static uint8_t fw_id;
#endif

#ifndef BOARD_ID
 #error "Please define a valid Board ID"
#endif



/* Get FW Unique ID */
uint8_t PnPLGetFWID(void)
{
#ifndef FW_ID
  return fw_id;
#else
  return FW_ID;
#endif
}

#ifndef FW_ID
/* Set FW Unique ID */
void PnPLSetFWID(uint8_t id)
{
  fw_id = id;
}
#endif

/* Unique ID is directly derived from STM32 UID and converted to string
string needs to be 25bytes 24+\0  */
static void PnPLGetUniqueID(char *id)
{
  uint32_t stm32_UID[3];

  stm32_UID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
  stm32_UID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
  stm32_UID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));

  (void)sprintf(id, "%08lX%08lX%08lX", (unsigned long)stm32_UID[0], (unsigned long)stm32_UID[1], (unsigned long)stm32_UID[2]);
}

/**
  * @brief Generate and store the Acquisition UUID
  * @param None
  * @retval None
  */
void PnPLGenerateAcquisitionUUID(char *uuid)
{
  uint32_t UUID[4];
  uint8_t *p8 = (uint8_t *)UUID;
  uint8_t index = 0;
    
  UUID[0] = (uint32_t)rand();
  UUID[1] = (uint32_t)rand();
  UUID[2] = (uint32_t)rand();
  UUID[3] = (uint32_t)rand();

  /*
  * UUID format
  * xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx
  * M: The four bits of digit M are the UUID version --> 4 is for random number
  * N: 1 to 3 most significant bits of digit N represent UUID variant --> using Variant 1 (10x)
  * */
  p8[5] = 0x40u | (p8[5] & 0xfu);
  p8[11] = 0x80u | (p8[11] & 0x3fu); /* in variant 1, 3rd bit can be ignored */

  index += (uint8_t)sprintf(&global_uuid[index], "%08lx-", (unsigned long)UUID[0]);
  index += (uint8_t)sprintf(&global_uuid[index], "%04lx-%04lx-", (unsigned long)((UUID[1] >> 16) & 0xFFFFu), (unsigned long)(UUID[1] & 0xFFFFu));
  index += (uint8_t)sprintf(&global_uuid[index], "%04lx-%04lx", (unsigned long)((UUID[2] >> 16) & 0xFFFFu), (unsigned long)(UUID[2] & 0xFFFFu));
  index += (uint8_t)sprintf(&global_uuid[index], "%08lx", (unsigned long)UUID[3]);

  (void)strcpy(uuid, global_uuid);
}

uint8_t PnPLAddComponent(IPnPLComponent_t *pComponent)
{
  uint8_t id = 0;
  uint16_t ii;
  bool add_ok = false;

  for (ii = 0; ii <= spPnPLObj.n_components; ii++)
  {
    if (spPnPLObj.Components[ii] == NULL)
    {
      spPnPLObj.Components[ii] = pComponent;
      add_ok = true;
      id = (uint8_t)ii;
    }
  }

  if (add_ok)
  {
    spPnPLObj.n_components++;
  }

  return id;
}

uint16_t PnPLGetNComponents(void)
{
  return spPnPLObj.n_components;
}


uint16_t PnPLGetComponentsNames(char **components_names)
{
  IPnPLComponent_t *p_obj;

  for (uint16_t i = 0; i<spPnPLObj.n_components; i++)
  {
    p_obj = (IPnPLComponent_t *)(spPnPLObj.Components[i]);
    strcpy(components_names[i], IPnPLComponentGetKey(p_obj));
  }

  return spPnPLObj.n_components;
}

uint8_t PnPLGetComponentValue(char *comp_name, char **SerializedJSON, uint32_t *size, uint8_t pretty)
{
  JSON_Object *JSON_CompObject;
  JSON_Value *tempJSON;
  JSON_Value *tempJSON_noKey;
  char *comp_string = NULL;;
  uint8_t comp_found = 0;

  for (uint8_t i = 0; i < PnPLGetNComponents(); i++)
  {
    IPnPLComponent_t *p_obj = (IPnPLComponent_t *)(spPnPLObj.Components[i]);
    if (strcmp(comp_name, IPnPLComponentGetKey(p_obj)) == 0)
    {
      IPnPLComponentGetStatus(p_obj, &comp_string, size, pretty);

      tempJSON = json_parse_string(comp_string);
      JSON_CompObject = json_object(tempJSON);
      tempJSON_noKey = json_object_get_value(JSON_CompObject, comp_name);

      /* convert to a json string and write to file */
      if (pretty == 1u)
      {
        *SerializedJSON = json_serialize_to_string_pretty(tempJSON_noKey);
        *size = json_serialization_size_pretty(tempJSON_noKey);
      }
      else
      {
        *SerializedJSON = json_serialize_to_string(tempJSON_noKey);
        *size = json_serialization_size(tempJSON_noKey);
      }

      comp_found = 1;

      json_free_serialized_string(comp_string);
      json_value_free(tempJSON);

      break;
    }
  }

  if (comp_found > 0u)
  {
    return PNPL_CMD_NO_ERROR_CODE;
  }

  return PNPL_CMD_ERROR_CODE;
}


uint8_t PnPLGetPresentationJSON(char **serializedJSON, uint32_t *size)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_Status;

  tempJSON = json_value_init_object();
  JSON_Status = json_value_get_object(tempJSON);

  json_object_dotset_number(JSON_Status, "board_id", BOARD_ID);
#ifndef FW_ID
  json_object_dotset_number(JSON_Status, "fw_id", fw_id);
#else
  json_object_dotset_number(JSON_Status, "fw_id", FW_ID);
#endif

  *serializedJSON = json_serialize_to_string(tempJSON);
  *size = json_serialization_size(tempJSON);

  json_value_free(tempJSON);
  return PNPL_CMD_NO_ERROR_CODE;
}


uint8_t PnPLGetDeviceStatusJSON(char **serializedJSON, uint32_t *size, uint8_t pretty)
{

  char serial_number[25];
//  char uuid[37];

  JSON_Value *tempJSON;
  JSON_Value *tempJSONDevice;
  JSON_Value *tempJSON1;
  JSON_Object *JSON_DeviceConfig;
  JSON_Object *JSON_Device;
  JSON_Array *JSON_DeviceArray;
  JSON_Array *JSON_ComponentArray;

  uint32_t sz_comp = 0;
  char *ser_comp = NULL;

  tempJSON = json_value_init_object();
  JSON_DeviceConfig = json_value_get_object(tempJSON);


  /*{
      "schema_version": "2.0.0",                 (Reference to the schema version adopted.)
      "uuid" : "<uuid-of-the-acquisition>",      (Unique identifier of an acquisition.)​
      "devices":[​
          {​
              "board_id": <board_id>,            (Unique identifier of a board type, according to Vespucci catalogs.)​
              "fw_id": <fw_id>,                  (Unique identifier of a firmware type, according to Vespucci catalogs.)​
              "sn": "<serial_number>",           (Unique serial number of the device that contributes to the acquisition.)​
              "components": [                    (Array of components of the device; this list has to reflect the device template.)​
                  {​
                      "device_info": {           (Optional - The "device_info" component contains additional information about the device.)​
                          "alias": "<alias>",​
                          "fw_name": "<name-of-the-firmware>",​
                          "fw_version": "<version-of-the-firmware>",​
                          "part_number": "<part-number>"​
                      }​
                  }​
              ]​
          }​
      ],​​
  }*/

  json_object_dotset_string(JSON_DeviceConfig, "schema_version", "2.0.0");
  //char uuid[37];
  //PnPLGenerateAcquisitionUUID(uuid);
  //json_object_dotset_string(JSON_Device, "uuid", uuid);
  json_object_dotset_string(JSON_DeviceConfig, "uuid", global_uuid);

  tempJSONDevice = json_value_init_object();
  JSON_Device = json_value_get_object(tempJSONDevice);

  json_object_set_value(JSON_DeviceConfig, "devices", json_value_init_array());
  JSON_DeviceArray = json_object_dotget_array(JSON_DeviceConfig, "devices");
  json_array_append_value(JSON_DeviceArray, tempJSONDevice);

  json_object_dotset_number(JSON_Device, "board_id", BOARD_ID);
#ifndef FW_ID
  json_object_dotset_number(JSON_Device, "fw_id", fw_id);
#else
  json_object_dotset_number(JSON_Device, "fw_id", FW_ID);
#endif
  PnPLGetUniqueID(serial_number);
  json_object_dotset_string(JSON_Device, "sn", serial_number);

//  json_object_set_value(JSON_Device, "device", json_value_init_array());
//  JSON_ComponentArray = json_object_dotget_array(JSON_Device, "device");
  json_object_set_value(JSON_Device, "components", json_value_init_array());
  JSON_ComponentArray = json_object_dotget_array(JSON_Device, "components");

  for (uint8_t i = 0; i < PnPLGetNComponents(); i++)
  {
    IPnPLComponent_t *p_obj = spPnPLObj.Components[i];
    sz_comp = 0;
    ser_comp = NULL;
    IPnPLComponentGetStatus(p_obj, &ser_comp, &sz_comp, 0);
    tempJSON1 = json_parse_string(ser_comp);
    json_array_append_value(JSON_ComponentArray, tempJSON1);
    json_free_serialized_string(ser_comp);
  }

  /* convert to a json string and write to file */
  if (pretty == 1u)
  {
    *serializedJSON = json_serialize_to_string_pretty(tempJSON);
    *size = json_serialization_size_pretty(tempJSON);
  }
  else
  {
    *serializedJSON = json_serialize_to_string(tempJSON);
    *size = json_serialization_size(tempJSON);
  }

  json_value_free(tempJSON);
  return PNPL_CMD_NO_ERROR_CODE;
}


uint8_t PnPLGetFilteredDeviceStatusJSON(char **serializedJSON, uint32_t *size, char **skip_list, uint32_t skip_list_size, uint8_t pretty)
{
  char serial_number[25];
 //  char uuid[37];

   JSON_Value *tempJSON;
   JSON_Value *tempJSONDevice;
   JSON_Value *tempJSON1;
   JSON_Object *JSON_DeviceConfig;
   JSON_Object *JSON_Device;
   JSON_Array *JSON_DeviceArray;
   JSON_Array *JSON_ComponentArray;

   uint32_t sz_comp = 0;
   char *ser_comp = NULL;

   tempJSON = json_value_init_object();
   JSON_DeviceConfig = json_value_get_object(tempJSON);


   /*{
       "schema_version": "2.0.0",                 (Reference to the schema version adopted.)
       "uuid" : "<uuid-of-the-acquisition>",      (Unique identifier of an acquisition.)​
       "devices":[​
           {​
               "board_id": <board_id>,            (Unique identifier of a board type, according to Vespucci catalogs.)​
               "fw_id": <fw_id>,                  (Unique identifier of a firmware type, according to Vespucci catalogs.)​
               "sn": "<serial_number>",           (Unique serial number of the device that contributes to the acquisition.)​
               "components": [                    (Array of components of the device; this list has to reflect the device template.)​
                   {​
                       "device_info": {           (Optional - The "device_info" component contains additional information about the device.)​
                           "alias": "<alias>",​
                           "fw_name": "<name-of-the-firmware>",​
                           "fw_version": "<version-of-the-firmware>",​
                           "part_number": "<part-number>"​
                       }​
                   }​
               ]​
           }​
       ],​
       ​
   }*/

   json_object_dotset_string(JSON_DeviceConfig, "schema_version", "2.0.0");
   //char uuid[37];
   //PnPLGenerateAcquisitionUUID(uuid);
   //json_object_dotset_string(JSON_Device, "uuid", uuid);
   json_object_dotset_string(JSON_DeviceConfig, "uuid", global_uuid);

   tempJSONDevice = json_value_init_object();
   JSON_Device = json_value_get_object(tempJSONDevice);

   json_object_set_value(JSON_DeviceConfig, "devices", json_value_init_array());
   JSON_DeviceArray = json_object_dotget_array(JSON_DeviceConfig, "devices");
   json_array_append_value(JSON_DeviceArray, tempJSONDevice);

   json_object_dotset_number(JSON_Device, "board_id", BOARD_ID);
#ifndef FW_ID
  json_object_dotset_number(JSON_Device, "fw_id", fw_id);
#else
  json_object_dotset_number(JSON_Device, "fw_id", FW_ID);
#endif
   PnPLGetUniqueID(serial_number);
   json_object_dotset_string(JSON_Device, "sn", serial_number);

 //  json_object_set_value(JSON_Device, "device", json_value_init_array());
 //  JSON_ComponentArray = json_object_dotget_array(JSON_Device, "device");
   json_object_set_value(JSON_Device, "components", json_value_init_array());
   JSON_ComponentArray = json_object_dotget_array(JSON_Device, "components");

   bool skip = false;
   for (uint8_t i = 0; i < PnPLGetNComponents(); i++)
   {
     IPnPLComponent_t *p_obj = spPnPLObj.Components[i];
     for (uint8_t y = 0; y < skip_list_size; y++)
     {
       if(strcmp(IPnPLComponentGetKey(p_obj), skip_list[y]) == 0)
       {
         skip = true;
       }
     }
     if(!skip)
     {
       sz_comp = 0;
       ser_comp = NULL;
       IPnPLComponentGetStatus(p_obj, &ser_comp, &sz_comp, 0);
       tempJSON1 = json_parse_string(ser_comp);
       json_array_append_value(JSON_ComponentArray, tempJSON1);
       json_free_serialized_string(ser_comp);
     }
     skip = false;
   }

   /* convert to a json string and write to file */
   if (pretty == 1u)
   {
     *serializedJSON = json_serialize_to_string_pretty(tempJSON);
     *size = json_serialization_size_pretty(tempJSON);
   }
   else
   {
     *serializedJSON = json_serialize_to_string(tempJSON);
     *size = json_serialization_size(tempJSON);
   }

   json_value_free(tempJSON);
   return PNPL_CMD_NO_ERROR_CODE;
}


uint8_t PnPLUpdateDeviceStatusFromJSON(char *serializedJSON)
{
  char componentName[COMP_KEY_MAX_LENGTH];

  JSON_Array *JSON_components, *JSON_devices;
  JSON_Object *component, *device;
  JSON_Value *component_value;

  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *JSON_ParseHandler = json_value_get_object(tempJSON);

  if (json_object_has_value(JSON_ParseHandler, "devices") == 1)
  {
    JSON_devices = json_object_get_array(JSON_ParseHandler, "devices");
    device = json_array_get_object(JSON_devices, 0);
    JSON_components = json_object_get_array(device, "components");
  }
  else
  {
    //old status version (without "device" key)
    JSON_components = json_object_get_array(JSON_ParseHandler, "");
  }

  for (uint8_t i = 0; i < json_array_get_count(JSON_components); i++)
  {
    component = json_array_get_object(JSON_components, i);
    strcpy(componentName, json_object_get_name(component, 0));
    for (uint8_t j = 0; j < PnPLGetNComponents(); j++)
    {
      IPnPLComponent_t *p_obj = (IPnPLComponent_t *)(spPnPLObj.Components[j]);
      if (strcmp(componentName, IPnPLComponentGetKey(p_obj)) == 0)
      {
        component_value = json_object_get_value(component, componentName);
        char *comp_string = json_serialize_to_string(json_value_get_parent(component_value));
        IPnPLComponentSetProperty(p_obj, comp_string);
        json_free_serialized_string(comp_string);
        break;
      }
    }
  }

  json_value_free(tempJSON);
  return PNPL_CMD_NO_ERROR_CODE;
}

static uint8_t extract_PnPL_cmd_data(char *commandString, uint8_t *commandType, char *componentName)
{
  JSON_Value *tempJSON = json_parse_string(commandString);
  if (tempJSON != NULL)
  {
    JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
    strcpy(componentName, json_object_get_name(tempJSONObject, 0));
    for (uint8_t i = 0; i < PnPLGetNComponents(); i++)
    {
      IPnPLComponent_t *p_obj = (IPnPLComponent_t *)(spPnPLObj.Components[i]);
      if (strcmp(IPnPLComponentGetKey(p_obj), componentName) == 0)
      {
        //Check if extracted string is a component added to the current FW
        *commandType = PNPL_CMD_SET;
        json_value_free(tempJSON);
        return PNPL_CMD_NO_ERROR_CODE;
      }
      //in case of control commands
      uint8_t nOfCommands = IPnPLComponentGetNCommands(p_obj);
      if (nOfCommands > 0u)
      {
        for (uint8_t j = 0; j < nOfCommands; j++)
        {
          if (strcmp(IPnPLComponentGetCommandKey(p_obj, j), componentName) == 0)
          {
            *commandType = PNPL_CMD_SET;
            json_value_free(tempJSON);
            return PNPL_CMD_NO_ERROR_CODE;
          }
        }
      }
    }

    if (strcmp(componentName, "get_status") == 0)
    {
      *commandType = PNPL_CMD_GET;
      strcpy(componentName, json_object_get_string(tempJSONObject, "get_status"));
      json_value_free(tempJSON);
      return PNPL_CMD_NO_ERROR_CODE;
    }
    else if (strcmp(componentName, "update_device_status") == 0)
    {
      *commandType = PNPL_CMD_UPDATE_DEVICE;
      JSON_Value *tempJSONValue = json_object_get_value(tempJSONObject, "update_device_status");
      strcpy(commandString, json_serialize_to_string(tempJSONValue));
      json_value_free(tempJSONValue);
      return PNPL_CMD_NO_ERROR_CODE;
    }
    else if (strcmp(componentName, "system_config") == 0)
    {
      *commandType = PNPL_CMD_SYSTEM_CONFIG;
      JSON_Object *tempJSONObject2 = json_object_get_object(tempJSONObject, "system_config");
      strcpy(componentName, json_object_get_string(tempJSONObject2, "comp_name"));
      json_value_free(tempJSON);
      return PNPL_CMD_NO_ERROR_CODE;
    }
    else if (strcmp(componentName, "system_info") == 0)//NOTE Used for OLD get_presentation command
    {
      *commandType = PNPL_CMD_SYSTEM_INFO;
      strcpy(componentName, "");
      json_value_free(tempJSON);
      return PNPL_CMD_NO_ERROR_CODE;
    }
    else if (strcmp(componentName, "get_presentation") == 0)//NOTE New get_presentation command
    {
      *commandType = PNPL_CMD_SYSTEM_INFO;
      strcpy(componentName, "");
      json_value_free(tempJSON);
      return PNPL_CMD_NO_ERROR_CODE;
    }
  }
  json_value_free(tempJSON);
  //Not JSON command!
  return PNPL_CMD_ERROR_CODE;
}

uint8_t PnPLParseCommand(char *commandString, PnPLCommand_t *command)
{
  uint8_t commandType = 0;
  char componentName[2 *
                     COMP_KEY_MAX_LENGTH]; //2* because this could be a comp or a comm key. If comm_key this is in the form (comp_key*comm_key)
  uint8_t ret = extract_PnPL_cmd_data(commandString, &commandType, componentName);

  command->comm_type = commandType;
  strcpy(command->comp_name, componentName);

  if (ret == PNPL_CMD_NO_ERROR_CODE)
  {
    if (commandType == PNPL_CMD_SET)
    {
      //Select right parse/update function
      for (uint8_t i = 0; i < PnPLGetNComponents(); i++)
      {
        IPnPLComponent_t *p_obj = (IPnPLComponent_t *)(spPnPLObj.Components[i]);
        if (strcmp(componentName, IPnPLComponentGetKey(p_obj)) == 0)
        {
          IPnPLComponentSetProperty(p_obj, commandString);
        }
        uint8_t nOfCommands = IPnPLComponentGetNCommands(p_obj); // 0 if none
        if (nOfCommands > 0u)
        {
          for (uint8_t j = 0; j < nOfCommands; j++)
          {
            if (strcmp(IPnPLComponentGetCommandKey(p_obj, j), componentName) == 0)
            {
              IPnPLCommandExecuteFunction(p_obj, commandString);
            }
          }
        }
      }
    }
    if (commandType == PNPL_CMD_SYSTEM_CONFIG)
    {
      //TODO merge this logic with SET?? or make a function!
      for (uint8_t i = 0; i < PnPLGetNComponents(); i++)
      {
        IPnPLComponent_t *p_obj = (IPnPLComponent_t *)(spPnPLObj.Components[i]);
        if (strcmp(componentName, IPnPLComponentGetKey(p_obj)) == 0)
        {
          IPnPLComponentSetProperty(p_obj, commandString);
        }
      }
    }
    if (commandType == PNPL_CMD_UPDATE_DEVICE)
    {
      PnPLUpdateDeviceStatusFromJSON(commandString);
    }
  }
  else
  {
    command->comm_type = PNPL_CMD_ERROR;
  }
  return ret;
}

uint8_t PnPLSerializeResponse(PnPLCommand_t *command, char **SerializedJSON, uint32_t *size, uint8_t pretty)
{
  uint8_t ret = PNPL_CMD_NO_ERROR_CODE;

  if(command->comm_type == PNPL_CMD_SYSTEM_INFO)
  {
   (void)PnPLGetPresentationJSON(SerializedJSON, size);
  }
  else if (command->comm_type == PNPL_CMD_GET)
  {
    uint16_t comp_found = 0;
    if (strcmp(command->comp_name, "all") == 0)
    {
      (void)PnPLGetDeviceStatusJSON(SerializedJSON, size, pretty);
      comp_found = PnPLGetNComponents();
    }
    else
    {
      for (uint8_t i = 0; i < PnPLGetNComponents(); i++)
      {
        IPnPLComponent_t *p_obj = (IPnPLComponent_t *)(spPnPLObj.Components[i]);
        if (strcmp(command->comp_name, IPnPLComponentGetKey(p_obj)) == 0)
        {
          if (IPnPLComponentGetStatus(p_obj, SerializedJSON, size, pretty) == 0u)
          {
            comp_found = 1;
          }
          break;
        }
      }
    }
    if (comp_found == 0u)
    {
      ret = PNPL_CMD_ERROR_CODE;
    }
  }
  else if (command->comm_type == PNPL_CMD_ERROR)
  {
    (void)strcpy(*SerializedJSON, "{\"PnPL_Error\":\"\"}\0");
    *size = 18;
  }
  return ret;
}

/*
 * {
 *  "comp_name": {
 *    "tele_name":
 *      {
 *        "tele_field1": value1   -
 *        ...                     - PAYLOAD --> JSON_Value
 *        "tele_fieldN": valueN   -
 *      }
 *  }
 * */
uint8_t PnPLSerializeTelemetry(char *compName, PnPLTelemetry_t *telemetryValue, uint8_t telemetryNum, char **telemetryJSON, uint32_t *size, uint8_t pretty)
{

  JSON_Value *tempJSON;
  JSON_Object *JSON_Telemetry;

  tempJSON = json_value_init_object();
  JSON_Telemetry = json_value_get_object(tempJSON);

  for(uint8_t i = 0; i < telemetryNum; i++)
  {
    char dotStr[(COMP_KEY_MAX_LENGTH * 2) + 1];
    (void)strcpy(dotStr, compName);
    uint8_t telemetry_type = telemetryValue[i].telemetry_type;
    if(telemetry_type != PNPL_TELEMETRY)
    {
      (void)strcat(dotStr, ".");
      (void)strcat(dotStr, telemetryValue[i].telemetry_name);
      switch(telemetry_type)
      {
        case PNPL_INT:
          (void)json_object_dotset_number(JSON_Telemetry, dotStr, (double)*(int*)(telemetryValue[i].telemetry_value));
          break;
        case PNPL_FLOAT:
          (void)json_object_dotset_number(JSON_Telemetry, dotStr, *(float*) (telemetryValue[i].telemetry_value));
          break;
        case PNPL_STRING:
          (void)json_object_dotset_string(JSON_Telemetry, dotStr, (const char*) telemetryValue[i].telemetry_value);
          break;
        case PNPL_BOOLEAN:
          (void)json_object_dotset_boolean(JSON_Telemetry, dotStr, *(int*) (telemetryValue[i].telemetry_value));
          break;
        default:
          break;
      }
    }
    else
    {
      char *sub_telemetries_json;
      uint32_t sub_telemetries_size;

      (void)PnPLSerializeTelemetry(telemetryValue[i].telemetry_name,
                             (PnPLTelemetry_t*) telemetryValue[i].telemetry_value,
                             telemetryValue[i].n_sub_telemetries,
                             &sub_telemetries_json,
                             &sub_telemetries_size,
                             0);
      (void)json_object_set_value(JSON_Telemetry, dotStr, json_parse_string(sub_telemetries_json));
    }
  }

  /* convert to a json string and write to file */
  if(pretty == 1u)
  {
    *telemetryJSON = json_serialize_to_string_pretty(tempJSON);
    *size = json_serialization_size_pretty(tempJSON);
  }
  else
  {
    *telemetryJSON = json_serialize_to_string(tempJSON);
    *size = json_serialization_size(tempJSON);
  }

  json_value_free(tempJSON);
  return PNPL_CMD_NO_ERROR_CODE;
}
