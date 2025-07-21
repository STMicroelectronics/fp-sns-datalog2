/**
  ******************************************************************************
  * @file    FDM_ACOTask_vtbl.h
  * @author  SRA
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */

#ifndef USER_INC_FDM_ACOTASK_VTBL_H_
#define USER_INC_FDM_ACOTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

// AManagedTaskEx virtual functions
sys_error_code_t FDM_ACOTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t FDM_ACOTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                              VOID **pvStackStart, ULONG *pnStackSize,
                                              UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                              ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t FDM_ACOTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode,
                                                  const EPowerMode eNewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t FDM_ACOTask_vtblHandleError(AManagedTask *_this, SysEvent xError); ///< @sa AMTHandleError
sys_error_code_t FDM_ACOTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                  EPowerMode eActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t FDM_ACOTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode eActivePowerMode,
                                                  const EPowerMode eNewPowerMode); ///< @sa AMTExOnEnterPowerMode
sys_error_code_t FDM_ACOTask_vtblOnEnterTaskControlLoop(AManagedTask *_this);

#ifdef __cplusplus
}
#endif

#endif /* USER_INC_FDM_ACOTask_VTBL_H_ */
