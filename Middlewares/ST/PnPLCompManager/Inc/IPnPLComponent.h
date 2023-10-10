/**
  ******************************************************************************
  * @file    IPnPLComponent.h
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
  ******************************************************************************
  */

#ifndef INCLUDE_IPNPLCOMPONENT_H_
#define INCLUDE_IPNPLCOMPONENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "PnPLCompManager_Conf.h"
/**
  * Create  type name for IPnPLComponent.
  */
typedef struct _IPnPLComponent_t IPnPLComponent_t;

// Public API declaration
//***********************
/** Public interface **/
static inline char *IPnPLComponentGetKey(IPnPLComponent_t *_this);
static inline uint8_t IPnPLComponentGetNCommands(IPnPLComponent_t *_this);
static inline char *IPnPLComponentGetCommandKey(IPnPLComponent_t *_this, uint8_t id);
static inline uint8_t IPnPLComponentGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty);
#ifdef PNPL_RESPONSES
  static inline uint8_t IPnPLComponentSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size, uint8_t pretty);
  static inline uint8_t IPnPLCommandExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size, uint8_t pretty);
#else
  static inline uint8_t IPnPLComponentSetProperty(IPnPLComponent_t *_this, char *serializedJSON);
  static inline uint8_t IPnPLCommandExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON);
#endif
#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_IPNPLCOMPONENT_H_ */
