/**
  ******************************************************************************
  * @file    FAKESENSORTask_vtbl.h
  * @author  SRA - MCD
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
#ifndef FAKESENSORTASK_VTBL_H_
#define FAKESENSORTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif



/* AManagedTask virtual functions */
sys_error_code_t FAKESENSORTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t FAKESENSORTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                                 VOID **pvStackStart, ULONG *pnStackSize, UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice,
                                                 ULONG *pnAutoStart, ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t FAKESENSORTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t FAKESENSORTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t FAKESENSORTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t FAKESENSORTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                     EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t FAKESENSORTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t FAKESENSORTask_vtblMicGetId(ISourceObservable *_this);
IEventSrc *FAKESENSORTask_vtblGetEventSourceIF(ISourceObservable *_this);
sys_error_code_t FAKESENSORTask_vtblMicGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal);
float FAKESENSORTask_vtblMicGetFS(ISourceObservable *_this);
float FAKESENSORTask_vtblMicGetSensitivity(ISourceObservable *_this);
EMData_t FAKESENSORTask_vtblMicGetDataInfo(ISourceObservable *_this);

sys_error_code_t FAKESENSORTask_vtblSensorSetODR(ISensor_t *_this, float ODR);
sys_error_code_t FAKESENSORTask_vtblSensorSetFS(ISensor_t *_this, float FS);
sys_error_code_t FAKESENSORTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t FAKESENSORTask_vtblSensorDisable(ISensor_t *_this);
boolean_t FAKESENSORTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t FAKESENSORTask_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t FAKESENSORTask_vtblSensorGetStatus(ISensor_t *_this);

#ifdef __cplusplus
}
#endif

#endif /* FAKESENSORTASK_VTBL_H_ */
