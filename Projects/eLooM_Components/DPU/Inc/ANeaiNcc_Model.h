/**
  ********************************************************************************
  * @file    ANeaiNcc_Model.h
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
#ifndef DPU_INC_ANEAINCC_MODEL_H_
#define DPU_INC_ANEAINCC_MODEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "INeaiNcc_Model.h"

/**
  * Create  type name for _ANeaiNcc_Model.
  */
typedef struct _ANeaiNcc_Model ANeaiNcc_Model_t;

/**
  * @brief Internal structure for ANeaiNcc_Model_t.
  * @details This is an abstract class declaration.
  */
struct _ANeaiNcc_Model
{
  /**
    * @brief Base class object.
    */
  INeaiNcc_Model_t super;

  /**
    * @brief Number of axes in the Neai ncc model.
    */
  uint32_t axis_number;

  /**
    * @brief User dimension of the input data.
    */
  uint32_t data_input_user_dim;

  /**
    * @brief Number of classes in the Neai ncc model.
    */
  uint32_t class_number;

  /**
    * @brief Pointer to the knowledge used by the Neai ncc model.
    */
  float_t const *p_knowledge;
};

/* Public API declaration */
/**************************/


#ifdef __cplusplus
}
#endif

#endif /* DPU_INC_ANEAINCC_MODEL_H_ */
