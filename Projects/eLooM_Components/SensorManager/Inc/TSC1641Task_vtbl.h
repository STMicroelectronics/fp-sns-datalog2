/**
  ******************************************************************************
  * @file    TSC1641Task_vtbl.h
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
#ifndef TSC1641TASK_VTBL_H_
#define TSC1641TASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

// AManagedTaskEx virtual functions
sys_error_code_t TSC1641Task_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t TSC1641Task_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                              VOID **pvStackStart, ULONG *pnStackSize,
                                              UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                              ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t TSC1641Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode,
                                                  const EPowerMode eNewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t TSC1641Task_vtblHandleError(AManagedTask *_this, SysEvent xError); ///< @sa AMTHandleError
sys_error_code_t TSC1641Task_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t TSC1641Task_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                  EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t TSC1641Task_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t TSC1641Task_vtblGetId(ISourceObservable *_this);
IEventSrc *TSC1641Task_vtblGetEventSourceIF(ISourceObservable *_this);
EMData_t TSC1641Task_vtblGetDataInfo(ISourceObservable *_this);
sys_error_code_t TSC1641Task_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t TSC1641Task_vtblSensorDisable(ISensor_t *_this);
boolean_t TSC1641Task_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t TSC1641Task_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t TSC1641Task_vtblSensorGetStatus(ISensor_t *_this);
SensorStatus_t *TSC1641Task_vtblSensorGetStatusPointer(ISensor_t *_this);
uint32_t TSC1641Task_vtblGetADCConversionTime(ISensorPowerMeter_t *_this);
uint32_t TSC1641Task_vtblGetRShunt(ISensorPowerMeter_t *_this);
sys_error_code_t TSC1641Task_vtblSensorSetADCConversionTime(ISensorPowerMeter_t *_this, uint32_t adc_conversion_time);
sys_error_code_t TSC1641Task_vtblSensorSetRShunt(ISensorPowerMeter_t *_this, uint32_t r_shunt);

#ifdef __cplusplus
}
#endif

#endif /* TSC1641TASK_VTBL_H_ */
