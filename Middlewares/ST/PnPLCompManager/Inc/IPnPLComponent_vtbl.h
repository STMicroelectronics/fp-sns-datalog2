/**
  ******************************************************************************
  * @file    IPnPLComponent_vtbl.h
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

#ifndef INCLUDE_IPNPLCOMPONENT_VTBL_H_
#define INCLUDE_IPNPLCOMPONENT_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "PnPLCompManager_Conf.h"

/**
  * Create a type name for IPnPLComponent_vtbl.
  */
typedef struct _IPnPLComponent_vtbl IPnPLComponent_vtbl;

struct _IPnPLComponent_vtbl
{
  char *(*PnPLComponentGetKey)(IPnPLComponent_t *_this);
  uint8_t (*PnPLComponentGetNCommands)(IPnPLComponent_t *_this);
  char *(*PnPLComponentGetCommandsKey)(IPnPLComponent_t *_this, uint8_t id);
  uint8_t (*PnPLComponentGetStatus)(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size, uint8_t pretty);
#ifdef PNPL_RESPONSES
  uint8_t (*PnPLComponentSetProperty)(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size, uint8_t pretty);
  uint8_t (*PnPLCommandExecuteFunction)(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size, uint8_t pretty);
#else
  uint8_t (*PnPLComponentSetProperty)(IPnPLComponent_t *_this, char *serializedJSON);
  uint8_t (*PnPLCommandExecuteFunction)(IPnPLComponent_t *_this, char *serializedJSON);
#endif
};

struct _IPnPLComponent_t
{
  /**
    * Pointer to the virtual table for the class.
    */
  const IPnPLComponent_vtbl *vptr;
};

// Inline functions definition
// ***************************
static inline char *IPnPLComponentGetKey(IPnPLComponent_t *_this)
{
  return _this->vptr->PnPLComponentGetKey(_this);
}

static inline uint8_t IPnPLComponentGetNCommands(IPnPLComponent_t *_this)
{
  return _this->vptr->PnPLComponentGetNCommands(_this);
}

static inline char *IPnPLComponentGetCommandKey(IPnPLComponent_t *_this, uint8_t id)
{
  return _this->vptr->PnPLComponentGetCommandsKey(_this, id);
}

static inline uint8_t IPnPLComponentGetStatus(IPnPLComponent_t *_this, char **serializedJSON, uint32_t *size,
                                              uint8_t pretty)
{
  return _this->vptr->PnPLComponentGetStatus(_this, serializedJSON, size, pretty);
}

#ifdef PNPL_RESPONSES
  static inline uint8_t IPnPLComponentSetProperty(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size, uint8_t pretty)
  {
    return _this->vptr->PnPLComponentSetProperty(_this, serializedJSON, response, size, pretty);
  }

  static inline uint8_t IPnPLCommandExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON, char **response, uint32_t *size, uint8_t pretty)
  {
    return _this->vptr->PnPLCommandExecuteFunction(_this, serializedJSON, response, size, pretty);
  }
#else
  static inline uint8_t IPnPLComponentSetProperty(IPnPLComponent_t *_this, char *serializedJSON)
  {
    return _this->vptr->PnPLComponentSetProperty(_this, serializedJSON);
  }

  static inline uint8_t IPnPLCommandExecuteFunction(IPnPLComponent_t *_this, char *serializedJSON)
  {
    return _this->vptr->PnPLCommandExecuteFunction(_this, serializedJSON);
  }
#endif

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_IPNPLCOMPONENT_VTBL_H_ */
