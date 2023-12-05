/**
  ******************************************************************************
  * @file    VD6283TXTask_vtbl.h
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
#ifndef VD6283TXTASK_VTBL_H_
#define VD6283TXTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* AManagedTask virtual functions */
sys_error_code_t VD6283TXTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t VD6283TXTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                               VOID **pvStackStart, ULONG *pnStackSize,
                                               UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                               ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t VD6283TXTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t VD6283TXTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t VD6283TXTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t VD6283TXTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                   EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t VD6283TXTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t VD6283TXTask_vtblLightGetId(ISourceObservable *_this);
IEventSrc *VD6283TXTask_vtblLightGetEventSourceIF(ISourceObservable *_this);
EMData_t VD6283TXTask_vtblLightGetDataInfo(ISourceObservable *_this);
sys_error_code_t VD6283TXTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t VD6283TXTask_vtblSensorDisable(ISensor_t *_this);
boolean_t VD6283TXTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t VD6283TXTask_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t VD6283TXTask_vtblSensorGetStatus(ISensor_t *_this);
sys_error_code_t VD6283TXTask_vtblLightGetIntermeasurementTime(ISensorLight_t *_this, uint32_t *p_measured,
                                                               uint32_t *p_nominal);
float VD6283TXTask_vtblLightGetExposureTime(ISensorLight_t *_this);
sys_error_code_t VD6283TXTask_vtblLightGetLightGain(ISensorLight_t *_this, float *LightGain);
sys_error_code_t VD6283TXTask_vtblSensorSetIntermeasurementTime(ISensorLight_t *_this, uint32_t intermeasurement_time);
sys_error_code_t VD6283TXTask_vtblSensorSetExposureTime(ISensorLight_t *_this, float exposure_time);
sys_error_code_t VD6283TXTask_vtblSensorSetLightGain(ISensorLight_t *_this, float LightGain, uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif /* VD6283TXTASK_VTBL_H_ */
