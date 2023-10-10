/**
  ******************************************************************************
  * @file    IIS3DWBTask_vtbl.h
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
#ifndef IIS3DWBTASK_VTBL_H_
#define IIS3DWBTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif




/* AManagedTask virtual functions */
sys_error_code_t IIS3DWBTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t IIS3DWBTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                              VOID **pvStackStart, ULONG *pnStackSize, UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice,
                                              ULONG *pnAutoStart, ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t IIS3DWBTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t IIS3DWBTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t IIS3DWBTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t IIS3DWBTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                  EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t IIS3DWBTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                  const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t IIS3DWBTask_vtblAccGetId(ISourceObservable *_this);
IEventSrc *IIS3DWBTask_vtblGetEventSourceIF(ISourceObservable *_this);
EMData_t IIS3DWBTask_vtblAccGetDataInfo(ISourceObservable *_this);
sys_error_code_t IIS3DWBTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t IIS3DWBTask_vtblSensorDisable(ISensor_t *_this);
boolean_t IIS3DWBTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t IIS3DWBTask_vtblSensorGetDescription(ISensor_t *_this);
SensorStatus_t IIS3DWBTask_vtblSensorGetStatus(ISensor_t *_this);
sys_error_code_t IIS3DWBTask_vtblAccGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal);
float IIS3DWBTask_vtblAccGetFS(ISensorMems_t *_this);
float IIS3DWBTask_vtblAccGetSensitivity(ISensorMems_t *_this);
sys_error_code_t IIS3DWBTask_vtblSensorSetODR(ISensorMems_t *_this, float odr);
sys_error_code_t IIS3DWBTask_vtblSensorSetFS(ISensorMems_t *_this, float fs);
sys_error_code_t IIS3DWBTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM);

#ifdef __cplusplus
}
#endif

#endif /* IIS3DWBTASK_VTBL_H_ */
