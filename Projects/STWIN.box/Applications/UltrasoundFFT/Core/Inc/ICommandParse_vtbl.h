/**
 ******************************************************************************
 * @file    ICommandParse_vtbl.h
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

#ifndef INCLUDE_ICommandParse_VTBL_H_
#define INCLUDE_ICommandParse_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Create a type name for ICommandParse_vtbl.
 */
typedef struct _ICommandParse_vtbl ICommandParse_vtbl;

struct _ICommandParse_vtbl
{
  sys_error_code_t (*parse_command)(ICommandParse_t *_this, char *commandString);
  sys_error_code_t (*serialize_response)(ICommandParse_t *_this, char **response_name, char **buff, uint32_t *size, uint8_t pretty);
};

struct _ICommandParse_t
{
  /**
   * Pointer to the virtual table for the class.
   */
  const ICommandParse_vtbl *vptr;
};

// Inline functions definition
// ***************************
inline sys_error_code_t IParseCommand(ICommandParse_t *_this, char *commandString)
{
  return _this->vptr->parse_command(_this, commandString);
}

inline sys_error_code_t ISerializeResponse(ICommandParse_t *_this, char **response_name, char **buff, uint32_t *size, uint8_t pretty)
{
  return _this->vptr->serialize_response(_this, response_name, buff, size, pretty);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ICommandParse_VTBL_H_ */
