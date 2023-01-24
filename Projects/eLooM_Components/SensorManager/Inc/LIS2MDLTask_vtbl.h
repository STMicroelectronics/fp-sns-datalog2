/**
  ******************************************************************************
  * @file    LIS2MDLTask_vtbl.h
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
#ifndef LIS2MDLTASK_VTBL_H_
#define LIS2MDLTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif




// AManagedTaskEx virtual functions
sys_error_code_t LIS2MDLTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t LIS2MDLTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                              VOID **pvStackStart, ULONG *pnStackSize, UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice,
                                              ULONG *pnAutoStart, ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t LIS2MDLTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode,
                                                  const EPowerMode eNewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t LIS2MDLTask_vtblHandleError(AManagedTask *_this, SysEvent xError); ///< @sa AMTHandleError
sys_error_code_t LIS2MDLTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t LIS2MDLTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                  EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t LIS2MDLTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t LIS2MDLTask_vtblMagGetId(ISourceObservable *_this);
IEventSrc *LIS2MDLTask_vtblMagGetEventSourceIF(ISourceObservable *_this);
sys_error_code_t LIS2MDLTask_vtblMagGetODR(ISourceObservable *_this, float *p_measured, float *p_nominal);
float LIS2MDLTask_vtblMagGetFS(ISourceObservable *_this);
float LIS2MDLTask_vtblMagGetSensitivity(ISourceObservable *_this);
EMData_t LIS2MDLTask_vtblMagGetDataInfo(ISourceObservable *_this);

sys_error_code_t LIS2MDLTask_vtblSensorSetODR(ISensor_t *_this, float ODR);
sys_error_code_t LIS2MDLTask_vtblSensorSetFS(ISensor_t *_this, float FS);
sys_error_code_t LIS2MDLTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t LIS2MDLTask_vtblSensorDisable(ISensor_t *_this);
boolean_t LIS2MDLTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t LIS2MDLTask_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t LIS2MDLTask_vtblSensorGetStatus(ISensor_t *_this);

#ifdef __cplusplus
}
#endif

#endif /* LIS2MDLTASK_VTBL_H_ */
