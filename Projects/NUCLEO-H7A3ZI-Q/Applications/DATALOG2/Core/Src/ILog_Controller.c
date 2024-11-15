/**
  ******************************************************************************
  * @file    ILog_Controller.c
  * @author  SRA
  * @brief
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:other:log_controller;2
  *
  * Created by: DTDL2PnPL_cGen version 1.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#include "ILog_Controller.h"
#include "ILog_Controller_vtbl.h"

/**
  * GCC requires one function forward declaration in only one .c source
  * in order to manage the inline.
  * See also http://stackoverflow.com/questions/26503235/c-inline-function-and-gcc
  */
#if defined (__GNUC__) || defined(__ICCARM__)
extern uint8_t ILog_Controller_save_config(ILog_Controller_t *_this);
extern uint8_t ILog_Controller_start_log(ILog_Controller_t *_this, uint32_t interface);
extern uint8_t ILog_Controller_stop_log(ILog_Controller_t *_this);
extern uint8_t ILog_Controller_set_time(ILog_Controller_t *_this, const char *datetime);
extern uint8_t ILog_Controller_switch_bank(ILog_Controller_t *_this);
#endif
