/**
  ******************************************************************************
  * @file    ILPS28QSWTask_vtbl.h
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
#ifndef ILPS28QSWTASK_VTBL_H_
#define ILPS28QSWTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

// AManagedTaskEx virtual functions

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
sys_error_code_t ILPS28QSWTask_vtblHardwareInit(AManagedTask *_this, void *pParams);
sys_error_code_t ILPS28QSWTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
                                                VOID **pvStackStart, ULONG *pnStackSize,
                                                UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart,
                                                ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t ILPS28QSWTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode ActivePowerMode,
                                                    const EPowerMode NewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t ILPS28QSWTask_vtblHandleError(AManagedTask *_this, SysEvent Error); ///< @sa AMTHandleError
sys_error_code_t ILPS28QSWTask_vtblOnEnterTaskControlLoop(AManagedTask *this); ///< @sa AMTOnEnterTaskControlLoop

/* AManagedTaskEx virtual functions */
sys_error_code_t ILPS28QSWTask_vtblForceExecuteStep(AManagedTaskEx *_this,
                                                    EPowerMode ActivePowerMode); ///< @sa AMTExForceExecuteStep
sys_error_code_t ILPS28QSWTask_vtblOnEnterPowerMode(AManagedTaskEx *_this, const EPowerMode ActivePowerMode,
                                                    const EPowerMode NewPowerMode); ///< @sa AMTExOnEnterPowerMode

uint8_t ILPS28QSWTask_vtblPressGetId(ISourceObservable *_this);
IEventSrc *ILPS28QSWTask_vtblPressGetEventSourceIF(ISourceObservable *_this);
EMData_t ILPS28QSWTask_vtblPressGetDataInfo(ISourceObservable *_this);
sys_error_code_t ILPS28QSWTask_vtblSensorEnable(ISensor_t *_this);
sys_error_code_t ILPS28QSWTask_vtblSensorDisable(ISensor_t *_this);
boolean_t ILPS28QSWTask_vtblSensorIsEnabled(ISensor_t *_this);
SensorDescriptor_t ILPS28QSWTask_vtblPressGetDescription(ISensor_t *_this);
SensorStatus_t ILPS28QSWTask_vtblPressGetStatus(ISensor_t *_this);
SensorStatus_t *ILPS28QSWTask_vtblPressGetStatusPointer(ISensor_t *_this);
sys_error_code_t ILPS28QSWTask_vtblPressGetODR(ISensorMems_t *_this, float *p_measured, float *p_nominal);
float ILPS28QSWTask_vtblPressGetFS(ISensorMems_t *_this);
float ILPS28QSWTask_vtblPressGetSensitivity(ISensorMems_t *_this);
sys_error_code_t ILPS28QSWTask_vtblSensorSetODR(ISensorMems_t *_this, float odr);
sys_error_code_t ILPS28QSWTask_vtblSensorSetFS(ISensorMems_t *_this, float fs);
sys_error_code_t ILPS28QSWTask_vtblSensorSetFifoWM(ISensorMems_t *_this, uint16_t fifoWM);

#ifdef __cplusplus
}
#endif

#endif /* ILPS28QSWTASK_VTBL_H_ */
