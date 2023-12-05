/**
  ******************************************************************************
  * @file    SensorManager.h
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
#ifndef SENSORMANAGER_H_
#define SENSORMANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif



#include "SensorManager_conf.h"
#include "ISensor.h"
#include "ISensor_vtbl.h"
#include "ISensorAudio.h"
#include "ISensorAudio_vtbl.h"
#include "ISensorLight.h"
#include "ISensorLight_vtbl.h"
#include "ISensorMems.h"
#include "ISensorMems_vtbl.h"
#include "ISensorRanging.h"
#include "ISensorRanging_vtbl.h"
#include "ISensorPresence.h"
#include "ISensorPresence_vtbl.h"
#include "SensorDef.h"


/* sensor error code */
/*********************/

#ifndef SYS_NO_ERROR_CODE
#define SYS_NO_ERROR_CODE                                   0
#endif
#ifndef SYS_SENSOR_TASK_BASE_ERROR_CODE
#define SYS_SENSOR_TASK_BASE_ERROR_CODE                     1
#endif
#define SYS_SENSOR_TASK_OP_ERROR_CODE                       SYS_SENSOR_TASK_BASE_ERROR_CODE + 1
#define SYS_SENSOR_TASK_UNKNOWN_MSG_ERROR_CODE              SYS_SENSOR_TASK_BASE_ERROR_CODE + 2
#define SYS_SENSOR_TASK_MSG_LOST_ERROR_CODE                 SYS_SENSOR_TASK_BASE_ERROR_CODE + 3
#define SYS_SENSOR_TASK_NO_DRDY_ERROR_CODE                  SYS_SENSOR_TASK_BASE_ERROR_CODE + 4


#ifndef HSD_USE_DUMMY_DATA
#define HSD_USE_DUMMY_DATA 0
#endif

#ifndef SM_MAX_SENSORS
#define SM_MAX_SENSORS               16
#endif

/**
  * Create  type name for _SensorManager_t.
  */
typedef struct _SensorManager_t SensorManager_t;

/**
  *  SensorManager_t internal structure.
  */
struct _SensorManager_t
{

  /**
    * Describes the sensor capabilities.
    */
  ISensor_t *Sensors[SM_MAX_SENSORS];

  /**
    * Indicates the number of sensors available.
    */
  uint16_t n_sensors;
};


/* Public API declaration */
/**************************/
ISourceObservable *SMGetSensorObserver(uint8_t id);
uint16_t SMGetNsensor(void);
sys_error_code_t SMSensorEnable(uint8_t id);
sys_error_code_t SMSensorDisable(uint8_t id);
SensorDescriptor_t SMSensorGetDescription(uint8_t id);
SensorStatus_t SMSensorGetStatus(uint8_t id);
sys_error_code_t SMDeviceGetDescription(SensorDescriptor_t *device_description);
SensorManager_t *SMGetSensorManager(void);
uint32_t SMGetnBytesPerSample(uint8_t id);

/* Specialized for ISensorMems class */
sys_error_code_t SMSensorSetODR(uint8_t id, float odr);
sys_error_code_t SMSensorSetFS(uint8_t id, float fs);
sys_error_code_t SMSensorSetFifoWM(uint8_t id, uint16_t fifoWM);

/* Specialized for ISensorAudio class */
sys_error_code_t SMSensorSetFrequency(uint8_t id, uint32_t frequency);
sys_error_code_t SMSensorSetVolume(uint8_t id, uint8_t volume);
sys_error_code_t SMSensorSetResolution(uint8_t id, uint8_t bit_depth);

/* Specialized for ISensorRanging class */
//sys_error_code_t SMSensorSetFrequency(uint8_t id, uint32_t frequency);  /*Declaration in common with ISensorAudio*/
//sys_error_code_t SMSensorSetResolution(uint8_t id, uint8_t resolution); /*Declaration in common with ISensorAudio*/
sys_error_code_t SMSensorSetRangingMode(uint8_t id, uint8_t mode);
sys_error_code_t SMSensorSetIntegrationTime(uint8_t id, uint32_t timing_budget);
sys_error_code_t SMSensorConfigIT(uint8_t id, ITConfig_t *p_it_config);
sys_error_code_t SMSensorSetAddress(uint8_t id, uint32_t address);
sys_error_code_t SMSensorSetPowerMode(uint8_t id, uint32_t power_mode);

/* Specialized for ISensorPresence class */
sys_error_code_t SMSensorSetDataFrequency(uint8_t id, float data_frequency);
sys_error_code_t SMSensorSetTransmittance(uint8_t id, float Transmittance);
sys_error_code_t SMSensorSetAverageTObject(uint8_t id, uint16_t average_tobject);
sys_error_code_t SMSensorSetAverageTAmbient(uint8_t id, uint16_t average_tambient);
sys_error_code_t SMSensorSetPresenceThreshold(uint8_t id, uint16_t presence_threshold);
sys_error_code_t SMSensorSetPresenceHysteresis(uint8_t id, uint8_t presence_hysteresis);
sys_error_code_t SMSensorSetMotionThreshold(uint8_t id, uint16_t motion_threshold);
sys_error_code_t SMSensorSetMotionHysteresis(uint8_t id, uint8_t motion_hysteresis);
sys_error_code_t SMSensorSetTAmbientShockThreshold(uint8_t id, uint16_t tambient_shock_threshold);
sys_error_code_t SMSensorSetTAmbientShockHysteresis(uint8_t id, uint8_t tambient_shock_hysteresis);
sys_error_code_t SMSensorSetLPF_P_M_Bandwidth(uint8_t id, uint16_t bandwidth);
sys_error_code_t SMSensorSetLPF_P_Bandwidth(uint8_t id, uint16_t bandwidth);
sys_error_code_t SMSensorSetLPF_M_Bandwidth(uint8_t id, uint16_t bandwidth);
sys_error_code_t SMSensorSetEmbeddedCompensation(uint8_t id, uint8_t EmbeddedCompensation);
sys_error_code_t SMSensorSetSoftwareCompensation(uint8_t id, uint8_t SoftwareCompensation);
sys_error_code_t SMSensorSetSoftwareCompensationAlgorithmConfig(uint8_t id, CompensationAlgorithmConfig_t *pAlgorithmConfig);


/* Specialized for ISensorLight class */
sys_error_code_t SMSensorSetIntermeasurementTime(uint8_t id, uint32_t intermeasurement_time);
sys_error_code_t SMSensorSetExposureTime(uint8_t id, float exposure_time);
sys_error_code_t SMSensorSetLightGain(uint8_t id, float LightGain, uint8_t channel);

/* Inline functions definition */
/*******************************/


#ifdef __cplusplus
}
#endif

#endif /* SENSORMANAGER_H_ */
