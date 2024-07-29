/**
  ******************************************************************************
  * @file    PnPLCompManager.c
  * @brief   The PnPLCompManager implements the interface used to handle PnP-like
  *          commands and properties generated through a Digital Twins Definition
  *          Language (DTDL)
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

#include <stdlib.h>
#include <stdio.h>
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
static uint8_t board_id;
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

/* Get BOARD Unique ID */
uint8_t PnPLGetBOARDID(void)
{
#ifndef BOARD_ID
  return board_id;
#else
  return BOARD_ID;
#endif
}

#ifndef FW_ID
/* Set FW Unique ID */
void PnPLSetFWID(uint8_t id)
{
  fw_id = id;
}
#endif

#ifndef BOARD_ID
/* Set BOARD Unique ID */
void PnPLSetBOARDID(uint8_t id)
{
  board_id = id;
}
#endif

PnPL_Malloc_Function prv_pnpl_malloc = malloc;
PnPL_Free_Function prv_pnpl_free = free;

PnPLockCallback pnpl_lock_fp = NULL;
PnPLUnlockCallback pnpl_unlock_fp = NULL;

/**
 * Private function declaration
 */
static uint8_t setTelemetryValue(uint8_t type, JSON_Object *json_obj, char *name, void *value,
                                 uint8_t n_sub_telemetries);
static uint8_t extract_PnPL_cmd_data(char *commandString, uint8_t *commandType, char *componentName);
static void PnPLGetUniqueID(char *id);

static uint8_t _PnPLGetPresentationJSON(char **serializedJSON, uint32_t *size);
static uint8_t _PnPLGetDeviceStatusJSON(char **serializedJSON, uint32_t *size, uint8_t pretty);
static uint8_t _PnPLUpdateDeviceStatusFromJSON(char *serializedJSON);
static uint8_t _PnPLParseCommand(char *commandString, PnPLCommand_t *command);

/**
 * Public function definition
 */

#ifdef pnpl_malloc
#error "pnpl_malloc already defined! Check your PnPLCompManager_Conf.h and remove it if present. Use PnPLSetAllocationFunctions() defined in this file to set your memory allocation functions."
#endif

#ifdef pnpl_free
#error "pnpl_free already defined! Check your PnPLCompManager_Conf.h and remove it if present. Use PnPLSetAllocationFunctions() defined in this file to set your memory allocation functions."
#endif

void *pnpl_malloc(size_t size){
  return prv_pnpl_malloc(size);
}

void pnpl_free(void *ptr){
  prv_pnpl_free(ptr);
}

/**
 * @brief Sets custom allocation functions for the PnPL library.
 *
 * @param malloc_fun Function pointer to the custom malloc function.
 * @param free_fun Function pointer to the custom free function.
 *
 * @note This function sets custom allocation functions for the PnPL library
 *       and ensures that they are set even if the BLE initialization fails.
 *       It also sets the allocation functions for the Parson library using
 *       the `json_set_allocation_functions` function.
 */
void PnPLSetAllocationFunctions(PnPL_Malloc_Function malloc_fun, PnPL_Free_Function free_fun)
{
  /* Parson allocation functions */
  json_set_allocation_functions(malloc_fun, free_fun);

  prv_pnpl_malloc = malloc_fun;
  prv_pnpl_free = free_fun;
}


/* Function to serialize a JSON object with a custom message based on the log type */
uint8_t PnPLCreateLogMessage(char **SerializedJSON, uint32_t *size, const char *message, PnPLLogType logType)
{
  if(SerializedJSON == NULL || size == NULL || message == NULL)
  {
    return PNPL_BASE_ERROR_CODE; // Return an error if any of the input pointers are NULL
  }

  // Determine the key based on the log type
  const char *key;
  switch(logType)
  {
    case PNPL_LOG_ERROR:
      key = "PnPL_Error";
      break;
    case PNPL_LOG_WARNING:
      key = "PnPL_Warning";
      break;
    case PNPL_LOG_INFO:
      key = "PnPL_Info";
      break;
    default:
      return PNPL_BASE_ERROR_CODE; // Return an error for an unrecognized log type
  }

  // Calculate the size needed for the JSON string with the message
  const char *jsonTemplate = "{\"%s\":\"%s\"}";
  *size = strlen(jsonTemplate) - 4 + strlen(key) + strlen(message) + 1; // -4 for the two %s, +1 for the null terminator

  *SerializedJSON = (char*) pnpl_malloc(*size); // Allocate memory for the JSON string

  if(*SerializedJSON == NULL)
  {
    return PNPL_BASE_ERROR_CODE; // Return an error if memory allocation failed
  }

  // Format the JSON string with the key and message
  sprintf(*SerializedJSON, jsonTemplate, key, message);

  return PNPL_NO_ERROR_CODE;
}

/**
 * @brief Sets custom lock and unlock callback for the PnPL library.
 *
 * @param lock_callback Function pointer to the custom lock function.
 * @param unlock_callback Function pointer to the custom unlock function.
 *
 */
uint8_t PnPL_SetLockUnlockCallbacks(PnPLockCallback lock_callback, PnPLUnlockCallback unlock_callback)
{
  uint8_t pnpn_RetVal = PNPL_NULL_PTR_ARG_CODE;

  if(NULL != lock_callback || NULL != unlock_callback)
  {
    pnpl_lock_fp   = lock_callback;
    pnpl_unlock_fp = unlock_callback;

    pnpn_RetVal = PNPL_NO_ERROR_CODE;
  }

  return pnpn_RetVal;
}

/**
  * @brief Generate and store the Acquisition UUID
  * @param None
  * @retval None
  */
void PnPLGenerateAcquisitionUUID(char *uuid)
{
  if(NULL != pnpl_lock_fp)
  {
    pnpl_lock_fp();
  }

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
  index += (uint8_t)sprintf(&global_uuid[index], "%04lx-%04lx-", (unsigned long)((UUID[1] >> 16) & 0xFFFFu),
                            (unsigned long)(UUID[1] & 0xFFFFu));
  index += (uint8_t)sprintf(&global_uuid[index], "%04lx-%04lx", (unsigned long)((UUID[2] >> 16) & 0xFFFFu),
                            (unsigned long)(UUID[2] & 0xFFFFu));
  index += (uint8_t)sprintf(&global_uuid[index], "%08lx", (unsigned long)UUID[3]);

  (void)strcpy(uuid, global_uuid);

  if(NULL != pnpl_unlock_fp)
  {
    pnpl_unlock_fp();
  }
}



uint8_t PnPLAddComponent(IPnPLComponent_t *pComponent)
{
  if(NULL != pnpl_lock_fp)
  {
    pnpl_lock_fp();
  }

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

  if(NULL != pnpl_unlock_fp)
  {
    pnpl_unlock_fp();
  }

  return id;
}

uint16_t PnPLGetNComponents(void)
{
  return spPnPLObj.n_components;
}

uint16_t PnPLGetComponentsNames(char **components_names)
{
  if(NULL != pnpl_lock_fp)
  {
    pnpl_lock_fp();
  }

  IPnPLComponent_t *p_obj;

  for (uint16_t i = 0; i < spPnPLObj.n_components; i++)
  {
    p_obj = (IPnPLComponent_t *)(spPnPLObj.Components[i]);
    (void)strcpy(components_names[i], IPnPLComponentGetKey(p_obj));
  }

  if(NULL != pnpl_unlock_fp)
  {
    pnpl_unlock_fp();
  }

  return spPnPLObj.n_components;
}



uint8_t PnPLGetComponentValue(char *comp_name, char **SerializedJSON, uint32_t *size, uint8_t pretty)
{
  uint8_t ret = PNPL_BASE_ERROR_CODE;
  JSON_Object *JSON_CompObject;
  JSON_Value *tempJSON;
  JSON_Value *tempJSON_noKey;
  char *comp_string = NULL;;
  uint8_t comp_found = 0;

  if(NULL != pnpl_lock_fp)
  {
    pnpl_lock_fp();
  }

  for (uint8_t i = 0; i < PnPLGetNComponents(); i++)
  {
    IPnPLComponent_t *p_obj = (IPnPLComponent_t *)(spPnPLObj.Components[i]);
    if (strcmp(comp_name, IPnPLComponentGetKey(p_obj)) == 0)
    {
      (void)IPnPLComponentGetStatus(p_obj, &comp_string, size, pretty);

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
    ret = PNPL_NO_ERROR_CODE;
  }

  if(NULL != pnpl_unlock_fp)
  {
    pnpl_unlock_fp();
  }

  return ret;
}

uint8_t PnPLGetPresentationJSON(char **serializedJSON, uint32_t *size)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_GetPresentation;

  if(NULL != pnpl_lock_fp)
  {
    pnpl_lock_fp();
  }

  tempJSON = json_value_init_object();
  JSON_GetPresentation = json_value_get_object(tempJSON);

#ifndef BOARD_ID
  (void)json_object_dotset_number(JSON_GetPresentation, "board_id", (float)board_id);
#else
  (void)json_object_dotset_number(JSON_GetPresentation, "board_id", (float)BOARD_ID);
#endif

#ifndef FW_ID
  (void)json_object_dotset_number(JSON_GetPresentation, "fw_id", (float)fw_id);
#else
  (void)json_object_dotset_number(JSON_GetPresentation, "fw_id", (float)FW_ID);
#endif

  *serializedJSON = json_serialize_to_string(tempJSON);
  *size = json_serialization_size(tempJSON);

  json_value_free(tempJSON);

  if(NULL != pnpl_unlock_fp)
  {
    pnpl_unlock_fp();
  }

  return PNPL_NO_ERROR_CODE;
}



uint8_t PnPLGetDeviceStatusJSON(char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  char serial_number[25];

  JSON_Value *tempJSON;
  JSON_Value *tempJSONDevice;
  JSON_Value *tempJSON1;
  JSON_Object *JSON_DeviceConfig;
  JSON_Object *JSON_Device;
  JSON_Array *JSON_DeviceArray;
  JSON_Array *JSON_ComponentArray;

  uint32_t sz_comp;
  char *ser_comp;

  if(NULL != pnpl_lock_fp)
  {
    pnpl_lock_fp();
  }

  tempJSON = json_value_init_object();
  JSON_DeviceConfig = json_value_get_object(tempJSON);

  /*
      "schema_version": "2.2.0",                 (Reference to the schema version adopted.)
      "uuid" : "<uuid-of-the-acquisition>",      (Unique identifier of an acquisition.)​
      "devices":[​
          {​
              "board_id": <board_id>,            (Unique identifier of a board type, according to Vespucci catalogs.)​
              "fw_id": <fw_id>,                  (Unique identifier of a firmware type, according to Vespucci catalogs.)
              "protocol_id": <protocol_id>       (Unique identifier of a protocol type: 0:BLE, 1:serial, 2:libusb)​​
              "sn": "<serial_number>",           (Unique serial number of the device that contributes to the acquisition.)​
              "pnpl_responses": true|false       (Optional - Boolean value that identifies whether PnPL Responses are used or not)[*1]​
              "pnpl_ble_responses": true|false   (Optional - Boolean value that identifies whether BLE PnPL Responses are used or not)[*1]​
              "components": [                    (Array of components of the device; this list has to reflect the device template.)​
                  {​
                      "device_info": {           (Optional - The "device_info" component contains additional information about the device.)​
                          "alias": "<alias>",​
                          "fw_name": "<name-of-the-firmware>",​
                          "fw_version": "<version-of-the-firmware>",​
                          "part_number": "<part-number>"​
                      }​
                  }​
              ],​
          }​
      ]

      [*1]: [Backward compatibility guaranteed] If this field is not present, then PnPL responses || BLE PnPL responses are not used.​​
  */

  (void)json_object_dotset_string(JSON_DeviceConfig, "schema_version", "2.2.0");
  (void)json_object_dotset_string(JSON_DeviceConfig, "uuid", global_uuid);

  tempJSONDevice = json_value_init_object();
  JSON_Device = json_value_get_object(tempJSONDevice);

  (void)json_object_set_value(JSON_DeviceConfig, "devices", json_value_init_array());
  JSON_DeviceArray = json_object_dotget_array(JSON_DeviceConfig, "devices");
  (void)json_array_append_value(JSON_DeviceArray, tempJSONDevice);

#ifndef BOARD_ID
  (void)json_object_dotset_number(JSON_Device, "board_id", (float)board_id);
#else
  (void)json_object_dotset_number(JSON_Device, "board_id", (float)BOARD_ID);
#endif

#ifndef FW_ID
  (void)json_object_dotset_number(JSON_Device, "fw_id", (float)fw_id);
#else
  (void)json_object_dotset_number(JSON_Device, "fw_id", (float)FW_ID);
#endif

  (void)json_object_dotset_number(JSON_Device, "protocol_id", 2); /* 0: BLE, 1: serial, 2:libusb */

  PnPLGetUniqueID(serial_number);
  (void)json_object_dotset_string(JSON_Device, "sn", serial_number);

#ifdef PNPL_RESPONSES
  (void)json_object_dotset_boolean(JSON_Device, "pnpl_responses", true);
#else
  (void)json_object_dotset_boolean(JSON_Device, "pnpl_responses", false);
#endif

#ifdef PNPL_BLE_RESPONSES
  (void)json_object_dotset_boolean(JSON_Device, "pnpl_ble_responses", true);
#else
  (void)json_object_dotset_boolean(JSON_Device, "pnpl_ble_responses", false);
#endif

  (void)json_object_set_value(JSON_Device, "components", json_value_init_array());
  JSON_ComponentArray = json_object_dotget_array(JSON_Device, "components");

  for (uint8_t i = 0; i < PnPLGetNComponents(); i++)
  {
    IPnPLComponent_t *p_obj = spPnPLObj.Components[i];
    sz_comp = 0;
    ser_comp = NULL;
    (void)IPnPLComponentGetStatus(p_obj, &ser_comp, &sz_comp, 0);
    tempJSON1 = json_parse_string(ser_comp);
    (void)json_array_append_value(JSON_ComponentArray, tempJSON1);
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

  if(NULL != pnpl_unlock_fp)
  {
    pnpl_unlock_fp();
  }

  return PNPL_NO_ERROR_CODE;
}



uint8_t PnPLGetFilteredDeviceStatusJSON(char **serializedJSON, uint32_t *size, char **skip_list,
                                        uint32_t skip_list_size, uint8_t pretty)
{
  char serial_number[25];
  JSON_Value *tempJSON;
  JSON_Value *tempJSONDevice;
  JSON_Value *tempJSON1;
  JSON_Object *JSON_DeviceConfig;
  JSON_Object *JSON_Device;
  JSON_Array *JSON_DeviceArray;
  JSON_Array *JSON_ComponentArray;

  uint32_t sz_comp;
  char *ser_comp;

  if(NULL != pnpl_lock_fp)
  {
    pnpl_lock_fp();
  }

  tempJSON = json_value_init_object();
  JSON_DeviceConfig = json_value_get_object(tempJSON);

  /*
      "schema_version": "2.2.0",                 (Reference to the schema version adopted.)
      "uuid" : "<uuid-of-the-acquisition>",      (Unique identifier of an acquisition.)​
      "devices":[​
          {​
              "board_id": <board_id>,            (Unique identifier of a board type, according to Vespucci catalogs.)​
              "fw_id": <fw_id>,                  (Unique identifier of a firmware type, according to Vespucci catalogs.)
              "protocol_id": <protocol_id>       (Unique identifier of a protocol type: 0:BLE, 1:serial, 2:libusb)​​
              "sn": "<serial_number>",           (Unique serial number of the device that contributes to the acquisition.)​
              "pnpl_responses": true|false       (Optional - Boolean value that identifies whether PnPL Responses are used or not)[*1]
              "pnpl_ble_responses": true|false   (Optional - Boolean value that identifies whether BLE PnPL Responses are used or not)[*1]​
              "components": [                    (Array of components of the device; this list has to reflect the device template.)​
                  {​
                      "device_info": {           (Optional - The "device_info" component contains additional information about the device.)​
                          "alias": "<alias>",​
                          "fw_name": "<name-of-the-firmware>",​
                          "fw_version": "<version-of-the-firmware>",​
                          "part_number": "<part-number>"​
                      }​
                  }​
              ],​
          }​
      ]

      [*1]: [Backward compatibility guaranteed] If this field is not present, then PnPL responses || BLE PnPL responses are not used.​​
  */

  (void)json_object_dotset_string(JSON_DeviceConfig, "schema_version", "2.2.0");
  (void)json_object_dotset_string(JSON_DeviceConfig, "uuid", global_uuid);

  tempJSONDevice = json_value_init_object();
  JSON_Device = json_value_get_object(tempJSONDevice);

  (void)json_object_set_value(JSON_DeviceConfig, "devices", json_value_init_array());
  JSON_DeviceArray = json_object_dotget_array(JSON_DeviceConfig, "devices");
  (void)json_array_append_value(JSON_DeviceArray, tempJSONDevice);

#ifndef BOARD_ID
  (void)json_object_dotset_number(JSON_Device, "board_id", (float)board_id);
#else
  (void)json_object_dotset_number(JSON_Device, "board_id", (float)BOARD_ID);
#endif

#ifndef FW_ID
  (void)json_object_dotset_number(JSON_Device, "fw_id", (float)fw_id);
#else
  (void)json_object_dotset_number(JSON_Device, "fw_id", (float)FW_ID);
#endif

  (void)json_object_dotset_number(JSON_Device, "protocol_id", 2); /* 0: BLE, 1: serial, 2:libusb */

  PnPLGetUniqueID(serial_number);
  (void)json_object_dotset_string(JSON_Device, "sn", serial_number);

#ifdef PNPL_RESPONSES
  (void)json_object_dotset_boolean(JSON_Device, "pnpl_responses", true);
#else
  (void)json_object_dotset_boolean(JSON_Device, "pnpl_responses", false);
#endif

#ifdef PNPL_BLE_RESPONSES
  (void)json_object_dotset_boolean(JSON_Device, "pnpl_ble_responses", true);
#else
  (void)json_object_dotset_boolean(JSON_Device, "pnpl_ble_responses", false);
#endif

  (void)json_object_set_value(JSON_Device, "components", json_value_init_array());
  JSON_ComponentArray = json_object_dotget_array(JSON_Device, "components");

  bool skip = false;
  for (uint8_t i = 0; i < PnPLGetNComponents(); i++)
  {
    IPnPLComponent_t *p_obj = spPnPLObj.Components[i];
    for (uint8_t y = 0; y < skip_list_size; y++)
    {
      if (strcmp(IPnPLComponentGetKey(p_obj), skip_list[y]) == 0)
      {
        skip = true;
      }
    }
    if (!skip)
    {
      sz_comp = 0;
      ser_comp = NULL;
      (void)IPnPLComponentGetStatus(p_obj, &ser_comp, &sz_comp, 0);
      tempJSON1 = json_parse_string(ser_comp);
      (void)json_array_append_value(JSON_ComponentArray, tempJSON1);
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

  if(NULL != pnpl_unlock_fp)
  {
    pnpl_unlock_fp();
  }

  return PNPL_NO_ERROR_CODE;
}

/**
 * @brief Updates the device status from a serialized JSON string.
 *
 * This function takes a serialized JSON string as input and updates the device status
 * based on the information in the JSON.
 *
 * @param serializedJSON A pointer to the serialized JSON string.
 * @return The status of the update operation. This can be used to determine if the update
 *         was successful or if an error occurred.
 */
uint8_t PnPLUpdateDeviceStatusFromJSON(char *serializedJSON)
{
  char componentName[COMP_KEY_MAX_LENGTH];

  JSON_Array *JSON_components, *JSON_devices;
  JSON_Object *component, *device;
  JSON_Value *component_value;

  if(NULL != pnpl_lock_fp)
  {
    pnpl_lock_fp();
  }

  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *JSON_ParseHandler = json_value_get_object(tempJSON);
  uint32_t components_number;

  if (json_object_has_value(JSON_ParseHandler, "devices") == 1)
  {
    JSON_devices = json_object_get_array(JSON_ParseHandler, "devices");
    device = json_array_get_object(JSON_devices, 0);
    JSON_components = json_object_get_array(device, "components");
    components_number = json_array_get_count(JSON_components);
  }
  else
  {
    /* old status version (without "device" key) */
    JSON_components = json_object_get_array(JSON_ParseHandler, "");
    components_number = json_array_get_count(JSON_components);
  }

  for (uint32_t i = 0; i < components_number; i++)
  {
    component = json_array_get_object(JSON_components, i);
    (void)strcpy(componentName, json_object_get_name(component, 0));
    for (uint8_t j = 0; j < PnPLGetNComponents(); j++)
    {
      IPnPLComponent_t *p_obj = (IPnPLComponent_t *)(spPnPLObj.Components[j]);
      if (strcmp(componentName, IPnPLComponentGetKey(p_obj)) == 0)
      {
        component_value = json_object_get_value(component, componentName);
        char *comp_string = json_serialize_to_string(json_value_get_parent(component_value));

        PnPLCommand_t pnpl_command;
        (void)_PnPLParseCommand(comp_string, &pnpl_command);

        json_free_serialized_string(comp_string);
        break;
      }
    }
  }

  json_value_free(tempJSON);

  if(NULL != pnpl_unlock_fp)
  {
    pnpl_unlock_fp();
  }

  return PNPL_NO_ERROR_CODE;
}


/**
 * @brief Parses a command string and populates a PnPLCommand_t structure.
 *
 * This function takes a command string and a pointer to a PnPLCommand_t structure,
 * and parses the command string to populate the structure with the relevant command
 * information. The command string should be in a specific format that matches the
 * expected command format.
 *
 * @param commandString The command string to parse.
 * @param command A pointer to the PnPLCommand_t structure to populate.
 * @return uint8_t Returns a status code indicating the success or failure of the parsing operation.
 */
uint8_t PnPLParseCommand(char *commandString, PnPLCommand_t *command)
{
  if(NULL != pnpl_lock_fp)
  {
    pnpl_lock_fp();
  }

  uint8_t commandType = 0;
  char componentName[2 *
                     COMP_KEY_MAX_LENGTH]; /* 2* because this could be a comp or a comm key. If comm_key this is in the form (comp_key*comm_key) */
  uint8_t ret = extract_PnPL_cmd_data(commandString, &commandType, componentName);

  command->comm_type = commandType;
  (void)strcpy(command->comp_name, componentName);

  if (ret == PNPL_NO_ERROR_CODE)
  {
    if (commandType == PNPL_CMD_SET)
    {
      /* Select right parse/update function */
      for (uint8_t i = 0; i < PnPLGetNComponents(); i++)
      {
        IPnPLComponent_t *p_obj = (IPnPLComponent_t *)(spPnPLObj.Components[i]);
        if (strcmp(componentName, IPnPLComponentGetKey(p_obj)) == 0)
        {
#if defined(PNPL_RESPONSES) || defined(PNPL_BLE_RESPONSES)
          char *set_response = 0;
          uint32_t size = 0;
          (void)IPnPLComponentSetProperty(p_obj, commandString, &set_response, &size, 0);
          /* SET Response */
          command->response = (char*)pnpl_malloc(size);
          if (command->response != NULL)
          {
            strcpy(command->response, set_response);
            pnpl_free(set_response);
          }
          /* SET Response*/
#else
          (void)IPnPLComponentSetProperty(p_obj, commandString);
#endif
        }
        uint8_t nOfCommands = IPnPLComponentGetNCommands(p_obj); // 0 if none
        if (nOfCommands > 0u)
        {
          for (uint8_t j = 0; j < nOfCommands; j++)
          {
            if (strcmp(IPnPLComponentGetCommandKey(p_obj, j), componentName) == 0)
            {
#if defined(PNPL_RESPONSES) || defined(PNPL_BLE_RESPONSES)
              char *cmd_response = 0;
              uint32_t size = 0;
              (void)IPnPLCommandExecuteFunction(p_obj, commandString, &cmd_response, &size, 0);
//              /* CMD Response */
              command->comm_type = PNPL_CMD_COMMAND;
//              command->response = (char*)pnpl_malloc(size);
//              if (command->response != NULL)
//              {
//                strcpy(command->response, cmd_response);
//                pnpl_free(cmd_response);
//              }
//              /* CMD Response*/
#else
              (void)IPnPLCommandExecuteFunction(p_obj, commandString);
#endif
            }
          }
        }
      }
    }
    if (commandType == PNPL_CMD_SYSTEM_CONFIG)
    {
      for (uint8_t i = 0; i < PnPLGetNComponents(); i++)
      {
        IPnPLComponent_t *p_obj = (IPnPLComponent_t *)(spPnPLObj.Components[i]);
        if (strcmp(componentName, IPnPLComponentGetKey(p_obj)) == 0)
        {
#if defined(PNPL_RESPONSES) || defined(PNPL_BLE_RESPONSES)
          char *set_response = 0;
          uint32_t size = 0;
          (void)IPnPLComponentSetProperty(p_obj, commandString, &set_response, &size, 0);
          /* SET Response */
          command->response = (char*)pnpl_malloc(size);
          if (command->response != NULL)
          {
            strcpy(command->response, set_response);
            pnpl_free(set_response);
          }
          /* SET Response*/
#else
          (void)IPnPLComponentSetProperty(p_obj, commandString);
#endif
        }
      }
    }
    if (commandType == PNPL_CMD_UPDATE_DEVICE)
    {
      (void)PnPLUpdateDeviceStatusFromJSON(commandString);
    }
  }
  else
  {
    command->comm_type = PNPL_CMD_ERROR;
  }

  if(NULL != pnpl_unlock_fp)
  {
    pnpl_unlock_fp();
  }

  return ret;
}

/**
 * @brief Serializes a PnPL command response into a JSON string.
 *
 * This function takes a PnPL command and serializes it into a JSON string representation.
 *
 * @param command Pointer to the PnPL command structure.
 * @param SerializedJSON Pointer to the variable that will hold the serialized JSON string.
 *                       The string is allocated within the function and it should be freed after use.
 * @param size Pointer to the variable that will hold the size of the serialized JSON string.
 * @param pretty Flag indicating whether the JSON string should be formatted for readability.
 *               Set to 1 for pretty formatting, or 0 for compact formatting.
 *
 * @return 0 if serialization is successful, or an error code if serialization fails.
 */
uint8_t PnPLSerializeResponse(PnPLCommand_t *command, char **SerializedJSON, uint32_t *size, uint8_t pretty)
{
  uint8_t ret = PNPL_NO_ERROR_CODE;

  if(NULL != pnpl_lock_fp)
  {
    pnpl_lock_fp();
  }

  if (command->comm_type == PNPL_CMD_SYSTEM_INFO)
  {
    (void)_PnPLGetPresentationJSON(SerializedJSON, size);
  }
  else if (command->comm_type == PNPL_CMD_GET)
  {
    uint16_t comp_found = 0;
    if (strcmp(command->comp_name, "all") == 0)
    {
      (void)_PnPLGetDeviceStatusJSON(SerializedJSON, size, pretty);
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
      PnPLCreateLogMessage(SerializedJSON, size, "", PNPL_LOG_ERROR);
      ret = PNPL_BASE_ERROR_CODE;
    }
  }
#if defined(PNPL_RESPONSES) || defined(PNPL_BLE_RESPONSES)
  else if (command->comm_type == PNPL_CMD_SET)
  {
    *size = strlen(command->response) + 1;
    *SerializedJSON = (char*)pnpl_malloc(*size);
    (void)strcpy(*SerializedJSON, command->response);
    pnpl_free(command->response);
  }
  else if (command->comm_type == PNPL_CMD_COMMAND)
  {
    *size = strlen(command->response) + 1;
    *SerializedJSON = (char*)pnpl_malloc(*size);
    (void)strcpy(*SerializedJSON, command->response);
    pnpl_free(command->response);
  }
#endif
  else if (command->comm_type == PNPL_CMD_ERROR)
  {
    PnPLCreateLogMessage(SerializedJSON, size, command->comp_name, PNPL_LOG_ERROR);
  }
  else
  {
    /* nothing to do */
  }

  if(NULL != pnpl_unlock_fp)
  {
    pnpl_unlock_fp();
  }

  return ret;
}

/**
 * @brief Frees the memory allocated for a serialized string.
 *
 * This function is used to free the memory allocated for a serialized string.
 *
 * @param string Pointer to the serialized string to be freed.
 */
void PnPLFreeSerializedString(char *string)
{

  if(NULL != pnpl_lock_fp)
  {
    pnpl_lock_fp();
  }

  json_free_serialized_string(string);

  if(NULL != pnpl_unlock_fp)
  {
    pnpl_unlock_fp();
  }
}


/**
 * @brief Serializes telemetry data into JSON format.
 *
 * This function takes the component name, telemetry values, telemetry count, and other parameters
 * to serialize the telemetry data into a JSON string. The serialized JSON string is stored in the
 * `telemetryJSON` parameter, and the size of the JSON string is stored in the `size` parameter.
 *
 * @param compName The name of the component.
 * @param telemetryValue The telemetry values to be serialized.
 * @param telemetryNum The number of telemetry values.
 * @param telemetryJSON Pointer to store the serialized JSON string.
 * @param size Pointer to store the size of the serialized JSON string.
 * @param pretty Flag indicating whether the JSON string should be formatted for readability.
 *               Set to 1 for pretty formatting, or 0 for compact formatting.
 * @return Returns 0 on success, or a non-zero value on failure.
 */
uint8_t PnPLSerializeTelemetry(char *compName, PnPLTelemetry_t *telemetryValue, uint8_t telemetryNum, char **telemetryJSON, uint32_t *size, uint8_t pretty)
{
  if(NULL != pnpl_lock_fp)
  {
    pnpl_lock_fp();
  }

  JSON_Value *root_value = json_value_init_object();
  JSON_Object *root_object = json_value_get_object(root_value);

  JSON_Value *telemetry_value = json_value_init_object();
  JSON_Object *telemetry_object = json_value_get_object(telemetry_value);

  for (uint8_t i = 0; i < telemetryNum; i++)
  {
    uint8_t telemetry_type = telemetryValue[i].telemetry_type;
    (void)setTelemetryValue(telemetry_type, telemetry_object, telemetryValue[i].telemetry_name, telemetryValue[i].telemetry_value,
                      telemetryValue[i].n_sub_telemetries);
  }

  (void)json_object_set_value(root_object, compName, telemetry_value);

  /* convert to a json string and write to file */
  if (pretty == 1u)
  {
    *telemetryJSON = json_serialize_to_string_pretty(root_value);
    *size = json_serialization_size_pretty(root_value);
  }
  else
  {
    *telemetryJSON = json_serialize_to_string(root_value);
    *size = json_serialization_size(root_value);
  }

  json_value_free(root_value);

  if(NULL != pnpl_unlock_fp)
  {
    pnpl_unlock_fp();
  }
  return PNPL_NO_ERROR_CODE;
}

/**
 * @brief Serializes a command response into a JSON string.
 *
 * This function takes a message and status and serializes them into a JSON string
 * representing a command response. The resulting JSON string is stored in the
 * `responseJSON` parameter, and the size of the string is stored in the `size`
 * parameter.
 *
 * @param responseJSON Pointer to a pointer that will store the resulting JSON string.
 * @param size Pointer to a variable that will store the size of the resulting JSON string.
 * @param pretty Flag indicating whether the resulting JSON string should be formatted for readability.
 * @param message The message to be included in the command response.
 * @param status The status of the command response (true for success, false for failure).
 *
 * @return The function returns 0 on success, or a non-zero value on failure.
 */
uint8_t PnPLSerializeCommandResponse(char **responseJSON, uint32_t *size, uint8_t pretty, const char *message, bool status)
{
  if(NULL != pnpl_lock_fp)
  {
    pnpl_lock_fp();
  }

  JSON_Value *respJSON = json_value_init_object();
  JSON_Object *respJSONObject = json_value_get_object(respJSON);

  json_object_dotset_string(respJSONObject, "PnPL_Response.message", message);
  json_object_dotset_boolean(respJSONObject, "PnPL_Response.status", status);

  if (pretty == 1)
  {
    *responseJSON = json_serialize_to_string_pretty(respJSON);
    *size = json_serialization_size_pretty(respJSON);
  }
  else
  {
    *responseJSON = json_serialize_to_string(respJSON);
    *size = json_serialization_size(respJSON);
  }

  json_value_free(respJSON);

  if(NULL != pnpl_unlock_fp)
  {
    pnpl_unlock_fp();
  }
  return PNPL_NO_ERROR_CODE;
}

/**
 * Private function definition
 */


/**
 * @brief Extracts PnPL command data from the given command string.
 *
 * This function parses the command string and extracts the command type and component name.
 *
 * @param commandString The command string to extract data from.
 * @param commandType Pointer to a variable to store the extracted command type.
 * @param componentName Pointer to a character array to store the extracted component name.
 * 
 * @return The function returns 0 on success, or a non-zero value on failure.
 */
static uint8_t extract_PnPL_cmd_data(char *commandString, uint8_t *commandType, char *componentName)
{
  JSON_Value *tempJSON = json_parse_string(commandString);
  if (tempJSON != NULL)
  {
    JSON_Object *tempJSONObject = json_value_get_object(tempJSON);
    (void)strcpy(componentName, json_object_get_name(tempJSONObject, 0));
    for (uint8_t i = 0; i < PnPLGetNComponents(); i++)
    {
      IPnPLComponent_t *p_obj = (IPnPLComponent_t *)(spPnPLObj.Components[i]);
      if (strcmp(IPnPLComponentGetKey(p_obj), componentName) == 0)
      {
        /* Check if extracted string is a component added to the current FW */
        *commandType = PNPL_CMD_SET;
        json_value_free(tempJSON);
        return PNPL_NO_ERROR_CODE;
      }
      /* in case of control commands */
      uint8_t nOfCommands = IPnPLComponentGetNCommands(p_obj);
      if (nOfCommands > 0u)
      {
        for (uint8_t j = 0; j < nOfCommands; j++)
        {
          if (strcmp(IPnPLComponentGetCommandKey(p_obj, j), componentName) == 0)
          {
            *commandType = PNPL_CMD_SET;
            json_value_free(tempJSON);
            return PNPL_NO_ERROR_CODE;
          }
        }
      }
    }

    if (strcmp(componentName, "get_status") == 0)
    {
      *commandType = PNPL_CMD_GET;
      (void)strcpy(componentName, json_object_get_string(tempJSONObject, "get_status"));
      json_value_free(tempJSON);
      return PNPL_NO_ERROR_CODE;
    }
    else if (strcmp(componentName, "update_device_status") == 0)
    {
      *commandType = PNPL_CMD_UPDATE_DEVICE;
      JSON_Value *tempJSONValue = json_object_get_value(tempJSONObject, "update_device_status");
      (void)strcpy(commandString, json_serialize_to_string(tempJSONValue));
      json_value_free(tempJSONValue);
      return PNPL_NO_ERROR_CODE;
    }
    else if (strcmp(componentName, "system_config") == 0)
    {
      *commandType = PNPL_CMD_SYSTEM_CONFIG;
      JSON_Object *tempJSONObject2 = json_object_get_object(tempJSONObject, "system_config");
      (void)strcpy(componentName, json_object_get_string(tempJSONObject2, "comp_name"));
      json_value_free(tempJSON);
      return PNPL_NO_ERROR_CODE;
    }
    else if (strcmp(componentName, "system_info") == 0)/* NOTE Used for OLD get_presentation command */
    {
      *commandType = PNPL_CMD_SYSTEM_INFO;
      (void)strcpy(componentName, "");
      json_value_free(tempJSON);
      return PNPL_NO_ERROR_CODE;
    }
    else if (strcmp(componentName, "get_presentation") == 0)/* NOTE New get_presentation command */
    {
      *commandType = PNPL_CMD_SYSTEM_INFO;
      (void)strcpy(componentName, "");
      json_value_free(tempJSON);
      return PNPL_NO_ERROR_CODE;
    }
    else if (strcmp(componentName, "get_identity") == 0)/* NOTE New get_identity command (Asked from SW-Platform) */
    {
      *commandType = PNPL_CMD_SYSTEM_INFO;
      (void)strcpy(componentName, "");
      json_value_free(tempJSON);
      return PNPL_NO_ERROR_CODE;
    }
    else
    {
      /* nothing to do */
    }
  }
  json_value_free(tempJSON);
  /* Not JSON command! */
  return PNPL_BASE_ERROR_CODE;
}

/**
 * @brief Retrieves the unique ID.
 *
 * This function retrieves the unique ID and stores it in the provided buffer.
 * The unique ID is derived from the STM32 UID and converted to a string.
 *
 * @param id Pointer to the buffer where the unique ID will be stored (minimum 25 bytes).
 */
static void PnPLGetUniqueID(char *id)
{
  uint32_t stm32_UID[3];

  stm32_UID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
  stm32_UID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
  stm32_UID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));

  (void)sprintf(id, "%08lX%08lX%08lX", (unsigned long)stm32_UID[0], (unsigned long)stm32_UID[1],
                (unsigned long)stm32_UID[2]);
}

/**
 * @brief Sets the telemetry value in the JSON object.
 *
 * This function sets the telemetry value in the JSON object based on the telemetry type.
 *
 * @param type The telemetry data type.
 * @param json_obj The JSON object to set the telemetry value in.
 * @param name The name of the telemetry value.
 * @param value The telemetry value.
 * @param n_sub_telemetries The number of sub-telemetries.
 * @return The function returns 0 on success, or a non-zero value on failure.
 */
static uint8_t setTelemetryValue(uint8_t type, JSON_Object *json_obj, char *name, void *value,
                                 uint8_t n_sub_telemetries)
{
  switch (type)
  {
    case PNPL_INT:
      (void)json_object_dotset_number(json_obj, name, (double) * (int *) value);
      break;
    case PNPL_FLOAT:
      (void)json_object_dotset_number(json_obj, name, *(float *) value);
      break;
    case PNPL_STRING:
      (void)json_object_dotset_string(json_obj, name, (const char *) value);
      break;
    case PNPL_BOOLEAN:
      (void)json_object_dotset_boolean(json_obj, name, *(int *) value);
      break;
    case PNPL_TELEMETRY:
      for (uint8_t j = 0; j < n_sub_telemetries; j++)
      {
        uint8_t sub_telemetry_type = ((PnPLTelemetry_t *) value)[j].telemetry_type;
        char dotStr[(COMP_KEY_MAX_LENGTH * 2) + 1];
        (void)strcpy(dotStr, name);
        (void)strcat(dotStr, ".");
        (void)strcat(dotStr, ((PnPLTelemetry_t *) value)[j].telemetry_name);
        uint8_t n_sub_sub_telemetries = ((PnPLTelemetry_t *) value)[j].n_sub_telemetries;
        (void)setTelemetryValue(sub_telemetry_type, json_obj, dotStr, ((PnPLTelemetry_t *) value)[j].telemetry_value,
                          n_sub_sub_telemetries);
      }
      break;
    default:
      break;
  }
  return PNPL_NO_ERROR_CODE;
}


/**
 * @brief Retrieves the presentation JSON including board and firmware IDs.
 *
 * @param[out] serializedJSON Pointer to a pointer that will store the serialized JSON.
 *                            The string is allocated within the function and should be freed after use.
 * @param[out] size Pointer to a variable that will store the size of the serialized JSON.
 */
static uint8_t _PnPLGetPresentationJSON(char **serializedJSON, uint32_t *size)
{
  JSON_Value *tempJSON;
  JSON_Object *JSON_GetPresentation;

  tempJSON = json_value_init_object();
  JSON_GetPresentation = json_value_get_object(tempJSON);

#ifndef BOARD_ID
  (void)json_object_dotset_number(JSON_GetPresentation, "board_id", (float)board_id);
#else
  (void)json_object_dotset_number(JSON_GetPresentation, "board_id", (float)BOARD_ID);
#endif

#ifndef FW_ID
  (void)json_object_dotset_number(JSON_GetPresentation, "fw_id", (float)fw_id);
#else
  (void)json_object_dotset_number(JSON_GetPresentation, "fw_id", (float)FW_ID);
#endif

  *serializedJSON = json_serialize_to_string(tempJSON);
  *size = json_serialization_size(tempJSON);

  json_value_free(tempJSON);
  return PNPL_NO_ERROR_CODE;
}

/**
 * @brief Retrieves the device status in JSON format.
 *
 * This function retrieves the device status and serializes it into a JSON string.
 *
 * @param serializedJSON Pointer to a char pointer that will store the serialized JSON string.
 *                       The string is allocated within the function and should be freed after use.
 * @param size Pointer to a uint32_t variable that will store the size of the serialized JSON string.
 * @param pretty Flag indicating whether the JSON string should be formatted for readability (1) or not (0).
 *
 * @return 0 if successful, non-zero otherwise.
 */
static uint8_t _PnPLGetDeviceStatusJSON(char **serializedJSON, uint32_t *size, uint8_t pretty)
{
  char serial_number[25];

  JSON_Value *tempJSON;
  JSON_Value *tempJSONDevice;
  JSON_Value *tempJSON1;
  JSON_Object *JSON_DeviceConfig;
  JSON_Object *JSON_Device;
  JSON_Array *JSON_DeviceArray;
  JSON_Array *JSON_ComponentArray;

  uint32_t sz_comp;
  char *ser_comp;

  tempJSON = json_value_init_object();
  JSON_DeviceConfig = json_value_get_object(tempJSON);

  /*
      "schema_version": "2.2.0",                 (Reference to the schema version adopted.)
      "uuid" : "<uuid-of-the-acquisition>",      (Unique identifier of an acquisition.)​
      "devices":[​
          {​
              "board_id": <board_id>,            (Unique identifier of a board type, according to Vespucci catalogs.)​
              "fw_id": <fw_id>,                  (Unique identifier of a firmware type, according to Vespucci catalogs.)
              "protocol_id": <protocol_id>       (Unique identifier of a protocol type: 0:BLE, 1:serial, 2:libusb)​​
              "sn": "<serial_number>",           (Unique serial number of the device that contributes to the acquisition.)​
              "pnpl_responses": true|false       (Optional - Boolean value that identifies whether PnPL Responses are used or not)[*1]
              "pnpl_ble_responses": true|false   (Optional - Boolean value that identifies whether BLE PnPL Responses are used or not)[*1]​​
              "components": [                    (Array of components of the device; this list has to reflect the device template.)​
                  {​
                      "device_info": {           (Optional - The "device_info" component contains additional information about the device.)​
                          "alias": "<alias>",​
                          "fw_name": "<name-of-the-firmware>",​
                          "fw_version": "<version-of-the-firmware>",​
                          "part_number": "<part-number>"​
                      }​
                  }​
              ],​
          }​
      ]

      [*1]: [Backward compatibility guaranteed] If this field is not present, then PnPL responses are not used.​​
  */

  (void)json_object_dotset_string(JSON_DeviceConfig, "schema_version", "2.2.0");
  (void)json_object_dotset_string(JSON_DeviceConfig, "uuid", global_uuid);

  tempJSONDevice = json_value_init_object();
  JSON_Device = json_value_get_object(tempJSONDevice);

  (void)json_object_set_value(JSON_DeviceConfig, "devices", json_value_init_array());
  JSON_DeviceArray = json_object_dotget_array(JSON_DeviceConfig, "devices");
  (void)json_array_append_value(JSON_DeviceArray, tempJSONDevice);

#ifndef BOARD_ID
  (void)json_object_dotset_number(JSON_Device, "board_id", (float)board_id);
#else
  (void)json_object_dotset_number(JSON_Device, "board_id", (float)BOARD_ID);
#endif

#ifndef FW_ID
  (void)json_object_dotset_number(JSON_Device, "fw_id", (float)fw_id);
#else
  (void)json_object_dotset_number(JSON_Device, "fw_id", (float)FW_ID);
#endif

  (void)json_object_dotset_number(JSON_Device, "protocol_id", 2); /* 0: BLE, 1: serial, 2:libusb */

  PnPLGetUniqueID(serial_number);
  (void)json_object_dotset_string(JSON_Device, "sn", serial_number);

#ifdef PNPL_RESPONSES
  (void)json_object_dotset_boolean(JSON_Device, "pnpl_responses", true);
#else
  (void)json_object_dotset_boolean(JSON_Device, "pnpl_responses", false);
#endif

#ifdef PNPL_BLE_RESPONSES
  (void)json_object_dotset_boolean(JSON_Device, "pnpl_ble_responses", true);
#else
  (void)json_object_dotset_boolean(JSON_Device, "pnpl_ble_responses", false);
#endif

  (void)json_object_set_value(JSON_Device, "components", json_value_init_array());
  JSON_ComponentArray = json_object_dotget_array(JSON_Device, "components");

  for (uint8_t i = 0; i < PnPLGetNComponents(); i++)
  {
    IPnPLComponent_t *p_obj = spPnPLObj.Components[i];
    sz_comp = 0;
    ser_comp = NULL;
    (void)IPnPLComponentGetStatus(p_obj, &ser_comp, &sz_comp, 0);
    tempJSON1 = json_parse_string(ser_comp);
    (void)json_array_append_value(JSON_ComponentArray, tempJSON1);
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
  return PNPL_NO_ERROR_CODE;
}

/**
 * @brief Updates the device status from a serialized JSON string.
 *
 * This function takes a serialized JSON string as input and updates the device status
 * accordingly. The device status is updated based on the information extracted from
 * the JSON string.
 *
 * @param serializedJSON The serialized JSON string containing the device status information.
 * @return The status of the update operation. Returns 0 if the update was successful, or
 *         a non-zero value indicating an error occurred.
 */
static uint8_t _PnPLUpdateDeviceStatusFromJSON(char *serializedJSON)
{
  char componentName[COMP_KEY_MAX_LENGTH];

  JSON_Array *JSON_components, *JSON_devices;
  JSON_Object *component, *device;
  JSON_Value *component_value;

  JSON_Value *tempJSON = json_parse_string(serializedJSON);
  JSON_Object *JSON_ParseHandler = json_value_get_object(tempJSON);
  uint32_t components_number;

  if (json_object_has_value(JSON_ParseHandler, "devices") == 1)
  {
    JSON_devices = json_object_get_array(JSON_ParseHandler, "devices");
    device = json_array_get_object(JSON_devices, 0);
    JSON_components = json_object_get_array(device, "components");
    components_number = json_array_get_count(JSON_components);
  }
  else
  {
    /* old status version (without "device" key) */
    JSON_components = json_object_get_array(JSON_ParseHandler, "");
    components_number = json_array_get_count(JSON_components);
  }

  for (uint32_t i = 0; i < components_number; i++)
  {
    component = json_array_get_object(JSON_components, i);
    (void)strcpy(componentName, json_object_get_name(component, 0));
    for (uint8_t j = 0; j < PnPLGetNComponents(); j++)
    {
      IPnPLComponent_t *p_obj = (IPnPLComponent_t *)(spPnPLObj.Components[j]);
      if (strcmp(componentName, IPnPLComponentGetKey(p_obj)) == 0)
      {
        component_value = json_object_get_value(component, componentName);
        char *comp_string = json_serialize_to_string(json_value_get_parent(component_value));

        PnPLCommand_t pnpl_command;
        (void)_PnPLParseCommand(comp_string, &pnpl_command);

        json_free_serialized_string(comp_string);
        break;
      }
    }
  }

  json_value_free(tempJSON);
  return PNPL_NO_ERROR_CODE;
}

/**
 * @brief Parses a command string and populates a PnPLCommand_t structure.
 *
 * This function takes a command string and a pointer to a PnPLCommand_t structure
 * and parses the command string to populate the structure with the relevant data.
 *
 * @param commandString The command string to be parsed.
 * @param command Pointer to the PnPLCommand_t structure to be populated.
 * @return uint8_t Returns 0 if the command string was successfully parsed, or an error code otherwise.
 */
static uint8_t _PnPLParseCommand(char *commandString, PnPLCommand_t *command)
{
  uint8_t commandType = 0;
  char componentName[2 *
                     COMP_KEY_MAX_LENGTH]; /* 2* because this could be a comp or a comm key. If comm_key this is in the form (comp_key*comm_key) */
  uint8_t ret = extract_PnPL_cmd_data(commandString, &commandType, componentName);

  command->comm_type = commandType;
  (void)strcpy(command->comp_name, componentName);

  if (ret == PNPL_NO_ERROR_CODE)
  {
    if (commandType == PNPL_CMD_SET)
    {
      /* Select right parse/update function */
      for (uint8_t i = 0; i < PnPLGetNComponents(); i++)
      {
        IPnPLComponent_t *p_obj = (IPnPLComponent_t *)(spPnPLObj.Components[i]);
        if (strcmp(componentName, IPnPLComponentGetKey(p_obj)) == 0)
        {
#if defined(PNPL_RESPONSES) || defined(PNPL_BLE_RESPONSES)
          char *set_response = 0;
          uint32_t size = 0;
          (void)IPnPLComponentSetProperty(p_obj, commandString, &set_response, &size, 0);
          /* SET Response */
          command->response = (char*)pnpl_malloc(size);
          if (command->response != NULL)
          {
            strcpy(command->response, set_response);
            pnpl_free(set_response);
          }
          /* SET Response*/
#else
          (void)IPnPLComponentSetProperty(p_obj, commandString);
#endif
        }
        uint8_t nOfCommands = IPnPLComponentGetNCommands(p_obj); // 0 if none
        if (nOfCommands > 0u)
        {
          for (uint8_t j = 0; j < nOfCommands; j++)
          {
            if (strcmp(IPnPLComponentGetCommandKey(p_obj, j), componentName) == 0)
            {
#if defined(PNPL_RESPONSES) || defined(PNPL_BLE_RESPONSES)
              char *cmd_response = 0;
              uint32_t size = 0;
              (void)IPnPLCommandExecuteFunction(p_obj, commandString, &cmd_response, &size, 0);
//              /* CMD Response */
              command->comm_type = PNPL_CMD_COMMAND;
//              command->response = (char*)pnpl_malloc(size);
//              if (command->response != NULL)
//              {
//                strcpy(command->response, cmd_response);
//                pnpl_free(cmd_response);
//              }
//              /* CMD Response*/
#else
              (void)IPnPLCommandExecuteFunction(p_obj, commandString);
#endif
            }
          }
        }
      }
    }
    if (commandType == PNPL_CMD_SYSTEM_CONFIG)
    {
      for (uint8_t i = 0; i < PnPLGetNComponents(); i++)
      {
        IPnPLComponent_t *p_obj = (IPnPLComponent_t *)(spPnPLObj.Components[i]);
        if (strcmp(componentName, IPnPLComponentGetKey(p_obj)) == 0)
        {
#if defined(PNPL_RESPONSES) || defined(PNPL_BLE_RESPONSES)
          char *set_response = 0;
          uint32_t size = 0;
          (void)IPnPLComponentSetProperty(p_obj, commandString, &set_response, &size, 0);
          /* SET Response */
          command->response = (char*)pnpl_malloc(size);
          if (command->response != NULL)
          {
            strcpy(command->response, set_response);
            pnpl_free(set_response);
          }
          /* SET Response*/
#else
          (void)IPnPLComponentSetProperty(p_obj, commandString);
#endif
        }
      }
    }
    if (commandType == PNPL_CMD_UPDATE_DEVICE)
    {
      (void)_PnPLUpdateDeviceStatusFromJSON(commandString);
    }
  }
  else
  {
    command->comm_type = PNPL_CMD_ERROR;
  }
  return ret;
}

