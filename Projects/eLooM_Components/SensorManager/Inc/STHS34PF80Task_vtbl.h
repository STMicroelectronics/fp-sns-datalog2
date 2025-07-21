/**
  ******************************************************************************
  * @file    STHS34PF80Task_vtbl.h
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
#ifndef STHS34PF80TASK_VTBL_H_
#define STHS34PF80TASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* AManagedTask virtual functions */
sys_error_code_t STHS34PF80Task_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t STHS34PF80Task_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                                 VOID **pvStackStart, ULONG *pnStackSize,
                                                 UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                                 ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t STHS34PF80Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t STHS34PF80Task_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t STHS34PF80Task_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t STHS34PF80Task_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                     EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t STHS34PF80Task_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t STHS34PF80Task_vtblPresenceGetId(ISourceObservable *_this);
IEventSrc *STHS34PF80Task_vtblPresenceGetEventSourceIF(ISourceObservable *_this);
EMData_t STHS34PF80Task_vtblPresenceGetDataInfo(ISourceObservable *_this);
sys_error_code_t STHS34PF80Task_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t STHS34PF80Task_vtblSensorDisable(ISensor_t *_this);
boolean_t STHS34PF80Task_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t STHS34PF80Task_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t STHS34PF80Task_vtblSensorGetStatus(ISensor_t *_this);
SensorStatus_t *STHS34PF80Task_vtblSensorGetStatusPointer(ISensor_t *_this);
sys_error_code_t STHS34PF80Task_vtblPresenceGetDataFrequency(ISensorPresence_t *_this, float_t *p_measured,
                                                             float_t *p_nominal);
float_t STHS34PF80Task_vtblPresenceGetTransmittance(ISensorPresence_t *_this);
uint16_t STHS34PF80Task_vtblPresenceGetAverageTObject(ISensorPresence_t *_this);
uint16_t STHS34PF80Task_vtblPresenceGetAverageTAmbient(ISensorPresence_t *_this);
uint16_t STHS34PF80Task_vtblPresenceGetPresenceThreshold(ISensorPresence_t *_this);
uint8_t STHS34PF80Task_vtblPresenceGetPresenceHysteresis(ISensorPresence_t *_this);
uint16_t STHS34PF80Task_vtblPresenceGetMotionThreshold(ISensorPresence_t *_this);
uint8_t STHS34PF80Task_vtblPresenceGetMotionHysteresis(ISensorPresence_t *_this);
uint16_t STHS34PF80Task_vtblPresenceGetTAmbientShockThreshold(ISensorPresence_t *_this);
uint8_t STHS34PF80Task_vtblPresenceGetTAmbientShockHysteresis(ISensorPresence_t *_this);
uint16_t STHS34PF80Task_vtblPresenceGetLPF_P_M_Bandwidth(ISensorPresence_t *_this);
uint16_t STHS34PF80Task_vtblPresenceGetLPF_P_Bandwidth(ISensorPresence_t *_this);
uint16_t STHS34PF80Task_vtblPresenceGetLPF_M_Bandwidth(ISensorPresence_t *_this);
uint8_t STHS34PF80Task_vtblPresenceGetEmbeddedCompensation(ISensorPresence_t *_this);
uint8_t STHS34PF80Task_vtblPresenceGetSoftwareCompensation(ISensorPresence_t *_this);
sys_error_code_t STHS34PF80Task_vtblPresenceGetSoftwareCompensationAlgorithmConfig(ISensorPresence_t *_this,
    CompensationAlgorithmConfig_t *pAlgorithmConfig);
sys_error_code_t STHS34PF80Task_vtblSensorSetDataFrequency(ISensorPresence_t *_this, float_t data_frequency);
sys_error_code_t STHS34PF80Task_vtblSensorSetTransmittance(ISensorPresence_t *_this, float_t Transmittance);
sys_error_code_t STHS34PF80Task_vtblSensorSetAverageTObject(ISensorPresence_t *_this, uint16_t average_tobject);
sys_error_code_t STHS34PF80Task_vtblSensorSetAverageTAmbient(ISensorPresence_t *_this, uint16_t average_tambient);
sys_error_code_t STHS34PF80Task_vtblSensorSetPresenceThreshold(ISensorPresence_t *_this, uint16_t presence_threshold);
sys_error_code_t STHS34PF80Task_vtblSensorSetPresenceHysteresis(ISensorPresence_t *_this, uint8_t presence_hysteresis);
sys_error_code_t STHS34PF80Task_vtblSensorSetMotionThreshold(ISensorPresence_t *_this, uint16_t motion_threshold);
sys_error_code_t STHS34PF80Task_vtblSensorSetMotionHysteresis(ISensorPresence_t *_this, uint8_t motion_hysteresis);
sys_error_code_t STHS34PF80Task_vtblSensorSetTAmbientShockThreshold(ISensorPresence_t *_this,
                                                                    uint16_t tambient_shock_threshold);
sys_error_code_t STHS34PF80Task_vtblSensorSetTAmbientShockHysteresis(ISensorPresence_t *_this,
                                                                     uint8_t tambient_shock_hysteresis);
sys_error_code_t STHS34PF80Task_vtblSensorSetLPF_P_M_Bandwidth(ISensorPresence_t *_this, uint16_t Bandwidth);
sys_error_code_t STHS34PF80Task_vtblSensorSetLPF_P_Bandwidth(ISensorPresence_t *_this, uint16_t Bandwidth);
sys_error_code_t STHS34PF80Task_vtblSensorSetLPF_M_Bandwidth(ISensorPresence_t *_this, uint16_t Bandwidth);
sys_error_code_t STHS34PF80Task_vtblSensorSetEmbeddedCompensation(ISensorPresence_t *_this,
                                                                  uint8_t embedded_compensation);
sys_error_code_t STHS34PF80Task_vtblSensorSetSoftwareCompensation(ISensorPresence_t *_this,
                                                                  uint8_t software_compensation);
sys_error_code_t STHS34PF80Task_vtblSensorSetSoftwareCompensationAlgorithmConfig(ISensorPresence_t *_this,
    CompensationAlgorithmConfig_t *pAlgorithmConfig);

#ifdef __cplusplus
}
#endif

#endif /* STHS34PF80TASK_VTBL_H_ */
