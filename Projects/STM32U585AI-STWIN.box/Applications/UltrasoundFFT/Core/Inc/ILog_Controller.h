/**
  ******************************************************************************
  * @file    ILog_Controller.h
  * @author  SRA
  * @brief
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:other:log_controller;2
  *
  * Created by: DTDL2PnPL_cGen version 1.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#ifndef INCLUDE_ILOG_CONTROLLER_H_
#define INCLUDE_ILOG_CONTROLLER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

/**
  * Create  type name for ILog_Controller.
  */
typedef struct _ILog_Controller_t ILog_Controller_t;

/* Public API declarations ---------------------------------------------------*/

/* Public interface */
inline uint8_t ILog_Controller_save_config(ILog_Controller_t *_this);
inline uint8_t ILog_Controller_start_log(ILog_Controller_t *_this, uint32_t interface);
inline uint8_t ILog_Controller_stop_log(ILog_Controller_t *_this);
inline uint8_t ILog_Controller_set_time(ILog_Controller_t *_this, const char *datetime);
inline uint8_t ILog_Controller_switch_bank(ILog_Controller_t *_this);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ILOG_CONTROLLER_H_ */
