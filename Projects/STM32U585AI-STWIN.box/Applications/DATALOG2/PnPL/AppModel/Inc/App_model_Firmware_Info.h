/**
  ******************************************************************************
  * @file    App_model_Firmware_Info.h
  * @author  SRA
  * @brief   Firmware_Info PnPL Components APIs
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
  * dtmi:vespucci:other:firmware_info;3
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

#ifndef APP_MODEL_FIRMWARE_INFO_H_
#define APP_MODEL_FIRMWARE_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif

/* USER includes -------------------------------------------------------------*/
#define DEVICE_ALIAS_LENGTH 16U
#define MAC_ADDRESS_LENGTH  18U

typedef struct _FirmwareInfoModel_t
{
  char *comp_name;
  /* FirmwareInfo Component Model USER code */
  char alias[DEVICE_ALIAS_LENGTH];
  char mac_address[MAC_ADDRESS_LENGTH];
} FirmwareInfoModel_t;

/* Firmware Information PnPL Component ---------------------------------------- */
uint8_t firmware_info_comp_init(void);
char *firmware_info_get_key(void);
uint8_t firmware_info_get_alias(char **value);
uint8_t firmware_info_get_fw_name(char **value);
uint8_t firmware_info_get_fw_version(char **value);
uint8_t firmware_info_get_part_number(char **value);
uint8_t firmware_info_get_device_url(char **value);
uint8_t firmware_info_get_fw_url(char **value);
uint8_t firmware_info_get_mac_address(char **value);
uint8_t firmware_info_set_alias(const char *value, char **response_message);


#ifdef __cplusplus
}
#endif

#endif /* APP_MODEL_FIRMWARE_INFO_H_ */
