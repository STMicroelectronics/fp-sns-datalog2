/**
 ******************************************************************************
 * @file    ICommandParse.c
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

#include "ICommandParse.h"
#include "ICommandParse_vtbl.h"

/**
 * GCC requires one function forward declaration in only one .c source
 * in order to manage the inline.
 * See also http://stackoverflow.com/questions/26503235/c-inline-function-and-gcc
 */
#if defined (__GNUC__) || defined(__ICCARM__)
extern sys_error_code_t IParseCommand(ICommandParse_t *_this, char *commandString);
extern sys_error_code_t ISerializeResponse(ICommandParse_t *_this, char **response_name, char **buff, uint32_t *size, uint8_t pretty);
#endif
