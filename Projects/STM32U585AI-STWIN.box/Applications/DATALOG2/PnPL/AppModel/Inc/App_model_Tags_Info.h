/**
  ******************************************************************************
  * @file    App_model_Tags_Info.h
  * @author  SRA
  * @brief   Tags_Info PnPL Components APIs
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
  * dtmi:vespucci:other:tags_info;3
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

#ifndef APP_MODEL_TAGS_INFO_H_
#define APP_MODEL_TAGS_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif

/* USER includes -------------------------------------------------------------*/

typedef struct _TagsInfoModel_t
{
  char *comp_name;
  /* TagsInfo Component Model USER code */
} TagsInfoModel_t;

/* Tags Information PnPL Component -------------------------------------------- */
uint8_t tags_info_comp_init(void);
char *tags_info_get_key(void);
uint8_t tags_info_get_max_tags_num(int32_t *value);
uint8_t tags_info_get_sw_tag0__label(char **value);
uint8_t tags_info_get_sw_tag0__enabled(bool *value);
uint8_t tags_info_get_sw_tag0__status(bool *value);
uint8_t tags_info_get_sw_tag1__label(char **value);
uint8_t tags_info_get_sw_tag1__enabled(bool *value);
uint8_t tags_info_get_sw_tag1__status(bool *value);
uint8_t tags_info_get_sw_tag2__label(char **value);
uint8_t tags_info_get_sw_tag2__enabled(bool *value);
uint8_t tags_info_get_sw_tag2__status(bool *value);
uint8_t tags_info_get_sw_tag3__label(char **value);
uint8_t tags_info_get_sw_tag3__enabled(bool *value);
uint8_t tags_info_get_sw_tag3__status(bool *value);
uint8_t tags_info_get_sw_tag4__label(char **value);
uint8_t tags_info_get_sw_tag4__enabled(bool *value);
uint8_t tags_info_get_sw_tag4__status(bool *value);
uint8_t tags_info_get_sw_tag5__label(char **value);
uint8_t tags_info_get_sw_tag5__enabled(bool *value);
uint8_t tags_info_get_sw_tag5__status(bool *value);
uint8_t tags_info_get_sw_tag6__label(char **value);
uint8_t tags_info_get_sw_tag6__enabled(bool *value);
uint8_t tags_info_get_sw_tag6__status(bool *value);
uint8_t tags_info_get_sw_tag7__label(char **value);
uint8_t tags_info_get_sw_tag7__enabled(bool *value);
uint8_t tags_info_get_sw_tag7__status(bool *value);
uint8_t tags_info_get_sw_tag8__label(char **value);
uint8_t tags_info_get_sw_tag8__enabled(bool *value);
uint8_t tags_info_get_sw_tag8__status(bool *value);
uint8_t tags_info_get_sw_tag9__label(char **value);
uint8_t tags_info_get_sw_tag9__enabled(bool *value);
uint8_t tags_info_get_sw_tag9__status(bool *value);
uint8_t tags_info_get_sw_tag10__label(char **value);
uint8_t tags_info_get_sw_tag10__enabled(bool *value);
uint8_t tags_info_get_sw_tag10__status(bool *value);
uint8_t tags_info_get_sw_tag11__label(char **value);
uint8_t tags_info_get_sw_tag11__enabled(bool *value);
uint8_t tags_info_get_sw_tag11__status(bool *value);
uint8_t tags_info_get_sw_tag12__label(char **value);
uint8_t tags_info_get_sw_tag12__enabled(bool *value);
uint8_t tags_info_get_sw_tag12__status(bool *value);
uint8_t tags_info_get_sw_tag13__label(char **value);
uint8_t tags_info_get_sw_tag13__enabled(bool *value);
uint8_t tags_info_get_sw_tag13__status(bool *value);
uint8_t tags_info_get_sw_tag14__label(char **value);
uint8_t tags_info_get_sw_tag14__enabled(bool *value);
uint8_t tags_info_get_sw_tag14__status(bool *value);
uint8_t tags_info_get_sw_tag15__label(char **value);
uint8_t tags_info_get_sw_tag15__enabled(bool *value);
uint8_t tags_info_get_sw_tag15__status(bool *value);
uint8_t tags_info_get_hw_tag0__label(char **value);
uint8_t tags_info_get_hw_tag0__enabled(bool *value);
uint8_t tags_info_get_hw_tag0__status(bool *value);
uint8_t tags_info_get_hw_tag1__label(char **value);
uint8_t tags_info_get_hw_tag1__enabled(bool *value);
uint8_t tags_info_get_hw_tag1__status(bool *value);
uint8_t tags_info_set_sw_tag0__label(const char *value, char **response_message);
uint8_t tags_info_set_sw_tag0__enabled(bool value, char **response_message);
uint8_t tags_info_set_sw_tag0__status(bool value, char **response_message);
uint8_t tags_info_set_sw_tag1__label(const char *value, char **response_message);
uint8_t tags_info_set_sw_tag1__enabled(bool value, char **response_message);
uint8_t tags_info_set_sw_tag1__status(bool value, char **response_message);
uint8_t tags_info_set_sw_tag2__label(const char *value, char **response_message);
uint8_t tags_info_set_sw_tag2__enabled(bool value, char **response_message);
uint8_t tags_info_set_sw_tag2__status(bool value, char **response_message);
uint8_t tags_info_set_sw_tag3__label(const char *value, char **response_message);
uint8_t tags_info_set_sw_tag3__enabled(bool value, char **response_message);
uint8_t tags_info_set_sw_tag3__status(bool value, char **response_message);
uint8_t tags_info_set_sw_tag4__label(const char *value, char **response_message);
uint8_t tags_info_set_sw_tag4__enabled(bool value, char **response_message);
uint8_t tags_info_set_sw_tag4__status(bool value, char **response_message);
uint8_t tags_info_set_sw_tag5__label(const char *value, char **response_message);
uint8_t tags_info_set_sw_tag5__enabled(bool value, char **response_message);
uint8_t tags_info_set_sw_tag5__status(bool value, char **response_message);
uint8_t tags_info_set_sw_tag6__label(const char *value, char **response_message);
uint8_t tags_info_set_sw_tag6__enabled(bool value, char **response_message);
uint8_t tags_info_set_sw_tag6__status(bool value, char **response_message);
uint8_t tags_info_set_sw_tag7__label(const char *value, char **response_message);
uint8_t tags_info_set_sw_tag7__enabled(bool value, char **response_message);
uint8_t tags_info_set_sw_tag7__status(bool value, char **response_message);
uint8_t tags_info_set_sw_tag8__label(const char *value, char **response_message);
uint8_t tags_info_set_sw_tag8__enabled(bool value, char **response_message);
uint8_t tags_info_set_sw_tag8__status(bool value, char **response_message);
uint8_t tags_info_set_sw_tag9__label(const char *value, char **response_message);
uint8_t tags_info_set_sw_tag9__enabled(bool value, char **response_message);
uint8_t tags_info_set_sw_tag9__status(bool value, char **response_message);
uint8_t tags_info_set_sw_tag10__label(const char *value, char **response_message);
uint8_t tags_info_set_sw_tag10__enabled(bool value, char **response_message);
uint8_t tags_info_set_sw_tag10__status(bool value, char **response_message);
uint8_t tags_info_set_sw_tag11__label(const char *value, char **response_message);
uint8_t tags_info_set_sw_tag11__enabled(bool value, char **response_message);
uint8_t tags_info_set_sw_tag11__status(bool value, char **response_message);
uint8_t tags_info_set_sw_tag12__label(const char *value, char **response_message);
uint8_t tags_info_set_sw_tag12__enabled(bool value, char **response_message);
uint8_t tags_info_set_sw_tag12__status(bool value, char **response_message);
uint8_t tags_info_set_sw_tag13__label(const char *value, char **response_message);
uint8_t tags_info_set_sw_tag13__enabled(bool value, char **response_message);
uint8_t tags_info_set_sw_tag13__status(bool value, char **response_message);
uint8_t tags_info_set_sw_tag14__label(const char *value, char **response_message);
uint8_t tags_info_set_sw_tag14__enabled(bool value, char **response_message);
uint8_t tags_info_set_sw_tag14__status(bool value, char **response_message);
uint8_t tags_info_set_sw_tag15__label(const char *value, char **response_message);
uint8_t tags_info_set_sw_tag15__enabled(bool value, char **response_message);
uint8_t tags_info_set_sw_tag15__status(bool value, char **response_message);
uint8_t tags_info_set_hw_tag0__label(const char *value, char **response_message);
uint8_t tags_info_set_hw_tag0__enabled(bool value, char **response_message);
uint8_t tags_info_set_hw_tag0__status(bool value, char **response_message);
uint8_t tags_info_set_hw_tag1__label(const char *value, char **response_message);
uint8_t tags_info_set_hw_tag1__enabled(bool value, char **response_message);
uint8_t tags_info_set_hw_tag1__status(bool value, char **response_message);


#ifdef __cplusplus
}
#endif

#endif /* APP_MODEL_TAGS_INFO_H_ */
