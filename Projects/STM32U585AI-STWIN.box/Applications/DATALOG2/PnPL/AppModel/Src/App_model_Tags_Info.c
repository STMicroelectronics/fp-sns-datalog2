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
  * dtmi:vespucci:other:tags_info;2
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
uint8_t __tags_info_set_sw_tagN__label(const char *value, char **response_message, int n);
uint8_t __tags_info_set_sw_tagN__enabled(bool value, char **response_message, int n);
uint8_t __tags_info_set_sw_tagN__status(bool value, char **response_message, int n);
uint8_t __tags_info_set_hw_tagN__label(const char *value, char **response_message, int n);
uint8_t __tags_info_set_hw_tagN__enabled(bool value, char **response_message, int n);
uint8_t __tags_info_set_hw_tagN__status(bool value, char **response_message, int n);

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
  tags_info_set_sw_tag5__enabled(true, NULL);
  tags_info_set_sw_tag6__enabled(true, NULL);
  tags_info_set_sw_tag7__enabled(true, NULL);
  tags_info_set_sw_tag8__enabled(true, NULL);
  tags_info_set_sw_tag9__enabled(true, NULL);
  tags_info_set_sw_tag10__enabled(true, NULL);
  tags_info_set_sw_tag11__enabled(true, NULL);
  tags_info_set_sw_tag12__enabled(true, NULL);
  tags_info_set_sw_tag13__enabled(true, NULL);
  tags_info_set_sw_tag14__enabled(true, NULL);
  tags_info_set_sw_tag15__enabled(true, NULL);
  tags_info_set_sw_tag0__status(false, NULL);
  tags_info_set_sw_tag1__status(false, NULL);
  tags_info_set_sw_tag2__status(false, NULL);
  tags_info_set_sw_tag3__status(false, NULL);
  tags_info_set_sw_tag4__status(false, NULL);
  tags_info_set_sw_tag5__status(false, NULL);
  tags_info_set_sw_tag6__status(false, NULL);
  tags_info_set_sw_tag7__status(false, NULL);
  tags_info_set_sw_tag8__status(false, NULL);
  tags_info_set_sw_tag9__status(false, NULL);
  tags_info_set_sw_tag10__status(false, NULL);
  tags_info_set_sw_tag11__status(false, NULL);
  tags_info_set_sw_tag12__status(false, NULL);
  tags_info_set_sw_tag13__status(false, NULL);
  tags_info_set_sw_tag14__status(false, NULL);
  tags_info_set_sw_tag15__status(false, NULL);

  /* By default, HW TAGS are disabled */
  tags_info_set_hw_tag0__enabled(false, NULL);
  tags_info_set_hw_tag1__enabled(false, NULL);
  tags_info_set_hw_tag0__status(false, NULL);
  tags_info_set_hw_tag1__status(false, NULL);

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

uint8_t tags_info_get_sw_tag5__label(char **value)
{
  *value = TMGetSWTagLabel(5);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag5__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(5);
  *value = sw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag5__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(5);
  *value = sw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag6__label(char **value)
{
  *value = TMGetSWTagLabel(6);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag6__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(6);
  *value = sw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag6__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(6);
  *value = sw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag7__label(char **value)
{
  *value = TMGetSWTagLabel(7);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag7__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(7);
  *value = sw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag7__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(7);
  *value = sw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag8__label(char **value)
{
  *value = TMGetSWTagLabel(8);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag8__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(8);
  *value = sw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag8__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(8);
  *value = sw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag9__label(char **value)
{
  *value = TMGetSWTagLabel(9);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag9__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(9);
  *value = sw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag9__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(9);
  *value = sw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag10__label(char **value)
{
  *value = TMGetSWTagLabel(10);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag10__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(10);
  *value = sw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag10__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(10);
  *value = sw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag11__label(char **value)
{
  *value = TMGetSWTagLabel(11);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag11__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(11);
  *value = sw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag11__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(11);
  *value = sw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag12__label(char **value)
{
  *value = TMGetSWTagLabel(12);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag12__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(12);
  *value = sw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag12__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(12);
  *value = sw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag13__label(char **value)
{
  *value = TMGetSWTagLabel(13);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag13__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(13);
  *value = sw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag13__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(13);
  *value = sw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag14__label(char **value)
{
  *value = TMGetSWTagLabel(14);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag14__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(14);
  *value = sw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag14__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(14);
  *value = sw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag15__label(char **value)
{
  *value = TMGetSWTagLabel(15);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag15__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(15);
  *value = sw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_sw_tag15__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(15);
  *value = sw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

/* USER CODE */
uint8_t __tags_info_set_sw_tagN__label(const char *value, char **response_message, int n)
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

uint8_t __tags_info_set_sw_tagN__enabled(bool value, char **response_message, int n)
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

uint8_t __tags_info_set_sw_tagN__status(bool value, char **response_message, int n)
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

/* USER CODE */
uint8_t __tags_info_set_hw_tagN__label(const char *value, char **response_message, int n)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = SYS_NO_ERROR_CODE;
  ret = TMSetHWTagLabel(value, n);
  if (ret != SYS_NO_ERROR_CODE)
  {
    if (response_message != NULL)
    {
      *response_message = "Error: Tag class label assignment failure";
    }
  }
  return ret;
}

uint8_t __tags_info_set_hw_tagN__enabled(bool value, char **response_message, int n)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = SYS_NO_ERROR_CODE;
  ret = TMEnableHWTag(value, n);
  if (ret != SYS_NO_ERROR_CODE)
  {
    if (response_message != NULL)
    {
      *response_message = "Error: Tag class enabling failure";
    }
  }
  return ret;
}

uint8_t __tags_info_set_hw_tagN__status(bool value, char **response_message, int n)
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
    TMSetHWTag(value, n);
  }
  else
  {
    TMInitHWTagStatus(false, n);
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


/* USER CODE */

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

uint8_t tags_info_set_sw_tag5__label(const char *value, char **response_message)
{
  return __tags_info_set_sw_tagN__label(value, response_message, 5);
}

uint8_t tags_info_set_sw_tag5__enabled(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__enabled(value, response_message, 5);
}

uint8_t tags_info_set_sw_tag5__status(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__status(value, response_message, 5);
}

uint8_t tags_info_set_sw_tag6__label(const char *value, char **response_message)
{
  return __tags_info_set_sw_tagN__label(value, response_message, 6);
}

uint8_t tags_info_set_sw_tag6__enabled(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__enabled(value, response_message, 6);
}

uint8_t tags_info_set_sw_tag6__status(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__status(value, response_message, 6);
}

uint8_t tags_info_set_sw_tag7__label(const char *value, char **response_message)
{
  return __tags_info_set_sw_tagN__label(value, response_message, 7);
}

uint8_t tags_info_set_sw_tag7__enabled(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__enabled(value, response_message, 7);
}

uint8_t tags_info_set_sw_tag7__status(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__status(value, response_message, 7);
}

uint8_t tags_info_set_sw_tag8__label(const char *value, char **response_message)
{
  return __tags_info_set_sw_tagN__label(value, response_message, 8);
}

uint8_t tags_info_set_sw_tag8__enabled(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__enabled(value, response_message, 8);
}

uint8_t tags_info_set_sw_tag8__status(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__status(value, response_message, 8);
}

uint8_t tags_info_set_sw_tag9__label(const char *value, char **response_message)
{
  return __tags_info_set_sw_tagN__label(value, response_message, 9);
}

uint8_t tags_info_set_sw_tag9__enabled(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__enabled(value, response_message, 9);
}

uint8_t tags_info_set_sw_tag9__status(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__status(value, response_message, 9);
}

uint8_t tags_info_set_sw_tag10__label(const char *value, char **response_message)
{
  return __tags_info_set_sw_tagN__label(value, response_message, 10);
}

uint8_t tags_info_set_sw_tag10__enabled(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__enabled(value, response_message, 10);
}

uint8_t tags_info_set_sw_tag10__status(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__status(value, response_message, 10);
}

uint8_t tags_info_set_sw_tag11__label(const char *value, char **response_message)
{
  return __tags_info_set_sw_tagN__label(value, response_message, 11);
}

uint8_t tags_info_set_sw_tag11__enabled(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__enabled(value, response_message, 11);
}

uint8_t tags_info_set_sw_tag11__status(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__status(value, response_message, 11);
}

uint8_t tags_info_set_sw_tag12__label(const char *value, char **response_message)
{
  return __tags_info_set_sw_tagN__label(value, response_message, 12);
}

uint8_t tags_info_set_sw_tag12__enabled(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__enabled(value, response_message, 12);
}

uint8_t tags_info_set_sw_tag12__status(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__status(value, response_message, 12);
}

uint8_t tags_info_set_sw_tag13__label(const char *value, char **response_message)
{
  return __tags_info_set_sw_tagN__label(value, response_message, 13);
}

uint8_t tags_info_set_sw_tag13__enabled(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__enabled(value, response_message, 13);
}

uint8_t tags_info_set_sw_tag13__status(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__status(value, response_message, 13);
}

uint8_t tags_info_set_sw_tag14__label(const char *value, char **response_message)
{
  return __tags_info_set_sw_tagN__label(value, response_message, 14);
}

uint8_t tags_info_set_sw_tag14__enabled(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__enabled(value, response_message, 14);
}

uint8_t tags_info_set_sw_tag14__status(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__status(value, response_message, 14);
}

uint8_t tags_info_set_sw_tag15__label(const char *value, char **response_message)
{
  return __tags_info_set_sw_tagN__label(value, response_message, 15);
}

uint8_t tags_info_set_sw_tag15__enabled(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__enabled(value, response_message, 15);
}

uint8_t tags_info_set_sw_tag15__status(bool value, char **response_message)
{
  return __tags_info_set_sw_tagN__status(value, response_message, 15);
}


uint8_t tags_info_get_hw_tag0__label(char **value)
{
  *value = TMGetHWTagLabel(0);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_hw_tag0__enabled(bool *value)
{
  HSD_HW_Tag_Class_t *hw_tag_class;
  hw_tag_class = TMGetHWTag(0);
  *value = hw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_hw_tag0__status(bool *value)
{
  HSD_HW_Tag_Class_t *hw_tag_class;
  hw_tag_class = TMGetHWTag(0);
  *value = hw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_hw_tag1__label(char **value)
{
  *value = TMGetHWTagLabel(1);
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_hw_tag1__enabled(bool *value)
{
  HSD_HW_Tag_Class_t *hw_tag_class;
  hw_tag_class = TMGetHWTag(1);
  *value = hw_tag_class->enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_get_hw_tag1__status(bool *value)
{
  HSD_HW_Tag_Class_t *hw_tag_class;
  hw_tag_class = TMGetHWTag(1);
  *value = hw_tag_class->status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_set_hw_tag0__label(const char *value, char **response_message)
{
  return __tags_info_set_hw_tagN__label(value, response_message, 0);
}

uint8_t tags_info_set_hw_tag0__enabled(bool value, char **response_message)
{
  return __tags_info_set_hw_tagN__enabled(value, response_message, 0);
}

uint8_t tags_info_set_hw_tag0__status(bool value, char **response_message)
{
  return __tags_info_set_hw_tagN__status(value, response_message, 0);
}

uint8_t tags_info_set_hw_tag1__label(const char *value, char **response_message)
{
  return __tags_info_set_hw_tagN__label(value, response_message, 1);
}

uint8_t tags_info_set_hw_tag1__enabled(bool value, char **response_message)
{
  return __tags_info_set_hw_tagN__enabled(value, response_message, 1);
}

uint8_t tags_info_set_hw_tag1__status(bool value, char **response_message)
{
  return __tags_info_set_hw_tagN__status(value, response_message, 1);
}
