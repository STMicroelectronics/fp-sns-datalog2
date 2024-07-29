/**
  ******************************************************************************
  * @file    PnPLCompManager.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PNPL_COMP_MANAGER_H_
#define _PNPL_COMP_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "PnPLCompManager_conf.h"
#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"
#include "PnPLDef.h"
#include "parson.h"

/**
  * Create  type name for _PnPLCompManager_t.
  */
typedef struct _PnPLCompManager_t PnPLCompManager_t;

/**
  *  PnPLCompManager_t internal structure.
  */
struct _PnPLCompManager_t
{

  /**
    * Describes the sensor capabilities.
    */
  IPnPLComponent_t *Components[COM_MAX_PNPL_COMPONENTS];

  /**
    * Indicates the number of PnPL components available.
    */
  uint16_t n_components;
};

typedef void * (*PnPL_Malloc_Function)(size_t);
typedef void   (*PnPL_Free_Function)(void *);

/**
 * @brief Type definitions for lock and unlock callbacks.
 */
typedef void (*PnPLockCallback)(void);
typedef void (*PnPLUnlockCallback)(void);


/* Enum to represent logging types */
typedef enum {
    PNPL_LOG_ERROR,
    PNPL_LOG_WARNING,
    PNPL_LOG_INFO
} PnPLLogType;

/* Public API declaration */
/**************************/
#ifndef FW_ID
void PnPLSetFWID(uint8_t id);
#endif
#ifndef BOARD_ID
void PnPLSetBOARDID(uint8_t id);
#endif
void  *pnpl_malloc(size_t size);
void  pnpl_free (void *ptr);
/* Call only once, before calling any other function from PnPL API. If not called, malloc and free
 from stdlib will be used for all allocations */
void PnPLSetAllocationFunctions(PnPL_Malloc_Function malloc_fun, PnPL_Free_Function free_fun);
uint8_t PnPL_SetLockUnlockCallbacks(PnPLockCallback lock_callback, PnPLUnlockCallback unlock_callback);
uint8_t PnPLCreateLogMessage(char **SerializedJSON, uint32_t *size, const char *message, PnPLLogType logType);
uint8_t PnPLGetFWID(void);
uint8_t PnPLGetBOARDID(void);
void PnPLGenerateAcquisitionUUID(char *uuid);
uint16_t PnPLGetNComponents(void);
uint16_t PnPLGetComponentsNames(char **components_names);
uint8_t PnPLGetComponentValue(char *comp_name, char **SerializedJSON, uint32_t *size, uint8_t pretty);
uint8_t PnPLAddComponent(IPnPLComponent_t *pComponent);
uint8_t PnPLAddSensorComponent(IPnPLComponent_t *pComponent, uint8_t sensor_id, uint8_t sensor_type);
uint8_t PnPLRemoveComponent(uint8_t id); //char*
uint8_t PnPLGetPresentationJSON(char **SerializedJSON, uint32_t *size);
uint8_t PnPLGetDeviceStatusJSON(char **SerializedJSON, uint32_t *size, uint8_t pretty);
uint8_t PnPLGetFilteredDeviceStatusJSON(char **serializedJSON, uint32_t *size, char **skip_list,
                                        uint32_t skip_list_size, uint8_t pretty);
uint8_t PnPLUpdateDeviceStatusFromJSON(char *serializedJSON);
uint8_t PnPLParseCommand(char *commandString, PnPLCommand_t *command);
uint8_t PnPLSerializeResponse(PnPLCommand_t *command, char **SerializedJSON, uint32_t *size, uint8_t pretty);
uint8_t PnPLSerializeTelemetry(char *compName, PnPLTelemetry_t *telemetryValue, uint8_t telemetryNum,
                               char **telemetryJSON, uint32_t *size, uint8_t pretty);
uint8_t PnPLSerializeCommandResponse(char **responseJSON, uint32_t *size, uint8_t pretty, const char *message, bool status);
/* Inline functions definition */
/*******************************/

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_COMP_MANAGER_H_ */

