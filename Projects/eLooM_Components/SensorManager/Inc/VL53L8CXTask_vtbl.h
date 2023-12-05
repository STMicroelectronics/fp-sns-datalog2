/**
  ******************************************************************************
  * @file    VL53L8CXTask_vtbl.h
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
#ifndef VL53L8CXTASK_VTBL_H_
#define VL53L8CXTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* AManagedTask virtual functions */
sys_error_code_t VL53L8CXTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t VL53L8CXTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                               VOID **pvStackStart, ULONG *pnStackSize,
                                               UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                               ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t VL53L8CXTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t VL53L8CXTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t VL53L8CXTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t VL53L8CXTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                   EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t VL53L8CXTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t VL53L8CXTask_vtblTofGetId(ISourceObservable *_this);
IEventSrc *VL53L8CXTask_vtblGetEventSourceIF(ISourceObservable *_this);
EMData_t VL53L8CXTask_vtblTofGetDataInfo(ISourceObservable *_this);
sys_error_code_t VL53L8CXTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t VL53L8CXTask_vtblSensorDisable(ISensor_t *_this);
boolean_t VL53L8CXTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t VL53L8CXTask_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t VL53L8CXTask_vtblSensorGetStatus(ISensor_t *_this);
sys_error_code_t VL53L8CXTask_vtblTofGetProfile(ISensorRanging_t *_this, ProfileConfig_t *p_config);
sys_error_code_t VL53L8CXTask_vtblTofGetIT(ISensorRanging_t *_this, ITConfig_t *p_it_config);
uint32_t VL53L8CXTask_vtblTofGetAddress(ISensorRanging_t *_this);
uint32_t VL53L8CXTask_vtblTofGetPowerMode(ISensorRanging_t *_this);
sys_error_code_t VL53L8CXTask_vtblSensorSetFrequency(ISensorRanging_t *_this, uint32_t frequency);
sys_error_code_t VL53L8CXTask_vtblSensorSetResolution(ISensorRanging_t *_this, uint8_t resolution);
sys_error_code_t VL53L8CXTask_vtblSensorSetRangingMode(ISensorRanging_t *_this, uint8_t mode);
sys_error_code_t VL53L8CXTask_vtblSensorSetIntegrationTime(ISensorRanging_t *_this, uint32_t timing_budget);
sys_error_code_t VL53L8CXTask_vtblSensorConfigIT(ISensorRanging_t *_this, ITConfig_t *p_it_config);
sys_error_code_t VL53L8CXTask_vtblSensorSetAddress(ISensorRanging_t *_this, uint32_t address);
sys_error_code_t VL53L8CXTask_vtblSensorSetPowerMode(ISensorRanging_t *_this, uint32_t power_mode);

#ifdef __cplusplus
}
#endif

#endif /* VL53L8CXTASK_VTBL_H_ */
