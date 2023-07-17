/**
  ******************************************************************************
  * @file    IStream_vtbl.h
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

#ifndef INCLUDE_ISTREAM_VTBL_H_
#define INCLUDE_ISTREAM_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
  * Create a type name for IStream_vtbl.
  */
typedef struct _IStream_vtbl IStream_vtbl;

struct _IStream_vtbl
{
  sys_error_code_t (*init_stream)(IStream_t *_this, uint8_t comm_interface_id, void *param);
  sys_error_code_t (*enable_stream)(IStream_t *_this);
  sys_error_code_t (*disable_stream)(IStream_t *_this);
  boolean_t (*is_enabled_stream)(IStream_t *_this);
  sys_error_code_t (*deinit_stream)(IStream_t *_this);
  sys_error_code_t (*start_stream)(IStream_t *_this, void *param);
  sys_error_code_t (*stop_stream)(IStream_t *_this);
  sys_error_code_t (*post_data_stream)(IStream_t *_this, uint8_t id_stream, uint8_t *buf, uint32_t size);
  sys_error_code_t (*alloc_resource_stream)(IStream_t *_this, uint8_t id_stream, uint32_t size, const char *stream_name);
  sys_error_code_t (*set_mode_stream)(IStream_t *_this, IStreamMode_t mode);
  sys_error_code_t (*dealloc)(IStream_t *_this, uint8_t id_stream);
  sys_error_code_t (*set_parse_IF)(IStream_t *_this, ICommandParse_t *ifn);
};

struct _IStream_t
{
  /**
    * Pointer to the virtual table for the class.
    */
  const IStream_vtbl *vptr;
};

// Inline functions definition
// ***************************

inline sys_error_code_t IStream_init(IStream_t *_this, uint8_t comm_interface_id, void *param)
{
  return _this->vptr->init_stream(_this, comm_interface_id, param);
}

inline sys_error_code_t IStream_enable(IStream_t *_this)
{
  return _this->vptr->enable_stream(_this);
}

inline sys_error_code_t IStream_disable(IStream_t *_this)
{
  return _this->vptr->disable_stream(_this);
}

inline boolean_t IStream_is_enabled(IStream_t *_this)
{
  return _this->vptr->is_enabled_stream(_this);
}

inline sys_error_code_t IStream_deinit(IStream_t *_this)
{
  return _this->vptr->deinit_stream(_this);
}

inline sys_error_code_t IStream_start(IStream_t *_this, void *param)
{
  return _this->vptr->start_stream(_this, param);
}

inline sys_error_code_t IStream_stop(IStream_t *_this)
{
  return _this->vptr->stop_stream(_this);
}
inline sys_error_code_t IStream_post_data(IStream_t *_this, uint8_t id_stream, uint8_t *buf, uint32_t size)
{
  return _this->vptr->post_data_stream(_this, id_stream, buf, size);
}

inline sys_error_code_t IStream_alloc_resource(IStream_t *_this, uint8_t id_stream, uint32_t size,
                                               const char *stream_name)
{
  return _this->vptr->alloc_resource_stream(_this, id_stream, size, stream_name);
}

inline sys_error_code_t IStream_set_mode(IStream_t *_this, IStreamMode_t mode)
{
  return _this->vptr->set_mode_stream(_this, mode);
}

inline sys_error_code_t IStream_dealloc(IStream_t *_this, uint8_t id_stream)
{
  return _this->vptr->dealloc(_this, id_stream);
}

inline sys_error_code_t IStream_set_parse_IF(IStream_t *_this, ICommandParse_t *ifn)
{
  return _this->vptr->set_parse_IF(_this, ifn);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ISTREAM_VTBL_H_ */
