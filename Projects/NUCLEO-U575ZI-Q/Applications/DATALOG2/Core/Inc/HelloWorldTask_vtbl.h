/**
  ******************************************************************************
  * @file    HelloWorldTask_vtbl.h
  * @author  SRA
  *
  * @brief   Virtual table for the ::HelloWorld_t class
  *
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef HELLOWORLDTASK_VTBL_H_
#define HELLOWORLDTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


/* IManagedTask virtual functions */
sys_error_code_t HelloWorldTask_vtblHardwareInit(AManagedTask *_this, void *params); ///< @sa AMTHardwareInit
sys_error_code_t HelloWorldTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *p_task_code, CHAR **p_name,
                                                 VOID **p_stack_start, ULONG *p_stack_size, UINT *p_priority, UINT *p_preempt_threshold, ULONG *p_time_slice,
                                                 ULONG *p_auto_start, ULONG *p_params); ///< @sa AMTOnCreateTask
sys_error_code_t HelloWorldTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode e_active_power_mode,
                                                     const EPowerMode e_new_power_mode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t HelloWorldTask_vtblHandleError(AManagedTask *_this, SysEvent error); ///< @sa AMTHandleError
sys_error_code_t HelloWorldTask_vtblOnEnterTaskControlLoop(AManagedTask *_this); ///< @sa AMTOnEnterTaskControlLoop

#ifdef __cplusplus
}
#endif

#endif /* HELLOWORLDTASK_VTBL_H_ */
