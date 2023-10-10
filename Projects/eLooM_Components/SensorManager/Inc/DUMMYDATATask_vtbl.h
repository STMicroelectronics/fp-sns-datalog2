/**
  ******************************************************************************
  * @file    DUMMYSENSORTask_vtbl.h
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
#ifndef DUMMYSENSORTASK_VTBL_H_
#define DUMMYSENSORTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif




/* AManagedTask virtual functions */
sys_error_code_t DUMMYSENSORTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t DUMMYSENSORTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                                  VOID **pvStackStart, ULONG *pnStackSize, UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice,
                                                  ULONG *pnAutoStart, ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t DUMMYSENSORTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                      const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t DUMMYSENSORTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t DUMMYSENSORTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t DUMMYSENSORTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                      EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t DUMMYSENSORTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                      const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t DUMMYSENSORTask_vtblAccGetId(ISourceObservable *_this);
IEventSrc *DUMMYSENSORTask_vtblGetEventSourceIF(ISourceObservable *_this);
sys_error_code_t DUMMYSENSORTask_vtblAccGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal);
float DUMMYSENSORTask_vtblAccGetFS(ISourceObservable *_this);
float DUMMYSENSORTask_vtblAccGetSensitivity(ISourceObservable *_this);
EMData_t DUMMYSENSORTask_vtblAccGetDataInfo(ISourceObservable *_this);

sys_error_code_t DUMMYSENSORTask_vtblSensorSetODR(ISensor_t *_this, float ODR);
sys_error_code_t DUMMYSENSORTask_vtblSensorSetFS(ISensor_t *_this, float FS);
sys_error_code_t DUMMYSENSORTask_vtblSensorSetFifoWM(ISensor_t *_this, uint16_t fifoWM);
sys_error_code_t DUMMYSENSORTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t DUMMYSENSORTask_vtblSensorDisable(ISensor_t *_this);
boolean_t DUMMYSENSORTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t DUMMYSENSORTask_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t DUMMYSENSORTask_vtblSensorGetStatus(ISensor_t *_this);

#ifdef __cplusplus
}
#endif

#endif /* DUMMYSENSORTASK_VTBL_H_ */
