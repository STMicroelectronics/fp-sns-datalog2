/**
  ******************************************************************************
  * @file    SHT40Task_vtbl.h
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
#ifndef SHT40TASK_VTBL_H_
#define SHT40TASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

// AManagedTaskEx virtual functions
sys_error_code_t SHT40Task_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t SHT40Task_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                            VOID **pvStackStart, ULONG *pnStackSize,
                                            UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                            ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t SHT40Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode,
                                                const EPowerMode eNewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t SHT40Task_vtblHandleError(AManagedTask *_this, SysEvent xError); ///< @sa AMTHandleError
sys_error_code_t SHT40Task_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t SHT40Task_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t SHT40Task_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t SHT40Task_vtblTempGetId(ISourceObservable *_this);
IEventSrc *SHT40Task_vtblTempGetEventSourceIF(ISourceObservable *_this);
EMData_t SHT40Task_vtblTempGetDataInfo(ISourceObservable *_this);
uint8_t SHT40Task_vtblHumGetId(ISourceObservable *_this);
IEventSrc *SHT40Task_vtblHumGetEventSourceIF(ISourceObservable *_this);
EMData_t SHT40Task_vtblHumGetDataInfo(ISourceObservable *_this);
sys_error_code_t SHT40Task_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t SHT40Task_vtblSensorDisable(ISensor_t *_this);
boolean_t SHT40Task_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t SHT40Task_vtblTempSensorGetDescription(ISensor_t *_this);
SensorStatus_t SHT40Task_vtblTempSensorGetStatus(ISensor_t *_this);
SensorStatus_t *SHT40Task_vtblTempSensorGetStatusPointer(ISensor_t *_this);
SensorDescriptor_t SHT40Task_vtblHumSensorGetDescription(ISensor_t *_this);
SensorStatus_t SHT40Task_vtblHumSensorGetStatus(ISensor_t *_this);
SensorStatus_t *SHT40Task_vtblHumSensorGetStatusPointer(ISensor_t *_this);
sys_error_code_t SHT40Task_vtblTempGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal);
float SHT40Task_vtblTempGetFS(ISensorMems_t *_this);
float SHT40Task_vtblTempGetSensitivity(ISensorMems_t *_this);
sys_error_code_t SHT40Task_vtblHumGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal);
float SHT40Task_vtblHumGetFS(ISensorMems_t *_this);
float SHT40Task_vtblHumGetSensitivity(ISensorMems_t *_this);
sys_error_code_t SHT40Task_vtblSensorSetODR(ISensorMems_t *_this, float odr);
sys_error_code_t SHT40Task_vtblSensorSetFS(ISensorMems_t *_this, float fs);
sys_error_code_t SHT40Task_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM);

#ifdef __cplusplus
}
#endif

#endif /* SHT40TASK_VTBL_H_ */
