/**
  ******************************************************************************
  * @file    IIS2DLPCTask_vtbl.h
  * @author  SRA - MCD
  *
  * @date    30-Jul-2021
  *
  * @brief
  *
  *
  *
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
#ifndef IIS2DLPCTASK_VTBL_H_
#define IIS2DLPCTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif




/* AManagedTask virtual functions */
sys_error_code_t IIS2DLPCTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t IIS2DLPCTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                               VOID **pvStackStart, ULONG *pnStackSize, UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice,
                                               ULONG *pnAutoStart, ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t IIS2DLPCTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t IIS2DLPCTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t IIS2DLPCTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t IIS2DLPCTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                   EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t IIS2DLPCTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t IIS2DLPCTask_vtblAccGetId(ISourceObservable *_this);
IEventSrc *IIS2DLPCTask_vtblGetEventSourceIF(ISourceObservable *_this);
sys_error_code_t IIS2DLPCTask_vtblAccGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal);
float IIS2DLPCTask_vtblAccGetFS(ISourceObservable *_this);
float IIS2DLPCTask_vtblAccGetSensitivity(ISourceObservable *_this);
EMData_t IIS2DLPCTask_vtblAccGetDataInfo(ISourceObservable *_this);

sys_error_code_t IIS2DLPCTask_vtblSensorSetODR(ISensor_t *_this, float ODR);
sys_error_code_t IIS2DLPCTask_vtblSensorSetFS(ISensor_t *_this, float FS);
sys_error_code_t IIS2DLPCTask_vtblSensorSetFifoWM(ISensor_t *_this, uint16_t fifoWM);
sys_error_code_t IIS2DLPCTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t IIS2DLPCTask_vtblSensorDisable(ISensor_t *_this);
boolean_t IIS2DLPCTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t IIS2DLPCTask_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t IIS2DLPCTask_vtblSensorGetStatus(ISensor_t *_this);

#ifdef __cplusplus
}
#endif

#endif /* IIS2DLPCTASK_VTBL_H_ */
