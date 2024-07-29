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


uint8_t tags_info_set_sw_tag0__label(const char *value, char **response_message)
{
  return TMSetSWTagLabel(value, 0);
}

uint8_t tags_info_set_sw_tag0__enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMEnableSWTag(value, 0);
}

uint8_t tags_info_set_sw_tag0__status(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 0);
  }
  else
  {
    TMInitSWTagStatus(false, 0);
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_set_sw_tag1__label(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMSetSWTagLabel(value, 1);
}

uint8_t tags_info_set_sw_tag1__enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMEnableSWTag(value, 1);
}

uint8_t tags_info_set_sw_tag1__status(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 1);
  }
  else
  {
    TMInitSWTagStatus(false, 1);
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_set_sw_tag2__label(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMSetSWTagLabel(value, 2);
}

uint8_t tags_info_set_sw_tag2__enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMEnableSWTag(value, 2);
}

uint8_t tags_info_set_sw_tag2__status(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 2);
  }
  else
  {
    TMInitSWTagStatus(false, 2);
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_set_sw_tag3__label(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMSetSWTagLabel(value, 3);
}

uint8_t tags_info_set_sw_tag3__enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMEnableSWTag(value, 3);
}

uint8_t tags_info_set_sw_tag3__status(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 3);
  }
  else
  {
    TMInitSWTagStatus(false, 3);
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_set_sw_tag4__label(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMSetSWTagLabel(value, 4);
}

uint8_t tags_info_set_sw_tag4__enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMEnableSWTag(value, 4);
}

uint8_t tags_info_set_sw_tag4__status(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 4);
  }
  else
  {
    TMInitSWTagStatus(false, 4);
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_set_sw_tag5__label(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMSetSWTagLabel(value, 5);
}

uint8_t tags_info_set_sw_tag5__enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMEnableSWTag(value, 5);
}

uint8_t tags_info_set_sw_tag5__status(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 5);
  }
  else
  {
    TMInitSWTagStatus(false, 5);
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_set_sw_tag6__label(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMSetSWTagLabel(value, 6);
}

uint8_t tags_info_set_sw_tag6__enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMEnableSWTag(value, 6);
}

uint8_t tags_info_set_sw_tag6__status(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 6);
  }
  else
  {
    TMInitSWTagStatus(false, 6);
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_set_sw_tag7__label(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMSetSWTagLabel(value, 7);
}

uint8_t tags_info_set_sw_tag7__enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMEnableSWTag(value, 7);
}

uint8_t tags_info_set_sw_tag7__status(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 7);
  }
  else
  {
    TMInitSWTagStatus(false, 7);
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_set_sw_tag8__label(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMSetSWTagLabel(value, 8);
}

uint8_t tags_info_set_sw_tag8__enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMEnableSWTag(value, 8);
}

uint8_t tags_info_set_sw_tag8__status(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 8);
  }
  else
  {
    TMInitSWTagStatus(false, 8);
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_set_sw_tag9__label(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMSetSWTagLabel(value, 9);
}

uint8_t tags_info_set_sw_tag9__enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMEnableSWTag(value, 9);
}

uint8_t tags_info_set_sw_tag9__status(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 9);
  }
  else
  {
    TMInitSWTagStatus(false, 9);
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_set_sw_tag10__label(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMSetSWTagLabel(value, 10);
}

uint8_t tags_info_set_sw_tag10__enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMEnableSWTag(value, 10);
}

uint8_t tags_info_set_sw_tag10__status(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 10);
  }
  else
  {
    TMInitSWTagStatus(false, 10);
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_set_sw_tag11__label(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMSetSWTagLabel(value, 11);
}

uint8_t tags_info_set_sw_tag11__enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMEnableSWTag(value, 11);
}

uint8_t tags_info_set_sw_tag11__status(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 11);
  }
  else
  {
    TMInitSWTagStatus(false, 11);
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_set_sw_tag12__label(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMSetSWTagLabel(value, 12);
}

uint8_t tags_info_set_sw_tag12__enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMEnableSWTag(value, 12);
}

uint8_t tags_info_set_sw_tag12__status(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 12);
  }
  else
  {
    TMInitSWTagStatus(false, 12);
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_set_sw_tag13__label(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMSetSWTagLabel(value, 13);
}

uint8_t tags_info_set_sw_tag13__enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMEnableSWTag(value, 13);
}

uint8_t tags_info_set_sw_tag13__status(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 13);
  }
  else
  {
    TMInitSWTagStatus(false, 13);
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_set_sw_tag14__label(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMSetSWTagLabel(value, 14);
}

uint8_t tags_info_set_sw_tag14__enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMEnableSWTag(value, 14);
}

uint8_t tags_info_set_sw_tag14__status(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 14);
  }
  else
  {
    TMInitSWTagStatus(false, 14);
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t tags_info_set_sw_tag15__label(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMSetSWTagLabel(value, 15);
}

uint8_t tags_info_set_sw_tag15__enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  return TMEnableSWTag(value, 15);
}

uint8_t tags_info_set_sw_tag15__status(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 15);
  }
  else
  {
    TMInitSWTagStatus(false, 15);
  }
  return PNPL_NO_ERROR_CODE;
}

