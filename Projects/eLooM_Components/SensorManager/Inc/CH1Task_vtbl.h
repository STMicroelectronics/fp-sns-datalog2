/**
  ******************************************************************************
  * @file    CH1Task_vtbl.h
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
#ifndef CH1TASK_VTBL_H_
#define CH1TASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

// AManagedTaskEx virtual functions
sys_error_code_t CH1Task_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t CH1Task_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                          VOID **pvStackStart, ULONG *pnStackSize,
                                          UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                          ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t CH1Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode,
                                              const EPowerMode eNewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t CH1Task_vtblHandleError(AManagedTask *_this, SysEvent xError); ///< @sa AMTHandleError
sys_error_code_t CH1Task_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t CH1Task_vtblForceExecuteStep(AManagedTaskEx *_this,
                                              EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t CH1Task_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                              const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t CH1Task_vtblGetId(ISourceObservable *_this);
IEventSrc *CH1Task_vtblGetEventSourceIF(ISourceObservable *_this);
EMData_t CH1Task_vtblGetDataInfo(ISourceObservable *_this);
sys_error_code_t CH1Task_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t CH1Task_vtblSensorDisable(ISensor_t *_this);
boolean_t CH1Task_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t CH1Task_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t CH1Task_vtblSensorGetStatus(ISensor_t *_this);
SensorStatus_t *CH1Task_vtblSensorGetStatusPointer(ISensor_t *_this);
sys_error_code_t CH1Task_vtblGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal);
float_t CH1Task_vtblGetFS(ISensorMems_t *_this);
float_t CH1Task_vtblGetSensitivity(ISensorMems_t *_this);
sys_error_code_t CH1Task_vtblSensorSetODR(ISensorMems_t *_this, float_t odr);
sys_error_code_t CH1Task_vtblSensorSetFS(ISensorMems_t *_this, float_t fs);
sys_error_code_t CH1Task_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM);

#ifdef __cplusplus
}
#endif

#endif /* CH1TASK_VTBL_H_ */
