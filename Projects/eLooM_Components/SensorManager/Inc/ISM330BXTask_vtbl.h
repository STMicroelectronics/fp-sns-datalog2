/**
  ******************************************************************************
  * @file    ISM330BXTask_vtbl.h
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
#ifndef ISM330BXTASK_VTBL_H_
#define ISM330BXTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* AManagedTask virtual functions */

/**
  * Initialize the hardware resource for the task.
  * This task doesn't need a driver extending the ::IDriver interface because:
  * - it manages two GPIO pins, that are the CS connected to the sensor SPI IF and the EXTI line.
  * - it uses the common sensor driver provided by the ST Sensor Solutions Software Team .
  *
  * @param _this [IN] specifies a task object.
  * @param pParams [IN] specifies task specific parameters. Not used
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
  * @sa AMTHardwareInit
  */
sys_error_code_t ISM330BXTask_vtblHardwareInit(AManagedTask *_this, void *pParams);
sys_error_code_t ISM330BXTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                               VOID **pvStackStart, ULONG *pnStackSize,
                                               UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                               ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t ISM330BXTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t ISM330BXTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t ISM330BXTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t ISM330BXTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                   EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t ISM330BXTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t ISM330BXTask_vtblAccGetId(ISourceObservable *_this);
IEventSrc *ISM330BXTask_vtblAccGetEventSourceIF(ISourceObservable *_this);
sys_error_code_t ISM330BXTask_vtblAccGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal);
float_t ISM330BXTask_vtblAccGetFS(ISensorMems_t *_this);
float_t ISM330BXTask_vtblAccGetSensitivity(ISensorMems_t *_this);
EMData_t ISM330BXTask_vtblAccGetDataInfo(ISourceObservable *_this);
uint8_t ISM330BXTask_vtblGyroGetId(ISourceObservable *_this);
IEventSrc *ISM330BXTask_vtblGyroGetEventSourceIF(ISourceObservable *_this);
sys_error_code_t ISM330BXTask_vtblGyroGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal);
float_t ISM330BXTask_vtblGyroGetFS(ISensorMems_t *_this);
float_t ISM330BXTask_vtblGyroGetSensitivity(ISensorMems_t *_this);
EMData_t ISM330BXTask_vtblGyroGetDataInfo(ISourceObservable *_this);

sys_error_code_t ISM330BXTask_vtblSensorSetODR(ISensorMems_t *_this, float_t odr);
sys_error_code_t ISM330BXTask_vtblSensorSetFS(ISensorMems_t *_this, float_t fs);
sys_error_code_t ISM330BXTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM);
sys_error_code_t ISM330BXTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t ISM330BXTask_vtblSensorDisable(ISensor_t *_this);
boolean_t ISM330BXTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t ISM330BXTask_vtblAccGetDescription(ISensor_t *_this);
SensorDescriptor_t ISM330BXTask_vtblGyroGetDescription(ISensor_t *_this);
SensorStatus_t ISM330BXTask_vtblAccGetStatus(ISensor_t *_this);
SensorStatus_t ISM330BXTask_vtblGyroGetStatus(ISensor_t *_this);
SensorStatus_t *ISM330BXTask_vtblAccGetStatusPointer(ISensor_t *_this);
SensorStatus_t *ISM330BXTask_vtblGyroGetStatusPointer(ISensor_t *_this);

sys_error_code_t ISM330BXTask_vtblSensorReadReg(ISensorLL_t *_this, uint16_t reg, uint8_t *data, uint16_t len);
sys_error_code_t ISM330BXTask_vtblSensorWriteReg(ISensorLL_t *_this, uint16_t reg, const uint8_t *data, uint16_t len);
sys_error_code_t ISM330BXTask_vtblSensorSyncModel(ISensorLL_t *_this);

uint8_t ISM330BXTask_vtblMlcGetId(ISourceObservable *_this);
IEventSrc *ISM330BXTask_vtblMlcGetEventSourceIF(ISourceObservable *_this);
sys_error_code_t ISM330BXTask_vtblMlcGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal);
float_t ISM330BXTask_vtblMlcGetFS(ISensorMems_t *_this);
float_t ISM330BXTask_vtblMlcGetSensitivity(ISensorMems_t *_this);
EMData_t ISM330BXTask_vtblMlcGetDataInfo(ISourceObservable *_this);
SensorDescriptor_t ISM330BXTask_vtblMlcGetDescription(ISensor_t *_this);
SensorStatus_t ISM330BXTask_vtblMlcGetStatus(ISensor_t *_this);
SensorStatus_t *ISM330BXTask_vtblMlcGetStatusPointer(ISensor_t *_this);

#ifdef __cplusplus
}
#endif

#endif /* ISM330BXTASK_VTBL_H_ */
