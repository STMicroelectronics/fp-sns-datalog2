/**
  ******************************************************************************
  * @file    App_model_Tags_Info.c
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
  * dtmi:vespucci:other:tags_info;1
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
uint8_t __tags_info_set_sw_tagN__label(const char *value, char **response_message, int32_t n);
uint8_t __tags_info_set_sw_tagN__enabled(bool value, char **response_message, int32_t n);
uint8_t __tags_info_set_sw_tagN__status(bool value, char **response_message, int32_t n);
uint8_t __tags_info_set_hw_tagN__label(const char *value, char **response_message, int32_t n);
uint8_t __tags_info_set_hw_tagN__enabled(bool value, char **response_message, int32_t n);
uint8_t __tags_info_set_hw_tagN__status(bool value, char **response_message, int32_t n);

/* USER defines --------------------------------------------------------------*/

/* Tags Information PnPL Component -------------------------------------------*/
extern AppModel_t app_model;

uint8_t tags_info_comp_init(void)
{
  app_model.tags_info_model.comp_name = tags_info_get_key();

  TMInit(HSD_SW_TAG_CLASS_NUM, HSD_HW_TAG_CLASS_NUM);

  tags_info_set_sw_tag0__enabled(true, NULL);
  tags_info_set_sw_tag1__enabled(true, NULL);
  tags_info_set_sw_tag2__enabled(true, NULL);
  tags_info_set_sw_tag3__enabled(true, NULL);
  tags_info_set_sw_tag4__enabled(true, NULL);
  tags_info_set_sw_tag0__status(false, NULL);
  tags_info_set_sw_tag1__status(false, NULL);
  tags_info_set_sw_tag2__status(false, NULL);
  tags_info_set_sw_tag3__status(false, NULL);
  tags_info_set_sw_tag4__status(false, NULL);
  return PNPL_NO_ERROR_CODE;
}

char *tags_info_get_key(void)
{
  return "tags_info";
}


uint8_t tags_info_get_max_tags_num(int32_t *value)
{
  *value = HSD_MAX_TAGS_NUM;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag0__label(char **value)
{
  *value = TMGetSWTagLabel(0);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag0__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(0);
  *value = sw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag0__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(0);
  *value = sw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag1__label(char **value)
{
  *value = TMGetSWTagLabel(1);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag1__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(1);
  *value = sw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag1__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(1);
  *value = sw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag2__label(char **value)
{
  *value = TMGetSWTagLabel(2);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag2__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(2);
  *value = sw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag2__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(2);
  *value = sw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag3__label(char **value)
{
  *value = TMGetSWTagLabel(3);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag3__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(3);
  *value = sw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag3__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(3);
  *value = sw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag4__label(char **value)
{
  *value = TMGetSWTagLabel(4);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag4__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(4);
  *value = sw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag4__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(4);
  *value = sw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}


/* USER CODE */
uint8_t __tags_info_set_sw_tagN__label(const char *value, char **response_message, int32_t n)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = SYS_NO_ERROR_CODE;
  ret = TMSetSWTagLabel(value, n);
  if (ret != SYS_NO_ERROR_CODE)
  {
    if (response_message != NULL)
    {
      *response_message = "Error: Tag class label assignment failure";
    }
  }
  return ret;
}

uint8_t __tags_info_set_sw_tagN__enabled(bool value, char **response_message, int32_t n)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = SYS_NO_ERROR_CODE;
  ret = TMEnableSWTag(value, n);
  if (ret != SYS_NO_ERROR_CODE)
  {
    if (response_message != NULL)
    {
      *response_message = "Error: Tag class enabling failure";
    }
  }
  return ret;
}

uint8_t __tags_info_set_sw_tagN__status(bool value, char **response_message, int32_t n)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  uint8_t ret = SYS_NO_ERROR_CODE;
  if (status)
  {
    TMSetSWTag(value, n);
  }
  else
  {
    TMInitSWTagStatus(false, n);
  }
  if (ret != SYS_NO_ERROR_CODE)
  {
    if (response_message != NULL)
    {
      *response_message = "Error: Acquisition tagging Unsuccessful";
    }
  }
  return ret;
}

uint8_t tags_info_set_sw_tag0__label(const char *value, char **response_message)
{
  return __tags_info_set_sw_tagN__label(value, response_message, 0);
}

uint8_t tags_info_set_sw_tag0__enabled(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__enabled(value, response_message, 0);
}

uint8_t tags_info_set_sw_tag0__status(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__status(value, response_message, 0);
}

uint8_t tags_info_set_sw_tag1__label(const char *value, char **response_message)
{
  return __tags_info_set_sw_tagN__label(value, response_message, 1);
}

uint8_t tags_info_set_sw_tag1__enabled(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__enabled(value, response_message, 1);
}

uint8_t tags_info_set_sw_tag1__status(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__status(value, response_message, 1);
}

uint8_t tags_info_set_sw_tag2__label(const char *value, char **response_message)
{
  return __tags_info_set_sw_tagN__label(value, response_message, 2);
}

uint8_t tags_info_set_sw_tag2__enabled(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__enabled(value, response_message, 2);
}

uint8_t tags_info_set_sw_tag2__status(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__status(value, response_message, 2);
}

uint8_t tags_info_set_sw_tag3__label(const char *value, char **response_message)
{
  return __tags_info_set_sw_tagN__label(value, response_message, 3);
}

uint8_t tags_info_set_sw_tag3__enabled(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__enabled(value, response_message, 3);
}

uint8_t tags_info_set_sw_tag3__status(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__status(value, response_message, 3);
}

uint8_t tags_info_set_sw_tag4__label(const char *value, char **response_message)
{
  return __tags_info_set_sw_tagN__label(value, response_message, 4);
}

uint8_t tags_info_set_sw_tag4__enabled(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__enabled(value, response_message, 4);
}

uint8_t tags_info_set_sw_tag4__status(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__status(value, response_message, 4);
}
