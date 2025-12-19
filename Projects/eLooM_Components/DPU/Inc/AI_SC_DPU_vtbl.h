/**
  ********************************************************************************
  * @file    AI_SC_DPU_vtbl.h
  * @author  STMicroelectronics - AIS - MCD Team
  * @version 1.0.0
  * @date    Oct 25, 2022
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
#ifndef DPU_INC_AI_SC_DPU_VTBL_H_
#define DPU_INC_AI_SC_DPU_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


/* IDPU2 virtual functions */
sys_error_code_t AI_SC_DPU_vtblProcess(IDPU2_t *_this, EMData_t in_data, EMData_t out_data); /*!< @sa IDPU2_Process */


#ifdef __cplusplus
}
#endif

#endif /* DPU_INC_AI_SC_DPU_VTBL_H_ */
