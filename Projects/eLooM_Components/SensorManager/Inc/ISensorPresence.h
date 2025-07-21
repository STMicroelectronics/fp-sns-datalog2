/**
  ******************************************************************************
  * @file    ISensorPresence.h
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

#ifndef INCLUDE_ISENSORPRESENCE_H_
#define INCLUDE_ISENSORPRESENCE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/systypes.h"
#include "services/syserror.h"
#include "services/systp.h"
#include "SensorDef.h"
#include "ISensor.h"

/**
  * Create  type name for ISensorPresence.
  */
typedef struct _ISensorPresence_t ISensorPresence_t;

// Public API declaration
//***********************
/** Public interface **/
static inline sys_error_code_t ISensorGetDataFrequency(ISensorPresence_t *_this, float_t *p_measured,
                                                       float_t *p_nominal);
static inline float_t ISensorGetTransmittance(ISensorPresence_t *_this);
static inline uint16_t ISensorGetAverageTObject(ISensorPresence_t *_this);
static inline uint16_t ISensorGetAverageTAmbient(ISensorPresence_t *_this);
static inline uint16_t ISensorGetPresenceThreshold(ISensorPresence_t *_this);
static inline uint8_t ISensorGetPresenceHysteresis(ISensorPresence_t *_this);
static inline uint16_t ISensorGetMotionThreshold(ISensorPresence_t *_this);
static inline uint8_t ISensorGetMotionHysteresis(ISensorPresence_t *_this);
static inline uint16_t ISensorGetTAmbientShockThreshold(ISensorPresence_t *_this);
static inline uint8_t ISensorGetTAmbientShockHysteresis(ISensorPresence_t *_this);
static inline uint16_t ISensorGetLPF_P_M_Bandwidth(ISensorPresence_t *_this);
static inline uint16_t ISensorGetLPF_P_Bandwidth(ISensorPresence_t *_this);
static inline uint16_t ISensorGetLPF_M_Bandwidth(ISensorPresence_t *_this);
static inline uint8_t ISensorGetEmbeddedCompensation(ISensorPresence_t *_this);
static inline uint8_t ISensorGetSoftwareCompensation(ISensorPresence_t *_this);
static inline sys_error_code_t ISensorGetSoftwareCompensationAlgorithmConfig(ISensorPresence_t *_this,
                                                                             CompensationAlgorithmConfig_t *pAlgorithmConfig);
static inline sys_error_code_t ISensorSetDataFrequency(ISensorPresence_t *_this, float_t DataFrequency);
static inline sys_error_code_t ISensorSetTransmittance(ISensorPresence_t *_this, float_t Transmittance);
static inline sys_error_code_t ISensorSetAverageTObject(ISensorPresence_t *_this, uint16_t avgTobject);
static inline sys_error_code_t ISensorSetAverageTAmbient(ISensorPresence_t *_this, uint16_t avgTambient);
static inline sys_error_code_t ISensorSetPresenceThreshold(ISensorPresence_t *_this, uint16_t PresenceThreshold);
static inline sys_error_code_t ISensorSetPresenceHysteresis(ISensorPresence_t *_this, uint8_t PresenceHysteresis);
static inline sys_error_code_t ISensorSetMotionThreshold(ISensorPresence_t *_this, uint16_t MotionThreshold);
static inline sys_error_code_t ISensorSetMotionHysteresis(ISensorPresence_t *_this, uint8_t MotionHysteresis);
static inline sys_error_code_t ISensorSetTAmbientShockThreshold(ISensorPresence_t *_this,
                                                                uint16_t TAmbientShockThreshold);
static inline sys_error_code_t ISensorSetTAmbientShockHysteresis(ISensorPresence_t *_this,
                                                                 uint8_t TAmbientShockHysteresis);
static inline sys_error_code_t ISensorSetLPF_P_M_Bandwidth(ISensorPresence_t *_this, uint16_t bandwidth);
static inline sys_error_code_t ISensorSetLPF_P_Bandwidth(ISensorPresence_t *_this, uint16_t bandwidth);
static inline sys_error_code_t ISensorSetLPF_M_Bandwidth(ISensorPresence_t *_this, uint16_t bandwidth);
static inline sys_error_code_t ISensorSetEmbeddedCompensation(ISensorPresence_t *_this, uint8_t EmbeddedCompensation);
static inline sys_error_code_t ISensorSetSoftwareCompensation(ISensorPresence_t *_this, uint8_t SoftwareCompensation);
static inline sys_error_code_t ISensorSetSoftwareCompensationAlgorithmConfig(ISensorPresence_t *_this,
                                                                             CompensationAlgorithmConfig_t *pAlgorithmConfig);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ISENSORPRESENCE_H_ */
