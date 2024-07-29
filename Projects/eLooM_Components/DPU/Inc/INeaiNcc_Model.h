/**
  ********************************************************************************
  * @file    INeaiNcc_Model.h
  * @author  STMicroelectronics - SRA Team
  * @version 1.0.0
  * @date    Feb 27, 2024
  *
  * @brief
  *
  * <ADD_FILE_DESCRIPTION>
  *
  ********************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ********************************************************************************
  */
#ifndef DPU_INC_INEAINCC_MODEL_H_
#define DPU_INC_INEAINCC_MODEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/systp.h"
#include "services/systypes.h"
#include "services/syserror.h"

#define SYS_NEAINCC_NO_ERROR_CODE                            0
#ifndef SYS_NEAINCC_BASE_ERROR_CODE
#define SYS_NEAINCC_BASE_ERROR_CODE                          1
#endif
#define SYS_NEAINCC_GENERIC_ERROR_CODE                       SYS_NEAINCC_NO_ERROR_CODE + 1
#define SYS_NEAINCC_NULL_PTR_ERROR_CODE                      SYS_NEAINCC_NO_ERROR_CODE + 2

/**
  * Create  type name for _INeaiNcc_Model_vtbl.
  */
typedef struct _INeaiNcc_Model_vtbl INeaiNcc_Model_vtbl;

/**
  * Create  type name for _INeaiNcc_Model.
  */
typedef struct _INeaiNcc_Model INeaiNcc_Model_t;

/**
  * Specifies the virtual table for the  class.
  */
struct _INeaiNcc_Model_vtbl
{
  uint8_t (*Init)(INeaiNcc_Model_t *_this, const float knowledge_buffer[]);
  uint8_t (*Classification)(INeaiNcc_Model_t *_this, float data_input[], float output_buffer[], uint16_t *id_class);
};

/**
  * INeaiNcc_Model interface internal state.
  * It declares only the virtual table used to implement the inheritance.
  */
struct _INeaiNcc_Model
{
  /**
    * Pointer to the virtual table for the class.
    */
  const INeaiNcc_Model_vtbl *vptr;
};

/* Public API declaration */
/**************************/

/**
  *
  * @param _this [IN] specifies a instance of the interface.
  * @param p_network_config
  * @return
  */
static inline uint8_t INeaiNcc_Init(INeaiNcc_Model_t *_this, const float knowledge_buffer[]);

/**
  *
  * @param _this [IN] specifies a instance of the interface.
  * @param data_input
  * @param output_buffer
  * @param id_class
  * @return
  */
static inline uint8_t INeaiNcc_Classification(INeaiNcc_Model_t *_this, float data_input[], float output_buffer[],
                                              uint16_t *id_class);


/* Inline functions definition */
/*******************************/

static inline uint8_t INeaiNcc_Init(INeaiNcc_Model_t *_this, const float knowledge_buffer[])
{
  return _this->vptr->Init(_this, knowledge_buffer);
}

static inline uint8_t INeaiNcc_Classification(INeaiNcc_Model_t *_this, float data_input[], float output_buffer[],
                                              uint16_t *id_class)
{
  return _this->vptr->Classification(_this, data_input, output_buffer, id_class);
}

#ifdef __cplusplus
}
#endif

#endif /* DPU_INC_INEAINCC_MODEL_H_ */
