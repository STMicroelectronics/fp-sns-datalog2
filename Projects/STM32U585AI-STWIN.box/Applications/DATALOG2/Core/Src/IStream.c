/**
  ******************************************************************************
  * @file    IStream.c
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

#include "IStream.h"
#include "IStream_vtbl.h"

// GCC requires one function forward declaration in only one .c source
// in order to manage the inline.
// See also http://stackoverflow.com/questions/26503235/c-inline-function-and-gcc
#if defined (__GNUC__) || defined(__ICCARM__)
extern sys_error_code_t IStream_init(IStream_t *_this, uint8_t comm_interface_id, void *param);
extern sys_error_code_t IStream_enable(IStream_t *_this);
extern sys_error_code_t IStream_disable(IStream_t *_this);
extern boolean_t IStream_is_enabled(IStream_t *_this);
extern sys_error_code_t IStream_deinit(IStream_t *_this);
extern sys_error_code_t IStream_start(IStream_t *_this, void *param);
extern sys_error_code_t IStream_stop(IStream_t *_this);
extern sys_error_code_t IStream_post_data(IStream_t *_this, uint8_t id_stream, uint8_t *buf, uint32_t size);
extern sys_error_code_t IStream_alloc_resource(IStream_t *_this, uint8_t id_stream, uint32_t size,
                                               const char *stream_name);
extern sys_error_code_t IStream_set_mode(IStream_t *_this, IStreamMode_t mode);
extern sys_error_code_t IStream_dealloc(IStream_t *_this, uint8_t id_stream);
extern sys_error_code_t IStream_set_parse_IF(IStream_t *_this, ICommandParse_t *ifn);
#endif
