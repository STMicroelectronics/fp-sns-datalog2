/**
  ******************************************************************************
  * @file    LSM6DSV16XTask_vtbl.h
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
#ifndef LSM6DSV16XTASK_VTBL_H_
#define LSM6DSV16XTASK_VTBL_H_

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
sys_error_code_t LSM6DSV16XTask_vtblHardwareInit(AManagedTask *_this, void *pParams);
sys_error_code_t LSM6DSV16XTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                                 VOID **pvStackStart, ULONG *pnStackSize,
                                                 UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                                 ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t LSM6DSV16XTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t LSM6DSV16XTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t LSM6DSV16XTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t LSM6DSV16XTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                     EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t LSM6DSV16XTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                     const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t LSM6DSV16XTask_vtblAccGetId(ISourceObservable *_this);
IEventSrc *LSM6DSV16XTask_vtblAccGetEventSourceIF(ISourceObservable *_this);
sys_error_code_t LSM6DSV16XTask_vtblAccGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal);
float LSM6DSV16XTask_vtblAccGetFS(ISensorMems_t *_this);
float LSM6DSV16XTask_vtblAccGetSensitivity(ISensorMems_t *_this);
EMData_t LSM6DSV16XTask_vtblAccGetDataInfo(ISourceObservable *_this);
uint8_t LSM6DSV16XTask_vtblGyroGetId(ISourceObservable *_this);
IEventSrc *LSM6DSV16XTask_vtblGyroGetEventSourceIF(ISourceObservable *_this);
sys_error_code_t LSM6DSV16XTask_vtblGyroGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal);
float LSM6DSV16XTask_vtblGyroGetFS(ISensorMems_t *_this);
float LSM6DSV16XTask_vtblGyroGetSensitivity(ISensorMems_t *_this);
EMData_t LSM6DSV16XTask_vtblGyroGetDataInfo(ISourceObservable *_this);

sys_error_code_t LSM6DSV16XTask_vtblSensorSetODR(ISensorMems_t *_this, float odr);
sys_error_code_t LSM6DSV16XTask_vtblSensorSetFS(ISensorMems_t *_this, float fs);
sys_error_code_t LSM6DSV16XTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM);
sys_error_code_t LSM6DSV16XTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t LSM6DSV16XTask_vtblSensorDisable(ISensor_t *_this);
boolean_t LSM6DSV16XTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t LSM6DSV16XTask_vtblAccGetDescription(ISensor_t *_this);
SensorDescriptor_t LSM6DSV16XTask_vtblGyroGetDescription(ISensor_t *_this);
SensorStatus_t LSM6DSV16XTask_vtblAccGetStatus(ISensor_t *_this);
SensorStatus_t LSM6DSV16XTask_vtblGyroGetStatus(ISensor_t *_this);
SensorStatus_t *LSM6DSV16XTask_vtblAccGetStatusPointer(ISensor_t *_this);
SensorStatus_t *LSM6DSV16XTask_vtblGyroGetStatusPointer(ISensor_t *_this);

sys_error_code_t LSM6DSV16XTask_vtblSensorReadReg(ISensorLL_t *_this, uint16_t reg, uint8_t *data, uint16_t len);
sys_error_code_t LSM6DSV16XTask_vtblSensorWriteReg(ISensorLL_t *_this, uint16_t reg, const uint8_t *data, uint16_t len);
sys_error_code_t LSM6DSV16XTask_vtblSensorSyncModel(ISensorLL_t *_this);

uint8_t LSM6DSV16XTask_vtblMlcGetId(ISourceObservable *_this);
IEventSrc *LSM6DSV16XTask_vtblMlcGetEventSourceIF(ISourceObservable *_this);
sys_error_code_t LSM6DSV16XTask_vtblMlcGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal);
float LSM6DSV16XTask_vtblMlcGetFS(ISensorMems_t *_this);
float LSM6DSV16XTask_vtblMlcGetSensitivity(ISensorMems_t *_this);
EMData_t LSM6DSV16XTask_vtblMlcGetDataInfo(ISourceObservable *_this);
SensorDescriptor_t LSM6DSV16XTask_vtblMlcGetDescription(ISensor_t *_this);
SensorStatus_t LSM6DSV16XTask_vtblMlcGetStatus(ISensor_t *_this);
SensorStatus_t *LSM6DSV16XTask_vtblMlcGetStatusPointer(ISensor_t *_this);

#ifdef __cplusplus
}
#endif

#endif /* LSM6DSV16XTASK_VTBL_H_ */
