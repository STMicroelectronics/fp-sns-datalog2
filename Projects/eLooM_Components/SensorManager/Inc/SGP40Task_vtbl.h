/**
  ******************************************************************************
  * @file    SGP40Task_vtbl.h
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
#ifndef SGP40TASK_VTBL_H_
#define SGP40TASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

// AManagedTaskEx virtual functions
sys_error_code_t SGP40Task_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t SGP40Task_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                            VOID **pvStackStart, ULONG *pnStackSize,
                                            UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                            ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t SGP40Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode,
                                                const EPowerMode eNewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t SGP40Task_vtblHandleError(AManagedTask *_this, SysEvent xError); ///< @sa AMTHandleError
sys_error_code_t SGP40Task_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t SGP40Task_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t SGP40Task_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t SGP40Task_vtblTempGetId(ISourceObservable *_this);
IEventSrc *SGP40Task_vtblTempGetEventSourceIF(ISourceObservable *_this);
EMData_t SGP40Task_vtblTempGetDataInfo(ISourceObservable *_this);
sys_error_code_t SGP40Task_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t SGP40Task_vtblSensorDisable(ISensor_t *_this);
boolean_t SGP40Task_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t SGP40Task_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t SGP40Task_vtblSensorGetStatus(ISensor_t *_this);
SensorStatus_t *SGP40Task_vtblSensorGetStatusPointer(ISensor_t *_this);
sys_error_code_t SGP40Task_vtblTempGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal);
float SGP40Task_vtblTempGetFS(ISensorMems_t *_this);
float SGP40Task_vtblTempGetSensitivity(ISensorMems_t *_this);
sys_error_code_t SGP40Task_vtblSensorSetODR(ISensorMems_t *_this, float odr);
sys_error_code_t SGP40Task_vtblSensorSetFS(ISensorMems_t *_this, float fs);
sys_error_code_t SGP40Task_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM);

#ifdef __cplusplus
}
#endif

#endif /* SGP40TASK_VTBL_H_ */
