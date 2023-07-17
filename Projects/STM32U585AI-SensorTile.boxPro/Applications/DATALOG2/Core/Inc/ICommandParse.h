/**
  ******************************************************************************
  * @file    ICommandParse.h
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

#ifndef INCLUDE_ICommandParse_H_
#define INCLUDE_ICommandParse_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/systypes.h"
#include "services/syserror.h"
#include "services/systp.h"

/**
  * Create  type name for ICommandParse.
  */
typedef struct _ICommandParse_t ICommandParse_t;

// Public API declaration
//***********************
/** Public interface **/
inline sys_error_code_t IParseCommand(ICommandParse_t *_this, char *commandString, uint8_t comm_interface_id);
inline sys_error_code_t ISerializeResponse(ICommandParse_t *_this, char **response_name, char **buff, uint32_t *size, uint8_t pretty);
inline sys_error_code_t ISendCtrlMsg(ICommandParse_t *_this,  uint32_t *msg, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ICommandParse_H_ */
