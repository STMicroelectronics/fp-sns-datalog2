/**
  ******************************************************************************
  * @file    LSM6DSV16BXTask_vtbl.h
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
#ifndef LSM6DSV16BXTASK_VTBL_H_
#define LSM6DSV16BXTASK_VTBL_H_

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
sys_error_code_t LSM6DSV16BXTask_vtblHardwareInit(AManagedTask *_this, void *pParams);
sys_error_code_t LSM6DSV16BXTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                                  VOID **pvStackStart, ULONG *pnStackSize,
                                                  UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                                  ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t LSM6DSV16BXTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                      const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t LSM6DSV16BXTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t LSM6DSV16BXTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t LSM6DSV16BXTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                      EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t LSM6DSV16BXTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                      const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t LSM6DSV16BXTask_vtblAccGetId(ISourceObservable *_this);
IEventSrc *LSM6DSV16BXTask_vtblAccGetEventSourceIF(ISourceObservable *_this);
sys_error_code_t LSM6DSV16BXTask_vtblAccGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal);
float_t LSM6DSV16BXTask_vtblAccGetFS(ISensorMems_t *_this);
float_t LSM6DSV16BXTask_vtblAccGetSensitivity(ISensorMems_t *_this);
EMData_t LSM6DSV16BXTask_vtblAccGetDataInfo(ISourceObservable *_this);
uint8_t LSM6DSV16BXTask_vtblGyroGetId(ISourceObservable *_this);
IEventSrc *LSM6DSV16BXTask_vtblGyroGetEventSourceIF(ISourceObservable *_this);
sys_error_code_t LSM6DSV16BXTask_vtblGyroGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal);
float_t LSM6DSV16BXTask_vtblGyroGetFS(ISensorMems_t *_this);
float_t LSM6DSV16BXTask_vtblGyroGetSensitivity(ISensorMems_t *_this);
EMData_t LSM6DSV16BXTask_vtblGyroGetDataInfo(ISourceObservable *_this);

sys_error_code_t LSM6DSV16BXTask_vtblSensorSetODR(ISensorMems_t *_this, float_t odr);
sys_error_code_t LSM6DSV16BXTask_vtblSensorSetFS(ISensorMems_t *_this, float_t fs);
sys_error_code_t LSM6DSV16BXTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM);
sys_error_code_t LSM6DSV16BXTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t LSM6DSV16BXTask_vtblSensorDisable(ISensor_t *_this);
boolean_t LSM6DSV16BXTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t LSM6DSV16BXTask_vtblAccGetDescription(ISensor_t *_this);
SensorDescriptor_t LSM6DSV16BXTask_vtblGyroGetDescription(ISensor_t *_this);
SensorStatus_t LSM6DSV16BXTask_vtblAccGetStatus(ISensor_t *_this);
SensorStatus_t LSM6DSV16BXTask_vtblGyroGetStatus(ISensor_t *_this);
SensorStatus_t *LSM6DSV16BXTask_vtblAccGetStatusPointer(ISensor_t *_this);
SensorStatus_t *LSM6DSV16BXTask_vtblGyroGetStatusPointer(ISensor_t *_this);

sys_error_code_t LSM6DSV16BXTask_vtblSensorReadReg(ISensorLL_t *_this, uint16_t reg, uint8_t *data, uint16_t len);
sys_error_code_t LSM6DSV16BXTask_vtblSensorWriteReg(ISensorLL_t *_this, uint16_t reg, const uint8_t *data,
                                                    uint16_t len);
sys_error_code_t LSM6DSV16BXTask_vtblSensorSyncModel(ISensorLL_t *_this);

uint8_t LSM6DSV16BXTask_vtblMlcGetId(ISourceObservable *_this);
IEventSrc *LSM6DSV16BXTask_vtblMlcGetEventSourceIF(ISourceObservable *_this);
sys_error_code_t LSM6DSV16BXTask_vtblMlcGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal);
float_t LSM6DSV16BXTask_vtblMlcGetFS(ISensorMems_t *_this);
float_t LSM6DSV16BXTask_vtblMlcGetSensitivity(ISensorMems_t *_this);
EMData_t LSM6DSV16BXTask_vtblMlcGetDataInfo(ISourceObservable *_this);
SensorDescriptor_t LSM6DSV16BXTask_vtblMlcGetDescription(ISensor_t *_this);
SensorStatus_t LSM6DSV16BXTask_vtblMlcGetStatus(ISensor_t *_this);
SensorStatus_t *LSM6DSV16BXTask_vtblMlcGetStatusPointer(ISensor_t *_this);

#ifdef __cplusplus
}
#endif

#endif /* LSM6DSV16BXTASK_VTBL_H_ */
