/**
  ******************************************************************************
  * @file    VL53L9Task_vtbl.h
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
  ******************************************************************************
  */
#ifndef VL53L9TASK_VTBL_H_
#define VL53L9TASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

sys_error_code_t VL53L9Task_vtblHardwareInit(AManagedTask *_this, void *pParams);
sys_error_code_t VL53L9Task_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                             VOID **pvStackStart, ULONG *pnStackSize,
                                             UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                             ULONG *pnParams);
sys_error_code_t VL53L9Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                 const EPowerMode NewPowerMode);
sys_error_code_t VL53L9Task_vtblHandleError(AManagedTask *_this, SysEvent Error);
sys_error_code_t VL53L9Task_vtblOnEnterTaskControlLoop(AManagedTask *_this);
sys_error_code_t VL53L9Task_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode ActivePowerMode);
sys_error_code_t VL53L9Task_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                 const EPowerMode NewPowerMode);

uint8_t VL53L9Task_vtblTofGetId(ISourceObservable *_this);
IEventSrc *VL53L9Task_vtblGetEventSourceIF(ISourceObservable *_this);
EMData_t VL53L9Task_vtblTofGetDataInfo(ISourceObservable *_this);
sys_error_code_t VL53L9Task_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t VL53L9Task_vtblSensorDisable(ISensor_t *_this);
boolean_t VL53L9Task_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t VL53L9Task_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t VL53L9Task_vtblSensorGetStatus(ISensor_t *_this);
SensorStatus_t *VL53L9Task_vtblSensorGetStatusPointer(ISensor_t *_this);
sys_error_code_t VL53L9Task_vtblTofGetProfile(ISensorRanging_t *_this, ProfileConfig_t *p_config);
sys_error_code_t VL53L9Task_vtblTofGetIT(ISensorRanging_t *_this, ITConfig_t *p_it_config);
uint32_t VL53L9Task_vtblTofGetAddress(ISensorRanging_t *_this);
uint32_t VL53L9Task_vtblTofGetPowerMode(ISensorRanging_t *_this);
sys_error_code_t VL53L9Task_vtblSensorSetFrequency(ISensorRanging_t *_this, uint32_t frequency);
sys_error_code_t VL53L9Task_vtblSensorSetResolution(ISensorRanging_t *_this, uint8_t resolution);
sys_error_code_t VL53L9Task_vtblSensorSetRangingMode(ISensorRanging_t *_this, uint8_t mode);
sys_error_code_t VL53L9Task_vtblSensorSetIntegrationTime(ISensorRanging_t *_this, uint32_t timing_budget);
sys_error_code_t VL53L9Task_vtblSensorConfigIT(ISensorRanging_t *_this, ITConfig_t *p_it_config);
sys_error_code_t VL53L9Task_vtblSensorSetAddress(ISensorRanging_t *_this, uint32_t address);
sys_error_code_t VL53L9Task_vtblSensorSetPowerMode(ISensorRanging_t *_this, uint32_t power_mode);

#ifdef __cplusplus
}
#endif

#endif /* VL53L9TASK_VTBL_H_ */