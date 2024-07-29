/**
  ******************************************************************************
  * @file    IIS2MDCTask_vtbl.h
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
#ifndef IIS2MDCTASK_VTBL_H_
#define IIS2MDCTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


// AManagedTaskEx virtual functions
sys_error_code_t IIS2MDCTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t IIS2MDCTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                              VOID **pvStackStart, ULONG *pnStackSize, UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice,
                                              ULONG *pnAutoStart, ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t IIS2MDCTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode,
                                                  const EPowerMode eNewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t IIS2MDCTask_vtblHandleError(AManagedTask *_this, SysEvent xError); ///< @sa AMTHandleError
sys_error_code_t IIS2MDCTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t IIS2MDCTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                  EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t IIS2MDCTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t IIS2MDCTask_vtblMagGetId(ISourceObservable *_this);
IEventSrc *IIS2MDCTask_vtblMagGetEventSourceIF(ISourceObservable *_this);
EMData_t IIS2MDCTask_vtblMagGetDataInfo(ISourceObservable *_this);
sys_error_code_t IIS2MDCTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t IIS2MDCTask_vtblSensorDisable(ISensor_t *_this);
boolean_t IIS2MDCTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t IIS2MDCTask_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t IIS2MDCTask_vtblSensorGetStatus(ISensor_t *_this);
SensorStatus_t *IIS2MDCTask_vtblSensorGetStatusPointer(ISensor_t *_this);
sys_error_code_t IIS2MDCTask_vtblMagGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal);
float IIS2MDCTask_vtblMagGetFS(ISensorMems_t *_this);
float IIS2MDCTask_vtblMagGetSensitivity(ISensorMems_t *_this);
sys_error_code_t IIS2MDCTask_vtblSensorSetODR(ISensorMems_t *_this, float odr);
sys_error_code_t IIS2MDCTask_vtblSensorSetFS(ISensorMems_t *_this, float fs);
sys_error_code_t IIS2MDCTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM);

#ifdef __cplusplus
}
#endif

#endif /* IIS2MDCTASK_VTBL_H_ */
