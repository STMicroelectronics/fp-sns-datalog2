/**
  ******************************************************************************
  * @file    IPnPLComponent.c
  * @author  SRA - MCD
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
  ******************************************************************************
  */

#include "IPnPLComponent.h"
#include "IPnPLComponent_vtbl.h"

// GCC requires one function forward declaration in only one .c source
// in order to manage the inline.
// See also http://stackoverflow.com/questions/26503235/c-inline-function-and-gcc
#if defined (__GNUC__) || defined(__ICCARM__)
extern char *IPnPLComponentGetKey(IPnPLComponent_t *_this);
extern uint8_t IPnPLComponentGetNCommands(IPnPLComponent_t *_this);
extern char *IPnPLComponentGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
extern uint8_t IPnPLComponentGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty);
#ifdef PNPL_RESPONSES
  extern uint8_t IPnPLComponentSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size, uint8_t pretty);
  extern uint8_t IPnPLCommandExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size, uint8_t pretty);
#else
  extern uint8_t IPnPLComponentSetProperty(IPnPLComponent_t *_this, char *serializedJSON);
  extern uint8_t IPnPLCommandExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON);
#endif
#endif
