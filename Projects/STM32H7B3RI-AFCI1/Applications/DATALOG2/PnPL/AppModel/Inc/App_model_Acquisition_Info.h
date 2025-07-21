/**
  ******************************************************************************
  * @file    App_model_Acquisition_Info.h
  * @author  SRA
  * @brief   Acquisition_Info PnPL Components APIs
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
  * dtmi:vespucci:other:acquisition_info;2
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/**
  ******************************************************************************
  * Component APIs *************************************************************
  * - Component init function
  *    <comp_name>_comp_init(void)
  * - Component get_key function
  *    <comp_name>_get_key(void)
  * - Component GET/SET Properties APIs ****************************************
  *  - GET Functions
  *    uint8_t <comp_name>_get_<prop_name>(prop_type *value)
  *      if prop_type == char --> (char **value)
  *  - SET Functions
  *    uint8_t <comp_name>_set_<prop_name>(prop_type value)
  *      if prop_type == char --> (char *value)
  *  - Component COMMAND Reaction Functions
  *      uint8_t <comp_name>_<command_name>(
  *                     field1_type field1_name, field2_type field2_name, ...,
  *                     fieldN_type fieldN_name);
  *  - Component TELEMETRY Send Functions
  *      uint8_t <comp_name>_create_telemetry(tel1_type tel1_name,
  *                     tel2_type tel2_name, ..., telN_type telN_name,
  *                     char **telemetry, uint32_t *size)
  ******************************************************************************
  */

#ifndef APP_MODEL_ACQUISITION_INFO_H_
#define APP_MODEL_ACQUISITION_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Acquisition_Info_PnPL.h"

/* USER includes -------------------------------------------------------------*/
#define HSD_ACQ_NAME_LENGTH 64U
#define HSD_ACQ_DESC_LENGTH 100U
#define HSD_ACQ_INTERNAL_TIMESTAMP_LENGTH 18U
#define HSD_ACQ_TIMESTAMP_LENGTH 25U

typedef struct _AcquisitionInfoModel_t
{
  char *comp_name;
  /* AcquisitionInfo Component Model USER code */
  char name[HSD_ACQ_NAME_LENGTH];
  char description[HSD_ACQ_DESC_LENGTH];
  int8_t interface;
  char uuid[37]; // UUID: 8-4-4-4-12 = 36char + \0
  char start_time[HSD_ACQ_TIMESTAMP_LENGTH];// "YYYY-MM-DDTHH:MM:SS.mmmZ"
  char end_time[HSD_ACQ_TIMESTAMP_LENGTH];// "YYYY-MM-DDTHH:MM:SS.mmmZ"
} AcquisitionInfoModel_t;

/* Acquisition Information PnPL Component ------------------------------------- */
uint8_t acquisition_info_comp_init(void);
char *acquisition_info_get_key(void);
uint8_t acquisition_info_get_name(char **value);
uint8_t acquisition_info_get_description(char **value);
uint8_t acquisition_info_get_uuid(char **value);
uint8_t acquisition_info_get_start_time(char **value);
uint8_t acquisition_info_get_end_time(char **value);
uint8_t acquisition_info_get_data_ext(char **value);
uint8_t acquisition_info_get_data_fmt(char **value);
uint8_t acquisition_info_get_tags(JSON_Value *value);
uint8_t acquisition_info_get_interface(pnpl_acquisition_info_interface_t *enum_id);
uint8_t acquisition_info_get_schema_version(char **value);
uint8_t acquisition_info_set_name(const char *value, char **response_message);
uint8_t acquisition_info_set_description(const char *value, char **response_message);


#ifdef __cplusplus
}
#endif

#endif /* APP_MODEL_ACQUISITION_INFO_H_ */
