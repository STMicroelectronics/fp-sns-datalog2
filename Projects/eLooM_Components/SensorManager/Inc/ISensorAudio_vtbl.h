/**
  ******************************************************************************
  * @file    ISensorAudio_vtbl.h
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

#ifndef INCLUDE_ISENSORAUDIO_VTBL_H_
#define INCLUDE_ISENSORAUDIO_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ISensor.h"
#include "ISensor_vtbl.h"

/**
  * Create a type name for ISensorAudio_vtbl.
  */
typedef struct _ISensorAudio_vtbl ISensorAudio_vtbl;

struct _ISensorAudio_vtbl
{
  ISensor_vtbl vtbl;
  uint32_t (*SensorGetFrequency)(ISensorAudio_t *_this);
  uint8_t (*SensorGetVolume)(ISensorAudio_t *_this);
  uint8_t (*SensorGetResolution)(ISensorAudio_t *_this);
  sys_error_code_t (*SensorSetFrequency)(ISensorAudio_t *_this, uint32_t frequency);
  sys_error_code_t (*SensorSetVolume)(ISensorAudio_t *_this, uint8_t volume);
  sys_error_code_t (*SensorSetResolution)(ISensorAudio_t *_this, uint8_t bit_depth);
};

struct _ISensorAudio_t
{
  /**
    * Pointer to the virtual table for the class.
    */
  const ISensorAudio_vtbl *vptr;
};

// Inline functions definition
// ***************************

static inline uint32_t ISensorGetFrequency(ISensorAudio_t *_this)
{
  return _this->vptr->SensorGetFrequency(_this);
}

static inline uint8_t ISensorGetVolume(ISensorAudio_t *_this)
{
  return _this->vptr->SensorGetVolume(_this);
}

static inline uint8_t ISensorGetResolution(ISensorAudio_t *_this)
{
  return _this->vptr->SensorGetResolution(_this);
}

static inline sys_error_code_t ISensorSetFrequency(ISensorAudio_t *_this, uint32_t frequency)
{
  return _this->vptr->SensorSetFrequency(_this, frequency);
}

static inline sys_error_code_t ISensorSetVolume(ISensorAudio_t *_this, uint8_t volume)
{
  return _this->vptr->SensorSetVolume(_this, volume);
}

static inline sys_error_code_t ISensorSetResolution(ISensorAudio_t *_this, uint8_t bit_depth)
{
  return _this->vptr->SensorSetResolution(_this, bit_depth);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ISENSORAUDIO_VTBL_H_ */
