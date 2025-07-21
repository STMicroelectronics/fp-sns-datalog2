/**
  ******************************************************************************
  * @file    IIS2DULPXTask_vtbl.h
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
#ifndef IIS2DULPXTASK_VTBL_H_
#define IIS2DULPXTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


/* AManagedTask virtual functions */
sys_error_code_t IIS2DULPXTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t IIS2DULPXTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                                VOID **pvStackStart, ULONG *pnStackSize, UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice,
                                                ULONG *pnAutoStart, ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t IIS2DULPXTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                    const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t IIS2DULPXTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t IIS2DULPXTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t IIS2DULPXTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                    EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t IIS2DULPXTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                    const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t IIS2DULPXTask_vtblAccGetId(ISourceObservable *_this);
IEventSrc *IIS2DULPXTask_vtblGetEventSourceIF(ISourceObservable *_this);
EMData_t IIS2DULPXTask_vtblAccGetDataInfo(ISourceObservable *_this);
uint8_t IIS2DULPXTask_vtblMlcGetId(ISourceObservable *_this);
IEventSrc *IIS2DULPXTask_vtblMlcGetEventSourceIF(ISourceObservable *_this);
EMData_t IIS2DULPXTask_vtblMlcGetDataInfo(ISourceObservable *_this);
sys_error_code_t IIS2DULPXTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t IIS2DULPXTask_vtblSensorDisable(ISensor_t *_this);
boolean_t IIS2DULPXTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t IIS2DULPXTask_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t IIS2DULPXTask_vtblSensorGetStatus(ISensor_t *_this);
SensorStatus_t *IIS2DULPXTask_vtblSensorGetStatusPointer(ISensor_t *_this);
SensorDescriptor_t IIS2DULPXTask_vtblMlcGetDescription(ISensor_t *_this);
SensorStatus_t IIS2DULPXTask_vtblMlcGetStatus(ISensor_t *_this);
SensorStatus_t *IIS2DULPXTask_vtblMlcGetStatusPointer(ISensor_t *_this);
sys_error_code_t IIS2DULPXTask_vtblAccGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal);
float_t IIS2DULPXTask_vtblAccGetFS(ISensorMems_t *_this);
float_t IIS2DULPXTask_vtblAccGetSensitivity(ISensorMems_t *_this);
sys_error_code_t IIS2DULPXTask_vtblMlcGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal);
float_t IIS2DULPXTask_vtblMlcGetFS(ISensorMems_t *_this);
float_t IIS2DULPXTask_vtblMlcGetSensitivity(ISensorMems_t *_this);
sys_error_code_t IIS2DULPXTask_vtblSensorSetODR(ISensorMems_t *_this, float_t odr);
sys_error_code_t IIS2DULPXTask_vtblSensorSetFS(ISensorMems_t *_this, float_t fs);
sys_error_code_t IIS2DULPXTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM);

sys_error_code_t IIS2DULPXTask_vtblSensorReadReg(ISensorLL_t *_this, uint16_t reg, uint8_t *data, uint16_t len);
sys_error_code_t IIS2DULPXTask_vtblSensorWriteReg(ISensorLL_t *_this, uint16_t reg, const uint8_t *data, uint16_t len);
sys_error_code_t IIS2DULPXTask_vtblSensorSyncModel(ISensorLL_t *_this);


#ifdef __cplusplus
}
#endif

#endif /* IIS2DULPXTASK_VTBL_H_ */
