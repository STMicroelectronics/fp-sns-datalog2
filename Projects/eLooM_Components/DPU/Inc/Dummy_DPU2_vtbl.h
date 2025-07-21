/**
  ******************************************************************************
  * @file    Dummy_DPUTask_vtbl.h
  * @author  SRA - MCD
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef USER_INC_DUMMY_DPU_VTBL_H_
#define USER_INC_DUMMY_DPU_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


/* IDPU2 virtual functions */
sys_error_code_t Dummy_DPU2_vtblProcess(IDPU2_t *_this, EMData_t in_data, EMData_t out_data); /*!< @sa IDPU2_Process */


#ifdef __cplusplus
}
#endif

#endif /* USER_INC_Dummy_DPUTask_VTBL_H_ */
