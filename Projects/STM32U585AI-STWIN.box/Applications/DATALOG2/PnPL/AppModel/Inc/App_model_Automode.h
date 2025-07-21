/**
  ******************************************************************************
  * @file    App_model_Automode.h
  * @author  SRA
  * @brief   Automode PnPL Components APIs
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
  * dtmi:vespucci:other:automode;3
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

#ifndef APP_MODEL_AUTOMODE_H_
#define APP_MODEL_AUTOMODE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* USER includes -------------------------------------------------------------*/

typedef struct _AutomodeModel_t
{
  char *comp_name;
  /* Automode Component Model USER code */
  bool enabled;
  int32_t nof_acquisitions;
  int32_t start_delay_s;
  int32_t logging_period_s;
  int32_t idle_period_s;
  bool started;
} AutomodeModel_t;

/* AutoMode PnPL Component ---------------------------------------------------- */
uint8_t automode_comp_init(void);
char *automode_get_key(void);
uint8_t automode_get_started(bool *value);
uint8_t automode_get_enabled(bool *value);
uint8_t automode_get_nof_acquisitions(int32_t *value);
uint8_t automode_get_start_delay_s(int32_t *value);
uint8_t automode_get_logging_period_s(int32_t *value);
uint8_t automode_get_idle_period_s(int32_t *value);
uint8_t automode_set_started(bool value, char **response_message);
uint8_t automode_set_enabled(bool value, char **response_message);
uint8_t automode_set_nof_acquisitions(int32_t value, char **response_message);
uint8_t automode_set_start_delay_s(int32_t value, char **response_message);
uint8_t automode_set_logging_period_s(int32_t value, char **response_message);
uint8_t automode_set_idle_period_s(int32_t value, char **response_message);


#ifdef __cplusplus
}
#endif

#endif /* APP_MODEL_AUTOMODE_H_ */
