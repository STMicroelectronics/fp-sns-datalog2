/**
  ******************************************************************************
  * @file    ISM330ISTask_vtbl.h
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

#ifndef ISM330ISTASK_VTBL_H_
#define ISM330ISTASK_VTBL_H_

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
sys_error_code_t ISM330ISTask_vtblHardwareInit(AManagedTask *_this, void *pParams);
sys_error_code_t ISM330ISTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                               VOID **pvStackStart, ULONG *pnStackSize,
                                               UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                               ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t ISM330ISTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t ISM330ISTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t ISM330ISTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t ISM330ISTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                   EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t ISM330ISTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                   const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t ISM330ISTask_vtblAccGetId(ISourceObservable *_this);
IEventSrc *ISM330ISTask_vtblAccGetEventSourceIF(ISourceObservable *_this);
EMData_t ISM330ISTask_vtblAccGetDataInfo(ISourceObservable *_this);
uint8_t ISM330ISTask_vtblGyroGetId(ISourceObservable *_this);
IEventSrc *ISM330ISTask_vtblGyroGetEventSourceIF(ISourceObservable *_this);
EMData_t ISM330ISTask_vtblGyroGetDataInfo(ISourceObservable *_this);
uint8_t ISM330ISTask_vtblIspuGetId(ISourceObservable *_this);
IEventSrc *ISM330ISTask_vtblIspuGetEventSourceIF(ISourceObservable *_this);
EMData_t ISM330ISTask_vtblIspuGetDataInfo(ISourceObservable *_this);
sys_error_code_t ISM330ISTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t ISM330ISTask_vtblSensorDisable(ISensor_t *_this);
boolean_t ISM330ISTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t ISM330ISTask_vtblAccGetDescription(ISensor_t *_this);
SensorStatus_t ISM330ISTask_vtblAccGetStatus(ISensor_t *_this);
SensorDescriptor_t ISM330ISTask_vtblGyroGetDescription(ISensor_t *_this);
SensorStatus_t ISM330ISTask_vtblGyroGetStatus(ISensor_t *_this);
SensorDescriptor_t ISM330ISTask_vtblIspuGetDescription(ISensor_t *_this);
SensorStatus_t ISM330ISTask_vtblIspuGetStatus(ISensor_t *_this);
sys_error_code_t ISM330ISTask_vtblAccGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal);
float ISM330ISTask_vtblAccGetFS(ISensorMems_t *_this);
float ISM330ISTask_vtblAccGetSensitivity(ISensorMems_t *_this);
sys_error_code_t ISM330ISTask_vtblGyroGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal);
float ISM330ISTask_vtblGyroGetFS(ISensorMems_t *_this);
float ISM330ISTask_vtblGyroGetSensitivity(ISensorMems_t *_this);
sys_error_code_t ISM330ISTask_vtblIspuGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal);
float ISM330ISTask_vtblIspuGetFS(ISensorMems_t *_this);
float ISM330ISTask_vtblIspuGetSensitivity(ISensorMems_t *_this);
sys_error_code_t ISM330ISTask_vtblSensorSetODR(ISensorMems_t *_this, float odr);
sys_error_code_t ISM330ISTask_vtblSensorSetFS(ISensorMems_t *_this, float fs);
sys_error_code_t ISM330ISTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM);

sys_error_code_t ISM330ISTask_vtblSensorReadReg(ISensorLL_t *_this, uint16_t reg, uint8_t *data, uint16_t len);
sys_error_code_t ISM330ISTask_vtblSensorWriteReg(ISensorLL_t *_this, uint16_t reg, const uint8_t *data, uint16_t len);
sys_error_code_t ISM330ISTask_vtblSensorSyncModel(ISensorLL_t *_this);


#ifdef __cplusplus
}
#endif

#endif /* ISM330ISTASK_VTBL_H_ */
