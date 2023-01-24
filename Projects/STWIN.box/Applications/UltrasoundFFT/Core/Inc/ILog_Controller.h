/**
  ******************************************************************************
  * @file    ILog_Controller.h
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

// Public API declaration
//***********************
/** Public interface **/
inline uint8_t ILog_Controller_start_log(ILog_Controller_t *_this, uint32_t interface);
inline uint8_t ILog_Controller_stop_log(ILog_Controller_t *_this);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ILOG_CONTROLLER_H_ */
