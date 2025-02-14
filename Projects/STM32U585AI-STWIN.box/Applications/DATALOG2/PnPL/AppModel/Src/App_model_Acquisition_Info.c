/**
  ******************************************************************************
  * @file    App_model_Acquisition_Info.c
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

#include "App_model.h"

/* USER includes -------------------------------------------------------------*/
#include "TagManager.h"
/* USER private function prototypes ------------------------------------------*/

/* USER defines --------------------------------------------------------------*/

/* Acquisition Information PnPL Component ------------------------------------*/
extern AppModel_t app_model;

uint8_t acquisition_info_comp_init(void)
{
  app_model.acquisition_info_model.comp_name = acquisition_info_get_key();

  /* USER Component initialization code */
  acquisition_info_set_name("STWIN.Box_acquisition", NULL);
  acquisition_info_set_description("", NULL);
  app_model.acquisition_info_model.interface = -1;

  return PNPL_NO_ERROR_CODE;
}

char *acquisition_info_get_key(void)
{
  return "acquisition_info";
}


uint8_t acquisition_info_get_name(char **value)
{
  *value = app_model.acquisition_info_model.name;
  return PNPL_NO_ERROR_CODE;
}

uint8_t acquisition_info_get_description(char **value)
{
  *value = app_model.acquisition_info_model.description;
  return PNPL_NO_ERROR_CODE;
}

uint8_t acquisition_info_get_uuid(char **value)
{
  *value = app_model.acquisition_info_model.uuid;
  return PNPL_NO_ERROR_CODE;
}

uint8_t acquisition_info_get_start_time(char **value)
{
  *value = app_model.acquisition_info_model.start_time;
  return PNPL_NO_ERROR_CODE;
}

uint8_t acquisition_info_get_end_time(char **value)
{
  *value = app_model.acquisition_info_model.end_time;
  return PNPL_NO_ERROR_CODE;
}

uint8_t acquisition_info_get_data_ext(char **value)
{
  *value = ".dat";
  return PNPL_NO_ERROR_CODE;
}

uint8_t acquisition_info_get_data_fmt(char **value)
{
  *value = "HSD_2.0.0";
  return PNPL_NO_ERROR_CODE;
}

uint8_t acquisition_info_get_tags(JSON_Value *value)
{
  JSON_Value *tempJSON1;
  JSON_Object *JSON_Tags;
  JSON_Array *JSON_TagsArray;

  JSON_Tags = json_value_get_object(value);

  json_object_set_value(JSON_Tags, "tags", json_value_init_array());
  JSON_TagsArray = json_object_dotget_array(JSON_Tags, "tags");

  int i;

  HSD_Tag_t *tag_list = TMGetTagList();
  uint8_t tag_list_size = TMGetTagListSize();
  if (tag_list_size > 0)
  {
    for (i = 0; i < tag_list_size; i++)
    {
      tempJSON1 = json_value_init_object();
      JSON_Object *tag_object = json_value_get_object(tempJSON1);
      json_object_set_string(tag_object, "l", tag_list[i].label);
      json_object_set_boolean(tag_object, "e", tag_list[i].status);
      json_object_set_string(tag_object, "ta", tag_list[i].abs_timestamp);
      json_array_append_value(JSON_TagsArray, tempJSON1);
    }
  }
  /* no need to free tempJSON1 as it is part of value */
  return PNPL_NO_ERROR_CODE;
}

uint8_t acquisition_info_get_interface(pnpl_acquisition_info_interface_t *enum_id)
{
  switch (app_model.acquisition_info_model.interface)
  {
    case 0:
      *enum_id = pnpl_acquisition_info_interface_sd;
      break;
    case 1:
      *enum_id = pnpl_acquisition_info_interface_usb;
      break;
  }
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t acquisition_info_get_schema_version(char **value)
{
  *value = "2.0.0";
  return PNPL_NO_ERROR_CODE;
}


uint8_t acquisition_info_set_name(const char *value, char **response_message)
{
  size_t value_len = strlen(value);
  if (value_len > 0 && value_len <= HSD_ACQ_NAME_LENGTH)
  {
    strcpy(app_model.acquisition_info_model.name, value);
    if (response_message != NULL)
    {
      *response_message = "";
    }
  }
  else
  {
    if (value_len <= 0)
    {
      if (response_message != NULL)
      {
        *response_message = "Empty acquisition name";
      }
      return PNPL_BASE_ERROR_CODE ;
    }
    else if (value_len > HSD_ACQ_NAME_LENGTH)
    {
      if (response_message != NULL)
      {
        *response_message = "Max number of character (64) exceeded for the acquisition name";
      }
      return PNPL_BASE_ERROR_CODE ;
    }
  }
  return PNPL_NO_ERROR_CODE ;
}

uint8_t acquisition_info_set_description(const char *value, char **response_message)
{
  size_t value_len = strlen(value);
  if (value_len > 0 && value_len <= HSD_ACQ_DESC_LENGTH)
  {
    strcpy(app_model.acquisition_info_model.description, value);
    if (response_message != NULL)
    {
      *response_message = "";
    }
  }
  else
  {
    if (value_len <= 0)
    {
      if (response_message != NULL)
      {
        *response_message = "Empty acquisition description";
      }
      return PNPL_BASE_ERROR_CODE ;
    }
    else if (value_len > HSD_ACQ_DESC_LENGTH)
    {
      if (response_message != NULL)
      {
        *response_message = "Max number of character (100) exceeded for the acquisition description";
      }
      return PNPL_BASE_ERROR_CODE ;
    }
  }
  return PNPL_NO_ERROR_CODE ;
}



