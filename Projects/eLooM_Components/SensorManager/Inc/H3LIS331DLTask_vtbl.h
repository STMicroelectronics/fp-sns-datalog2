/**
  ******************************************************************************
  * @file    H3LIS331DLTask_vtbl.h
  * @author  AME/AMS/S2CSupport
  *
  * @date    17-Aug-2023
  *
  * @brief
  *
  *
  *
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
#ifndef H3LIS331DLTASK_VTBL_H_
#define H3LIS331DLTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


/* AManagedTask virtual functions */
sys_error_code_t H3LIS331DLTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t H3LIS331DLTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                                 VOID **pvStackStart, ULONG *pnStackSize, UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice,
                                                 ULONG *pnAutoStart, ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t H3LIS331DLTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t H3LIS331DLTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t H3LIS331DLTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t H3LIS331DLTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                     EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t H3LIS331DLTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t H3LIS331DLTask_vtblAccGetId(ISourceObservable *_this);
IEventSrc *H3LIS331DLTask_vtblGetEventSourceIF(ISourceObservable *_this);
EMData_t H3LIS331DLTask_vtblAccGetDataInfo(ISourceObservable *_this);
sys_error_code_t H3LIS331DLTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t H3LIS331DLTask_vtblSensorDisable(ISensor_t *_this);
boolean_t H3LIS331DLTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t H3LIS331DLTask_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t H3LIS331DLTask_vtblSensorGetStatus(ISensor_t *_this);
SensorStatus_t *H3LIS331DLTask_vtblSensorGetStatusPointer(ISensor_t *_this);
sys_error_code_t H3LIS331DLTask_vtblAccGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal);
float_t H3LIS331DLTask_vtblAccGetFS(ISensorMems_t  *_this);
float_t H3LIS331DLTask_vtblAccGetSensitivity(ISensorMems_t *_this);


sys_error_code_t H3LIS331DLTask_vtblSensorSetODR(ISensorMems_t *_this, float_t odr);
sys_error_code_t H3LIS331DLTask_vtblSensorSetFS(ISensorMems_t *_this, float_t fs);
sys_error_code_t H3LIS331DLTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM);


#ifdef __cplusplus
}
#endif

#endif /* H3LIS331DLTASK_VTBL_H_ */
