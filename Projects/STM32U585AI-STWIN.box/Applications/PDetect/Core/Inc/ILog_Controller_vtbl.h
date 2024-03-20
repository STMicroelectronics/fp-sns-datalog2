/**
  ******************************************************************************
  * @file    ILog_Controller_vtbl.h
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
  * Created by: DTDL2PnPL_cGen version 1.2.3
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#ifndef INCLUDE_ILOG_CONTROLLER_VTBL_H_
#define INCLUDE_ILOG_CONTROLLER_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
  * Create a type name for ILog_Controller_vtbl.
  */
typedef struct _ILog_Controller_vtbl ILog_Controller_vtbl;

struct _ILog_Controller_vtbl
{
  uint8_t (*log_controller_save_config)(ILog_Controller_t *_this);
  uint8_t (*log_controller_start_log)(ILog_Controller_t *_this, int32_t interface);
  uint8_t (*log_controller_stop_log)(ILog_Controller_t *_this);
  uint8_t (*log_controller_set_time)(ILog_Controller_t *_this, const char *datetime);
  uint8_t (*log_controller_switch_bank)(ILog_Controller_t *_this);
};

struct _ILog_Controller_t
{
  /**
    * Pointer to the virtual table for the class.
    */
  const ILog_Controller_vtbl *vptr;
};

/* Inline functions definition -----------------------------------------------*/
inline uint8_t ILog_Controller_save_config(ILog_Controller_t *_this)
{
  return _this->vptr->log_controller_save_config(_this);
}
inline uint8_t ILog_Controller_start_log(ILog_Controller_t *_this, int32_t interface)
{
  return _this->vptr->log_controller_start_log(_this, interface);
}
inline uint8_t ILog_Controller_stop_log(ILog_Controller_t *_this)
{
  return _this->vptr->log_controller_stop_log(_this);
}
inline uint8_t ILog_Controller_set_time(ILog_Controller_t *_this, const char *datetime)
{
  return _this->vptr->log_controller_set_time(_this, datetime);
}
inline uint8_t ILog_Controller_switch_bank(ILog_Controller_t *_this)
{
  return _this->vptr->log_controller_switch_bank(_this);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ILOG_CONTROLLER_VTBL_H_ */
