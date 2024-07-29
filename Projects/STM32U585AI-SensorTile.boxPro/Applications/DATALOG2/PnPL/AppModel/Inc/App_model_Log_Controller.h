/**
  ******************************************************************************
  * @file    App_model_Log_Controller.h
  * @author  SRA
  * @brief   Log_Controller PnPL Components APIs
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
  * dtmi:vespucci:other:log_controller;3
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

#ifndef APP_MODEL_LOG_CONTROLLER_H_
#define APP_MODEL_LOG_CONTROLLER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* USER includes -------------------------------------------------------------*/

typedef struct _LogControllerModel_t
{
  char *comp_name;
  /* LogController Component Model USER code */
  bool status;
  bool sd_mounted;
  bool sd_failed;
  int8_t interface;
} LogControllerModel_t;

/* Log Controller PnPL Component ---------------------------------------------- */
uint8_t log_controller_comp_init(void);
char *log_controller_get_key(void);
uint8_t log_controller_get_log_status(bool *value);
uint8_t log_controller_get_sd_mounted(bool *value);
uint8_t log_controller_get_sd_failed(bool *value);
uint8_t log_controller_get_controller_type(int32_t *value);
uint8_t log_controller_save_config(void);
uint8_t log_controller_start_log(int32_t interface);
uint8_t log_controller_stop_log(void);
uint8_t log_controller_set_time(const char *datetime);
uint8_t log_controller_switch_bank(void);
uint8_t log_controller_set_dfu_mode(void);
uint8_t log_controller_enable_all(bool status);


#ifdef __cplusplus
}
#endif

#endif /* APP_MODEL_LOG_CONTROLLER_H_ */
