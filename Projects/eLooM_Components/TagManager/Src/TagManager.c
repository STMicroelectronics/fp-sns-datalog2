/**
 ******************************************************************************
 * @file    TagManager.c
 * @author  SRA - MCD
 * @version 1.0.0
 * @date    25-July-2022
 * @brief   This file provides a set of functions to handle Hardware and
 *          Software Tags
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "TagManager.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "services/SysTimestamp.h"

static HSD_SW_Tag_Class_t SWTagClasses[HSD_SW_TAG_CLASS_NUM];
static HSD_HW_Tag_Class_t HWTagClasses[HSD_HW_TAG_CLASS_NUM];
static HSD_Tag_t TagList[HSD_MAX_TAGS_NUM];
static uint8_t TagsN = 0;
static _tm StartTime;

double TMGetTimestamp(void);

sys_error_code_t TMInit(uint8_t n_sw_tags, uint8_t n_hw_tags)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if((n_sw_tags > HSD_SW_TAG_CLASS_NUM) || (n_hw_tags > HSD_HW_TAG_CLASS_NUM))
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }
  else
  {
    uint8_t ii;
    for(ii = 0; ii < n_sw_tags; ii++)
    {
      (void) snprintf(SWTagClasses[ii].label, HSD_TAGS_LABEL_LENGTH, HSD_TAGS_DEFAULT_SW, ii);
      SWTagClasses[ii].status = true;
    }
    for(ii = 0; ii < n_hw_tags; ii++)
    {
      (void) snprintf(HWTagClasses[ii].label, HSD_TAGS_LABEL_LENGTH, HSD_TAGS_DEFAULT_HW, ii);
      HWTagClasses[ii].status = true;
    }
    TagsN = 0;
    StartTime.tm_year = 0;
    StartTime.tm_mon = 0;
    StartTime.tm_mday = 0;
    StartTime.tm_hour = 0;
    StartTime.tm_min = 0;
    StartTime.tm_sec = 0;
  }

  return res;
}

sys_error_code_t TMSetStartTime(_tm start_time)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  StartTime = start_time;
  if(mktime(&StartTime) == -1)
  {
    res = SYS_BASE_ERROR_CODE;
  }
  return res;
}

sys_error_code_t TMCalculateEndTime(char *end_time)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  _tm endTime = StartTime;
  double timestamp_end = TMGetTimestamp();
  int timestamp_int = (int) timestamp_end;

  endTime.tm_sec += timestamp_int;
  if(mktime(&endTime) == -1)
  {
    res = SYS_BASE_ERROR_CODE;
  }
  else
  {
    double timestamp_dec = timestamp_end - (double) timestamp_int;
    timestamp_dec = timestamp_dec * 1000.0;
    (void) sprintf(end_time, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", endTime.tm_year, endTime.tm_mon + 1, endTime.tm_mday, endTime.tm_hour, endTime.tm_min,
                   endTime.tm_sec, (int) timestamp_dec);
    // WHY THIS +1 (in months) ???
    //  struct tm {
    //     int tm_sec;         /* seconds,  range 0 to 59          */
    //     int tm_min;         /* minutes, range 0 to 59           */
    //     int tm_hour;        /* hours, range 0 to 23             */
    //     int tm_mday;        /* day of the month, range 1 to 31  */
    //     int tm_mon;         /* month, range 0 to 11             */ <------ months (0..11)
    //     int tm_year;        /* The number of years since 1900   */
    //     int tm_wday;        /* day of the week, range 0 to 6    */
    //     int tm_yday;        /* day in the year, range 0 to 365  */
    //     int tm_isdst;       /* daylight saving time             */
    //  };
  }
  return res;
}

sys_error_code_t TMEnableSWTag(bool enabled, uint8_t id)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if(id >= HSD_SW_TAG_CLASS_NUM)
  {
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
  }
  else
  {
    SWTagClasses[id].enabled = enabled;
  }

  return res;
}

sys_error_code_t TMEnableHWTag(bool enabled, uint8_t id)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if(id >= HSD_HW_TAG_CLASS_NUM)
  {
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
  }
  else
  {
    HWTagClasses[id].enabled = enabled;
  }

  return res;
}

sys_error_code_t TMSetSWTagLabel(const char *label, uint8_t id)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  if(id >= HSD_SW_TAG_CLASS_NUM)
  {
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
  }
  else
  {
    (void) snprintf(SWTagClasses[id].label, HSD_TAGS_LABEL_LENGTH, "%s", label);
  }
  return res;
}

sys_error_code_t TMSetHWTagLabel(const char *label, uint8_t id)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if(id >= HSD_HW_TAG_CLASS_NUM)
  {
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
  }
  else
  {
    (void) snprintf(HWTagClasses[id].label, HSD_TAGS_LABEL_LENGTH, "%s", label);
  }
  return res;
}

HSD_SW_Tag_Class_t* TMGetSWTag(uint8_t id)
{
  HSD_SW_Tag_Class_t *res = NULL;

  if(id >= HSD_SW_TAG_CLASS_NUM)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
  }
  else
  {
    res = &SWTagClasses[id];
  }
  return res;
}

HSD_HW_Tag_Class_t* TMGetHWTag(uint8_t id)
{
  HSD_HW_Tag_Class_t *res = NULL;

  if(id >= HSD_HW_TAG_CLASS_NUM)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
  }
  else
  {
    res = &HWTagClasses[id];
  }
  return res;
}

char* TMGetSWTagLabel(uint8_t id)
{
  char *res = NULL;

  if(id >= HSD_SW_TAG_CLASS_NUM)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
  }
  else
  {
    res = SWTagClasses[id].label;
  }
  return res;
}

char* TMGetHWTagLabel(uint8_t id)
{
  if(id >= HSD_HW_TAG_CLASS_NUM)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    return NULL;
  }
  else
  {
    return HWTagClasses[id].label;
  }
}

sys_error_code_t TMInitSWTagStatus(bool status, uint8_t id)
{
  SWTagClasses[id].status = status;
  return 0;
}

sys_error_code_t TMInitHWTagStatus(bool status, uint8_t id)
{
  HWTagClasses[id].status = status;
  return 0;
}

sys_error_code_t TMSetSWTag(bool status, uint8_t id)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if((id >= HSD_SW_TAG_CLASS_NUM) || (SWTagClasses[id].enabled == false) || (TagsN >= HSD_MAX_TAGS_NUM))
  {
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
  }
  else
  {
    TagList[TagsN].timestamp = TMGetTimestamp();
    TagList[TagsN].status = status;
    SWTagClasses[id].status = status;
    (void) snprintf(TagList[TagsN].label, HSD_TAGS_LABEL_LENGTH, "%s", SWTagClasses[id].label);

    _tm tagTime = StartTime;
    int timestamp_int = (int) TagList[TagsN].timestamp;
    tagTime.tm_sec += timestamp_int;

    if(mktime(&tagTime) == -1)
    {
      res = SYS_BASE_ERROR_CODE;
    }
    else
    {
      double timestamp_dec = TagList[TagsN].timestamp - (double) timestamp_int;
      timestamp_dec = timestamp_dec * 1000.0;
      (void) sprintf(TagList[TagsN].abs_timestamp, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", tagTime.tm_year, tagTime.tm_mon + 1, tagTime.tm_mday, tagTime.tm_hour,
                     tagTime.tm_min, tagTime.tm_sec, (int) timestamp_dec);
      // WHY THIS +1 (in months) ???
      //  struct tm {
      //     int tm_sec;         /* seconds,  range 0 to 59          */
      //     int tm_min;         /* minutes, range 0 to 59           */
      //     int tm_hour;        /* hours, range 0 to 23             */
      //     int tm_mday;        /* day of the month, range 1 to 31  */
      //     int tm_mon;         /* month, range 0 to 11             */ <------ months (0..11)
      //     int tm_year;        /* The number of years since 1900   */
      //     int tm_wday;        /* day of the week, range 0 to 6    */
      //     int tm_yday;        /* day in the year, range 0 to 365  */
      //     int tm_isdst;       /* daylight saving time             */
      //  };
      TagsN++;
    }
  }

  return res;
}

sys_error_code_t TMSetHWTag(bool status, uint8_t id)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if((id >= HSD_HW_TAG_CLASS_NUM) || (HWTagClasses[id].enabled == false) || (TagsN >= HSD_MAX_TAGS_NUM))
  {
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
  }
  else
  {
    TagList[TagsN].timestamp = TMGetTimestamp();
    TagList[TagsN].status = status;
    HWTagClasses[id].status = status;
    (void) snprintf(TagList[TagsN].label, HSD_TAGS_LABEL_LENGTH, "%s", HWTagClasses[id].label);

    _tm tagTime = StartTime;
    int timestamp_int = (int) TagList[TagsN].timestamp;
    tagTime.tm_sec += timestamp_int;

    if(mktime(&tagTime) == -1)
    {
      res = SYS_BASE_ERROR_CODE;
    }
    else
    {
      double timestamp_dec = TagList[TagsN].timestamp - (double) timestamp_int;
      timestamp_dec = timestamp_dec * 1000.0;
      sprintf(TagList[TagsN].abs_timestamp, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", tagTime.tm_year, tagTime.tm_mon + 1, tagTime.tm_mday, tagTime.tm_hour,
              tagTime.tm_min, tagTime.tm_sec, (int) timestamp_dec);
      // WHY THIS +1 (in months) ???
      //  struct tm {
      //     int tm_sec;         /* seconds,  range 0 to 59          */
      //     int tm_min;         /* minutes, range 0 to 59           */
      //     int tm_hour;        /* hours, range 0 to 23             */
      //     int tm_mday;        /* day of the month, range 1 to 31  */
      //     int tm_mon;         /* month, range 0 to 11             */ <------ months (0..11)
      //     int tm_year;        /* The number of years since 1900   */
      //     int tm_wday;        /* day of the week, range 0 to 6    */
      //     int tm_yday;        /* day in the year, range 0 to 365  */
      //     int tm_isdst;       /* daylight saving time             */
      //  };
      TagsN++;
    }
  }
  return res;
}

HSD_Tag_t* TMGetTagList(void)
{
  return TagList;
}

uint8_t TMGetTagListSize(void)
{
  return TagsN;
}

double TMGetTimestamp(void)
{
  return SysTsGetTimestampF(SysGetTimestampSrv());
}

sys_error_code_t TMResetTagListCounter(void)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  TagsN = 0;
  return res;
}

sys_error_code_t TMCloseAllOpenedTags(void)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint8_t id;
  for(id = 0; id < HSD_SW_TAG_CLASS_NUM; id++)
  {
    if(SWTagClasses[id].enabled == true && SWTagClasses[id].status == true)
    {
      TMSetSWTag(false, id);
    }
  }
  return res;
}

