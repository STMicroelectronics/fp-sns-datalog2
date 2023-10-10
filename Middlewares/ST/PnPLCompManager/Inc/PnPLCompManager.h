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

#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"
#include "PnPLCompManager_Conf.h"
#include "PnPLDef.h"
#include "parson.h"

/* Includes ------------------------------------------------------------------*/

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

/* Public API declaration */
/**************************/
#ifndef FW_ID
void PnPLSetFWID(uint8_t id);
#endif
#ifndef BOARD_ID
void PnPLSetBOARDID(uint8_t id);
#endif
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


/* Inline functions definition */
/*******************************/

#ifdef __cplusplus
}
#endif

#endif /* _PNPL_COMP_MANAGER_H_ */

