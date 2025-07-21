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

#define HSD_SW_TAG_CLASS_NUM 16U //automatically counted from "sw_tag" key in Comment Property field
#define HSD_HW_TAG_CLASS_NUM 16U //automatically counted from "hw_tag" key in Comment Property field

#ifdef __cplusplus
}
#endif

#endif /* _TAGMANAGER_CONF_H_ */
