/**
  ******************************************************************************
  * @file    MP23DB01HPTask_vtbl.h
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

#ifndef MP23DB01HPTASK_VTBL_H_
#define MP23DB01HPTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


/* AManagedTask virtual functions */
sys_error_code_t MP23DB01HPTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t MP23DB01HPTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                                 VOID **pvStackStart, ULONG *pnStackSize, UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice,
                                                 ULONG *pnAutoStart, ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t MP23DB01HPTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t MP23DB01HPTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t MP23DB01HPTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t MP23DB01HPTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                     EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t MP23DB01HPTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t MP23DB01HPTask_vtblMicGetId(ISourceObservable *_this);
IEventSrc *MP23DB01HPTask_vtblGetEventSourceIF(ISourceObservable *_this);
EMData_t MP23DB01HPTask_vtblMicGetDataInfo(ISourceObservable *_this);
sys_error_code_t MP23DB01HPTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t MP23DB01HPTask_vtblSensorDisable(ISensor_t *_this);
boolean_t MP23DB01HPTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t MP23DB01HPTask_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t MP23DB01HPTask_vtblSensorGetStatus(ISensor_t *_this);
SensorStatus_t *MP23DB01HPTask_vtblSensorGetStatusPointer(ISensor_t *_this);
uint32_t MP23DB01HPTask_vtblMicGetFrequency(ISensorAudio_t *_this);
uint8_t MP23DB01HPTask_vtblMicGetVolume(ISensorAudio_t *_this);
uint8_t MP23DB01HPTask_vtblMicGetResolution(ISensorAudio_t *_this);
sys_error_code_t MP23DB01HPTask_vtblSensorSetFrequency(ISensorAudio_t *_this, uint32_t frequency);
sys_error_code_t MP23DB01HPTask_vtblSensorSetVolume(ISensorAudio_t *_this, uint8_t volume);
sys_error_code_t MP23DB01HPTask_vtblSensorSetResolution(ISensorAudio_t *_this, uint8_t bit_depth);

#ifdef __cplusplus
}
#endif

#endif /* MP23DB01HPTASK_VTBL_H_ */
