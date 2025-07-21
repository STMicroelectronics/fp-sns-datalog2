/**
  ******************************************************************************
  * @file    TagManager.h
  * @author  SRA - MCD
  * @version 1.0.0
  * @date    25-July-2022
  * @brief   This file provides a set of functions to handle Hardware and
  *          Software Tags
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef _TAGMANAGER_H_
#define _TAGMANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "TagManager_conf.h"
#include "services/syserror.h"
#include "services/sysmem.h"
#include <time.h>

//acq_info/Tags
#define HSD_TAGS_LABEL_LENGTH     25U
#define HSD_TAGS_PINDESC_LENGTH   8U
#define HSD_TAGS_TIMESTAMP_LENGTH 25U

#ifndef HSD_SW_TAG_CLASS_NUM
#define HSD_SW_TAG_CLASS_NUM      5U //automatically counted from "sw_tag" key in Comment Property field
#endif
#ifndef HSD_HW_TAG_CLASS_NUM
#define HSD_HW_TAG_CLASS_NUM      5U //automatically counted from "hw_tag" key in Comment Property field
#endif
#ifndef HSD_MAX_TAGS_NUM
#define HSD_MAX_TAGS_NUM          100U
#endif

#define HSD_TAGS_DEFAULT_SW       "SW_TAG_%d"
#define HSD_TAGS_DEFAULT_HW       "HW_TAG_%d"

typedef struct tm _tm;

typedef struct _HSD_SW_Tag_Class_t
{
  char label[HSD_TAGS_LABEL_LENGTH];
  bool enabled;
  bool status;
} HSD_SW_Tag_Class_t;

typedef struct _HSD_HW_Tag_Class_t
{
  char label[HSD_TAGS_LABEL_LENGTH];
  char pin_desc[HSD_TAGS_PINDESC_LENGTH];
  bool enabled; //e
  bool status;
} HSD_HW_Tag_Class_t;

typedef struct _HSD_Tag_t
{
  char label[HSD_TAGS_LABEL_LENGTH]; //l
  bool status; //e
  double_t timestamp; //tr
  char abs_timestamp[HSD_TAGS_TIMESTAMP_LENGTH]; //"ta": "YYYY-MM-DDTHH:MM:SS.mmmZ"
} HSD_Tag_t;

/* Public API declaration */
/**************************/
sys_error_code_t TMInit(uint8_t n_sw_tags, uint8_t n_hw_tags);
sys_error_code_t TMSetStartTime(_tm start_time);
sys_error_code_t TMCalculateEndTime(char *end_time);
sys_error_code_t TMEnableSWTag(bool status, uint8_t id);
sys_error_code_t TMEnableHWTag(bool status, uint8_t id);
sys_error_code_t TMSetSWTagLabel(const char *label, uint8_t id);
sys_error_code_t TMSetHWTagLabel(const char *label, uint8_t id);
HSD_SW_Tag_Class_t *TMGetSWTag(uint8_t id);
HSD_HW_Tag_Class_t *TMGetHWTag(uint8_t id);
char *TMGetSWTagLabel(uint8_t id);
char *TMGetHWTagLabel(uint8_t id);

sys_error_code_t TMInitSWTagStatus(bool status, uint8_t id);
sys_error_code_t TMInitHWTagStatus(bool status, uint8_t id);
sys_error_code_t TMSetSWTag(bool enable, uint8_t id);
sys_error_code_t TMSetHWTag(bool enable, uint8_t id);
HSD_Tag_t *TMGetTagList(void);
uint8_t TMGetTagListSize(void);
sys_error_code_t TMResetTagListCounter(void);
sys_error_code_t TMCloseAllOpenedTags(void);

/* Inline functions definition */
/*******************************/

#ifdef __cplusplus
}
#endif

#endif /* _TAGMANAGER_H_ */
