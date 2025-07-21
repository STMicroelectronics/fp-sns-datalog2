/**
  ******************************************************************************
  * @file    TagManager_conf.h
  * @author  SRA - MCD
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
#ifndef _TAGMANAGER_CONF_H_
#define _TAGMANAGER_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

//acq_info/Tags
#define HSD_TAGS_LABEL_LENGTH     25U
#define HSD_TAGS_PINDESC_LENGTH   8U
#define HSD_TAGS_TIMESTAMP_LENGTH 25U

#define HSD_SW_TAG_CLASS_NUM      5U //automatically counted from "sw_tag" key in Comment Property field
#define HSD_HW_TAG_CLASS_NUM      5U //automatically counted from "hw_tag" key in Comment Property field
#define HSD_MAX_TAGS_NUM          100U

#define HSD_TAGS_DEFAULT_SW       "SW_TAG_%d"
#define HSD_TAGS_DEFAULT_HW       "HW_TAG_%d"

#ifdef __cplusplus
}
#endif

#endif /* _TAGMANAGER_CONF_H_ */
