/**
  ******************************************************************************
  * @file    LIS2DU12Task_vtbl.h
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
#ifndef LIS2DU12TASK_VTBL_H_
#define LIS2DU12TASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


/* AManagedTask virtual functions */
sys_error_code_t LIS2DU12Task_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t LIS2DU12Task_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                               VOID **pvStackStart, ULONG *pnStackSize, UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice,
                                               ULONG *pnAutoStart, ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t LIS2DU12Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t LIS2DU12Task_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t LIS2DU12Task_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t LIS2DU12Task_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                   EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t LIS2DU12Task_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t LIS2DU12Task_vtblAccGetId(ISourceObservable *_this);
IEventSrc *LIS2DU12Task_vtblGetEventSourceIF(ISourceObservable *_this);
sys_error_code_t LIS2DU12Task_vtblAccGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal);
float LIS2DU12Task_vtblAccGetFS(ISensorMems_t *_this);
float LIS2DU12Task_vtblAccGetSensitivity(ISensorMems_t *_this);
EMData_t LIS2DU12Task_vtblAccGetDataInfo(ISourceObservable *_this);

sys_error_code_t LIS2DU12Task_vtblSensorSetODR(ISensorMems_t *_this, float odr);
sys_error_code_t LIS2DU12Task_vtblSensorSetFS(ISensorMems_t *_this, float fs);
sys_error_code_t LIS2DU12Task_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM);
sys_error_code_t LIS2DU12Task_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t LIS2DU12Task_vtblSensorDisable(ISensor_t *_this);
boolean_t LIS2DU12Task_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t LIS2DU12Task_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t LIS2DU12Task_vtblSensorGetStatus(ISensor_t *_this);
SensorStatus_t *LIS2DU12Task_vtblSensorGetStatusPointer(ISensor_t *_this);

#ifdef __cplusplus
}
#endif

#endif /* LIS2DU12TASK_VTBL_H_ */
