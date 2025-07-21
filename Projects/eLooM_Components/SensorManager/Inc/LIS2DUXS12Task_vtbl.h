/**
  ******************************************************************************
  * @file    LIS2DUXS12Task_vtbl.h
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
#ifndef LIS2DUXS12TASK_VTBL_H_
#define LIS2DUXS12TASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


/* AManagedTask virtual functions */
sys_error_code_t LIS2DUXS12Task_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t LIS2DUXS12Task_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                                 VOID **pvStackStart, ULONG *pnStackSize, UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice,
                                                 ULONG *pnAutoStart, ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t LIS2DUXS12Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t LIS2DUXS12Task_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t LIS2DUXS12Task_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t LIS2DUXS12Task_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                     EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t LIS2DUXS12Task_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t LIS2DUXS12Task_vtblAccGetId(ISourceObservable *_this);
IEventSrc *LIS2DUXS12Task_vtblGetEventSourceIF(ISourceObservable *_this);
EMData_t LIS2DUXS12Task_vtblAccGetDataInfo(ISourceObservable *_this);
uint8_t LIS2DUXS12Task_vtblMlcGetId(ISourceObservable *_this);
IEventSrc *LIS2DUXS12Task_vtblMlcGetEventSourceIF(ISourceObservable *_this);
EMData_t LIS2DUXS12Task_vtblMlcGetDataInfo(ISourceObservable *_this);
sys_error_code_t LIS2DUXS12Task_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t LIS2DUXS12Task_vtblSensorDisable(ISensor_t *_this);
boolean_t LIS2DUXS12Task_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t LIS2DUXS12Task_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t LIS2DUXS12Task_vtblSensorGetStatus(ISensor_t *_this);
SensorStatus_t *LIS2DUXS12Task_vtblSensorGetStatusPointer(ISensor_t *_this);
SensorDescriptor_t LIS2DUXS12Task_vtblMlcGetDescription(ISensor_t *_this);
SensorStatus_t LIS2DUXS12Task_vtblMlcGetStatus(ISensor_t *_this);
SensorStatus_t *LIS2DUXS12Task_vtblMlcGetStatusPointer(ISensor_t *_this);
sys_error_code_t LIS2DUXS12Task_vtblAccGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal);
float_t LIS2DUXS12Task_vtblAccGetFS(ISensorMems_t *_this);
float_t LIS2DUXS12Task_vtblAccGetSensitivity(ISensorMems_t *_this);
sys_error_code_t LIS2DUXS12Task_vtblMlcGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal);
float_t LIS2DUXS12Task_vtblMlcGetFS(ISensorMems_t *_this);
float_t LIS2DUXS12Task_vtblMlcGetSensitivity(ISensorMems_t *_this);
sys_error_code_t LIS2DUXS12Task_vtblSensorSetODR(ISensorMems_t *_this, float_t odr);
sys_error_code_t LIS2DUXS12Task_vtblSensorSetFS(ISensorMems_t *_this, float_t fs);
sys_error_code_t LIS2DUXS12Task_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM);

sys_error_code_t LIS2DUXS12Task_vtblSensorReadReg(ISensorLL_t *_this, uint16_t reg, uint8_t *data, uint16_t len);
sys_error_code_t LIS2DUXS12Task_vtblSensorWriteReg(ISensorLL_t *_this, uint16_t reg, const uint8_t *data, uint16_t len);
sys_error_code_t LIS2DUXS12Task_vtblSensorSyncModel(ISensorLL_t *_this);


#ifdef __cplusplus
}
#endif

#endif /* LIS2DUXS12TASK_VTBL_H_ */
