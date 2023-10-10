/**
  ******************************************************************************
  * @file    filex_dctrl_class_vtbl.h
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
#ifndef FILEX_DCTRL_VTBL_CLASS_H_
#define FILEX_DCTRL_VTBL_CLASS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* IStream virtual functions */
sys_error_code_t filex_dctrl_vtblStream_init(IStream_t *_this, uint8_t comm_interface_id, void *param);
sys_error_code_t filex_dctrl_vtblStream_enable(IStream_t *_this);
sys_error_code_t filex_dctrl_vtblStream_disable(IStream_t *_this);
bool filex_dctrl_vtblStream_is_enabled(IStream_t *_this);
sys_error_code_t filex_dctrl_vtblStream_deinit(IStream_t *_this);
sys_error_code_t filex_dctrl_vtblStream_start(IStream_t *_this, void *param);
sys_error_code_t filex_dctrl_vtblStream_stop(IStream_t *_this);
sys_error_code_t filex_dctrl_vtblStream_post_data(IStream_t *_this, uint8_t id_stream, uint8_t *buf, uint32_t size);
sys_error_code_t filex_dctrl_vtblStream_alloc_resource(IStream_t *_this, uint8_t id_stream, uint32_t size,
                                                       const char *stream_name);
sys_error_code_t filex_dctrl_vtblStream_set_mode(IStream_t *_this, IStreamMode_t mode);
sys_error_code_t filex_dctrl_vtblStream_dealloc(IStream_t *_this, uint8_t id_stream);
sys_error_code_t filex_dctrl_vtblStream_set_parse_IF(IStream_t *_this, ICommandParse_t *ifn);

/** Inline functions definition */
/********************************/

#ifdef __cplusplus
}
#endif

#endif /* FILEX_DCTRL_CLASSVTBL__H_ */
