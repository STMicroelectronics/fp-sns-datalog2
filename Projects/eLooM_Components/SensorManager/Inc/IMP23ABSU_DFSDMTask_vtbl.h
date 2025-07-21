/**
  ******************************************************************************
  * @file    IMP23ABSUTask_vtbl.h
  * @author  SRA - MCD
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
#ifndef IMP23ABSUTASK_DFSDM_VTBL_H_
#define IMP23ABSUTASK_DFSDM_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


/* AManagedTask virtual functions */
sys_error_code_t IMP23ABSUTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t IMP23ABSUTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                                VOID **pvStackStart, ULONG *pnStackSize, UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice,
                                                ULONG *pnAutoStart, ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t IMP23ABSUTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                    const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t IMP23ABSUTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t IMP23ABSUTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t IMP23ABSUTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                    EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t IMP23ABSUTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                    const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t IMP23ABSUTask_vtblMicGetId(ISourceObservable *_this);
IEventSrc *IMP23ABSUTask_vtblGetEventSourceIF(ISourceObservable *_this);
EMData_t IMP23ABSUTask_vtblMicGetDataInfo(ISourceObservable *_this);
sys_error_code_t IMP23ABSUTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t IMP23ABSUTask_vtblSensorDisable(ISensor_t *_this);
boolean_t IMP23ABSUTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t IMP23ABSUTask_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t IMP23ABSUTask_vtblSensorGetStatus(ISensor_t *_this);
SensorStatus_t *IMP23ABSUTask_vtblSensorGetStatusPointer(ISensor_t *_this);
uint32_t IMP23ABSUTask_vtblMicGetFrequency(ISensorAudio_t *_this);
uint8_t IMP23ABSUTask_vtblMicGetVolume(ISensorAudio_t *_this);
uint8_t IMP23ABSUTask_vtblMicGetResolution(ISensorAudio_t *_this);
sys_error_code_t IMP23ABSUTask_vtblSensorSetFrequency(ISensorAudio_t *_this, uint32_t frequency);
sys_error_code_t IMP23ABSUTask_vtblSensorSetVolume(ISensorAudio_t *_this, uint8_t volume);
sys_error_code_t IMP23ABSUTask_vtblSensorSetResolution(ISensorAudio_t *_this, uint8_t bit_depth);

#ifdef __cplusplus
}
#endif

#endif /* IMP23ABSUTASK_DFSDM_VTBL_H_ */
