/**
  ******************************************************************************
  * @file    STTS22HTask_vtbl.h
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
#ifndef STTS22HTASK_VTBL_H_
#define STTS22HTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

// AManagedTaskEx virtual functions
sys_error_code_t STTS22HTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t STTS22HTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                              VOID **pvStackStart, ULONG *pnStackSize,
                                              UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                              ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t STTS22HTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode,
                                                  const EPowerMode eNewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t STTS22HTask_vtblHandleError(AManagedTask *_this, SysEvent xError); ///< @sa AMTHandleError
sys_error_code_t STTS22HTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t STTS22HTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                  EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t STTS22HTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t STTS22HTask_vtblTempGetId(ISourceObservable *_this);
IEventSrc *STTS22HTask_vtblTempGetEventSourceIF(ISourceObservable *_this);
sys_error_code_t STTS22HTask_vtblTempGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal);
float STTS22HTask_vtblTempGetFS(ISourceObservable *_this);
float STTS22HTask_vtblTempGetSensitivity(ISourceObservable *_this);
EMData_t STTS22HTask_vtblTempGetDataInfo(ISourceObservable *_this);

sys_error_code_t STTS22HTask_vtblSensorSetODR(ISensor_t *_this, float ODR);
sys_error_code_t STTS22HTask_vtblSensorSetFS(ISensor_t *_this, float FS);
sys_error_code_t STTS22HTask_vtblSensorSetFifoWM(ISensor_t *_this, uint16_t fifoWM);
sys_error_code_t STTS22HTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t STTS22HTask_vtblSensorDisable(ISensor_t *_this);
boolean_t STTS22HTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t STTS22HTask_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t STTS22HTask_vtblSensorGetStatus(ISensor_t *_this);

#ifdef __cplusplus
}
#endif

#endif /* STTS22HTASK_VTBL_H_ */
