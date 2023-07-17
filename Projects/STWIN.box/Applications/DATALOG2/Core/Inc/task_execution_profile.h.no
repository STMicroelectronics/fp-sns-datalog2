/**
  ******************************************************************************
  * @file    task_execution_profile.h
  * @author  SRA
  * @brief   Application Controller Task
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
  
#ifndef TASK_EXE_PROFILE_H_
#define TASK_EXE_PROFILE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Public function declaration */
/*******************************/
void _tx_execution_thread_enter(void);
void _tx_execution_thread_exit(void);

#ifdef __cplusplus
}
#endif

#endif /* TASK_EXE_PROFILE_H_ */
