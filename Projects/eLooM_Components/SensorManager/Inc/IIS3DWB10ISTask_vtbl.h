/**
  ******************************************************************************
  * @file    IIS3DWB10ISTask_vtbl.h
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

#ifndef IIS3DWB10ISTASK_VTBL_H_
#define IIS3DWB10ISTASK_VTBL_H_

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
sys_error_code_t IIS3DWB10ISTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t IIS3DWB10ISTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                                  VOID **pvStackStart, ULONG *pnStackSize, UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice,
                                                  ULONG *pnAutoStart, ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t IIS3DWB10ISTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                      const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t IIS3DWB10ISTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t IIS3DWB10ISTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t IIS3DWB10ISTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                      EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t IIS3DWB10ISTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                      const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t IIS3DWB10ISTask_vtblAccGetId(ISourceObservable *_this);
IEventSrc *IIS3DWB10ISTask_vtblAccGetEventSourceIF(ISourceObservable *_this);
EMData_t IIS3DWB10ISTask_vtblAccGetDataInfo(ISourceObservable *_this);
uint8_t IIS3DWB10ISTask_vtblIspuGetId(ISourceObservable *_this);
IEventSrc *IIS3DWB10ISTask_vtblIspuGetEventSourceIF(ISourceObservable *_this);
EMData_t IIS3DWB10ISTask_vtblIspuGetDataInfo(ISourceObservable *_this);
sys_error_code_t IIS3DWB10ISTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t IIS3DWB10ISTask_vtblSensorDisable(ISensor_t *_this);
boolean_t IIS3DWB10ISTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t IIS3DWB10ISTask_vtblAccGetDescription(ISensor_t *_this);
SensorStatus_t IIS3DWB10ISTask_vtblAccGetStatus(ISensor_t *_this);
SensorStatus_t *IIS3DWB10ISTask_vtblAccGetStatusPointer(ISensor_t *_this);
SensorDescriptor_t IIS3DWB10ISTask_vtblIspuGetDescription(ISensor_t *_this);
SensorStatus_t IIS3DWB10ISTask_vtblIspuGetStatus(ISensor_t *_this);
SensorStatus_t *IIS3DWB10ISTask_vtblIspuGetStatusPointer(ISensor_t *_this);
sys_error_code_t IIS3DWB10ISTask_vtblAccGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal);
float_t IIS3DWB10ISTask_vtblAccGetFS(ISensorMems_t *_this);
float_t IIS3DWB10ISTask_vtblAccGetSensitivity(ISensorMems_t *_this);
sys_error_code_t IIS3DWB10ISTask_vtblIspuGetODR(ISensorMems_t *_this, float_t *p_measured, float_t *p_nominal);
float_t IIS3DWB10ISTask_vtblIspuGetFS(ISensorMems_t *_this);
float_t IIS3DWB10ISTask_vtblIspuGetSensitivity(ISensorMems_t *_this);
sys_error_code_t IIS3DWB10ISTask_vtblSensorSetODR(ISensorMems_t *_this, float_t odr);
sys_error_code_t IIS3DWB10ISTask_vtblSensorSetFS(ISensorMems_t *_this, float_t fs);
sys_error_code_t IIS3DWB10ISTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM);

sys_error_code_t IIS3DWB10ISTask_vtblSensorReadReg(ISensorLL_t *_this, uint16_t reg, uint8_t *data, uint16_t len);
sys_error_code_t IIS3DWB10ISTask_vtblSensorWriteReg(ISensorLL_t *_this, uint16_t reg, const uint8_t *data,
                                                    uint16_t len);
sys_error_code_t IIS3DWB10ISTask_vtblSensorSyncModel(ISensorLL_t *_this);


#ifdef __cplusplus
}
#endif

#endif /* IIS3DWB10ISTASK_VTBL_H_ */
