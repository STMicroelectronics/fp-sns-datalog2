/**
  ******************************************************************************
  * @file    IStream.h
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

#ifndef INCLUDE_ISTREAM_H_
#define INCLUDE_ISTREAM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/systypes.h"
#include "services/syserror.h"
#include "services/systp.h"
#include "ICommandParse.h"
#include "ICommandParse_vtbl.h"

typedef enum _IStreamMode_t
{
  RECEIVE = 0, TRANSMIT
} IStreamMode_t;

/**
  * Create  type name for IStream.
  */
typedef struct _IStream_t IStream_t;

// Public API declaration
//***********************
/** Public interface **/
inline sys_error_code_t IStream_init(IStream_t *_this, uint8_t comm_interface_id, void *param);
inline sys_error_code_t IStream_enable(IStream_t *_this);
inline sys_error_code_t IStream_disable(IStream_t *_this);
inline boolean_t IStream_is_enabled(IStream_t *_this);
inline sys_error_code_t IStream_deinit(IStream_t *_this);
inline sys_error_code_t IStream_start(IStream_t *_this, void *param);
inline sys_error_code_t IStream_stop(IStream_t *_this);
inline sys_error_code_t IStream_post_data(IStream_t *_this, uint8_t id_stream, uint8_t *buf, uint32_t size);
inline sys_error_code_t IStream_alloc_resource(IStream_t *_this, uint8_t id_stream, uint32_t size,
                                               const char *stream_name);
inline sys_error_code_t IStream_set_mode(IStream_t *_this, IStreamMode_t mode);
inline sys_error_code_t IStream_dealloc(IStream_t *_this, uint8_t id_stream);
inline sys_error_code_t IStream_set_parse_IF(IStream_t *_this, ICommandParse_t *ifn);
#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ISTREAM_H_ */
