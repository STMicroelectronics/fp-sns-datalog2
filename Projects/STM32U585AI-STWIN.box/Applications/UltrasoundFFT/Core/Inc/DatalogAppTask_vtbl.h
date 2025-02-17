/**
  ******************************************************************************
  * @file    DatalogAppTask_vtbl.h
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
#ifndef DatalogAppTASK_VTBL_H_
#define DatalogAppTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* AManagedTask virtual functions */
sys_error_code_t DatalogAppTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t DatalogAppTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                                 VOID **pvStackStart, ULONG *pnStackSize,
                                                 UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                                 ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t DatalogAppTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t DatalogAppTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t DatalogAppTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t DatalogAppTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                     EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t DatalogAppTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

//// IIListener virtual functions
sys_error_code_t algorithmEvtListener_OnStatusChange_vtbl(IListener *_this);

// IProcessListener virtual functions
void *algorithmEvtListener_ProcessGetOwner_vtbl(IEventListener *_this);
void algorithmEvtListener_ProcessSetOwner_vtbl(IEventListener *_this, void *pxOwner);
sys_error_code_t algorithmEvtListener_ProcessOnNewDataReady_vtbl(IEventListener *_this, const DataEvent_t *p_evt);

// ICommandParse_t virtual functions
sys_error_code_t DatalogAppTask_vtblICommandParse_t_parse_cmd(ICommandParse_t *_this, char *commandString,
                                                              uint8_t mode);
sys_error_code_t DatalogAppTask_vtblICommandParse_t_serialize_response(ICommandParse_t *_this, char **response_name,
                                                                       char **buff, uint32_t *size,
                                                                       uint8_t pretty);

// ILogController_t virtual functions
//uint8_t DatalogAppTask_save_config_vtbl(ILog_Controller_t *_this); //NULL
uint8_t DatalogAppTask_start_vtbl(ILog_Controller_t *_this, uint32_t interface);
uint8_t DatalogAppTask_stop_vtbl(ILog_Controller_t *_this);
uint8_t DatalogAppTask_set_time_vtbl(ILog_Controller_t *_this, const char *datetime);
//uint8_t DatalogAppTask_switch_bank_vtbl(ILog_Controller_t *_this); //NULL

#ifdef __cplusplus
}
#endif

#endif /* DatalogAppTASK_VTBL_H_ */
