/**
  ******************************************************************************
  * @file    ISensorPresence_vtbl.h
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

#ifndef INCLUDE_ISENSORPRESENCE_VTBL_H_
#define INCLUDE_ISENSORPRESENCE_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ISensor.h"
#include "ISensor_vtbl.h"

/**
  * Create a type name for ISensorPresence_vtbl.
  */
typedef struct _ISensorPresence_vtbl ISensorPresence_vtbl;

struct _ISensorPresence_vtbl
{
  ISensor_vtbl vtbl;
  sys_error_code_t (*SensorGetDataFrequency)(ISensorPresence_t *_this, float *p_measured, float *p_nominal);
  float (*SensorGetTransmittance)(ISensorPresence_t *_this);
  uint16_t (*SensorGetAverageTObject)(ISensorPresence_t *_this);
  uint16_t (*SensorGetAverageTAmbient)(ISensorPresence_t *_this);
  uint16_t (*SensorGetPresenceThreshold)(ISensorPresence_t *_this);
  uint8_t (*SensorGetPresenceHysteresis)(ISensorPresence_t *_this);
  uint16_t (*SensorGetMotionThreshold)(ISensorPresence_t *_this);
  uint8_t (*SensorGetMotionHysteresis)(ISensorPresence_t *_this);
  uint16_t (*SensorGetTAmbientShockThreshold)(ISensorPresence_t *_this);
  uint8_t (*SensorGetTAmbientShockHysteresis)(ISensorPresence_t *_this);
  uint16_t (*SensorGetLPF_P_M_Bandwidth)(ISensorPresence_t *_this);
  uint16_t (*SensorGetLPF_P_Bandwidth)(ISensorPresence_t *_this);
  uint16_t (*SensorGetLPF_M_Bandwidth)(ISensorPresence_t *_this);
  uint8_t (*SensorGetEmbeddedCompensation)(ISensorPresence_t *_this);
  uint8_t (*SensorGetSoftwareCompensation)(ISensorPresence_t *_this);
  sys_error_code_t (*SensorGetSoftwareCompensationAlgorithmConfig)(ISensorPresence_t *_this, CompensationAlgorithmConfig_t *pAlgorithmConfig);
  sys_error_code_t (*SensorSetDataFrequency)(ISensorPresence_t *_this, float DataFrequency);
  sys_error_code_t (*SensorSetTransmittance)(ISensorPresence_t *_this, float Transmittance);
  sys_error_code_t (*SensorSetAverageTObject)(ISensorPresence_t *_this, uint16_t avgTobject);
  sys_error_code_t (*SensorSetAverageTAmbient)(ISensorPresence_t *_this, uint16_t avgTambient);
  sys_error_code_t (*SensorSetPresenceThreshold)(ISensorPresence_t *_this, uint16_t PresenceThreshold);
  sys_error_code_t (*SensorSetPresenceHysteresis)(ISensorPresence_t *_this, uint8_t PresenceHysteresis);
  sys_error_code_t (*SensorSetMotionThreshold)(ISensorPresence_t *_this, uint16_t MotionThreshold);
  sys_error_code_t (*SensorSetMotionHysteresis)(ISensorPresence_t *_this, uint8_t MotionHysteresis);
  sys_error_code_t (*SensorSetTAmbientShockThreshold)(ISensorPresence_t *_this, uint16_t TAmbientShockThreshold);
  sys_error_code_t (*SensorSetTAmbientShockHysteresis)(ISensorPresence_t *_this, uint8_t TAmbientShockHysteresis);
  sys_error_code_t (*SensorSetLPF_P_M_Bandwidth)(ISensorPresence_t *_this, uint16_t bandwidth);
  sys_error_code_t (*SensorSetLPF_P_Bandwidth)(ISensorPresence_t *_this, uint16_t bandwidth);
  sys_error_code_t (*SensorSetLPF_M_Bandwidth)(ISensorPresence_t *_this, uint16_t bandwidth);
  sys_error_code_t (*SensorSetEmbeddedCompensation)(ISensorPresence_t *_this, uint8_t EmbeddedCompensation);
  sys_error_code_t (*SensorSetSoftwareCompensation)(ISensorPresence_t *_this, uint8_t SoftwareCompensation);
  sys_error_code_t (*SensorSetSoftwareCompensationAlgorithmConfig)(ISensorPresence_t *_this, CompensationAlgorithmConfig_t *pAlgorithmConfig);
};

struct _ISensorPresence_t
{
  /**
    * Pointer to the virtual table for the class.
    */
  const ISensorPresence_vtbl *vptr;
};

// Inline functions definition
// ***************************

static inline sys_error_code_t ISensorGetDataFrequency(ISensorPresence_t *_this, float *p_measured, float *p_nominal)
{
  return _this->vptr->SensorGetDataFrequency(_this, p_measured, p_nominal);
}

static inline float ISensorGetTransmittance(ISensorPresence_t *_this)
{
  return _this->vptr->SensorGetTransmittance(_this);
}

static inline uint16_t ISensorGetAverageTObject(ISensorPresence_t *_this)
{
  return _this->vptr->SensorGetAverageTObject(_this);
}

static inline uint16_t ISensorGetAverageTAmbient(ISensorPresence_t *_this)
{
  return _this->vptr->SensorGetAverageTAmbient(_this);
}

static inline uint16_t ISensorGetPresenceThreshold(ISensorPresence_t *_this)
{
  return _this->vptr->SensorGetPresenceThreshold(_this);
}

static inline uint8_t ISensorGetPresenceHysteresis(ISensorPresence_t *_this)
{
  return _this->vptr->SensorGetPresenceHysteresis(_this);
}

static inline uint16_t ISensorGetMotionThreshold(ISensorPresence_t *_this)
{
  return _this->vptr->SensorGetMotionThreshold(_this);
}

static inline uint8_t ISensorGetMotionHysteresis(ISensorPresence_t *_this)
{
  return _this->vptr->SensorGetMotionHysteresis(_this);
}

static inline uint16_t ISensorGetTAmbientShockThreshold(ISensorPresence_t *_this)
{
  return _this->vptr->SensorGetTAmbientShockThreshold(_this);
}

static inline uint8_t ISensorGetTAmbientShockHysteresis(ISensorPresence_t *_this)
{
  return _this->vptr->SensorGetTAmbientShockHysteresis(_this);
}

static inline uint16_t ISensorGetLPF_P_M_Bandwidth(ISensorPresence_t *_this)
{
  return _this->vptr->SensorGetLPF_P_M_Bandwidth(_this);
}

static inline uint16_t ISensorGetLPF_P_Bandwidth(ISensorPresence_t *_this)
{
  return _this->vptr->SensorGetLPF_P_Bandwidth(_this);
}

static inline uint16_t ISensorGetLPF_M_Bandwidth(ISensorPresence_t *_this)
{
  return _this->vptr->SensorGetLPF_M_Bandwidth(_this);
}

static inline uint8_t ISensorGetEmbeddedCompensation(ISensorPresence_t *_this)
{
  return _this->vptr->SensorGetEmbeddedCompensation(_this);
}

static inline uint8_t ISensorGetSoftwareCompensation(ISensorPresence_t *_this)
{
  return _this->vptr->SensorGetSoftwareCompensation(_this);
}

static inline sys_error_code_t ISensorGetSoftwareCompensationAlgorithmConfig(ISensorPresence_t *_this, CompensationAlgorithmConfig_t *pAlgorithmConfig)
{
  return _this->vptr->SensorGetSoftwareCompensationAlgorithmConfig(_this, pAlgorithmConfig);
}

static inline sys_error_code_t ISensorSetDataFrequency(ISensorPresence_t *_this, float DataFrequency)
{
  return _this->vptr->SensorSetDataFrequency(_this, DataFrequency);
}

static inline sys_error_code_t ISensorSetTransmittance(ISensorPresence_t *_this, float Transmittance)
{
  return _this->vptr->SensorSetTransmittance(_this, Transmittance);
}

static inline sys_error_code_t ISensorSetAverageTObject(ISensorPresence_t *_this, uint16_t averageTobject)
{
  return _this->vptr->SensorSetAverageTObject(_this, averageTobject);
}

static inline sys_error_code_t ISensorSetAverageTAmbient(ISensorPresence_t *_this, uint16_t averageTambient)
{
  return _this->vptr->SensorSetAverageTAmbient(_this, averageTambient);
}

static inline sys_error_code_t ISensorSetPresenceThreshold(ISensorPresence_t *_this, uint16_t PresenceThreshold)
{
  return _this->vptr->SensorSetPresenceThreshold(_this, PresenceThreshold);
}

static inline sys_error_code_t ISensorSetPresenceHysteresis(ISensorPresence_t *_this, uint8_t PresenceHysteresis)
{
  return _this->vptr->SensorSetPresenceHysteresis(_this, PresenceHysteresis);
}

static inline sys_error_code_t ISensorSetMotionThreshold(ISensorPresence_t *_this, uint16_t MotionThreshold)
{
  return _this->vptr->SensorSetMotionThreshold(_this, MotionThreshold);
}

static inline sys_error_code_t ISensorSetMotionHysteresis(ISensorPresence_t *_this, uint8_t MotionHysteresis)
{
  return _this->vptr->SensorSetMotionHysteresis(_this, MotionHysteresis);
}

static inline sys_error_code_t ISensorSetTAmbientShockThreshold(ISensorPresence_t *_this, uint16_t TAmbientShockThreshold)
{
  return _this->vptr->SensorSetTAmbientShockThreshold(_this, TAmbientShockThreshold);
}

static inline sys_error_code_t ISensorSetTAmbientShockHysteresis(ISensorPresence_t *_this, uint8_t TAmbientShockHysteresis)
{
  return _this->vptr->SensorSetTAmbientShockHysteresis(_this, TAmbientShockHysteresis);
}

static inline sys_error_code_t ISensorSetLPF_P_M_Bandwidth(ISensorPresence_t *_this, uint16_t bandwidth)
{
  return _this->vptr->SensorSetLPF_P_M_Bandwidth(_this, bandwidth);
}

static inline sys_error_code_t ISensorSetLPF_P_Bandwidth(ISensorPresence_t *_this, uint16_t bandwidth)
{
  return _this->vptr->SensorSetLPF_P_Bandwidth(_this, bandwidth);
}

static inline sys_error_code_t ISensorSetLPF_M_Bandwidth(ISensorPresence_t *_this, uint16_t bandwidth)
{
  return _this->vptr->SensorSetLPF_M_Bandwidth(_this, bandwidth);
}

static inline sys_error_code_t ISensorSetEmbeddedCompensation(ISensorPresence_t *_this, uint8_t EmbeddedCompensation)
{
  return _this->vptr->SensorSetEmbeddedCompensation(_this, EmbeddedCompensation);
}

static inline sys_error_code_t ISensorSetSoftwareCompensation(ISensorPresence_t *_this, uint8_t SoftwareCompensation)
{
  return _this->vptr->SensorSetSoftwareCompensation(_this, SoftwareCompensation);
}

static inline sys_error_code_t ISensorSetSoftwareCompensationAlgorithmConfig(ISensorPresence_t *_this, CompensationAlgorithmConfig_t *pAlgorithmConfig)
{
  return _this->vptr->SensorSetSoftwareCompensationAlgorithmConfig(_this, pAlgorithmConfig);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ISENSORPRESENCE_VTBL_H_ */
